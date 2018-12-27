# Matrix Layouts
install(FILES matrix_layouts/razerblade16.json DESTINATION share/razergenie/matrix_layouts)
install(FILES matrix_layouts/razerdefault22.json DESTINATION share/razergenie/matrix_layouts)
install(FILES matrix_layouts/razerblade25.json DESTINATION share/razergenie/matrix_layouts)

# Appstream XML
install(FILES xyz.z3ntu.razergenie.appdata.xml DESTINATION share/metainfo)

# Desktop file
install(FILES xyz.z3ntu.razergenie.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

# Translations
set(TRANSLATION_FILES
    translations/de.ts translations/fr.ts)  # add new languages here!
qt5_add_translation(QM_FILES ${TRANSLATION_FILES})

add_custom_target(translations ALL DEPENDS ${QM_FILES}) # this is apparently needed that the .qm files are generated at all...
install(FILES ${QM_FILES} DESTINATION share/razergenie/translations)
