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

#include <c_lexer/Token.h>

#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace c_lexer {

class Lexeme {
public:
  Lexeme(std::string &&lexeme, Token token, std::uint32_t row,
         std::uint32_t col)
      : lexeme_(std::move(lexeme)), token_(token), row_(row), col_(col) {}

  Lexeme(const Lexeme &) = default;
  Lexeme(Lexeme &&) noexcept = default;
  Lexeme &operator=(Lexeme &&) noexcept = default;
  ~Lexeme() = default;

  Token token() const { return token_; }
  operator Token() const { return token_; }
  const char *token_str() const {
    return ttos[static_cast<std::underlying_type_t<Token>>(token_)];
  }

  std::string lexeme_;
  Token token_;
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
  explicit Lexer(std::unique_ptr<SourceReader> &&sr);
  ~Lexer() = default;

  const Lexeme &peek() const;
  Lexeme eat();
  void preload(std::size_t);

  template <typename S, typename... Args>
  S &print_error(S &os, Args &&...args) {
    return printer(os, "c_lexer[", row_, ',', col_, "]: ", args...);
  }

protected:
  Lexeme scan_token();

  template <typename S, typename T0, typename... Ts>
  S &printer(S &os, T0 &&t0, Ts &&...ts) {
    os << std::forward<T0>(t0);

    if constexpr (sizeof...(ts))
      printer(os, std::forward<Ts>(ts)...);

    return os;
  }

  std::unique_ptr<SourceReader> sr_;
  std::uint32_t row_;
  std::uint32_t col_;
  std::queue<Lexeme> lookahead_;
};

std::vector<Lexeme> scan_tokens(const char *s);

} // namespace c_lexer
