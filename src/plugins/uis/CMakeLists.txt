# CMake configuration for UI plugins

# Set common properties for UI plugins
set(CUTEIME_UI_PLUGIN_INSTALL_DIR ${CUTEIME_PLUGIN_INSTALL_DIR}/uis)

# Common include directories for UI plugins
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/libs/qt
)

# Common compile definitions for UI plugins
add_definitions(-DQT_PLUGIN -DCUTEIME_PLUGIN)

# Add UI plugin subdirectories only if they exist
foreach(UI_PLUGIN
    aboutcuteime
    candidatewindow
    settings
    systemtray
    toolbar
    userdictionary
)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${UI_PLUGIN}/CMakeLists.txt)
        add_subdirectory(${UI_PLUGIN})
    endif()
endforeach()