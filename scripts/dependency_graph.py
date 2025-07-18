import os
import re
import networkx as nx
import pydot # Only pydot is imported for visualization

# --- Configuration ---
ROOT_DIR = "."  # Set this to the root of your project
OUTPUT_GRAPH_FILENAME = "dependency_graph.png"

# --- Core Logic Functions ---

def find_modules(root_dir):
    """
    Finds directories starting with 'libsbx-' in the root_dir.
    Assumes these are your internal modules.
    """
    return [
        name for name in os.listdir(root_dir)
        if os.path.isdir(os.path.join(root_dir, name)) and name.startswith("libsbx-")
    ]

def parse_cmakelists(path):
    """
    Parses a CMakeLists.txt file to extract linked libraries from
    target_link_libraries commands, categorizing them as internal (libsbx::)
    or external.
    """
    try:
        with open(path, "r") as f:
            content = f.read()
    except FileNotFoundError:
        print(f"Error: CMakeLists.txt not found at {path}")
        return []

    target_link_libraries_regex = r"target_link_libraries\s*\((.*?)\)"
    all_extracted_libraries = []

    for m in re.finditer(target_link_libraries_regex, content, re.DOTALL):
        block_content = m.group(1) # Content inside the parentheses

        capture_libs = False
        lines = block_content.splitlines()
        for line in lines:
            stripped_line = line.strip()

            if stripped_line == "PUBLIC" or stripped_line == "PRIVATE":
                capture_libs = True
                continue # Skip the PUBLIC/PRIVATE keyword line itself

            if capture_libs:
                # Basic filter for library names: non-empty, no comments, no CMake variables/commands, no spaces
                if stripped_line and \
                   not stripped_line.startswith(("#", "//", "${")) and \
                   " " not in stripped_line:
                    # Ensure it's not another keyword (like PUBLIC/PRIVATE if indented)
                    if stripped_line not in ["PUBLIC", "PRIVATE"]:
                        all_extracted_libraries.append(stripped_line)
                elif stripped_line == "":
                    # Stop capturing if an empty line is encountered, indicating end of list
                    capture_libs = False
                elif stripped_line.startswith(("PUBLIC", "PRIVATE")):
                    # Stop capturing if another PUBLIC/PRIVATE is encountered
                    capture_libs = False

    final_deps = []
    for dep in all_extracted_libraries:
        # Reformat internal libsbx:: modules to libsbx-ModuleName
        match = re.match(r"libsbx::(\w+)", dep)
        if match:
            final_deps.append(f"libsbx-{match.group(1)}")
        else:
            # Keep external dependencies as is
            final_deps.append(dep)

    return final_deps

def build_graph(root_dir):
    """
    Builds a directed graph of module dependencies.
    Nodes are tagged as 'internal' (libsbx-) or 'external'.
    """
    graph = nx.DiGraph()
    modules = find_modules(root_dir)

    # Add all internal modules as nodes and tag them
    for mod in modules:
        graph.add_node(mod, type="internal")
    
    # Keep track of all nodes added to prevent duplicate external nodes
    all_nodes_in_graph = set(modules)

    # Parse CMakeLists for each module and add dependencies to the graph
    for mod in modules:
        cmake_path = os.path.join(root_dir, mod, "CMakeLists.txt")
        if not os.path.isfile(cmake_path):
            print(f"Warning: Module {mod} has no CMakeLists.txt file at {cmake_path}. Skipping.")
            continue

        deps = parse_cmakelists(cmake_path)
        for dep in deps:
            # Add external dependencies as nodes if they don't exist and tag them
            if dep not in all_nodes_in_graph:
                graph.add_node(dep, type="external") 
                all_nodes_in_graph.add(dep)
            
            # Add the edge from the current module to its dependency
            graph.add_edge(mod, dep)

    return graph

# --- Visualization Function (pydot only) ---

def visualize_graph(graph, output_filename, dot_debug_path=None):
    """
    Generates and saves a visualization of the dependency graph using Graphviz 'dot'.
    Visualizes dependency flow top-to-bottom, with internal modules ordered by dependency.
    """
    print(f"Generating graph to {output_filename}...")

    if not nx.is_directed_acyclic_graph(graph):
        cycles = list(nx.simple_cycles(graph))
        raise ValueError(f"The graph contains cycles: {cycles}")

    dot = pydot.Dot("dependency_graph", graph_type="digraph", strict=True)
    dot.set_rankdir("TB")  # Top to Bottom
    dot.set_overlap("false")
    dot.set_splines("true")
    dot.set_nodesep("0.8")
    dot.set_ranksep("1.2 equally")

    # Add nodes with styling based on type
    for node, data in graph.nodes(data=True):
        node_type = data.get("type", "unknown")
        attrs = {
            "style": "filled",
            "fontname": "monospace",
            "fixedsize": "false",
            "margin": "0.25,0.15",
            "fontsize": "10" if node_type == "internal" else "9",
            "fillcolor": "lightblue" if node_type == "internal" else "lightcoral",
            "shape": "box" if node_type == "internal" else "ellipse"
        }
        dot_node = pydot.Node(node, **attrs)
        dot.add_node(dot_node)

    # Add edges
    for u, v in graph.edges():
        dot_edge = pydot.Edge(
            u, v,
            color="gray30",
            arrowhead="vee",
            arrowsize=1.1,
            penwidth=1.5
        )
        dot.add_edge(dot_edge)

    # Save .dot file for debugging
    if dot_debug_path:
        dot.write_raw(dot_debug_path)
        print(f"DOT source written to {dot_debug_path}")

    try:
        dot.write_png(output_filename)
        print(f"Graph successfully saved to {output_filename}")
    except FileNotFoundError:
        raise RuntimeError("Graphviz 'dot' executable not found. Please install Graphviz and add it to your PATH.")
    except Exception as e:
        raise RuntimeError(f"Error writing the graph file: {e}")

# --- Main Execution Block ---

if __name__ == "__main__":
    try:
        # Build the dependency graph
        graph = build_graph(ROOT_DIR)
        
        # Print detected dependencies
        print("\n--- Modules and their dependencies ---")
        for node in graph.nodes:
            successors = list(graph.successors(node))
            node_type = graph.nodes[node].get('type', 'unknown')
            
            if successors:
                print(f"[{node_type}] {node} -> {successors}")
            else:
                print(f"[{node_type}] {node} (no direct dependencies listed)")

        # Visualize the graph using Graphviz 'dot'
        visualize_graph(graph, OUTPUT_GRAPH_FILENAME)

    except ImportError:
        print("\nERROR: 'pydot' Python package not found.")
        print("Please install it: pip install pydot graphviz")
        print("Ensure Graphviz system binaries are also installed for 'dot' command.")
        exit(1) # Exit with an error code
    except RuntimeError as e:
        print(f"\nERROR: {e}")
        exit(1)
    except ValueError as e:
        print(f"\nERROR: {e}")
        exit(1)
    except Exception as e:
        print(f"\nAn unexpected error occurred: {e}")
        exit(1)