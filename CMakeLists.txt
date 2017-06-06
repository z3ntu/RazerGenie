cmake_minimum_required(VERSION 2.8.12)
project(razergenie)

# Experimental features switch
option(ENABLE_EXPERIMENTAL "ENABLE_EXPERIMENTAL" OFF)

# Fix for GCC < 6.0
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(RAZERGENIE_VERSION_MAJOR 0)
set(RAZERGENIE_VERSION_MINOR 0)
set(RAZERGENIE_VERSION_PATCH 1)
set(RAZERGENIE_VERSION_STRING ${RAZERGENIE_VERSION_MAJOR}.${RAZERGENIE_VERSION_MINOR}.${RAZERGENIE_VERSION_PATCH})

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 REQUIRED COMPONENTS DBus Network Xml Widgets)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

include(KDEInstallDirs)
#include(KDECompilerSettings) # this fucking piece of shit breaks my shared library -.-
include(KDECMakeSettings)
include(FeatureSummary)
include(CheckIncludeFiles)

add_subdirectory(logo)
add_subdirectory(other)
add_subdirectory(src)
