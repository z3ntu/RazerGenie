add_subdirectory(librazer)

if(ENABLE_EXPERIMENTAL)
    add_definitions(-DENABLE_EXPERIMENTAL)
    message("razergenie: Experimental features enabled.")
else()
    message("razergenie: Experimental features disabled.")
endif()

set(razergenie_SRCS main.cpp
                    razergenie.cpp
                    razerimagedownloader.cpp
                    razerdevicewidget.cpp
                    devicelistwidget.cpp
                    customeditor.cpp
                    )

QT5_WRAP_UI(razergenie_SRCS ../ui/razergenie.ui)

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_executable(razergenie ${razergenie_SRCS})
target_link_libraries(razergenie razer Qt5::Widgets Qt5::DBus Qt5::Network)

install(TARGETS razergenie DESTINATION ${CMAKE_INSTALL_BINDIR})
