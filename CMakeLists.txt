cmake_minimum_required(VERSION 3.5)

set (CMAKE_CXX_STANDARD 17)

project (logfilegen VERSION 0.4.0 LANGUAGES CXX)
add_definitions(-DVERSION_NUMBER="\\"${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}\\"")


set(SOURCES
    utl.cpp
    pairfile.cpp
    tpl.cpp
    logrot.cpp
    cycle.cpp
    main.cpp
)

add_executable (logfilegen ${SOURCES})
