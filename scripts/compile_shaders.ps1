param(
  [string]$directory="."
)

$glslc = "glslc.exe"

function compile_shader {
  param (
    [string]$path
  )

  $shader_name = Split-Path "$path" -Leaf
  
  Write-Host "Compiling shader: '$shader_name'"

  $binary_dir = "$path\bin"

  if (!(Test-Path "$binary_dir")) {
    New-Item -ItemType "Directory" -Path "$binary_dir" | Out-Null
  }

  $files = Get-ChildItem "$path"

  foreach ($file in $files) {
    if (Test-Path -Path "$file" -PathType Leaf) {
      $stage = (Get-Item "$file").BaseName
      $output = "$binary_dir\$stage.spv"

      Write-Host "  Building stage: '$stage'"

      & "$glslc" -fshader-stage="$stage" -c "$file" -o "$output"
    }
  }
}

$shaders = Get-ChildItem "$directory"

foreach ($shader in $shaders) {
  compile_shader $shader
}

# & "$glslc" "-fshader-stage=${_TYPE} -c ${_FILE} -o ${_OUTPUT_FILE}"
