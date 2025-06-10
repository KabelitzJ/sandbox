import subprocess
import shutil
import os
from pathlib import Path

def find_glslc():
  glslc = shutil.which("glslc")
  if not glslc:
    raise FileNotFoundError("glslc compiler not found in PATH")
  return glslc

def print_badge(message: str, character: str = "="):
  print()
  print(character * len(message))
  print(message)
  print(character * len(message))
  print()

def compile_shader(glslc_path: str, shader_dir: Path, shader_root_dir: Path):
  print(f"Compiling shaders: {shader_dir}")

  files = list(shader_dir.glob("*.glsl"))
  binary_dir = shader_dir / "bin"
  binary_dir.mkdir(parents=True, exist_ok=True)

  for file in files:
    stage = file.stem  # filename without .glsl

    if stage not in ["vertex", "fragment", "compute", "geometry", "tesscontrol", "tesseval"]:
      continue

    print(f"Found stage: {stage}")
    output = binary_dir / f"{stage}.spv"

    cmd = [
      glslc_path,
      f"-fshader-stage={stage}",
      "-c", str(file),
      "-o", str(output),
      "--target-env=vulkan1.4",
      "--target-spv=spv1.5",
      "-std=460core",
      f"-I{shader_root_dir}"
    ]

    print(f"  Building stage {stage}")
    subprocess.run(cmd, check=True)

  print()

def main(shader_root: str):
  shader_root_dir = Path(shader_root).resolve()
  glslc_path = find_glslc()

  print_badge(f"Compiling shaders in {shader_root_dir}")

  for shader_dir in shader_root_dir.iterdir():
    if not shader_dir.is_dir():
      continue
    if shader_dir.name == "libsbx":
      continue
    compile_shader(glslc_path, shader_dir, shader_root_dir)

if __name__ == "__main__":
  import sys
  if len(sys.argv) != 2:
    print("Usage: python compile_shaders.py <shader_root_dir>")
    exit(1)

  main(sys.argv[1])
