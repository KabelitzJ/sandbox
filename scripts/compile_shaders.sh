#!/bin/bash

glslc=$(which glslc)

shader_root_dir=$1

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
    
    $glslc -fshader-stage=$stage -c $file -o $output --target-env="vulkan1.3" --target-spv="spv1.5" -std="450 core" -I"$shader_root_dir"
  done

  echo
}

# Get all direct subdirectories in the shaders directory
shaders=$(find $shader_root_dir -maxdepth 1 -mindepth 1 -type d)

# echo
# echo "=====================$(printf "%.0s=" $(seq 1 $path_length))"
print_badge "Compiling shaders in $shader_root_dir"
# echo "=====================$(printf "%.0s=" $(seq 1 $path_length))"
# echo

for shader in $shaders; do
  if [ "$(basename $shader)" == "libsbx" ]; then
    continue
  fi
  
  compile_shader $shader
done
