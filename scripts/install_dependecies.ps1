param(
  [string]$directory="$pwd"
)

conan install $directory --output-folder=build/dependencies --build=missing -s build_type=Debug
