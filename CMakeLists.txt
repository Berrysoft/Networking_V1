cmake_minimum_required(VERSION 3.1)
project(Networking)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include_directories(include)

function(add_headers VAR)
  set(headers ${${VAR}})
  foreach (header ${ARGN})
    set(headers ${headers} include/experimental/${header})
  endforeach()
  set(${VAR} ${headers} PARENT_SCOPE)
endfunction()

add_headers(NET_HEADERS net netfwd executor timer io_context buffer socket internet)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/experimental/ NET_SOURCE)
add_library(net SHARED ${NET_SOURCE})
