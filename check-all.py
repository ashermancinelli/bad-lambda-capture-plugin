#!/usr/bin/env python
import argparse, os, sys, json

parser = argparse.ArgumentParser(
        description='Check all files in compilation database with clang tool.',
        usage=f'''{sys.argv[0]} [options]

{'-' * 80}

Long Description:

    This tool reads each entry from a `compile_commands.json` database, and runs
    each command after appending the following flags:
        -Xclang -load
        -Xclang PLUGIN_LIBRARY
        -Xclang -plugin
        -Xclang PLUGIN_NAME

    The tool will log each source file the tool runs on.

{'-' * 80}

''', formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('-p', '--compile-commands', required=False, type=str,
        help='Path to compilation database')
parser.add_argument('-l', '--plugin-library', required=True, type=str,
        help='Path to clang plugin library')
parser.add_argument('-n', '--plugin-name', required=True, type=str,
        help='Name of clang plugin')

if __name__ == '__main__':
    args = parser.parse_args()

    database = args.compile_commands if not args.compile_commands is None \
            else 'compile_commands.json'

    invoke = lambda x: f'{x} -Xclang -load -Xclang' \
            f' {os.path.abspath(args.plugin_library)}' \
            f' -Xclang -plugin -Xclang {args.plugin_name}' \
            ' -fsyntax-only'

    database = json.load(open(database, 'r'))
    
    for translation_unit in database:
        d = os.path.abspath(translation_unit['directory'])
        f = os.path.abspath(translation_unit['file'])
        c = os.path.abspath(translation_unit['command'])
        # print(d, f, c, '\nINVOK:', invoke(c)); exit()
        print(f'Running tool on {f}')
        os.system(f'cd {d} && {invoke(c)}')
