#!/usr/bin/env python3

"""
This script generates the keyword portion of the C lexer.
"""

import argparse
import sys
from collections import defaultdict
from typing import Dict, Set, Tuple

keys = [
    'alignas',
    'alignof',
    'auto',
    'bool',
    'break',
    'case',
    'char',
    'const',
    'constexpr',
    'continue',
    'default',
    'do',
    'double',
    'else',
    'enum',
    'extern',
    'false',
    'float',
    'for',
    'goto',
    'if',
    'inline',
    'int',
    'long',
    'nullptr',
    'register',
    'restrict',
    'return',
    'short',
    'signed',
    'sizeof',
    'static',
    'static_assert',
    'struct',
    'switch',
    'thread_local',
    'true',
    'typedef',
    'typeof',
    'typeof_unqual',
    'union',
    'unsigned',
    'void',
    'volatile', 
    'while', 
    '_Alignas',
    '_Alignof',
    '_Atomic',
    '_BitInt',
    '_Bool',
    '_Complex', 
    '_Decimal128',
    '_Decimal32',
    '_Decimal64',
    '_Generic',
    '_Imaginary',
    '_Noreturn',
    '_Static_assert',
    '_Thread_local',
]

tokens = [
    'ALIGNAS',
    'ALIGNOF',
    'AUTO',
    'BOOL',
    'BREAK',
    'CASE',
    'CHAR',
    'CONST',
    'CONSTEXPR',
    'CONTINUE',
    'DEFAULT',
    'DO',
    'DOUBLE',
    'ELSE',
    'ENUM',
    'EXTERN',
    'FALSE',
    'FLOAT',
    'FOR',
    'GOTO',
    'IF',
    'INLINE',
    'INT',
    'LONG',
    'NULLPTR',
    'REGISTER',
    'RESTRICT',
    'RETURN',
    'SHORT',
    'SIGNED',
    'SIZEOF',
    'STATIC',
    'STATIC_ASSERT',
    'STRUCT',
    'SWITCH',
    'THREAD_LOCAL',
    'TRUE',
    'TYPEDEF',
    'TYPEOF',
    'TYPEOF_UNQUAL',
    'UNION',
    'UNSIGNED',
    'VOID',
    'VOLATILE',
    'WHILE',
    '_ALIGNAS',
    '_ALIGNOF',
    '_ATOMIC',
    '_BITINT',
    '_BOOL',
    '_COMPLEX',
    '_DECIMAL128',
    '_DECIMAL32',
    '_DECIMAL64',
    '_GENERIC',
    '_IMAGINARY',
    '_NORETURN',
    '_STATIC_ASSERT',
    '_THREAD_LOCAL',
]

tknfor = {}
for i, k in enumerate(keys):
    tknfor[k] = tokens[i]

def substrings(s: str):
    """
    For a given keyword string, eg 'while', produce all the incrementing
    substrings up to, but not including, the last character.

    w
    wh
    whi
    whil
    """
    for i in range(1, len(s) + 1):
        yield s[:i]

