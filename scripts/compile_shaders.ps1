param(
  [Parameter(Mandatory = $true)]
  [ValidateNotNullOrEmpty()]
  [string]$directory,

  [string]$vulkan_version = "1.3",
  [string]$spv_version = "1.5",
  [string]$std = "450 core"
)

if ($directory -eq "") {
  Write-Host "[Error] No directory specified."
  Write-Host
  Write-Host "Usage: $PSCommandPath -directory <path>"
  Write-Host

  return
}

Write-Host
Write-Host "Compiling shaders in directory: '$directory'"
Write-Host

$glslc = "glslc.exe"

function compile_shader {
  param (
    [string]$path
  )

  $files = Get-ChildItem -Path "$path" -Filter "*.glsl" -File

  if ($files.Count -eq 0) {
    return 0
  }

  $shader_name = Split-Path "$path" -Leaf


  Write-Host "Compiling shader: '$shader_name'"

  $binary_dir = "$path\bin"

  New-Item -ItemType "Directory" -Path "$binary_dir" -Force | Out-Null

  $available_stages = @("vertex", "fragment", "geometry", "compute", "tessellation_control", "tessellation_evaluation")

  foreach ($file in $files) {
    if (Test-Path -Path "$file" -PathType Leaf) {
      $stage = (Get-Item "$file").BaseName

      if ($available_stages -notcontains $stage) {
        continue
      }

      $output = "$binary_dir\$stage.spv"

      Write-Host "  Building stage: '$stage'"

      & "$glslc" -fshader-stage="$stage" -c "$file" -o "$output" --target-env="vulkan1.3" --target-spv="spv1.5" -std="450 core" -I"$directory"
    }
  }

  Write-Host

  return 1
}

$shaders = Get-ChildItem -Directory "$directory"

foreach ($shader in $shaders) {
  if ((Split-Path "$shader" -Leaf) -eq "libsbx") {
    continue
  }

  if ((compile_shader $shader) -eq 0) {
    Write-Host "[Error] Directory '$shader' does not contain shader sources."
    Write-Host
    Write-Host "Usage: $PSCommandPath -directory <path>"
    Write-Host
    return 1
  }
}

