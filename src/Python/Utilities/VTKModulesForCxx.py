#!/usr/bin/env python

import collections
import json
import os
import re
from pathlib import Path


def get_program_parameters(argv):
    import argparse
    description = 'Generate a find_package(VTK COMPONENTS ...) command for CMake.'
    epilogue = '''
Uses modules.json and your source files to generate a
  find_package(VTK COMPONENTS ...) command listing all the vtk modules
  needed by the C++ source and header files in your code.

Paths for more than one source path can be specified.

Note than if there are spaces in the paths, enclose the path in quotes.

If it is unable to find modules for your headers then
  a list of these, along with the files they are in, is produced
  so you can manually add the corresponding modules or rebuild VTK
  to include the missing modules.

You will need to manually add any third-party modules
   (if used) to the find_package command.
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('json', default=['modules.json'], help='The path to the VTK JSON file (modules.json).')
    parser.add_argument('sources', nargs='+', help='The path to the source files.')
    parser.add_argument('-f', '--file', help='The file name to write the output too.')
    args = parser.parse_args()
    return args.json, args.sources, args.file


class Patterns:
    header_pattern = re.compile(r'^#include *[<\"](\S+)[>\"]')
    vtk_include_pattern = re.compile(r'^(vtk\S+)')
    vtk_qt_include_pattern = re.compile(r'^(QVTK\S+)')


def get_headers_modules(json_data):
    """
    From the parsed JSON data file make a dictionary whose key is the
     header filename and value is the module.
    :param json_data: The parsed JSON file modules.json.
    :return:
    """

    # The headers should be unique to a module, however we will not assume this.
    res = collections.defaultdict(set)
    for k, v in json_data['modules'].items():
        if 'headers' in v:
            for k1 in v['headers']:
                res[k1].add(k)
    return res


def get_vtk_components(jpath, paths):
    """
    Get the VTK components
    :param jpath: The path to the JSON file.
    :param paths: The C++ file paths.
    :return:
    """

    with open(jpath) as data_file:
        json_data = json.load(data_file)
    vtk_headers_modules = get_headers_modules(json_data)

    modules = set()
    inc_no_mod = set()
    inc_no_mod_headers = collections.defaultdict(set)
    mod_implements = collections.defaultdict(set)
    headers = collections.defaultdict(set)

    for path in paths:
        if path.is_file():
            content = path.read_text().split('\n')
            for line in content:
                m = Patterns.header_pattern.match(line.strip())
                if m:
                    # We have a header name, split it from its path (if the path exists).
                    header_parts = os.path.split(m.group(1))
                    m = Patterns.vtk_include_pattern.match(header_parts[1])
                    if m:
                        headers[m.group(1)].add(path)
                        continue
                    m = Patterns.vtk_qt_include_pattern.match(header_parts[1])
                    if m:
                        headers[m.group(1)].add(path)
    for incl in headers:
        if incl in vtk_headers_modules:
            m = vtk_headers_modules[incl]
            for v in m:
                modules.add(v)
        else:
            inc_no_mod.add(incl)
            inc_no_mod_headers[incl] = headers[incl]

    if headers:
        for m in modules:
            if not json_data['modules'][m]['implementable']:
                continue
            for i in json_data['modules']:
                if i in modules:
                    continue
                if m in json_data['modules'][i]['implements']:
                    # Suggest module i since it implements m
                    mod_implements[i].add(m)

    return modules, mod_implements, inc_no_mod, inc_no_mod_headers


def disp_components(modules, module_implements):
    """
    For the found modules display them in a form that the user can
     copy/paste into their CMakeLists.txt file.
    :param modules: The modules.
    :param module_implements: Modules implementing other modules.
    :return:
    """
    res = ['find_package(VTK\n COMPONENTS']
    for m in sorted(modules):
        res.append('    {:s}'.format(m.split('::')[1]))
    if module_implements:
        keys = sorted(module_implements)
        max_width = len(max(keys, key=len).split('::')[1])
        comments = [
            '    #',
            '    # These modules are suggested since they implement an existing module.',
            '    # You may need to uncomment one or more of these.',
            '    # If vtkRenderWindow is used and you want to use OpenGL,',
            '    #   you also need the RenderingOpenGL2 module.',
            '    # If vtkRenderWindowInteractor is used,',
            '    #    uncomment RenderingUI and possibly InteractionStyle.',
            '    # If text rendering is used, uncomment RenderingFreeType',
            '    #'
        ]
        res.extend(comments)
        for key in keys:
            res.append(
                f'    # {key.split("::")[1]:<{max_width}} # implements {", ".join(sorted(module_implements[key]))}')
    res.append(')')

    return res


def disp_missing_components(inc_no_mod, inc_no_mod_headers):
    """
    Display the headers along with the missing VTK modules.

    :param inc_no_mod: Missing modules.
    :param inc_no_mod_headers: Headers with missing modules.
    :return:
    """
    if inc_no_mod:
        res = [''
               '*' * 64,
               'You will need to manually add the modules that',
               '  use these headers to the find_package command.',
               'These could be external modules not in the modules.json file.',
               'Or you may need to rebuild VTK to include the missing modules.',
               '',
               'Here are the vtk headers and corresponding files:']
        sinmd = sorted(inc_no_mod)
        for i in sinmd:
            sh = sorted(list(inc_no_mod_headers[i]))
            res.append(f'in {i}:')
            for j in sh:
                res.append(f'   {j}')
        res.append('*' * 64)

        return res
    else:
        return None


def main(json_path, src_paths, ofn):
    jpath = Path(json_path)
    if jpath.is_dir():
        jpath = jpath / 'modules.json'
    if not jpath.is_file():
        print(f'Non existent JSON file: {jpath}')
        return

    paths = list()
    valid_ext = ['.h', '.hxx', '.cxx', '.cpp', '.txx']
    path_list = list()
    for fn in src_paths:
        path = Path(fn)
        if path.is_file() and path.suffix in valid_ext:
            paths.append(path)
        elif path.is_dir():
            for e in valid_ext:
                path_list += list(Path(fn).rglob(f'*{e}'))
            program_path = Path(__file__)
            for path in path_list:
                if path.resolve() != program_path.resolve():
                    paths.append(path)
        else:
            print(f'Non existent path: {path}')

    modules, mod_implements, inc_no_mod, inc_no_mod_headers = get_vtk_components(jpath, paths)

    res = '\n'.join(disp_components(modules, mod_implements))
    if inc_no_mod:
        res += '\n'.join(disp_missing_components(inc_no_mod, inc_no_mod_headers))

    if ofn:
        path = Path(ofn)
        if path.suffix == '':
            path = Path(ofn).with_suffix('.txt')
        path.write_text(res)
    else:
        print(res)


if __name__ == '__main__':
    import sys

    json_paths, src_paths, ofn = get_program_parameters(sys.argv)
    main(json_paths, src_paths, ofn)
