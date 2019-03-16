libopenrazer_version =  '0.0.1'

openrazerlib_sources = [
    'libopenrazer.cpp',
    'device.cpp',
    'led.cpp',
    'manager.cpp',
    'razercapability.cpp'
]
processed = qt5.preprocess(
    moc_headers : ['libopenrazer.h']
)

openrazerlib = library('openrazer',
                       [openrazerlib_sources, processed],
                       version : libopenrazer_version,
                       dependencies : [qt5_dep, razer_test_dep],
                       install : true)

libopenrazer = declare_dependency(
    dependencies: [qt5_dep, razer_test_dep],
    include_directories : include_directories('.'),
    link_with: openrazerlib
)

# install_headers('libopenrazer.h', 'razercapability.h', subdir : 'libopenrazer')

# pkg = import('pkgconfig')
# pkg.generate(name: meson.project_name(),
#              description: 'Qt wrapper around the D-Bus API from OpenRazer',
#              libraries : openrazerlib,
#              version: libopenrazer_version)


# Demo executable
if get_option('buildtype') == 'debug'
  message('Building libopenrazerdemo...')
  executable('libopenrazerdemo',
             'libopenrazerdemo.cpp',
             dependencies : [qt5_dep, razer_test_dep, libopenrazer])
endif
