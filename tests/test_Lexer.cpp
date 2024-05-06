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
#include <c_lexer/Token.h>

using c_lexer::Lexeme;
using c_lexer::Lexer;
using c_lexer::scan_tokens;
using c_lexer::SourceReader;
using c_lexer::Token;
using c_lexer::ttos;

#include "tests/tests.h"

#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

using test_tup_t = std::tuple<Token, const char *, const char *>;

void basic_token_test(const test_tup_t &tup) {
  const Token tok = std::get<0>(tup);
  const char *tstr = std::get<1>(tup);
  const char *lex = std::get<2>(tup);

  EXPECT_STREQ(tstr, ttos[EnumToUnsigned(tok)]);

  std::vector<Lexeme> v = scan_tokens(lex);

  ASSERT_EQ(static_cast<std::vector<Lexeme>::size_type>(2), v.size());
  EXPECT_EQ(tok, v[0]);
  EXPECT_STREQ(v[0].lexeme_.c_str(), lex);

  EXPECT_EQ(Token::END, v[1]);
}

class OperatorFixture : public ::testing::TestWithParam<test_tup_t> {};

test_tup_t operators[] = {
    {Token::PLUS, "PLUS", "+"},
    {Token::MINUS, "MINUS", "-"},
    {Token::STAR, "STAR", "*"},
    {Token::DIV, "DIV", "/"},
    {Token::MOD, "MOD", "%"},
    {Token::INCR, "INCR", "++"},
    {Token::DECR, "DECR", "--"},
    {Token::EQUALS, "EQUALS", "=="},
    {Token::NOTEQUALS, "NOTEQUALS", "!="},
    {Token::GREATER, "GREATER", ">"},
    {Token::LESS, "LESS", "<"},
    {Token::GREATER_OR_EQUAL, "GREATER_OR_EQUAL", ">="},
    {Token::LESS_OR_EQUAL, "LESS_OR_EQUAL", "<="},
    {Token::LOG_NOT, "LOG_NOT", "!"},
    {Token::LOG_AND, "LOG_AND", "&&"},
    {Token::LOG_OR, "LOG_OR", "||"},
    {Token::BIT_NOT, "BIT_NOT", "~"},
    {Token::AMP, "AMP", "&"},
    {Token::BIT_OR, "BIT_OR", "|"},
    {Token::BIT_XOR, "BIT_XOR", "^"},
    {Token::LSHIFT, "LSHIFT", "<<"},
    {Token::RSHIFT, "RSHIFT", ">>"},
    {Token::ASSIGN, "ASSIGN", "="},
    {Token::ADD_ASSIGN, "ADD_ASSIGN", "+="},
    {Token::SUB_ASSIGN, "SUB_ASSIGN", "-="},
    {Token::MUL_ASSIGN, "MUL_ASSIGN", "*="},
    {Token::DIV_ASSIGN, "DIV_ASSIGN", "/="},
    {Token::MOD_ASSIGN, "MOD_ASSIGN", "%="},
    {Token::AND_ASSIGN, "AND_ASSIGN", "&="},
    {Token::OR_ASSIGN, "OR_ASSIGN", "|="},
    {Token::XOR_ASSIGN, "XOR_ASSIGN", "^="},
    {Token::LSHIFT_ASSIGN, "LSHIFT_ASSIGN", "<<="},
    {Token::RSHIFT_ASSIGN, "RSHIFT_ASSIGN", ">>="},
};

