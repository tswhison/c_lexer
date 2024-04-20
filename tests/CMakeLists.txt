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

myproj_add_test(TARGET example_test SRCS example_test.cpp CXXSTD 11)

# mycapp tests
myproj_add_test_lib(TARGET myarch-static SRCS
                    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/static/arch.c CSTD 11)

myproj_add_test_lib(
  TARGET
  mycapp-static
  SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/../app/main.c
  INCS
  ${CMAKE_CURRENT_SOURCE_DIR}/../libs
  LIBS
  myarch-static
  DEFS
  main=mycapp_main
  CSTD
  11)

myproj_add_test(
  TARGET
  test_myarch
  SRCS
  test_myarch.cpp
  INCS
  ${CMAKE_CURRENT_SOURCE_DIR}/../libs
  LIBS
  myarch-static
  CXXSTD
  17)

myproj_add_test(
  TARGET
  test_mycapp
  SRCS
  test_mycapp.cpp
  LIBS
  myarch-static
  mycapp-static
  CXXSTD
  17)

# mycxxapp tests
myproj_add_test_lib(
  TARGET myslib-static SRCS ${CMAKE_CURRENT_SOURCE_DIR}/../libs/shared/slib.cpp
  CXXSTD 17)

myproj_add_test_lib(
  TARGET mymod-static SRCS ${CMAKE_CURRENT_SOURCE_DIR}/../libs/module/mlib.cpp
  CXXSTD 17)

myproj_add_test_lib(
  TARGET
  mycxxapp-static
  SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/../app/main.cpp
  INCS
  ${CMAKE_CURRENT_SOURCE_DIR}/../libs
  LIBS
  myslib-static
  DEFS
  main=mycxxapp_main
  CXXSTD
  17)

myproj_add_test(
  TARGET
  test_myslib
  SRCS
  test_myslib.cpp
  INCS
  ${CMAKE_CURRENT_SOURCE_DIR}/../libs
  LIBS
  myslib-static
  CXXSTD
  17)

myproj_add_test(
  TARGET
  test_mymod
  SRCS
  test_mymod.cpp
  INCS
  ${CMAKE_CURRENT_SOURCE_DIR}/../libs
  LIBS
  mymod-static
  CXXSTD
  17)

myproj_add_test(
  TARGET
  test_mycxxapp
  SRCS
  test_mycxxapp.cpp
  LIBS
  myslib-static
  mycxxapp-static
  CXXSTD
  17)
