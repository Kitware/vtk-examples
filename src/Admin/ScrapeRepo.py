#!/usr/bin/env python

import concurrent.futures
import contextlib
import hashlib
import multiprocessing
import os
import re
import shutil
import subprocess
import sys
import tarfile
import tempfile
import time
from collections import Counter, defaultdict
from pathlib import Path, PurePath
from urllib.parse import urlencode
from urllib.request import urlopen

try:
    import markdown
except ModuleNotFoundError:
    print("ScrapeRepo: " + "markdown package is required but not found.")
    print("ScrapeRepo: " + "install with `pip|conda install markdown`")
    exit(0)

#  Locks
vtk_modules_cache_lock = multiprocessing.Lock()
stats_lock = multiprocessing.Lock()


def get_program_parameters():
    import argparse
    description = 'Scrape the repository, editing where appropriate, and place files in the docs folder.'
    epilogue = '''

    Note:
       The time taken for the first run on this script will be around 370s as caches have to be created,
       subsequent runs take around 2-5s since existing caches will be used.
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('repo_dir', nargs='?', default='src',
                        help='The path to the folder containing the source files e.g. <local_path>/vtk-examples/src')
    parser.add_argument('site_url', help='The source repository URL e.g. https://gitlab.kitware.com/vtk/vtk-examples')
    parser.add_argument('web_site_url', help='The website URL e.g. https://<username>.github.io/<site_name>/site')
    parser.add_argument('web_repo_url', help='The website source URL e.g. https://github.com/<username>/<site_name>')
    parser.add_argument('web_repo_dir',
                        help='The path to the folder containing the web source files e.g. <local_path>/<site_name>')
    parser.add_argument('vtk_src_dir', help='The local directory containing the VTK source')
    args = parser.parse_args()

    return args.repo_dir, args.site_url, args.web_site_url, args.web_repo_url, args.web_repo_dir, args.vtk_src_dir


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


def create_html_ids(from_file, html_id_set):
    """
    Create a set of html ids.
    :param from_file:
    :param html_id_set:
    :return:
    """
    with open(from_file, 'r') as chapterLine:
        for line in chapterLine:
            id_found = re.findall(r'<figure[ ]*id="([^\"]*)">', line)
            if len(id_found) > 0:
                html_id_set.add(id_found[0])


def copy_chapter_add_links(from_to_file, html_id_set, stats):
    """
     Copy a file and add figure and doxygen links.

     Only add links to doxygen if we are outside code blocks delimited by ```
        and only add links to a figure if the figure exists.

    :param from_to_file: [from, to]
    :param html_id_set: A set of html ids
    :param stats: Statistics
    :return:
    """
    in_code = False
    with open(from_to_file[1], 'w') as ofh:
        with open(from_to_file[0], 'r') as chapter_line:
            for line in chapter_line:
                if line.count(r'```') % 2 != 0:
                    in_code = not in_code
                if not in_code:
                    # Make the language link relative, also drop off VTKBook.
                    line = re.sub(r'][ ]*\([ ]*/\w+/', r'](../', line)
                    line = add_vtk_nightly_doc_link(line, stats)
                    figure_found = sorted(re.findall(r'\*\*(Figure[^\*]*)\*\*', line), reverse=True)
                    if len(figure_found) > 0:
                        for figure in figure_found:
                            if figure in html_id_set:
                                line = line.replace(figure, r'<a href="#' + figure.replace(
                                    "Figure", "FIGURE") + '">**' + figure.replace(
                                    "Figure", "FIGURE") + r'**</a>')
                line = line.replace("FIGURE", "Figure")
                ofh.write(line)


def load_vtk_modules_cache(cache_path):
    """
    Load the VTK module cache into a dictionary.
    :param cache_path: The path to the VTK module cache.
    :return: The VTK module cache as a dictionary.
    """
    cache_dict = dict()
    if cache_path.is_file():
        lines = cache_path.read_text().split('\n')
        for line in lines:
            words = line.split(' ')
            if words:
                # Filter out any empty strings in the list.
                cache_dict[Path(words[0])] = list(filter(None, words[1:]))
    return cache_dict


def update_vtk_modules_cache(src_file, cmd_path, vtk_src_path, vtk_modules_cache, stats):
    """
    If the source code VTK modules are not in the cache, then get them.
    :param src_file: The source file path.
    :param cmd_path: The command to run.
    :param vtk_src_path: The path to the VTK source repository.
    :param vtk_modules_cache: The VTK modules cache.
    :param stats: Statistics
    :return:
    """

    # compute sha of src
    if src_file.exists() and src_file.is_file():
        with open(src_file, 'r') as ifh:
            src = ifh.read()
    else:
        print('The path: {} \nDoes not exist.'.format(src_file))
        return
    sha = hashlib.sha256(str.encode(src)).hexdigest()
    if src_file in vtk_modules_cache:
        if str(sha) == vtk_modules_cache[src_file][0]:
            with stats_lock:
                stats['vtk_modules_hits'] += 1
    else:
        with stats_lock:
            stats['vtk_modules_misses'] += 1
        if cmd_path.exists():
            try:
                cmd = str(cmd_path) + ' -p ' + str(vtk_src_path) + ' -s ' + str(src_file)
                process = subprocess.run(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, )
            except subprocess.CalledProcessError as err:
                print('ERROR:', err)
                if process.returncode != 0:
                    print('returncode:', process.returncode)
                    print('Have {} bytes in stdout:\n{}'.format(
                        len(process.stdout),
                        process.stdout.decode('utf-8'))
                    )
                    print('Have {} bytes in stderr:\n{}'.format(
                        len(process.stderr),
                        process.stderr.decode('utf-8'))
                    )
            result = process.stdout.decode('utf-8')
            vtk_modules = look_for_vtk_modules(result)
            with vtk_modules_cache_lock:
                vtk_modules_cache[src_file] = [sha] + vtk_modules
                print("VTK Modules: cache miss: ", str(src_file))
        else:
            s = 'Unable to continue, the path: {} \nDoes not exist.'.format(cmd_path)
            sys.exit(s)


def look_for_vtk_modules(str_to_parse):
    """
    Parse the string looking for any VTK modules.
    This string will have been generated by "WhatModulesVTK.py".

    :param str_to_parse: The string to parse
    :return: The VTK modules found.
    """
    words = str_to_parse.split('\n')
    vtk_modules = list()
    for word in words:
        if "find_package" in word:
            continue
        if ")" in word:
            continue
        if "Your" in word:
            continue
        if "All" in word:
            continue
        if word == "":
            continue
        vtk_modules.append(word.strip())
    return vtk_modules


def load_test_image_cache(cache_path):
    """
    Load the cache into a dictionary.
    :param cache_path: The path to the test image cache.
    :return: The tiny URL cache dictionary
    """
    cache_dict = dict()
    if cache_path.is_file():
        lines = cache_path.read_text().split('\n')
        for line in lines:
            words = line.split(' ')
            if len(words) == 2:
                cache_dict[words[0]] = words[1]
    return cache_dict


def make_tiny(url):
    """
    Make a tiny URL.
    :param url: The url to use.
    :return: The tiny URL.
    """
    request_url = ('http://tinyurl.com/api-create.php?' +
                   urlencode({'url': url}))
    with contextlib.closing(urlopen(request_url)) as response:
        return response.read().decode('utf-8')


def get_tiny_url(cache_dict, url, stats):
    """
    If the tiny URL is not in the cache, get it.
    :param cache_dict:
    :param url:
    :param stats: Statistics
    :return:
    """
    if url in list(cache_dict.keys()):
        stats['test_image_hits'] += 1
        return cache_dict[url]
    tiny_one = make_tiny(url)
    cache_dict[url] = tiny_one
    stats['test_image_misses'] += 1
    print("Test image cache miss: " + url)
    return cache_dict[url]


def make_tiny_url(url):
    """
    Given a URL get a tiny URL.
    :param url: The URL
    :return: Return the URL and the corresponding tiny URL.
    """
    return url, make_tiny(url)


def make_missing_tiny_urls(urls_to_shorten):
    """
    Set up a thread pool and make the missing tiny URLS.
    :param urls_to_shorten: The URLs to shorten.
    :return:
    """
    # The default value of max_workers is min(32, os.cpu_count() + 4) for Python 3.8 or greater
    with concurrent.futures.ThreadPoolExecutor() as executor:
        future_results = [executor.submit(make_tiny_url, k) for k in urls_to_shorten]
        # Block execution until all the tasks are completed
        concurrent.futures.wait(future_results)
        for future in future_results:
            yield future.result()
            # try:
            #     yield future.result()
            # except:
            #     print_exc()


def update_test_image_cache(test_image_cache, lines, stats):
    """
    Update the test_image_cache cache dictionary
    :param test_image_cache: The URL cache dictionary.
    :param lines: Lines possibly containing URLs.
    :param stats: Statistics
    :return:
    """
    #  Extract the lines that need tiny urls
    needed_urls = dict()
    urls_to_shorten = list()
    for k, v in lines.items():
        if v[0]:
            needed_urls[k] = v
    for k, v in needed_urls.items():
        if v[2] in list(test_image_cache.keys()):
            stats['test_image_hits'] += 1
        else:
            stats['test_image_misses'] += 1
            print("Test image cache miss: " + v[2])
            urls_to_shorten.append(v[2])
    res = make_missing_tiny_urls(urls_to_shorten)
    #  Update the cache
    for r in res:
        test_image_cache[r[0]] = r[1]


def add_image_link(test_image_cache, lines, stats):
    """
    Add a link to the image if one exists.
    :param test_image_cache: The cache of test images.
    :param lines: Lines some of which need an image.
    :param stats: Statistics
    :return:
    """
    update_test_image_cache(test_image_cache, lines, stats)
    for k, v in lines.items():
        if v[0]:
            #  Needs an image link.
            img = '<img class="lazy" style="float:center" data-src="' + test_image_cache[
                v[2]] + '?raw=true" width="64" />'
            s = ' | <a href="{}?raw=true target="_blank">{}\n</a>'.format(v[2], img)
            lines[k][2] = s
    return


def is_qt_example(example):
    """
    Is the example a Qt example?
    :param example: The example.
    :return: True if it is a Qt example.
    """
    reg = re.compile(r".*Qt.*", re.DOTALL)
    return reg.match(example)


def get_vtk_cmake_file(s):
    """
    Pull out the CMake file.
    :param s: The string containing the CMake file.
    :return: The CMake file.
    """
    reg = re.compile(r"```cmake(.*?)```", re.DOTALL)
    return [''.join(reg.findall(s))]


def write_extra_cxx_code(to_file, code_name, code):
    """
    Write C++ extra code to the file.
    :param to_file: The file to write to.
    :param code_name: The name of the example.
    :param code: The code to write to the file.
    :return:
    """
    to_file.write("**" + code_name + "**" + "\n")
    to_file.write("```c++" + "\n")
    to_file.write(code)
    to_file.write("```" + "\n")


def lines_with_vtk_classes(src_file_name):
    """
    Return A string of line numbers to highlight that have vtk class mentions.
    :param src_file_name: The source file
    :return: A string of line numbers to highlight.
    """
    with open(src_file_name, 'r') as ifh:
        lines = ifh.readlines()
        line_number = 1
        reg = re.compile(r'(vtk[0-9a-zA-Z]*)')
        hl_lines = []
        for line in lines:
            if reg.search(line):
                hl_lines.append(str(line_number))
            line_number += 1
        hl_lines = ' '.join(hl_lines)
        hl_lines = 'hl_lines="' + hl_lines + '"\n'
    return hl_lines


def find_other_given_lang(example, example_lang, other_lang, other_ext):
    """
    Given a language example return a link to another example if it exists.
    :param example: The example path.
    :param example_lang: The example language.
    :param other_lang: The other language.
    :param other_ext: The other extension.
    :return:
    """
    if other_lang == example_lang:
        return ''
    other_path = example
    parts = list(other_path.parts)
    for i, v in enumerate(reversed(parts)):
        if v == example_lang:
            idx = len(parts) - i - 1
            parts[idx] = other_lang
            break
    other_path = Path('/'.join(parts)).with_suffix(other_ext)
    if other_path.is_file():
        #  Now make a link to the path
        link_path = Path('/'.join(other_path.parts[-3:]))
        return f'([{other_lang}](../../../{link_path.parent}/{link_path.stem}))'
    return ''


def add_vtk_nightly_doc_link(s, stats):
    """
    If vtkXXXX is in the string, add a link to the doxygen file.
    :param s: The string.
    :param stats: Statistics
    :return:
    """
    # This is the name of the repository.
    repo_name = r'(vtk-examples)'
    # ?...? has been used to indicate that no link is to be built.
    no_link = r'[\?{1}](vtk[0-9a-zA-Z\-]+)[\?{1}]'
    link = r'(vtk[^ &:\.\-][0-9a-zA-Z]+)'

    reg1 = re.compile(repo_name)
    reg2 = re.compile(no_link)
    reg3 = re.compile(link)

    if reg1.findall(s):
        s = re.sub(repo_name, '_ktv' + r'\1', s)
    if reg2.findall(s):
        s = re.sub(no_link, '_ktv' + r'\1', s)
    s = s.replace('_ktvvtk', '_ktv_')
    n = len(reg3.findall(s))
    if n > 0:
        stats['doxy_count'] += n
        s = re.sub(link,
                   r'[\1](' + r'https://www.vtk.org/doc/nightly/html/class' + r'\1.html#details)', s)
    s = s.replace('_ktv_', 'vtk')
    return s


def get_example_line(s):
    """
    Get the example line.
    :param s: A line of text.
    :return: The example line if it exists.
    """
    reg = re.compile(r'^\[[^\(]*\]\(([^)]*)')
    if reg.match(s):
        return [''.join(reg.findall(s))]
    return ['']


def add_thumbnails_and_links(web_repo_url, src_path, doc_path, baseline_path, test_images, from_file, to_file,
                             stats):
    """
    Add thumbnails, and language links, then copy to the doc_dir.
    :param web_repo_url: The repository URL.
    :param src_path: Path to the source folder e.g. vtk-examples/src.
    :param doc_path: Path to the docs folder e.g. vtk-examples-doc/docs.
    :param baseline_path: The baseline directories relative to root_path/repo_dir.
    :param test_images: The cache containing the test images.
    :param from_file: The file to copy/edit
    :param to_file: The save file name.
    :param stats: Statistics.
    :return:
    """
    from_path = src_path / from_file
    to_path = doc_path / to_file
    # We need to treat [Lang]HowTo.md files, e.g. CxxHowTo.md, in a special way
    has_how_to = False
    if 'HowTo' in from_file:
        lang = re.split('HowTo', from_file)[0]
        has_how_to = True
    with open(from_path, 'r') as md_file:
        lines = dict()
        line_count = 0
        x = []
        for line in md_file:
            example_line = get_example_line(line.strip())[0]
            with_doxy = add_vtk_nightly_doc_link(line, stats)
            x.append(False)
            x.append(with_doxy.rstrip())
            if example_line != '':
                stats['thumb_count'] += 1
                example_path = PurePath(example_line)
                example_path = example_path.with_name('Test' + example_path.stem + '.png')
                baseline = baseline_path.joinpath(example_path.relative_to('/'))
                if baseline.is_file():
                    baseline_url = '/'.join([web_repo_url, 'blob/gh-pages', 'src/Testing/Baseline',
                                             str(example_path.relative_to('/').parent), example_path.name])
                    x[0] = True
                    x.append(baseline_url)
            lines[line_count] = x
            line_count += 1
            x = []
        add_image_link(test_images, lines, stats)
        if from_file != 'VTKBookFigures.md':
            if not has_how_to:
                for k, v in lines.items():
                    line_changed = False
                    if v[1] != '':
                        if '](/Coverage' in v[1]:
                            # Make the coverage link relative.
                            v[1] = re.sub(r'][ ]*\([ ]*/', r'](', v[1])
                            v[1] = v[1].replace('.md', '')
                            line_changed = True
                        if '/CSharp/' in v[1] or '/Cxx/' in v[1] or '/Java/' in v[1] or '/Python/' in v[1]:
                            # Make the language link relative, also drop off the language.
                            v[1] = re.sub(r'][ ]*\([ ]*/\w+/', r'](', v[1])
                            line_changed = True
                    if line_changed:
                        lines[k] = v
            else:
                for k, v in lines.items():
                    line_changed = False
                    if v[1] != '':
                        if '/CSharp/' in v[1] or '/Cxx/' in v[1] or '/Java/' in v[1] or '/Python/' in v[1]:
                            # Make the language link relative to the src folder.
                            link_head = r'](' + r'../' + lang + r'/'
                            if '.md' in v[1]:
                                v[1] = v[1].replace('.md', '')
                            v[1] = re.sub(r'][ ]*\([ ]*/\w+/', link_head, v[1])
                            line_changed = True
                    if line_changed:
                        lines[k] = v
        else:
            for k, v in lines.items():
                line_changed = False
                if v[1] != '':
                    v[1] = re.sub(r'][ ]*\([ ]*/', r'](../', v[1])
                    line_changed = True
                if line_changed:
                    lines[k] = v

    with open(to_path, 'w') as ofn:
        k = sorted(lines.keys())
        for kv in k:
            line = ''.join(lines[kv][1:])
            ofn.write(line + '\n')


def add_trame_thumbnails_and_links(web_repo_url, src_path, doc_path, baseline_path, test_images, from_file, to_file,
                             stats):
    """
    Add thumbnails, and language links, then copy to the doc_dir.
    :param web_repo_url: The repository URL.
    :param src_path: Path to the source folder e.g. vtk-examples/src.
    :param doc_path: Path to the docs folder e.g. vtk-examples-doc/docs.
    :param baseline_path: The baseline directories relative to root_path/repo_dir.
    :param test_images: The cache containing the test images.
    :param from_file: The file to copy/edit
    :param to_file: The save file name.
    :param stats: Statistics.
    :return:
    """
    from_path = src_path / from_file
    to_path = doc_path / to_file
    with open(from_path, 'r') as md_file:
        lines = dict()
        line_count = 0
        x = []
        for line in md_file:
            example_line = get_example_line(line.strip())[0]
            with_doxy = add_vtk_nightly_doc_link(line, stats)
            x.append(False)
            x.append(with_doxy.rstrip())
            if example_line != '':
                stats['thumb_count'] += 1
                example_path = PurePath(example_line)
                example_path = example_path.with_name('Test' + example_path.stem + '.png')
                baseline = baseline_path.joinpath(example_path.relative_to('/'))
                if baseline.is_file():
                    baseline_url = '/'.join([web_repo_url, 'blob/gh-pages', 'src/Testing/Baseline',
                                             str(example_path.relative_to('/').parent), example_path.name])
                    x[0] = True
                    x.append(baseline_url)
            lines[line_count] = x
            line_count += 1
            x = []
        add_image_link(test_images, lines, stats)
        for k, v in lines.items():
            line_changed = False
            if v[1] != '':
                if '](/Coverage' in v[1]:
                    # Make the coverage link relative.
                    v[1] = re.sub(r'][ ]*\([ ]*/', r'](', v[1])
                    v[1] = v[1].replace('.md', '')
                    line_changed = True
                if '/CSharp/' in v[1] or '/Cxx/' in v[1] or '/Java/' in v[1] or '/Python/' in v[1]:
                    # Make the language link relative, also drop off the language.
                    v[1] = re.sub(r'][ ]*\([ ]*/\w+/', r'](', v[1])
                    line_changed = True
            if line_changed:
                lines[k] = v

        # if from_file != 'VTKBookFigures.md':
        #     if not has_how_to:
        #         for k, v in lines.items():
        #             line_changed = False
        #             if v[1] != '':
        #                 if '](/Coverage' in v[1]:
        #                     # Make the coverage link relative.
        #                     v[1] = re.sub(r'][ ]*\([ ]*/', r'](', v[1])
        #                     v[1] = v[1].replace('.md', '')
        #                     line_changed = True
        #                 if '/CSharp/' in v[1] or '/Cxx/' in v[1] or '/Java/' in v[1] or '/Python/' in v[1]:
        #                     # Make the language link relative, also drop off the language.
        #                     v[1] = re.sub(r'][ ]*\([ ]*/\w+/', r'](', v[1])
        #                     line_changed = True
        #             if line_changed:
        #                 lines[k] = v
        #     else:
        #         for k, v in lines.items():
        #             line_changed = False
        #             if v[1] != '':
        #                 if '/CSharp/' in v[1] or '/Cxx/' in v[1] or '/Java/' in v[1] or '/Python/' in v[1]:
        #                     # Make the language link relative to the src folder.
        #                     link_head = r'](' + r'../' + lang + r'/'
        #                     if '.md' in v[1]:
        #                         v[1] = v[1].replace('.md', '')
        #                     v[1] = re.sub(r'][ ]*\([ ]*/\w+/', link_head, v[1])
        #                     line_changed = True
        #             if line_changed:
        #                 lines[k] = v
        # else:
        #     for k, v in lines.items():
        #         line_changed = False
        #         if v[1] != '':
        #             v[1] = re.sub(r'][ ]*\([ ]*/', r'](../', v[1])
        #             line_changed = True
        #         if line_changed:
        #             lines[k] = v

    with open(to_path, 'w') as ofn:
        k = sorted(lines.keys())
        for kv in k:
            line = ''.join(lines[kv][1:])
            ofn.write(line + '\n')


def fill_cmake_lists(cmake_contents, example_name, extra_names, vtk_modules, web_repo_url):
    """
    Fill in the template parameters in a CMakeLists template file.
    The output is a CMakeLists.txt file with Name substituted for {{{1}}}
    :param cmake_contents: The template file.
    :param example_name: The example file name.
    :param extra_names: Any needed extra files needed to build the example.
    :param vtk_modules: The VTK modules e.g. vtkCommonCore ... in a string.
    :param web_repo_url: The web repository URL.
    :return: A CMakeLists.txt file.
    """
    r1 = re.sub(r'WWW', web_repo_url, cmake_contents)
    r2 = re.sub(r'XXX', example_name.stem, r1)
    r3 = re.sub(r'YYY', extra_names, r2)
    r4 = re.sub(r'ZZZ', vtk_modules, r3)
    return r4


def fill_qt_cmake_lists(cmake_contents, example_name, vtk_modules, web_repo_url):
    """
    Fill in the template parameters in a CMakeLists template file.
    The output is a CMakeLists.txt file with Name substituted for {{{1}}}
    :param cmake_contents: The template file.
    :param example_name: The example file name.
    :param vtk_modules: The VTK modules e.g. vtkCommonCore ... in a string.
    :param web_repo_url: The web repository URL.
    :return: A CMakeLists.txt file
    """
    r1 = re.sub(r'WWW', web_repo_url, cmake_contents)
    r2 = re.sub(r'XXX', example_name.stem, r1)
    r3 = re.sub(r'ZZZ', vtk_modules, r2)
    return r3


def fill_trame_instructions(trame_contents, example_name, web_repo_url):
    """
    Fill in the template parameters for the trame template file.

    :param trame_contents: The template file.
    :param example_name: The example file name.
    :param web_repo_url: The web repository URL.
    :return: A text file.
    """
    r1 = re.sub(r'WWW', web_repo_url, trame_contents)
    r2 = re.sub(r'XXX', example_name.stem, r1)
    return r2


def get_example_paths(src_path, available_languages, example_paths):
    """
    Get the examples paths and any supplementary files.
    :param src_path:
    :param available_languages:
    :param example_paths:
    :return:
    """
    excluded_paths = ['Boneyard', 'Broken', 'Deprecated', 'Untested', 'Databases', 'Problems', 'Wishlist']
    # Scan all Cxx directories and look for extras.
    # This holds any extra files needed for an example,
    # key is the extra file path and the set holds the additional paths needed by an example.
    all_extras = defaultdict(set)
    for path in (src_path / 'Cxx').iterdir():
        if path.is_dir() and path.parts[-1] not in excluded_paths:
            files = path.glob('*.extras')
            for f in files:
                with open(f, 'r') as ifn:
                    for line in ifn:
                        line = line.strip()
                        all_extras[f.with_suffix(available_languages['Cxx'])].add(f.parent / line)
    # Now get all the examples.
    for lang, lang_ext in available_languages.items():
        for path in (src_path / lang).iterdir():
            if path.is_dir() and path.parts[-1] not in excluded_paths:
                files = path.glob('*' + lang_ext)
                for f in files:
                    if f in all_extras:
                        example_paths[lang][f] = all_extras[f]
                    else:
                        example_paths[lang][f] = set()
    for lang in available_languages:
        # Remove the files in all_extras that have been picked up.
        for k, v in all_extras.items():
            for vv in v:
                if vv in example_paths[lang]:
                    example_paths[lang].pop(vv)

def extract_paths(src_path, fn):
    """
    Parse the markdown file extracting the paths in the tables.

    :param src_path: The path to the sources.
    :param fn: The top level markdown file with the extension e.g. trame.md.
    :return:  A list of the paths
    """
    """
    :return:
    """
    row = re.compile(r'^.*(\[.+\]\(.+\)).*\|.+$')
    row_key = re.compile(r'\[(.*?)\].*\((.*?)\)')
    paths = list()
    path = Path(src_path) / fn
    if path.is_file():
        lines = path.read_text().split('\n')
        for line in lines:
            if row.match(line):
                kv = line.split('|')[0].strip()
                m = row_key.match(kv)
                if m:
                    paths.append(m.group(2))
    return paths



def get_trame_paths(src_path, trame_md):
    """
    Get the examples paths and any supplementary files.
    :param src_path:
    :param available_languages:
    :param example_paths:
    :return:
    """
    excluded_paths = ['data']
    # Scan all Cxx directories and look for extras.
    # This holds any extra files needed for an example,
    # key is the extra file path and the set holds the additional paths needed by an example.
    all_extras = defaultdict(set)
    for path in (src_path / 'Cxx').iterdir():
        if path.is_dir() and path.parts[-1] not in excluded_paths:
            files = path.glob('*.extras')
            for f in files:
                with open(f, 'r') as ifn:
                    for line in ifn:
                        line = line.strip()
                        all_extras[f.with_suffix(available_languages['Cxx'])].add(f.parent / line)
    # Now get all the examples.
    for lang, lang_ext in available_languages.items():
        for path in (src_path / lang).iterdir():
            if path.is_dir() and path.parts[-1] not in excluded_paths:
                files = path.glob('*' + lang_ext)
                for f in files:
                    if f in all_extras:
                        example_paths[lang][f] = all_extras[f]
                    else:
                        example_paths[lang][f] = set()
    for lang in available_languages:
        # Remove the files in all_extras that have been picked up.
        for k, v in all_extras.items():
            for vv in v:
                if vv in example_paths[lang]:
                    example_paths[lang].pop(vv)


def make_markdown_example_page(example_paths, available_languages, src_path, doc_path,
                               repo_name, web_repo_url, vtk_modules_cache,
                               example_to_CMake, stats):
    """

    :param example_paths: Example paths
    :param available_languages: Available languages
    :param src_path: The path to the sources.
    :param doc_path: The path to the docs.
    :param repo_name: The repository name.
    :param web_repo_url: The web repository URL.
    :param vtk_modules_cache: The VTK modules cache.
    :param example_to_CMake: A dictionary to hold CMakeLists.txt files.
    :param stats: Statistics
    :return:
    """
    suffix_lang = dict()
    for key, value in available_languages.items():
        if value in suffix_lang:
            suffix_lang[value].append(key)
        else:
            suffix_lang[value] = [key]
    cmake_qt_template = src_path / '/'.join(['Admin', 'VTKQtCMakeLists'])
    cmake_template = src_path / '/'.join(['Admin', 'VTKCMakeLists'])
    for lang in example_paths:
        for source_path in example_paths[lang]:
            other_languages = list()
            for a_lang, a_ext in list(available_languages.items()):
                if lang != a_lang:
                    other_link = find_other_given_lang(source_path, lang, a_lang, a_ext)
                    if other_link != '':
                        other_languages.append(other_link)
            parts = source_path.parts
            baseline_path = src_path / '/'.join(
                ['Testing', 'Baseline', parts[-3], parts[-2], 'Test' + source_path.stem + '.png'])
            dest = doc_path / '/'.join([parts[-3], parts[-2], source_path.stem + '.md'])
            dest.parent.mkdir(parents=True, exist_ok=True)
            # Generate markdown for the example web page
            with open(dest, 'w') as md_file:
                md_file.write(
                    '[' + repo_name + '](/)/[' + lang + '](/' + lang + ')/' + parts[
                        -2] + '/' + source_path.stem + '\n\n')
                if baseline_path.is_file():
                    image_url = '/'.join([web_repo_url, 'blob/gh-pages/src/Testing/Baseline', parts[-3], parts[-2],
                                          'Test' + source_path.stem + '.png?raw=true'])
                    # href to open image in new tab
                    md_file.write('<a href="' + image_url + ' target="_blank">' + '\n')
                    md_file.write(
                        '<img style="border:2px solid beige;float:center" src="' + image_url + '" width="256" />' + '\n')
                    md_file.write('</a>' + '\n')
                    md_file.write('<hr>\n')
                    md_file.write('\n')

                description_path = src_path / '/'.join([parts[-3], parts[-2], source_path.stem + ".md"])
                # Add a description if a .md file exists for the example
                if description_path.exists() and description_path.is_file():
                    with open(description_path, 'r') as description_file:
                        description = description_file.read()
                    description = add_vtk_nightly_doc_link(description, stats)
                    md_file.write(description)
                # Add examples from other available languages if they exist
                if len(other_languages) > 0:
                    see_also = '\n!!! Tip "Other languages"\n'
                    see = '    See '
                    for other in other_languages:
                        see_also += see + other
                        see = ', '
                    see_also += '\n'
                    md_file.write(see_also)
                    md_file.write('\n')

                question = [
                    '!!! question\n    ',
                    'If you have a question about this example,',
                    ' please use the [VTK Discourse Forum](https://discourse.vtk.org/)\n\n'
                ]
                md_file.write(''.join(question))

                # Get the source code and highlight it.
                with open(source_path, 'r') as ifh:
                    src = ifh.read()
                hilite_lines = lines_with_vtk_classes(source_path)
                md_file.write('###Code\n')
                md_file.write('**' + source_path.name + '**' + '\n')
                vtk_modules = None
                if source_path.suffix == '.cxx':
                    md_file.write('``` c++ ' + hilite_lines + '\n')
                    # Get the vtk_modules used in this example
                    vtk_modules = vtk_modules_cache[source_path][1:]
                    stats['cxx_count'] += 1
                elif source_path.suffix == '.cs':
                    md_file.write('``` csharp ' + hilite_lines + '\n')
                    stats['cs_count'] += 1
                elif source_path.suffix == '.py':
                    md_file.write('``` python ' + hilite_lines + '\n')
                    stats['py_count'] += 1
                elif source_path.suffix == '.java':
                    md_file.write('``` java ' + hilite_lines + '\n')
                    stats['java_count'] += 1
                md_file.write(src)
                md_file.write('```' + '\n')

                # Check for extras.
                extra_names = ''
                if example_paths[lang][source_path]:
                    for path in example_paths[lang][source_path]:
                        if path.suffix == available_languages['Cxx']:
                            extra_names += ' ' + path.name
                        with open(path, 'r') as extra_fh:
                            extra_code = extra_fh.read()
                        hilite_lines = lines_with_vtk_classes(path)
                        md_file.write('**' + path.name + '**' + '\n')
                        md_file.write('``` c++ ' + hilite_lines + '\n')
                        md_file.write(extra_code)
                        md_file.write("```" + "\n")
                else:
                    pass

                # Check for a CMake file.
                custom_CMake_path = source_path.with_suffix('.cmake')
                if os.path.isfile(custom_CMake_path):
                    with open(custom_CMake_path, 'r') as ifh:
                        cmake = ifh.read()
                else:
                    if parts[-3] == 'Cxx':
                        # Use the templates to generate the CMake file.
                        if is_qt_example(src):
                            with open(cmake_qt_template, 'r') as CMakeFile:
                                CMake_contents = CMakeFile.read()
                            # Create component lines for the VTK modules
                            needed_modules = ''
                            for vtk_module in vtk_modules:
                                if 'vtk' in vtk_module:
                                    needed_modules += '\n  ' + vtk_module
                                else:
                                    needed_modules += '\n  ' + 'vtk' + vtk_module
                            if needed_modules == '':
                                pass
                            cmake = fill_qt_cmake_lists(CMake_contents, source_path, needed_modules, web_repo_url)
                        else:
                            with open(cmake_template, 'r') as CMakeFile:
                                CMake_contents = CMakeFile.read()
                            # Create component lines for the VTK modules
                            needed_modules = ''
                            for vtk_module in vtk_modules:
                                if 'vtk' in vtk_module:
                                    needed_modules += '\n  ' + vtk_module
                                else:
                                    needed_modules += '\n  ' + 'vtk' + vtk_module
                            cmake = fill_cmake_lists(CMake_contents, source_path, extra_names, needed_modules,
                                                     web_repo_url)

                if parts[-3] == 'Cxx':
                    example_to_CMake[source_path] = get_vtk_cmake_file(cmake)
                    md_file.write(cmake)


def make_instruction_pages(web_repo_url, web_site_url, site_repo_url, src_path, doc_path, from_file,
                           to_file):
    """
    Make the instruction pages. The keys in the dictionary patterns are used to replace the
    corresponding keys in the instructions.

    :param web_repo_url: The web repository URL.
    :param web_site_url: The web site URL.
    :param site_repo_url: The URL corresponding to the source files repository.
    :param src_path: Path to the src folder e.g. vtk-examples/src.
    :param doc_path: Usually 'docs'.
    :param from_file: The file to copy/edit
    :param to_file: The save file name.
    :return:
    """
    src = src_path / '/'.join(['Instructions', from_file])
    dest = doc_path / '/'.join(['Instructions', to_file])

    patterns = {'__BLOB__': site_repo_url + '/blob/master',
                '__TREE__': site_repo_url + '/tree/master',
                '__SITE_REPOSITORY__': site_repo_url,
                '__ARCHIVE__': site_repo_url + '/archive/master.zip',
                '__GIT_REPO__': site_repo_url + '.git',
                '__REPO_NAME__': list(filter(None, site_repo_url.split('/')))[-1],
                '__USER_NAME__': list(filter(None, site_repo_url.split('/')))[-2],
                '__WEB_BLOB__': web_repo_url + '/blob/gh-pages',
                '__WEB_SITE__': web_repo_url + '/site',
                '__WEB_SITE_URL__': web_site_url,
                '__WEB_REPOSITORY__': web_repo_url,
                '__WEB_REPO_NAME__': list(filter(None, web_repo_url.split('/')))[-1],
                '__WEB_USER_NAME__': list(filter(None, web_repo_url.split('/')))[-2],
                }
    keys = patterns.keys()
    with open(src, 'r') as ifh:
        lines = ifh.readlines()
        lines_to_update = list()
    for idx in range(0, len(lines)):
        if any(k in lines[idx] for k in keys):
            lines_to_update.append(idx)
    for idx in lines_to_update:
        for k in keys:
            lines[idx] = re.sub(k, patterns[k], lines[idx])
    with open(dest, 'w') as ofh:
        for line in lines:
            ofh.write(line)


def make_examples_sources_html(example_paths, src_path, doc_path, web_repo_url, web_site_url):
    """
    Make HTML pages of examples, images and sources.

    :param example_paths: The examples.
    :param src_path: Path to the src folder e.g. vtk-examples/src.
    :param doc_path: The path to the docs.
    :param web_repo_url: The web repository URL.
    :param web_site_url: The web site URL.
    :return:
    """
    doctype = '<!doctype html>'
    style = '\n'.join([
        '  <style>',
        '  h1 {text-align:center;}',
        '  h2 {text-align:left;}',
        '  h3 {text-align:left;}',
        '  h4 {text-align:left;}',
        '  h5 {text-align:left;}',
        '  h6 {text-align:left;}',
        '  p {text-align:left;}',
        '  table {font-family: arial, sans-serif;border-collapse: collapse;}',
        '  th {border: 1px solid #dddddd;text-align: center;padding: 8px;}',
        '  td {border: 1px solid #dddddd;text-align: left;padding: 8px;}',
        '  tr {font-family: monospace;}',
        '  tr:nth-child(even) {background-color: #eeeeee;}',
        '  table.time_header td.time_fmt {text-align: left;}',
        '  .cour {font-family: Courier;}',
        '  html, body {height: 100%;}',
        '  html {display: table;margin: auto;}',
        '  body {display: table-cell;vertical-align: middle;}',
        '  thead {color: DarkGreen;}',
        '  tbody {color: MidnightBlue;}',
        '  tfoot {color: SaddleBrown;',
        '  </style>',
    ])
    counters = dict()
    langs = list()
    for lang in example_paths:
        counters[lang] = 0
        langs.append(example_paths[lang])
    for lang in example_paths:
        title = f'Examples_Sources_{lang}'
        ret = f'{doctype}\n<html lang="en">\n<head>\n  <meta charset="utf-8">\n  <title>{title}</title>{style}</head>\n'

        contents = ['<h1>Examples and their source</h1>',
                    '<table style="margin: auto;">',
                    '  <caption>',
                    f'    <b>{lang}</b>',
                    '  </caption>',
                    '<thead>',
                    '  <tr>',
                    '    <th>Example</th>',
                    '    <th>Figure</th>',
                    '    <th>Source</th>',
                    '  </tr>',
                    '</thead>',
                    ]
        source_paths = list()
        for p in example_paths[lang].keys():
            source_paths.append(str(p))
        source_paths = sorted(source_paths)
        for p in source_paths:
            source_path = Path(p)
            parts = source_path.parts
            p = '/'.join(list(source_path.parts[-3:-1]) + [source_path.stem])
            eg = f'<A HREF={web_site_url}/{p}/>{source_path.stem}</A>'
            p = '/'.join(['blob/master'] + list(source_path.parts[-4:-1]) + [source_path.name])
            src = f'<A HREF={web_repo_url}/{p}/>{source_path.name}</A>'
            baseline_path = src_path / '/'.join(
                ['Testing', 'Baseline', parts[-3], parts[-2], 'Test' + source_path.stem + '.png'])
            if baseline_path.exists() and baseline_path.is_file():
                image_url = '/'.join([web_repo_url, 'blob/gh-pages/src/Testing/Baseline', parts[-3], parts[-2],
                                      'Test' + source_path.stem + '.png?raw=true'])
                # This opens the target in a new tab.
                image_link = '<a href="' + image_url + ' target="_blank">' + '\n'
                # This displays the image on the page.
                image_link += '<img style="border:2px solid beige;float:center" src="' + image_url + '" width="256" />'
            else:
                image_link = ''
            contents += [f'      <td>{eg}</td>\n    <td>{image_link}</td>\n    <td>{src}</td>\n  </tr>\n']
            counters[lang] += 1
        contents += ['  </table>']
        footer = [
            '  <footer class="cour">',
            '    Number of examples<br>',
        ]
        d = 6
        sk = f'{lang:6s}'.replace(' ', '&nbsp;')
        sv = f'{counters[lang]:{d}d}'.replace(' ', '&nbsp;')
        footer.append(f'    {sk}: {sv}<br>')
        footer.append('  </footer>')
        ret += '\n'.join(contents)
        ret += '\n'.join(footer)

        with open(doc_path / f'Examples_Sources_{lang}.html', 'w') as index_file:
            index_file.write(ret)


def make_tarball(src, dest):
    """
    Create the tar file for the example.
    The tarballs are stored in the tree docs/Tarballs.

    :param src: The source folder.
    :param dest: The destination file name.
    :return:
    """
    tar = tarfile.open(dest, 'w')
    tar.add(src, arcname=dest.stem)
    tar.close()


def make_cxx_tarballs(web_repo_dir, example_paths, example_to_CMake, ref_mtime, stats):
    """
    Create tarballs for each example.
    :param web_repo_dir: Repository path.
    :param example_paths:  The examples.
    :param example_to_CMake: A dictionary to holding the CMakeLists.txt file.
    :param ref_mtime: The reference mtime.
    :param stats: Statistics.
    :return:
    """

    tmp_dir = Path(tempfile.mkdtemp(prefix='VTKTarballs') + '/') / 'Cxx'

    # Create the Tarballs directory in the source tree if not present
    tar_dir = Path(web_repo_dir) / 'Tarballs' / 'Cxx'
    tar_dir.mkdir(parents=True, exist_ok=True)

    # Create tarballs
    # For each example page, create a directory and copy that example's files
    # into the directory
    # If the example has a CMakeLists.txt file, copy that.
    # Also, create a subdir called build. This directory is handy when you want to
    # configure with CMake and build the example.
    tarball_args = list()
    for example in example_paths['Cxx']:
        if example not in example_to_CMake:
            continue
        # Make the temporary directories for the example
        tar_tmp_path = tmp_dir / example.stem
        tar_tmp_path.mkdir(parents=True, exist_ok=True)
        tar_fn = tar_tmp_path / example.name
        # Copy the code for the example
        shutil.copy(example, tar_fn)
        os.utime(tar_fn, (ref_mtime, ref_mtime))

        # An example may have multiple source files
        for example_extras in example_paths['Cxx'][example]:
            # Get the file name
            tar_fn = tar_tmp_path / example_extras.name
            # Copy the code for the example
            shutil.copy(example_extras, tar_fn)
            os.utime(tar_fn, (ref_mtime, ref_mtime))

        # Some examples do not have a CMakeLists.txt file
        if example in example_to_CMake:
            (tar_tmp_path / 'build').mkdir(parents=True, exist_ok=True)
            cmake_fn = tar_tmp_path / 'CMakeLists.txt'
            with open(cmake_fn, 'w') as cmake_fh:
                cmake_fh.write(example_to_CMake[example][0])
            os.utime(cmake_fn, (ref_mtime, ref_mtime))
            os.utime(tar_tmp_path / 'build', (ref_mtime, ref_mtime))

        # Set the mtimes for the directories containing the example
        # Since the mtime is stored in the tar header for each file and directory,
        # we need a consistent time so that a tar of an unchanged example will be equal
        # to the one in the repo
        os.utime(tar_tmp_path, (ref_mtime, ref_mtime))

        tarball_args.append((tar_tmp_path, tar_dir / (example.stem + '.tar')))

    # The default value of max_workers is min(32, os.cpu_count() + 4) for Python 3.8 or greater
    with concurrent.futures.ThreadPoolExecutor() as executor:
        future_results = [executor.submit(make_tarball, *k) for k in tarball_args]
        # Block execution until all the tasks are completed
        concurrent.futures.wait(future_results)
        # for future in future_results:
        #     yield future.result()

    stats['cxx_tar_count'] = len(tarball_args)
    os.utime(tmp_dir, (0, ref_mtime))
    # Cleanup the temporary directories
    shutil.rmtree(tmp_dir)

def make_trame_tarballs(web_repo_dir, src_path, example_paths, ref_mtime, stats):
    """
    Create tarballs for each example.
    :param web_repo_dir: Repository path.
    :param example_paths:  The examples.
    :param src_path:  The path to the sources.
    :param ref_mtime: The reference mtime.
    :param stats: Statistics.
    :return:
    """

    tmp_dir = Path(tempfile.mkdtemp(prefix='VTKTarballs') + '/')

    # Create the Tarballs directory in the source tree if not present
    tar_dir = Path(web_repo_dir) / 'Tarballs'
    tar_dir.mkdir(parents=True, exist_ok=True)

    # Create tarballs
    # For each example, create a directory and copy that example's files
    # into the directory
    tarball_args = list()
    for example in example_paths:
        example_path = PurePath(example)
        tar_tmp_path = tmp_dir / example_path.parent.relative_to(example_path.parent.anchor)
        tar_tmp_path.mkdir(parents=True, exist_ok=True)
        tar_fn = tar_tmp_path / example_path.name
        source_path =  Path(src_path) / example_path.relative_to(example_path.anchor)
        shutil.copytree(source_path, tar_fn)
        os.utime(tar_fn, (ref_mtime, ref_mtime))

        # Set the mtimes for the directories containing the example
        # Since the mtime is stored in the tar header for each file and directory,
        # we need a consistent time so that a tar of an unchanged example will be equal
        # to the one in the repo
        os.utime(tar_tmp_path, (ref_mtime, ref_mtime))
        destination_path = tar_dir / example_path.relative_to(example_path.anchor).with_suffix('.tar')
        destination_path.parent.mkdir(parents=True, exist_ok=True)
        tarball_args.append((tar_fn, tar_dir / destination_path))

    # The default value of max_workers is min(32, os.cpu_count() + 4) for Python 3.8 or greater
    with concurrent.futures.ThreadPoolExecutor() as executor:
        future_results = [executor.submit(make_tarball, *k) for k in tarball_args]
        # Block execution until all the tasks are completed
        concurrent.futures.wait(future_results)
        # for future in future_results:
        #     yield future.result()

    stats['trame_tar_count'] = len(tarball_args)
    os.utime(tmp_dir, (0, ref_mtime))
    # Cleanup the temporary directories
    shutil.rmtree(tmp_dir)


def get_statistics(stats):
    """
    Extract the statistics.

    :param stats: The statistics.
    :return: A list of statistics.
    """
    totals = list()
    for k, v in stats.items():
        if k in ['cxx_count', 'cs_count', 'py_count', 'java_count']:
            totals.append(v)
    res = list()
    res.append('ScrapeRepo Summary')
    res.append('  C++ examples:             ' + str(stats['cxx_count']))
    res.append('  CSharp examples:          ' + str(stats['cs_count']))
    res.append('  Python examples:          ' + str(stats['py_count']))
    res.append('  Java examples:            ' + str(stats['java_count']))
    res.append('  Total examples:           ' + str(sum(totals)))
    res.append('  Tarballs C++:             ' + str(stats['cxx_tar_count']))
    res.append('  Tarballs trame:           ' + str(stats['trame_tar_count']))
    res.append('  Doxygen added:            ' + str(stats['doxy_count']))
    res.append('  Thumbnails added:         ' + str(stats['thumb_count']))
    res.append('  Test Image Cache hits:    ' + str(stats['test_image_hits']))
    res.append('  Test Image Cache misses:  ' + str(stats['test_image_misses']))
    res.append('  VTK Modules Cache hits:   ' + str(stats['vtk_modules_hits']))
    res.append('  VTK Modules Cache misses: ' + str(stats['vtk_modules_misses']))
    return res


def main():
    # Initialize the statistics
    stats = Counter()
    stats['test_image_hits'] = 0
    stats['test_image_misses'] = 0
    stats['vtk_modules_hits'] = 0
    stats['vtk_modules_misses'] = 0
    stats['cxx_count'] = 0
    stats['cs_count'] = 0
    stats['py_count'] = 0
    stats['java_count'] = 0
    stats['trame_count'] = 0
    stats['cxx_tar_count'] = 0
    stats['trame_tar_count'] = 0
    stats['thumb_count'] = 0
    stats['doxy_count'] = 0

    repo_dir, site_url, web_site_url, web_repo_url, web_repo_dir, vtk_src_dir = get_program_parameters()
    print('Paths and folders to use:')
    print('REPO_DIR:      ', repo_dir)
    print('SITE_URL:      ', site_url)
    print('WEB_SITE_URL:  ', web_site_url)
    print('WEB_REPO_URL:  ', web_repo_url)
    print('WEB_REPO_DIR:  ', web_repo_dir)
    print('VTK_SOURCE_DIR:', vtk_src_dir)
    print()

    web_repo_path = Path(web_repo_dir)
    vtk_src_path = Path(vtk_src_dir)
    # Find the repository path, this program resides in the Admin folder
    # so go up two levels and append the repo_dir.
    root_path = Path(__file__).parent / ".." / ".."
    root_path = root_path.resolve()
    src_path = root_path / repo_dir
    doc_path = web_repo_path / "docs"
    doc_path.mkdir(parents=True, exist_ok=True)
    cache_path = (web_repo_path / 'src/Cache')
    cache_path.mkdir(parents=True, exist_ok=True)
    # Load the caches, create the caches if they don't exist.
    test_images_cache_path = cache_path / 'TestImages.cache'
    Path(test_images_cache_path.stem).mkdir(parents=True, exist_ok=True)
    vtk_modules_cache_path = cache_path / 'VTKModules.cache'
    Path(vtk_modules_cache_path.stem).mkdir(parents=True, exist_ok=True)
    test_images_dict = load_test_image_cache(test_images_cache_path)
    vtk_modules_cache = load_vtk_modules_cache(vtk_modules_cache_path)
    #  Baseline images path (assumed to exist)
    baseline_src_path = web_repo_path / 'src/Testing/Baseline/'

    # The name of the repository on the server.
    repo_name = list(filter(None, web_repo_url.split('/')))[-1]
    # The user name for the repository.
    # user_name = list(filter(None, web_repo_url.split('/')))[-2]

    ref_stat1 = os.stat(src_path / 'Admin/VTKQtCMakeLists')
    ref_stat2 = os.stat(src_path / 'Admin/VTKCMakeLists')
    ref_stat3 = os.stat(src_path / 'Admin/trame_template')
    ref_mtime = ref_stat1.st_mtime
    if ref_stat2.st_mtime > ref_stat1.st_mtime:
        ref_mtime = ref_stat2.st_mtime
    if ref_stat3.st_mtime > ref_stat1.st_mtime:
        ref_mtime = ref_stat3.st_mtime

    # Create a dict to hold the example paths grouped by language.
    # Data format
    # [k1][k2]v
    # Where:
    # k1 = Language
    # k2 = Example path
    # v  = A set holding any additional paths needed by the example.
    example_paths = defaultdict(lambda: defaultdict(lambda: defaultdict(set)))
    # Create a dict to hold CMakeLists.txt file
    example_to_CMake = dict()

    # Create Snippets directories for Cxx, Python and Java
    (doc_path / 'Cxx/Snippets').mkdir(parents=True, exist_ok=True)
    (doc_path / 'Python/Snippets').mkdir(parents=True, exist_ok=True)
    (doc_path / 'Java/Snippets').mkdir(parents=True, exist_ok=True)

    # Add thumbnails and language links to each of the language summary pages, Snippets and Book figures
    pages = ['Cxx.md', 'CxxHowTo.md',
             'Python.md', 'PythonHowTo.md',
             'CSharp.md', 'CSharpHowTo.md',
             'Java.md', 'JavaHowTo.md',
             'JavaScript.md',
             'Cxx/Snippets.md', 'Python/Snippets.md', 'Java/Snippets.md',
             'VTKBookFigures.md', 'VTKFileFormats.md']
    for p in pages:
        add_thumbnails_and_links(web_repo_url, src_path, doc_path, baseline_src_path, test_images_dict, p, p,
                                     stats)

    snippets = ['Cxx/Snippets', 'Python/Snippets', 'Java/Snippets']
    for snippet in snippets:
        src = src_path / snippet
        dest = doc_path / snippet
        p = src.glob('*.md')
        files = [x for x in p if x.is_file()]
        for f in files:
            shutil.copy(f, dest)

    # Copy favicon.png
    dest = doc_path / 'assets/images'
    dest.mkdir(parents=True, exist_ok=True)
    shutil.copy(src_path / 'Images/favicon.png', dest)

    # Copy repo .md files
    shutil.copy(src_path / 'index.md', doc_path)
    shutil.copy(src_path / 'VTKBook.md', doc_path)

    # Get a list of all  examples
    # A dictionary of available languages and extensions
    available_languages = {'Cxx': '.cxx', 'Python': '.py', 'Java': '.java', 'CSharp': '.cs'}

    # Copy coverage files
    for k in available_languages.keys():
        dest = doc_path / k / 'Coverage'
        dest.mkdir(parents=True, exist_ok=True)
        src = web_repo_path / '/'.join(['src/Coverage', k + 'VTKClassesNotUsed.md'])
        shutil.copy(src, dest)
        src = web_repo_path / '/'.join(['src/Coverage', k + 'VTKClassesUsed.md'])
        with open(src, 'r') as ifh:
            lines = ifh.readlines()
        dest = doc_path / '/'.join([k, 'Coverage', k + 'VTKClassesUsed.md'])
        with open(dest, 'w') as ofh:
            for line in lines:
                # Make the link to the example relative.
                line = re.sub(r'][ ]*\([ ]*/\w+/', r'](../../', line)
                ofh.write(line)

    # Copy Instructions files
    dest = doc_path / 'Instructions'
    dest.mkdir(parents=True, exist_ok=True)
    instruction_files = ['ForUsers.md', 'ForDevelopers.md', 'ForAdministrators.md', 'Guidelines.md',
                         'ConvertingFiguresToExamples.md', 'WebSiteMaintenance.md']
    for f in instruction_files:
        make_instruction_pages(web_repo_url, web_site_url, site_url, src_path, doc_path, f, f)

    # Copy VTKBook files
    dest = doc_path / 'VTKBook'
    dest.mkdir(parents=True, exist_ok=True)
    html_id_set = set()
    chapters = ['00Preface', '01Chapter1', '02Chapter2', '03Chapter3', '04Chapter4', '05Chapter5', '06Chapter6',
                '07Chapter7', '08Chapter8', '09Chapter9', '10Chapter10', '11Chapter11', '12Chapter12', '13Glossary']
    ch_src = [src_path / '/'.join(['VTKBook', ch + '.md']) for ch in chapters]
    for ch in ch_src:
        create_html_ids(ch, html_id_set)
    ch_dest = [doc_path / '/'.join(['VTKBook', ch + '.md']) for ch in chapters]
    ch_src_dest = list(zip(ch_src, ch_dest))
    print('Found', len(html_id_set), ' VTK Book figures with html ids')
    for ch in ch_src_dest:
        copy_chapter_add_links(ch, html_id_set, stats)

    # Copy VTKBookLaTeX files
    dest = doc_path / 'VTKBookLaTeX'
    dest.mkdir(parents=True, exist_ok=True)
    shutil.copy(src_path / 'VTKBookLaTeX/VTKTextBook.md', dest)

    # Get a list of all the examples.
    get_example_paths(src_path, available_languages, example_paths)

    # Update any VTK modules.
    srcs = example_paths['Cxx']
    cmd_path = src_path / '/'.join(['Admin', 'WhatModulesVTK.py'])
    parameters = list()
    for src in srcs:
        parameters.append((src, cmd_path, vtk_src_path, vtk_modules_cache, stats))
    with concurrent.futures.ThreadPoolExecutor() as executor:
        future_results = [executor.submit(update_vtk_modules_cache, *p) for p in parameters]
        concurrent.futures.wait(future_results)

    # Rewrite the VTK modules cache file if necessary
    if stats['vtk_modules_misses'] > 0:
        with open(vtk_modules_cache_path, 'w') as cf:
            for key, contents in vtk_modules_cache.items():
                if key.exists() and key.is_file():
                    cf.write(str(key) + ' ' + ' '.join(contents) + '\n')

    make_markdown_example_page(example_paths, available_languages, src_path, doc_path,
                                   repo_name, web_repo_url, vtk_modules_cache,
                                   example_to_CMake, stats)

    # This is not added to the web pages but can be useful for debugging.
    # Only enable if you need it.
    # make_examples_sources_html(example_paths, src_path, doc_path, web_repo_url, web_site_url)
    make_cxx_tarballs(web_repo_dir, example_paths, example_to_CMake, ref_mtime, stats)

    #  Now deal with examples where each individual example is a whole folder.
    trame_example_paths = extract_paths(src_path, 'trame.md')

    # Create tarballs for each example
    make_trame_tarballs(web_repo_dir, src_path, trame_example_paths, ref_mtime, stats)

    # Update the test image cache file if necessary
    if stats['test_image_misses'] > 0:
        with open(test_images_cache_path, 'w') as cf:
            for key in test_images_dict.keys():
                cf.write(key + ' ' + test_images_dict[key] + '\n')

    # Report stats
    print('\n'.join(get_statistics(stats)))


if __name__ == '__main__':
    with ElapsedTime() as et:
        main()
    print('Time taken: {:0.3f}s'.format(et.interval))
