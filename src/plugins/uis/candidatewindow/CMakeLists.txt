# CMake configuration for candidatewindow plugin

set(PLUGIN_NAME cuteime-candidatewindow)

# Source files
set(PLUGIN_SOURCES
    list.cpp
    object.cpp
    plugin.cpp
)

# Header files
set(PLUGIN_HEADERS
    list.h
    object.h
    plugin.h
)

# UI files
set(PLUGIN_UI_FILES
    list.ui
)

# Resource files
set(PLUGIN_RESOURCES
    candidatewindow.json
)

# Use the plugin creation function defined in src/plugins/CMakeLists.txt
add_cuteime_plugin(${PLUGIN_NAME}
    CATEGORY "uis"
    SOURCES ${PLUGIN_SOURCES}
    HEADERS ${PLUGIN_HEADERS}
    UI_FILES ${PLUGIN_UI_FILES}
    RESOURCES ${PLUGIN_RESOURCES}
    LIBRARIES
        cuteime_qt
)

# Additional include directories specific to this plugin
target_include_directories(${PLUGIN_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Additional compile definitions if needed
target_compile_definitions(${PLUGIN_NAME} PRIVATE
    QT_PLUGIN
    CUTEIME_PLUGIN
)