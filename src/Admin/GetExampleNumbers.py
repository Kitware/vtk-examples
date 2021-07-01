#!/usr/bin/env python

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


def count_examples(base_directory, languages):
    """
    Get the number of examples by language and total number of examples.

    :param base_directory: The path to the VTK example source files.
    :param languages: Folder and language name.
    :return: Language name and number of examples.
    """
    examples_count = dict()
    total = 0
    for k, v in languages.items():
        fn = Path(base_directory, k + '.md')
        # Does the directory exist?
        if not fn.is_file():
            raise RuntimeError(f'Non-existent folder: {str(fn)}')
        pattern = re.compile(r'^[\[][a-zA-Z0-9_ \-]+[\]][(]/' + k + '/')
        examples_count[v] = 0

        # Open and read the file counting the examples in it.
        subtotal = 0
        with open(fn) as f:
            for line in f:
                m = pattern.match(line)
                if m:
                    subtotal += 1
        examples_count[v] = subtotal
        total += subtotal
    examples_count['Total'] = total
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
    # Folder and language name.
    languages = {'CSharp': 'C#', 'Cxx': 'C++', 'Java': 'Java', 'Python': 'Python'}
    example_source = get_program_parameters()
    if not Path(example_source).is_dir():
        print(f'The path: {example_source} does not exist.')
    count = count_examples(example_source, languages)
    res = list()
    for k, v in count.items():
        res.append(f'{k:6s} {v:4d}')
    print('Number of examples.')
    print('\n'.join(res))


if __name__ == '__main__':
    with ElapsedTime() as et:
        main()
    print('\nTime taken: {:0.3f}s'.format(et.interval))
