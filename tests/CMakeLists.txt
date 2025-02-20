# CMake configuration for the tests directory

# Enable testing
enable_testing()

# Find Qt Test package
find_package(Qt5 COMPONENTS Test REQUIRED)

# Add subdirectories for system and unit tests
add_subdirectory(system)
add_subdirectory(unit)

# Function to create a test target
function(add_cuteime_test TEST_NAME TEST_SOURCE)
    # Create test executable
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    
    # Link Qt Test and other required libraries
    target_link_libraries(${TEST_NAME}
        PRIVATE
            Qt5::Test
            Qt5::Core
            Qt5::Gui
            Qt5::Widgets
            cuteime_qt
    )
    
    # Add test to CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    
    # Set test properties
    set_tests_properties(${TEST_NAME} PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=minimal"
    )
endfunction()

# Add test executables
add_cuteime_test(tst_cuteimeabstractipcobject 
    unit/libs/qt4/ipc/cuteimeabstractipcobject/tst_cuteimeabstractipcobject.cpp)
add_cuteime_test(tst_cuteimeapplicationmanager 
    unit/libs/qt4/ipc/cuteimeapplicationmanager/tst_cuteimeapplicationmanager.cpp)
add_cuteime_test(tst_cuteimecandidatemanager 
    unit/libs/qt4/ipc/cuteimecandidatemanager/tst_cuteimecandidatemanager.cpp)
add_cuteime_test(tst_cuteimeinputmethodmanager 
    unit/libs/qt4/ipc/cuteimeinputmethodmanager/tst_cuteimeinputmethodmanager.cpp)
add_cuteime_test(tst_cuteimekeyboardmanager 
    unit/libs/qt4/ipc/cuteimekeyboardmanager/tst_cuteimekeyboardmanager.cpp)
add_cuteime_test(tst_cuteimepreeditmanager 
    unit/libs/qt4/ipc/cuteimepreeditmanager/tst_cuteimepreeditmanager.cpp)

# Include directories for tests
target_include_directories(tst_cuteimeabstractipcobject PRIVATE
    ${CMAKE_SOURCE_DIR}/src/libs/qt
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/libs/qt4/ipc/cuteimeabstractipcobject
)

target_include_directories(tst_cuteimeapplicationmanager PRIVATE
    ${CMAKE_SOURCE_DIR}/src/libs/qt
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/libs/qt4/ipc/cuteimeapplicationmanager
)

target_include_directories(tst_cuteimecandidatemanager PRIVATE
    ${CMAKE_SOURCE_DIR}/src/libs/qt
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/libs/qt4/ipc/cuteimecandidatemanager
)

target_include_directories(tst_cuteimeinputmethodmanager PRIVATE
    ${CMAKE_SOURCE_DIR}/src/libs/qt
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/libs/qt4/ipc/cuteimeinputmethodmanager
)

target_include_directories(tst_cuteimekeyboardmanager PRIVATE
    ${CMAKE_SOURCE_DIR}/src/libs/qt
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/libs/qt4/ipc/cuteimekeyboardmanager
)

target_include_directories(tst_cuteimepreeditmanager PRIVATE
    ${CMAKE_SOURCE_DIR}/src/libs/qt
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/libs/qt4/ipc/cuteimepreeditmanager
)

# Add a custom target to run all tests
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS 
        tst_cuteimeabstractipcobject
        tst_cuteimeapplicationmanager
        tst_cuteimecandidatemanager
        tst_cuteimeinputmethodmanager
        tst_cuteimekeyboardmanager
        tst_cuteimepreeditmanager
)