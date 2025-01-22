#ifndef DEMO_IMGUI_BINDINGS_IMGUI_HPP_
#define DEMO_IMGUI_BINDINGS_IMGUI_HPP_

#if LIBSBX_IMGUI_DOCKING
#include <demo/imgui/bindings/1.89.7-docking/imgui_impl_glfw.h>
#include <demo/imgui/bindings/1.89.7-docking/imgui_impl_vulkan.h>
#else 
#include <demo/imgui/bindings/1.89.7/imgui_impl_glfw.h>
#include <demo/imgui/bindings/1.89.7/imgui_impl_vulkan.h>
#endif

#endif // DEMO_IMGUI_BINDINGS_IMGUI_HPP_
