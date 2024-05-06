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
    'TKN_ALIGNAS',
    'TKN_ALIGNOF',
    'TKN_AUTO',
    'TKN_BOOL',
    'TKN_BREAK',
    'TKN_CASE',
    'TKN_CHAR',
    'TKN_CONST',
    'TKN_CONSTEXPR',
    'TKN_CONTINUE',
    'TKN_DEFAULT',
    'TKN_DO',
    'TKN_DOUBLE',
    'TKN_ELSE',
    'TKN_ENUM',
    'TKN_EXTERN',
    'TKN_FALSE',
    'TKN_FLOAT',
    'TKN_FOR',
    'TKN_GOTO',
    'TKN_IF',
    'TKN_INLINE',
    'TKN_INT',
    'TKN_LONG',
    'TKN_NULLPTR',
    'TKN_REGISTER',
    'TKN_RESTRICT',
    'TKN_RETURN',
    'TKN_SHORT',
    'TKN_SIGNED',
    'TKN_SIZEOF',
    'TKN_STATIC',
    'TKN_STATIC_ASSERT',
    'TKN_STRUCT',
    'TKN_SWITCH',
    'TKN_THREAD_LOCAL',
    'TKN_TRUE',
    'TKN_TYPEDEF',
    'TKN_TYPEOF',
    'TKN_TYPEOF_UNQUAL',
    'TKN_UNION',
    'TKN_UNSIGNED',
    'TKN_VOID',
    'TKN_VOLATILE',
    'TKN_WHILE',
    'TKN__ALIGNAS',
    'TKN__ALIGNOF',
    'TKN__ATOMIC',
    'TKN__BITINT',
    'TKN__BOOL',
    'TKN__COMPLEX',
    'TKN__DECIMAL128',
    'TKN__DECIMAL32',
    'TKN__DECIMAL64',
    'TKN__GENERIC',
    'TKN__IMAGINARY',
    'TKN__NORETURN',
    'TKN__STATIC_ASSERT',
    'TKN__THREAD_LOCAL',
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
          st = GOT_IDENT_CONT;
        } else {
          r(TKN_IF, 2);
        }
        break;
      default:
        hold(GOT_IDENT_CONT);
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
          st = GOT_do;
        } else if (is_ident_cont(sr_->peek())) {
          st = GOT_IDENT_CONT;
        } else {
          r(TKN_DO, 2);
        }
        break;

       ('e' omitted for brevity)

      default:
        hold(GOT_IDENT_CONT);
        break;
      } // switch (c) for GOT_d
    break;

    case 3)
    When no keyword is being matched, we can generate the following:

    case GOT_de:
      switch (c) {
      case 'f':
        st = GOT_def;
        break;
      default:
        hold(GOT_IDENT_CONT);
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
    print('        st = GOT_IDENT_CONT;')
    print('      } else {')
    print(f'        r({tknfor[nxt]}, {len(nxt)});')
    print('      }')


def generate_case_2(collisions: Dict, nxt: str):
    print(f"      if (sr_->peek() == '{collisions[nxt]}') {{")
    print(f'        st = GOT_{nxt};')
    print('      } else if (is_ident_cont(sr_->peek())) {')
    print('        st = GOT_IDENT_CONT;')
    print('      } else {')
    print(f'        r({tknfor[nxt]}, {len(nxt)});')
    print('      }')


def generate_case_3(nxt: str):
    print(f'      st = GOT_{nxt};')


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
        print('      hold(GOT_IDENT_CONT);')
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
