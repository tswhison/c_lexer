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
#include <string>
#include <type_traits>

namespace c_lexer {

enum class Token {
  PLUS,  // +
  MINUS, // -
  STAR,  // *
  DIV,   // /
  MOD,   // %
  INCR,  // ++
  DECR,  // --

  EQUALS,           // ==
  NOTEQUALS,        // !=
  GREATER,          // >
  LESS,             // <
  GREATER_OR_EQUAL, // >=
  LESS_OR_EQUAL,    // <=

  LOG_NOT, // !
  LOG_AND, // &&
  LOG_OR,  // ||

  BIT_NOT, // ~
  AMP,     // &
  BIT_OR,  // |
  BIT_XOR, // ^
  LSHIFT,  // <<
  RSHIFT,  // >>

  ASSIGN,        // =
  ADD_ASSIGN,    // +=
  SUB_ASSIGN,    // -=
  MUL_ASSIGN,    // *=
  DIV_ASSIGN,    // /=
  MOD_ASSIGN,    // %=
  AND_ASSIGN,    // &=
  OR_ASSIGN,     // |=
  XOR_ASSIGN,    // ^=
  LSHIFT_ASSIGN, // <<=
  RSHIFT_ASSIGN, // >>=

  ARROW,    // ->
  DOT,      // .
  ELLIPSIS, // ...

  COMMA,    // ,
  QUESTION, // ?
  COLON,    // :
  LPAREN,   // (
  RPAREN,   // )
  LBRACE,   // {
  RBRACE,   // }
  LSQUARE,  // [
  RSQUARE,  // ]
  SEMI,     // ;

  IDENTIFIER,
  INTEGER_LIT, // 123  0xbeef '\n'
  FLOAT_LIT,   // 3.14
  STRING_LIT,  // "abc"

  ALIGNAS,        // alignas (C23)
  ALIGNOF,        // alignof (C23)
  AUTO,           // auto
  BOOL,           // bool (C23)
  BREAK,          // break
  CASE,           // case
  CHAR,           // char
  CONST,          // const
  CONSTEXPR,      // constexpr (C23)
  CONTINUE,       // continue
  DEFAULT,        // default
  DO,             // do
  DOUBLE,         // double
  ELSE,           // else
  ENUM,           // enum
  EXTERN,         // extern
  FALSE,          // false (C23)
  FLOAT,          // float
  FOR,            // for
  GOTO,           // goto
  IF,             // if
  INLINE,         // inline (C99)
  INT,            // int
  LONG,           // long
  NULLPTR,        // nullptr (C23)
  REGISTER,       // register
  RESTRICT,       // restrict (C99)
  RETURN,         // return
  SHORT,          // short
  SIGNED,         // signed
  SIZEOF,         // sizeof
  STATIC,         // static
  STATIC_ASSERT,  // static_assert (C23)
  STRUCT,         // struct
  SWITCH,         // switch
  THREAD_LOCAL,   // thread_local (C23)
  TRUE,           // true (C23)
  TYPEDEF,        // typedef
  TYPEOF,         // typeof (C23)
  TYPEOF_UNQUAL,  // typeof_unqual (C23)
  UNION,          // union
  UNSIGNED,       // unsigned
  VOID,           // void
  VOLATILE,       // volatile
  WHILE,          // while
  _ALIGNAS,       // _Alignas (C11)
  _ALIGNOF,       // _Alignof (C11)
  _ATOMIC,        // _Atomic (C11)
  _BITINT,        // _BitInt (C23)
  _BOOL,          // _Bool (C99)
  _COMPLEX,       // _Complex (C99)
  _DECIMAL128,    // _Decimal128 (C23)
  _DECIMAL32,     // _Decimal32 (C23)
  _DECIMAL64,     // _Decimal64 (C23)
  _GENERIC,       // _Generic (C11)
  _IMAGINARY,     // _Imaginary (C99)
  _NORETURN,      // _Noreturn (C11)
  _STATIC_ASSERT, // _Static_assert (C11)
  _THREAD_LOCAL,  // _Thread_local (C11)

  END,
  INVALID
};

extern const char
    *ttos[static_cast<std::underlying_type_t<Token>>(Token::INVALID) + 1];

} // namespace c_lexer
