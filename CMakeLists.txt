cmake_minimum_required(VERSION 3.5)

set (CMAKE_CXX_STANDARD 11)

option(USE_STATIC "Static build" OFF)
option(USE_CLANG "Build with Clang" OFF)


if(USE_CLANG)
  set(CMAKE_CXX_COMPILER clang++)
  set(CMAKE_C_COMPILER clang)
  message("+ Build with Clang++")
endif(USE_CLANG)


project (logfilegen VERSION 3.0.3 LANGUAGES CXX)
add_definitions(-DVERSION_NUMBER="\\"${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}\\"")


set(SOURCES
    utl.cpp
    params.cpp
    pairfile.cpp
    macro.cpp
    vars.cpp
    tpl.cpp
    logrot.cpp
    cycle.cpp
    main.cpp
)


add_executable (logfilegen ${SOURCES})

#set(CMAKE_EXE_LINKER_FLAGS "-pthread -lpthread")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O3 ")


#if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.1.0)
#  target_link_libraries(logfilegen -lstdc++fs)
#endif()


if(USE_STATIC)
  message("+ static build")

if(!USE_CLANG)
  target_link_libraries(logfilegen -static -static-libstdc++ -static-libgcc)
else()
  set(CMAKE_EXE_LINKER_FLAGS "-static")
#  target_link_libraries(logfilegen -static -static-libc++)
endif()
endif()


if(WIN32)
  target_link_libraries(logfilegen  -lws2_32)
endif()


install (TARGETS logfilegen DESTINATION bin)
