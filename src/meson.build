conf_data = configuration_data()
conf_data.set('version', meson.project_version())
conf_data.set('datadir', join_paths(get_option('prefix'), get_option('datadir'), 'razergenie'))

configure_file(input : 'config.h.in',
               output : 'config.h',
               configuration : conf_data)

razergenie_sources = ['main.cpp', 'razergenie.cpp', 'razerimagedownloader.cpp', 'devicewidget.cpp', 'devicelistwidget.cpp', 'ledwidget.cpp', 'util.cpp',
                      'customeditor/customeditor.cpp', 'customeditor/matrixpushbutton.cpp', 'customeditor/kbdlayout.cpp', 'preferences/preferences.cpp']

processed = qt5.preprocess(
  moc_headers : ['razergenie.h', 'razerimagedownloader.h', 'devicelistwidget.h', 'ledwidget.h', 'customeditor/customeditor.h', 'preferences/preferences.h'],
  ui_files : '../ui/razergenie.ui'
)

razergenie = executable('razergenie',
                        [razergenie_sources, processed],
                        dependencies : [qt5_dep, razer_test_dep, libopenrazer_dep],
                        install : true)
