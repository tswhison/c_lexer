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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <dlfcn.h>

#include <iostream>

#include "module/mlib.h"
#include "shared/slib.h"

void *dlhandle;

SomeInterface *LoadModuleObject(const char *lib) {
  dlhandle = dlopen(lib, RTLD_LAZY | RTLD_LOCAL);
  if (!dlhandle)
    return nullptr;

  void *fn = dlsym(dlhandle, "getObject");
  if (!fn)
    return nullptr;

  SomeInterface *(*getobj)() = reinterpret_cast<SomeInterface *(*)()>(fn);

  return getobj ? getobj() : nullptr;
}

void UnloadModule(SomeInterface *p) {
  if (p)
    delete p;
  if (dlhandle) {
    dlclose(dlhandle);
    dlhandle = nullptr;
  }
}

int main(int argc, char *argv[]) {
  UNUSED_PARAM(argc);
  UNUSED_PARAM(argv);

  int res = 0;

  std::cout << "Hello, world!" << std::endl;

  SomeClass s;

  SomeInterface *i = LoadModuleObject("libmymod.so");
  if (i && i->method("Slim Shady") != 42) {
    std::cout << "Whoops, didn't get 42." << std::endl;
    res = 1;
  }

  UnloadModule(i);

  return res;
}
