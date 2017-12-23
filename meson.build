project('razergenie', 'cpp',
        default_options : ['cpp_std=c++11'])

razergenie_version = '0.6'
libopenrazer_version = '0.0.1'

qt5 = import('qt5')
qt5_dep = dependency('qt5', modules: ['Widgets', 'DBus', 'Network', 'Xml'])

if get_option('enable_experimental')
  add_global_arguments('-DENABLE_EXPERIMENTAL', language : 'cpp')
  message('razergenie: Experimental features enabled.')
else
  message('razergenie: Experimental features disabled.')
endif

install_data('logo/razergenie.svg', install_dir : join_paths(get_option('datadir'), 'icons/hicolor/scalable/apps'))
install_data('other/razergenie.desktop', install_dir : join_paths(get_option('datadir'), 'applications'))

subdir('data')
subdir('src/librazer')
subdir('src')
