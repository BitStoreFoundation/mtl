cmake_minimum_required( VERSION 3.21.0 FATAL_ERROR )

project( test_math LANGUAGES CXX )			

set( SOURCE_EXE tests/test_math.cpp )

#include_directories( tests )

add_executable( test_math ${SOURCE_EXE} )