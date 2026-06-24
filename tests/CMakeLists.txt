find_package(Qt6 COMPONENTS Test QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Test REQUIRED)
endif()

# Test MockTransport
add_executable(test_mock_transport
    test_mock_transport.cpp
    ${CMAKE_SOURCE_DIR}/mocktransport.cpp
    ${CMAKE_SOURCE_DIR}/itransport.h
)

set_target_properties(test_mock_transport PROPERTIES
    AUTOMOC ON
)

target_link_libraries(test_mock_transport
    PRIVATE Qt${QT_VERSION_MAJOR}::Test
)

target_include_directories(test_mock_transport
    PRIVATE ${CMAKE_SOURCE_DIR}
)

add_test(NAME MockTransportTests COMMAND test_mock_transport)

# Test GrattenGa1483Controller with MockTransport
add_executable(test_gratten_controller
    test_gratten_controller.cpp
    ${CMAKE_SOURCE_DIR}/mocktransport.cpp
    ${CMAKE_SOURCE_DIR}/itransport.h
    ${CMAKE_SOURCE_DIR}/grattenga1483controller.cpp
    ${CMAKE_SOURCE_DIR}/abstractscpicontroller.cpp
    ${CMAKE_SOURCE_DIR}/laninterface.cpp
    ${CMAKE_SOURCE_DIR}/iakipcontroller.h
)

set_target_properties(test_gratten_controller PROPERTIES
    AUTOMOC ON
)

target_link_libraries(test_gratten_controller
    PRIVATE Qt${QT_VERSION_MAJOR}::Test
    PRIVATE Qt${QT_VERSION_MAJOR}::Network
)

target_include_directories(test_gratten_controller
    PRIVATE ${CMAKE_SOURCE_DIR}
)

add_test(NAME GrattenControllerTests COMMAND test_gratten_controller)

# Test SettingsManager
add_executable(test_settings_manager
    test_settings_manager.cpp
    ${CMAKE_SOURCE_DIR}/settingsmanager.cpp
    ${CMAKE_SOURCE_DIR}/settingsmanager.h
)

set_target_properties(test_settings_manager PROPERTIES
    AUTOMOC ON
)

target_link_libraries(test_settings_manager
    PRIVATE Qt${QT_VERSION_MAJOR}::Test
    PRIVATE Qt${QT_VERSION_MAJOR}::Core
)

target_include_directories(test_settings_manager
    PRIVATE ${CMAKE_SOURCE_DIR}
)

add_test(NAME SettingsManagerTests COMMAND test_settings_manager)
