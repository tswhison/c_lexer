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

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

if(NOT CMAKE_C_COMPILER)
  message(FATAL_ERROR "No C compiler found.")
endif()

if(NOT CMAKE_CXX_COMPILER)
  message(FATAL_ERROR "No C++ compiler found.")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE BOOL "Create compile commands for modern editors." FORCE)

set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads)

# ##############################################################################
# Set default CMAKE_BUILD_TYPE when not specified.
# ##############################################################################
if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE
      "RelWithDebInfo"
      CACHE STRING "Set default build type." FORCE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Coverage")
  find_program(MYPROJ_GCOV_EXECUTABLE gcov)
  if(NOT MYPROJ_GCOV_EXECUTABLE)
    message(FATAL_ERROR "Coverage requested, but gcov not found.")
  endif()
  find_program(MYPROJ_LCOV_EXECUTABLE lcov)
  find_program(MYPROJ_GENHTML_EXECUTABLE genhtml)
  set(GCOV_CFLAGS
      "--coverage -fprofile-arcs -ftest-coverage"
      CACHE STRING "gcov CFLAGS" FORCE)
  set(GCOV_LIBRARY
      "gcov"
      CACHE STRING "gcov library" FORCE)
endif()

set(MYPROJ_EXTRA_CFLAGS "-fPIC -Wall -Wextra -Werror -pthread")
set(MYPROJ_EXTRA_CXXFLAGS "-fPIC -Wall -Wextra -Werror")

if(CMAKE_C_FLAGS_DEBUG STREQUAL "-g")
  set(CMAKE_C_FLAGS_DEBUG "-ggdb3")
endif()

foreach(_cflag CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
               CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS_MINSIZEREL)
  string(REGEX MATCH "${MYPROJ_EXTRA_CFLAGS}" FOUND_EXTRA_CFLAGS ${${_cflag}})
  if(NOT FOUND_EXTRA_CFLAGS)
    set(${_cflag}
        "${${_cflag}} ${MYPROJ_EXTRA_CFLAGS}"
        CACHE STRING "With strict warnings" FORCE)
  endif()
endforeach()

if(CMAKE_CXX_FLAGS_DEBUG STREQUAL "-g")
  set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3")
endif()

foreach(_cxxflag CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                 CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_MINSIZEREL)
  string(REGEX MATCH "${MYPROJ_EXTRA_CXXFLAGS}" FOUND_EXTRA_CXXFLAGS
               ${${_cxxflag}})
  if(NOT FOUND_EXTRA_CXXFLAGS)
    set(${_cxxflag}
        "${${_cxxflag}} ${MYPROJ_EXTRA_CXXFLAGS}"
        CACHE STRING "With strict warnings" FORCE)
  endif()
endforeach()

# TODO: need -pie ?

macro(set_c_standard target which extensions)
  set_property(TARGET ${target} PROPERTY C_EXTENSIONS ${extensions})
  set_property(TARGET ${target} PROPERTY C_STANDARD_REQUIRED ON)

  foreach(_cstd "89" "c89" "1989" "90" "c90" "1990")
    if(${_cstd} STREQUAL ${which})
      set_property(TARGET ${target} PROPERTY C_STANDARD 90)
    endif()
  endforeach()

  foreach(_cstd "99" "c99" "1999")
    if(${_cstd} STREQUAL ${which})
      set_property(TARGET ${target} PROPERTY C_STANDARD 99)
    endif()
  endforeach()

  foreach(_cstd "11" "c11" "2011")
    if(${_cstd} STREQUAL ${which})
      set_property(TARGET ${target} PROPERTY C_STANDARD 11)
    endif()
  endforeach()

  foreach(_cstd "17" "c17" "2017")
    if(${_cstd} STREQUAL ${which})
      set_property(TARGET ${target} PROPERTY C_STANDARD 17)
    endif()
  endforeach()

  foreach(_cstd "23" "c23" "2023")
    if(${_cstd} STREQUAL ${which})
      set_property(TARGET ${target} PROPERTY C_STANDARD 23)
    endif()
  endforeach()
endmacro()

