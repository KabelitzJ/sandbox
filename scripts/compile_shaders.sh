#!/bin/bash

glslc=$(which glslc)

function compile_shader {
  path=$1

  echo "Compiling shaders: '$path'"

  files=$(find $path -name "*.glsl")

  binary_dir="$path/bin"

  # Create the binary directory if it doesn't exist

  if [ ! -d $binary_dir ]; then
    mkdir -p $binary_dir
  fi

  for file in $files; do
    # Get the file name without the extension
    stage=$(basename $file .glsl)
    output="$binary_dir/$stage.spv"

    # Compile the shader
    echo "  Building stage '$stage' to '$output'"

    $glslc -fshader-stage=$stage -c $file -o $output
  done
}

# Get all subdirectories in the shaders directory
shaders=$(find $1 -maxdepth 1 -mindepth 1 -type d)

for shader in $shaders; do
  if [ $shader == "common" ]; then
    continue
  fi
  
  compile_shader $shader
done
