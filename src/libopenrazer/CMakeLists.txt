cmake_minimum_required(VERSION 2.8.12)
project(libopenrazer)

set(LIBRAZER_VERSION_MAJOR 0)
set(LIBRAZER_VERSION_MINOR 0)
set(LIBRAZER_VERSION_PATCH 1)
set(LIBRAZER_VERSION_STRING ${LIBRAZER_VERSION_MAJOR}.${LIBRAZER_VERSION_MINOR}.${LIBRAZER_VERSION_PATCH})

# The name automatically gets "lib" prepended to "openrazer" -> "libopenrazer.so"
add_library(openrazer SHARED
            libopenrazer.cpp
            razercapability.cpp
            )
target_link_libraries(openrazer Qt5::Gui Qt5::DBus Qt5::Xml)

set_target_properties(openrazer PROPERTIES VERSION ${LIBRAZER_VERSION_STRING}
                                       SOVERSION ${LIBRAZER_VERSION_MAJOR})

# Demo executable
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(libopenrazerdemo libopenrazerdemo.cpp)
    target_link_libraries(libopenrazerdemo openrazer Qt5::DBus Qt5::Xml Qt5::Widgets)
endif()

install(TARGETS openrazer DESTINATION ${CMAKE_INSTALL_LIBDIR})