def find_collisions(key: str, data: Dict, already: Set, collisions: Dict):
    """
    For C'23, four pairs of keywords collide with each other with respect
    to scanning:

    'const' and 'constexpr'
    'do' and 'double'
    'static' and 'static_assert'
    'typeof' and 'typeof_unqual'

    For example, if we have scanned 'cons', and we have just eaten 't',
    if the result of peek() is an 'e', we must continue trying to scan
    'constexpr'.

    case 1)
    When there is no collision, the scanning code can look like the following,
    assuming that a keyword is matched.

    case GOT_i:
      switch (c) {
      case 'f':
        if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::IF, 2);
        }
        break;
      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_i
      break;

    case 2)
    When a collision occurs, we must generate the following, assuming that
    a keyword can also be matched:

    case GOT_d:
      switch (c) {
      case 'o':
        if (sr_->peek() == 'u') {
          nextst(GOT_do);
        } else if (is_ident_cont(sr_->peek())) {
          nextst(GOT_IDENT);
        } else {
          r(Token::DO, 2);
        }
        break;

       ('e' omitted for brevity)

      default:
        holdst(GOT_IDENT);
        break;
      } // switch (c) for GOT_d
    break;

    case 3)
    When no keyword is being matched, we can generate the following:

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

    """

    for s in substrings(key):
        if s in already:
            continue
        already.add(s)

        for k in keys:
            if k != s and k.find(s) == 0:
                fragment = k[:len(s)+1]

                if fragment in keys:
                    fragment = '*' + fragment

                if fragment not in data[s]:
                    data[s].append(fragment)

    for k in keys:
        if k in data:
            fragment = '*' + k # change the * to a !
            index = k[:-1]
            if fragment in data[index]:
                data[index].remove(fragment)
                data[index].append('!' + k)

                # Find our next character to match.
                for d in data[k]:
                    collisions[k] = d[-1:]


def generate_defines(start: int, data: Dict):
    for i in sorted(data.keys()):
        print(f'#define GOT_{i} {start}')
        start += 1


def generate_case_1(nxt: str):
    print('      if (is_ident_cont(sr_->peek())) {')
    print('        nextst(GOT_IDENT);')
    print('      } else {')
    print(f'        r(Token::{tknfor[nxt]}, {len(nxt)});')
    print('      }')


def generate_case_2(collisions: Dict, nxt: str):
    print(f"      if (sr_->peek() == '{collisions[nxt]}') {{")
    print(f'        nextst(GOT_{nxt});')
    print('      } else if (is_ident_cont(sr_->peek())) {')
    print('        nextst(GOT_IDENT);')
    print('      } else {')
    print(f'        r(Token::{tknfor[nxt]}, {len(nxt)});')
    print('      }')


def generate_case_3(nxt: str):
    print(f'      nextst(GOT_{nxt});')


def generate_source(data: Dict, collisions: Dict):
    for i in sorted(data.keys()):

        print(f'case GOT_{i}:')
        print('  switch (c) {')

        for nxt in data[i]:
            special = nxt[0]
            if special in {'*', '!'}:
                nxt = nxt[1:]

            print(f"    case '{nxt[-1]}':")

            if special == '*':
                generate_case_1(nxt)
            elif special == '!':
                generate_case_2(collisions, nxt)
            else:
                generate_case_3(nxt)

            print('      break;')


        print('    default:')
        print('      holdst(GOT_IDENT);')
        print('      break;')
        print(f'}} // switch (c) for GOT_{i}')
        print('break;')
        print()


def parse_args() -> Tuple:
    parser = argparse.ArgumentParser()

    parser.add_argument('-v', '--version', action='version',
                        version='%(prog)s 0.0.1',
                        help='display version information and exit')

    subparser = parser.add_subparsers(dest='which')

    defines = subparser.add_parser('defines')
    defines.add_argument('-s', '--start', type=int, default=3,
                         help='The start value for the first #define.')

    subparser.add_parser('code')
    subparser.add_parser('tests')

    return (parser, parser.parse_args())


def generate_tests(data: Dict):
    for key in sorted(data.keys()):
        print(f'"{key}z", ')
    for key in keys:
        print(f'"{key}z", ')


def main():
    data = defaultdict(list)
    already = set()
    collisions = {}

    for k in keys:
        find_collisions(k, data, already, collisions)

    #print(f'collisions: {collisions}')

    #for k in keys:
    #    if k in data:
    #        print(f'k: {k}')

    #for d in sorted(data):
    #    print(f'data[{d}] = {data[d]}')

    parser, args = parse_args()
    if args.which is None:
        parser.print_help()
        sys.exit(1)

    match args.which:
        case 'defines':
            generate_defines(args.start, data)
        case 'code':
            generate_source(data, collisions)
        case 'tests':
            generate_tests(data)

if __name__ == '__main__':
    main()
