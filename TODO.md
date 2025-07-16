# TODO's for libsbx and demo application

## libsbx-animations
- [x] There is a leak somewhere in the animation system, need to investigate. // 2025.07.15
- [ ] Add support for animation blending.
- [ ] The mesh seems to be inverted on itself, need to check the mesh import process / transformations.

## libsbx-utility
- [x] Do a proper logger implementation that doesn't create a new logger for each scope. // 2025.07.15
- [x] Implement a `spdlog` sink that stores the messages for in-editor display. // 2025.07.15

## libsbx-assets
- [ ] Implement a proper asset manager that can handle different asset types and their dependencies.

## libsbx-models
- [ ] Abstract model loading end `sbx::graphics::model` creation (generic mesh source?).
- [ ] Implement a proper spatial partitioning system for the scene graph (octree with view frustum culling?).

## libsbx-scenes
- [ ] Implement node removal with hierarchy component -> remove relationship component

## Hint

### Windows

```sh
cmake --build ... -j$env:NUMBER_OF_PROCESSORS
```

### Unix

```sh
cmake --build ... -j$(nproc)
```