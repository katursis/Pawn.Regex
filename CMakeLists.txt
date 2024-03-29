cmake_minimum_required(VERSION 3.10)

project(pawnregex)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-DHAVE_STDINT_H)
endif()

include_directories(lib)

add_library(${PROJECT_NAME} MODULE
  src/main.h
  src/main.cc
  src/plugin.h
  src/plugin.cc
  src/native_param.h
  src/script.h
  src/script.cc
  src/Pawn.Regex.inc
  plugin.def
)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

if(CMAKE_COMPILER_IS_GNUCC)
  set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE Off)
  set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY COMPILE_FLAGS "-m32 -O3 -w")
  set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS "-m32 -O3 -static-libgcc -static-libstdc++")
endif()
