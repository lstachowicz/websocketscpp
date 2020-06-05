cmake_minimum_required(VERSION 3.6)

set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../..)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()
add_subdirectory(${ROOT_DIR}/ext-lib/googletest)

if(CMAKE_GENERATOR STREQUAL "Unix Makefiles")
	set(HAVE_COVERAGE TRUE)
	include(${ROOT_DIR}/websocketscpp/make/CodeCoverage.cmake)
	append_coverage_compiler_flags()
endif()
