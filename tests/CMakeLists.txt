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

myproj_add_test_lib(
  TARGET
  c_lexer-static
  SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/../libs/c_lexer/Token.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../libs/c_lexer/Lexer.cpp
  CXXSTD
  17)

myproj_add_test(
  TARGET
  test_Token
  SRCS
  test_Token.cpp
  INCS
  ${CMAKE_CURRENT_SOURCE_DIR}/../libs
  LIBS
  c_lexer-static
  CXXSTD
  17)

myproj_add_test(
  TARGET
  test_Lexer
  SRCS
  test_Lexer.cpp
  INCS
  ${CMAKE_CURRENT_SOURCE_DIR}/../libs
  LIBS
  c_lexer-static
  CXXSTD
  17)
