add_subdirectory(librazer)

set(kcm_razerdrivers_SRCS kcm_razerdrivers.cpp
                          razerimagedownloader.cpp
                          razerpagewidgetitem.cpp
                          )

ki18n_wrap_ui(kcm_razerdrivers_SRCS ../ui/kcm_razerdrivers.ui)

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_library(kcm_razerdrivers MODULE ${kcm_razerdrivers_SRCS})
target_link_libraries(kcm_razerdrivers razer KF5::ConfigWidgets KF5::I18n Qt5::DBus Qt5::Network)

install(TARGETS kcm_razerdrivers DESTINATION ${CMAKE_INSTALL_PLUGINDIR})
