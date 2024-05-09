// MIT License
//
// Copyright (c) 2024 Tim Whisonant
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <unistd.h>

#include "tests/tests.h"

int c_lexview_main(int argc, char *argv[]);

TEST(c_lexview, test) {
  int argc = 1;
  char app[] = {'c', '_', 'l', 'e', 'x', 'v', 'i', 'e', 'w', 0};
  char *argv[2] = {app, nullptr};

  std::istringstream iss("int main(void)");
  std::streambuf *sb_save = std::cin.rdbuf();
  std::cin.rdbuf(iss.rdbuf());

  EXPECT_EQ(c_lexview_main(argc, argv), 0);

  std::cin.rdbuf(sb_save);

  char tmpfile[32];
  std::strcpy(tmpfile, "tmptest-XXXXXX.c");
  std::ofstream tmpout(tmpfile);
  tmpout.write("{ return 0; }\n", 14);
  tmpout.close();

  argv[1] = tmpfile;
  argc = 2;
  EXPECT_EQ(c_lexview_main(argc, argv), 0);

  unlink(tmpfile);
}
