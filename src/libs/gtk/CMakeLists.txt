# CMake configuration for the src/libs/gtk directory

# Find GTK3 package
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Add source files
set(GTK_SRC_FILES
    debug/cuteimedebug.c
    ipc/cuteimeabstractipcobject.c
    ipc/cuteimeapplicationmanager.c
    ipc/cuteimeinputmethodmanager.c
    ipc/cuteimekeyboardmanager.c
    ipc/cuteimekeymanager.c
    ipc/cuteimepreedititem.c
    ipc/cuteimepreeditmanager.c
)

# Add header files
set(GTK_HEADER_FILES
    debug/cuteimedebug.h
    ipc/cuteimeabstractipcobject.h
    ipc/cuteimeapplicationmanager.h
    ipc/cuteimeinputmethodmanager.h
    ipc/cuteimekeyboardmanager.h
    ipc/cuteimekeymanager.h
    ipc/cuteimepreedititem.h
    ipc/cuteimepreeditmanager.h
)

# Create library
add_library(cuteime_gtk SHARED
    ${GTK_SRC_FILES}
    ${GTK_HEADER_FILES}
)

# Set library properties
set_target_properties(cuteime_gtk PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Include directories
target_include_directories(cuteime_gtk
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/debug
        ${CMAKE_CURRENT_SOURCE_DIR}/ipc
    PRIVATE
        ${GTK3_INCLUDE_DIRS}
)

# Add compile definitions
target_compile_definitions(cuteime_gtk
    PRIVATE
        CUTEIME_GTK_LIBRARY
)

# Link libraries
target_link_libraries(cuteime_gtk
    PUBLIC
        ${GTK3_LIBRARIES}
)

# Set compile options from GTK3
target_compile_options(cuteime_gtk
    PRIVATE
        ${GTK3_CFLAGS_OTHER}
)

# Install targets
install(TARGETS cuteime_gtk
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers if SDK is enabled
if(CUTEIME_CONFIG STREQUAL "sdk")
    install(FILES ${GTK_HEADER_FILES}
        DESTINATION include/cuteime/gtk
    )
endif()