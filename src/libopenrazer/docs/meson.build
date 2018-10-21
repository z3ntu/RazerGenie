qdoc = find_program('qdoc', required : false)

if qdoc.found()
  # TODO Fix up includes to work automatically (especially the gcc include)
  custom_target('docs',
    output: 'html',
    input : 'config.qdocconf',
    command: [qdoc, '@INPUT@', '-I', meson.current_source_dir(),
              '-I/usr/include/qt', '-I/usr/include/qt/QtCore', '-I/usr/include/qt/QtGui', '-I/usr/include/qt/QtDBus', '-I/usr/include/qt/QtXml',
              '-I/usr/lib/gcc/x86_64-pc-linux-gnu/8.2.0/include'])
endif
