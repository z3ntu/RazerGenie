# Matrix Layouts
matrixlayoutdir = join_paths(get_option('datadir'), 'razergenie/matrix_layouts')
install_data('matrix_layouts/razerblade16.json', install_dir : matrixlayoutdir)
install_data('matrix_layouts/razerdefault22.json', install_dir : matrixlayoutdir)
install_data('matrix_layouts/razerblade25.json', install_dir : matrixlayoutdir)

# Appstream XML
metadatadir = join_paths(get_option('datadir'), 'metainfo')
install_data('xyz.z3ntu.razergenie.appdata.xml', install_dir: metadatadir)

# Desktop file
install_data('xyz.z3ntu.razergenie.desktop', install_dir : join_paths(get_option('datadir'), 'applications'))

# Translations
languages = ['de']  # add new languages here!
translationsdir = join_paths(get_option('datadir'), 'razergenie/translations')
lrelease = find_program(['lrelease-qt5', 'lrelease'])

foreach lang : languages
    custom_target('translation_' + lang,
        output : lang + '.qm',
        input : 'translations/'+lang+'.ts',
        command : [lrelease, '@INPUT@', '-qm', '@OUTPUT@'],
        install : true,
        install_dir : translationsdir)
endforeach
