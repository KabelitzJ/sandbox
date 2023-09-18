import os

entries = os.listdir(".")

dependencies = {}
found_circle = False

for entry in entries:
  if entry.startswith("libsbx-"):
    with open(os.path.join(entry, "CMakeLists.txt"), "r") as file:
      for line in file:
       if line.find("libsbx::") != -1:
        key = entry.split("libsbx-")[1].split("-")[0]
        if key not in dependencies:
          dependencies[key] = []

        dependencies[key].append(line.split("libsbx::")[1].split(" ")[0].strip())

for key, value in dependencies.items():
  for dependency in value:
    if dependency in dependencies and key in dependencies[dependency]:
      print("Circular dependency detected: " + key + " <-> " + dependency)
      found_circle = True

if not found_circle:
  print("No circular dependency detected.")
      
print("Dependency analysis completed.")

