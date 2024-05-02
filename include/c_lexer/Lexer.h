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

#pragma once
#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "Token.h"

class Lexeme {
public:
  Lexeme(std::string &&lexeme, std::uint32_t row, std::uint32_t col)
      : lexeme_(std::move(lexeme)), row_(row), col_(col) {}

  std::string lexeme_;
  std::uint32_t row_;
  std::uint32_t col_;
};

class SourceReader {
public:
  explicit SourceReader(std::istream &input) : input_(input) {}

  char peek() { return input_.peek(); }
  char get() { return input_.get(); }
  void unget(char c) { input_.putback(c); }
  bool eof() const { return input_.eof(); }

protected:
  std::istream &input_;
};

class Lexer {
public:
  typedef std::tuple<enum Token, Lexeme> item_t;

  explicit Lexer(SourceReader *sr);

  item_t peek();
  item_t eat();

protected:
  item_t scan_token();
  bool scan_if(std::string *s);
  bool scan_else(std::string *s);
  bool scan_identifier(std::string *s);
  bool scan_integer(std::string *s);

  SourceReader *sr_;
  std::uint32_t row_;
  std::uint32_t col_;
  std::vector<item_t> lookahead_;
};
