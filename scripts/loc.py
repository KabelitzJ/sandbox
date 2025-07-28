import os
from collections import defaultdict

# Configuration
VALID_EXTENSIONS = {'.cpp', '.hpp', '.ipp'}
EXCLUDE_TOP_LEVEL_DIRS = {'cmake', 'build', 'docs', 'images', '.vscode', '.github', 'scripts'}
BASE_DIR = os.path.abspath('.')  # Current directory

def count_lines_in_file(filepath):
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            return sum(1 for _ in f)
    except Exception as e:
        print(f"Could not read {filepath}: {e}")
        return 0

def get_module_name(filepath, base_dir):
    rel_path = os.path.relpath(filepath, base_dir)
    parts = rel_path.split(os.sep)
    return parts[0] if len(parts) > 1 else '(root)'

def count_loc_grouped_by_module(base_dir):
    loc_per_module = defaultdict(int)

    for root, dirs, files in os.walk(base_dir):
        # Skip excluded top-level dirs
        rel_root = os.path.relpath(root, base_dir)
        top_level = rel_root.split(os.sep)[0]
        if top_level in EXCLUDE_TOP_LEVEL_DIRS:
            dirs[:] = []  # don't recurse further
            continue

        for file in files:
            _, ext = os.path.splitext(file)
            if ext in VALID_EXTENSIONS:
                full_path = os.path.join(root, file)
                module = get_module_name(full_path, base_dir)
                loc = count_lines_in_file(full_path)
                loc_per_module[module] += loc

    return loc_per_module

if __name__ == '__main__':
    results = count_loc_grouped_by_module(BASE_DIR)
    total = 0
    for module, loc in sorted(results.items()):
        print(f"{module}: {loc}")
        total += loc
    print(f"\nTotal: {total}")
