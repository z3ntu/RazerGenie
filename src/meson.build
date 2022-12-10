conf_data = configuration_data({
  'RAZERGENIE_VERSION' : '"' + meson.project_version() + '"',
  'RAZERGENIE_DATADIR' : '"' + get_option('prefix') / get_option('datadir') / 'razergenie' + '"',
})

configure_file(output : 'config.h',
               configuration : conf_data)

razergenie_sources = files([
  'customeditor/customeditor.cpp',
  'customeditor/matrixpushbutton.cpp',
  'preferences/preferences.cpp',
  'devicelistwidget.cpp',
  'devicewidget.cpp',
  'ledwidget.cpp',
  'main.cpp',
  'razergenie.cpp',
  'razerimagedownloader.cpp',
  'util.cpp',
])

processed = qt.preprocess(
  moc_headers : files([
    'customeditor/customeditor.h',
    'preferences/preferences.h',
    'devicelistwidget.h',
    'ledwidget.h',
    'razergenie.h',
    'razerimagedownloader.h',
  ]),
  ui_files : files([
    '../ui/razergenie.ui',
  ])
)

razergenie = executable('razergenie',
                        [razergenie_sources, processed],
                        dependencies : [qt_dep, libopenrazer_dep],
                        install : true)
