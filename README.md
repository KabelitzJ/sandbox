# Sandbox

The sandbox engine a ecs-based, modular rendering engine implemented using c++17 features.

![Build and test](https://github.com/KabelitzJ/sandbox/actions/workflows/build_and_test.yml/badge.svg?branch=development)

# Content

- [TODOs](#todos)
- [Build](#build)
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

```
mkdir build
```

Then you'll need to install and build (if not already in your local conan repository) the external dependencies.
For this you need conan installed and run the following commands in the root directory

```
conan install . --install-folder build/ --build missing
```

After that you can generate the cmake build files with

```
cmake . -B build -G "<generator>" -DCMAKE_BUILD_TYPE=<config> -DSBX_BUILD_DEMO=<demo> -DSBX_BUILD_TESTS=<tests> -DSBX_GENERATE_DOCS=<docs>
```

Where
| Tag           | Description                                                                  | Example value                   |
|---------------|------------------------------------------------------------------------------|---------------------------------|
| \<generator\> | Generator used to build the project                                          | MinGW Makefiles, Unix Makefiles<br>([available cmake generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html))|
| \<config\>    | Guild configuration                                                          | Debug, Release<br>([available cmake configurations](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html#variable:CMAKE_BUILD_TYPE))|
| \<demo\>      | Should the demo executable be build                                          | True or False                   |
| \<tests\>     | Should the tests be build                                                    | True or False                   |
| \<docs\>      | Should the documentation files be generated                                  | True or False                   |

If you build the tests you can run them by running

```
./build/tests/sbx_test.exe
```

# License
This project is published under the [MIT License](LICENSE).