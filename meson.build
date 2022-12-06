project('razergenie', 'cpp',
        version : '0.8.1',
        meson_version : '>=0.44.0',
        default_options : ['cpp_std=c++11'])

if get_option('include_matrix_discovery')
  add_global_arguments('-DINCLUDE_MATRIX_DISCOVERY', language : 'cpp')
  message('razergenie: Matrix discovery feature included.')
else
  message('razergenie: Matrix discovery feature not included.')
endif

# Disable any API deprecated in 5.15 or older Qt versions
add_global_arguments('-DQT_DISABLE_DEPRECATED_BEFORE=0x050F00', language : 'cpp')

qt5 = import('qt5')
qt5_dep = dependency('qt5', modules: ['Core', 'DBus', 'Gui', 'Network', 'Widgets'])

libopenrazer_dep = dependency('libopenrazer', fallback : ['libopenrazer', 'libopenrazer_dep'])

install_data('logo/xyz.z3ntu.razergenie.svg', install_dir : join_paths(get_option('datadir'), 'icons/hicolor/scalable/apps'))

subdir('data')
subdir('src')
