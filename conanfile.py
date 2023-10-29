import os
from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain, CMake, cmake_layout
from conan.tools.build import check_max_cppstd, check_min_cppstd

class libsbx_recipe(ConanFile):
  name = "libsbx"
  version = "0.1.0"
  
  # Metadata
  license = "MIT"
  author = "Jonas Kabelitz <jonas-kabelitz@gmx.de>"
  description = "libsbx library"
  topics = (
    "libsbx",
    "lib"
  )

  # Binary configuration
  settings = (
    "os", 
    "compiler", 
    "build_type", 
    "arch"
  )
  options = {
    "shared": [True, False],
    "fPIC": [True, False],
    "build_demo": [True, False],
    "build_tests": [True, False]
  }
  default_options = {
    "shared": False,
    "fPIC": True,
    "build_demo": True,
    "build_tests": True
  }

  # Source directories
  exports_sources = (
    "CMakeLists.txt",
    "libsbx-units/*",
    "libsbx-utility/*",
    "libsbx-async/*",
    "libsbx-audio/*",
    "libsbx-io/*",
    "libsbx-math/*",
    "libsbx-memory/*",
    "libsbx-core/*",
    "libsbx-devices/*",
    "libsbx-graphics/*",
    "libsbx-ecs/*",
    "libsbx-scenes/*",
    "libsbx-scripting/*",
    "libsbx-signals/*",
    "libsbx-assets/*",
    "libsbx-models/*",
    "libsbx-shadows/*",
    "libsbx-physics/*",
    "demo/*"
  )

  def config_options(self):
    if self.settings.os == "Windows":
      self.options.fPIC = False

  def layout(self):
    cmake_layout(self)

    # Define custom layout for build artifacts

    is_compiler_multi_config = self.settings.compiler == "msvc"

    self.folders.build = "build"

    if not is_compiler_multi_config:
      self.folders.build = os.path.join(self.folders.build, str(self.settings.build_type).lower())

    self.folders.generators = os.path.join(self.folders.build, "dependencies")

  def requirements(self):
    self.requires("fmt/10.0.0")
    self.requires("spdlog/1.11.0")
    self.requires("yaml-cpp/0.7.0")
    self.requires("glfw/3.3.8")
    self.requires("sol2/3.3.1")
    self.requires("tinyobjloader/2.0.0-rc10")
    self.requires("spirv-cross/1.3.243.0")
    self.requires("spirv-headers/1.5.4")
    self.requires("stb/cci.20220909")
    self.requires("range-v3/0.12.0")
    self.requires("freetype/2.13.0")
    self.requires("gtest/1.14.0")
    self.requires("openal-soft/1.22.2")
    self.requires("drwav/0.13.12")
    self.requires("drmp3/0.6.34")

  def generate(self):
    deps = CMakeDeps(self)
    toolchain = CMakeToolchain(self)

    deps.generate()
    toolchain.generate()

  def build(self):
    cmake = CMake(self)

    variables = {
      "SBX_BUILD_DEMO": "On" if self.options.build_demo else "Off",
      "SBX_BUILD_SHARED": "On" if self.options.shared else "Off",
      "SBX_BUILD_TESTS": "On" if self.options.build_tests else "Off"
    }

    cmake.configure(variables)
    cmake.build()

  def package(self):
    cmake = CMake(self)
    cmake.install()
