#!/usr/bin/env python

import collections
import importlib
import json
import re
from pathlib import Path

from vtkmodules.vtkCommonCore import vtkVersion


def get_program_parameters(argv):
    import argparse
    description = 'Generate import statements for the VTK classes in your Python code.'
    epilogue = '''
The output will contain program name(s) followed by the import statements.
You can specify a folder for the Python sources or paths to several sources.

Note: If there are spaces in the paths, enclose the path in quotes.
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('-j', '--json',
                        help='The path to the VTK JSON file (modules.json).')
    parser.add_argument('sources', nargs='+', help='The path to a folder of Python files or to a Python file.')
    parser.add_argument('-f', '--file', help='The file name to write the output too.')
    args = parser.parse_args()
    return args.json, args.sources, args.file


class Patterns:
    vtk_patterns = [
        # Class pattern.
        re.compile(r'(vtk[a-zA-Z0-9]+)\('),
        # Constants pattern.
        re.compile(r'(VTK_[A-Z_]+)'),
        # Special patterns ...
        re.compile(r'(mutable)\('),
        # Handle vtkClass.yyy
        re.compile(r'(vtk[a-zA-Z0-9]+)\.'),
        # Handle class xx(vtkClass):
        re.compile(r'\( ?(vtk[a-zA-Z0-9]+) ?\)'),
    ]
    skip_patterns = [
        # Empty lines
        re.compile(r'^ *$'),
        # import ...
        re.compile(r'^ *import'),
        # from ... import ...
        re.compile(r'^ *from[ \S]+import'),
        # Any vtk class on its own
        re.compile(r'^ *vtk[a-zA-Z0-9]+,*$'),
        # Single closing bracket
        re.compile(r'^ *\)+$'),
    ]


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


def get_classes_constants(paths):
    """
    Extract the vtk class names and constants from the path.

    :param paths: The path(s) to the Python file(s).
    :return: The file name, the VTK classes and any VTK constants.
    """

    res = collections.defaultdict(set)
    for path in paths:
        content = path.read_text().split('\n')
        for line in content:
            for pattern in Patterns.skip_patterns:
                m = pattern.search(line)
                if m:
                    continue
            for pattern in Patterns.vtk_patterns:
                m = pattern.search(line)
                if m:
                    for g in m.groups():
                        res[str(path)].add(g)
    return res


def format_imports(imports):
    name_keys = sorted(imports.keys())
    res = list()
    for name in name_keys:
        res.append(f'\n{name}')
        module_keys = sorted(imports[name].keys())
        for module in module_keys:
            classes = sorted(list(imports[name][module]))
            if len(classes) == 1:
                res.append(f'from vtkmodules.{module} import {classes[0]}')
            else:
                c_list = list()
                for c in classes:
                    c_list.append(f'    {c}')
                s = '(\n'
                s += ',\n'.join(c_list)
                s += '\n    )'
                res.append(f'from vtkmodules.{module} import {s}')
        additional_modules = ['vtkInteractionStyle', 'vtkRenderingFreeType',
                              'vtkRenderingContextOpenGL2', 'vtkRenderingOpenGL2', 'vtkRenderingVolumeOpenGL2',
                              'vtkRenderingUI']
        comments = [
            '',
            '# You may need to uncomment one or more of the following imports.',
            '# If vtkRenderWindow is used and you want to use OpenGL,',
            '#   you also need the vtkRenderingOpenGL2 module.',
            '# If vtkRenderWindowInteractor is used, uncomment vtkInteractionStyle',
            '# If text rendering is used, uncomment vtkRenderingFreeType.',
            '#',
            '# If using PyCharm, preface each one you select with this line:',
            '# noinspection PyUnresolvedReferences',
            '#',
        ]
        res += comments
        for module in sorted(additional_modules):
            res.append(f'# import vtkmodules.{module}')
        res.append('')
    return res


def main(json_path, src_paths, ofn):
    use_json = not vtk_version_ok(9, 0, 20210918)
    if use_json:
        if not json_path:
            print('modules.json (from your VTK build directory) is needed.')
            return
        jpath = Path(json_path)
        if jpath.is_dir():
            jpath = jpath / 'modules.json'
        if not jpath.is_file():
            print(f'Non existent JSON file: {jpath}')
    else:
        jpath = None

    paths = list()
    for fn in src_paths:
        path = Path(fn)
        if path.is_file() and path.suffix == '.py':
            paths.append(path)
        elif path.is_dir():
            path_list = list(Path(fn).rglob('*.py'))
            program_path = Path(__file__)
            for path in path_list:
                if path.resolve() != program_path.resolve():
                    paths.append(path)
        else:
            print(f'Non existent path: {path}')

    classes_constants = get_classes_constants(paths)
    if not classes_constants:
        print('No classes or constants were present.')
        return

    if use_json:
        vtk_modules = get_available_modules(jpath)
    else:
        vtklib = importlib.__import__('vtkmodules')
        vtk_modules = sorted(vtklib.__all__)

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
        if path.suffix == '':
            path = Path(ofn).with_suffix('.txt')
        path.write_text('\n'.join(res))
    else:
        print('\n'.join(res))


def vtk_version_ok(major, minor, build):
    """
    Check the VTK version.

    :param major: Requested major version.
    :param minor: Requested minor version.
    :param build: Requested build version.
    :return: True if the requested VTK version is >= the actual VTK version.
    """
    requested_version = (100 * int(major) + int(minor)) * 100000000 + int(build)
    ver = vtkVersion()
    actual_version = (100 * ver.GetVTKMajorVersion() + ver.GetVTKMinorVersion()) \
                     * 100000000 + ver.GetVTKBuildVersion()
    if actual_version >= requested_version:
        return True
    else:
        return False


if __name__ == '__main__':
    import sys

    json_path, src_paths, ofn = get_program_parameters(sys.argv)
    main(json_path, src_paths, ofn)
