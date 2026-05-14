cmake_minimum_required(VERSION 3.16)

project(ADEQuiz LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(stations
    station/Stations.cpp
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
