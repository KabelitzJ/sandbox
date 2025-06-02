#version 460 core

#extension GL_EXT_buffer_reference: enable

#define MAX_STACK_SIZE 256

#define null_node 4294967295

struct stack_entry {
  uint node;
  mat4 parent_transform;
  bool is_parent_dirty;
}; // struct stack_entry

struct stack {
  stack_entry entries[MAX_STACK_SIZE];
  uint size;
}; // struct stack

bool stack_push(inout stack stack, stack_entry entry) {
  if (stack.size >= MAX_STACK_SIZE) {
    return false;
  }

  stack.entries[stack.size] = entry;
  stack.size += 1;

  return true;
}

bool pop(inout stack stack, out stack_entry entry) {
  if (stack.size == 0) {
    return false;
  }

  stack.size -= 1;
  entry = stack.entries[stack.size];

  return true;
}

struct hierarchy {
  uint parent; 
  uint first_child; 
  uint next_sibling; 
  uint previous_sibling; 
}; // struct hierarchy

struct global_transform {
  mat4 model;
  mat4 normal;
}; // struct global_transform

layout(local_size_x = 32) in;

layout(std430, buffer_reference) readonly buffer hierarchy_buffer {
  hierarchy data[];
};

layout(std430, buffer_reference) buffer global_transform_buffer {
  global_transform data[];
};

layout(push_constant) uniform push_data {
  hierarchy_buffer hierarchies;
  global_transform_buffer global_transforms;
} push;

void main() {
  stack stack;

  
}
