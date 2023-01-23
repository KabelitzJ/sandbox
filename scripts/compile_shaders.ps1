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

  Write-Host "$binary_dir"

  New-Item -ItemType "Directory" -Path "$binary_dir" -Force | Out-Null

  $files = Get-ChildItem -Path "$path" -Filter "*.glsl" -File

  foreach ($file in $files) {
    if (Test-Path -Path "$file" -PathType Leaf) {
      $stage = (Get-Item "$file").BaseName
      $output = "$binary_dir\$stage.spv"

      Write-Host "  Building stage: '$stage'"

      & "$glslc" -fshader-stage="$stage" -c "$file" -o "$output"
    }
  }

  Write-Host ""
}

$shaders = Get-ChildItem "$directory" -Directory

foreach ($shader in $shaders) {
  compile_shader $shader
}
