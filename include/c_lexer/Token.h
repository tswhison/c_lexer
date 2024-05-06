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

namespace c_lexer {

enum Token {
  TKN_PLUS,  // +
  TKN_MINUS, // -
  TKN_STAR,  // *
  TKN_DIV,   // /
  TKN_MOD,   // %
  TKN_INCR,  // ++
  TKN_DECR,  // --

  TKN_EQUALS,           // ==
  TKN_NOTEQUALS,        // !=
  TKN_GREATER,          // >
  TKN_LESS,             // <
  TKN_GREATER_OR_EQUAL, // >=
  TKN_LESS_OR_EQUAL,    // <=

  TKN_LOG_NOT, // !
  TKN_LOG_AND, // &&
  TKN_LOG_OR,  // ||

  TKN_BIT_NOT, // ~
  TKN_AMP,     // &
  TKN_BIT_OR,  // |
  TKN_BIT_XOR, // ^
  TKN_LSHIFT,  // <<
  TKN_RSHIFT,  // >>

  TKN_ASSIGN,        // =
  TKN_ADD_ASSIGN,    // +=
  TKN_SUB_ASSIGN,    // -=
  TKN_MUL_ASSIGN,    // *=
  TKN_DIV_ASSIGN,    // /=
  TKN_MOD_ASSIGN,    // %=
  TKN_AND_ASSIGN,    // &=
  TKN_OR_ASSIGN,     // |=
  TKN_XOR_ASSIGN,    // ^=
  TKN_LSHIFT_ASSIGN, // <<=
  TKN_RSHIFT_ASSIGN, // >>=

  TKN_ARROW,    // ->
  TKN_DOT,      // .
  TKN_ELLIPSIS, // ...

  TKN_COMMA,    // ,
  TKN_QUESTION, // ?
  TKN_COLON,    // :
  TKN_LPAREN,   // (
  TKN_RPAREN,   // )
  TKN_LBRACE,   // {
  TKN_RBRACE,   // }
  TKN_LSQUARE,  // [
  TKN_RSQUARE,  // ]
  TKN_SEMI,     // ;

  TKN_IDENTIFIER,
  TKN_INTEGER_LIT, // 123  0xbeef '\n'
  TKN_FLOAT_LIT,   // 3.14
  TKN_STRING_LIT,  // "abc"

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

  TKN_EOF,
  TKN_INVALID
};

extern const char *ttos[TKN_INVALID + 1];

} // namespace c_lexer
