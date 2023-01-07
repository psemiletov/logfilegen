cmake_minimum_required(VERSION 3.5)

set (CMAKE_CXX_STANDARD 11)

project (logfilegen VERSION 0.0.1 LANGUAGES CXX)

set(SOURCES
    libretta_pairfile.cpp
    tpl.cpp
    main.cpp
)

add_executable (logfilegen ${SOURCES})
