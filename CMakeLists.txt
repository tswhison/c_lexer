# MIT License
#
# Copyright (c) 2024 Tim Whisonant
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.14)
# FIXME: rename the project.
project(myproj C CXX)

# FIXME: global search/replace of MYPROJ.
set(MYPROJ_INCLUDE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "Project include directory")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

set(EXECUTABLE_OUTPUT_PATH
    ${CMAKE_BINARY_DIR}/bin
    CACHE PATH "Exe build directory" FORCE)

set(LIBRARY_OUTPUT_PATH
    ${CMAKE_BINARY_DIR}/lib
    CACHE PATH "Lib build directory" FORCE)

# ##############################################################################
# MyProj version
# ##############################################################################
set(MYPROJ_VERSION_LOCAL
    ""
    CACHE STRING "MyProj local version" FORCE)
set(MYPROJ_VERSION_MAJOR
    0
    CACHE STRING "MyProj major version" FORCE)
set(MYPROJ_VERSION_MINOR
    1
    CACHE STRING "MyProj minor version" FORCE)
set(MYPROJ_VERSION_REVISION
    0${MYPROJ_VERSION_LOCAL}
    CACHE STRING "MyProj revision version" FORCE)
set(MYPROJ_VERSION
    ${MYPROJ_VERSION_MAJOR}.${MYPROJ_VERSION_MINOR}.${MYPROJ_VERSION_REVISION}
    CACHE STRING "MyProj version" FORCE)

get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
if("${LIB64}" STREQUAL "TRUE")
  set(MYPROJ_LIB_INSTALL_DIR
      "lib64"
      CACHE INTERNAL "arch-specific library path")
else()
  set(MYPROJ_LIB_INSTALL_DIR
      "lib"
      CACHE INTERNAL "arch-specific library path")
endif()

option(MYPROJ_INSTALL_RPATH "Enable RPATH creation during install" OFF)
mark_as_advanced(MYPROJ_INSTALL_RPATH)

set(CMAKE_SKIP_BUILD_RPATH FALSE)
if(MYPROJ_INSTALL_RPATH)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${MYPROJ_LIB_INSTALL_DIR}")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

include(MyProjGit)
include(MyProjCompiler)

# ##############################################################################
# install header files
# ##############################################################################
install(DIRECTORY include/myproj DESTINATION include)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/config/config.h.in"
               "${CMAKE_BINARY_DIR}/include/config.h")

option(MYPROJ_BUILD_TESTS "Enable unit test build" OFF)
mark_as_advanced(MYPROJ_BUILD_TESTS)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# ##############################################################################
# googletest
# ##############################################################################
set(GTEST_URL
    https://github.com/google/googletest
    CACHE STRING "URL for googletest")
set(GTEST_VERSION
    1.11.0
    CACHE STRING "Version for googletest")
set(GTEST_TAG
    v${GEST_VERSION}
    CACHE STRING "Tag for googletest")

FetchContent_Declare(
  gtest
  GIT_REPOSITORY ${GTEST_URL}
  GIT_TAG ${GTEST_TAG})

if(MYPROJ_BUILD_TESTS)
  enable_testing()

  if(NOT GTest_FOUND)
    find_package(GTest ${GTEST_VERSION})
  endif()

  if(NOT GTest_FOUND)
    set(GTEST_ROOT
        ${FETCHCONTENT_BASE_DIR}/gtest-src
        CACHE PATH "googletest root directory" FORCE)

    set(BUILD_SHARED_LIBS_ORIG ${BUILD_SHARED_LIBS})
    set(BUILD_SHARED_LIBS ON)

    FetchContent_MakeAvailable(gtest)

    set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_ORIG})

    set(GTEST_INCLUDE_DIR
        ${GTEST_ROOT}/googletest/include
        CACHE PATH "googletest include directory" FORCE)
    set(GTEST_LIBRARY
        ${LIBRARY_OUTPUT_PATH}/libgtest.so
        CACHE PATH "path to googletest library" FORCE)
    set(GTEST_MAIN_LIBRARY
        ${LIBRARY_OUTPUT_PATH}/libgtest_main.so
        CACHE PATH "path to googletest main library" FORCE)
    set(GTEST_LIBRARY_DEBUG
        ${LIBRARY_OUTPUT_PATH}/libgtestd.so
        CACHE PATH "path to (debug) googletest library" FORCE)
    set(GTEST_MAIN_LIBRARY_DEBUG
        ${LIBRARY_OUTPUT_PATH}/libgtest_maind.so
        CACHE PATH "path to (debug) googletest main library" FORCE)
    set(GTest_FOUND
        TRUE
        CACHE BOOL "googletest found by FetchContent" FORCE)

    add_library(gtest_IMPORT SHARED IMPORTED)
    set_property(TARGET gtest_IMPORT PROPERTY IMPORTED_LOCATION
                                              "${GTEST_LIBRARY}")
    add_dependencies(gtest_IMPORT gtest)

    add_library(gtest_MAIN_IMPORT SHARED IMPORTED)
    set_property(TARGET gtest_MAIN_IMPORT PROPERTY IMPORTED_LOCATION
                                                   "${GTEST_MAIN_LIBRARY}")
    add_dependencies(gtest_MAIN_IMPORT gtest)

    add_library(gtest_IMPORT_DEBUG SHARED IMPORTED)
    set_property(TARGET gtest_IMPORT_DEBUG PROPERTY IMPORTED_LOCATION
                                                    "${GTEST_LIBRARY_DEBUG}")
    add_dependencies(gtest_IMPORT_DEBUG gtest)

    add_library(gtest_MAIN_IMPORT_DEBUG SHARED IMPORTED)
    set_property(TARGET gtest_MAIN_IMPORT_DEBUG
                 PROPERTY IMPORTED_LOCATION "${GTEST_MAIN_LIBRARY_DEBUG}")
    add_dependencies(gtest_MAIN_IMPORT_DEBUG gtest)

    set(GTEST_IMPORTED
        TRUE
        CACHE BOOL "googletest imported from source" FORCE)

  endif()

  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(GTEST_LIBRARIES
        ${GTEST_LIBRARY_DEBUG} ${GTEST_MAIN_LIBRARY_DEBUG}
        CACHE STRING "googletest link libraries" FORCE)
  else()
    set(GTEST_LIBRARIES
        ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY}
        CACHE STRING "googletest link libraries" FORCE)
  endif()

  message(STATUS "GTEST_INCLUDE_DIR: ${GTEST_INCLUDE_DIR}")
  message(STATUS "GTEST_LIBRARY: ${GTEST_LIBRARY}")
  message(STATUS "GTEST_MAIN_LIBRARY: ${GTEST_MAIN_LIBRARY}")
  message(STATUS "GTEST_LIBRARY_DEBUG: ${GTEST_LIBRARY_DEBUG}")
  message(STATUS "GTEST_MAIN_LIBRARY_DEBUG: ${GTEST_MAIN_LIBRARY_DEBUG}")

endif()

add_subdirectory(libs)
add_subdirectory(app)
