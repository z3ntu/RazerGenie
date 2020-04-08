# Matrix Layouts

if build_machine.system() == 'darwin'
  install_data('Info.plist', install_dir : 'Contents')

  svg2icns = find_program(meson.project_build_root() / '../scripts/svg2icns.sh')
  custom_target('xyz.z3ntu.razergenie.icns',
                input : 'xyz.z3ntu.razergenie.svg',
                output : 'xyz.z3ntu.razergenie.icns',
                command : [svg2icns, '@INPUT@', '@OUTPUT@'],
                install : true,
                install_dir : 'Contents/Resources')
endif

install_data('matrix_layouts/razerblade16.json',
             'matrix_layouts/razerblade25.json',
             'matrix_layouts/razerdefault18.json',
             'matrix_layouts/razerdefault22.json',
             'matrix_layouts/razerhunt22.json',
             'matrix_layouts/razermouse20.json',
             'matrix_layouts/razermousepad15.json',
             install_dir : razergenie_data_dir / 'matrix_layouts')

# Logo
install_data('xyz.z3ntu.razergenie.svg',
             install_dir : get_option('datadir') / 'icons/hicolor/scalable/apps')

# Appstream XML
install_data('xyz.z3ntu.razergenie.appdata.xml',
             install_dir : get_option('datadir') / 'metainfo')

# Desktop file
install_data('xyz.z3ntu.razergenie.desktop',
             install_dir : get_option('datadir') / 'applications')

subdir('translations')
