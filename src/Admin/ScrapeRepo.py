#!/usr/bin/env python

import concurrent.futures
import contextlib
import hashlib
import os
import re
import shutil
import subprocess
import tarfile
import tempfile
import time
from collections import Counter
from urllib.parse import urlencode
from urllib.request import urlopen

try:
    import markdown
except ModuleNotFoundError:
    print("ScrapeRepo: " + "markdown package is required but not found.")
    print("ScrapeRepo: " + "install with `pip|conda install markdown`")
    exit(0)


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
                        help='The local directory in the VTK Examples folder containing the source files e.g. src')
    parser.add_argument('doc_dir', nargs='?', default='docs',
                        help='The directory to receive the markdown pages in the VTK Examples folder e.g. docs')
    parser.add_argument('repo_url',
                        help='repo_url is the github repo URL e.g. https://github.com/**YOUR_NAME**/VTKExamples')
    parser.add_argument('vtk_src', help='The local directory containing the VTK source')
    args = parser.parse_args()
    return args.repo_dir, args.doc_dir, args.repo_url, args.vtk_src


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


def load_components_cache(cache_path):
    """
    Load the component cache into a dictionary.
    :param cache_path: The path to the component cache.
    :return: The component cache as a dictionary.
    """
    cache_dict = dict()
    if os.path.isfile(cache_path):
        with open(cache_path, 'r') as cf:
            for line in cf:
                words = line.split()
                cache_dict[words[0]] = ""
                for word in words[1:]:
                    cache_dict[words[0]] += " " + word
    else:
        f = open(cache_path, 'x')
        f.close()
    return cache_dict


def get_components(repo_path, components_cache, vtk_src_dir, src_file, src, stats):
    """
    If the source code components are not in the cache, find them.
    :param repo_path:
    :param components_cache:
    :param vtk_src_dir:
    :param src_file:
    :param src:
    :param stats: Statistics
    :return:
    """
    # compute sha of src
    sha = hashlib.sha256(str.encode(src)).hexdigest()
    if src_file in list(components_cache.keys()):
        words = components_cache[src_file].split()
        if str(sha) == words[0]:
            stats['components_hits'] += 1
            return words[1:]
    stats['components_misses'] += 1
    try:
        cmd = make_path(repo_path, 'Admin', 'WhatModulesVTKPy3.py') + ' -p ' + vtk_src_dir + ' -s ' + src_file
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
    components = parse_WhatModulesVTKOutput(result)

    components_cache[str(src_file)] = sha
    for component in components:
        components_cache[str(src_file)] += " " + component
    print("Components: cache miss: ", str(src_file))
    return components


def parse_WhatModulesVTKOutput(output):
    words = output.split('\n')
    components = list()
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
        components.append(word.strip())
    return components


def load_test_image_cache(cache_path):
    """
    Load the  into a dictionary.
    :param cache_path: The path to the test image cache.
    :return: The tiny URL cache dictionary
    """
    cache_dict = dict()
    if os.path.isfile(cache_path):
        with open(cache_path, 'r') as cf:
            for line in cf:
                words = line.split()
                cache_dict[words[0]] = words[1]
    else:
        f = open(cache_path, 'x')
        f.close()
    return cache_dict


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


