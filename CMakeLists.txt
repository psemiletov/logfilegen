cmake_minimum_required(VERSION 3.5)

set (CMAKE_CXX_STANDARD 11)

project (logfilegen VERSION 0.2.0 LANGUAGES CXX)
add_definitions(-DVERSION_NUMBER="\\"${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}\\"")


set(SOURCES
    libretta_pairfile.cpp
    tpl.cpp
    main.cpp
)

add_executable (logfilegen ${SOURCES})
