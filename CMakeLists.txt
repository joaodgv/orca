# CMakeLists.txt for OrcaHUD project
# Minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Project name
project(OrcaHUD)

# Create the executable
add_executable(OrcaHUD gui/main.cpp gui/main.h gui/simple.cpp gui/simple.h)

# find wxWidgets package
find_package(wxWidgets REQUIRED COMPONENTS core base)
if(wxWidgets_FOUND)
  include(${wxWidgets_USE_FILE})
  # and for each of your dependent executable/library targets:
    target_link_libraries(OrcaHUD ${wxWidgets_LIBRARIES})
endif()
