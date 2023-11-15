import os

def main():
  commands = [
    "git pull",
    "conan install . --profile=debug/x86_64/windows --build=missing",
    "cmake . -B \"./build/debug/\" -G \"MinGW Makefiles\" -DCMAKE_BUILD_TYPE=Debug",
    "cmake --build ./build/debug"
  ]

  for command in commands:
    os.system(command)

if __name__ == "__main__":
  main()
