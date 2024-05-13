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

#include <c_lexer/Lexer.h>

using c_lexer::Lexeme;
using c_lexer::Lexer;
using c_lexer::SourceReader;
using c_lexer::Token;

#include <algorithm>
#include <fstream>
#include <memory>
#include <unordered_map>
#include <vector>

int main(int argc, char *argv[]) {
  std::ifstream f;
  if (argc > 1) {
    f.open(argv[1]);
  }

  std::istream &in = f.is_open() ? f : std::cin;

  std::unique_ptr<SourceReader> reader = std::make_unique<SourceReader>(in);

  Lexer lexer(std::move(reader));
  lexer.preload(3);

  std::unordered_map<const char *, std::uint32_t> counts;

  while (lexer.peek() != Token::END) {
    Lexeme lexeme = lexer.eat();

    if (auto iter = counts.find(lexeme.token_str()); iter != counts.end()) {
      iter->second++;
    } else {
      counts.insert({lexeme.token_str(), 1});
    }

    std::cout << lexeme.lexeme_ << " (" << lexeme.row_ << "," << lexeme.col_
              << ") : Token::" << lexeme.token_str() << '\n';
  }

  if (f.is_open())
    f.close();

  std::vector<std::pair<const char *, std::uint32_t>> v(counts.begin(),
                                                        counts.end());
  std::sort(v.begin(), v.end(),
            [](auto &left, auto &right) { return left.second > right.second; });

  std::cout << '\n';
  for (const auto &pair : v) {
    std::cout << "Token::" << pair.first << ' ' << pair.second << '\n';
  }

  return 0;
}
