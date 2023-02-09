cmake_minimum_required(VERSION 3.5)

set (CMAKE_CXX_STANDARD 17)

project (logfilegen VERSION 1.2.0 LANGUAGES CXX)
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

set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/local/lib/pkgconfig")

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
  target_link_libraries(logfilegen ${prometheus-cpp-pull_LIBRARIES})
  include_directories(${prometheus-cpp-pull_INCLUDE_DIRS})
  message("+ prom pull support")
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
