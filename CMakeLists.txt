cmake_minimum_required(VERSION 2.8.12)
project(kcm_razerdrivers)

set(KCM_RAZERDRIVERS_VERSION "0.0.1")

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 REQUIRED COMPONENTS DBus Network)
find_package(KF5 REQUIRED COMPONENTS ConfigWidgets I18n)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)
include(FeatureSummary)
include(CheckIncludeFiles)

add_subdirectory(logo)
add_subdirectory(other)
add_subdirectory(src)
