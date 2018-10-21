project('razergenie', 'cpp',
        version : '0.8',
        meson_version : '>=0.44.0',
        default_options : ['cpp_std=c++11'])

qt5 = import('qt5')
qt5_dep = dependency('qt5', modules: ['Core', 'DBus', 'Gui', 'Network', 'Widgets'])

razer_test_dep = dependency('razer_test', fallback : ['razer_test', 'razer_test_dep'])
libopenrazer_dep = dependency('libopenrazer', fallback : ['libopenrazer', 'libopenrazer_dep'])

if get_option('include_matrix_discovery')
  add_global_arguments('-DINCLUDE_MATRIX_DISCOVERY', language : 'cpp')
  message('razergenie: Matrix discovery feature included.')
else
  message('razergenie: Matrix discovery feature not included.')
endif

install_data('logo/xyz.z3ntu.razergenie.svg', install_dir : join_paths(get_option('datadir'), 'icons/hicolor/scalable/apps'))

subdir('data')
subdir('src')