TEST_P(OperatorFixture, optest) {
  ASSERT_NO_FATAL_FAILURE(basic_token_test(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(my, OperatorFixture, ::testing::ValuesIn(operators));

class SeparatorFixture : public ::testing::TestWithParam<test_tup_t> {};

test_tup_t separators[] = {
    {Token::ARROW, "ARROW", "->"},        {Token::DOT, "DOT", "."},
    {Token::ELLIPSIS, "ELLIPSIS", "..."}, {Token::COMMA, "COMMA", ","},
    {Token::QUESTION, "QUESTION", "?"},   {Token::COLON, "COLON", ":"},
    {Token::LPAREN, "LPAREN", "("},       {Token::RPAREN, "RPAREN", ")"},
    {Token::LBRACE, "LBRACE", "{"},       {Token::RBRACE, "RBRACE", "}"},
    {Token::LSQUARE, "LSQUARE", "["},     {Token::RSQUARE, "RSQUARE", "]"},
    {Token::SEMI, "SEMI", ";"},
};

TEST_P(SeparatorFixture, septest) {
  ASSERT_NO_FATAL_FAILURE(basic_token_test(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(my, SeparatorFixture, ::testing::ValuesIn(separators));

class KeywordFixture : public ::testing::TestWithParam<test_tup_t> {};

test_tup_t keywords[] = {
    {Token::ALIGNAS, "ALIGNAS", "alignas"},
    {Token::ALIGNOF, "ALIGNOF", "alignof"},
    {Token::AUTO, "AUTO", "auto"},
    {Token::BOOL, "BOOL", "bool"},
    {Token::BREAK, "BREAK", "break"},
    {Token::CASE, "CASE", "case"},
    {Token::CHAR, "CHAR", "char"},
    {Token::CONST, "CONST", "const"},
    {Token::CONSTEXPR, "CONSTEXPR", "constexpr"},
    {Token::CONTINUE, "CONTINUE", "continue"},
    {Token::DEFAULT, "DEFAULT", "default"},
    {Token::DO, "DO", "do"},
    {Token::DOUBLE, "DOUBLE", "double"},
    {Token::ELSE, "ELSE", "else"},
    {Token::ENUM, "ENUM", "enum"},
    {Token::EXTERN, "EXTERN", "extern"},
    {Token::FALSE, "FALSE", "false"},
    {Token::FLOAT, "FLOAT", "float"},
    {Token::FOR, "FOR", "for"},
    {Token::GOTO, "GOTO", "goto"},
    {Token::IF, "IF", "if"},
    {Token::INLINE, "INLINE", "inline"},
    {Token::INT, "INT", "int"},
    {Token::LONG, "LONG", "long"},
    {Token::NULLPTR, "NULLPTR", "nullptr"},
    {Token::REGISTER, "REGISTER", "register"},
    {Token::RESTRICT, "RESTRICT", "restrict"},
    {Token::RETURN, "RETURN", "return"},
    {Token::SHORT, "SHORT", "short"},
    {Token::SIGNED, "SIGNED", "signed"},
    {Token::SIZEOF, "SIZEOF", "sizeof"},
    {Token::STATIC, "STATIC", "static"},
    {Token::STATIC_ASSERT, "STATIC_ASSERT", "static_assert"},
    {Token::STRUCT, "STRUCT", "struct"},
    {Token::SWITCH, "SWITCH", "switch"},
    {Token::THREAD_LOCAL, "THREAD_LOCAL", "thread_local"},
    {Token::TRUE, "TRUE", "true"},
    {Token::TYPEDEF, "TYPEDEF", "typedef"},
    {Token::TYPEOF, "TYPEOF", "typeof"},
    {Token::TYPEOF_UNQUAL, "TYPEOF_UNQUAL", "typeof_unqual"},
    {Token::UNION, "UNION", "union"},
    {Token::UNSIGNED, "UNSIGNED", "unsigned"},
    {Token::VOID, "VOID", "void"},
    {Token::VOLATILE, "VOLATILE", "volatile"},
    {Token::WHILE, "WHILE", "while"},
    {Token::_ALIGNAS, "_ALIGNAS", "_Alignas"},
    {Token::_ALIGNOF, "_ALIGNOF", "_Alignof"},
    {Token::_ATOMIC, "_ATOMIC", "_Atomic"},
    {Token::_BITINT, "_BITINT", "_BitInt"},
    {Token::_BOOL, "_BOOL", "_Bool"},
    {Token::_COMPLEX, "_COMPLEX", "_Complex"},
    {Token::_DECIMAL128, "_DECIMAL128", "_Decimal128"},
    {Token::_DECIMAL32, "_DECIMAL32", "_Decimal32"},
    {Token::_DECIMAL64, "_DECIMAL64", "_Decimal64"},
    {Token::_GENERIC, "_GENERIC", "_Generic"},
    {Token::_IMAGINARY, "_IMAGINARY", "_Imaginary"},
    {Token::_NORETURN, "_NORETURN", "_Noreturn"},
    {Token::_STATIC_ASSERT, "_STATIC_ASSERT", "_Static_assert"},
    {Token::_THREAD_LOCAL, "_THREAD_LOCAL", "_Thread_local"},
};

TEST_P(KeywordFixture, keytest) {
  ASSERT_NO_FATAL_FAILURE(basic_token_test(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(my, KeywordFixture, ::testing::ValuesIn(keywords));

void empty_token_test(const test_tup_t &tup) {
  const Token tok = std::get<0>(tup);
  const char *tstr = std::get<1>(tup);
  const char *lex = std::get<2>(tup);

  EXPECT_STREQ(tstr, ttos[EnumToUnsigned(tok)]);

  std::vector<Lexeme> v = scan_tokens(lex);

  ASSERT_EQ(static_cast<std::vector<Lexeme>::size_type>(1), v.size());
  EXPECT_EQ(tok, v[0]);
  EXPECT_STREQ(v[0].lexeme_.c_str(), lex);
}

class EmptyFixture : public ::testing::TestWithParam<test_tup_t> {};

test_tup_t empty[] = {
    {Token::END, "END", ""},
};

TEST_P(EmptyFixture, emptytest) {
  ASSERT_NO_FATAL_FAILURE(empty_token_test(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(my, EmptyFixture, ::testing::ValuesIn(empty));

TEST(Invalid, test) {
  EXPECT_STREQ(ttos[EnumToUnsigned(Token::INVALID)], "INVALID");
}

class BasicIntegerLiteralFixture : public ::testing::TestWithParam<test_tup_t> {
};

test_tup_t basic_integer[] = {
    {Token::INTEGER_LIT, "INTEGER_LIT", "0"},
};

TEST_P(BasicIntegerLiteralFixture, basicinttest) {
  ASSERT_NO_FATAL_FAILURE(basic_token_test(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(my, BasicIntegerLiteralFixture,
                         ::testing::ValuesIn(basic_integer));

void integer_literal_test(const char *src) {
  std::vector<Lexeme> v = scan_tokens(src);

  ASSERT_EQ(static_cast<std::vector<Lexeme>::size_type>(2), v.size());
  EXPECT_EQ(Token::INTEGER_LIT, v[0]);
  EXPECT_STREQ(v[0].lexeme_.c_str(), src);

  EXPECT_EQ(Token::END, v[1]);
}

class IntegerLiteralFixture : public ::testing::TestWithParam<const char *> {};

const char *integer[] = {"0",
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
                         "\'\\t\\t\'"};

TEST_P(IntegerLiteralFixture, inttest) {
  ASSERT_NO_FATAL_FAILURE(integer_literal_test(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(my, IntegerLiteralFixture,
                         ::testing::ValuesIn(integer));

void integer_ident_test(const char *src) {
  std::vector<Lexeme> v = scan_tokens(src);

  ASSERT_EQ(static_cast<std::vector<Lexeme>::size_type>(3), v.size());
  EXPECT_EQ(Token::INTEGER_LIT, v[0]);
  EXPECT_EQ(Token::IDENTIFIER, v[1]);
  EXPECT_EQ(Token::END, v[2]);
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

void float_literal_test(const char *src) {
  std::vector<Lexeme> v = scan_tokens(src);

  ASSERT_EQ(static_cast<std::vector<Lexeme>::size_type>(2), v.size());
  EXPECT_EQ(Token::FLOAT_LIT, v[0]);
  EXPECT_STREQ(v[0].lexeme_.c_str(), src);

  EXPECT_EQ(Token::END, v[1]);
}

class FloatLiteralFixture : public ::testing::TestWithParam<const char *> {};

const char *floats[] = {
    "0e1",       "0e-1",      "0E+1",     "0e-1f",    "0E-1F",     "0e+1l",
    "12e-06L",   "0e-1df",    "0E-1dd",   "0e+1dl",   "0e-1DF",    "0E-1DD",
    "0e+1DL",    "11e1",      "11e-5",    "11e1f",    "11e-5F",    "11e1l",
    "11e-5L",    "11e1df",    "11e-5dd",  "11e1dl",   "11e1DF",    "11e-5DD",
    "11e1DL",

    ".11",       ".11f",      ".11F",     ".11l",     ".11L",      ".11df",
    ".11dd",     ".11dl",     ".11DF",    ".11DD",    ".11DL",     ".11e1f",
    ".11e-5F",   ".11e1l",    ".11e-5L",  ".11e1df",  ".11e-5dd",  ".11e1dl",
    ".11e1DF",   ".11e-5DD",  ".11e1DL",

    "3.11",      "3.11f",     "3.11F",    "3.11l",    "3.11L",     "3.11df",
    "3.11dd",    "3.11dl",    "3.11DF",   "3.11DD",   "3.11DL",    "3.11e1f",
    "3.11e-5F",  "3.11e1l",   "3.11e-5L", "3.11e1df", "3.11e-5dd", "3.11e1dl",
    "3.11e1DF",  "3.11e-5DD", "3.11e1DL",

    "0.",        "12.",       "12.e5",    "12.E+5dl", "34.e-05L",  "67543.E6",
    "0.E1dd",    "0.0e-1",    "0.01E+1",  "0.0e-1f",  "0.0E-1F",   "0.0e+1l",
    "12.0e-06L", "0.0e-1df",  "0.0E-1dd", "0.0e+1dl", "0.0e-1DF",  "0.0E-1DD",
    "0.0e+1DL",
};

TEST_P(FloatLiteralFixture, floattest) {
  ASSERT_NO_FATAL_FAILURE(float_literal_test(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(my, FloatLiteralFixture, ::testing::ValuesIn(floats));

void neg_float_literal_test(const char *src) {
  std::vector<Lexeme> v = scan_tokens(src);

  EXPECT_EQ(Token::INVALID, v[0]);

  if (v.size() == 3) {
    std::string f(src);
    f = f.substr(0, f.length() - 1);

    EXPECT_STREQ(v[0].lexeme_.c_str(), f.c_str());

    std::string i(src);
    i = i.substr(i.length() - 1, 1);

    EXPECT_EQ(Token::IDENTIFIER, v[1]);
    EXPECT_STREQ(v[1].lexeme_.c_str(), i.c_str());

    EXPECT_EQ(Token::END, v[2]);
  } else {
    EXPECT_EQ(Token::END, v[1]);
  }
}

class NegFloatLiteralFixture : public ::testing::TestWithParam<const char *> {};

const char *negative_floats[] = {
    "0.0e-1dL", // suffix mismatch: dL should be DL or dl.
    "0.0e-1Df", // suffix mismatch: Df should be DF or df.
    "0.0e+",    // exponent missing power digit(s)
    "0.0e-",    // exponent missing power digit(s)
    "0.0e",     // exponent missing power digit(s)
};

TEST_P(NegFloatLiteralFixture, negfloattest) {
  ASSERT_NO_FATAL_FAILURE(neg_float_literal_test(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(my, NegFloatLiteralFixture,
                         ::testing::ValuesIn(negative_floats));

TEST(SrcLocation, test) {
  const char *src = R"c(int main(int argc, char *argv[]) {
  return 0;
})c";

  std::vector<Lexeme> v = scan_tokens(src);

#define su(x) static_cast<std::uint32_t>(x)

  ASSERT_EQ(su(18), v.size());

  EXPECT_EQ(Token::INT, v[0]); // int
  EXPECT_EQ(su(1), v[0].row_);
  EXPECT_EQ(su(1), v[0].col_);

  EXPECT_EQ(Token::IDENTIFIER, v[1]); // main
  EXPECT_EQ(su(1), v[1].row_);
  EXPECT_EQ(su(5), v[1].col_);

  EXPECT_EQ(Token::LPAREN, v[2]); // (
  EXPECT_EQ(su(1), v[2].row_);
  EXPECT_EQ(su(9), v[2].col_);

  EXPECT_EQ(Token::INT, v[3]); // int
  EXPECT_EQ(su(1), v[3].row_);
  EXPECT_EQ(su(10), v[3].col_);

  EXPECT_EQ(Token::IDENTIFIER, v[4]); // argc
  EXPECT_EQ(su(1), v[4].row_);
  EXPECT_EQ(su(14), v[4].col_);

  EXPECT_EQ(Token::COMMA, v[5]); // ,
  EXPECT_EQ(su(1), v[5].row_);
  EXPECT_EQ(su(18), v[5].col_);

  EXPECT_EQ(Token::CHAR, v[6]); // char
  EXPECT_EQ(su(1), v[6].row_);
  EXPECT_EQ(su(20), v[6].col_);

  EXPECT_EQ(Token::STAR, v[7]); // *
  EXPECT_EQ(su(1), v[7].row_);
  EXPECT_EQ(su(25), v[7].col_);

  EXPECT_EQ(Token::IDENTIFIER, v[8]); // argv
  EXPECT_EQ(su(1), v[8].row_);
  EXPECT_EQ(su(26), v[8].col_);

  EXPECT_EQ(Token::LSQUARE, v[9]); // [
  EXPECT_EQ(su(1), v[9].row_);
  EXPECT_EQ(su(30), v[9].col_);

  EXPECT_EQ(Token::RSQUARE, v[10]); // ]
  EXPECT_EQ(su(1), v[10].row_);
  EXPECT_EQ(su(31), v[10].col_);

  EXPECT_EQ(Token::RPAREN, v[11]); // )
  EXPECT_EQ(su(1), v[11].row_);
  EXPECT_EQ(su(32), v[11].col_);

  EXPECT_EQ(Token::LBRACE, v[12]); // {
  EXPECT_EQ(su(1), v[12].row_);
  EXPECT_EQ(su(34), v[12].col_);

  EXPECT_EQ(Token::RETURN, v[13]); // return
  EXPECT_EQ(su(2), v[13].row_);
  EXPECT_EQ(su(3), v[13].col_);

  EXPECT_EQ(Token::INTEGER_LIT, v[14]); // 0
  EXPECT_EQ(su(2), v[14].row_);
  EXPECT_EQ(su(10), v[14].col_);

  EXPECT_EQ(Token::SEMI, v[15]); // ;
  EXPECT_EQ(su(2), v[15].row_);
  EXPECT_EQ(su(11), v[15].col_);

  EXPECT_EQ(Token::RBRACE, v[16]); // }
  EXPECT_EQ(su(3), v[16].row_);
  EXPECT_EQ(su(1), v[16].col_);

  EXPECT_EQ(Token::END, v[17]);
  EXPECT_EQ(su(3), v[17].row_);
  EXPECT_EQ(su(2), v[17].col_);

#undef su
}

TEST(TwoDots, test) {
  const char *src = "..";

  std::vector<Lexeme> v = scan_tokens(src);

#define su(x) static_cast<std::uint32_t>(x)

  ASSERT_EQ(su(3), v.size());

  EXPECT_EQ(Token::DOT, v[0]);
  EXPECT_EQ(su(1), v[0].row_);
  EXPECT_EQ(su(1), v[0].col_);

  EXPECT_EQ(Token::DOT, v[1]);
  EXPECT_EQ(su(1), v[1].row_);
  EXPECT_EQ(su(2), v[1].col_);

  EXPECT_EQ(Token::END, v[2]);
  EXPECT_EQ(su(1), v[2].row_);
  EXPECT_EQ(su(3), v[2].col_);

#undef su
}

TEST(IdentThenInvalid, test) {
  const char *src = "_$";

  std::vector<Lexeme> v = scan_tokens(src);

#define su(x) static_cast<std::uint32_t>(x)

  ASSERT_EQ(su(2), v.size());

  EXPECT_EQ(Token::IDENTIFIER, v[0]);
  EXPECT_EQ(su(1), v[0].row_);
  EXPECT_EQ(su(1), v[0].col_);

  EXPECT_EQ(Token::END, v[1]);
  EXPECT_EQ(su(1), v[1].row_);
  EXPECT_EQ(su(3), v[1].col_);

#undef su
}

TEST(InvalidChar, test) {
  const char *src = R"invalid(`#
$ @
 \)invalid";

  std::vector<Lexeme> v = scan_tokens(src);

#define su(x) static_cast<std::uint32_t>(x)

  ASSERT_EQ(su(1), v.size());

  EXPECT_EQ(Token::END, v[0]);
  EXPECT_EQ(su(3), v[0].row_);
  EXPECT_EQ(su(3), v[0].col_);

#undef su
}

TEST(InvalidHex, test) {
  const char *src = "0xx";

  std::vector<Lexeme> v = scan_tokens(src);

#define su(x) static_cast<std::uint32_t>(x)

  ASSERT_EQ(su(3), v.size());

  EXPECT_EQ(Token::INVALID, v[0]);
  EXPECT_EQ(su(1), v[0].row_);
  EXPECT_EQ(su(1), v[0].col_);

  EXPECT_EQ(Token::IDENTIFIER, v[1]);
  EXPECT_EQ(su(1), v[1].row_);
  EXPECT_EQ(su(3), v[1].col_);

  EXPECT_EQ(Token::END, v[2]);
  EXPECT_EQ(su(1), v[2].row_);
  EXPECT_EQ(su(4), v[2].col_);

#undef su
}

TEST(EmptyCharConst, test) {
  const char *src = "\'\'";

  std::vector<Lexeme> v = scan_tokens(src);

#define su(x) static_cast<std::uint32_t>(x)

  ASSERT_EQ(su(2), v.size());

  EXPECT_EQ(Token::INVALID, v[0]);
  EXPECT_EQ(su(1), v[0].row_);
  EXPECT_EQ(su(1), v[0].col_);

  EXPECT_EQ(Token::END, v[1]);
  EXPECT_EQ(su(1), v[1].row_);
  EXPECT_EQ(su(3), v[1].col_);

#undef su
}

TEST(NewlineInCharConst, test) {
  const char *src = R"invalid('
)invalid";

  std::vector<Lexeme> v = scan_tokens(src);

#define su(x) static_cast<std::uint32_t>(x)

  ASSERT_EQ(su(2), v.size());

  EXPECT_EQ(Token::INVALID, v[0]);
  EXPECT_EQ(su(1), v[0].row_);
  EXPECT_EQ(su(1), v[0].col_);

  EXPECT_EQ(Token::END, v[1]);
  EXPECT_EQ(su(2), v[1].row_);
  EXPECT_EQ(su(1), v[1].col_);

  const char *src2 = R"invalid('a
)invalid";

  v = scan_tokens(src2);

  ASSERT_EQ(su(2), v.size());

  EXPECT_EQ(Token::INVALID, v[0]);
  EXPECT_EQ(su(1), v[0].row_);
  EXPECT_EQ(su(1), v[0].col_);

  EXPECT_EQ(Token::END, v[1]);
  EXPECT_EQ(su(2), v[1].row_);
  EXPECT_EQ(su(1), v[1].col_);

  const char *src3 = R"invalid('\ta
)invalid";

  v = scan_tokens(src3);

  ASSERT_EQ(su(2), v.size());

  EXPECT_EQ(Token::INVALID, v[0]);
  EXPECT_EQ(su(1), v[0].row_);
  EXPECT_EQ(su(1), v[0].col_);

  EXPECT_EQ(Token::END, v[1]);
  EXPECT_EQ(su(2), v[1].row_);
  EXPECT_EQ(su(1), v[1].col_);

  const char *src4 = R"invalid('\t\t
)invalid";

  v = scan_tokens(src4);

  ASSERT_EQ(su(2), v.size());

  EXPECT_EQ(Token::INVALID, v[0]);
  EXPECT_EQ(su(1), v[0].row_);
  EXPECT_EQ(su(1), v[0].col_);

  EXPECT_EQ(Token::END, v[1]);
  EXPECT_EQ(su(2), v[1].row_);
  EXPECT_EQ(su(1), v[1].col_);

#undef su
}

TEST(BadEscapeInCharConst, test) {
  const char *src = R"invalid('\j')invalid";

  std::vector<Lexeme> v = scan_tokens(src);

#define su(x) static_cast<std::uint32_t>(x)

  ASSERT_EQ(su(3), v.size());

  EXPECT_EQ(Token::INVALID, v[0]);
  EXPECT_EQ(su(1), v[0].row_);
  EXPECT_EQ(su(1), v[0].col_);

  EXPECT_EQ(Token::INVALID, v[1]);
  EXPECT_EQ(su(1), v[1].row_);
  EXPECT_EQ(su(4), v[1].col_);

  EXPECT_EQ(Token::END, v[2]);
  EXPECT_EQ(su(1), v[2].row_);
  EXPECT_EQ(su(5), v[2].col_);

#undef su
}

void identifier_test(const char *src) {
  std::vector<Lexeme> v = scan_tokens(src);

  ASSERT_EQ(static_cast<std::vector<Lexeme>::size_type>(2), v.size());
  EXPECT_EQ(Token::IDENTIFIER, v[0]);
  EXPECT_STREQ(v[0].lexeme_.c_str(), src);

  EXPECT_EQ(Token::END, v[1]);
}

class IdentifierFixture : public ::testing::TestWithParam<const char *> {};

const char *identifiers[] = {
    "_z",
    "_Az",
    "_Alz",
    "_Aliz",
    "_Aligz",
    "_Alignz",
    "_Alignaz",
    "_Alignoz",
    "_Atz",
    "_Atoz",
    "_Atomz",
    "_Atomiz",
    "_Bz",
    "_Biz",
    "_Bitz",
    "_BitIz",
    "_BitInz",
    "_Boz",
    "_Booz",
    "_Cz",
    "_Coz",
    "_Comz",
    "_Compz",
    "_Complz",
    "_Complez",
    "_Dz",
    "_Dez",
    "_Decz",
    "_Deciz",
    "_Decimz",
    "_Decimaz",
    "_Decimalz",
    "_Decimal1z",
    "_Decimal12z",
    "_Decimal3z",
    "_Decimal6z",
    "_Gz",
    "_Gez",
    "_Genz",
    "_Genez",
    "_Generz",
    "_Generiz",
    "_Iz",
    "_Imz",
    "_Imaz",
    "_Imagz",
    "_Imagiz",
    "_Imaginz",
    "_Imaginaz",
    "_Imaginarz",
    "_Nz",
    "_Noz",
    "_Norz",
    "_Norez",
    "_Noretz",
    "_Noretuz",
    "_Noreturz",
    "_Sz",
    "_Stz",
    "_Staz",
    "_Statz",
    "_Statiz",
    "_Staticz",
    "_Static_z",
    "_Static_az",
    "_Static_asz",
    "_Static_assz",
    "_Static_assez",
    "_Static_asserz",
    "_Tz",
    "_Thz",
    "_Thrz",
    "_Threz",
    "_Threaz",
    "_Threadz",
    "_Thread_z",
    "_Thread_lz",
    "_Thread_loz",
    "_Thread_locz",
    "_Thread_locaz",
    "az",
    "alz",
    "aliz",
    "aligz",
    "alignz",
    "alignaz",
    "alignoz",
    "auz",
    "autz",
    "bz",
    "boz",
    "booz",
    "brz",
    "brez",
    "breaz",
    "cz",
    "caz",
    "casz",
    "chz",
    "chaz",
    "coz",
    "conz",
    "consz",
    "constz",
    "constez",
    "constexz",
    "constexpz",
    "contz",
    "contiz",
    "continz",
    "continuz",
    "dz",
    "dez",
    "defz",
    "defaz",
    "defauz",
    "defaulz",
    "doz",
    "douz",
    "doubz",
    "doublz",
    "ez",
    "elz",
    "elsz",
    "enz",
    "enuz",
    "exz",
    "extz",
    "extez",
    "exterz",
    "fz",
    "faz",
    "falz",
    "falsz",
    "flz",
    "floz",
    "floaz",
    "foz",
    "gz",
    "goz",
    "gotz",
    "iz",
    "inz",
    "inlz",
    "inliz",
    "inlinz",
    "lz",
    "loz",
    "lonz",
    "nz",
    "nuz",
    "nulz",
    "nullz",
    "nullpz",
    "nullptz",
    "rz",
    "rez",
    "regz",
    "regiz",
    "regisz",
    "registz",
    "registez",
    "resz",
    "restz",
    "restrz",
    "restriz",
    "restricz",
    "retz",
    "retuz",
    "returz",
    "sz",
    "shz",
    "shoz",
    "shorz",
    "siz",
    "sigz",
    "signz",
    "signez",
    "sizz",
    "sizez",
    "sizeoz",
    "stz",
    "staz",
    "statz",
    "statiz",
    "staticz",
    "static_z",
    "static_az",
    "static_asz",
    "static_assz",
    "static_assez",
    "static_asserz",
    "strz",
    "struz",
    "strucz",
    "swz",
    "swiz",
    "switz",
    "switcz",
    "tz",
    "thz",
    "thrz",
    "threz",
    "threaz",
    "threadz",
    "thread_z",
    "thread_lz",
    "thread_loz",
    "thread_locz",
    "thread_locaz",
    "trz",
    "truz",
    "tyz",
    "typz",
    "typez",
    "typedz",
    "typedez",
    "typeoz",
    "typeofz",
    "typeof_z",
    "typeof_uz",
    "typeof_unz",
    "typeof_unqz",
    "typeof_unquz",
    "typeof_unquaz",
    "uz",
    "unz",
    "uniz",
    "unioz",
    "unsz",
    "unsiz",
    "unsigz",
    "unsignz",
    "unsignez",
    "vz",
    "voz",
    "voiz",
    "volz",
    "volaz",
    "volatz",
    "volatiz",
    "volatilz",
    "wz",
    "whz",
    "whiz",
    "whilz",
    "alignasz",
    "alignofz",
    "autoz",
    "boolz",
    "breakz",
    "casez",
    "charz",
    "constz",
    "constexprz",
    "continuez",
    "defaultz",
    "doz",
    "doublez",
    "elsez",
    "enumz",
    "externz",
    "falsez",
    "floatz",
    "forz",
    "gotoz",
    "ifz",
    "inlinez",
    "intz",
    "longz",
    "nullptrz",
    "registerz",
    "restrictz",
    "returnz",
    "shortz",
    "signedz",
    "sizeofz",
    "staticz",
    "static_assertz",
    "structz",
    "switchz",
    "thread_localz",
    "truez",
    "typedefz",
    "typeofz",
    "typeof_unqualz",
    "unionz",
    "unsignedz",
    "voidz",
    "volatilez",
    "whilez",
    "_Alignasz",
    "_Alignofz",
    "_Atomicz",
    "_BitIntz",
    "_Boolz",
    "_Complexz",
    "_Decimal128z",
    "_Decimal32z",
    "_Decimal64z",
    "_Genericz",
    "_Imaginaryz",
    "_Noreturnz",
    "_Static_assertz",
    "_Thread_localz",
};

TEST_P(IdentifierFixture, identtest) {
  ASSERT_NO_FATAL_FAILURE(identifier_test(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(my, IdentifierFixture,
                         ::testing::ValuesIn(identifiers));
