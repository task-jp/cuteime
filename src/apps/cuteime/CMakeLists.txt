# CMake configuration for the src/apps/cuteime directory

# Find required Qt packages
find_package(Qt5 COMPONENTS Core Gui Widgets WaylandClient REQUIRED)

# Add source files
set(CUTEIME_APP_SOURCES
    application.cpp
    main.cpp
)

# Add UI files if any exist
set(CUTEIME_APP_UIS
    # Add .ui files here
)

# Add resource files if any exist
set(CUTEIME_APP_RESOURCES
    ${CMAKE_SOURCE_DIR}/cuteime.qrc
)

# Add executable
add_executable(cuteime_app 
    ${CUTEIME_APP_SOURCES}
    ${CUTEIME_APP_UIS}
    ${CUTEIME_APP_RESOURCES}
)

# Include directories
target_include_directories(cuteime_app
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/libs/qt
        ${CMAKE_SOURCE_DIR}/src/libs/qt/server
        ${CMAKE_SOURCE_DIR}/src/libs/qt/ipc
        ${CMAKE_SOURCE_DIR}/src/libs/qt/utils
        ${CMAKE_SOURCE_DIR}/src/libs/qt/widgets
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

# Link libraries
target_link_libraries(cuteime_app
    PRIVATE
        cuteime_qt
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::WaylandClient
        ${WAYLAND_LIBRARIES}
)

# Install target
install(TARGETS cuteime_app
    RUNTIME DESTINATION bin
)

# Install desktop file and icons if they exist
install(FILES ${CMAKE_SOURCE_DIR}/share/applications/cuteime.desktop
    DESTINATION share/applications
)

install(FILES ${CMAKE_SOURCE_DIR}/share/icons/cuteime.png
    DESTINATION share/icons/hicolor/128x128/apps
)