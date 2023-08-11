import os
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
from conan.tools.build import check_max_cppstd, check_min_cppstd

class libsbx_recipe(ConanFile):
  name = "libsbx"
  version = "0.1.0"
  
  # Metadata
  license = "MIT"
  author = "Jonas Kabelitz <jonas-kabelitz@gmx.de>"
  description = "libsbx library"
  topics = ("libsbx", "lib")

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
    "build_demo": [True, False]
  }
  default_options = {
    "shared": False,
    "fPIC": True,
    "build_demo": True
  }

  # Source directories
  exports_sources = (
    "CMakeLists.txt",
    "libsbx-units",
    "libsbx-utility",
    "libsbx-async",
    "libsbx-io",
    "libsbx-math",
    "libsbx-memory",
    "libsbx-core",
    "libsbx-devices",
    "libsbx-graphics",
    "libsbx-ecs",
    "libsbx-scenes",
    "libsbx-scripting",
    "libsbx-signals",
    "libsbx-assets",
    "libsbx-models",
    "demo/*"
  )

  generators = (
    "CMakeToolchain",
    "CMakeDeps"
  )

  def config_options(self):
    if self.settings.os == "Windows":
      del self.options.fPIC

  def layout(self):
    cmake_layout(self)

    # Define custom layout for build artifacts

    is_compiler_multi_config = self.settings.get_safe("compiler") == "msvc"

    if is_compiler_multi_config:
      self.folders.generators = os.path.join("build", "dependencies")
    else:
      self.folders.generators = os.path.join("build", str(self.settings.build_type).lower(), "dependencies")

  def validate(self):
    check_min_cppstd(self, "14")
    check_max_cppstd(self, "23")

  def requirements(self):
    self.requires("fmt/10.0.0")
    self.requires("spdlog/1.11.0")
    self.requires("yaml-cpp/0.7.0")
    self.requires("glfw/3.3.8")
    self.requires("sol2/3.3.0")
    self.requires("tinyobjloader/2.0.0-rc10")
    self.requires("spirv-cross/1.3.243.0")
    self.requires("spirv-headers/1.5.4")
    self.requires("stb/cci.20220909")

  def build(self):
    cmake = CMake(self)

    variables = {
      "SBX_BUILD_DEMO": "On" if self.options["build_demo"] else "Off",
      "SBX_BUILD_SHARED": "On" if self.options["shared"] else "Off"
    }

    cmake.configure(variables)
    cmake.build()

  def package(self):
    cmake = CMake(self)
    cmake.install()

  def package_info(self):
    self.cpp_info.libs = ["libsbx"]
