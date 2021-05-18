#!/usr/bin/env python

import json
import os
import random
import tempfile
from dataclasses import dataclass
from datetime import datetime
from operator import itemgetter
from pathlib import Path
from urllib.error import HTTPError
from urllib.request import urlretrieve


def get_program_parameters():
    import argparse
    description = 'Get examples that use a particular VTK class for a given language.'
    epilogue = '''
The JSON file needed by this script is obtained from the gh-pages branch
 of the vtk-examples GitHub site.
It is stored in your tempfile directory.
'''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('vtk_class', help='The desired VTK class.')
    parser.add_argument('language', help='The desired language, one of: CSharp, Cxx, Java, Python.')
    parser.add_argument('-a', '--all_values', action="store_true",
                        help='All examples (Warning: Can be a very long list).')
    parser.add_argument('-n', '--number', type=int, default=5, help='The maximum number of examples.')
    parser.add_argument('-m', '--md', action="store_true",
                        help='Display links in markdown inline format e.g. [label](URL).')

    args = parser.parse_args()
    return args.vtk_class, args.language, args.all_values, args.md, args.number


@dataclass(frozen=True)
class Links:
    """
    The URL to the JSON cross reference file.
    """
    xref_url: str = \
        'https://raw.githubusercontent.com/Kitware/vtk-examples/gh-pages/src/Coverage/vtk_vtk-examples_xref.json'


def download_file(dl_path, dl_url, overwrite=False):
    """
    Use the URL to get a file.

    :param dl_path: The path to download the file to.
    :param dl_url: The URL of the file.
    :param overwrite: If true, do a download even if the file exists.
    :return: The path to the file as a pathlib Path.
    """
    file_name = dl_url.split('/')[-1]

    # Create necessary sub-directories in the dl_path
    # (if they don't exist).
    Path(dl_path).mkdir(parents=True, exist_ok=True)
    # Download if it doesn't exist in the directory overriding if overwrite is True.
    path = Path(dl_path, file_name)
    if not path.is_file() or overwrite:
        try:
            urlretrieve(dl_url, path)
        except HTTPError as e:
            raise RuntimeError(f'Failed to download {dl_url}. {e.reason}')
    return path


def get_examples(d, vtk_class, lang, all_values=False, number=5, md_fmt=False):
    """
    For the VTK Class and language return the
     total number of examples and a list of examples.

    :param d: The dictionary.
    :param vtk_class: The VTK Class e.g. vtkActor.
    :param lang: The language, e.g. Cxx.
    :param all_values: True if all examples are needed.
    :param number: The number of values.
    :param md_fmt: Use Markdown format with label and URL defined together.
    :return: Total number of examples and a list of examples.
    """
    try:
        kv = d[vtk_class][lang].items()
    except KeyError:
        return None, None
    if len(kv) > number:
        if all_values:
            samples = list(kv)
        else:
            samples = random.sample(list(kv), number)
    else:
        samples = kv
    if md_fmt:
        links = [f'[{s.rsplit("/", 1)[1]}]({s})' for s in sorted(map(itemgetter(1), samples))]
    else:
        links = sorted(map(itemgetter(1), samples))
    return len(links), links


def get_crossref_dict(ref_dir):
    """
    Download and return the json cross-reference file.

    This function ensures that the dictionary is recent.

    :param ref_dir: The directory where the file will be downloaded
    :return: The dictionary cross-referencing vtk classes to examples
    """
    path = download_file(ref_dir, Links.xref_url, overwrite=False)
    if not path.is_file():
        print(f'The path: {str(path)} does not exist.')
        return None
    dt = datetime.today().timestamp() - os.path.getmtime(path)
    # Force a new download if the time difference is > 10 minutes.
    if dt > 600:
        path = download_file(ref_dir, Links.xref_url, overwrite=True)
    with open(path) as json_file:
        return json.load(json_file)


def main():
    vtk_class, language, all_values, md, number = get_program_parameters()
    language = language.lower()
    available_languages = {k.lower(): k for k in ['CSharp', 'Cxx', 'Java', 'Python']}
    available_languages.update({'cpp': 'Cxx', 'c++': 'Cxx', 'c#': 'CSharp'})
    if language not in available_languages:
        print(f'The language: {language} is not available.')
        tmp = ', '.join(sorted([lang for lang in set(available_languages.values())]))
        print(f'Choose one of these: {tmp}.')
        return
    else:
        language = available_languages[language]
    xref_dict = get_crossref_dict(tempfile.gettempdir())
    if xref_dict is None:
        return

    total_number, examples = get_examples(xref_dict, vtk_class, language, all_values=all_values, number=number,
                                          md_fmt=md)
    if examples:
        if total_number <= number or all_values:
            print(f'VTK Class: {vtk_class}, language: {language}\n'
                  f'Number of example(s): {total_number}.')
        else:
            print(f'VTK Class: {vtk_class}, language: {language}\n'
                  f'Number of example(s): {total_number} with {number} random sample(s) shown.')
        print('\n'.join(examples))
    else:
        print(f'No examples for the VTK Class: {vtk_class} and language: {language}')


if __name__ == '__main__':
    main()
