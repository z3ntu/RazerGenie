project('razergenie', 'cpp',
        default_options : ['cpp_std=c++11'])

razergenie_version = '0.9.0'
libopenrazer_version = '0.0.1'

qt5 = import('qt5')
qt5_dep = dependency('qt5', modules: ['Widgets', 'DBus', 'Network', 'Xml'])

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
