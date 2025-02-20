# CMake configuration for plugins

# Find required Qt packages
find_package(Qt5 COMPONENTS Core Gui Widgets WaylandClient REQUIRED)

# Set common plugin properties
set(CUTEIME_PLUGIN_INSTALL_DIR lib/cuteime/plugins)

# Function to create a plugin target
function(add_cuteime_plugin PLUGIN_NAME)
    cmake_parse_arguments(PLUGIN "" "CATEGORY" "SOURCES;HEADERS;UI_FILES;RESOURCES;LIBRARIES" ${ARGN})
    
    # Create plugin library
    add_library(${PLUGIN_NAME} MODULE)

    # Add sources to the target
    target_sources(${PLUGIN_NAME}
        PRIVATE
            ${PLUGIN_SOURCES}
            ${PLUGIN_HEADERS}
            ${PLUGIN_UI_FILES}
            ${PLUGIN_RESOURCES}
    )

    # Set plugin properties
    set_target_properties(${PLUGIN_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/${PLUGIN_CATEGORY}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/${PLUGIN_CATEGORY}"
    )

    # Link common libraries
    target_link_libraries(${PLUGIN_NAME}
        PRIVATE
            cuteime_qt
            Qt5::Core
            Qt5::Gui
            Qt5::Widgets
            Qt5::WaylandClient
            ${PLUGIN_LIBRARIES}
    )

    # Install plugin
    install(TARGETS ${PLUGIN_NAME}
        LIBRARY DESTINATION ${CUTEIME_PLUGIN_INSTALL_DIR}/${PLUGIN_CATEGORY}
        RUNTIME DESTINATION ${CUTEIME_PLUGIN_INSTALL_DIR}/${PLUGIN_CATEGORY}
    )
endfunction()

# Export the function for use in subdirectories
export(PACKAGE CuteIMEPlugin)

# Add plugin subdirectories only if they exist
foreach(PLUGIN_DIR
    clients
    converters
    engines
    inputmethods
    interpreters
    languages
    system
    uis
    utils
)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN_DIR}/CMakeLists.txt)
        add_subdirectory(${PLUGIN_DIR})
    endif()
endforeach()