#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json
import re
import time
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path
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
    parser.add_argument('vtk_examples', help='The path to the VTK example source files. e.g. vtk-examples/src')
    parser.add_argument('coverage_dest',
                        help='The path to the folder to store the coverage files. Usually web_dir/src/Coverage')
    parser.add_argument('-c', '--columns',
                        help='Specify the number of columns for unused VTK classes output, default is 5.', nargs='?',
                        const=5, type=int, default=5)
    parser.add_argument('-e', '--excluded_columns',
                        help='Specify the number of columns for excluded VTK classes output, default is 3.', nargs='?',
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


class VTKClassesInExamples:
    """
    Determine what classes are being used or not used in the examples.
    """

    def __init__(self, base_path: Path, output_path: Path, columns, excluded_columns, add_vtk_html):
        """
        Note: base_path, output_path are Path objects.
        :param base_path: The path to the VTK Examples sources, usually some_path/vtk-examples/src
        :param output_path: Tha path to where the coverage files will be written.
        :param columns: When generating the classes not used table, the number of columns to use.
        :param excluded_columns: When generating the excluded classes table, the number of columns to use.
        :param add_vtk_html: True if the Doxygen documentation paths are to be added to the vtk classes in the tables.
        """
        self.example_types = ['CSharp', 'Cxx', 'Java', 'Python']
        # Classes common to most examples.
        self.excluded_classes = ['vtkActor', 'vtkCamera', 'vtkNamedColors', 'vtkPolyDataMapper', 'vtkProperty',
                                 'vtkRenderer', 'vtkRenderWindow', 'vtkRenderWindowInteractor', ]

        # Make sure that they are paths.
        # See: https://stackoverflow.com/questions/58647584/how-to-test-if-object-is-a-pathlib-path
        if isinstance(base_path, Path):
            self.base_path = base_path
        else:
            self.base_path = Path(base_path)
        if isinstance(output_path, Path):
            self.output_path = output_path
        else:
            self.output_path = Path(output_path)

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

        # Set up our patterns.
        suffix_patterns = dict()
        suffix_patterns['CSharp'] = re.compile(r'\.cs$')
        suffix_patterns['Cxx'] = re.compile(r'\.(hxx|HXX|hpp|HPP|[hH]\+\+|[hH]|cpp|CPP|cxx|CXX|[cC]\+\+|txx|TXX)$')
        suffix_patterns['Java'] = re.compile(r'\.java$')
        suffix_patterns['Python'] = re.compile(r'\.py$')

        # Set up our folders to exclude.
        excluded_dirs = dict()
        excluded_dirs['CSharp'] = []
        excluded_dirs['Cxx'] = ['CMakeTechniques', 'Untested']
        excluded_dirs['Java'] = ['Untested']
        excluded_dirs['Python'] = ['Problems']
        for d in excluded_dirs:
            excluded_dirs[d] = sorted(['Deprecated', 'Snippets'] + excluded_dirs[d])

        for eg in self.example_types:
            # Get the paths to the examples in a particular sub directory e.g Cxx.
            file_paths = defaultdict(list)
            directory = self.base_path / eg
            # Does the directory exist?
            if not directory.is_dir():
                raise RuntimeError(f'Non-existent folder: {str(directory)}')

            # Pull out the subdirectories directly under directory.
            # Also exclude some of these subdirectories.
            subdirs = [f for f in directory.iterdir() if f.is_dir() and f.parts[-1] not in excluded_dirs[eg]]
            # Now get the file paths we want.
            for subdir in subdirs:
                path_list = [f for f in subdir.iterdir() if f.is_file()]
                for path in path_list:
                    m = suffix_patterns[eg].match(path.suffix)
                    if m:
                        key = '/'.join(path.parts[-3:-1])
                        file_paths[key].append(path)
            self.example_file_paths[eg] = file_paths

    def get_vtk_classes_from_examples(self):
        """
        Find the vtk classes used in the examples.
        """

        # Set up our patterns.
        class_patterns = dict()
        class_patterns['CSharp'] = re.compile(r'^[A-Za-z0-9=. <>()_\t]+(vtk[A-Za-z0-9]+)')
        class_patterns['Cxx'] = re.compile(
            r'^[ \t]*#include[ ]+<(vtk[A-Za-z0-9]+)+.h>$|'  # match: #include <vtkClass.h>
            r'.*[< ]+(vtk[A-Za-z0-9]+)[> ]|'  # match: <vtkClass>
            r'.*[= ]+(vtk[A-Za-z0-9]+)[ ]*::New'  # match: vtkClass::New()
        )
        class_patterns['Java'] = re.compile(r'^[A-Za-z0-9=. _\t]+new[ ]+(vtk[A-Za-z0-9]+)[ ]*\(')
        class_patterns['Python'] = re.compile(r'^[A-Za-z0-9=. ()_\t]+(vtk[A-Za-z0-9]+)[ ]*\(')
        # Skip some lines in the files.
        skip_patterns = re.compile(
            r'(^ *$)|'  # Empty lines.
            r'(^ *[(|\)]+$)|'  # Single opening or closing bracket.
            r'(^ *[{|}]+$)'  # Single opening or closing curly brace.
        )

        for eg in self.example_types:
            res = defaultdict(lambda: defaultdict(set))
            for k, v in self.example_file_paths[eg].items():
                for fn in v:
                    # Open and read the file building a set of classes.
                    content = fn.read_text().split('\n')
                    for line in content:
                        if skip_patterns.match(line):
                            continue
                        m = class_patterns[eg].match(line)
                        if m:
                            c = m.group(m.lastindex)
                            if c in self.vtk_classes:
                                res[c][k].add(Path(fn).stem)
            self.classes_used[eg] = res

    def get_crossreferences(self):
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

    def get_used_classes(self):
        """
        Make a table of classes used for each set of examples.
        """
        links = Links()
        vtk_docs_link = links.vtk_docs

        eg_fmt = '[{:s}]({:s})'
        h1 = '# VTK Classes used in the Examples\n'
        h2 = '## {:s}\n'
        h3 = '### {:s}\n'

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
        th1ec += '|'
        th2ec += '|'
        trec += '|'

        # Classes and examples.
        th1 = '| VTK Class | Examples |'
        th2 = '|--------------|----------------------|'
        tr = '| {:s} | {:s} |'
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
                f' {len(self.classes_used[eg])} are demonstrated here.\n')
            # Excluded classes
            res.append(h3.format('Excluded classes'))
            res.append('These classes are excluded since they occur in the majority of the examples:\n')
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
            res.append('')
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
            res.append('')
            self.classes_used_table[eg] = res

    def get_unused_classes(self):
        """
        Make a table of classes that are not used for each set of examples.
        """
        links = Links()
        vtk_docs_link = links.vtk_docs

        h1 = '# VTK Classes not used in the Examples\n'
        h2 = '## {:s}\n'

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
        th1 += '|'
        th2 += '|'
        tr += '|'

        for eg in self.example_types:
            res = list()
            unused_classes = list()
            for vtk_class in sorted(self.vtk_classes):
                if vtk_class not in self.classes_used[eg]:
                    unused_classes.append(f'{vtk_class}')
            res.append(h1)
            res.append(h2.format(eg))
            res.append(
                f'Out of {len(self.vtk_classes)} available VTK classes, {len(unused_classes)} have not been used.')
            res.append('')
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

            res.append('')
            self.classes_not_used_table[eg] = res

    def build_tables(self):
        self.get_vtk_classes_from_html()
        self.get_example_file_paths()
        self.get_vtk_classes_from_examples()
        self.get_crossreferences()
        self.get_used_classes()
        self.get_unused_classes()

    def generate_files(self):
        if not self.output_path.is_dir():
            self.output_path.mkdir(parents=True, exist_ok=True)
        # Write out all the VTK Classes that we found.
        if self.vtk_classes:
            keys = '\n'.join(sorted(self.vtk_classes.keys()))
            fn = self.output_path / 'vtk_classes.txt'
            fn.write_text(keys)
        if self.vtk_examples_xref:
            fn = self.output_path / 'vtk_vtk-examples_xref.json'
            with open(fn, 'w') as outfile:
                # For debugging.
                # json.dump(self.vtk_examples_xref, outfile, indent = 2)
                json.dump(self.vtk_examples_xref, outfile)
        for eg in self.example_types:
            fn = self.output_path / (eg + 'VTKClassesUsed.md')
            fn.write_text('\n'.join(self.classes_used_table[eg]))
            fn = self.output_path / (eg + 'VTKClassesNotUsed.md')
            fn.write_text('\n'.join(self.classes_not_used_table[eg]))


def main():
    example_source, coverage_dest, columns, excluded_columns, add_vtk_html = get_program_parameters()
    source_path = Path(example_source)
    coverage_path = Path(coverage_dest)
    if not source_path.is_dir():
        print(f'The path: {source_path} does not exist.')
        return
    if not coverage_path.is_dir():
        print(f'The path: {coverage_path} does not exist.')
        print(f'Creating it.')
    vtk_classes = VTKClassesInExamples(source_path, coverage_dest, columns, excluded_columns, add_vtk_html)
    vtk_classes.build_tables()
    vtk_classes.generate_files()


if __name__ == '__main__':
    with ElapsedTime() as et:
        main()
    print('Time taken: {:0.3f}s'.format(et.interval))
