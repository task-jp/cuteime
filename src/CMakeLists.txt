# CMake configuration for the src directory

# Find required Qt packages
find_package(Qt5 COMPONENTS Core Gui Widgets WaylandClient REQUIRED)

# Add subdirectories
add_subdirectory(apps)
add_subdirectory(imports)
add_subdirectory(libs)
add_subdirectory(plugins)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/apps/cuteime
    ${CMAKE_CURRENT_SOURCE_DIR}/imports/cuteime/core
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/gtk
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/qt
)

# Set compile definitions
add_definitions(-DQT_NO_KEYWORDS)

# Set common properties
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Install directories
set(CUTEIME_PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib/cuteime/plugins)
set(CUTEIME_DATA_DIR ${CMAKE_INSTALL_PREFIX}/share/cuteime)
set(CUTEIME_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/cuteime)

# Add compile definitions
add_definitions(
    -DCUTEIME_PLUGIN_PATH="${CUTEIME_PLUGIN_DIR}"
    -DCUTEIME_DATA_PATH="${CUTEIME_DATA_DIR}"
)