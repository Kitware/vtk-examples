#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json
import os
import re
import time
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path, PurePath
from urllib.request import urlopen


def get_program_parameters():
    import argparse
    description = 'Produce markdown tables of VTK classes that are used/unused in the examples.'
    epilogue = '''
A JSON file consisting of vtk classes cross-referenced to the examples is also produced.
   It is called vtk_vtk-examples_xref.json.
The markdown tables and the JSON file are written to some_path/src/Coverage

Note:
   To add links to the VTK class documentation on the web, just add -a as a parameter.
   To vary the number of columns for unused classes to say 8, just add -c8 as a parameter.
'''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('vtk_examples', help='The path to the VTK example source files.')
    parser.add_argument('coverage_dest',
                        help='The path to the folder to store the coverage files. Usually web_dir/src/Coverage')
    parser.add_argument('-c', '--columns',
                        help='Specify the number of columns for unused VTK classes output, default is 5.', nargs='?',
                        const=5, type=int, default=5)
    parser.add_argument('-e', '--excluded_columns',
                        help='Specify the number of columns for excluded VTK classes output, default is 5.', nargs='?',
                        const=3, type=int, default=3)
    parser.add_argument('-a', '--add_vtk_html', help='Add html paths to the VTK classes.', action='store_true')

    args = parser.parse_args()
    return args.vtk_examples, args.coverage_dest, args.columns, args.excluded_columns, args.add_vtk_html


@dataclass(frozen=True)
class Links:
    """
    The URL's to the VTK documentation and to the vtk-examples site.
    """
    vtk_docs: str = 'https://www.vtk.org/doc/nightly/html/'
    vtk_examples: str = 'https://kitware.github.io/vtk-examples/site/'


class ElapsedTime:
    """
    Return the value (in fractional seconds) of a performance counter,
     i.e. a clock with the highest available resolution to measure a short duration.
    This includes sleep time and is system-wide.
    """

    def __enter__(self):
        self.start = time.perf_counter()
        return self

    def __exit__(self, *args):
        self.end = time.perf_counter()
        self.interval = self.end - self.start


