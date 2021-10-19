# Sandbox

The sandbox engine a light-weight, ecs-based, modular rendering engine implemented using c++17 features.

![Build and test](https://github.com/KabelitzJ/sandbox/actions/workflows/build_and_test.yml/badge.svg?branch=development)

# Content

- [TODOs](#todos)
- [Build](#build)
- [Using sbx](#using-sbx)
- [License](#license)

# TODOs

- ecs
  - views
  - iterators
  - value accessors
  - copy / move constructors
  - copy / move operators
- rendering
  - shader
  - mesh
  - texture
  - gl objects
- processes
- editor

# Build

First you should make a separate folder for the generated build files

```bash
mkdir build
```

Then you'll need to install and build (if not already in your local conan repository) the external dependencies.
For this you need [conan installed](https://docs.conan.io/en/latest/installation.html#install-with-pip-recommended) and run the following commands in the root directory

```bash
conan install . --install-folder build/ --build missing
```

After that you can generate the [cmake](https://cmake.org/download/) build files with

```bash
cmake . -B build -G "<generator>" -DCMAKE_BUILD_TYPE=<config> -DSBX_BUILD_DEMO=<demo> -DSBX_BUILD_TESTS=<tests> -DSBX_GENERATE_DOCS=<docs>
```

Where

| Tag           | Description                                                                  | Example value                   |
| ------------- | ---------------------------------------------------------------------------- | ------------------------------- |
| `generator`   | Generator used to build the project                                          | `MinGW Makefiles`, `Unix Makefiles`<br>([available cmake generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html))|
| `config`      | Guild configuration                                                          | `Debug`, `Release`<br>([available cmake configurations](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html#variable:CMAKE_BUILD_TYPE))|
| `demo`        | Should the demo executable be build                                          | `True` or `False`               |
| `tests`       | Should the tests be build                                                    | `True` or `False`               |
| `docs`        | Should the documentation files be generated                                  | `True` or `False`               |

If you build the tests you can run them by running

```bash
./build/tests/sbx_test.exe
```

# Using sbx

The modularity of sbx allows you quickly set up a running example. For a deeper dive into the available functionalities you can
check out the `demo/` directive of the [wiki page](https://github.com/KabelitzJ/sandbox/wiki).

To start using sbx you'll need to include

```cpp
#include <core/core.hpp>
```

inside one of you `.cpp` files. You then need to define the entry point for sbx.

```cpp
void sbx::setup(sbx::engine& engine) {

}
```

Sbx defines the `void main(int, char**)` method for you so _DO NOT_ define it yourself.

Inside this entry point you can add your custom modules to the engine.
Start by creating your own module that inherits from `sbx::module` and define the tree lifecycle methods.

```cpp
class my_module final : public sbx::module {

public:
  // data can be passed via the constructor
  my_module(const some_data_object& data) : _data{data} { }
  ~my_module() { }

  void initialize() override { }

  void update(const sbx::time delta_time) override { }

  void terminate() override { }

private:
  // Any local state for that module can be declared here
  some_data_object& _data{};

};
```

After that you can add your module to the engine inside the entry point.

```cpp
void sbx::setup(sbx::engine& engine) {
  // Arguments will be used to instantiate a new instance of my_module
  engine.add_module<my_module>(some_data_instance);
}
```

# License

This project is published under the [MIT License](LICENSE).
