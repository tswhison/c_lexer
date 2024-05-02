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

#include <cstdio>
#include <utility>

#include <c_lexer/Lexer.h>

Lexer::Lexer(SourceReader *sr) : sr_(sr), row_(1), col_(1) {
  lookahead_.push_back(scan_token());
}

Lexer::item_t Lexer::peek() { return lookahead_[0]; }

Lexer::item_t Lexer::eat() {
  item_t i = lookahead_[0];
  lookahead_[0] = scan_token();
  return i;
}

#define r(_t, _c)                                                              \
  col_ += (_c);                                                                \
  return item_t(_t, Lexeme(std::move(lex), row_, col_ - (_c)))

// C identifier names start with [a-zA-Z_] and continue with [a-zA-Z_0-9]
constexpr bool is_ident_start(char c) { return std::isalpha(c) || c == '_'; }

constexpr bool is_ident_cont(char c) { return std::isalnum(c) || c == '_'; }

#define START 0

#define GOT_GT 1
#define GOT_LT 2

#define GOT_a 3
#define GOT_al 4
#define GOT_ali 5
#define GOT_alig 6
#define GOT_align 7
#define GOT_aligna 8
#define GOT_aligno 9

#define GOT_au 10
#define GOT_aut 11

#define GOT_b 12
#define GOT_bo 13
#define GOT_boo 14

#define GOT_br 15
#define GOT_bre 16
#define GOT_brea 17

#define GOT_c 18
#define GOT_ca 19
#define GOT_cas 20

#define GOT_ch 21
#define GOT_cha 22

#define GOT_co 23
#define GOT_con 24
#define GOT_cons 25

#define GOT_IDENT_START 998
#define GOT_IDENT_CONT 999

#define END 1000

