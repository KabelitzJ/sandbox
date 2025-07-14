# TODO's for libsbx and demo application

## libsbx-animations
- [ ] There is a leak somewhere in the animation system, need to investigate.
- [ ] Add support for animation blending.
- [ ] The mesh seems to be inverted on itself, need to check the mesh import process / transformations.

## libsbx-utility
- [ ] Do a proper logger implementation that doesn't create a new logger for each scope.
- [ ] Implement a `spdlog` sink that stores the messages for in-editor display.

## libsbx-assets
- [ ] Implement a proper asset manager that can handle different asset types and their dependencies.

## libsbx-models
- [ ] Abstract model loading end `sbx::graphics::model` creation (generic mesh source?).
- [ ] Implement a proper spatial partitioning system for the scene graph (octree with view frustum culling?).