class VTKClassesInExamples(object):
    """
    Determine what classes are being used or not used in the examples.
    """

    def __init__(self, base_directory, output_directory, columns, excluded_columns, add_vtk_html):
        """
        :param base_directory: The path to the VTK Examples sources, usually some_path/vtk-examples/src
        :param output_directory: Where the coverage file will be written.
        :param columns: When generating the classes not used table, the number of columns to use.
        :param excluded_columns: When generating the excluded classes table, the number of columns to use.
        :param add_vtk_html: True if the Doxygen documentation paths are to be added to the vtk classes in the tables.
        """
        self.example_types = ['CSharp', 'Cxx', 'Java', 'Python']
        # Classes common to most examples.
        self.excluded_classes = ['vtkActor', 'vtkCamera', 'vtkNamedColors', 'vtkPolyDataMapper', 'vtkProperty',
                                 'vtkRenderer', 'vtkRenderWindow', 'vtkRenderWindowInteractor', ]

        self.base_directory = base_directory
        self.output_directory = output_directory
        self.columns = columns
        self.excluded_columns = excluded_columns
        self.add_vtk_html = add_vtk_html

        # A dictionary consisting of the class name as the key and the link class name as the value.
        self.vtk_classes = dict()
        # A dictionary consisting of [example type][directory name][full file paths of each example ...]
        self.example_file_paths = dict()
        # A dictionary consisting of [example type][vtk class][relative example path]{examples, ...}
        self.classes_used = dict()

        # Markdown tables of classes used keyed on [example type]
        self.classes_used_table = dict()
        # Markdown tables of classes not used keyed on [example type]
        self.classes_not_used_table = dict()

        # This cross references the VTK Class with the relevant example indexed by
        #    VTK Class and Language.
        # The first index is the VTK class.
        # The second index corresponds to the languages: CSharp, Cxx, Java, Python
        #    and, additionally, VTKLink.
        # For the indexed languages, each value contains a dictionary consisting of
        #    the example name as the key and the link as the value or None.
        # For the index VTKLink, the value contains a dictionary consisting of the name of
        #    the VTK class as the index and the relevant link to the VTK class description
        #    as the value.
        self.vtk_examples_xref = defaultdict(lambda: defaultdict(str))

    def get_vtk_classes_from_html(self):
        """
        Parse the html file, getting a list of the classes.
        """
        vtk_only_pattern = re.compile(r'^(vtk[A-Za-z0-9]+)$')
        # We want the first match, hence the use of ?.
        # Adding a ? on a quantifier (?, * or +) makes it non-greedy.
        # Selecting only objects marked as classes.
        vtk_class_pattern = re.compile(r'<span class=\"icon\">C.*?href=\"(.*?)\" target=\"_self\">(.*?)</a>')
        links = Links()
        vtk_docs_link = f'{links.vtk_docs}annotated.html'

        try:
            f = urlopen(vtk_docs_link)
            for line in f:
                s = re.findall(vtk_class_pattern, line.decode('utf-8'))
                if s:
                    for item in s:
                        # We only want vtk classes
                        if item[0].startswith('classvtk'):
                            m = vtk_only_pattern.match(item[1])
                            if m:
                                self.vtk_classes[item[1]] = item[0]
                            continue
            f.close()
        except IOError:
            print(f'Unable to open the URL: {vtk_docs_link}')

    def get_example_file_paths(self):
        """
        For each example, get the example file paths.
        """
        for eg in self.example_types:
            # Get the paths to the examples in a particular sub directory e.g Cxx.
            file_paths = defaultdict(list)
            directory = Path(self.base_directory, eg)
            # Does the directory exist?
            if not directory.is_dir():
                raise RuntimeError(f'Non-existent folder: {str(directory)}')
            exclude_dirs = ['Deprecated']
            if eg == 'CSharp':
                fn_pattern = re.compile(r'^[0-9a-zA-Z_\-]+\.cs$')
                exclude_dirs = exclude_dirs + ['WishList']
            elif eg == 'Cxx':
                fn_pattern = re.compile(
                    r'^[0-9a-zA-Z_\-]+\.(hxx|HXX|hpp|HPP|[hH]\+\+|[hH]|cpp|CPP|cxx|CXX|[cC]\+\+|txx|TXX)$')
                exclude_dirs = exclude_dirs + ['CMakeTechniques', 'Untested']
            elif eg == 'Java':
                fn_pattern = re.compile(r'^[0-9a-zA-Z_\-]+\.java$')
                exclude_dirs = exclude_dirs + ['Untested']
            elif eg == 'Python':
                fn_pattern = re.compile(r'^[0-9a-zA-Z_\-]+\.py$')
                exclude_dirs = exclude_dirs + ['Problems']
            else:
                raise RuntimeError('Unknown example type.')

            # Walk the tree.
            for root, directories, files in os.walk(str(directory)):
                # The only visible folders on the web pages are those directly under the language
                # e.g. Cxx/xx is visible but Cxx/xx/yy is not.
                if root[len(str(directory)):].count(os.sep) > 1:
                    continue
                sp = PurePath(root).parts
                idx = sp.index(eg)
                key = '/'.join(sp[idx:])
                if exclude_dirs and len(sp[idx:]) > 1:
                    if sp[idx:][1] in exclude_dirs:
                        continue
                for filename in files:
                    m = fn_pattern.match(filename)
                    if m:
                        file_paths[key].append(str(Path(root, filename)))
            self.example_file_paths[eg] = file_paths

    def vtk_classes_in_examples(self):
        """
        Find the vtk classes used in the examples.
        """
        for eg in self.example_types:
            res = defaultdict(lambda: defaultdict(set))
            if eg == 'CSharp':
                class_pattern = re.compile(r'^[A-Za-z0-9=. <>()_\t]+(vtk[A-Za-z0-9]+)')
            elif eg == 'Cxx':
                class_pattern = re.compile(
                    r'^[ \t]*#include[ ]+<(vtk[A-Za-z0-9]+)+.h>$|'  # match: #include <vtkClass.h>
                    r'.*[< ]+(vtk[A-Za-z0-9]+)[> ]|'  # match: <vtkClass>
                    r'.*[= ]+(vtk[A-Za-z0-9]+)[ ]*::New'  # match: vtkClass::New()
                )
            elif eg == 'Java':
                class_pattern = re.compile(r'^[A-Za-z0-9=. _\t]+new[ ]+(vtk[A-Za-z0-9]+)[ ]*\(')
            elif eg == 'Python':
                class_pattern = re.compile(r'^[A-Za-z0-9=. ()_\t]+(vtk[A-Za-z0-9]+)[ ]*\(')
            else:
                raise RuntimeError('Unknown example type.')

            for k, v in self.example_file_paths[eg].items():
                for fn in v:
                    # Open and read the file building a list of classes.
                    with open(fn) as f:
                        for line in f:
                            m = class_pattern.match(line)
                            if m:
                                c = m.group(m.lastindex)
                                if c in self.vtk_classes:
                                    res[c][k].add(Path(fn).stem)
            self.classes_used[eg] = res

    def make_crossreferences(self):
        """
        Cross-reference the VTK classes to to the corresponding
         VTK Example(s) by language(s).
        """
        links = Links()
        vtk_examples_link = links.vtk_examples
        vtk_docs_link = links.vtk_docs
        for eg in self.example_types:
            vtk_keys = list(sorted(list(self.classes_used[eg].keys()), key=lambda x: (x.lower(), x.swapcase())))
            for vtk_class in vtk_keys:
                paths = self.classes_used[eg][vtk_class]
                # Here we are assuming no two files have the same name.
                # tmp will be used to generate a sorted list of files.
                tmp = dict()
                for path, fn in paths.items():
                    for f in fn:
                        tmp[f] = f'{vtk_examples_link}{path}/{f}'
                self.vtk_examples_xref[vtk_class][eg] = tmp

            for vtk_class in vtk_keys:
                self.vtk_examples_xref[vtk_class]['VTKLink'] = {
                    vtk_class: f'{vtk_docs_link}{self.vtk_classes[vtk_class]}#details'}

    def make_classes_used_table(self):
        """
        Make a table of classes used for each set of examples.
        """
        links = Links()
        vtk_docs_link = links.vtk_docs

        eg_fmt = '[{:s}]({:s})'
        h1 = '# VTK Classes used in the Examples\n\n'
        h2 = '## {:s}\n\n'
        h3 = '### {:s}\n\n'

        # Excluded classes columns.
        h1ec = ' VTK Class '
        h2ec = '-----------'
        r1ec = ' {:s} '
        th1ec = '|' + h1ec
        th2ec = '|' + h2ec
        trec = '|' + r1ec
        for i in range(1, self.excluded_columns):
            th1ec += '|' + h1ec
            th2ec += '|' + h2ec
            trec += '|' + r1ec
        th1ec += '|\n'
        th2ec += '|\n'
        trec += '|\n'

        # Classes and examples.
        th1 = '| VTK Class | Examples |\n'
        th2 = '|--------------|----------------------|\n'
        tr = '| {:s} | {:s} |\n'
        for eg in self.example_types:
            if eg == 'Cxx':
                excl_classes = self.excluded_classes + ['vtkSmartPointer', 'vtkNew']
            else:
                excl_classes = self.excluded_classes
            res = list()
            res.append(h1)
            res.append(h2.format(eg))
            res.append(
                f'Out of {len(self.vtk_classes)} available VTK classes,'
                f' {len(self.classes_used[eg])} are demonstrated here.\n\n')
            # Excluded classes
            res.append(h3.format('Excluded classes'))
            res.append('These classes are excluded since they occur in the majority of the examples:\n\n')
            res.append(th1ec)
            res.append(th2ec)
            tmp = []
            for c in list(sorted(excl_classes, key=lambda x: (x.lower(), x.swapcase()))):
                if self.add_vtk_html:
                    tmp.append(f'[{c}]({vtk_docs_link}{self.vtk_classes[c]}#details)')
                else:
                    tmp.append(f'{c}')
                if len(tmp) == self.excluded_columns:
                    res.append(trec.format(*tmp))
                    del tmp[:]
            if tmp:
                while len(tmp) < self.excluded_columns:
                    tmp.append('')
                res.append(trec.format(*tmp))
            res.append('\n')
            res.append(h3.format('Classes used'))
            res.append(th1)
            res.append(th2)
            vtk_keys = list(sorted(list(self.classes_used[eg].keys()), key=lambda x: (x.lower(), x.swapcase())))
            for c in vtk_keys:
                if c not in excl_classes:
                    paths = self.classes_used[eg][c]
                    f_list = ''
                    # Here we are assuming no two files have the same name.
                    # tmp will be used to generate a sorted list of files.
                    tmp = dict()
                    for path, fn in paths.items():
                        for f in fn:
                            # NOTE: Need leading '/'
                            tmp[f] = eg_fmt.format(f, '/' + path + '/' + f)
                    tmp_keys = list(sorted(list(tmp.keys()), key=lambda x: (x.lower(), x.swapcase())))
                    for k in tmp_keys:
                        f_list += tmp[k] + ' '
                    tmp.clear()
                    if self.add_vtk_html:
                        res.append(tr.format(f'[{c}]({vtk_docs_link}{self.vtk_classes[c]}#details)', f_list.strip()))
                    else:
                        res.append(tr.format(f'{c}', f_list.strip()))
            self.classes_used_table[eg] = res

    def make_classes_not_used_table(self):
        """
        Make a table of classes that are not used for each set of examples.
        """
        links = Links()
        vtk_docs_link = links.vtk_docs

        h1 = '# VTK Classes not used in the Examples\n\n'
        h2 = '## {:s}\n\n'

        h1c = ' VTK Class '
        h2c = '-----------'
        r1c = ' {:s} '
        th1 = '|' + h1c
        th2 = '|' + h2c
        tr = '|' + r1c
        for i in range(1, self.columns):
            th1 += '|' + h1c
            th2 += '|' + h2c
            tr += '|' + r1c
        th1 += '|\n'
        th2 += '|\n'
        tr += '|\n'

        for eg in self.example_types:
            res = list()
            unused_classes = list()
            for vtk_class in sorted(self.vtk_classes):
                if vtk_class not in self.classes_used[eg]:
                    unused_classes.append(f'{vtk_class}')
            res.append(h1)
            res.append(h2.format(eg))
            res.append(
                f'Out of {len(self.vtk_classes)} available VTK classes, {len(unused_classes)} have not been used.\n')
            res.append('\n')
            res.append(th1)
            res.append(th2)

            idx = 0
            tmp = list()
            for c in unused_classes:
                if self.add_vtk_html:
                    tmp.append(f'[{c}]({vtk_docs_link}{self.vtk_classes[c]}#details)')
                else:
                    tmp.append(c)
                idx += 1
                if idx % self.columns == 0:
                    while len(tmp) < self.columns:
                        tmp.append('')
                    res.append(tr.format(*tmp))
                    idx = 0
                    tmp = list()
            if tmp:
                while len(tmp) < self.columns:
                    tmp.append('')
                res.append(tr.format(*tmp))

            self.classes_not_used_table[eg] = res

    def build_tables(self):
        self.get_vtk_classes_from_html()
        self.get_example_file_paths()
        self.vtk_classes_in_examples()
        self.make_crossreferences()
        self.make_classes_used_table()
        self.make_classes_not_used_table()

    def print_tables(self):
        p = Path(self.output_directory)
        if not p.is_dir():
            p.mkdir(parents=True, exist_ok=True)
        if self.vtk_examples_xref:
            fn = Path(self.output_directory, 'vtk_vtk-examples_xref.json')
            with open(fn, 'w') as outfile:
                json.dump(self.vtk_examples_xref, outfile)
        for eg in self.example_types:
            fn = Path(self.output_directory, eg + 'VTKClassesUsed.md')
            with open(fn, 'w') as f:
                f.writelines(self.classes_used_table[eg])
            fn = Path(self.output_directory, eg + 'VTKClassesNotUsed.md')
            with open(fn, 'w') as f:
                f.writelines(self.classes_not_used_table[eg])


def main():
    example_source, coverage_dest, columns, excluded_columns, add_vtk_html = get_program_parameters()
    if not Path(example_source).is_dir():
        print(f'The path: {example_source} does not exist.')
    if not Path(coverage_dest).is_dir():
        print(f'The path: {coverage_dest} does not exist.')
    vtk_classes = VTKClassesInExamples(example_source, coverage_dest, columns, excluded_columns, add_vtk_html)
    vtk_classes.build_tables()
    vtk_classes.print_tables()


if __name__ == '__main__':
    with ElapsedTime() as et:
        main()
    print('Time taken: {:0.3f}s'.format(et.interval))
