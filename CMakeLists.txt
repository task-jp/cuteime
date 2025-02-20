cmake_minimum_required(VERSION 3.10)

project(CuteIME VERSION 0.1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Define installation paths
set(CUTEIME_APP_PATH "${CMAKE_INSTALL_PREFIX}/bin")
set(CUTEIME_DATA_PATH "${CMAKE_INSTALL_PREFIX}/share/cuteime")
set(CUTEIME_PLUGIN_PATH "${CMAKE_INSTALL_PREFIX}/lib/cuteime/plugins")
set(CUTEIME_TRANSLATIONS_PATH "${CMAKE_INSTALL_PREFIX}/share/cuteime/translations")

# Add compile definitions for paths and version
add_definitions(
    -DCUTEIME_VERSION="${PROJECT_VERSION}"
    -DCUTEIME_APP_PATH="${CUTEIME_APP_PATH}"
    -DCUTEIME_DATA_PATH="${CUTEIME_DATA_PATH}"
    -DCUTEIME_PLUGIN_PATH="${CUTEIME_PLUGIN_PATH}"
    -DCUTEIME_TRANSLATIONS_PATH="${CUTEIME_TRANSLATIONS_PATH}"
)

# Find Qt packages
find_package(Qt5 COMPONENTS Core Gui Widgets WaylandClient REQUIRED)

# Find and link Wayland
find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND REQUIRED wayland-client wayland-protocols)

# Add Wayland include directories
include_directories(${WAYLAND_INCLUDE_DIRS})

# Link Wayland libraries
link_libraries(${WAYLAND_LIBRARIES})

# Include subdirectories
add_subdirectory(src)
add_subdirectory(share)

# Conditional subdirectories
if(CUTEIME_CONFIG STREQUAL "sdk")
    add_subdirectory(doc)
    add_subdirectory(tools)
endif()

if(CUTEIME_CONFIG STREQUAL "examples")
    add_subdirectory(examples)
endif()

if(CUTEIME_CONFIG STREQUAL "tests")
    add_subdirectory(tests)
    enable_testing()
endif()