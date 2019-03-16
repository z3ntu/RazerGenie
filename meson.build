project('razergenie', 'cpp',
        version : '0.7',
        meson_version : '>=0.44.0',
        default_options : ['cpp_std=c++11'])

qt5 = import('qt5')
qt5_dep = dependency('qt5', modules: ['Core', 'Gui', 'Widgets', 'DBus', 'Network', 'Xml'])

razer_test_dep = dependency('razer_test')

if get_option('enable_experimental')
  add_global_arguments('-DENABLE_EXPERIMENTAL', language : 'cpp')
  message('razergenie: Experimental features enabled.')
else
  message('razergenie: Experimental features disabled.')
endif

if get_option('include_matrix_discovery')
  add_global_arguments('-DINCLUDE_MATRIX_DISCOVERY', language : 'cpp')
  message('razergenie: Matrix discovery feature included.')
else
  message('razergenie: Matrix discovery feature not included.')
endif

install_data('logo/xyz.z3ntu.razergenie.svg', install_dir : join_paths(get_option('datadir'), 'icons/hicolor/scalable/apps'))

subdir('data')
subdir('src/libopenrazer')
subdir('src')
