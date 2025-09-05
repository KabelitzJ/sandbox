#!/usr/bin/env python3
import subprocess
import shutil
from pathlib import Path
import sys
import os

STAGES = [
  "vertex", 
  "fragment", 
  "compute", 
  "geometry", 
  "tesscontrol", 
  "tesseval"
]

# Slang stage → (profile, entry)
SLANG_STAGE_INFO = {
  "vertex":      ("vs_6_0", "VSMain"),
  "fragment":    ("ps_6_0", "FSMain"),
  "pixel":       ("ps_6_0", "PSMain"),
  "compute":     ("cs_6_0", "CSMain"),
  "geometry":    ("gs_6_0", "GSMain"),
  "tesscontrol": ("hs_6_0", "HSMain"),
  "tesseval":    ("ds_6_0", "DSMain"),
}

def which_or_none(name: str):
  return shutil.which(name)

def ensure_link(target: Path, link_path: Path):
  """
  Create a symlink from link_path -> target.
  Fallback to hardlink, then copy, if symlink isn't available.
  """
  try:
    if link_path.exists() or link_path.is_symlink():
      link_path.unlink()
    # Use relative path in the symlink for portability
    rel = os.path.relpath(target, start=link_path.parent)
    link_path.symlink_to(rel)
    return
  except (OSError, NotImplementedError):
    pass
  # Try hardlink
  try:
    if link_path.exists():
      link_path.unlink()
    os.link(target, link_path)
    return
  except OSError:
    pass
  # Fallback: copy
  shutil.copy2(target, link_path)

def print_badge(message: str, character: str = "="):
  print()
  print(character * len(message))
  print(message)
  print(character * len(message))
  print()

def compile_glsl_dir(glslc_path: str, shader_dir: Path, shader_root_dir: Path):
  files = list(shader_dir.glob("*.glsl"))

  if not files:
    return
  
  if glslc_path is None:
    print(f"[WARN] Skipping GLSL in {shader_dir} (glslc not found).")
    return

  print(f"Compiling GLSL shaders: {shader_dir}")

  binary_dir = shader_dir / "bin"
  binary_dir.mkdir(parents=True, exist_ok=True)

  for file in files:
    stage = file.stem

    if stage not in STAGES:
      continue

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

    print(f"  GLSL {stage}: {' '.join(cmd)}")

    subprocess.run(cmd, check=True)
  
  print()

def compile_slang_dir(slangc_path: str, shader_dir: Path, shader_root_dir: Path):
  files = list(shader_dir.glob("*.slang"))

  if not files:
    return

  if slangc_path is None:
    print(f"[WARN] Skipping Slang in {shader_dir} (slangc not found).")
    return

  print(f"Compiling Slang shaders: {shader_dir}")

  binary_dir = shader_dir / "bin"
  binary_dir.mkdir(parents=True, exist_ok=True)

  for file in files:
    stage = file.stem
    info = SLANG_STAGE_INFO.get(stage)

    if not info:
      continue

    profile, entry = info
    output = binary_dir / f"{stage}.spv"

    cmd = [
      slangc_path,
      str(file),
      "-target", "spirv",
      "-profile", profile,
      "-entry", entry,
      "-o", str(output),
      f"-I{shader_root_dir}",
    ]

    print(f"  Slang {stage}: {' '.join(cmd)}")
    
    subprocess.run(cmd, check=True)

    if stage == "pixel":
      ensure_link(binary_dir / "pixel.spv", binary_dir / "fragment.spv")

  print()

def compile_shader_dir(shader_dir: Path, shader_root_dir: Path, glslc_path: str, slangc_path: str):
  compile_slang_dir(slangc_path, shader_dir, shader_root_dir)
  compile_glsl_dir(glslc_path, shader_dir, shader_root_dir)

def main(shader_root: str):
  shader_root_dir = Path(shader_root).resolve()
  glslc_path = which_or_none("glslc")
  slangc_path = which_or_none("slangc")

  print_badge(f"Compiling shaders in {shader_root_dir}")

  for shader_dir in shader_root_dir.iterdir():
    if not shader_dir.is_dir():
      continue
    if shader_dir.name == "libsbx":
      continue
    compile_shader_dir(shader_dir, shader_root_dir, glslc_path, slangc_path)

if __name__ == "__main__":
  if len(sys.argv) != 2:
    print("Usage: python compile_shaders.py <shader_root_dir>")
    exit(1)

  main(sys.argv[1])
