from noise import pnoise2
import numpy as np

# Parameters
size = 500.0             # Total width and height of the terrain
subdivisions = 100       # Number of subdivisions (must be >= 2)

# Derived parameters
spacing = size / (subdivisions - 1)
half_size = size / 2.0
height_scale = 40.0
noise_scale = 0.02

# Perlin noise parameters
octaves = 4
persistence = 0.5
lacunarity = 2.0

# Generate height map using Perlin noise
heights = np.zeros((subdivisions, subdivisions))
for i in range(subdivisions):
    for j in range(subdivisions):
        x = i * noise_scale
        y = j * noise_scale
        heights[i, j] = pnoise2(x, y, octaves=octaves, persistence=persistence, lacunarity=lacunarity) * height_scale

# Generate vertices
vertices = []
for i in range(subdivisions):
    for j in range(subdivisions):
        x = i * spacing - half_size
        y = j * spacing - half_size
        z = heights[i, j]
        vertices.append((x, y, z))

# Generate faces
faces = []
for i in range(subdivisions - 1):
    for j in range(subdivisions - 1):
        idx = lambda x, y: x * subdivisions + y + 1
        a = idx(i, j)
        b = idx(i + 1, j)
        c = idx(i, j + 1)
        d = idx(i + 1, j + 1)
        faces.append((a, b, c))
        faces.append((b, d, c))

# Write to OBJ
obj_lines = ["# Centered Perlin noise-based terrain\n"]
for v in vertices:
    obj_lines.append(f"v {v[0]:.4f} {v[1]:.4f} {v[2]:.4f}\n")
for f in faces:
    obj_lines.append(f"f {f[0]} {f[1]} {f[2]}\n")

# Save the file
obj_path = "perlin_terrain.obj"
with open(obj_path, "w") as f:
    f.writelines(obj_lines)
