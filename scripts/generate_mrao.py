import argparse
from PIL import Image
import numpy as np

def load_grayscale_or_default(path, size, default_value):
  """Load grayscale image or return default value array"""
  if path:
    img = Image.open(path).convert("L")
    
    if img.size != size:
      raise ValueError(f"Image {path} has size {img.size}, expected {size}")

    return np.array(img)
  else:
    default_pixel = int(round(default_value * 255))
    
    return np.full((size[1], size[0]), default_pixel, dtype=np.uint8)  # (height, width)

def combine_grayscale_to_rgb(m_path, r_path, ao_path, output_path):
  # Determine image size from the first available image
  size = None

  for path in [m_path, r_path, ao_path]:
    if path:
      size = Image.open(path).size
      break

  if size is None:
    raise ValueError("At least one input image must be provided to determine image size.")

  # Load channels or fill with default values
  red = load_grayscale_or_default(m_path, size, 0.0)   # Metallic default = 0.0
  green = load_grayscale_or_default(r_path, size, 0.5)   # Roughness default = 0.5
  blue = load_grayscale_or_default(ao_path, size, 1.0)  # AO default = 1.0

  # Combine and save
  rgb = np.stack((red, green, blue), axis=-1)
  out_image = Image.fromarray(rgb, mode="RGB")
  out_image.save(output_path)

  print(f"Saved MRAO image to {output_path}")

if __name__ == "__main__":
  parser = argparse.ArgumentParser(description="Combine metallic, roughness, and AO grayscale images into a single RGB MRAO image.")

  parser.add_argument("-m", "--metallic", type=str, help="Path to metallic grayscale image")
  parser.add_argument("-r", "--roughness", type=str, help="Path to roughness grayscale image")
  parser.add_argument("-ao", "--ambient-occlusion", type=str, help="Path to ambient occlusion grayscale image")
  parser.add_argument("-o", "--output", type=str, required=True, help="Path to save the output RGB MRAO image (PNG)")

  args = parser.parse_args()

  combine_grayscale_to_rgb(args.metallic, args.roughness, args.ambient_occlusion, args.output)
