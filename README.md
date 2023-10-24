# Sandbox Game Engine

This is a game engine project that is currently under heavy development and is mainly for educational purposes.

## 📑 Table of Contents

- [Features](#🚀-features)
- [Getting Started](#🛠️-getting-started)
  - [Prerequisites](#📋-prerequisites)
  - [Cloning the repository](#📥-cloning-the-repository)
  - [Installing dependencies](#📦-installing-dependencies)
  - [Building](#🔨-building)
  - [Running](#🚀-running)
- [Contributing](#🤝-contributing)
- [License](#📝-license)
- [Contact](#📧-contact)

## 🚀 Features

Here are the features that are ready or under development:

- [x] Entity-Component-System architecture
- [ ] 2D / UI rendering 🔜
- [x] 3D rendering
- [x] Physics engine
- [x] Scripting support 
- [ ] Audio engine 🔜
- [ ] Networking support 🔜
- [ ] AI system 🔜

## 🛠️ Getting Started

### 📋 Prerequisites

To build the project, you need the following tools:

- [CMake](https://cmake.org/)
- [Conan](https://conan.io/)
- [MinGW](http://www.mingw.org/) (or any other C++ compiler)

To get started with the project, follow these steps:

_Note: The project is configured so that all commands are run from the root directory of the project_

### 📥 Cloning the repository

To clone the repository, run the following command:

```bash
git clone https://github.com/KabelitzJ/sandbox.git
```

### 📦 Installing dependencies 

The dependencies for this project are managed using `Conan`. To install the dependencies, run the following command:

```bash
conan install . --profile=default --build=missing
```

### 🔨 Building

The project uses `CMake` as the build tool. To build the project, run the following commands:

```bash
cmake . -B "build/debug/" -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
```

Adjust the generator and build type according to your needs.

After that, run the following command to build the project:

```bash
cmake --build "build/debug/"
```
### 🚀 Running

To run the demo executable, run the following command:

```bash
./build/debug/bin/demo.exe
```

## 🤝 Contributing

Contributions to the project are welcome. To contribute, follow these steps:

1. Fork the repository
2. Create a new branch
3. Make your changes
4. Submit a pull request

You can also contribute by opening an issue.

## 📝 License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

Feel free to use this project for your own purposes. If you do you may send me a message, I would love to see what you have created with this project.

## 📧 Contact

GitHub: [KabelitzJ](https://github.com/KabelitzJ)
