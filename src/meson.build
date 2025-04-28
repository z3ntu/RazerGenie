conf_data = configuration_data({
  'RAZERGENIE_VERSION' : '"' + meson.project_version() + '"',
  'RAZERGENIE_DATADIR' : '"' + get_option('prefix') / get_option('datadir') / 'razergenie' + '"',
})

configure_file(output : 'config.h',
               configuration : conf_data)

razergenie_sources = files([
  'customeditor/customeditor.cpp',
  'customeditor/matrixpushbutton.cpp',
  'devicewidget/clickeventfilter.cpp',
  'devicewidget/devicewidget.cpp',
  'devicewidget/dpicomboboxwidget.cpp',
  'devicewidget/dpisliderwidget.cpp',
  'devicewidget/dpistagewidget.cpp',
  'devicewidget/ledwidget.cpp',
  'devicewidget/lightingwidget.cpp',
  'devicewidget/performancewidget.cpp',
  'devicewidget/powerwidget.cpp',
  'preferences/preferences.cpp',
  'deviceinfodialog.cpp',
  'devicelistwidget.cpp',
  'inputremappinginfodialog.cpp',
  'main.cpp',
  'razergenie.cpp',
  'razerimagedownloader.cpp',
  'util.cpp',
])

processed = qt.preprocess(
  moc_headers : files([
    'customeditor/customeditor.h',
    'devicewidget/clickeventfilter.h',
    'devicewidget/devicewidget.h',
    'devicewidget/dpicomboboxwidget.h',
    'devicewidget/dpisliderwidget.h',
    'devicewidget/dpistagewidget.h',
    'devicewidget/ledwidget.h',
    'devicewidget/lightingwidget.h',
    'devicewidget/performancewidget.h',
    'devicewidget/powerwidget.h',
    'preferences/preferences.h',
    'deviceinfodialog.h',
    'devicelistwidget.h',
    'inputremappinginfodialog.h',
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
