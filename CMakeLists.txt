cmake_minimum_required(VERSION 3.5)

set (CMAKE_CXX_STANDARD 17)

option(USE_PROMCPP "Use prometheus_cpp" OFF)
option(USE_STATIC "Static build" OFF)
option(USE_CLANG "Build with Clang" OFF)
option(USE_OPENSUSE "Build with OpenSUSE" OFF)


function( set_required_build_settings_for_GCC8 )
    # Always link with libstdc++fs.a when using GCC 8.
    # Note: This command makes sure that this option comes pretty late on the cmdline.
    link_libraries( "$<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:-lstdc++fs>" )
endfunction()


if(USE_CLANG)
  set(CMAKE_CXX_COMPILER clang++)
  set(CMAKE_C_COMPILER clang)
  message("+ Build with Clang++")
endif(USE_CLANG)



project (logfilegen VERSION 2.0.0 LANGUAGES CXX)
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



#if(USE_OPENSUSE)
#  target_link_libraries(logfilegen -lstdc++fs)
#   add_definitions(-DUSE_OPENSUSE=1)
#endif(USE_OPENSUSE)


if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.1.0)
  target_link_libraries(logfilegen -lstdc++fs)
endif()



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


set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/local/lib/pkgconfig")

if(USE_PROMCPP)

find_package(PkgConfig REQUIRED)

pkg_check_modules(prometheus-cpp-core QUIET prometheus-cpp-core)
if(prometheus-cpp-core_FOUND)
  add_definitions(-DPROM=1)
  target_link_libraries(logfilegen ${prometheus-cpp-core_LIBRARIES})
  include_directories(${prometheus-cpp-core_INCLUDE_DIRS})
  message("+ prom core support")
endif()

pkg_check_modules(prometheus-cpp-pull QUIET prometheus-cpp-pull)
if(prometheus-cpp-core_FOUND)
  add_definitions(-DPROM=1)
  target_link_libraries(logfilegen ${prometheus-cpp-pull_LIBRARIES} )
  include_directories(${prometheus-cpp-pull_INCLUDE_DIRS})
  message("+ prom pull support")
endif()

else()

 target_link_libraries(logfilegen ${prometheus-cpp-pull_LIBRARIES})


endif()


#find_package(prometheus-cpp CONFIG)

#if(prometheus-cpp_FOUND)

 # message("+ prometheus-cpp support")

 # add_definitions(-DUSE_PROM=1)


 # target_link_libraries(logfilegen PRIVATE prometheus-cpp::pull)
 # target_link_libraries(logfilegen ${prometheus-cpp-core_LIBRARIES} )
 # include_directories(${prometheus-cpp-core_INCLUDE_DIRS})

  # target_link_libraries(logfilegen PRIVATE prometheus-cpp::pull)

#endif()


install (TARGETS logfilegen DESTINATION bin)
