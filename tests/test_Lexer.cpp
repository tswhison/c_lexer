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

#include "gtest/gtest.h"
#include <sstream>
#include <tuple>
#include <utility>
#include <vector>

#include "tests.h"
#include <c_lexer/Lexer.h>

typedef std::tuple<enum Token, const char *, const char *> test_tup_t;

void basic_token_test(const test_tup_t &tup) {
  const Token tok = std::get<0>(tup);
  const char *tstr = std::get<1>(tup);
  const char *lex = std::get<2>(tup);

  EXPECT_STREQ(tstr, ttos[tok]);

  std::vector<Lexeme> v = scan_tokens(lex);

  ASSERT_EQ(static_cast<std::vector<Lexeme>::size_type>(2), v.size());
  EXPECT_EQ(tok, v[0]);
  EXPECT_STREQ(v[0].lexeme_.c_str(), lex);

  EXPECT_EQ(TKN_EOF, v[1]);
}

class OperatorFixture : public ::testing::TestWithParam<test_tup_t> {};

test_tup_t operators[] = {
    {TKN_PLUS, "TKN_PLUS", "+"},
    {TKN_MINUS, "TKN_MINUS", "-"},
    {TKN_STAR, "TKN_STAR", "*"},
    {TKN_DIV, "TKN_DIV", "/"},
    {TKN_MOD, "TKN_MOD", "%"},
    {TKN_INCR, "TKN_INCR", "++"},
    {TKN_DECR, "TKN_DECR", "--"},
    {TKN_EQUALS, "TKN_EQUALS", "=="},
    {TKN_NOTEQUALS, "TKN_NOTEQUALS", "!="},
    {TKN_GREATER, "TKN_GREATER", ">"},
    {TKN_LESS, "TKN_LESS", "<"},
    {TKN_GREATER_OR_EQUAL, "TKN_GREATER_OR_EQUAL", ">="},
    {TKN_LESS_OR_EQUAL, "TKN_LESS_OR_EQUAL", "<="},
    {TKN_LOG_NOT, "TKN_LOG_NOT", "!"},
    {TKN_LOG_AND, "TKN_LOG_AND", "&&"},
    {TKN_LOG_OR, "TKN_LOG_OR", "||"},
    {TKN_BIT_NOT, "TKN_BIT_NOT", "~"},
    {TKN_AMP, "TKN_AMP", "&"},
    {TKN_BIT_OR, "TKN_BIT_OR", "|"},
    {TKN_BIT_XOR, "TKN_BIT_XOR", "^"},
    {TKN_LSHIFT, "TKN_LSHIFT", "<<"},
    {TKN_RSHIFT, "TKN_RSHIFT", ">>"},
    {TKN_ASSIGN, "TKN_ASSIGN", "="},
    {TKN_ADD_ASSIGN, "TKN_ADD_ASSIGN", "+="},
    {TKN_SUB_ASSIGN, "TKN_SUB_ASSIGN", "-="},
    {TKN_MUL_ASSIGN, "TKN_MUL_ASSIGN", "*="},
    {TKN_DIV_ASSIGN, "TKN_DIV_ASSIGN", "/="},
    {TKN_MOD_ASSIGN, "TKN_MOD_ASSIGN", "%="},
    {TKN_AND_ASSIGN, "TKN_AND_ASSIGN", "&="},
    {TKN_OR_ASSIGN, "TKN_OR_ASSIGN", "|="},
    {TKN_XOR_ASSIGN, "TKN_XOR_ASSIGN", "^="},
    {TKN_LSHIFT_ASSIGN, "TKN_LSHIFT_ASSIGN", "<<="},
    {TKN_RSHIFT_ASSIGN, "TKN_RSHIFT_ASSIGN", ">>="},
};

