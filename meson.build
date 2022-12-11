project('razergenie', 'cpp',
        version : '0.9.0',
        meson_version : '>=0.49.0',
        default_options : ['cpp_std=c++11'])

# Disable any API deprecated in 5.15 or older Qt versions
add_project_arguments('-DQT_DISABLE_DEPRECATED_BEFORE=0x050F00', language : 'cpp')

qt = import('qt5')
qt_dep = dependency('qt5', modules: ['Core', 'DBus', 'Gui', 'Network', 'Widgets'])

libopenrazer_dep = dependency('libopenrazer', fallback : ['libopenrazer', 'libopenrazer_dep'])

install_data('logo/xyz.z3ntu.razergenie.svg',
             install_dir : get_option('datadir') / 'icons/hicolor/scalable/apps')

subdir('data')
subdir('src')
