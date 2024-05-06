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

#include <c_lexer/Token.h>

namespace c_lexer {

#define s(x) #x

const char *ttos[TKN_INVALID + 1] = {
    s(TKN_PLUS),  // +
    s(TKN_MINUS), // -
    s(TKN_STAR),  // *
    s(TKN_DIV),   // /
    s(TKN_MOD),   // %
    s(TKN_INCR),  // ++
    s(TKN_DECR),  // --

    s(TKN_EQUALS),           // ==
    s(TKN_NOTEQUALS),        // !=
    s(TKN_GREATER),          // >
    s(TKN_LESS),             // <
    s(TKN_GREATER_OR_EQUAL), // >=
    s(TKN_LESS_OR_EQUAL),    // <=

    s(TKN_LOG_NOT), // !
    s(TKN_LOG_AND), // &&
    s(TKN_LOG_OR),  // ||

    s(TKN_BIT_NOT), // ~
    s(TKN_AMP),     // &
    s(TKN_BIT_OR),  // |
    s(TKN_BIT_XOR), // ^
    s(TKN_LSHIFT),  // <<
    s(TKN_RSHIFT),  // >>

    s(TKN_ASSIGN),        // =
    s(TKN_ADD_ASSIGN),    // +=
    s(TKN_SUB_ASSIGN),    // -=
    s(TKN_MUL_ASSIGN),    // *=
    s(TKN_DIV_ASSIGN),    // /=
    s(TKN_MOD_ASSIGN),    // %=
    s(TKN_AND_ASSIGN),    // &=
    s(TKN_OR_ASSIGN),     // |=
    s(TKN_XOR_ASSIGN),    // ^=
    s(TKN_LSHIFT_ASSIGN), // <<=
    s(TKN_RSHIFT_ASSIGN), // >>=

    s(TKN_ARROW),    // ->
    s(TKN_DOT),      // .
    s(TKN_ELLIPSIS), // ...

    s(TKN_COMMA),    // ,
    s(TKN_QUESTION), // ?
    s(TKN_COLON),    // :
    s(TKN_LPAREN),   // (
    s(TKN_RPAREN),   // )
    s(TKN_LBRACE),   // {
    s(TKN_RBRACE),   // }
    s(TKN_LSQUARE),  // [
    s(TKN_RSQUARE),  // ]
    s(TKN_SEMI),     // ;

    s(TKN_IDENTIFIER),
    s(TKN_INTEGER_LIT), // 123  0xbeef '\n'
    s(TKN_FLOAT_LIT),   // 3.14
    s(TKN_STRING_LIT),  // "abc"

    s(TKN_ALIGNAS),        // alignas (C23)
    s(TKN_ALIGNOF),        // alignof (C23)
    s(TKN_AUTO),           // auto
    s(TKN_BOOL),           // bool (C23)
    s(TKN_BREAK),          // break
    s(TKN_CASE),           // case
    s(TKN_CHAR),           // char
    s(TKN_CONST),          // const
    s(TKN_CONSTEXPR),      // constexpr (C23)
    s(TKN_CONTINUE),       // continue
    s(TKN_DEFAULT),        // default
    s(TKN_DO),             // do
    s(TKN_DOUBLE),         // double
    s(TKN_ELSE),           // else
    s(TKN_ENUM),           // enum
    s(TKN_EXTERN),         // extern
    s(TKN_FALSE),          // false (C23)
    s(TKN_FLOAT),          // float
    s(TKN_FOR),            // for
    s(TKN_GOTO),           // goto
    s(TKN_IF),             // if
    s(TKN_INLINE),         // inline (C99)
    s(TKN_INT),            // int
    s(TKN_LONG),           // long
    s(TKN_NULLPTR),        // nullptr (C23)
    s(TKN_REGISTER),       // register
    s(TKN_RESTRICT),       // restrict (C99)
    s(TKN_RETURN),         // return
    s(TKN_SHORT),          // short
    s(TKN_SIGNED),         // signed
    s(TKN_SIZEOF),         // sizeof
    s(TKN_STATIC),         // static
    s(TKN_STATIC_ASSERT),  // static_assert (C23)
    s(TKN_STRUCT),         // struct
    s(TKN_SWITCH),         // switch
    s(TKN_THREAD_LOCAL),   // thread_local (C23)
    s(TKN_TRUE),           // true (C23)
    s(TKN_TYPEDEF),        // typedef
    s(TKN_TYPEOF),         // typeof (C23)
    s(TKN_TYPEOF_UNQUAL),  // typeof_unqual (C23)
    s(TKN_UNION),          // union
    s(TKN_UNSIGNED),       // unsigned
    s(TKN_VOID),           // void
    s(TKN_VOLATILE),       // volatile
    s(TKN_WHILE),          // while
    s(TKN__ALIGNAS),       // _Alignas (C11)
    s(TKN__ALIGNOF),       // _Alignof (C11)
    s(TKN__ATOMIC),        // _Atomic (C11)
    s(TKN__BITINT),        // _BitInt (C23)
    s(TKN__BOOL),          // _Bool (C99)
    s(TKN__COMPLEX),       // _Complex (C99)
    s(TKN__DECIMAL128),    // _Decimal128 (C23)
    s(TKN__DECIMAL32),     // _Decimal32 (C23)
    s(TKN__DECIMAL64),     // _Decimal64 (C23)
    s(TKN__GENERIC),       // _Generic (C11)
    s(TKN__IMAGINARY),     // _Imaginary (C99)
    s(TKN__NORETURN),      // _Noreturn (C11)
    s(TKN__STATIC_ASSERT), // _Static_assert (C11)
    s(TKN__THREAD_LOCAL),  // _Thread_local (C11)

    s(TKN_EOF),
    s(TKN_INVALID)};

} // namespace c_lexer
