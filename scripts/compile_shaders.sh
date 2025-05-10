#!/bin/bash

glslc=$(which glslc)

function print_badge {
  message="$1"
  message_length=${#message}

  character="${2:-"="}"

  echo
  echo "$(printf "%.0s$character" $(seq 1 $message_length))"
  echo "$message"
  echo "$(printf "%.0s$character" $(seq 1 $message_length))"
  echo
}

function compile_shader {
  path="$1"

  echo "Compiling shaders: $path"

  files=$(find $path -name "*.glsl")

  binary_dir="$path/bin"

  # Create the binary directory if it doesnt exist

  if [ ! -d $binary_dir ]; then
    mkdir -p $binary_dir
  fi

  for file in $files; do
    # Get the file name without the extension
    stage=$(basename $file .glsl)
    output="$binary_dir/$stage.spv"

    # Compile the shader
    echo "  Building stage "$stage"" # to "$output""
    
    $glslc -fshader-stage=$stage -c $file -o $output
  done

  echo
}

path=${1}
path_length=${#path}

# Get all direct subdirectories in the shaders directory
shaders=$(find $path -maxdepth 1 -mindepth 1 -type d)

# echo
# echo "=====================$(printf "%.0s=" $(seq 1 $path_length))"
print_badge "Compiling shaders in $path"
# echo "=====================$(printf "%.0s=" $(seq 1 $path_length))"
# echo

for shader in $shaders; do
  if [ "$(basename $shader)" == "common" ]; then
    continue
  fi
  
  compile_shader $shader
done
