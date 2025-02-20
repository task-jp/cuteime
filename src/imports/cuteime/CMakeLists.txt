# CMake configuration for the src/imports/cuteime directory

# Find required Qt packages for QML
find_package(Qt5 COMPONENTS Qml Quick REQUIRED)

# Set QML import path
set(CUTEIME_QML_IMPORT_PATH ${CMAKE_INSTALL_PREFIX}/lib/qt5/qml/Cuteime)

# Function to create QML plugin
function(add_cuteime_qml_plugin PLUGIN_NAME)
    cmake_parse_arguments(PLUGIN "" "URI;VERSION" "SOURCES;HEADERS;QML_FILES;RESOURCES" ${ARGN})
    
    # Create plugin library
    add_library(${PLUGIN_NAME} MODULE
        ${PLUGIN_SOURCES}
        ${PLUGIN_HEADERS}
        ${PLUGIN_QML_FILES}
        ${PLUGIN_RESOURCES}
    )

    # Set plugin properties
    set_target_properties(${PLUGIN_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/${PLUGIN_URI}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/${PLUGIN_URI}"
    )

    # Link Qt libraries
    target_link_libraries(${PLUGIN_NAME}
        PRIVATE
            Qt5::Core
            Qt5::Gui
            Qt5::Qml
            Qt5::Quick
            cuteime_qt
    )

    # Install plugin
    install(TARGETS ${PLUGIN_NAME}
        LIBRARY DESTINATION ${CUTEIME_QML_IMPORT_PATH}/${PLUGIN_URI}
        RUNTIME DESTINATION ${CUTEIME_QML_IMPORT_PATH}/${PLUGIN_URI}
    )

    # Install QML files
    if(PLUGIN_QML_FILES)
        install(FILES ${PLUGIN_QML_FILES}
            DESTINATION ${CUTEIME_QML_IMPORT_PATH}/${PLUGIN_URI}
        )
    endif()

    # Generate and install qmldir file
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/qmldir"
        "module ${PLUGIN_URI}\n"
        "plugin ${PLUGIN_NAME}\n"
        "typeinfo plugins.qmltypes\n"
    )
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qmldir"
        DESTINATION ${CUTEIME_QML_IMPORT_PATH}/${PLUGIN_URI}
    )
endfunction()

# Export QML plugin creation function
export(PACKAGE CuteimeQml)

# Add QML module subdirectories only if they exist
foreach(QML_MODULE
    core
    gui
)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${QML_MODULE}/CMakeLists.txt)
        add_subdirectory(${QML_MODULE})
    endif()
endforeach()