TEST_P(OperatorFixture, optest) {
  ASSERT_NO_FATAL_FAILURE(basic_token_test(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(my, OperatorFixture, ::testing::ValuesIn(operators));

class SeparatorFixture : public ::testing::TestWithParam<test_tup_t> {};

test_tup_t separators[] = {
    {TKN_ARROW, "TKN_ARROW", "->"},        {TKN_DOT, "TKN_DOT", "."},
    {TKN_ELLIPSIS, "TKN_ELLIPSIS", "..."}, {TKN_COMMA, "TKN_COMMA", ","},
    {TKN_QUESTION, "TKN_QUESTION", "?"},   {TKN_COLON, "TKN_COLON", ":"},
    {TKN_LPAREN, "TKN_LPAREN", "("},       {TKN_RPAREN, "TKN_RPAREN", ")"},
    {TKN_LBRACE, "TKN_LBRACE", "{"},       {TKN_RBRACE, "TKN_RBRACE", "}"},
    {TKN_LSQUARE, "TKN_LSQUARE", "["},     {TKN_RSQUARE, "TKN_RSQUARE", "]"},
    {TKN_SEMI, "TKN_SEMI", ";"},
};

TEST_P(SeparatorFixture, septest) {
  ASSERT_NO_FATAL_FAILURE(basic_token_test(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(my, SeparatorFixture, ::testing::ValuesIn(separators));

class KeywordFixture : public ::testing::TestWithParam<test_tup_t> {};

test_tup_t keywords[] = {
    {TKN_ALIGNAS, "TKN_ALIGNAS", "alignas"},
    {TKN_ALIGNOF, "TKN_ALIGNOF", "alignof"},
    {TKN_AUTO, "TKN_AUTO", "auto"},
    {TKN_BOOL, "TKN_BOOL", "bool"},
    {TKN_BREAK, "TKN_BREAK", "break"},
    {TKN_CASE, "TKN_CASE", "case"},
    {TKN_CHAR, "TKN_CHAR", "char"},
    {TKN_CONST, "TKN_CONST", "const"},
    {TKN_CONSTEXPR, "TKN_CONSTEXPR", "constexpr"},
    {TKN_CONTINUE, "TKN_CONTINUE", "continue"},
    {TKN_DEFAULT, "TKN_DEFAULT", "default"},
    {TKN_DO, "TKN_DO", "do"},
    {TKN_DOUBLE, "TKN_DOUBLE", "double"},
    {TKN_ELSE, "TKN_ELSE", "else"},
    {TKN_ENUM, "TKN_ENUM", "enum"},
    {TKN_EXTERN, "TKN_EXTERN", "extern"},
    {TKN_FALSE, "TKN_FALSE", "false"},
    {TKN_FLOAT, "TKN_FLOAT", "float"},
    {TKN_FOR, "TKN_FOR", "for"},
    {TKN_GOTO, "TKN_GOTO", "goto"},
    {TKN_IF, "TKN_IF", "if"},
    {TKN_INLINE, "TKN_INLINE", "inline"},
    {TKN_INT, "TKN_INT", "int"},
    {TKN_LONG, "TKN_LONG", "long"},
    {TKN_NULLPTR, "TKN_NULLPTR", "nullptr"},
    {TKN_REGISTER, "TKN_REGISTER", "register"},
    {TKN_RESTRICT, "TKN_RESTRICT", "restrict"},
    {TKN_RETURN, "TKN_RETURN", "return"},
    {TKN_SHORT, "TKN_SHORT", "short"},
    {TKN_SIGNED, "TKN_SIGNED", "signed"},
    {TKN_SIZEOF, "TKN_SIZEOF", "sizeof"},
    {TKN_STATIC, "TKN_STATIC", "static"},
    {TKN_STATIC_ASSERT, "TKN_STATIC_ASSERT", "static_assert"},
    {TKN_STRUCT, "TKN_STRUCT", "struct"},
    {TKN_SWITCH, "TKN_SWITCH", "switch"},
    {TKN_THREAD_LOCAL, "TKN_THREAD_LOCAL", "thread_local"},
    {TKN_TRUE, "TKN_TRUE", "true"},
    {TKN_TYPEDEF, "TKN_TYPEDEF", "typedef"},
    {TKN_TYPEOF, "TKN_TYPEOF", "typeof"},
    {TKN_TYPEOF_UNQUAL, "TKN_TYPEOF_UNQUAL", "typeof_unqual"},
    {TKN_UNION, "TKN_UNION", "union"},
    {TKN_UNSIGNED, "TKN_UNSIGNED", "unsigned"},
    {TKN_VOID, "TKN_VOID", "void"},
    {TKN_VOLATILE, "TKN_VOLATILE", "volatile"},
    {TKN_WHILE, "TKN_WHILE", "while"},
    {TKN__ALIGNAS, "TKN__ALIGNAS", "_Alignas"},
    {TKN__ALIGNOF, "TKN__ALIGNOF", "_Alignof"},
    {TKN__ATOMIC, "TKN__ATOMIC", "_Atomic"},
    {TKN__BITINT, "TKN__BITINT", "_BitInt"},
    {TKN__BOOL, "TKN__BOOL", "_Bool"},
    {TKN__COMPLEX, "TKN__COMPLEX", "_Complex"},
    {TKN__DECIMAL128, "TKN__DECIMAL128", "_Decimal128"},
    {TKN__DECIMAL32, "TKN__DECIMAL32", "_Decimal32"},
    {TKN__DECIMAL64, "TKN__DECIMAL64", "_Decimal64"},
    {TKN__GENERIC, "TKN__GENERIC", "_Generic"},
    {TKN__IMAGINARY, "TKN__IMAGINARY", "_Imaginary"},
    {TKN__NORETURN, "TKN__NORETURN", "_Noreturn"},
    {TKN__STATIC_ASSERT, "TKN__STATIC_ASSERT", "_Static_assert"},
    {TKN__THREAD_LOCAL, "TKN__THREAD_LOCAL", "_Thread_local"},
};

TEST_P(KeywordFixture, keytest) {
  ASSERT_NO_FATAL_FAILURE(basic_token_test(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(my, KeywordFixture, ::testing::ValuesIn(keywords));

void empty_token_test(const test_tup_t &tup) {
  const Token tok = std::get<0>(tup);
  const char *tstr = std::get<1>(tup);
  const char *lex = std::get<2>(tup);

  EXPECT_STREQ(tstr, ttos[tok]);

  std::vector<Lexeme> v = scan_tokens(lex);

  ASSERT_EQ(static_cast<std::vector<Lexeme>::size_type>(1), v.size());
  EXPECT_EQ(tok, v[0]);
  EXPECT_STREQ(v[0].lexeme_.c_str(), lex);
}

class EmptyFixture : public ::testing::TestWithParam<test_tup_t> {};

test_tup_t empty[] = {
    {TKN_EOF, "TKN_EOF", ""},
};

TEST_P(EmptyFixture, emptytest) {
  ASSERT_NO_FATAL_FAILURE(empty_token_test(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(my, EmptyFixture, ::testing::ValuesIn(empty));

TEST(Invalid, test) { EXPECT_STREQ(ttos[TKN_INVALID], "TKN_INVALID"); }

class BasicIntegerLiteralFixture : public ::testing::TestWithParam<test_tup_t> {
};

test_tup_t basic_integer[] = {
    {TKN_INTEGER_LIT, "TKN_INTEGER_LIT", "0"},
};

TEST_P(BasicIntegerLiteralFixture, basicinttest) {
  ASSERT_NO_FATAL_FAILURE(basic_token_test(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(my, BasicIntegerLiteralFixture,
                         ::testing::ValuesIn(basic_integer));

void integer_literal_test(const char *src) {
  std::vector<Lexeme> v = scan_tokens(src);

  ASSERT_EQ(static_cast<std::vector<Lexeme>::size_type>(2), v.size());
  EXPECT_EQ(TKN_INTEGER_LIT, v[0]);
  EXPECT_STREQ(v[0].lexeme_.c_str(), src);

  EXPECT_EQ(TKN_EOF, v[1]);
}

class IntegerLiteralFixture : public ::testing::TestWithParam<const char *> {};

const char *integer[] = {
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "0123456789",
    "123456789",

    "0xbeef",
    "0xau",
    "0xaul",
    "0xauL",
    "0xaull",
    "0xauLL",
    "0xaU",
    "0xaUl",
    "0xaUL",
    "0xaUll",
    "0xaULL",
    "0xal",
    "0xalu",
    "0xalU",
    "0xaL",
    "0xaLu",
    "0xaLU",
    "0xall",
    "0xallu",
    "0xallU",
    "0xaLL",
    "0xaLLu",
    "0xaLLU",

    "0u",
    "0ul",
    "0uL",
    "0ull",
    "0uLL",
    "0U",
    "0Ul",
    "0UL",
    "0Ull",
    "0ULL",
    "0l",
    "0lu",
    "0lU",
    "0L",
    "0Lu",
    "0LU",
    "0ll",
    "0llu",
    "0llU",
    "0LL",
    "0LLu",
    "0LLU",
    "1u",
    "1ul",
    "1uL",
    "1ull",
    "1uLL",
    "1U",
    "1Ul",
    "1UL",
    "1Ull",
    "1ULL",
    "1l",
    "1lu",
    "1lU",
    "1L",
    "1Lu",
    "1LU",
    "1ll",
    "1llu",
    "1llU",
    "1LL",
    "1LLu",
    "1LLU",

    "0777",
    "0777u",
    "0777ul",
    "0777uL",
    "0777ull",
    "0777uLL",
    "0777U",
    "0777Ul",
    "0777UL",
    "0777Ull",
    "0777ULL",
    "0777l",
    "0777lu",
    "0777lU",
    "0777L",
    "0777Lu",
    "0777LU",
    "0777ll",
    "0777llu",
    "0777llU",
    "0777LL",
    "0777LLu",
    "0777LLU",

    "\'a\'",
    "u\'a\'",
    "u8\'a\'",
    "U\'a\'",
    "L\'a\'",
    "\'\\\'\'",
    "\'\\\"\'",
    "\'\\?\'",
    "'\\\\\'",
    "\'\\a\'",
    "\'\\b\'",
    "\'\\f\'",
    "\'\\n\'",
    "\'\\r\'",
    "\'\\t\'",
    "\'\\v\'",
    "u\'\\a\'",
    "u8\'\\b\'",
    "U\'\\f\'",
    "L\'\\n\'",
    "u\'\\r\'",
    "u8\'\\t\'",
    "U\'\\v\'",
    "\'\\7\'",
    "u\'\\77\'",
    "U\'\\777\'",
    "L\'\\777a\'",
    "\'\\xa\'",
    "u\'\\xab\'",
    "u8\'\\xabc\'",
    "U\'\\xabcd\'",
    "\' \'",
};

TEST_P(IntegerLiteralFixture, inttest) {
  ASSERT_NO_FATAL_FAILURE(integer_literal_test(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(my, IntegerLiteralFixture,
                         ::testing::ValuesIn(integer));

void integer_ident_test(const char *src) {
  std::vector<Lexeme> v = scan_tokens(src);

  ASSERT_EQ(static_cast<std::vector<Lexeme>::size_type>(3), v.size());
  EXPECT_EQ(TKN_INTEGER_LIT, v[0]);
  EXPECT_EQ(TKN_IDENTIFIER, v[1]);
  EXPECT_EQ(TKN_EOF, v[2]);
}

class IntegerIdentFixture : public ::testing::TestWithParam<const char *> {};

const char *integer_ident[] = {
    "0xax",    "0xaux",   "0xaulx", "0xauLx",  "0xaullx", "0xauLLx",
    "0xaUx",   "0xaUlx",  "0xaULx", "0xaUllx", "0xaULLx", "0xalx",
    "0xalux",  "0xalUx",  "0xaLx",  "0xaLux",  "0xaLUx",  "0xallx",
    "0xallux", "0xallUx", "0xaLLx", "0xaLLux", "0xaLLUx",
};

TEST_P(IntegerIdentFixture, intidenttest) {
  ASSERT_NO_FATAL_FAILURE(integer_ident_test(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(my, IntegerIdentFixture,
                         ::testing::ValuesIn(integer_ident));
