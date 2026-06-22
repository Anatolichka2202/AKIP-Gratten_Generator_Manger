find_package(Qt6 COMPONENTS Test QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Test REQUIRED)
endif()

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