Lexer::item_t Lexer::scan_token() {
  std::string lex;
  int st = START;
  char c;
  bool eat_ws = true;

  while (true) {
    c = sr_->eof() ? EOF : sr_->get();

    if (eat_ws && std::isspace(c)) {
      switch (c) {
      case '\n':
        ++row_;
        col_ = 1;
        break;
      case ' ':
      case '\t':
        ++col_;
        break;
      }
      continue;
    }

    if (c != EOF)
      lex.push_back(c);

    switch (st) {
    case END:
      r(TKN_EOF, 0);

    case START:
      eat_ws = false;
      switch (c) {
      case EOF:
        st = END;
        break;
      case '~':
        r(TKN_BIT_NOT, 1);
      case ',':
        r(TKN_COMMA, 1);
      case '?':
        r(TKN_QUESTION, 1);
      case ':':
        r(TKN_COLON, 1);
      case '(':
        r(TKN_LPAREN, 1);
      case ')':
        r(TKN_RPAREN, 1);
      case '{':
        r(TKN_LBRACE, 1);
      case '}':
        r(TKN_RBRACE, 1);
      case '[':
        r(TKN_LSQUARE, 1);
      case ']':
        r(TKN_RSQUARE, 1);
      case ';':
        r(TKN_SEMI, 1);
      case '%':
        if (sr_->peek() == '=') {
          lex.push_back(sr_->get());
          r(TKN_MOD_ASSIGN, 2);
        } else {
          r(TKN_MOD, 1);
        }
      case '/':
        if (sr_->peek() == '=') {
          lex.push_back(sr_->get());
          r(TKN_DIV_ASSIGN, 2);
        } else {
          r(TKN_DIV, 1);
        }
      case '*':
        if (sr_->peek() == '=') {
          lex.push_back(sr_->get());
          r(TKN_MUL_ASSIGN, 2);
        } else {
          r(TKN_STAR, 1);
        }
      case '=':
        if (sr_->peek() == '=') {
          lex.push_back(sr_->get());
          r(TKN_EQUALS, 2);
        } else {
          r(TKN_ASSIGN, 1);
        }
      case '!':
        if (sr_->peek() == '=') {
          lex.push_back(sr_->get());
          r(TKN_NOTEQUALS, 2);
        } else {
          r(TKN_LOG_NOT, 1);
        }
      case '^':
        if (sr_->peek() == '=') {
          lex.push_back(sr_->get());
          r(TKN_XOR_ASSIGN, 2);
        } else {
          r(TKN_BIT_XOR, 1);
        }
      case '.':
        if (sr_->peek() == '.') {
          lex.push_back(sr_->get());
          if (sr_->peek() == '.') {
            lex.push_back(sr_->get());
            r(TKN_ELLIPSIS, 3);
          }
          lex.pop_back();
          sr_->unget('.');
          r(TKN_DOT, 1);
        } else {
          r(TKN_DOT, 1);
        }
      case '|':
        if (sr_->peek() == '|') {
          lex.push_back(sr_->get());
          r(TKN_LOG_OR, 2);
        } else if (sr_->peek() == '=') {
          lex.push_back(sr_->get());
          r(TKN_OR_ASSIGN, 2);
        } else {
          r(TKN_BIT_OR, 1);
        }
      case '&':
        if (sr_->peek() == '&') {
          lex.push_back(sr_->get());
          r(TKN_LOG_AND, 2);
        } else if (sr_->peek() == '=') {
          lex.push_back(sr_->get());
          r(TKN_AND_ASSIGN, 2);
        } else {
          r(TKN_AMP, 1);
        }
      case '+':
        if (sr_->peek() == '+') {
          lex.push_back(sr_->get());
          r(TKN_INCR, 2);
        } else if (sr_->peek() == '=') {
          lex.push_back(sr_->get());
          r(TKN_ADD_ASSIGN, 2);
        } else {
          r(TKN_PLUS, 1);
        }
      case '-':
        if (sr_->peek() == '-') {
          lex.push_back(sr_->get());
          r(TKN_DECR, 2);
        } else if (sr_->peek() == '=') {
          lex.push_back(sr_->get());
          r(TKN_SUB_ASSIGN, 2);
        } else if (sr_->peek() == '>') {
          lex.push_back(sr_->get());
          r(TKN_ARROW, 2);
        } else {
          r(TKN_MINUS, 1);
        }
      case '>':
        st = GOT_GT;
        break;
      case '<':
        st = GOT_LT;
        break;
      case 'a': // alignas alignof auto
        st = GOT_a;
        break;
      case 'b': // bool break
        st = GOT_b;
        break;
      case 'c': // case char const constexpr continue
        st = GOT_c;
        break;
      case 'd': // default do double
      case 'e': // else enum extern
      case 'f': // false float for
      case 'g': // goto
      case 'i': // if inline int
      case 'l': // long
      case 'n': // nullptr
      case 'r': // register restrict return
      case 's': // short signed sizeof static static_assert struct switch
      case 't': // thread_local true typedef typeof typeof_unqual
      case 'u': // union unsigned
      case 'v': // void volatile
      case 'w': // while
      case '_': // _Alignas _Alignof _Atomic _BitInt _Bool _Complex
                // _Decimal128 _Decimal32 _Decimal64 _Generic _Imaginary
                // _Noreturn _Static_assert _Thread_local
        st = GOT_IDENT_START;
        break;
      } // switch(c) for START
      break;

    case GOT_GT:
      switch (c) {
      case '=':
        r(TKN_GREATER_OR_EQUAL, 2);
      case '>':
        if (sr_->peek() == '=') {
          lex.push_back(sr_->get());
          r(TKN_RSHIFT_ASSIGN, 3);
        } else {
          r(TKN_RSHIFT, 2);
        }
      default:
        sr_->unget(c);
        lex.pop_back();
        r(TKN_GREATER, 1);
      } // switch(c) for GOT_GT
      break;

    case GOT_LT:
      switch (c) {
      case '=':
        r(TKN_LESS_OR_EQUAL, 2);
      case '<':
        if (sr_->peek() == '=') {
          lex.push_back(sr_->get());
          r(TKN_LSHIFT_ASSIGN, 3);
        } else {
          r(TKN_LSHIFT, 2);
        }
      default:
        sr_->unget(c);
        lex.pop_back();
        r(TKN_LESS, 1);
      } // switch(c) for GOT_LT
      break;

    case GOT_a:
      switch (c) {
      case 'l':
        st = GOT_al;
        break;
      case 'u':
        st = GOT_au;
        break;
      default:
        st = GOT_IDENT_START;
        break;
      } // switch(c) for GOT_a
      break;

    case GOT_au:
      switch (c) {
      case 't':
        st = GOT_aut;
        break;
      default:
        st = GOT_IDENT_START;
        break;
      } // switch(c) for GOT_au
      break;

    case GOT_aut:
      switch (c) {
      case 'o':
        r(TKN_AUTO, 4);
      default:
        st = GOT_IDENT_START;
        break;
      } // switch(c) for GOT_aut
      break;

    case GOT_IDENT_START:
      // TODO: complete me
      r(TKN_IDENTIFIER, lex.length());
      break;

    default:
      return item_t(TKN_INVALID, Lexeme("<not implemented yet>", row_, col_));
    }
  }

  /*

  TKN_ALIGNAS,        // alignas (C23)
  TKN_ALIGNOF,        // alignof (C23)
  TKN_AUTO,           // auto
  TKN_BOOL,           // bool (C23)
  TKN_BREAK,          // break
  TKN_CASE,           // case
  TKN_CHAR,           // char
  TKN_CONST,          // const
  TKN_CONSTEXPR,      // constexpr (C23)
  TKN_CONTINUE,       // continue
  TKN_DEFAULT,        // default
  TKN_DO,             // do
  TKN_DOUBLE,         // double
  TKN_ELSE,           // else
  TKN_ENUM,           // enum
  TKN_EXTERN,         // extern
  TKN_FALSE,          // false (C23)
  TKN_FLOAT,          // float
  TKN_FOR,            // for
  TKN_GOTO,           // goto
  TKN_IF,             // if
  TKN_INLINE,         // inline (C99)
  TKN_INT,            // int
  TKN_LONG,           // long
  TKN_NULLPTR,        // nullptr (C23)
  TKN_REGISTER,       // register
  TKN_RESTRICT,       // restrict (C99)
  TKN_RETURN,         // return
  TKN_SHORT,          // short
  TKN_SIGNED,         // signed
  TKN_SIZEOF,         // sizeof
  TKN_STATIC,         // static
  TKN_STATIC_ASSERT,  // static_assert (C23)
  TKN_STRUCT,         // struct
  TKN_SWITCH,         // switch
  TKN_THREAD_LOCAL,   // thread_local (C23)
  TKN_TRUE,           // true (C23)
  TKN_TYPEDEF,        // typedef
  TKN_TYPEOF,         // typeof (C23)
  TKN_TYPEOF_UNQUAL,  // typeof_unqual (C23)
  TKN_UNION,          // union
  TKN_UNSIGNED,       // unsigned
  TKN_VOID,           // void
  TKN_VOLATILE,       // volatile
  TKN_WHILE,          // while
  TKN__ALIGNAS,       // _Alignas (C11)
  TKN__ALIGNOF,       // _Alignof (C11)
  TKN__ATOMIC,        // _Atomic (C11)
  TKN__BITINT,        // _BitInt (C23)
  TKN__BOOL,          // _Bool (C99)
  TKN__COMPLEX,       // _Complex (C99)
  TKN__DECIMAL128,    // _Decimal128 (C23)
  TKN__DECIMAL32,     // _Decimal32 (C23)
  TKN__DECIMAL64,     // _Decimal64 (C23)
  TKN__GENERIC,       // _Generic (C11)
  TKN__IMAGINARY,     // _Imaginary (C99)
  TKN__NORETURN,      // _Noreturn (C11)
  TKN__STATIC_ASSERT, // _Static_assert (C11)
  TKN__THREAD_LOCAL,  // _Thread_local (C11)

  TKN_IDENTIFIER,
  TKN_INTEGER_LIT, // 123  0xbeef
  TKN_FLOAT_LIT,   // 3.14
  TKN_STRING_LIT,  // "abc"
  TKN_CHAR_LIT,    // 'x'

  */

  return item_t(TKN_INVALID, Lexeme("<invalid>", row_, col_));
}
