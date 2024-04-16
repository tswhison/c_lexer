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

find_program(MYPROJ_GIT_EXECUTABLE git)

if(EXISTS ${MYPROJ_GIT_EXECUTABLE})

  execute_process(
    COMMAND ${MYPROJ_GIT_EXECUTABLE} log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE MYPROJ_GIT_COMMIT_HASH
    RESULT_VARIABLE MYPROJ_GIT_LOG_RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(NOT ${MYPROJ_GIT_LOG_RESULT} EQUAL 0)
    set(MYPROJ_GIT_COMMIT_HASH unknown)
  endif()

  execute_process(
    COMMAND ${MYPROJ_GIT_EXECUTABLE} diff --stat
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE MYPROJ_GIT_DIFF_OUTPUT
    RESULT_VARIABLE MYPROJ_GIT_DIFF_RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(NOT ${MYPROJ_GIT_DIFF_RESULT} EQUAL 0)
    set(MYPROJ_GIT_SRC_TREE_DIRTY 0)
  else()
    if(MYPROJ_GIT_DIFF_OUTPUT)
      set(MYPROJ_GIT_SRC_TREE_DIRTY 1)
    else()
      set(MYPROJ_GIT_SRC_TREE_DIRTY 0)
    endif()
  endif()
else()
  set(MYPROJ_GIT_COMMIT_HASH unknown)
  set(MYPROJ_GIT_SRC_TREE_DIRTY 0)
endif()
