project('razergenie', 'cpp',
        version : '1.3.0',
        meson_version : '>=0.57.0',
        default_options : ['cpp_std=c++17'])

# Disable any API deprecated in 5.15 or older Qt versions
add_project_arguments('-DQT_DISABLE_DEPRECATED_BEFORE=0x050F00', language : 'cpp')

qt = import('qt6')
qt_dep = dependency('qt6', modules: ['Core', 'DBus', 'Gui', 'Network', 'Widgets'])

libopenrazer_dep = dependency('libopenrazer', version : '>=0.4.0', fallback : ['libopenrazer', 'libopenrazer_dep'])

if build_machine.system() == 'darwin'
  razergenie_data_dir = 'Contents/Resources'
else
  razergenie_data_dir = get_option('datadir') / 'razergenie'
endif

subdir('data')
subdir('src')
