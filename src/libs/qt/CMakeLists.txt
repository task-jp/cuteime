# CMake configuration for the Qt library component

# Find required Qt packages
find_package(Qt5 COMPONENTS Core Gui Widgets WaylandClient DBus REQUIRED)

# Check for X11 support
if(UNIX AND NOT APPLE)
    find_package(Qt5 COMPONENTS X11Extras REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(XCB REQUIRED xcb-keysyms)
endif()

# Set platform-specific key manager source
if(APPLE)
    set(KEY_MANAGER_SOURCE ipc/cuteimekeymanager_mac.cpp)
elseif(WIN32)
    set(KEY_MANAGER_SOURCE ipc/cuteimekeymanager_win.cpp)
elseif(UNIX)
    set(KEY_MANAGER_SOURCE ipc/cuteimekeymanager_x11.cpp)
endif()

# Set up source files from different components
set(CUTEIME_QT_SOURCES
    debug/cuteimedebug.cpp
    ipc/cuteimeabstractipcobject.cpp
    ipc/cuteimeapplicationmanager.cpp
    ipc/cuteimecandidatemanager.cpp
    ipc/cuteimeinputmethodmanager.cpp
    ipc/cuteimekeyboardmanager.cpp
    ipc/cuteimekeymanager.cpp
    ${KEY_MANAGER_SOURCE}
    ipc/cuteimepreeditmanager.cpp
    ipc/dbus.cpp
    items/cuteimeconversionitem.cpp
    items/cuteimepreedititem.cpp
    plugins/cuteimeabstractpluginobject.cpp
    plugins/cuteimeconverter.cpp
    plugins/cuteimeengine.cpp
    plugins/cuteimeenginedictionary.cpp
    plugins/cuteimeinputmethod.cpp
    plugins/cuteimeinterpreter.cpp
    plugins/cuteimelanguage.cpp
    plugins/cuteimeplugin.cpp
    plugins/cuteimepluginmanager.cpp
    plugins/cuteimesettingswidget.cpp
    server/cuteimeserver.cpp
    utils/cuteimecommandlineargs.cpp
    utils/cuteimedynamictranslator.cpp
    utils/cuteimekeysequence.cpp
    utils/qbinding.cpp
    utils/qboolsignal.cpp
    utils/qmotion.cpp
    utils/setproperty.cpp
    widgets/cuteimekeywidget.cpp
    widgets/cuteimewidget.cpp
)

set(CUTEIME_QT_HEADERS
    cuteimeglobal.h
    debug/cuteimedebug.h
    ipc/cuteimeabstractipcobject.h
    ipc/cuteimeapplicationmanager.h
    ipc/cuteimecandidatemanager.h
    ipc/cuteimeinputmethodmanager.h
    ipc/cuteimekeyboardmanager.h
    ipc/cuteimekeymanager.h
    ipc/cuteimepreeditmanager.h
    ipc/dbus.h
    items/cuteimeconversionitem.h
    items/cuteimepreedititem.h
    plugins/cuteimeabstractpluginobject.h
    plugins/cuteimeconverter.h
    plugins/cuteimeengine.h
    plugins/cuteimeenginedictionary.h
    plugins/cuteimeinputmethod.h
    plugins/cuteimeinterpreter.h
    plugins/cuteimelanguage.h
    plugins/cuteimeplugin.h
    plugins/cuteimepluginmanager.h
    plugins/cuteimesettingswidget.h
    server/cuteimeserver.h
    utils/cuteimecommandlineargs.h
    utils/cuteimedynamictranslator.h
    utils/cuteimekeysequence.h
    utils/qbinding.h
    utils/qboolsignal.h
    utils/qmotion.h
    utils/setproperty.h
    widgets/cuteimekeywidget.h
    widgets/cuteimewidget.h
)

# Add Qt resource files
set(CUTEIME_QT_RESOURCES
    qt.qrc
)

# Add UI files
set(CUTEIME_QT_UI_FILES
    plugins/cuteimesettingswidget.ui
)

# Create library target
add_library(cuteime_qt SHARED
    ${CUTEIME_QT_SOURCES}
    ${CUTEIME_QT_HEADERS}
    ${CUTEIME_QT_RESOURCES}
    ${CUTEIME_QT_UI_FILES}
)

# Set library properties
set_target_properties(cuteime_qt PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    DEFINE_SYMBOL CUTEIME_LIBRARY
)

# Set include directories
target_include_directories(cuteime_qt PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/debug
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc
    ${CMAKE_CURRENT_SOURCE_DIR}/items
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins
    ${CMAKE_CURRENT_SOURCE_DIR}/server
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
)

# Link libraries
target_link_libraries(cuteime_qt
    PUBLIC
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::WaylandClient
        Qt5::DBus
)

# Add X11 support if needed
if(UNIX AND NOT APPLE)
    target_link_libraries(cuteime_qt
        PUBLIC
            Qt5::X11Extras
            ${XCB_LIBRARIES}
    )
    target_include_directories(cuteime_qt PUBLIC ${XCB_INCLUDE_DIRS})
endif()

# Install targets
install(TARGETS cuteime_qt
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers if SDK is enabled
if(CUTEIME_CONFIG STREQUAL "sdk")
    install(FILES ${CUTEIME_QT_HEADERS}
        DESTINATION include/cuteime/qt${Qt5Core_VERSION_MAJOR}
    )
endif()