# The name automatically gets "lib" prepended to "razer" -> "librazer.so"
librazer_sources = ['librazer.cpp', 'razercapability.cpp']
librazer = shared_library('razer',
                          librazer_sources,
                          version : librazer_version,
                          soversion : librazer_version.split('.')[0],
                          dependencies : qt5_dep,
                          install : true)

# Demo executable
if get_option('buildtype') == 'debug'
  message('Building librazerdemo...')
  librazerdemo = executable('librazerdemo', 'librazer.cpp',
                            dependencies : qt5_dep,
                            link_with : librazer)
endif
