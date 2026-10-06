test_python = find_program(get_option('test_python'))
qt_test_dep = dependency('qt6', modules: ['Test'])
test_moc = qt.preprocess(moc_headers: ['scrollwidget_test.h', '../src/devicewidget/powerwidget.h', '../src/devicewidget/scrollwidget.h'])
scroll_test = executable('scrollwidget-test',
  ['scrollwidget_test.cpp', '../src/devicewidget/scrollwidget.cpp', '../src/devicewidget/devicecapabilities.cpp', '../src/devicewidget/powerwidget.cpp', '../src/util.cpp', test_moc],
  dependencies: [qt_dep, qt_test_dep, libopenrazer_dep],
  include_directories: include_directories('../src'),
  cpp_args: ['-DTEST_PYTHON="' + test_python.full_path() + '"', '-DMOCK_PATH="' + meson.current_source_dir() / 'mock_scroll.py' + '"'])
test('scroll-controls', find_program('dbus-run-session'), args: ['--', scroll_test], env: ['QT_QPA_PLATFORM=offscreen'])
