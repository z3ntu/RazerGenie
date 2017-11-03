# Matrix Layouts
install(FILES matrix_layouts/razerblade25.json DESTINATION share/razergenie/matrix_layouts)
install(FILES matrix_layouts/razerdefault22.json DESTINATION share/razergenie/matrix_layouts)

# Translations
set(TRANSLATION_FILES
    translations/de.ts)  # add new languages here!
qt5_add_translation(QM_FILES ${TRANSLATION_FILES})

add_custom_target(translations ALL DEPENDS ${QM_FILES}) # this is apparently needed that the .qm files are generated at all...
install(FILES ${QM_FILES} DESTINATION share/razergenie/translations)
