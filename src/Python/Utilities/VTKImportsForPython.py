#!/usr/bin/env python

import collections
import importlib
import json
import re
from pathlib import Path


def get_program_parameters(argv):
    import argparse
    description = 'Generate a list of Python import statements.'
    epilogue = '''
This list of Python imports is generated using the VTK modules, along
 with the VTK classes and constants in your Python source file(s).
 
The list will contain the program name followed by the import statements.

Uses modules.json (from your VTK build directory) and your source
 folder(s), file(s) to get the VTK modules, classes
 and constants needed by the python source.

Note: If there are spaces in the paths, enclose the path in quotes.

    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('json',
                        help='The path to the VTK JSON file (modules.json).')
    parser.add_argument('sources', nargs='+', help='The path to a folder of Python files or to a Python file.')
    parser.add_argument('-f', '--file', help='The file name to write the output too.')
    args = parser.parse_args()
    return args.json, args.sources, args.file


def get_available_modules(jpath):
    """
    From the parsed JSON data file make a list of the VTK modules.

    :param jpath: The JSON file path.
    :return: VTK Classes and modules.
    """
    with open(jpath) as data_file:
        json_data = json.load(data_file)

    res = list()
    for k in json_data['modules'].keys():
        m = k.split('::')
        if len(m) > 1:
            res.append(f'vtk{m[1]}')
    return sorted(res)


def get_classes_constants(path):
    """
    Extract the vtk class names and constants from the path.

    :param path: The path to the Python file.
    :return: The file name, the VTK classes and any VTK constants.
    """
    res = collections.defaultdict(set)
    content = path.read_text().split('\n')
    for line in content:
        vtk_class_pattern = re.compile(r'(vtk[a-zA-Z]+\S+\(\))')
        vtk_class_pattern1 = re.compile(r'[^\(\)]+')
        vtk_constants_pattern = re.compile(r'(VTK_[A-Z_]+)')
        m = vtk_class_pattern.search(line)
        if m:
            for g in m.groups():
                m1 = vtk_class_pattern1.match(g)
                res[path.stem].add(m1.group())
        m = vtk_constants_pattern.search(line)
        if m:
            for g in m.groups():
                res[path.stem].add(g)

    return res


def format_imports(imports):
    name_keys = sorted(imports.keys())
    res = list()
    for name in name_keys:
        res.append(f'{name}\n')
        module_keys = sorted(imports[name].keys())
        for module in module_keys:
            classes = sorted(list(imports[name][module]))
            if len(classes) == 1:
                res.append(f'from vtkmodules.{module} import {classes[0]}\n')
            else:
                c_list = list()
                for c in classes:
                    c_list.append(f'    {c}')
                s = '(\n'
                s += ',\n'.join(c_list)
                s += '\n    )'
                res.append(f'from vtkmodules.{module} import {s}\n')
        res.append('\n')

    return res


def main(json_path, src_paths, ofn):
    jpath = Path(json_path)
    if jpath.is_dir():
        jpath = jpath / 'modules.json'
    if not jpath.is_file():
        print(f'Non existent JSON file: {jpath}')

    classes_constants = collections.defaultdict(set)
    for fn in src_paths:
        path = Path(fn)
        if path.is_file() and path.suffix == '.py':
            classes_constants.update(get_classes_constants(path))
        elif path.is_dir():
            paths = list(Path(fn).rglob('*.py'))
            program_path = Path(__file__)
            for path in paths:
                if path.resolve() != program_path.resolve():
                    classes_constants.update(get_classes_constants(path))

    if not classes_constants:
        print('No classes or constants were present.')
        return

    vtk_modules = get_available_modules(jpath)
    name_to_module = dict()
    for module in vtk_modules:
        try:
            module_dict = importlib.import_module('vtkmodules.' + module).__dict__
            for name in module_dict:
                name_to_module[name] = module
        except ModuleNotFoundError:
            # print(module, ' not found.')
            continue

    imports = collections.defaultdict(lambda: collections.defaultdict(set))
    for name, classes_constants in classes_constants.items():
        for vtk_class in classes_constants:
            if vtk_class in name_to_module:
                module = name_to_module[vtk_class]
                imports[name][module].add(vtk_class)

    res = format_imports(imports)
    if ofn:
        path = Path(ofn)
        path.write_text(''.join(res))
    else:
        print(''.join(res))


if __name__ == '__main__':
    import sys

    json_path, src_paths, ofn = get_program_parameters(sys.argv)
    main(json_path, src_paths, ofn)
