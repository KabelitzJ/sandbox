#ifndef GRID_CONFIG_GLSL_
#define GRID_CONFIG_GLSL_

const float grid_size = 500.0;
const float grid_cell_size = 1.0;

#define GRID_BLACK 0

#if GRID_BLACK
const vec4 grid_color_thick = vec4(0.1, 0.1, 0.1, 1.0);
const vec4 grid_color_thin = vec4(0.2, 0.2, 0.2, 1.0);
#else
const vec4 grid_color_thick = vec4(1.0, 1.0, 1.0, 1.0);
const vec4 grid_color_thin = vec4(0.5, 0.5, 0.5, 1.0);
#endif // GRID_BLACK

const float grid_min_pixels_between_cells = 2.0;

#endif //GRID_CONFIG_GLSL_