def tiny_url(url):
    """
    Given a URL make a tiny URL.
    :param url:
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
        future_results = [executor.submit(tiny_url, k) for k in urls_to_shorten]
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


def get_VTK_CMake_file(s):
    """
    Pull out the CMake file.
    :param s: The string containing the CMake file.
    :return: The CMake file.
    """
    reg = re.compile(r"```cmake(.*?)```", re.DOTALL)
    return [''.join(reg.findall(s))]


def write_extra_Cxx_code(to_file, code_name, code):
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


def lines_with_VTK_classes(src_file_name):
    """
    Return A string of line numbers to highlight that have vtk class mentions.
    :param src_file_name: The source file
    :return: A string of line numbers to highlight.
    """
    with open(src_file_name, 'r') as ifh:
        lines = ifh.readlines()
        line_number = 1
        reg = re.compile(r'(vtk[0-9a-zA-Z]*)')
        hl_lines = 'hl_lines="'
        for line in lines:
            if reg.search(line):
                hl_lines += str(line_number) + " "
            line_number += 1
        hl_lines += '"\n'
    return hl_lines


def split_path(filepath, maxdepth=20):
    """
    Split a path into its constituent elements.

    Note:
       It is probably best to use os.path.normpath(path) before splitting the path.
       You can reconstruct the path after splitting as follows:
          os.path.join(*splitpath(os.path.normpath(filepath)))

    :param filepath: The path
    :param maxdepth: The maximum recursion depth - increase for a very long path
    :return:
    """
    (head, tail) = os.path.split(filepath)
    return split_path(head, maxdepth - 1) + [tail] if maxdepth and head and head != filepath else [head or tail]


def make_path(*args, relative=False):
    """
    Return a path name
    :param args: One of more arguments to make a path from.
    :param relative: If true make the path relative, i.e. no leading /
    :return: The path.
    """
    components = []
    for a in args:
        components.append(a.split('/'))
    flattened_list = [item for sublist in components for item in sublist]
    if flattened_list[0] == '' and not relative:
        flattened_list[0] = '/'
    # Strip out remaining ''s
    list1 = [elem for elem in flattened_list if elem]
    path = os.path.join(*list1)
    if 'https:' in path or 'http:' in path:
        path = path.split('/')
        for i in range(path.count('https:')):
            path[i] = path[i] + '/'
        for i in range(path.count('http:')):
            path[i] = path[i] + '/'
        path = '/'.join(path)
    return path


def find_other_given_lang(example, example_lang, other_lang, other_ext):
    """
    Given a language example return a link to another example if it exists.
    :param example: The example name.
    :param example_lang: The example language.
    :param other_lang: The other language.
    :param other_ext: The other extension.
    :return:
    """
    if other_lang == example_lang:
        return ''
    other_link = re.sub(r'/' + example_lang + r'/', r'/' + other_lang + '/', example)
    other_path = other_link + other_ext
    if os.path.exists(other_path):
        path_elements = split_path(other_link)[-3:]
        return '([' + other_lang + '](' + make_path('..', '..', '..', *path_elements) + '))'
    return ''


def add_vtk_nightly_doc_link(s, stats):
    """
    If vtkXXXX is in the string, add a link to the doxygen file.
    :param s: The string.
    :param stats: Statistics
    :return:
    """
    reg = re.compile(r'[^\./\[s\-](vtk[^ &][0-9a-zA-Z]*)')
    if reg.findall(s):
        stats['doxy_count'] += 1
        return re.sub(r'[^\./\[-](vtk[0-9a-zA-Z]*)',
                      r' [\1](' + r'https://www.vtk.org/doc/nightly/html/class' + r'\1.html#details)', s)
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


def add_thumbnails_and_links(repo_url, root_path, repo_dir, doc_dir, baseline_dir, test_images, from_file, to_file,
                             stats):
    """
    Add thumbnails, and language links, then copy to the doc_dir.
    :param repo_url: The repository URL.
    :param root_path: Path to the top-level folder e.g. VTKExamples.
    :param repo_dir: Usually 'src'.
    :param doc_dir: Usually 'docs'.
    :param baseline_dir: The baseline directors relative to root_path/repo_dir.
    :param test_images: The cache containing the rest images.
    :param from_file: The file to copy/edit
    :param to_file: The save file name.
    :param stats: Statistics.
    :return:
    """
    from_path = make_path(root_path, repo_dir, from_file)
    to_path = make_path(root_path, doc_dir, to_file)
    baseline_path = make_path(root_path, repo_dir, baseline_dir)
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
                example_name = os.path.split(example_line)[1]
                example_dir = os.path.split(example_line)[0]
                baseline = make_path(baseline_path, example_dir, "Test" + example_name + ".png")
                if os.path.exists(baseline):
                    baseline_url = make_path(repo_url, "blob/master", "src", baseline_dir, example_dir,
                                             "Test" + example_name + ".png")
                    x[0] = True
                    x.append(baseline_url)
            lines[line_count] = x
            line_count += 1
            x = []
        add_image_link(test_images, lines, stats)
        if from_file != 'VTKBookFigures.md':
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
                    v[1] = re.sub(r'][ ]*\([ ]*/', r'](../', v[1])
                    line_changed = True
                if line_changed:
                    lines[k] = v

    with open(to_path, 'w') as ofn:
        k = sorted(lines.keys())
        for kv in k:
            line = ''.join(lines[kv][1:])
            ofn.write(line + '\n')


def fill_CMake_lists(cmake_contents, example_name, extra_names, components):
    """
    Fill in the template parameters in a CMakeLists template file.
    The output is a CMakeLists.txt file with Name substituted for {{{1}}}
    :param cmake_contents: The template file.
    :param example_name: The example file name.
    :param extra_names: Any needed extra files needed to build the example.
    :param components: The VTK components e.g. vtkCommonCore.
    :return: A CMakeLists.txt file.
    """
    r1 = re.sub(r'XXX', example_name, cmake_contents)
    r2 = re.sub(r'YYY', extra_names, r1)
    r3 = re.sub(r'ZZZ', components, r2)
    return r3


def fill_Qt_CMake_lists(cmake_contents, example_name, components):
    """
    Fill in the template parameters in a CMakeLists template file.
    The output is a CMakeLists.txt file with Name substituted for {{{1}}}
    :param cmake_contents: The template file.
    :param example_name: The example file name.
    :param components: The VTK components e.g. vtkCommonCore.
    :return: A CMakeLists.txt file
    """
    r1 = re.sub(r'XXX', example_name, cmake_contents)
    reg = re.sub(r'ZZZ', components, r1)
    return reg


def make_markdown_example_page(f, lang, lang_ext, root, available_languages, repo_path, doc_path,
                               kit_name, repo_name, repo_url, user_name, components_dict, vtk_src_dir,
                               example_to_file_names, example_to_CMake, code_to_page, stats):
    """
    Here we make the markdown page for a given example.
    :param f: The example.
    :param lang: The language.
    :param lang_ext: The language extension.
    :param root: The root name of the example.
    :param available_languages: A dictionary of available languages.
    :param repo_path: Repository path.
    :param doc_path: The path to the docs.
    :param kit_name: The kit name.
    :param repo_name: The repository name.
    :param repo_url: The repository URL.
    :param user_name: The user name.
    :param components_dict: The components dictionary.
    :param vtk_src_dir: The VTK source direcrory.
    :param example_to_file_names: A dictionary to hold the file names for each example.
    :param example_to_CMake: A dictionary to hold CMakeLists.txt file.
    :param code_to_page: A dictionary to hold code name and page name.
    :param stats: Statistics
    :return:
    """
    other_languages = list()
    example_name = os.path.splitext(f)[0]
    example_ext = os.path.splitext(f)[1]
    if example_ext != lang_ext:
        return
    # Find examples in other available_languages
    fp = root + '/' + f
    for lLang, lExt in list(available_languages.items()):
        other_link = find_other_given_lang(os.path.splitext(fp)[0], lang, lLang, lExt)
        if other_link != '':
            other_languages.append(other_link)
    baseline_path = os.path.join(repo_path, 'Testing', 'Baseline', lang, kit_name, 'Test' +
                                 example_name + '.png')
    path_name = os.path.join(doc_path, lang, kit_name)
    if not os.path.exists(path_name):
        if path_name != '':
            os.makedirs(path_name)
    dest = os.path.join(doc_path, lang, kit_name, example_name + '.md')
    # Generate markdown for the example web page
    with open(dest, 'w') as md_file:
        md_file.write(
            '[' + repo_name + '](/)/[' + lang + '](/' + lang + ')/' + kit_name + '/' + example_name + '\n\n')

        if os.path.isfile(baseline_path):
            image_url = repo_url + '/blob/master/src/Testing/Baseline/' + lang + '/' + kit_name + '/Test' \
                        + example_name + '.png?raw=true'
            # href to open image in new tab
            md_file.write('<a href="' + image_url + ' target="_blank">' + '\n')
            md_file.write(
                '<img style="border:2px solid beige;float:center" src="' + image_url + '" width="256" />' + '\n')
            md_file.write('</a>' + '\n')
            md_file.write('<hr>\n')
            md_file.write('\n')
        description_path = os.path.join(repo_path, lang, kit_name, example_name + ".md")
        # Add a description if a .md file exists for the example
        if os.path.isfile(description_path):
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

        # Add email contact for questions
        question = \
            '\n!!! question\n    ' \
            'If you have a simple question about this example contact us at <a href=mailto:' \
            + repo_name + 'ExProject@gmail.com?subject=' + example_name + lang_ext + '&body=' + 'https://' \
            + user_name + '.github.io/' + repo_name + '/site/' + lang + '/' + kit_name + '/' + example_name \
            + '>' + repo_name + \
            'Project</a>\n    If your question is more complex and may require extended discussion,' \
            ' please use the [VTK Discourse Forum](https://discourse.vtk.org/)\n'
        md_file.write(question)
        md_file.write('\n')

        source_file_name = os.path.join(repo_path, lang, kit_name, example_name + lang_ext)
        with open(source_file_name, 'r') as ifh:
            src = ifh.read()
        hilite_lines = lines_with_VTK_classes(source_file_name)
        md_file.write('###Code\n')
        md_file.write('**' + example_name + lang_ext + '**' + '\n')
        components = None
        if lang_ext == '.cxx':
            md_file.write('``` c++ ' + hilite_lines + '\n')
            # Get the components used in this example
            components = get_components(repo_path, components_dict, vtk_src_dir, source_file_name, src, stats)
            stats['cxx_count'] += 1
        elif lang_ext == '.cs':
            md_file.write('```csharp' + hilite_lines + '\n')
            stats['cs_count'] += 1
        elif lang_ext == '.py':
            md_file.write('```python' + hilite_lines + '\n')
            stats['py_count'] += 1
        elif lang_ext == '.java':
            md_file.write('```java' + hilite_lines + '\n')
            stats['java_count'] += 1
        md_file.write(src)
        md_file.write('```' + '\n')

        # Store the full file names for the example
        if example_name not in example_to_file_names:
            example_to_file_names[example_name] = set()
        source_file = os.path.join(repo_path, lang, kit_name, example_name + example_ext)
        example_to_file_names[example_name].add(source_file)

        extras_path = os.path.join(repo_path, lang, kit_name, example_name + '.extras')
        extra_names = ''
        if os.path.isfile(extras_path):
            with open(extras_path, 'r') as extras_file:
                for line in extras_file:
                    line = line.strip()
                    if line == '':
                        continue
                    extra_path = os.path.join(repo_path, lang, kit_name, line)
                    source_file = os.path.join(repo_path, lang, kit_name, line)

                    example_to_file_names[example_name].add(source_file)
                    with open(extra_path, 'r') as extra_fh:
                        extra_code = extra_fh.read()
                    write_extra_Cxx_code(md_file, line, extra_code)
                    extent = os.path.splitext(line)
                    if extent[1] == '.cxx':
                        extra_names += ' ' + line
        custom_CMake_path = os.path.join(repo_path, lang, kit_name, example_name + '.cmake')
        if os.path.isfile(custom_CMake_path):
            with open(custom_CMake_path, 'r') as ifh:
                cmake = ifh.read()
        else:
            if lang_ext == '.cxx':
                if is_qt_example(src):
                    with open(os.path.join(repo_path, 'Admin', 'VTKQtCMakeLists'), 'r') as CMakeFile:
                        CMake_contents = CMakeFile.read()
                    # Create component lines
                    Components = ''
                    for component in components:
                        if 'vtk' in component:
                            Components += '\n  ' + component
                        else:
                            Components += '\n  ' + 'vtk' + component
                    # If there are no components found, assume we need then all
                    # This occurs when the source file includes another Cxx file
                    cmake = fill_Qt_CMake_lists(CMake_contents, example_name, Components)
                else:
                    with open(os.path.join(repo_path, 'Admin', 'VTKCMakeLists'), 'r') as CMakeFile:
                        CMake_contents = CMakeFile.read()
                    # Create component lines
                    Components = ''
                    for component in components:
                        if 'vtk' in component:
                            Components += '\n  ' + component
                        else:
                            Components += '\n  ' + 'vtk' + component
                    # If there are no components found, assume we need then all
                    # This occurs when the source file includes another Cxx file
                    cmake = fill_CMake_lists(CMake_contents, example_name, extra_names, Components)
        if lang == 'Cxx':
            example_to_CMake[example_name] = get_VTK_CMake_file(cmake)
            md_file.write(cmake)
    code_to_page[example_name + lang_ext] = '/' + lang + '/' + kit_name + '/' + example_name


def make_tarballs(repo_path, example_to_file_names, example_to_CMake, ref_mtime):
    """
    Create tarballs for each example.
    :param repo_path: Repository path.
    :param example_to_file_names: A dictionary to holding the file names for each example.
    :param example_to_CMake: A dictionary to holding the CMakeLists.txt file.
    :param ref_mtime: The reference reference mtime.
    :return:
    """

    tmp_dir = tempfile.mkdtemp(prefix='VTKTarballs') + '/'

    # Create the Tarballs directory in the source tree if not present
    # If it does not exist, assume the tarball repo has not been cloned
    # and we need to ignore tar files
    tar_dir = make_path(repo_path, 'Tarballs')
    if not os.path.exists(tar_dir):
        os.makedirs(tar_dir)
        with open(make_path(tar_dir, '.gitignore'), 'w') as ofh:
            ofh.write('*,tar\n')

    # Create tarballs
    # For each example page, create a directory and copy that example's files
    # into the directory
    # If the example has a CMakeLists.txt file, copy that.
    # Also, create a subdir called build. This directory is handy when you want to
    # configure with CMake and build the example.
    for example in example_to_file_names:
        if example not in example_to_CMake:
            continue
        # Make the temporary directories for the example
        src_dir = make_path(tmp_dir, example)
        # codeFileName = srcDir + '/' + example + '.cxx'
        if not os.path.exists(src_dir):
            os.makedirs(src_dir)

            # An example may have multiple source files
            for exampleFileName in example_to_file_names[example]:
                # Get the file name
                tar_fn = make_path(src_dir, os.path.split(exampleFileName)[1])
                # Copy the code for the example
                shutil.copy(exampleFileName, tar_fn)
                os.utime(tar_fn, (ref_mtime, ref_mtime))

        # Some examples do not have a CMakeLists.txt file
        if example in example_to_CMake:
            os.makedirs(make_path(src_dir, 'build'))
            cmake_fn = make_path(src_dir, 'CMakeLists.txt')
            with open(cmake_fn, 'w') as cmake_fh:
                cmake_fh.write(example_to_CMake[example][0])
            os.utime(cmake_fn, (ref_mtime, ref_mtime))

        # Set the mtimes for the directories containing the example
        # Since the mtime is stored in the tar header for each file and directory,
        # we need a consistent time so that a tar of an unchanged example will be equal
        # to the one in the repo
        os.utime(src_dir, (ref_mtime, ref_mtime))
        os.utime(make_path(src_dir, 'build'), (ref_mtime, ref_mtime))

        # Now create the tar file for the example
        # The tarballs are stored in the source tree
        tar = tarfile.open(make_path(repo_path, 'Tarballs', example + '.tar'), 'w')
        tar.add(src_dir, arcname=example)
        tar.close()

    os.utime(tmp_dir, (0, ref_mtime))
    # Cleanup the temporary directories
    shutil.rmtree(tmp_dir)


def get_statistics(stats):
    """

    :param stats: The statistics.
    :return: A list of statistics.
    """
    totals = list()
    for k, v in stats.items():
        if k in ['cxx_count', 'cs_count', 'py_count', 'java_count']:
            totals.append(v)
    res = list()
    res.append('ScrapeRepo Summary')
    res.append('  C++ examples:            ' + str(stats['cxx_count']))
    res.append('  CSharp examples:         ' + str(stats['cs_count']))
    res.append('  Python examples:         ' + str(stats['py_count']))
    res.append('  Java examples:           ' + str(stats['java_count']))
    res.append('  Total examples:          ' + str(sum(totals)))
    res.append('  Doxygen added:           ' + str(stats['doxy_count']))
    res.append('  Thumbnails added:        ' + str(stats['thumb_count']))
    res.append('  Test Image Cache hits:   ' + str(stats['test_image_hits']))
    res.append('  Test Image Cache misses: ' + str(stats['test_image_misses']))
    res.append('  Components Cache hits:   ' + str(stats['components_hits']))
    res.append('  Components Cache misses: ' + str(stats['components_misses']))
    return res


def main():
    # Initialize the statistics
    stats = Counter()
    stats['test_image_hits'] = 0
    stats['test_image_misses'] = 0
    stats['components_hits'] = 0
    stats['components_misses'] = 0
    stats['cxx_count'] = 0
    stats['cs_count'] = 0
    stats['py_count'] = 0
    stats['java_count'] = 0
    stats['thumb_count'] = 0
    stats['doxy_count'] = 0

    repo_dir, doc_dir, repo_url, vtk_src_dir = get_program_parameters()

    sub_str = './'
    if repo_dir.startswith(sub_str):
        repo_dir = re.sub(sub_str, '', repo_dir)
    if doc_dir.startswith(sub_str):
        doc_dir = re.sub(sub_str, '', doc_dir)
    # Find the root path, this program resides in the Admin folder so go up two levels.
    root_path = os.path.dirname(os.path.abspath(__file__))
    for i in range(2):
        root_path = os.path.dirname(root_path)
    # The name of the repository on the server.
    repo_name = list(filter(None, repo_url.split('/')))[-1]
    # The user name for the repository.
    user_name = list(filter(None, repo_url.split('/')))[-2]

    repo_path = make_path(root_path, repo_dir)
    doc_path = make_path(root_path, doc_dir)

    # Make sure the wiki docs folder exists
    if not os.path.exists(doc_path):
        os.makedirs(doc_path)

    # The cache
    cache_path = make_path(repo_path, 'Cache')
    if not os.path.exists(cache_path):
        os.makedirs(cache_path)

    # Load the caches, create the caches if they doen't exist.
    test_image_dict = load_test_image_cache(make_path(cache_path, 'TestImageCache'))
    components_dict = load_components_cache(make_path(cache_path, 'ComponentsCache'))

    #  Baseline top level path relative to src
    baseline_src_path = 'Testing/Baseline'

    # Make the reference mtime to be the most recent of the two CMakeLists templates
    ref_stat1 = os.stat(make_path(repo_path, 'Admin/VTKQtCMakeLists'))
    ref_stat2 = os.stat(make_path(repo_path, 'Admin/VTKCMakeLists'))
    ref_mtime = ref_stat1.st_mtime
    if ref_stat2.st_mtime > ref_stat1.st_mtime:
        ref_mtime = ref_stat2.st_mtime

    # Create a dict to hold code name and page name
    code_to_page = dict()

    # Create a dict to hold CMakeLists.txt file
    example_to_CMake = dict()

    # Create a dict to hold the file names for each example
    example_to_file_names = dict()

    # Create Snippets directories for Cxx, Python and Java
    if not os.path.exists(make_path(doc_path, 'Cxx/Snippets')):
        os.makedirs(make_path(doc_path, 'Cxx/Snippets'))

    if not os.path.exists(make_path(doc_path, 'Python/Snippets')):
        os.makedirs(make_path(doc_path, 'Python/Snippets'))

    if not os.path.exists(make_path(doc_path, 'Java/Snippets')):
        os.makedirs(make_path(doc_path, 'Java/Snippets'))

    # Add thumbnails and language links to each of the language summary pages, Snippets and Book figures
    pages = ['Cxx.md', 'Python.md', 'CSharp.md', 'Java.md', 'JavaScript.md', 'Cxx/Snippets.md', 'Python/Snippets.md',
             'Java/Snippets.md', 'VTKBookFigures.md', 'VTKFileFormats.md']
    for p in pages:
        add_thumbnails_and_links(repo_url, root_path, repo_dir, doc_dir, baseline_src_path, test_image_dict, p, p,
                                 stats)

    # C++ Snippets
    src = make_path(repo_path, 'Cxx/Snippets')
    dest = make_path(doc_path, 'Cxx/Snippets')
    for f in os.listdir(src):
        snippet = os.path.splitext(f)[0]
        shutil.copy(make_path(src, snippet + '.md'), dest)

    # Python Snippets
    src = make_path(repo_path, 'Python/Snippets')
    dest = make_path(doc_path, 'Python/Snippets')
    for f in os.listdir(src):
        snippet = os.path.splitext(f)[0]
        shutil.copy(make_path(src, snippet + '.md'), dest)

    # Java Snippets
    src = make_path(repo_path, 'Java/Snippets')
    dest = make_path(doc_path, 'Java/Snippets')
    for f in os.listdir(src):
        snippet = os.path.splitext(f)[0]
        shutil.copy(make_path(src, snippet + '.md'), dest)

    # Copy favicon.png
    dest = make_path(make_path(doc_path, 'assets/images'))
    if not os.path.exists(dest):
        os.makedirs(dest)
    shutil.copy(make_path(repo_path, 'Images/favicon.png'), dest)

    # Copy repo .md files
    shutil.copy(make_path(repo_path, 'index.md'), doc_path)
    shutil.copy(make_path(repo_path, 'VTKBook.md'), doc_path)

    # Get a list of all  examples
    # A dictionary of available languages
    available_languages = {'Cxx': '.cxx', 'Python': '.py', 'Java': '.java', 'CSharp': '.cs'}

    # Copy coverage files
    for k in available_languages.keys():
        dest = make_path(doc_path, k, 'Coverage')
        if not os.path.exists(dest):
            os.makedirs(dest)
        shutil.copy(make_path(repo_path, 'Coverage', k + 'VTKClassesNotUsed.md'), dest)
        src = make_path(repo_path, 'Coverage', k + 'VTKClassesUsed.md')
        with open(src, 'r') as ifh:
            lines = ifh.readlines()
        dest = make_path(doc_path, k, 'Coverage', k + 'VTKClassesUsed.md')
        with open(dest, 'w') as ofh:
            for line in lines:
                # Make the link to the example relative.
                line = re.sub(r'][ ]*\([ ]*/\w+/', r'](../../', line)
                ofh.write(line)

    # Copy Instructions files
    dest = make_path(doc_path, 'Instructions')
    if not os.path.exists(dest):
        os.makedirs(dest)
    shutil.copy(make_path(repo_path, 'Instructions/ForUsers.md'), dest)
    shutil.copy(make_path(repo_path, 'Instructions/ForDevelopers.md'), dest)
    shutil.copy(make_path(repo_path, 'Instructions/ForAdministrators.md'), dest)
    shutil.copy(make_path(repo_path, 'Instructions/Guidelines.md'), dest)
    shutil.copy(make_path(repo_path, 'Instructions/ConvertingFiguresToExamples.md'), dest)

    # Copy VTKBook files
    if not os.path.exists(make_path(doc_path, 'VTKBook')):
        os.makedirs(make_path(doc_path, 'VTKBook'))
    html_id_set = set()
    chapters = ['00Preface', '01Chapter1', '02Chapter2', '03Chapter3', '04Chapter4', '05Chapter5', '06Chapter6',
                '07Chapter7', '08Chapter8', '09Chapter9', '10Chapter10', '11Chapter11', '12Chapter12', '13Glossary']
    ch_src = [make_path(repo_path, 'VTKBook', ch + '.md') for ch in chapters]
    for ch in ch_src:
        create_html_ids(ch, html_id_set)
    ch_dest = [make_path(doc_path, 'VTKBook', ch + '.md') for ch in chapters]
    ch_src_dest = list(zip(ch_src, ch_dest))
    print('Found', len(html_id_set), 'figures with html ids')
    for ch in ch_src_dest:
        copy_chapter_add_links(ch, html_id_set, stats)

    # Copy VTKBookLaTeX files
    dest = make_path(doc_path, 'VTKBookLaTeX')
    if not os.path.exists(dest):
        os.makedirs(dest)
    shutil.copy(make_path(repo_path, 'VTKBookLaTeX/VTKTextBook.md'), dest)

    # Scan all Cxx directories and look for extras
    all_extras = set()
    for root, dirs, files in os.walk(repo_path):
        to_find = make_path(repo_path, 'Cxx')
        start = root.find(to_find)
        if start < 0:
            continue
        for f in files:
            file_name = os.path.splitext(f)
            if file_name[1] == '.extras':
                extras_path = make_path(root, f)
                with open(extras_path, 'r') as ifn:
                    for line in ifn:
                        line = line.strip()
                        all_extras.add(line)

    for lang, lang_ext in list(available_languages.items()):
        to_find = os.path.join(repo_dir, lang)
        for root, dirs, files in os.walk(repo_path):
            start = root.find(to_find)
            if start < 0:
                continue
            # Get the part of the file name that comes after repo_dir
            # e.g. if the file name is VTKExamples/src/Cxx/GeometricObjects/Line,
            # Path will be GeometricObjects/Line
            kit_name = root[start + 1 + len(to_find):]

            if kit_name == '':
                continue
            if kit_name.find('Boneyard') >= 0:
                continue
            if kit_name.find('Broken') >= 0:
                continue
            if kit_name.find('Deprecated') >= 0:
                continue
            if kit_name.find('Untested') >= 0:
                continue
            if kit_name.find('Databases') >= 0:
                continue
            if kit_name.find('Wishlist') >= 0:
                continue
            for f in files:
                # skip files that are listed as extras
                if f in all_extras:
                    continue
                if f == 'CMakeLists.txt':
                    continue
                example_ext = os.path.splitext(f)[1]
                if example_ext != lang_ext:
                    continue
                # Make the markdown page for each example
                make_markdown_example_page(f, lang, lang_ext, root, available_languages, repo_path, doc_path,
                                           kit_name, repo_name, repo_url, user_name, components_dict, vtk_src_dir,
                                           example_to_file_names, example_to_CMake, code_to_page, stats)

    # Generate an html page that links each example code file to its Wiki Example page
    with open(os.path.join(doc_path, 'ExampleCodeToWikiPage.html'), 'w') as index_file:
        index_file.write('Navigate to the page that contains the source code of an example<br>')
        index_file.write('\n')
        sorted_by_code = sorted(code_to_page.items())
        for item in sorted_by_code:
            index_file.write("<A HREF=" + repo_url + "/wikis" + re.sub(" ", "_", item[1]) + ">" + item[0] + "</A>")
            index_file.write(
                "<A HREF=" + repo_url + "/blob/master" + re.sub(" ", "_", item[1]) + ".md" + ">" + "(md)" + "</A>")
            index_file.write("<br>\n")

    # Create tarballs for each example
    make_tarballs(repo_path, example_to_file_names, example_to_CMake, ref_mtime)

    # Update the test image cache file if necessary
    if stats['test_image_misses'] > 0:
        with open(make_path(cache_path, 'TestImageCache'), 'w') as cf:
            for key in test_image_dict.keys():
                cf.write(key + ' ' + test_image_dict[key] + '\n')

    # Rewrite the components cache file if necessary
    if stats['components_misses'] > 0:
        with open(make_path(cache_path, 'ComponentsCache'), 'w') as cf:
            for key, contents in list(components_dict.items()):
                if os.path.exists(key):
                    # add the vtk prefix to the component to support older versions of vtk
                    cf.write(key + ' ' + contents + '\n')

    # Report stats
    print('\n'.join(get_statistics(stats)))


if __name__ == '__main__':
    with ElapsedTime() as et:
        main()
    print('Time taken: {:0.3f}s'.format(et.interval))
