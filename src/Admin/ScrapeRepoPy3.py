#!/usr/bin/env python

# This is a currently untested Python 3 version of ScrapeRepo
#
# concurrent.futures is used to set up a thread pool
#   to make processing of tiny urls faster

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
    description = 'Copy/edit files from the src folder into the docs folder.'
    epilogue = '''

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
    :return:
    """
    in_code = False
    with open(from_to_file[1], 'w') as outFile:
        with open(from_to_file[0], 'r') as chapterLine:
            for line in chapterLine:
                if line.count(r'```') % 2 != 0:
                    in_code = not in_code
                if not in_code:
                    line = AddDoxygen(line, stats)
                    figureFound = sorted(re.findall(r'\*\*(Figure[^\*]*)\*\*', line), reverse=True)
                    if len(figureFound) > 0:
                        for figure in figureFound:
                            if figure in html_id_set:
                                line = line.replace(figure, r'<a href="#' + figure.replace(
                                    "Figure", "FIGURE") + '">**' + figure.replace(
                                    "Figure", "FIGURE") + r'**</a>')
                line = line.replace("FIGURE", "Figure")
                outFile.write(line)


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
    :return:
    """
    if url in list(cache_dict.keys()):
        stats['test_image_hits'] += 1
        return cache_dict[url]
    tinyOne = make_tiny(url)
    cache_dict[url] = tinyOne
    stats['test_image_misses'] += 1
    print("Test image cache miss: " + url)
    return cache_dict[url]


def tiny_url(url):
    """
    Given a URL make a tiny URL.
    :param k:
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
    :param cache_dict: The URL cache dictionary.
    :param lines: Lines possibly containing URLs.
    :return:
    """
    #  Extract the lines that need tiny urls
    needUrls = dict()
    urlsToShorten = list()
    for k, v in lines.items():
        if v[0]:
            needUrls[k] = v
    for k, v in needUrls.items():
        if v[2] in list(test_image_cache.keys()):
            stats['test_image_hits'] += 1
        else:
            stats['test_image_misses'] += 1
            print("Test image cache miss: " + v[2])
            urlsToShorten.append(v[2])
    res = make_missing_tiny_urls(urlsToShorten)
    #  Update the cache
    for r in res:
        test_image_cache[r[0]] = r[1]

def add_image_link(test_image_cache, lines, stats):
    """
    Add a link to the image if one exists.
    :param test_image_cache: The cache of test images.
    :param lines: Lines some of which need an image.
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


# def GetVTKQtCMake(example):
#     """
#     Get the Qt CMake file.
#     :param example: The example.
#     :return:
#     """
#     reg = re.compile(r"\{\{(VTKQtCMakeLists)\|(.*?)\}\}.*?", re.DOTALL)
#     return [reg.findall(example)]


# Get the VTK CMake file
def GetVTKCMakelists(S):
    reg = re.compile(r"```cmake(.*?)```", re.DOTALL)
    return [''.join(reg.findall(S))]


# add header and code hilighting to c++ code

def WriteCxxCode(toFile, codeName, code):
    toFile.write("**" + codeName + "**" + "\n")
    toFile.write("```c++" + "\n")
    toFile.write(code)
    toFile.write("```" + "\n")


# Return hl_lines to highlight lines that have vtk classes mentions

def FindLinesWithVTK(srcFileName):
    srcFile = open(srcFileName, 'r')
    lines = srcFile.readlines()
    lineNumber = 1
    reg = re.compile(r'(vtk[0-9a-zA-Z]*)')
    hl_lines = 'hl_lines="'
    for line in lines:
        if reg.search(line):
            hl_lines += str(lineNumber) + " "
        lineNumber += 1
    hl_lines += '"\n'
    srcFile.close()
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


def make_path(*args):
    """
    Return a path name
    :param args: One of more arguments to make a path from.
    :return: The path.
    """
    components = []
    for a in args:
        components.append(a.split('/'))
    flat_list = [item for sublist in components for item in sublist]
    if flat_list[0] == '':
        flat_list[0] = '/'
    # Strip out remaining ''s
    list1 = [elem for elem in flat_list if elem]
    path = os.path.join(*list1)
    if 'https:' in path or 'http:' in path:
        path = path.split('/')
        for i in range(path.count('https:')):
            path[i] = path[i] + '/'
        for i in range(path.count('http:')):
            path[i] = path[i] + '/'
        path = '/'.join(path)
    return path


# Given a lang example return a link to another example if it exists
def find_other_given_lang(example, exampleLang, otherLang, otherExt):
    if otherLang == exampleLang:
        return ''
    otherLink = re.sub(r'/' + exampleLang + r'/', r'/' + otherLang + '/', example)
    otherPath = otherLink + otherExt
    if os.path.exists(otherPath):
        path_elements = split_path(otherLink)[-3:]
        return '([' + otherLang + '](' + make_path('..', '..', '..', *path_elements) + '))'
    return ''


def AddDoxygen(s, stats):
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
                      r' [\1](' + r'http://www.vtk.org/doc/nightly/html/class' + r'\1.html#details)', s)
    return s


# # add doxygen links to a file
# def AddDoxygens(repo_dir, repo_url, fromFile, toFile):
#     mdFile = open(fromFile, 'r')
#     outFile = open(toFile, 'w')
#     for line in mdFile:
#         withDoxy = AddDoxygen(line)
#         outFile.write(withDoxy.rstrip())
#         outFile.write("\n")
#     mdFile.close()
#     outFile.close()


# add thumbnails to example tables
def find_thumbnail(S):
    reg = re.compile(r'^\[[^\(]*\]\(([^)]*)')
    if reg.match(S):
        return [''.join(reg.findall(S))]
    return ['']


# add thumbnails to a file
def add_thumbnails(repo_url, root_path, repo_dir, doc_dir, baseline_dir, cache_dict, from_file, to_file, stats):
    from_path = make_path(root_path, repo_dir, from_file)
    to_path = make_path(root_path, doc_dir, to_file)
    with open(from_path, 'r') as md_file:
        lines = dict()
        line_count = 0
        x = []
        for line in md_file:
            if '](/Coverage' in line:
                # Make the coverage link relative.
                line = re.sub(r'][ ]*\([ ]*/', r'](', line)
            else:
                # Make the language link relative.
                line = re.sub(r'][ ]*\([ ]*/\w+/', r'](', line)
            # line = line.replace('(/Cxx/','(')
            example_line = find_thumbnail(line.strip())[0]
            withDoxy = AddDoxygen(line, stats)
            x.append(False)
            x.append(withDoxy.rstrip())
            if example_line != '':
                stats['thumb_count'] += 1
                exampleName = os.path.split(example_line)[1]
                exampleDir = os.path.split(example_line)[0]
                baseline = make_path(root_path, repo_dir, baseline_dir, exampleDir, "Test" + exampleName + ".png")
                if os.path.exists(baseline):
                    baselineURL = make_path(repo_url, "blob/master", "src", baseline_dir, exampleDir,
                                            "Test" + exampleName + ".png")
                    x[0] = True
                    x.append(baselineURL)
            lines[line_count] = x
            line_count += 1
            x = []
        add_image_link(cache_dict, lines, stats)
    with open(to_path, 'w') as ofn:
        k = sorted(lines.keys())
        for kv in k:
            line = ''.join(lines[kv][1:])
            ofn.write(line + '\n')


# Fill in the template parameters in a CMakeLists template file
# The output is a CMakeLists.txt file with Name substituted for {{{1}}}

def fill_CMake_lists(cmake_contents, example_name, extra_names, components):
    r1 = re.sub(r'XXX', example_name, cmake_contents)
    r2 = re.sub(r'YYY', extra_names, r1)
    reg = re.sub(r'ZZZ', components, r1)
    return reg


def fill_Qt_CMake_lists(cmake_contents, example_name, components):
    r1 = re.sub(r'XXX', example_name, cmake_contents)
    reg = re.sub(r'ZZZ', components, r1)
    return reg


def get_statistics(stats):
    totals = list()
    for k, v in stats.items():
        if k in ['cxx_count', 'cs_count', 'py_count', 'java_count']:
            totals.append(v)
    res = list()
    res.append('ScrapeRepo Summary')
    res.append('    C++ examples: ' + str(stats['cxx_count']))
    res.append('    CSharp examples: ' + str(stats['cs_count']))
    res.append('    Python examples: ' + str(stats['py_count']))
    res.append('    Java examples: ' + str(stats['java_count']))
    res.append('    Total examples: ' + str(sum(totals)))
    res.append('    Doxygen added: ' + str(stats['doxy_count']))
    res.append('    Thumbnails added: ' + str(stats['thumb_count']))
    res.append('    TinyUrl Cache hits: ' + str(stats['test_image_hits']))
    res.append('    TinyUrl Cache misses: ' + str(stats['test_image_misses']))
    res.append('    Components Cache hits: ' + str(stats['components_hits']))
    res.append('    Components Cache misses: ' + str(stats['components_misses']))
    return res


#####################################################################

#####################################################################
#  This is the main module.
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

    # Baseline top level
    baseline_path = make_path(repo_dir, '/Testing/Baseline')
    baseline_src_path = 'Testing/Baseline'

    # Load the test image cache
    test_image_dict = load_test_image_cache(make_path(repo_path, 'Admin/TinyUrlCache'))

    # Load the Component cache
    components_dict = load_components_cache(make_path(repo_path, 'Admin/ComponentsCache'))

    # Read the Wiki Templates
    with open(make_path(repo_path, 'Admin/VTKCMakeLists'), 'r') as VTKTemplateFile:
        VTKTemplate = VTKTemplateFile.read()

    with open(make_path(repo_path, 'Admin/VTKQtCMakeLists'), 'r') as VTKQtTemplateFile:
        VTKQtTemplate = VTKQtTemplateFile.read()

    # Make the reference mtime to be the most recent of the two CMakeLists templates
    ref_stat1 = os.stat(make_path(repo_path, 'Admin/VTKQtCMakeLists'))
    ref_stat2 = os.stat(make_path(repo_path, 'Admin/VTKCMakeLists'))
    ref_mtime = ref_stat1.st_mtime
    if ref_stat2.st_mtime > ref_stat1.st_mtime:
        ref_mtime = ref_stat2.st_mtime

    # Create a dict to hold code name and page name
    codeToPage = dict()

    # Create a dict to hold CMakeLists.txt file
    exampleToCMake = dict()

    # Create a dict to hold the file names for each example
    exampleToFileNames = dict()

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
        add_thumbnails(repo_url, root_path, repo_dir, doc_dir, baseline_src_path, test_image_dict, p, p, stats)

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
        shutil.copy(make_path(repo_path, 'Coverage', k + 'VTKClassesUsed.md'), dest)

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

    for lang, langExt in list(available_languages.items()):
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
                other_languages = list()
                # skip files that are listed as extras
                if f in all_extras:
                    continue
                if f == 'CMakeLists.txt':
                    continue
                ExampleName = os.path.splitext(f)[0]
                ExampleExt = os.path.splitext(f)[1]
                if ExampleExt != langExt:
                    continue
                # Find examples in other available_languages
                fp = root + '/' + f
                for lLang, lExt in list(available_languages.items()):
                    otherLink = find_other_given_lang(os.path.splitext(fp)[0], lang, lLang, lExt)
                    if otherLink != '':
                        other_languages.append(otherLink)
                BaselinePath = os.path.join(repo_path, 'Testing', 'Baseline', lang, kit_name, 'Test' +
                                            ExampleName + '.png')
                PathName = os.path.join(doc_path, lang, kit_name)
                if not os.path.exists(PathName):
                    if PathName != '':
                        os.makedirs(PathName)
                OutputFile = os.path.join(doc_path, lang, kit_name, ExampleName + '.md')
                MdFile = open(OutputFile, 'w')
                MdFile.write(
                    '[' + repo_name + '](/)/[' + lang + '](/' + lang + ')/' + kit_name + '/' + ExampleName + '\n\n')

                if os.path.isfile(BaselinePath):
                    ImgUrl = repo_url + '/blob/master/src/Testing/Baseline/' + lang + '/' + kit_name + '/Test' + ExampleName + '.png?raw=true'
                    # href to open image in new tab
                    MdFile.write('<a href="' + ImgUrl + ' target="_blank">' + '\n')
                    MdFile.write(
                        '<img style="border:2px solid beige;float:center" src="' + ImgUrl + '" width="256" />' + '\n')
                    MdFile.write('</a>' + '\n')
                    MdFile.write('<hr>\n')
                    MdFile.write('\n')
                DescriptionPath = os.path.join(repo_path, lang, kit_name, ExampleName + ".md")
                # Add a description if a .md file exists for the example
                if os.path.isfile(DescriptionPath):
                    DescriptionFile = open(DescriptionPath, 'r')
                    description = DescriptionFile.read()
                    DescriptionFile.close()
                    description = AddDoxygen(description, stats)
                    MdFile.write(description)
                # Add examples from other available languages if they exist
                if len(other_languages) > 0:
                    seeAlso = '\n!!! Tip "Other languages"\n'
                    see = '    See '
                    for other in other_languages:
                        seeAlso += see + other
                        see = ', '
                    seeAlso += '\n'
                    MdFile.write(seeAlso)
                    MdFile.write('\n')

                # Add email contact for questions
                question = '\n!!! question\n    If you have a simple question about this example contact us at <a href=mailto:' + repo_name + 'ExProject@gmail.com?subject=' + ExampleName + langExt + '&body=' + 'https://' + user_name + '.github.io/' + repo_name + '/site/' + lang + '/' + kit_name + '/' + ExampleName + '>' + repo_name + 'Project</a>\n    If your question is more complex and may require extended discussion, please use the [VTK Discourse Forum](https://discourse.vtk.org/)\n'
                MdFile.write(question)
                MdFile.write('\n')

                SrcFileName = os.path.join(repo_path, lang, kit_name, ExampleName + langExt)
                with open(SrcFileName, 'r') as SrcFile:
                    src = SrcFile.read()
                hiliteLines = FindLinesWithVTK(SrcFileName)
                MdFile.write('###Code\n')
                MdFile.write('**' + ExampleName + langExt + '**' + '\n')
                if langExt == '.cxx':
                    MdFile.write('``` c++ ' + hiliteLines + '\n')
                    # Get the components used in this example
                    components = get_components(repo_path, components_dict, vtk_src_dir, SrcFileName, src, stats)
                    stats['cxx_count'] += 1
                elif langExt == '.cs':
                    MdFile.write('```csharp' + hiliteLines + '\n')
                    stats['cs_count'] += 1
                elif langExt == '.py':
                    MdFile.write('```python' + hiliteLines + '\n')
                    stats['py_count'] += 1
                elif langExt == '.java':
                    MdFile.write('```java' + hiliteLines + '\n')
                    stats['java_count'] += 1
                MdFile.write(src)
                MdFile.write('```' + '\n')

                # Store the full file names for the example
                if ExampleName not in exampleToFileNames:
                    exampleToFileNames[ExampleName] = set()
                SrcFile = os.path.join(repo_path, lang, kit_name, ExampleName + ExampleExt)
                exampleToFileNames[ExampleName].add(SrcFile)

                ExtrasPath = os.path.join(repo_path, lang, kit_name, ExampleName + '.extras')
                ExtraNames = ''
                if os.path.isfile(ExtrasPath):
                    ExtrasFile = open(ExtrasPath, 'r')
                    for line in ExtrasFile:
                        line = line.strip()
                        if line == '':
                            continue
                        ExtraPath = os.path.join(repo_path, lang, kit_name, line)
                        SrcFile = os.path.join(repo_path, lang, kit_name, line)

                        exampleToFileNames[ExampleName].add(SrcFile)
                        ExtraFile = open(ExtraPath, 'r')
                        extraCode = ExtraFile.read()
                        ExtraFile.close()
                        WriteCxxCode(MdFile, line, extraCode)
                        extent = os.path.splitext(line)
                        if extent[1] == '.cxx':
                            ExtraNames += ' ' + line
                    ExtrasFile.close()
                CustomCMakePath = os.path.join(repo_path, lang, kit_name, ExampleName + '.cmake')
                if os.path.isfile(CustomCMakePath):
                    CustomCMakeFile = open(CustomCMakePath, 'r')
                    cmake = CustomCMakeFile.read()
                    CustomCMakeFile.close()
                else:
                    if is_qt_example(src):
                        CMakeFile = open(os.path.join(repo_path, 'Admin', 'VTKQtCMakeLists'), 'r')
                        CMakeContents = CMakeFile.read()
                        CMakeFile.close()
                        # Create component lines
                        Components = ''
                        for component in components:
                            if 'vtk' in component:
                                Components += '\n  ' + component
                            else:
                                Components += '\n  ' + 'vtk' + component
                        # If there are no components found, assume we need then all
                        # This occurs when the source file includes another Cxx file
                        # print('Components: ' + Components)
                        cmake = fill_Qt_CMake_lists(CMakeContents, ExampleName, Components)
                    else:
                        with open(os.path.join(repo_path, 'Admin', 'VTKCMakeLists'), 'r') as CMakeFile:
                            CMakeContents = CMakeFile.read()
                        # Create component lines
                        Components = ''
                        for component in components:
                            if 'vtk' in component:
                                Components += '\n  ' + component
                            else:
                                Components += '\n  ' + 'vtk' + component
                        # If there are no components found, assume we need then all
                        # This occurs when the source file includes another Cxx file
                        # print('Components: ' + Components)
                        cmake = fill_CMake_lists(CMakeContents, ExampleName, ExtraNames, Components)
                if lang == 'Cxx':
                    exampleToCMake[ExampleName] = GetVTKCMakelists(cmake)
                    MdFile.write(cmake)
                MdFile.close()
                codeToPage[ExampleName + langExt] = '/' + lang + '/' + kit_name + '/' + ExampleName

    # Generate an html page that links each example code file to its Wiki Example page
    indexFile = open(os.path.join(doc_path, 'ExampleCodeToWikiPage.html'), 'w')
    indexFile.write('Navigate to the page that contains the source code of an example<br>')
    indexFile.write('\n')
    sortedByCode = sorted(codeToPage.items())
    for item in sortedByCode:
        indexFile.write("<A HREF=" + repo_url + "/wikis" + re.sub(" ", "_", item[1]) + ">" + item[0] + "</A>")
        indexFile.write(
            "<A HREF=" + repo_url + "/blob/master" + re.sub(" ", "_", item[1]) + ".md" + ">" + "(md)" + "</A>")
        indexFile.write("<br>\n")
    indexFile.close()

    # Create tarballs for each example
    tmpDir = tempfile.mkdtemp(prefix='VTKTarballs') + '/'

    # Create the Tarballs directory in the source tree if not present
    # If it does not exist, assume the tarball repo has not been cloned
    # and we need to ignore tar files
    if not os.path.exists('src/Tarballs'):
        os.makedirs('src/Tarballs')
        ignoreFile = open('src/Tarballs/.gitignore', 'w')
        ignoreFile.write('*,tar\n')
        ignoreFile.close()

    # Create tarballs
    # For each example page, create a directory and copy that example's files
    # into the directory
    # If the example has a CMakeLists.txt file, copy that.
    # Also, create a subdir called build. This directory is handy when you want to
    # configure with CMake and build the example.
    for example in exampleToFileNames:
        if example not in exampleToCMake:
            continue
        # Make the directories for the example
        srcDir = tmpDir + example
        # codeFileName = srcDir + '/' + example + '.cxx'
        if not os.path.exists(srcDir):
            os.makedirs(srcDir)

            # An example may have multiple source files
            for exampleFileName in exampleToFileNames[example]:
                # Get the file name
                tarFileName = srcDir + '/' + os.path.split(exampleFileName)[1]
                # Copy the code for the example
                shutil.copy(exampleFileName, tarFileName)
                os.utime(tarFileName, (ref_mtime, ref_mtime))

        # Some examples do not have a CMakeLists.txt file
        if example in exampleToCMake:
            os.makedirs(srcDir + '/build')
            cmakeFileName = srcDir + '/' + 'CMakeLists.txt'
            cmakeFile = open(cmakeFileName, 'w')
            cmakeFile.write(exampleToCMake[example][0])
            cmakeFile.close()
            os.utime(cmakeFileName, (ref_mtime, ref_mtime))

        # Set the mtimes for the directories containing the example
        # Since the mtime is stored in the tar header for each file and directory,
        # we need a consistent time so that a tar of an unchanged example will be equal
        # to the one in the repo
        os.utime(srcDir, (ref_mtime, ref_mtime))
        os.utime(srcDir + '/build', (ref_mtime, ref_mtime))

        # Now create the tar file for the example
        # The tarballs are stored in the source tree
        tar = tarfile.open(make_path(repo_path, 'Tarballs', example + '.tar'), 'w')
        tar.add(srcDir, arcname=example)
        tar.close()

    os.utime(tmpDir, (0, ref_mtime))
    # Cleanup the temporary directories
    shutil.rmtree(tmpDir)

    # Update the test image cache file if necessary
    if stats['test_image_misses'] > 0:
        with open(make_path(repo_path, 'Admin/TinyUrlCache'), 'w') as cf:
            for key in test_image_dict.keys():
                cf.write(key + ' ' + test_image_dict[key] + '\n')

    # Rewrite the components cache file if necessary
    if stats['components_misses'] > 0:
        with open(make_path(repo_path, 'Admin/ComponentsCache'), 'w') as cf:
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
