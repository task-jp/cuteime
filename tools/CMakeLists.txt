# CMake configuration for the tools directory

# Add subdirectories for applications and plugins
add_subdirectory(apps)
add_subdirectory(plugins)

# Include directories for applications
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/apps/gtkbutton
    ${CMAKE_CURRENT_SOURCE_DIR}/apps/gtkicon
)

# Add source files for applications
set(TOOLS_APPS_SRC
    apps/gtkbutton/main.c
    apps/gtkicon/main.c
)

# Add executables for applications
add_executable(gtkbutton ${TOOLS_APPS_SRC})
add_executable(gtkicon ${TOOLS_APPS_SRC})

# Link libraries for applications
target_link_libraries(gtkbutton ${GTK_LIBRARIES})
target_link_libraries(gtkicon ${GTK_LIBRARIES})