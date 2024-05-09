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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <stack>
#include <utility>

namespace c_lexer {

Lexer::Lexer(std::unique_ptr<SourceReader> &&sr)
    : sr_(std::move(sr)), row_(1), col_(1) {
  lookahead_.push(scan_token());
}

const Lexeme &Lexer::peek() const { return lookahead_.front(); }

Lexeme Lexer::eat() {
  Lexeme l(lookahead_.front());

  lookahead_.pop();
  if (lookahead_.empty())
    lookahead_.push(scan_token());

  return l;
}

void Lexer::preload(std::size_t n) {
  if (lookahead_.back().token() == Token::END)
    return;

  for (std::size_t i = 0; i < n; ++i) {
    lookahead_.push(scan_token());
    if (lookahead_.back().token() == Token::END)
      return;
  }
}

#define eat_whitespace()                                                       \
  do {                                                                         \
    c = sr_->eof() ? EOF : sr_->get();                                         \
    while (std::isspace(c)) {                                                  \
      switch (c) {                                                             \
      case '\n':                                                               \
        ++row_;                                                                \
        col_ = 1;                                                              \
        break;                                                                 \
      case '\v':                                                               \
        row_ += rows_per_vtab;                                                 \
        col_ = 1;                                                              \
        break;                                                                 \
      case '\f':                                                               \
        row_ += rows_per_formfeed;                                             \
        col_ = 1;                                                              \
        break;                                                                 \
      case ' ':                                                                \
        ++col_;                                                                \
        break;                                                                 \
      case '\t':                                                               \
        col_ += cols_per_htab;                                                 \
        break;                                                                 \
      }                                                                        \
      c = sr_->eof() ? EOF : sr_->get();                                       \
    }                                                                          \
  } while (0)

#define r(_tkn, _cols)                                                         \
  do {                                                                         \
    col_ += (_cols);                                                           \
    return Lexeme(std::move(lex), _tkn, row_, col_ - (_cols));                 \
  } while (0)

#define rinvalid() r(Token::INVALID, lex.length())

#define backup(_ch)                                                            \
  do {                                                                         \
    if ((_ch) != EOF) {                                                        \
      sr_->unget(_ch);                                                         \
      lex.pop_back();                                                          \
    }                                                                          \
  } while (0)

#define nextst(_s)                                                             \
  do {                                                                         \
    st = _s;                                                                   \
  } while (0)

#define holdst(_s)                                                             \
  do {                                                                         \
    _hold = true;                                                              \
    nextst(_s);                                                                \
  } while (0)

#define advance() lex.push_back(sr_->get())

#define advancest(_s)                                                          \
  do {                                                                         \
    advance();                                                                 \
    nextst(_s);                                                                \
  } while (0)

#define pushst(_s)                                                             \
  do {                                                                         \
    states.push(st);                                                           \
    nextst(_s);                                                                \
  } while (0)

#define popst()                                                                \
  do {                                                                         \
    nextst(states.top());                                                      \
    states.pop();                                                              \
  } while (0)

#define pop_holdst()                                                           \
  do {                                                                         \
    popst();                                                                   \
    _hold = true;                                                              \
  } while (0)

// C identifier names start with [a-zA-Z_] and continue with [a-zA-Z_0-9]
inline bool is_ident_start(char c) { return std::isalpha(c) || c == '_'; }

inline bool is_ident_cont(char c) { return std::isalnum(c) || c == '_'; }

inline bool is_int_suffix_start(char c) {
  return std::strchr("uUlLwW", c) != NULL;
}

inline bool
is_simple_escape_sequence(char c) { // c is the char after the backslash
  return std::strchr("\'\"?\\abfnrtv", c) != NULL;
}

#define START 0

#define GOT_GT 1
#define GOT_LT 2

#define GOT__ 3
#define GOT__A 4
#define GOT__Al 5
#define GOT__Ali 6
#define GOT__Alig 7
#define GOT__Align 8
#define GOT__Aligna 9
#define GOT__Aligno 10
#define GOT__At 11
#define GOT__Ato 12
#define GOT__Atom 13
#define GOT__Atomi 14
#define GOT__B 15
#define GOT__Bi 16
#define GOT__Bit 17
#define GOT__BitI 18
#define GOT__BitIn 19
#define GOT__Bo 20
#define GOT__Boo 21
#define GOT__C 22
#define GOT__Co 23
#define GOT__Com 24
#define GOT__Comp 25
#define GOT__Compl 26
#define GOT__Comple 27
#define GOT__D 28
#define GOT__De 29
#define GOT__Dec 30
#define GOT__Deci 31
#define GOT__Decim 32
#define GOT__Decima 33
#define GOT__Decimal 34
#define GOT__Decimal1 35
#define GOT__Decimal12 36
#define GOT__Decimal3 37
#define GOT__Decimal6 38
#define GOT__G 39
#define GOT__Ge 40
#define GOT__Gen 41
#define GOT__Gene 42
#define GOT__Gener 43
#define GOT__Generi 44
#define GOT__I 45
#define GOT__Im 46
#define GOT__Ima 47
#define GOT__Imag 48
#define GOT__Imagi 49
#define GOT__Imagin 50
#define GOT__Imagina 51
#define GOT__Imaginar 52
#define GOT__N 53
#define GOT__No 54
#define GOT__Nor 55
#define GOT__Nore 56
#define GOT__Noret 57
#define GOT__Noretu 58
#define GOT__Noretur 59
#define GOT__S 60
#define GOT__St 61
#define GOT__Sta 62
#define GOT__Stat 63
#define GOT__Stati 64
#define GOT__Static 65
#define GOT__Static_ 66
#define GOT__Static_a 67
#define GOT__Static_as 68
#define GOT__Static_ass 69
#define GOT__Static_asse 70
#define GOT__Static_asser 71
#define GOT__T 72
#define GOT__Th 73
#define GOT__Thr 74
#define GOT__Thre 75
#define GOT__Threa 76
#define GOT__Thread 77
#define GOT__Thread_ 78
#define GOT__Thread_l 79
#define GOT__Thread_lo 80
#define GOT__Thread_loc 81
#define GOT__Thread_loca 82
#define GOT_a 83
#define GOT_al 84
#define GOT_ali 85
#define GOT_alig 86
#define GOT_align 87
#define GOT_aligna 88
#define GOT_aligno 89
#define GOT_au 90
#define GOT_aut 91
#define GOT_b 92
#define GOT_bo 93
#define GOT_boo 94
#define GOT_br 95
#define GOT_bre 96
#define GOT_brea 97
#define GOT_c 98
#define GOT_ca 99
#define GOT_cas 100
#define GOT_ch 101
#define GOT_cha 102
#define GOT_co 103
#define GOT_con 104
#define GOT_cons 105
#define GOT_conste 107
#define GOT_constex 108
#define GOT_constexp 109
#define GOT_cont 110
#define GOT_conti 111
#define GOT_contin 112
#define GOT_continu 113
#define GOT_d 114
#define GOT_de 115
#define GOT_def 116
#define GOT_defa 117
#define GOT_defau 118
#define GOT_defaul 119
#define GOT_dou 121
#define GOT_doub 122
#define GOT_doubl 123
#define GOT_e 124
#define GOT_el 125
#define GOT_els 126
#define GOT_en 127
#define GOT_enu 128
#define GOT_ex 129
#define GOT_ext 130
#define GOT_exte 131
#define GOT_exter 132
#define GOT_f 133
#define GOT_fa 134
#define GOT_fal 135
#define GOT_fals 136
#define GOT_fl 137
#define GOT_flo 138
#define GOT_floa 139
#define GOT_fo 140
#define GOT_g 141
#define GOT_go 142
#define GOT_got 143
#define GOT_i 144
#define GOT_in 145
#define GOT_inl 146
#define GOT_inli 147
#define GOT_inlin 148
#define GOT_l 149
#define GOT_lo 150
#define GOT_lon 151
#define GOT_n 152
#define GOT_nu 153
#define GOT_nul 154
#define GOT_null 155
#define GOT_nullp 156
#define GOT_nullpt 157
#define GOT_r 158
#define GOT_re 159
#define GOT_reg 160
#define GOT_regi 161
#define GOT_regis 162
#define GOT_regist 163
#define GOT_registe 164
#define GOT_res 165
#define GOT_rest 166
#define GOT_restr 167
#define GOT_restri 168
#define GOT_restric 169
#define GOT_ret 170
#define GOT_retu 171
#define GOT_retur 172
#define GOT_s 173
#define GOT_sh 174
#define GOT_sho 175
#define GOT_shor 176
#define GOT_si 177
#define GOT_sig 178
#define GOT_sign 179
#define GOT_signe 180
#define GOT_siz 181
#define GOT_size 182
#define GOT_sizeo 183
#define GOT_st 184
#define GOT_sta 185
#define GOT_stat 186
#define GOT_stati 187
#define GOT_static_ 189
#define GOT_static_a 190
#define GOT_static_as 191
#define GOT_static_ass 192
#define GOT_static_asse 193
#define GOT_static_asser 194
#define GOT_str 195
#define GOT_stru 196
#define GOT_struc 197
#define GOT_sw 198
#define GOT_swi 199
#define GOT_swit 200
#define GOT_switc 201
#define GOT_t 202
#define GOT_th 203
#define GOT_thr 204
#define GOT_thre 205
#define GOT_threa 206
#define GOT_thread 207
#define GOT_thread_ 208
#define GOT_thread_l 209
#define GOT_thread_lo 210
#define GOT_thread_loc 211
#define GOT_thread_loca 212
#define GOT_tr 213
#define GOT_tru 214
#define GOT_ty 215
#define GOT_typ 216
#define GOT_type 217
#define GOT_typed 218
#define GOT_typede 219
#define GOT_typeo 220
#define GOT_typeof_ 222
#define GOT_typeof_u 223
#define GOT_typeof_un 224
#define GOT_typeof_unq 225
#define GOT_typeof_unqu 226
#define GOT_typeof_unqua 227
#define GOT_u 228
#define GOT_un 229
#define GOT_uni 230
#define GOT_unio 231
#define GOT_uns 232
#define GOT_unsi 233
#define GOT_unsig 234
#define GOT_unsign 235
#define GOT_unsigne 236
#define GOT_v 237
#define GOT_vo 238
#define GOT_voi 239
#define GOT_vol 240
#define GOT_vola 241
#define GOT_volat 242
#define GOT_volati 243
#define GOT_volatil 244
#define GOT_w 245
#define GOT_wh 246
#define GOT_whi 247
#define GOT_whil 248

#define GOT_0x 300
#define GOT_INT_LITERAL 301
#define GOT_HEX_LITERAL 302
#define GOT_OCT_LITERAL 303

#define GOT_U 304
#define GOT_L 305
#define GOT_CHAR_CONST_START 306
#define GOT_CHAR_CONST_CONT 307

#define GOT_FLOAT_CONST_e 308
#define GOT_FLOAT_CONST_e_SIGN 309
#define GOT_FLOAT_CONST_e_SIGN_DIG 310
#define GOT_FLOAT_CONST_e_SUFd 311
#define GOT_FLOAT_CONST_e_SUFD 312

#define GOT_FLOAT_CONST_DOT 313
#define GOT_FLOAT_CONST_DOT_DIGIT 314

#define GOT_0x_DOT 315
#define GOT_HEX_CONST_p 316
#define GOT_HEX_CONST_p_SIGN 317
#define GOT_HEX_CONST_p_SIGN_DIG 318

#define GOT_HEX_CONST_DOT 319
#define GOT_HEX_CONST_DOT_XDIGIT 320

#define GOT_BIN_CONST_START 321
#define GOT_BIN_CONST_CONT 322

#define GOT_INT_SUFFIX_START 323
#define GOT_INT_SUFFIX_u 324
#define GOT_INT_SUFFIX_U 325
#define GOT_INT_SUFFIX_l 326
#define GOT_INT_SUFFIX_L 327
#define GOT_INT_SUFFIX_w 328
#define GOT_INT_SUFFIX_W 329

#define GOT_STRING_LIT_START 330

#define GOT_ESCAPE_SEQUENCE 331
#define GOT_ESCAPE_SEQUENCE_BS_OCT1 332
#define GOT_ESCAPE_SEQUENCE_BS_OCT2 333
#define GOT_ESCAPE_SEQUENCE_BS_x 334
#define GOT_ESCAPE_SEQUENCE_BS_x_XDIGIT 335
#define GOT_ESCAPE_SEQUENCE_BS_u0 336
#define GOT_ESCAPE_SEQUENCE_BS_u1 337
#define GOT_ESCAPE_SEQUENCE_BS_u2 338
#define GOT_ESCAPE_SEQUENCE_BS_u3 339
#define GOT_ESCAPE_SEQUENCE_BS_U0 340
#define GOT_ESCAPE_SEQUENCE_BS_U1 341
#define GOT_ESCAPE_SEQUENCE_BS_U2 342
#define GOT_ESCAPE_SEQUENCE_BS_U3 343
#define GOT_ESCAPE_SEQUENCE_BS_U4 344
#define GOT_ESCAPE_SEQUENCE_BS_U5 345
#define GOT_ESCAPE_SEQUENCE_BS_U6 346
#define GOT_ESCAPE_SEQUENCE_BS_U7 347

#define GOT_IDENT 999

#define THE_END 1000

const std::uint32_t cols_per_htab = 1;
const std::uint32_t rows_per_vtab = 1;
const std::uint32_t rows_per_formfeed = 1;

Lexeme Lexer::scan_token() {
  std::string lex;
  std::stack<int> states;
  int st = START;
  char c;
  bool _hold = true;

  states.push(st);

  eat_whitespace();

  if (c != EOF)
    lex.push_back(c);

  while (true) {
    if (_hold) {
      _hold = false;
    } else {
      c = sr_->eof() ? EOF : sr_->get();
      if (c != EOF)
        lex.push_back(c);
    }

    switch (st) {
    case THE_END:
      r(Token::END, 0);

    case START:
      switch (c) {
      case EOF:
        holdst(THE_END);
        break;
      case '~':
        r(Token::BIT_NOT, 1);
      case ',':
        r(Token::COMMA, 1);
      case '?':
        r(Token::QUESTION, 1);
      case ':':
        r(Token::COLON, 1);
      case '(':
        r(Token::LPAREN, 1);
      case ')':
        r(Token::RPAREN, 1);
      case '{':
        r(Token::LBRACE, 1);
      case '}':
        r(Token::RBRACE, 1);
      case '[':
        r(Token::LSQUARE, 1);
      case ']':
        r(Token::RSQUARE, 1);
      case ';':
        r(Token::SEMI, 1);
      case '%':
        if (sr_->peek() == '=') {
          advance();
          r(Token::MOD_ASSIGN, 2);
        } else {
          r(Token::MOD, 1);
        }
      case '/':
        if (sr_->peek() == '=') {
          advance();
          r(Token::DIV_ASSIGN, 2);
        } else {
          r(Token::DIV, 1);
        }
      case '*':
        if (sr_->peek() == '=') {
          advance();
          r(Token::MUL_ASSIGN, 2);
        } else {
          r(Token::STAR, 1);
        }
      case '=':
        if (sr_->peek() == '=') {
          advance();
          r(Token::EQUALS, 2);
        } else {
          r(Token::ASSIGN, 1);
        }
      case '!':
        if (sr_->peek() == '=') {
          advance();
          r(Token::NOTEQUALS, 2);
        } else {
          r(Token::LOG_NOT, 1);
        }
      case '^':
        if (sr_->peek() == '=') {
          advance();
          r(Token::XOR_ASSIGN, 2);
        } else {
          r(Token::BIT_XOR, 1);
        }
      case '.': {
        const int peek = sr_->peek();
        if (peek == '.') {
          advance();
          if (sr_->peek() == '.') {
            advance();
            r(Token::ELLIPSIS, 3);
          }
          backup('.');
          r(Token::DOT, 1);
        } else if (std::isdigit(peek)) {
          nextst(GOT_FLOAT_CONST_DOT_DIGIT);
        } else {
          r(Token::DOT, 1);
        }
      } break;
      case '|': {
        const char peek = sr_->peek();
        if (peek == '|') {
          advance();
          r(Token::LOG_OR, 2);
        } else if (peek == '=') {
          advance();
          r(Token::OR_ASSIGN, 2);
        } else {
          r(Token::BIT_OR, 1);
        }
      }
      case '&': {
        const char peek = sr_->peek();
        if (peek == '&') {
          advance();
          r(Token::LOG_AND, 2);
        } else if (peek == '=') {
          advance();
          r(Token::AND_ASSIGN, 2);
        } else {
          r(Token::AMP, 1);
        }
      }
      case '+': {
        const char peek = sr_->peek();
        if (peek == '+') {
          advance();
          r(Token::INCR, 2);
        } else if (peek == '=') {
          advance();
          r(Token::ADD_ASSIGN, 2);
        } else {
          r(Token::PLUS, 1);
        }
      }
      case '-': {
        const char peek = sr_->peek();
        if (peek == '-') {
          advance();
          r(Token::DECR, 2);
        } else if (peek == '=') {
          advance();
          r(Token::SUB_ASSIGN, 2);
        } else if (peek == '>') {
          advance();
          r(Token::ARROW, 2);
        } else {
          r(Token::MINUS, 1);
        }
      }
      case '>':
        nextst(GOT_GT);
        break;
      case '<':
        nextst(GOT_LT);
        break;
      case '\'':
        nextst(GOT_CHAR_CONST_START);
        break;
      case '\"':
        nextst(GOT_STRING_LIT_START);
        break;
      case '0': {
        const char peek = sr_->peek();
        if (peek == 'x' || peek == 'X') {
          advancest(GOT_0x);
        } else if (peek == 'b' || peek == 'B') {
          advancest(GOT_BIN_CONST_START);
        } else {
          nextst(GOT_OCT_LITERAL);
        }
      } break;
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        holdst(GOT_INT_LITERAL);
        break;
      case 'a': // alignas alignof auto
        nextst(GOT_a);
        break;
      case 'b': // bool break
        nextst(GOT_b);
        break;
      case 'c': // case char const constexpr continue
        nextst(GOT_c);
        break;
      case 'd': // default do double
        nextst(GOT_d);
        break;
      case 'e': // else enum extern
        nextst(GOT_e);
        break;
      case 'f': // false float for
        nextst(GOT_f);
        break;
      case 'g': // goto
        nextst(GOT_g);
        break;
      case 'i': // if inline int
        nextst(GOT_i);
        break;
      case 'l': // long
        nextst(GOT_l);
        break;
      case 'L':
        nextst(GOT_L);
        break;
      case 'n': // nullptr
        nextst(GOT_n);
        break;
      case 'r': // register restrict return
        nextst(GOT_r);
        break;
      case 's': // short signed sizeof static static_assert struct switch
        nextst(GOT_s);
        break;
      case 't': // thread_local true typedef typeof typeof_unqual
        nextst(GOT_t);
        break;
      case 'u': // union unsigned
        nextst(GOT_u);
        break;
      case 'U':
        nextst(GOT_U);
        break;
      case 'v': // void volatile
        nextst(GOT_v);
        break;
      case 'w': // while
        nextst(GOT_w);
        break;
      case '_': // _Alignas _Alignof _Atomic _BitInt _Bool _Complex
                // _Decimal128 _Decimal32 _Decimal64 _Generic _Imaginary
                // _Noreturn _Static_assert _Thread_local
        nextst(GOT__);
        break;
      default:
        if (is_ident_start(c)) {
          holdst(GOT_IDENT);
        } else {
          print_error(std::cerr, "Skipped invalid character ",
                      static_cast<int>(c), ' ', std::isprint(c) ? c : ' ',
                      '\n');
          ++col_;

          eat_whitespace();
          holdst(START);
        }
      } // switch (c) for START
      break;

    case GOT_GT:
      switch (c) {
      case '=':
        r(Token::GREATER_OR_EQUAL, 2);
      case '>':
        if (sr_->peek() == '=') {
          advance();
          r(Token::RSHIFT_ASSIGN, 3);
        } else {
          r(Token::RSHIFT, 2);
        }
      default:
        backup(c);
        r(Token::GREATER, 1);
      } // switch (c) for GOT_GT
      break;

    case GOT_LT:
      switch (c) {
      case '=':
        r(Token::LESS_OR_EQUAL, 2);
      case '<':
        if (sr_->peek() == '=') {
          advance();
          r(Token::LSHIFT_ASSIGN, 3);
        } else {
          r(Token::LSHIFT, 2);
        }
      default:
        backup(c);
        r(Token::LESS, 1);
      } // switch (c) for GOT_LT
      break;

    case GOT_IDENT:
      if (is_ident_cont(c)) {
        // Eat c and remain in this state.
      } else {
        backup(c);
        r(Token::IDENTIFIER, lex.length());
      }
      break;

    case GOT__:
      switch (c) {
      case 'A':
        nextst(GOT__A);
        break;
      case 'B':
        nextst(GOT__B);
        break;
      case 'C':
        nextst(GOT__C);
        break;
      case 'D':
        nextst(GOT__D);
        break;
      case 'G':
        nextst(GOT__G);
        break;
      case 'I':
        nextst(GOT__I);
        break;
      case 'N':
        nextst(GOT__N);
        break;
      case 'S':
        nextst(GOT__S);
        break;
      case 'T':
        nextst(GOT__T);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__
      break;

    case GOT__A:
      switch (c) {
      case 'l':
        nextst(GOT__Al);
        break;
      case 't':
        nextst(GOT__At);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__A
      break;

    case GOT__Al:
      switch (c) {
      case 'i':
        nextst(GOT__Ali);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Al
      break;

    case GOT__Ali:
      switch (c) {
      case 'g':
        nextst(GOT__Alig);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Ali
      break;

    case GOT__Alig:
      switch (c) {
      case 'n':
        nextst(GOT__Align);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Alig
      break;

    case GOT__Align:
      switch (c) {
      case 'a':
        nextst(GOT__Aligna);
        break;
      case 'o':
        nextst(GOT__Aligno);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Align
      break;

    case GOT__Aligna:
      switch (c) {
      case 's':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::_ALIGNAS, 8);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Aligna
      break;

    case GOT__Aligno:
      switch (c) {
      case 'f':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::_ALIGNOF, 8);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Aligno
      break;

    case GOT__At:
      switch (c) {
      case 'o':
        nextst(GOT__Ato);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__At
      break;

    case GOT__Ato:
      switch (c) {
      case 'm':
        nextst(GOT__Atom);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Ato
      break;

    case GOT__Atom:
      switch (c) {
      case 'i':
        nextst(GOT__Atomi);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Atom
      break;

    case GOT__Atomi:
      switch (c) {
      case 'c':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::_ATOMIC, 7);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Atomi
      break;

    case GOT__B:
      switch (c) {
      case 'i':
        nextst(GOT__Bi);
        break;
      case 'o':
        nextst(GOT__Bo);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__B
      break;

    case GOT__Bi:
      switch (c) {
      case 't':
        nextst(GOT__Bit);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Bi
      break;

    case GOT__Bit:
      switch (c) {
      case 'I':
        nextst(GOT__BitI);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Bit
      break;

    case GOT__BitI:
      switch (c) {
      case 'n':
        nextst(GOT__BitIn);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__BitI
      break;

    case GOT__BitIn:
      switch (c) {
      case 't':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::_BITINT, 7);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__BitIn
      break;

    case GOT__Bo:
      switch (c) {
      case 'o':
        nextst(GOT__Boo);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Bo
      break;

    case GOT__Boo:
      switch (c) {
      case 'l':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::_BOOL, 5);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Boo
      break;

    case GOT__C:
      switch (c) {
      case 'o':
        nextst(GOT__Co);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__C
      break;

    case GOT__Co:
      switch (c) {
      case 'm':
        nextst(GOT__Com);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Co
      break;

    case GOT__Com:
      switch (c) {
      case 'p':
        nextst(GOT__Comp);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Com
      break;

    case GOT__Comp:
      switch (c) {
      case 'l':
        nextst(GOT__Compl);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Comp
      break;

    case GOT__Compl:
      switch (c) {
      case 'e':
        nextst(GOT__Comple);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Compl
      break;

    case GOT__Comple:
      switch (c) {
      case 'x':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::_COMPLEX, 8);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Comple
      break;

    case GOT__D:
      switch (c) {
      case 'e':
        nextst(GOT__De);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__D
      break;

    case GOT__De:
      switch (c) {
      case 'c':
        nextst(GOT__Dec);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__De
      break;

    case GOT__Dec:
      switch (c) {
      case 'i':
        nextst(GOT__Deci);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Dec
      break;

    case GOT__Deci:
      switch (c) {
      case 'm':
        nextst(GOT__Decim);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Deci
      break;

    case GOT__Decim:
      switch (c) {
      case 'a':
        nextst(GOT__Decima);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Decim
      break;

    case GOT__Decima:
      switch (c) {
      case 'l':
        nextst(GOT__Decimal);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Decima
      break;

    case GOT__Decimal:
      switch (c) {
      case '1':
        nextst(GOT__Decimal1);
        break;
      case '3':
        nextst(GOT__Decimal3);
        break;
      case '6':
        nextst(GOT__Decimal6);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Decimal
      break;

    case GOT__Decimal1:
      switch (c) {
      case '2':
        nextst(GOT__Decimal12);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Decimal1
      break;

    case GOT__Decimal12:
      switch (c) {
      case '8':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::_DECIMAL128, 11);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Decimal12
      break;

    case GOT__Decimal3:
      switch (c) {
      case '2':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::_DECIMAL32, 10);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Decimal3
      break;

    case GOT__Decimal6:
      switch (c) {
      case '4':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::_DECIMAL64, 10);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Decimal6
      break;

    case GOT__G:
      switch (c) {
      case 'e':
        nextst(GOT__Ge);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__G
      break;

    case GOT__Ge:
      switch (c) {
      case 'n':
        nextst(GOT__Gen);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Ge
      break;

    case GOT__Gen:
      switch (c) {
      case 'e':
        nextst(GOT__Gene);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Gen
      break;

    case GOT__Gene:
      switch (c) {
      case 'r':
        nextst(GOT__Gener);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Gene
      break;

    case GOT__Gener:
      switch (c) {
      case 'i':
        nextst(GOT__Generi);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Gener
      break;

    case GOT__Generi:
      switch (c) {
      case 'c':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::_GENERIC, 8);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Generi
      break;

    case GOT__I:
      switch (c) {
      case 'm':
        nextst(GOT__Im);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__I
      break;

    case GOT__Im:
      switch (c) {
      case 'a':
        nextst(GOT__Ima);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Im
      break;

    case GOT__Ima:
      switch (c) {
      case 'g':
        nextst(GOT__Imag);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Ima
      break;

    case GOT__Imag:
      switch (c) {
      case 'i':
        nextst(GOT__Imagi);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Imag
      break;

    case GOT__Imagi:
      switch (c) {
      case 'n':
        nextst(GOT__Imagin);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Imagi
      break;

    case GOT__Imagin:
      switch (c) {
      case 'a':
        nextst(GOT__Imagina);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Imagin
      break;

    case GOT__Imagina:
      switch (c) {
      case 'r':
        nextst(GOT__Imaginar);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Imagina
      break;

    case GOT__Imaginar:
      switch (c) {
      case 'y':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::_IMAGINARY, 10);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Imaginar
      break;

    case GOT__N:
      switch (c) {
      case 'o':
        nextst(GOT__No);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__N
      break;

    case GOT__No:
      switch (c) {
      case 'r':
        nextst(GOT__Nor);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__No
      break;

    case GOT__Nor:
      switch (c) {
      case 'e':
        nextst(GOT__Nore);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Nor
      break;

    case GOT__Nore:
      switch (c) {
      case 't':
        nextst(GOT__Noret);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Nore
      break;

    case GOT__Noret:
      switch (c) {
      case 'u':
        nextst(GOT__Noretu);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Noret
      break;

    case GOT__Noretu:
      switch (c) {
      case 'r':
        nextst(GOT__Noretur);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Noretu
      break;

    case GOT__Noretur:
      switch (c) {
      case 'n':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::_NORETURN, 9);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Noretur
      break;

    case GOT__S:
      switch (c) {
      case 't':
        nextst(GOT__St);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__S
      break;

    case GOT__St:
      switch (c) {
      case 'a':
        nextst(GOT__Sta);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__St
      break;

    case GOT__Sta:
      switch (c) {
      case 't':
        nextst(GOT__Stat);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Sta
      break;

    case GOT__Stat:
      switch (c) {
      case 'i':
        nextst(GOT__Stati);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Stat
      break;

    case GOT__Stati:
      switch (c) {
      case 'c':
        nextst(GOT__Static);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Stati
      break;

    case GOT__Static:
      switch (c) {
      case '_':
        nextst(GOT__Static_);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Static
      break;

    case GOT__Static_:
      switch (c) {
      case 'a':
        nextst(GOT__Static_a);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Static_
      break;

    case GOT__Static_a:
      switch (c) {
      case 's':
        nextst(GOT__Static_as);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Static_a
      break;

    case GOT__Static_as:
      switch (c) {
      case 's':
        nextst(GOT__Static_ass);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Static_as
      break;

    case GOT__Static_ass:
      switch (c) {
      case 'e':
        nextst(GOT__Static_asse);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Static_ass
      break;

    case GOT__Static_asse:
      switch (c) {
      case 'r':
        nextst(GOT__Static_asser);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Static_asse
      break;

    case GOT__Static_asser:
      switch (c) {
      case 't':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::_STATIC_ASSERT, 14);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Static_asser
      break;

    case GOT__T:
      switch (c) {
      case 'h':
        nextst(GOT__Th);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__T
      break;

    case GOT__Th:
      switch (c) {
      case 'r':
        nextst(GOT__Thr);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Th
      break;

    case GOT__Thr:
      switch (c) {
      case 'e':
        nextst(GOT__Thre);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Thr
      break;

    case GOT__Thre:
      switch (c) {
      case 'a':
        nextst(GOT__Threa);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Thre
      break;

    case GOT__Threa:
      switch (c) {
      case 'd':
        nextst(GOT__Thread);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Threa
      break;

    case GOT__Thread:
      switch (c) {
      case '_':
        nextst(GOT__Thread_);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Thread
      break;

    case GOT__Thread_:
      switch (c) {
      case 'l':
        nextst(GOT__Thread_l);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Thread_
      break;

    case GOT__Thread_l:
      switch (c) {
      case 'o':
        nextst(GOT__Thread_lo);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Thread_l
      break;

    case GOT__Thread_lo:
      switch (c) {
      case 'c':
        nextst(GOT__Thread_loc);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Thread_lo
      break;

    case GOT__Thread_loc:
      switch (c) {
      case 'a':
        nextst(GOT__Thread_loca);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Thread_loc
      break;

    case GOT__Thread_loca:
      switch (c) {
      case 'l':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::_THREAD_LOCAL, 13);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT__Thread_loca
      break;

    case GOT_a:
      switch (c) {
      case 'l':
        nextst(GOT_al);
        break;
      case 'u':
        nextst(GOT_au);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_a
      break;

    case GOT_al:
      switch (c) {
      case 'i':
        nextst(GOT_ali);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_al
      break;

    case GOT_ali:
      switch (c) {
      case 'g':
        nextst(GOT_alig);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_ali
      break;

    case GOT_alig:
      switch (c) {
      case 'n':
        nextst(GOT_align);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_alig
      break;

    case GOT_align:
      switch (c) {
      case 'a':
        nextst(GOT_aligna);
        break;
      case 'o':
        nextst(GOT_aligno);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_align
      break;

    case GOT_aligna:
      switch (c) {
      case 's':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::ALIGNAS, 7);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_aligna
      break;

    case GOT_aligno:
      switch (c) {
      case 'f':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::ALIGNOF, 7);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_aligno
      break;

    case GOT_au:
      switch (c) {
      case 't':
        nextst(GOT_aut);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_au
      break;

    case GOT_aut:
      switch (c) {
      case 'o':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::AUTO, 4);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_aut
      break;

    case GOT_b:
      switch (c) {
      case 'o':
        nextst(GOT_bo);
        break;
      case 'r':
        nextst(GOT_br);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_b
      break;

    case GOT_bo:
      switch (c) {
      case 'o':
        nextst(GOT_boo);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_bo
      break;

    case GOT_boo:
      switch (c) {
      case 'l':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::BOOL, 4);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_boo
      break;

    case GOT_br:
      switch (c) {
      case 'e':
        nextst(GOT_bre);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_br
      break;

    case GOT_bre:
      switch (c) {
      case 'a':
        nextst(GOT_brea);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_bre
      break;

    case GOT_brea:
      switch (c) {
      case 'k':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::BREAK, 5);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_brea
      break;

    case GOT_c:
      switch (c) {
      case 'a':
        nextst(GOT_ca);
        break;
      case 'h':
        nextst(GOT_ch);
        break;
      case 'o':
        nextst(GOT_co);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_c
      break;

    case GOT_ca:
      switch (c) {
      case 's':
        nextst(GOT_cas);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_ca
      break;

    case GOT_cas:
      switch (c) {
      case 'e':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::CASE, 4);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_cas
      break;

    case GOT_ch:
      switch (c) {
      case 'a':
        nextst(GOT_cha);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_ch
      break;

    case GOT_cha:
      switch (c) {
      case 'r':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::CHAR, 4);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_cha
      break;

    case GOT_co:
      switch (c) {
      case 'n':
        nextst(GOT_con);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_co
      break;

    case GOT_con:
      switch (c) {
      case 's':
        nextst(GOT_cons);
        break;
      case 't':
        nextst(GOT_cont);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_con
      break;

    case GOT_cons:
      switch (c) {
      case 't':
        if (sr_->peek() == 'e') {
          advancest(GOT_conste);
        } else if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::CONST, 5);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_cons
      break;

    case GOT_conste:
      switch (c) {
      case 'x':
        nextst(GOT_constex);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_conste
      break;

    case GOT_constex:
      switch (c) {
      case 'p':
        nextst(GOT_constexp);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_constex
      break;

    case GOT_constexp:
      switch (c) {
      case 'r':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::CONSTEXPR, 9);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_constexp
      break;

    case GOT_cont:
      switch (c) {
      case 'i':
        nextst(GOT_conti);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_cont
      break;

    case GOT_conti:
      switch (c) {
      case 'n':
        nextst(GOT_contin);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_conti
      break;

    case GOT_contin:
      switch (c) {
      case 'u':
        nextst(GOT_continu);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_contin
      break;

    case GOT_continu:
      switch (c) {
      case 'e':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::CONTINUE, 8);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_continu
      break;

    case GOT_d:
      switch (c) {
      case 'e':
        nextst(GOT_de);
        break;
      case 'o':
        if (sr_->peek() == 'u') {
          advancest(GOT_dou);
        } else if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::DO, 2);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_d
      break;

    case GOT_de:
      switch (c) {
      case 'f':
        nextst(GOT_def);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_de
      break;

    case GOT_def:
      switch (c) {
      case 'a':
        nextst(GOT_defa);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_def
      break;

    case GOT_defa:
      switch (c) {
      case 'u':
        nextst(GOT_defau);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_defa
      break;

    case GOT_defau:
      switch (c) {
      case 'l':
        nextst(GOT_defaul);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_defau
      break;

    case GOT_defaul:
      switch (c) {
      case 't':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::DEFAULT, 7);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_defaul
      break;

    case GOT_dou:
      switch (c) {
      case 'b':
        nextst(GOT_doub);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_dou
      break;

    case GOT_doub:
      switch (c) {
      case 'l':
        nextst(GOT_doubl);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_doub
      break;

    case GOT_doubl:
      switch (c) {
      case 'e':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::DOUBLE, 6);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_doubl
      break;

    case GOT_e:
      switch (c) {
      case 'l':
        nextst(GOT_el);
        break;
      case 'n':
        nextst(GOT_en);
        break;
      case 'x':
        nextst(GOT_ex);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_e
      break;

    case GOT_el:
      switch (c) {
      case 's':
        nextst(GOT_els);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_el
      break;

    case GOT_els:
      switch (c) {
      case 'e':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::ELSE, 4);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_els
      break;

    case GOT_en:
      switch (c) {
      case 'u':
        nextst(GOT_enu);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_en
      break;

    case GOT_enu:
      switch (c) {
      case 'm':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::ENUM, 4);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_enu
      break;

    case GOT_ex:
      switch (c) {
      case 't':
        nextst(GOT_ext);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_ex
      break;

    case GOT_ext:
      switch (c) {
      case 'e':
        nextst(GOT_exte);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_ext
      break;

    case GOT_exte:
      switch (c) {
      case 'r':
        nextst(GOT_exter);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_exte
      break;

    case GOT_exter:
      switch (c) {
      case 'n':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::EXTERN, 6);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_exter
      break;

    case GOT_f:
      switch (c) {
      case 'a':
        nextst(GOT_fa);
        break;
      case 'l':
        nextst(GOT_fl);
        break;
      case 'o':
        nextst(GOT_fo);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_f
      break;

    case GOT_fa:
      switch (c) {
      case 'l':
        nextst(GOT_fal);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_fa
      break;

    case GOT_fal:
      switch (c) {
      case 's':
        nextst(GOT_fals);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_fal
      break;

    case GOT_fals:
      switch (c) {
      case 'e':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::FALSE, 5);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_fals
      break;

    case GOT_fl:
      switch (c) {
      case 'o':
        nextst(GOT_flo);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_fl
      break;

    case GOT_flo:
      switch (c) {
      case 'a':
        nextst(GOT_floa);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_flo
      break;

    case GOT_floa:
      switch (c) {
      case 't':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::FLOAT, 5);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_floa
      break;

    case GOT_fo:
      switch (c) {
      case 'r':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::FOR, 3);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_fo
      break;

    case GOT_g:
      switch (c) {
      case 'o':
        nextst(GOT_go);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_g
      break;

    case GOT_go:
      switch (c) {
      case 't':
        nextst(GOT_got);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_go
      break;

    case GOT_got:
      switch (c) {
      case 'o':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::GOTO, 4);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_got
      break;

    case GOT_i:
      switch (c) {
      case 'f':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::IF, 2);
        }
        break;
      case 'n':
        nextst(GOT_in);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_i
      break;

    case GOT_in:
      switch (c) {
      case 'l':
        nextst(GOT_inl);
        break;
      case 't':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::INT, 3);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_in
      break;

    case GOT_inl:
      switch (c) {
      case 'i':
        nextst(GOT_inli);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_inl
      break;

    case GOT_inli:
      switch (c) {
      case 'n':
        nextst(GOT_inlin);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_inli
      break;

    case GOT_inlin:
      switch (c) {
      case 'e':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::INLINE, 6);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_inlin
      break;

    case GOT_l:
      switch (c) {
      case 'o':
        nextst(GOT_lo);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_l
      break;

    case GOT_lo:
      switch (c) {
      case 'n':
        nextst(GOT_lon);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_lo
      break;

    case GOT_lon:
      switch (c) {
      case 'g':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::LONG, 4);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_lon
      break;

    case GOT_n:
      switch (c) {
      case 'u':
        nextst(GOT_nu);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_n
      break;

    case GOT_nu:
      switch (c) {
      case 'l':
        nextst(GOT_nul);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_nu
      break;

    case GOT_nul:
      switch (c) {
      case 'l':
        nextst(GOT_null);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_nul
      break;

    case GOT_null:
      switch (c) {
      case 'p':
        nextst(GOT_nullp);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_null
      break;

    case GOT_nullp:
      switch (c) {
      case 't':
        nextst(GOT_nullpt);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_nullp
      break;

    case GOT_nullpt:
      switch (c) {
      case 'r':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::NULLPTR, 7);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_nullpt
      break;

    case GOT_r:
      switch (c) {
      case 'e':
        nextst(GOT_re);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_r
      break;

    case GOT_re:
      switch (c) {
      case 'g':
        nextst(GOT_reg);
        break;
      case 's':
        nextst(GOT_res);
        break;
      case 't':
        nextst(GOT_ret);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_re
      break;

    case GOT_reg:
      switch (c) {
      case 'i':
        nextst(GOT_regi);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_reg
      break;

    case GOT_regi:
      switch (c) {
      case 's':
        nextst(GOT_regis);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_regi
      break;

    case GOT_regis:
      switch (c) {
      case 't':
        nextst(GOT_regist);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_regis
      break;

    case GOT_regist:
      switch (c) {
      case 'e':
        nextst(GOT_registe);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_regist
      break;

    case GOT_registe:
      switch (c) {
      case 'r':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::REGISTER, 8);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_registe
      break;

    case GOT_res:
      switch (c) {
      case 't':
        nextst(GOT_rest);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_res
      break;

    case GOT_rest:
      switch (c) {
      case 'r':
        nextst(GOT_restr);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_rest
      break;

    case GOT_restr:
      switch (c) {
      case 'i':
        nextst(GOT_restri);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_restr
      break;

    case GOT_restri:
      switch (c) {
      case 'c':
        nextst(GOT_restric);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_restri
      break;

    case GOT_restric:
      switch (c) {
      case 't':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::RESTRICT, 8);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_restric
      break;

    case GOT_ret:
      switch (c) {
      case 'u':
        nextst(GOT_retu);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_ret
      break;

    case GOT_retu:
      switch (c) {
      case 'r':
        nextst(GOT_retur);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_retu
      break;

    case GOT_retur:
      switch (c) {
      case 'n':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::RETURN, 6);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_retur
      break;

    case GOT_s:
      switch (c) {
      case 'h':
        nextst(GOT_sh);
        break;
      case 'i':
        nextst(GOT_si);
        break;
      case 't':
        nextst(GOT_st);
        break;
      case 'w':
        nextst(GOT_sw);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_s
      break;

    case GOT_sh:
      switch (c) {
      case 'o':
        nextst(GOT_sho);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_sh
      break;

    case GOT_sho:
      switch (c) {
      case 'r':
        nextst(GOT_shor);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_sho
      break;

    case GOT_shor:
      switch (c) {
      case 't':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::SHORT, 5);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_shor
      break;

    case GOT_si:
      switch (c) {
      case 'g':
        nextst(GOT_sig);
        break;
      case 'z':
        nextst(GOT_siz);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_si
      break;

    case GOT_sig:
      switch (c) {
      case 'n':
        nextst(GOT_sign);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_sig
      break;

    case GOT_sign:
      switch (c) {
      case 'e':
        nextst(GOT_signe);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_sign
      break;

    case GOT_signe:
      switch (c) {
      case 'd':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::SIGNED, 6);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_signe
      break;

    case GOT_siz:
      switch (c) {
      case 'e':
        nextst(GOT_size);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_siz
      break;

    case GOT_size:
      switch (c) {
      case 'o':
        nextst(GOT_sizeo);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_size
      break;

    case GOT_sizeo:
      switch (c) {
      case 'f':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::SIZEOF, 6);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_sizeo
      break;

    case GOT_st:
      switch (c) {
      case 'a':
        nextst(GOT_sta);
        break;
      case 'r':
        nextst(GOT_str);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_st
      break;

    case GOT_sta:
      switch (c) {
      case 't':
        nextst(GOT_stat);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_sta
      break;

    case GOT_stat:
      switch (c) {
      case 'i':
        nextst(GOT_stati);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_stat
      break;

    case GOT_stati:
      switch (c) {
      case 'c':
        if (sr_->peek() == '_') {
          advancest(GOT_static_);
        } else if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::STATIC, 6);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_stati
      break;

    case GOT_static_:
      switch (c) {
      case 'a':
        nextst(GOT_static_a);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_static_
      break;

    case GOT_static_a:
      switch (c) {
      case 's':
        nextst(GOT_static_as);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_static_a
      break;

    case GOT_static_as:
      switch (c) {
      case 's':
        nextst(GOT_static_ass);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_static_as
      break;

    case GOT_static_ass:
      switch (c) {
      case 'e':
        nextst(GOT_static_asse);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_static_ass
      break;

    case GOT_static_asse:
      switch (c) {
      case 'r':
        nextst(GOT_static_asser);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_static_asse
      break;

    case GOT_static_asser:
      switch (c) {
      case 't':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::STATIC_ASSERT, 13);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_static_asser
      break;

    case GOT_str:
      switch (c) {
      case 'u':
        nextst(GOT_stru);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_str
      break;

    case GOT_stru:
      switch (c) {
      case 'c':
        nextst(GOT_struc);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_stru
      break;

    case GOT_struc:
      switch (c) {
      case 't':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::STRUCT, 6);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_struc
      break;

    case GOT_sw:
      switch (c) {
      case 'i':
        nextst(GOT_swi);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_sw
      break;

    case GOT_swi:
      switch (c) {
      case 't':
        nextst(GOT_swit);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_swi
      break;

    case GOT_swit:
      switch (c) {
      case 'c':
        nextst(GOT_switc);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_swit
      break;

    case GOT_switc:
      switch (c) {
      case 'h':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::SWITCH, 6);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_switc
      break;

    case GOT_t:
      switch (c) {
      case 'h':
        nextst(GOT_th);
        break;
      case 'r':
        nextst(GOT_tr);
        break;
      case 'y':
        nextst(GOT_ty);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_t
      break;

    case GOT_th:
      switch (c) {
      case 'r':
        nextst(GOT_thr);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_th
      break;

    case GOT_thr:
      switch (c) {
      case 'e':
        nextst(GOT_thre);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_thr
      break;

    case GOT_thre:
      switch (c) {
      case 'a':
        nextst(GOT_threa);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_thre
      break;

    case GOT_threa:
      switch (c) {
      case 'd':
        nextst(GOT_thread);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_threa
      break;

    case GOT_thread:
      switch (c) {
      case '_':
        nextst(GOT_thread_);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_thread
      break;

    case GOT_thread_:
      switch (c) {
      case 'l':
        nextst(GOT_thread_l);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_thread_
      break;

    case GOT_thread_l:
      switch (c) {
      case 'o':
        nextst(GOT_thread_lo);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_thread_l
      break;

    case GOT_thread_lo:
      switch (c) {
      case 'c':
        nextst(GOT_thread_loc);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_thread_lo
      break;

    case GOT_thread_loc:
      switch (c) {
      case 'a':
        nextst(GOT_thread_loca);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_thread_loc
      break;

    case GOT_thread_loca:
      switch (c) {
      case 'l':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::THREAD_LOCAL, 12);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_thread_loca
      break;

    case GOT_tr:
      switch (c) {
      case 'u':
        nextst(GOT_tru);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_tr
      break;

    case GOT_tru:
      switch (c) {
      case 'e':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::TRUE, 4);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_tru
      break;

    case GOT_ty:
      switch (c) {
      case 'p':
        nextst(GOT_typ);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_ty
      break;

    case GOT_typ:
      switch (c) {
      case 'e':
        nextst(GOT_type);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_typ
      break;

    case GOT_type:
      switch (c) {
      case 'd':
        nextst(GOT_typed);
        break;
      case 'o':
        nextst(GOT_typeo);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_type
      break;

    case GOT_typed:
      switch (c) {
      case 'e':
        nextst(GOT_typede);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_typed
      break;

    case GOT_typede:
      switch (c) {
      case 'f':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::TYPEDEF, 7);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_typede
      break;

    case GOT_typeo:
      switch (c) {
      case 'f':
        if (sr_->peek() == '_') {
          advancest(GOT_typeof_);
        } else if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::TYPEOF, 6);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_typeo
      break;

    case GOT_typeof_:
      switch (c) {
      case 'u':
        nextst(GOT_typeof_u);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_typeof_
      break;

    case GOT_typeof_u:
      switch (c) {
      case 'n':
        nextst(GOT_typeof_un);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_typeof_u
      break;

    case GOT_typeof_un:
      switch (c) {
      case 'q':
        nextst(GOT_typeof_unq);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_typeof_un
      break;

    case GOT_typeof_unq:
      switch (c) {
      case 'u':
        nextst(GOT_typeof_unqu);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_typeof_unq
      break;

    case GOT_typeof_unqu:
      switch (c) {
      case 'a':
        nextst(GOT_typeof_unqua);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_typeof_unqu
      break;

    case GOT_typeof_unqua:
      switch (c) {
      case 'l':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::TYPEOF_UNQUAL, 13);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_typeof_unqua
      break;

    case GOT_u: {
      const int peek = sr_->peek();
      switch (c) {
      case 'n':
        nextst(GOT_un);
        break;
      case '8':
        if (peek == '\'') {
          advancest(GOT_CHAR_CONST_START);
        } else if (peek == '\"') {
          advancest(GOT_STRING_LIT_START);
        } else {
          nextst(GOT_IDENT);
        }
        break;
      case '\'':
        nextst(GOT_CHAR_CONST_START);
        break;
      case '\"':
        nextst(GOT_STRING_LIT_START);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_u
    } break;

    case GOT_U:
      switch (c) {
      case '\'':
        nextst(GOT_CHAR_CONST_START);
        break;
      case '\"':
        nextst(GOT_STRING_LIT_START);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_U
      break;

    case GOT_L:
      switch (c) {
      case '\'':
        nextst(GOT_CHAR_CONST_START);
        break;
      case '\"':
        nextst(GOT_STRING_LIT_START);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_L
      break;

    case GOT_un:
      switch (c) {
      case 'i':
        nextst(GOT_uni);
        break;
      case 's':
        nextst(GOT_uns);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_un
      break;

    case GOT_uni:
      switch (c) {
      case 'o':
        nextst(GOT_unio);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_uni
      break;

    case GOT_unio:
      switch (c) {
      case 'n':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::UNION, 5);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_unio
      break;

    case GOT_uns:
      switch (c) {
      case 'i':
        nextst(GOT_unsi);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_uns
      break;

    case GOT_unsi:
      switch (c) {
      case 'g':
        nextst(GOT_unsig);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_unsi
      break;

    case GOT_unsig:
      switch (c) {
      case 'n':
        nextst(GOT_unsign);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_unsig
      break;

    case GOT_unsign:
      switch (c) {
      case 'e':
        nextst(GOT_unsigne);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_unsign
      break;

    case GOT_unsigne:
      switch (c) {
      case 'd':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::UNSIGNED, 8);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_unsigne
      break;

    case GOT_v:
      switch (c) {
      case 'o':
        nextst(GOT_vo);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_v
      break;

    case GOT_vo:
      switch (c) {
      case 'i':
        nextst(GOT_voi);
        break;
      case 'l':
        nextst(GOT_vol);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_vo
      break;

    case GOT_voi:
      switch (c) {
      case 'd':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::VOID, 4);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_voi
      break;

    case GOT_vol:
      switch (c) {
      case 'a':
        nextst(GOT_vola);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_vol
      break;

    case GOT_vola:
      switch (c) {
      case 't':
        nextst(GOT_volat);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_vola
      break;

    case GOT_volat:
      switch (c) {
      case 'i':
        nextst(GOT_volati);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_volat
      break;

    case GOT_volati:
      switch (c) {
      case 'l':
        nextst(GOT_volatil);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_volati
      break;

    case GOT_volatil:
      switch (c) {
      case 'e':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::VOLATILE, 8);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_volatil
      break;

    case GOT_w:
      switch (c) {
      case 'h':
        nextst(GOT_wh);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_w
      break;

    case GOT_wh:
      switch (c) {
      case 'i':
        nextst(GOT_whi);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_wh
      break;

    case GOT_whi:
      switch (c) {
      case 'l':
        nextst(GOT_whil);
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_whi
      break;

    case GOT_whil:
      switch (c) {
      case 'e':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::WHILE, 5);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_whil
      break;

    case GOT_INT_LITERAL: {
      const int isdig = std::isdigit(c);
      const char peek = sr_->peek();
      if (isdig && std::isdigit(peek)) {
        // Eat c and remain in this state.
      } else if (isdig) {
        if (is_int_suffix_start(peek)) {
          nextst(GOT_INT_SUFFIX_START);
        } else if (peek == 'e' || peek == 'E') {
          advancest(GOT_FLOAT_CONST_e);
        } else if (peek == '.') {
          advancest(GOT_FLOAT_CONST_DOT);
        } else if (peek == '\'') {
          // Eat peek and remain in this state.
          advance();
        } else {
          r(Token::INTEGER_LIT, lex.length());
        }
      } else if (is_int_suffix_start(c)) {
        holdst(GOT_INT_SUFFIX_START);
      } else if (c == 'e' || c == 'E') {
        nextst(GOT_FLOAT_CONST_e);
      } else if (c == '.') {
        nextst(GOT_FLOAT_CONST_DOT);
      } else if (c == '\'') {
        // Eat c and remain in this state.
      } else {
        backup(c);
        r(Token::INTEGER_LIT, lex.length());
      }
    } break;

    case GOT_OCT_LITERAL: {
      const int isoct = c >= '0' && c <= '7';
      const char peek = sr_->peek();
      if (c == '\'' || (isoct && (peek >= '0' && peek <= '7'))) {
        // Eat c and remain in this state.
      } else if (isoct) {
        if (is_int_suffix_start(peek)) {
          nextst(GOT_INT_SUFFIX_START);
        } else if (peek == 'e' || peek == 'E') {
          advancest(GOT_FLOAT_CONST_e);
        } else if (peek == '.') {
          advancest(GOT_FLOAT_CONST_DOT);
        } else if (peek == '\'') {
          advance();
        } else if (std::isdigit(peek)) {
          nextst(GOT_INT_LITERAL);
        } else {
          r(Token::INTEGER_LIT, lex.length());
        }
      } else if (is_int_suffix_start(c)) {
        holdst(GOT_INT_SUFFIX_START);
      } else if (c == 'e' || c == 'E') {
        nextst(GOT_FLOAT_CONST_e);
      } else if (c == '.') {
        nextst(GOT_FLOAT_CONST_DOT);
      } else {
        backup(c);
        r(Token::INTEGER_LIT, lex.length());
      }
    } break;

    case GOT_0x:
      if (std::isxdigit(c)) {
        holdst(GOT_HEX_LITERAL);
      } else if (c == '.') {
        nextst(GOT_0x_DOT);
      } else {
        print_error(
            std::cerr,
            "Hexadecimal prefix must be followed by a hexadecimal digit.\n");
        backup(c);
        rinvalid();
      }
      break;

    case GOT_0x_DOT:
      if (std::isxdigit(c)) {
        nextst(GOT_HEX_CONST_DOT_XDIGIT);
      } else {
        print_error(std::cerr, "Missing digits in hexadecimal constant.\n");
        backup(c);
        rinvalid();
      }
      break;

    case GOT_HEX_LITERAL: {
      const int isxdig = std::isxdigit(c);
      const char peek = sr_->peek();
      if (isxdig && std::isxdigit(peek)) {
        // Eat c and remain in this state.
      } else if (isxdig) {
        if (is_int_suffix_start(peek)) {
          nextst(GOT_INT_SUFFIX_START);
        } else if (peek == '.') {
          advancest(GOT_HEX_CONST_DOT);
        } else if (peek == 'p' || peek == 'P') {
          advancest(GOT_HEX_CONST_p);
        } else if (peek == '\'') {
          advance();
        } else {
          r(Token::INTEGER_LIT, lex.length());
        }
      } else if (is_int_suffix_start(c)) {
        holdst(GOT_INT_SUFFIX_START);
      } else if (c == '.') {
        nextst(GOT_HEX_CONST_DOT);
      } else if (c == 'p' || c == 'P') {
        nextst(GOT_HEX_CONST_p);
      } else if (c == '\'') {
        // Eat c and remain in this state.
      } else {
        backup(c);
        r(Token::INTEGER_LIT, lex.length());
      }
    } break;

    case GOT_CHAR_CONST_START:
      // So far, we have scanned a character prefix (uUL), if any,
      // and a '\''
      switch (c) {
      case '\'':
        print_error(std::cerr, "Character constant cannot be empty.\n");
        rinvalid();
      case '\n':
        print_error(std::cerr, "Unterminated character constant detected.\n");
        backup(c);
        rinvalid();
      case '\\':
        nextst(GOT_CHAR_CONST_CONT);
        pushst(GOT_ESCAPE_SEQUENCE);
        break;
      default:
        nextst(GOT_CHAR_CONST_CONT);
        break;
      } // switch (c) for GOT_CHAR_CONST_START
      break;

    case GOT_CHAR_CONST_CONT:
      switch (c) {
      case '\'':
        r(Token::INTEGER_LIT, lex.length());
      case EOF:
      case '\n':
        print_error(std::cerr, "Unterminated character constant detected.\n");
        backup(c);
        rinvalid();
      case '\\':
        pushst(GOT_ESCAPE_SEQUENCE);
        break;
      default:
        // Eat c and remain in this state.
        break;
      } // switch (c) for GOT_CHAR_CONST_CONT
      break;

    case GOT_FLOAT_CONST_DOT: {
      // So far, we have scanned {D}+"."
      const int isdig = std::isdigit(c);
      const char *suffix = "fFlLdD";
      if (isdig) {
        nextst(GOT_FLOAT_CONST_DOT_DIGIT);
      } else if (c == 'e' || c == 'E') {
        nextst(GOT_FLOAT_CONST_e);
      } else if (std::strchr(suffix, c)) {
        holdst(GOT_FLOAT_CONST_e_SIGN_DIG);
      } else {
        backup(c);
        r(Token::FLOAT_LIT, lex.length());
      }
    } break;

    case GOT_FLOAT_CONST_DOT_DIGIT:
      // So far, we have scanned {D}*"."{D}
      switch (c) {
      case 'e':
      case 'E':
        nextst(GOT_FLOAT_CONST_e);
        break;
      case 'f':
      case 'F':
      case 'l':
      case 'L':
      case 'd':
      case 'D':
        holdst(GOT_FLOAT_CONST_e_SIGN_DIG);
        break;
      default:
        if (std::isdigit(c)) {
          // Eat c and remain in this state.
        } else {
          backup(c);
          r(Token::FLOAT_LIT, lex.length());
        }
        break;
      } // switch (c) for GOT_FLOAT_CONST_DOT_DIGIT
      break;

    case GOT_FLOAT_CONST_e:
      switch (c) {
      case '-':
      case '+':
        nextst(GOT_FLOAT_CONST_e_SIGN);
        break;
      default:
        if (std::isdigit(c)) {
          nextst(GOT_FLOAT_CONST_e_SIGN_DIG);
        } else {
          print_error(std::cerr,
                      "Floating point constant missing exponent digit(s).\n");
          backup(c);
          rinvalid();
        }
        break;
      } // switch (c) for GOT_FLOAT_CONST_e
      break;

    case GOT_FLOAT_CONST_e_SIGN:
      if (std::isdigit(c)) {
        nextst(GOT_FLOAT_CONST_e_SIGN_DIG);
      } else {
        print_error(std::cerr,
                    "Floating point constant missing exponent digit(s).\n");
        backup(c);
        rinvalid();
      }
      break;

    case GOT_FLOAT_CONST_e_SIGN_DIG:
      // So far, we have scanned {D}+{E}[+-]{D}
      switch (c) {
      case 'f':
      case 'F':
      case 'l':
      case 'L':
        r(Token::FLOAT_LIT, lex.length());
      case 'd':
        nextst(GOT_FLOAT_CONST_e_SUFd);
        break;
      case 'D':
        nextst(GOT_FLOAT_CONST_e_SUFD);
        break;
      case '\'': // Eat c and remain in this state.
        break;
      default:
        if (std::isdigit(c)) {
          // Eat c and remain in this state.
        } else {
          backup(c);
          r(Token::FLOAT_LIT, lex.length());
        }
        break;
      } // switch (c) for GOT_FLOAT_CONST_e
      break;

    case GOT_FLOAT_CONST_e_SUFd:
      switch (c) {
      case 'f':
      case 'd':
      case 'l':
        r(Token::FLOAT_LIT, lex.length());
        break;
      default:
        print_error(std::cerr,
                    "Valid floating point constant suffixes are {df dd dl}.\n");
        backup(c);
        rinvalid();
        break;
      } // switch (c) for GOT_FLOAT_CONST_e_SUFd
      break;

    case GOT_FLOAT_CONST_e_SUFD:
      switch (c) {
      case 'F':
      case 'D':
      case 'L':
        r(Token::FLOAT_LIT, lex.length());
        break;
      default:
        print_error(std::cerr,
                    "Valid floating point constant suffixes are {DF DD DL}.\n");
        backup(c);
        rinvalid();
        break;
      } // switch (c) for GOT_FLOAT_CONST_e_SUFD
      break;

    case GOT_HEX_CONST_DOT: {
      // So far, we have scanned 0x{H}+"."
      const int isxdig = std::isxdigit(c);
      if (isxdig) {
        nextst(GOT_HEX_CONST_DOT_XDIGIT);
      } else if (c == 'p' || c == 'P') {
        nextst(GOT_HEX_CONST_p);
      } else {
        print_error(
            std::cerr,
            "Hexadecimal floating-point constant missing exponent field.\n");
        backup(c);
        rinvalid();
      }
    } break;

    case GOT_HEX_CONST_DOT_XDIGIT:
      // So far, we have scanned 0x{H}*"."{H}
      switch (c) {
      case 'p':
      case 'P':
        nextst(GOT_HEX_CONST_p);
        break;
      case '\'': // Eat c and remain in this state.
        break;
      default:
        if (std::isxdigit(c)) {
          // Eat c and remain in this state.
        } else {
          print_error(
              std::cerr,
              "Hexadecimal floating-point constant missing exponent field.\n");
          backup(c);
          rinvalid();
        }
        break;
      } // switch (c) for GOT_HEX_CONST_DOT_XDIGIT
      break;

    case GOT_HEX_CONST_p:
      switch (c) {
      case '-':
      case '+':
        nextst(GOT_HEX_CONST_p_SIGN);
        break;
      default:
        if (std::isdigit(c)) {
          nextst(GOT_HEX_CONST_p_SIGN_DIG);
        } else {
          print_error(std::cerr,
                      "Floating point constant missing exponent digit(s).\n");
          backup(c);
          rinvalid();
        }
        break;
      } // switch (c) for GOT_HEX_CONST_p
      break;

    case GOT_HEX_CONST_p_SIGN_DIG:
      // So far, we have scanned 0x{H}+{P}[+-]{D}
      switch (c) {
      case 'f':
      case 'F':
      case 'l':
      case 'L':
        r(Token::FLOAT_LIT, lex.length());
      default:
        if (std::isdigit(c)) {
          // Eat c and remain in this state.
        } else {
          backup(c);
          r(Token::FLOAT_LIT, lex.length());
        }
        break;
      } // switch (c) for GOT_FLOAT_CONST_e
      break;

    case GOT_HEX_CONST_p_SIGN:
      if (std::isdigit(c)) {
        nextst(GOT_HEX_CONST_p_SIGN_DIG);
      } else {
        print_error(std::cerr,
                    "Floating point constant missing exponent digit(s).\n");
        backup(c);
        rinvalid();
      }
      break;

    case GOT_BIN_CONST_START:
      switch (c) {
      case '0':
      case '1':
        nextst(GOT_BIN_CONST_CONT);
        break;
      default:
        print_error(std::cerr, "Binary constant must begin with a 0 or 1.\n");
        backup(c);
        rinvalid();
        break;
      } // switch (c) for GOT_BIN_CONST
      break;

    case GOT_BIN_CONST_CONT:
      if (is_int_suffix_start(c)) {
        holdst(GOT_INT_SUFFIX_START);
      } else if (c == '0' || c == '1' || c == '\'') {
        // Eat c and remain in this state.
      } else {
        backup(c);
        r(Token::INTEGER_LIT, lex.length());
      }
      break;

    case GOT_INT_SUFFIX_START:
      switch (c) {
      case 'u':
        nextst(GOT_INT_SUFFIX_u);
        break;
      case 'U':
        nextst(GOT_INT_SUFFIX_U);
        break;
      case 'l':
        nextst(GOT_INT_SUFFIX_l);
        break;
      case 'L':
        nextst(GOT_INT_SUFFIX_L);
        break;
      case 'w':
        nextst(GOT_INT_SUFFIX_w);
        break;
      case 'W':
        nextst(GOT_INT_SUFFIX_W);
        break;
      } // switch (c) for GOT_INT_SUFFIX_START
      break;

    case GOT_INT_SUFFIX_u: { // ul ull uL uLL uwb uWB
      const int peek = sr_->peek();
      switch (c) {
      case 'l':
        if (peek == 'l')
          advance();
        r(Token::INTEGER_LIT, lex.length());
      case 'L':
        if (peek == 'L')
          advance();
        r(Token::INTEGER_LIT, lex.length());
      case 'w':
        if (peek == 'b') {
          advance();
          r(Token::INTEGER_LIT, lex.length());
        }
        break;
      case 'W':
        if (peek == 'B') {
          advance();
          r(Token::INTEGER_LIT, lex.length());
        }
        break;
      } // switch (c) for GOT_INT_SUFFIX_u
      print_error(std::cerr, "Invalid integer suffix after u.\n");
      rinvalid();
    } break;

    case GOT_INT_SUFFIX_U: { // Ul Ull UL ULL Uwb UWB
      const int peek = sr_->peek();
      switch (c) {
      case 'l':
        if (peek == 'l')
          advance();
        r(Token::INTEGER_LIT, lex.length());
      case 'L':
        if (peek == 'L')
          advance();
        r(Token::INTEGER_LIT, lex.length());
      case 'w':
        if (peek == 'b') {
          advance();
          r(Token::INTEGER_LIT, lex.length());
        }
        break;
      case 'W':
        if (peek == 'B') {
          advance();
          r(Token::INTEGER_LIT, lex.length());
        }
        break;
      } // switch (c) for GOT_INT_SUFFIX_U
      print_error(std::cerr, "Invalid integer suffix after U.\n");
      rinvalid();
    } break;

    case GOT_INT_SUFFIX_l: { // l lu lU ll llu llU
      const int peek = sr_->peek();
      switch (c) {
      case 'l':
        if (peek == 'u' || peek == 'U')
          advance();
        r(Token::INTEGER_LIT, lex.length());
      case 'u':
      case 'U':
        r(Token::INTEGER_LIT, lex.length());
      default:
        backup(c);
        r(Token::INTEGER_LIT, lex.length());
      } // switch (c) for GOT_INT_SUFFIX_l
    } break;

    case GOT_INT_SUFFIX_L: { // L Lu LU LL LLu LLU
      const int peek = sr_->peek();
      switch (c) {
      case 'L':
        if (peek == 'u' || peek == 'U')
          advance();
        r(Token::INTEGER_LIT, lex.length());
      case 'u':
      case 'U':
        r(Token::INTEGER_LIT, lex.length());
      default:
        backup(c);
        r(Token::INTEGER_LIT, lex.length());
      } // switch (c) for GOT_INT_SUFFIX_L
    } break;

    case GOT_INT_SUFFIX_w: { // wb wbu wbU
      const int peek = sr_->peek();
      switch (c) {
      case 'b':
        if (peek == 'u' || peek == 'U')
          advance();
        r(Token::INTEGER_LIT, lex.length());
      default:
        print_error(std::cerr, "Invalid integer suffix after w.\n");
        rinvalid();
      } // switch (c) for GOT_INT_SUFFIX_w
    } break;

    case GOT_INT_SUFFIX_W: { // WB WBu WBU
      const int peek = sr_->peek();
      switch (c) {
      case 'B':
        if (peek == 'u' || peek == 'U')
          advance();
        r(Token::INTEGER_LIT, lex.length());
      default:
        print_error(std::cerr, "Invalid integer suffix after W.\n");
        rinvalid();
      } // switch (c) for GOT_INT_SUFFIX_W
    } break;

    case GOT_STRING_LIT_START: // so far we have {encoding-prefix}?["]
      switch (c) {
      case '\"':
        r(Token::STRING_LIT, lex.length());
      case EOF:
      case '\n':
        print_error(std::cerr, "Unterminated string literal detected.\n");
        backup(c);
        rinvalid();
      case '\\':
        pushst(GOT_ESCAPE_SEQUENCE);
        break;
      default:
        // Eat c and remain in this state.
        break;
      } // switch (c) for GOT_STRING_LIT_START
      break;

    case GOT_ESCAPE_SEQUENCE:
      if (is_simple_escape_sequence(c)) {
        popst();
      } else if (c >= '0' && c <= '7') {
        nextst(GOT_ESCAPE_SEQUENCE_BS_OCT1);
      } else if (c == 'x') {
        nextst(GOT_ESCAPE_SEQUENCE_BS_x);
      } else if (c == 'u') {
        nextst(GOT_ESCAPE_SEQUENCE_BS_u0);
      } else if (c == 'U') {
        nextst(GOT_ESCAPE_SEQUENCE_BS_U0);
      } else {
        print_error(std::cerr, "Invalid escape sequence.\n");
        rinvalid();
      }
      break;

    case GOT_ESCAPE_SEQUENCE_BS_OCT1:
      if (c >= '0' && c <= '7') {
        nextst(GOT_ESCAPE_SEQUENCE_BS_OCT2);
      } else {
        pop_holdst();
      }
      break;

    case GOT_ESCAPE_SEQUENCE_BS_OCT2:
      if (c >= '0' && c <= '7') {
        popst();
      } else {
        pop_holdst();
      }
      break;

    case GOT_ESCAPE_SEQUENCE_BS_x:
      if (std::isxdigit(c)) {
        nextst(GOT_ESCAPE_SEQUENCE_BS_x_XDIGIT);
      } else {
        print_error(std::cerr,
                    "Missing digits in hexadecimal escape sequence.\n");
        rinvalid();
      }
      break;

    case GOT_ESCAPE_SEQUENCE_BS_x_XDIGIT:
      if (std::isxdigit(c)) {
        // Eat c and remain in this state.
      } else {
        pop_holdst();
      }
      break;

    case GOT_ESCAPE_SEQUENCE_BS_u0:
      if (std::isxdigit(c)) {
        nextst(GOT_ESCAPE_SEQUENCE_BS_u1);
      } else {
        print_error(std::cerr,
                    "universal character name must have the form \\uhhhh.\n");
        rinvalid();
      }
      break;

    case GOT_ESCAPE_SEQUENCE_BS_u1:
      if (std::isxdigit(c)) {
        nextst(GOT_ESCAPE_SEQUENCE_BS_u2);
      } else {
        print_error(std::cerr,
                    "universal character name must have the form \\uhhhh.\n");
        rinvalid();
      }
      break;

    case GOT_ESCAPE_SEQUENCE_BS_u2:
      if (std::isxdigit(c)) {
        nextst(GOT_ESCAPE_SEQUENCE_BS_u3);
      } else {
        print_error(std::cerr,
                    "universal character name must have the form \\uhhhh.\n");
        rinvalid();
      }
      break;

    case GOT_ESCAPE_SEQUENCE_BS_u3:
      if (std::isxdigit(c)) {
        popst();
      } else {
        print_error(std::cerr,
                    "universal character name must have the form \\uhhhh.\n");
        rinvalid();
      }
      break;

    case GOT_ESCAPE_SEQUENCE_BS_U0:
      if (std::isxdigit(c)) {
        nextst(GOT_ESCAPE_SEQUENCE_BS_U1);
      } else {
        print_error(
            std::cerr,
            "Universal character name must have the form \\Uhhhhhhhh.\n");
        rinvalid();
      }
      break;

    case GOT_ESCAPE_SEQUENCE_BS_U1:
      if (std::isxdigit(c)) {
        nextst(GOT_ESCAPE_SEQUENCE_BS_U2);
      } else {
        print_error(
            std::cerr,
            "Universal character name must have the form \\Uhhhhhhhh.\n");
        rinvalid();
      }
      break;

    case GOT_ESCAPE_SEQUENCE_BS_U2:
      if (std::isxdigit(c)) {
        nextst(GOT_ESCAPE_SEQUENCE_BS_U3);
      } else {
        print_error(
            std::cerr,
            "Universal character name must have the form \\Uhhhhhhhh.\n");
        rinvalid();
      }
      break;

    case GOT_ESCAPE_SEQUENCE_BS_U3:
      if (std::isxdigit(c)) {
        nextst(GOT_ESCAPE_SEQUENCE_BS_U4);
      } else {
        print_error(
            std::cerr,
            "Universal character name must have the form \\Uhhhhhhhh.\n");
        rinvalid();
      }
      break;

    case GOT_ESCAPE_SEQUENCE_BS_U4:
      if (std::isxdigit(c)) {
        nextst(GOT_ESCAPE_SEQUENCE_BS_U5);
      } else {
        print_error(
            std::cerr,
            "Universal character name must have the form \\Uhhhhhhhh.\n");
        rinvalid();
      }
      break;

    case GOT_ESCAPE_SEQUENCE_BS_U5:
      if (std::isxdigit(c)) {
        nextst(GOT_ESCAPE_SEQUENCE_BS_U6);
      } else {
        print_error(
            std::cerr,
            "Universal character name must have the form \\Uhhhhhhhh.\n");
        rinvalid();
      }
      break;

    case GOT_ESCAPE_SEQUENCE_BS_U6:
      if (std::isxdigit(c)) {
        nextst(GOT_ESCAPE_SEQUENCE_BS_U7);
      } else {
        print_error(
            std::cerr,
            "Universal character name must have the form \\Uhhhhhhhh.\n");
        rinvalid();
      }
      break;

    case GOT_ESCAPE_SEQUENCE_BS_U7:
      if (std::isxdigit(c)) {
        popst();
      } else {
        print_error(
            std::cerr,
            "Universal character name must have the form \\Uhhhhhhhh.\n");
        rinvalid();
      }
      break;

    /*
      String Literals

      encoding-prefix: u8 u U L

      string-literal:  encoding-prefix(opt) " s-char-sequence(opt) "

      s-char-sequence: s-char
                       s-char-sequence s-char

      s-char: (any member of the source character set except ", \, or \n.)
              escape-sequence

      escape-sequence:  simple-escape-sequence
                        octal-escape-sequence
                        hexadecimal-escape-sequence
                        universal-character-name

      simple-escape-sequence: \' \" \? \\ \a \b \f \n \r \t \v

      octal-escape-sequence: \ octal-digit
                             \ octal-digit octal-digit
                             \ octal-digit octal-digit octal-digit

      hexadecimal-escape-sequence: \x hexadecimal-digit
                             hexadecimal-escape-sequence hexadecimal-digit

      universal-character-name: \u hex-quad
                                \U hex-quad hex-quad

      hex-quad: hdigit hdigit hdigit hdigit



      Binary Constants

      binary-prefix: 0b 0B

      binary-digit: 0 1

      binary-constant: binary-prefix binary-digit
                       binary-constant '(opt) binary-digit

      integer-suffix: ul uL Ul UL
                      ull uLL Ull ULL
                      uwb uWB Uwb UWB
                      l L lu lU Lu LU
                      ll LL llu llU LLu LLU
                      wb WB wbu wbU WBu WBU



      Floating Point

      D = 0 .. 9

      Exponent
      E = [Ee][+-]?{D}+

      Floating point Suffix
      FS = f F l L df dd dl DF DD DL

      {D}+{E}{FS}?
        1e-10L 1E3F

      H = [0-9a-fA-F]
      HE = [pP][+-]?{D}+

      0[xX]{H}+[']?[.]{H}*[pP][+-]?{D}+



      Character Constants

        Character Prefix
        CP = u8 u U L

        [^'\\\n] = anything except '\'', '\\', and '\n'

        Escape Sequence
        ES = (\\(['"\?\\abfnrtv]|[0-7]{1,3}|x[a-fA-F0-9]+))

        Char Const
        {CP}?"'"([^'\\\n]|{ES})+"'"

        'a' u'a'
        U'\'' L'\"' '\?' U'\\' L'\a' L'\b' u'\f' U'\n' L'\r' u'\t' U'\v'
        u'\7' U'\777' 'u\xbeef'

      Integer literals:

        Integer Suffix
       IS =  ul uL Ul UL
             ull uLL Ull ULL
             uwb uWB Uwb UWB
             l L lu lU Lu LU
             ll LL llu llU LLu LLU
             wb WB wbu wbU WBu WBU

        Digit
        D = [0-9]

        Non-zero Digit
        NZ = [1-9]

        Octal Digit
        O = [0-7]

        CP = u U L

      A) Decimal

          {NZ}{D}*{IS}?

      B) Hexadecimal
        Hex Prefix
        HP = 0x or 0X
        H  = [a-fA-F0-9]

          {HP}{H}+{IS}?

      C) Octal

        0{O}*{IS}?

    */
    default:
      rinvalid();
    }
  }

  rinvalid();
}

std::vector<Lexeme> scan_tokens(const char *s) {
  std::stringstream stream(std::move(std::string(s)), std::ios_base::in);
  std::unique_ptr<SourceReader> reader = std::make_unique<SourceReader>(stream);
  Lexer lexer(std::move(reader));

  std::vector<Lexeme> v;
  while (lexer.peek() != Token::END) {
    v.push_back(lexer.eat());
  }
  v.push_back(lexer.peek()); // Add Token::END

  return v;
}

} // namespace c_lexer