macro(set_cxx_standard target which extensions)
  set_property(TARGET ${target} PROPERTY CXX_EXTENSIONS ${extensions})
  set_property(TARGET ${target} PROPERTY CXX_STANDARD_REQUIRED ON)

  foreach(_cxxstd "98" "c++98" "1998")
    if(${_cxxstd} STREQUAL ${which})
      set_property(TARGET ${target} PROPERTY CXX_STANDARD 98)
    endif()
  endforeach()

  foreach(_cxxstd "11" "c++11" "2011")
    if(${_cxxstd} STREQUAL ${which})
      set_property(TARGET ${target} PROPERTY CXX_STANDARD 11)
    endif()
  endforeach()

  foreach(_cxxstd "14" "c++14" "2014")
    if(${_cxxstd} STREQUAL ${which})
      set_property(TARGET ${target} PROPERTY CXX_STANDARD 14)
    endif()
  endforeach()

  foreach(_cxxstd "17" "c++17" "2017")
    if(${_cxxstd} STREQUAL ${which})
      set_property(TARGET ${target} PROPERTY CXX_STANDARD 17)
    endif()
  endforeach()

  foreach(_cxxstd "20" "c++20" "2020")
    if(${_cxxstd} STREQUAL ${which})
      set_property(TARGET ${target} PROPERTY CXX_STANDARD 20)
    endif()
  endforeach()

  foreach(_cxxstd "23" "c++23" "2023")
    if(${_cxxstd} STREQUAL ${which})
      set_property(TARGET ${target} PROPERTY CXX_STANDARD 23)
    endif()
  endforeach()

  foreach(_cxxstd "26" "c++26" "2026")
    if(${_cxxstd} STREQUAL ${which})
      set_property(TARGET ${target} PROPERTY CXX_STANDARD 26)
    endif()
  endforeach()

endmacro()

function(myproj_add_exe)
  set(options CEXTENSIONS CXXEXTENSIONS)
  set(oneValueArgs TARGET COMPONENT DESTINATION EXPORT CSTD CXXSTD)
  set(multiValueArgs SRC LIBS INCS)
  cmake_parse_arguments(MYPROJ_ADD_EXE "${options}" "${oneValueArgs}"
                        "${multiValueArgs}" ${ARGN})

  add_executable(${MYPROJ_ADD_EXE_TARGET} ${MYPROJ_ADD_EXE_SRC})

  target_include_directories(
    ${MYPROJ_ADD_EXE_TARGET}
    PUBLIC $<BUILD_INTERFACE:${MYPROJ_INCLUDE_PATH}>
           $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

  if(MYPROJ_ADD_EXE_INCS)
    target_include_directories(${MYPROJ_ADD_EXE_TARGET}
                               PUBLIC ${MYPROJ_ADD_EXE_INCS})
  endif()

  target_compile_definitions(${MYPROJ_ADD_EXE_TARGET} PRIVATE HAVE_CONFIG_H=1
                                                              PIC=1)

  target_link_libraries(${MYPROJ_ADD_EXE_TARGET} ${MYPROJ_ADD_EXE_LIBS})

  if(MYPROJ_ADD_EXE_CEXTENSIONS)
    set(exts ON)
  else()
    set(exts OFF)
  endif()
  if(MYPROJ_ADD_EXE_CSTD)
    set_c_standard(${MYPROJ_ADD_EXE_TARGET} ${MYPROJ_ADD_EXE_CSTD} ${exts})
  endif()

  if(MYPROJ_ADD_EXE_CXXEXTENSIONS)
    set(exts ON)
  else()
    set(exts OFF)
  endif()
  if(MYPROJ_ADD_EXE_CXXSTD)
    set_cxx_standard(${MYPROJ_ADD_EXE_TARGET} ${MYPROJ_ADD_EXE_CXXSTD} ${exts})
  endif()

  if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
    set_property(
      SOURCE ${MYPROJ_ADD_EXE_SRC}
      APPEND_STRING
      PROPERTY COMPILE_FLAGS "${GCOV_CFLAGS}")
    target_link_libraries(${MYPROJ_ADD_EXE_TARGET} "-l${GCOV_LIBRARY}")
  endif()

  if(MYPROJ_INSTALL_RPATH)
    set_target_properties(
      ${MYPROJ_ADD_EXE_TARGET}
      PROPERTIES INSTALL_RPATH "\$ORIGIN/../${MYPROJ_LIB_INSTALL_DIR}"
                 INSTALL_RPATH_USE_LINK_PATH TRUE
                 SKIP_BUILD_RPATH FALSE
                 BUILD_WITH_INSTALL_RPATH FALSE)
  endif()

  if(MYPROJ_ADD_EXE_COMPONENT)
    if(MYPROJ_ADD_EXE_DESTINATION)
      set(dest ${MYPROJ_ADD_EXE_DESTINATION})
    else()
      set(dest bin)
    endif()

    if(MYPROJ_ADD_EXE_EXPORT)
      install(
        TARGETS ${MYPROJ_ADD_EXE_TARGET}
        EXPORT ${MYPROJ_ADD_EXE_EXPORT}
        RUNTIME DESTINATION ${dest} COMPONENT ${MYPROJ_ADD_EXE_COMPONENT})
    else()
      install(TARGETS ${MYPROJ_ADD_EXE_TARGET}
              RUNTIME DESTINATION ${dest} COMPONENT ${MYPROJ_ADD_EXE_COMPONENT})
    endif()
  endif()

endfunction()
