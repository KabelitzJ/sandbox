import os
import sys

def main():
  if len(sys.argv) < 2:
    print("Usage: python push.py <commit message>")
    return

  commit_message = sys.argv[1]

  commands = [
    "git add .",
    f"git commit -m \"{commit_message}\"",
    "git push"
  ]

  for command in commands:
    os.system(command)

if __name__ == "__main__":
  main()
