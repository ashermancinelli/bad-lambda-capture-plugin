#!/usr/bin/env python
import argparse, os, sys, json

parser = argparse.ArgumentParser(
        description='Check all files in compilation database with clang tool.')
parser.add_argument('-c', '--compile-commands', required=False, type=str,
        help='Path to compilation database')
parser.add_argument('-t', '--tool-path', required=True, type=str,
        help='Path to clang tool library')
parser.add_argument('-p', '--plugin', required=True, type=str,
        help='Name of clang plugin')

if __name__ == '__main__':
    args = parser.parse_args()

    database = args.compile_commands if not args.compile_commands is None \
            else './compile_commands.json'

    invoke = lambda x: f'{x} -Xclang -load -Xclang' \
            f' {os.path.abspath(args.tool_path)}' \
            f' -Xclang -plugin -Xclang {args.plugin}' \
            ' -fsyntax-only'

    database = json.load(open(database, 'r'))
    
    for translation_unit in database:
        d = os.path.abspath(translation_unit['directory'])
        f = os.path.abspath(translation_unit['file'])
        c = os.path.abspath(translation_unit['command'])
        # print(d, f, c, '\nINVOK:', invoke(c)); exit()
        print(f'Running tool on {f}')
        os.system(f'cd {d} && {invoke(c)}')
