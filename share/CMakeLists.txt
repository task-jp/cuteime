# CMake configuration for the share directory

# Find Qt Linguist tools
find_package(Qt5 COMPONENTS LinguistTools REQUIRED)

# Set up translations
set(TS_FILES
    cuteime/translations/cuteime_en.ts
    cuteime/translations/cuteime_ja.ts
)

# Generate QM files from TS files
qt5_add_translation(QM_FILES ${TS_FILES})

# Install desktop file
install(FILES 
    applications/cuteime.desktop
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications
)

# Install application icon
install(FILES
    applications/cuteime.ico
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps
)

# Install icons
set(ICON_FILES
    icons/cuteime_blue.png
    icons/cuteime_cyan.png
    icons/cuteime_gray.png
    icons/cuteime_green.png
    icons/cuteime_magenta.png
    icons/cuteime_red.png
    icons/cuteime_yellow.png
    icons/cuteime.png
)

# Install icons to hicolor theme
foreach(ICON ${ICON_FILES})
    install(FILES ${ICON}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/128x128/apps
    )
endforeach()

# Install translations
install(FILES ${QM_FILES}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/cuteime/translations
)

# Add custom target for translations
add_custom_target(translations ALL DEPENDS ${QM_FILES})