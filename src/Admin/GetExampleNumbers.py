#!/usr/bin/env python
# -*- coding: utf-8 -*-

import re
import time
from pathlib import Path


def get_program_parameters():
    import argparse
    description = 'Count the number of examples.'
    epilogue = '''
'''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('vtk_examples', help='The path to the VTK example source files.')

    args = parser.parse_args()
    return args.vtk_examples


def count_examples(base_directory):
    example_types = ['CSharp', 'Cxx', 'Java', 'Python']
    examples_count = dict()
    for eg in example_types:
        fn = Path(base_directory, eg + '.md')
        # Does the directory exist?
        if not fn.is_file():
            raise RuntimeError(f'Non-existent folder: {str(fn)}')
        pattern = re.compile(r'^[\[][a-zA-Z0-9_ \-]+[\]][(]/' + eg + '/')
        examples_count[eg] = 0

        # Open and read the file counting the examples in it.
        with open(fn) as f:
            for line in f:
                m = pattern.match(line)
                if m:
                    examples_count[eg] += 1
    return examples_count


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


def main():
    example_source = get_program_parameters()
    if not Path(example_source).is_dir():
        print(f'The path: {example_source} does not exist.')
    count = count_examples(example_source)
    res = list()
    sum = 0
    for k, v in count.items():
        sum += v
        if k == 'Cxx':
            res.append(f'{"C++":6s} {v:4d}')
        else:
            res.append(f'{k:6s} {v:4d}')
    res.append(f'{"Total":6s} {sum:4d}')
    print('Number of examples.')
    print('\n'.join(res))


if __name__ == '__main__':
    with ElapsedTime() as et:
        main()
    print('Time taken: {:0.3f}s'.format(et.interval))
