add_subdirectory(librazer)

set(kcm_razerdrivers_SRCS main.cpp
                          kcm_razerdrivers.cpp
                          razerimagedownloader.cpp
                          razerpagewidgetitem.cpp
                          customeditor.cpp
                          )

QT5_WRAP_UI(kcm_razerdrivers_SRCS ../ui/kcm_razerdrivers.ui
                                  ../ui/daemon_not_available.ui)

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_executable(kcm_razerdrivers ${kcm_razerdrivers_SRCS})
target_link_libraries(kcm_razerdrivers razer Qt5::Widgets Qt5::DBus Qt5::Network KF5::WidgetsAddons)

install(TARGETS kcm_razerdrivers DESTINATION ${CMAKE_INSTALL_BINDIR})
