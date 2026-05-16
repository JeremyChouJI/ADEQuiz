cmake_minimum_required(VERSION 3.16)

project(ADEQuiz LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(stations
    app/ApplicationPaths.cpp
    flow/FlowParser.cpp
    production/ProductionLine.cpp
    state/StateManager.cpp
    station/StationFactory.cpp
    station/Stations.cpp
    ui/ConsoleUI.cpp
)

target_include_directories(stations
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(ADEQuiz
    main.cpp
)

target_link_libraries(ADEQuiz
    PRIVATE
        stations
)

enable_testing()

add_executable(station_behavior_tests
    tests/station_behavior_tests.cpp
)

target_link_libraries(station_behavior_tests
    PRIVATE
        stations
)

add_test(NAME station_behavior_tests COMMAND station_behavior_tests)

add_executable(flow_parser_tests
    tests/flow_parser_tests.cpp
)

target_link_libraries(flow_parser_tests
    PRIVATE
        stations
)

add_test(NAME flow_parser_tests COMMAND flow_parser_tests)

add_executable(production_line_tests
    tests/production_line_tests.cpp
)

target_link_libraries(production_line_tests
    PRIVATE
        stations
)

add_test(NAME production_line_tests COMMAND production_line_tests)

add_executable(console_ui_tests
    tests/console_ui_tests.cpp
)

target_link_libraries(console_ui_tests
    PRIVATE
        stations
)

add_test(NAME console_ui_tests COMMAND console_ui_tests)

add_executable(state_manager_tests
    tests/state_manager_tests.cpp
)

target_link_libraries(state_manager_tests
    PRIVATE
        stations
)

add_test(NAME state_manager_tests COMMAND state_manager_tests)

add_executable(application_paths_tests
    tests/application_paths_tests.cpp
)

target_link_libraries(application_paths_tests
    PRIVATE
        stations
)

add_test(NAME application_paths_tests COMMAND application_paths_tests)
