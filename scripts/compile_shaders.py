#!/usr/bin/env python3
import subprocess
import shutil
import os
from pathlib import Path
from typing import Optional


STAGES = ["vertex", "fragment", "compute", "geometry", "tesscontrol", "tesseval"]

# Slang stage → (profile, entry)
SLANG_STAGE_INFO = {
  "vertex":      ("vs_6_0", "main"),
  "fragment":    ("ps_6_0", "main"),
  "pixel":       ("ps_6_0", "main"),
  "compute":     ("cs_6_0", "main"),
  "geometry":    ("gs_6_0", "main"),
  "tesscontrol": ("hs_6_0", "main"),
  "tesseval":    ("ds_6_0", "main"),
}


def which_or_none(name: str) -> Optional[str]:
  return shutil.which(name)


def print_badge(message: str, character: str = "="):
  print()
  print(character * len(message))
  print(message)
  print(character * len(message))
  print()


def run_cmd(cmd: list) -> bool:
  try:
    subprocess.run(cmd, check=True)
    return True

  except FileNotFoundError:
    # Tool missing; discovery handles messaging, treat as failure here.
    return False

  except subprocess.CalledProcessError as e:
    print(f"[ERROR] Command failed ({e.returncode}): {' '.join(cmd)}")
    return False


def compile_glsl_dir(glslc_path: Optional[str], shader_dir: Path, shader_root_dir: Path) -> int:
  files = list(shader_dir.glob("*.glsl"))

  if not files:
    return 0

  if glslc_path is None:
    print(f"[WARN] Found GLSL files in {shader_dir} but glslc not found — skipping these:")
    for f in files:
      print(f"  - {f.name}")
    return 0

  print(f"Compiling GLSL shaders: {shader_dir}")

  binary_dir = shader_dir / "bin"
  binary_dir.mkdir(parents=True, exist_ok=True)

  built = 0

  for file in files:
    stage = file.stem  # e.g., vertex.glsl → "vertex"

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
      f"-I{shader_root_dir}",
    ]

    # print(f"  GLSL {stage}: {' '.join(cmd)}")

    if run_cmd(cmd):
      built += 1

  return built


def compile_slang_dir(slangc_path: Optional[str], shader_dir: Path, shader_root_dir: Path) -> int:
  files = list(shader_dir.glob("*.slang"))

  if not files:
    return 0

  if slangc_path is None:
    print(f"[WARN] Found Slang files in {shader_dir} but slangc not found — skipping these:")
    for f in files:
      print(f"  - {f.name}")
    return 0

  print(f"Compiling SLANG shaders: {shader_dir}")

  binary_dir = shader_dir / "bin"
  binary_dir.mkdir(parents=True, exist_ok=True)

  # Deterministic order helps when both fragment.slang and pixel.slang exist
  files = sorted(files, key=lambda p: p.name)

  built = 0

  for file in files:
    stage = file.stem  # e.g., pixel.slang → "pixel"
    info = SLANG_STAGE_INFO.get(stage)

    if not info:
      continue

    profile, entry = info

    # Output rule:
    # - fragment.slang → fragment.spv
    # - pixel.slang    → pixel.spv then RENAME to fragment.spv
    # - others         → <stage>.spv
    if stage == "fragment":
      out_spv = binary_dir / "fragment.spv"
    elif stage == "pixel":
      out_spv = binary_dir / "pixel.spv"
    else:
      out_spv = binary_dir / f"{stage}.spv"

    cmd = [
      slangc_path,
      str(file),
      "-target", "spirv",
      "-profile", profile,
      "-entry", entry,
      "-capability", "spirv_1_5",
      "-capability", "SPV_EXT_physical_storage_buffer",
      "-matrix-layout-column-major",
      "-o", str(out_spv),
      f"-I{shader_root_dir}",
    ]

    if not run_cmd(cmd):
      continue

    if stage == "pixel":
      dest = binary_dir / "fragment.spv"
      os.replace(out_spv, dest)

    built += 1

  return built


def compile_shader_dir(shader_dir: Path, shader_root_dir: Path, glslc_path: Optional[str], slangc_path: Optional[str]) -> int:
  total = 0

  total += compile_slang_dir(slangc_path, shader_dir, shader_root_dir)
  total += compile_glsl_dir(glslc_path, shader_dir, shader_root_dir)

  return total


def main(shader_root: str) -> int:
  shader_root_dir = Path(shader_root).resolve()

  glslc_path = which_or_none("glslc")
  slangc_path = which_or_none("slangc")

  if glslc_path is None and slangc_path is None:
    print("[ERROR] Neither 'glslc' nor 'slangc' found in PATH. Install at least one.")
    return 2

  if glslc_path is None:
    print("[WARN] glslc not found — GLSL (*.glsl) files will be skipped.")

  if slangc_path is None:
    print("[WARN] slangc not found — Slang (*.slang) files will be skipped.")

  print_badge(f"Compiling shaders in {shader_root_dir}")

  total_built = 0

  for shader_dir in shader_root_dir.iterdir():
    if not shader_dir.is_dir():
      continue

    if shader_dir.name == "libsbx":
      continue

    built = compile_shader_dir(shader_dir, shader_root_dir, glslc_path, slangc_path)
    total_built += built

  print(f"Done. Built {total_built} shader stage(s).")

  return 0 if total_built > 0 else 1


if __name__ == "__main__":
  import sys

  if len(sys.argv) != 2:
    print("Usage: python compile_shaders.py <shader_root_dir>")
    sys.exit(1)

  sys.exit(main(sys.argv[1]))
