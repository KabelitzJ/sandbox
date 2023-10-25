import os
from pathlib import Path as path

blacklist = ["build", "docs", "scripts", "assets", ".git", ".vscode"]
extensions = [".cpp", ".hpp", ".ipp", ".tpp", ".mpp"]

def count_code_lines(file_path):
  with open(file_path, 'r') as file:
    return sum(1 for _ in file)

def should_skip_directory(directory: path):
  return any(directory.name == name for name in blacklist)

def main() -> None:
  root = path(__file__).parent.parent

  total_sum: int = 0

  for entry in os.listdir(root):
    entry = path(entry)

    directory_sum: int = 0

    if not entry.is_dir() or should_skip_directory(entry):
      continue

    for parent, _, files in os.walk(entry):
      for file in files:
        file = path(file)

        if file.suffix in extensions:
          file_path = path(parent) / file
          directory_sum += count_code_lines(file_path)

    print(f"{entry}: {directory_sum}")

    total_sum += directory_sum

  print(f"Total: {total_sum}")


if __name__ == "__main__":
  main()
