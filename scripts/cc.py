#!/usr/bin/env python3
# MIT License
#
# Copyright (c) 2024 Tim Whisonant
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import argparse
import json
from pathlib import Path


def find_compile_commands(fname: str) -> Path:
    curdir = Path.cwd()
    ccpath = curdir

    ccfile = ccpath.joinpath(fname)
    while not ccfile.exists() and not ccpath.joinpath('.git').exists():
        ccpath = ccpath.parent
        ccfile = ccpath.joinpath(fname)

    return ccfile


def parse_args():
    parser = argparse.ArgumentParser()

    parser.add_argument('jsonfile', nargs='?',
                        default='build/compile_commands.json',
                        help='relative path to compile_commands.json')

    parser.add_argument('-v', '--version', action='version',
                        version='%(prog)s 1.0.0',
                        help='display version information and exit')

    return parser, parser.parse_args()


def main():
    incs = [
        '-I/usr/include/c++/11',
        '-I/usr/include/x86_64-linux-gnu/c++/11',
    ]

    _, args = parse_args()

    ccfile = find_compile_commands(args.jsonfile)

    with open(ccfile, 'r', encoding='UTF-8') as fd:
        data = json.load(fd)

    for elem in data:
        path = Path(elem['directory'])
        if path.name in ['googlemock', 'googletest']:
            continue

        cmd = elem['command']
        index = cmd.find('-I')

        cmd = cmd[:index] + ' '.join(incs) + ' ' + cmd[index:]

        elem['command'] = cmd

    with open(ccfile, 'w', encoding='UTF-8') as fd:
        json.dump(data, fd, indent=2)

if __name__ == '__main__':
    main()
