## Administrators

Administrators have write access to the [VTK Examples Source](__SITE_REPOSITORY__) and to the [VTK Examples Web Source](__WEB_REPOSITORY__). If you are a  User [go here](../ForUsers) or a  Developer [go here](../ForDevelopers).

## Organization of the Repositories

The source files for the examples and other files are stored in [VTK Examples Source](__SITE_REPOSITORY__). The web files are stored in a separate repository [VTK Examples Web Source](__WEB_REPOSITORY__).

### Source Repository

The repository contains several types of files.

All example source code, descriptions, test data, and test baselines are stored in the *src/* tree.

The major elements of the tree are:

``` bash
|--
  |-- CMakeLists.txt   # To build all of the examples
  |-- _layouts         # Contains _default.html
  |-- custom_theme     # tweaks to the material theme
  |-- mkdocs.yml       # config file for MkDocs
  |-- src              # all of the content to create site/
    |-- SyncSiteWithRepo.sh  # master script to update site/
    |-- Admin
    |   |-- ScrapeRepo.py       # script to create docs/ in the web site folder
    |   |-- VTKCMakeLists       # template for Cxx examples
    |   |-- VTKQtCMakeLists     # template for Cxx Qt
    |   |-- Trame_template      # template for Trame
    |-- Artifacts       # Additional images, docs, etc.
    |-- Images          # Images used in creating the static web pages.
    |-- Instructions    # Documentation
    |-- LANGUAGE.md     # Summary file for language
    |-- LANGUAGE        # Language examples
    |   |-- TOPIC
    |-- Testing
    |   |-- Baseline # Baselines for examples
    |   |   |-- LANGUAGE
    |   |   |   |-- TOPIC
    |   |-- Data    # Data for examples
    |-- VTKBook         # Markdown version of the VTK Book
    |-- VTKBookLaTeX    # LaTex version of the VTK Book
    |-- stylesheets
        |-- extra.css # mkdocs search config
```

### Web Repository

This repository is separate from the vtk examples source files. It contains the generated web pages and other needed files that are copied across from the source repository.

``` bash
|--
  |-- site
  |-- src               # needed content for site generation and site/ operation
    |-- Artifacts       # Additional images, docs, etc.
    |-- Cache           # Stores cache files used by ScrapeRepo.py.
    |-- Coverage        # Stores VTK classes used and classes used by language.
    |-- Images          # Images used in creating the static web pages.
    |-- Trame           # Additional files specific to this topic.
    |-- VTKBook         # Additional files specific to this topic.
    |-- VTKFileFormats  # Additional files specific to this topic.
    |-- Testing
    |   |-- Baseline # Baselines for examples
    |   |   |-- LANGUAGE
    |   |   |   |-- TOPIC
    |   |-- Data    # Data for examples
|-- Tarballs        # Tar files for each Cxx and Trame example
```

## Look and Feel

A priority in moving from the wiki media [VTK Wiki Examples](http://www.vtk.org/Wiki/VTK/Examples) to the Github pages [VTK Examples](__REPOSITORY__/site/) was to provide a modern, familiar look and feel. We also wanted to support tablet and mobile platforms. We chose [MkDocs](http://www.mkdocs.org/) because it generated static HTML pages that can be hosted anywhere.

<img style="border:2px solid beige;float:center" src="__WEB_BLOB__/src/Artifacts/OldVersusNew.png?raw=true" />

### [MkDocs](http://www.mkdocs.org/)

#### Installing MkDocs

MkDocs is a python package and can be installed using `conda` or `pip`.

#### Configuring MkDocs

The *mkdocs.yml* file contains the configuration parameters:

```bash
site_name: __WEB_REPO_NAME__
site_url: __WEB_SITE_URL__
repo_name: __WEB_USER_NAME__/__WEB_REPO_NAME__
repo_url: __WEB_REPOSITORY__

theme:
    name: material
    custom_dir: custom_theme/

markdown_extensions:
    - pymdownx.arithmatex
    - pymdownx.tasklist:
        custom_checkbox: true
    - pymdownx.magiclink
    - admonition
    - codehilite:
        guess_lang: false
        use_pygments: true
    - footnotes
    - toc:
        permalink: true
google_analytics:
    -  'UA-3660134-4'
    -  'auto'
extra_css:
    -  'stylesheets/extra.css'
copyright: '<font color=#666>Content is available under </font><a href="https://creativecommons.org/licenses/by/2.5/" title="Attribution2.5">Attribution2.5</a> <font color=#666>unless otherwise noted.</font>'
extra_javascript:
    - 'https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.0/MathJax.js?config=TeX-MML-AM_CHTML'
```

#### Markdown Extensions

A number of markdown extensions are available.

##### [admonition](http://squidfunk.github.io/mkdocs-material/extensions/admonition/)

Admonitions are specially marked "topics" that can appear anywhere an ordinary body element can. They contain arbitrary body elements. Typically, an admonition is rendered as an offset block in a document, sometimes outlined or shaded, with a title matching the admonition type.
For example:

!!! note
    This is a note or seealso admonition

!!! summary
    This is a summary or todo admonition

!!! info
    This is an info or todo admonition

!!! tip
    This is a tip, hint or important admonition

!!! success
    This is a success, check or done admonition

!!! question
    This is a question,help faq admonition

!!! warning
    This is a warning, caution or attention admonition

!!! failure
    This is a failure, fail or missing admonition

!!! danger
    This is a danger or error admonition

!!! bug
    This is a bug admonition

!!! quote
    This is a quote or cite admonition

!!! cite
    This is a cite admonition

##### [codehilite](http://squidfunk.github.io/mkdocs-material/extensions/codehilite/)

The codehilite extension highlights code depending on the language.

##### toc

The toc extension generates a table of contents, like the one at the right of this page.

### [MkDocs Materials theme](http://squidfunk.github.io/mkdocs-material/)

The Materials theme is built using Google's [Material Design](https://material.io/guidelines/) guidelines. It offers a unified experience across platforms. The look and feel is a familiar one.

#### Configuring Materials

The materials theme is selected in the *theme* section of the *mkdocs.yml* file.

## [Google Analytics](https://analytics.google.com/)

Google Analytics tracks the site usage, providing lots of useful statistics. To have Google Analytics track web usage, you need to register the URL with the Google Analytics System. After registration, an HTML snippet is provided to include on every web page to be tracked.

### Configuring Google Analytics

The *google_analytics* keyword in the *mkdocs.yml* file specifies the google analytics unique code for this web site. *Currently commented out as this only applies to the old site.*

The *custom_theme/main.html* file defines the metadata for the google site verification:

``` html
<meta name="google-site-verification" content="nJqh3TVb_44fMl2gS61agt_bRNFEaI496UaXYL-wLZI" />
```

## [Google Custom Search Engine](https://cse.google.com/cse/)

The overall look and feel are established at [https://cse.google.com/cse/](https://cse.google.com/cse/). After setting up the search engine, you can get the code to add to the web pages.

``` javascript
<script>
  (function() {
    var cx = '47656ffcafe2e7ab2';
    var gcse = document.createElement('script');
    gcse.type = 'text/javascript';
    gcse.async = true;
    gcse.src = 'https://cse.google.com/cse.js?cx=' + cx;
    var s = document.getElementsByTagName('script')[0];
    s.parentNode.insertBefore(gcse, s);
  })();
</script>
```

### Configuring GCSE

The code is added to [custom_theme/main.html](__BLOB__/custom_theme/main.html).

``` html
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<gcse:searchbox-only>Search</gcse:searchbox-only>
```

### Implementing GCSE

The search box is added to the web pages by adding the gcse search box html to [custom_theme/partials/search.html](__BLOB__/custom_theme/partials/search.html).

``` html
    <gcse:searchbox-only>Search</gcse:searchbox-only>
```

## Performance

### [Lazy Image Loading](https://davidwalsh.name/lazyload-image-fade)

The first implementation had problems loading the [Cxx summary](__WEB_SITE_URL__/Cxx). The number of embedded images exceeded the throttle limits of Github. A lazy image load solution solved the problem. Now, images are only loaded if they appear in the browser window.

The Lazy Image Loading is implemented in javascript. The javascript is stored in *custom_theme/main.html*. It uses a clever *trick*. Images that should be lazy loaded use a *<img>* tag with a *class=lazy* and attribute *data_src*. If the image is in the viewport, the *data_src* is removed, and the image renders.

### Tiny URLs

The  web pages have many URL's that link to images and VTK doxygen pages. The long length of these URLs can increase page load times, especially on the language summary pages.

[http://tinyurl.com](http://tinyurl.com) provides a service to map the long URL's into shorted URLs. To speed up the process, we cache the long/short URLs in a file `src/Cache/TestImages.cache` in the local checkout of the web repository. *ScrapeRepo.py* uses this cache to only convert URLs that are not in the cache. *ScrapeRepo.py* updates the cache after each run.

### Minify HTML pages

*mkdocs* generates HTML pages from the markdown files in `doc` in the local checkout of the web repository. These pages have lots of white space. We use the python package [htmlmin](https://htmlmin.readthedocs.io/en/latest/) to compress the generated HTML. The compressed pages are about 30% smaller after running *htmlmin*. We use the command line to process each `site` in the local checkout of the web repository.

*htmlmin* must be installed and can be installed using `conda` or `pip`

## Administrator Tasks

They are

- Vet new submissions making sure the guidelines are followed.
- Merge and ...
- Update web pages after new submissions by running `src/SyncSiteWithRepo/py` in the .

In order do do this, administrators need to clone the Web Repository and follow these [Web Site Maintenance Instructions](../WebSiteMaintenance).

## Some notes about ScrapeRepo.py and SyncSiteWithRepo.sh

### [ScrapeRepo.py](__BLOB__/src/Admin/ScrapeRepo.py)

<img style="border:2px solid beige;float:center" src="__WEB_BLOB__/src/Artifacts/ScrapeRepo.png?raw=true" />

Given a *repo_dir*, *site_url*, *web_site_url*, *web_repo_url*, *web_repo_dir*  and a *vtk_src_dir*, `ScrapeRepo.py` proceeds as follows with all files being copied in the the Web Repository:

1. Adds thumbnails to the *Cxx.md*, *Python.md* etc. summary pages.  A thumbnail is added if a _src/Testing/Baseline/_**LANG**/**TOPIC**/_Test_**EXAMPLE**_.png_ image exists for the example.

2. Copies non-example *.md* files like *Cxx.md* and *Python.md* files to *docs/*.

3. For each example source file create _docs/_**LANG**/**TOPIC**/**EXAMPLE**_.md_ file that contains:
    1. If _src/Testing/Baseline/_**LANG**/**TOPIC**_/Test_**EXAMPLE**_.png_ exists add the image
    2. If a _src/_**LANG**/**TOPIC**/**EXAMPLE**_.md_ description file exists, insert the description
    3. Insert the source code from _src/_**LANG**/**TOPIC**/**EXAMPLE**.**EXTENSION**

4. For each Cxx source file
    1. If _Cxx/_**TOPIC**/**EXAMPLE**_.cmake_ exists, add it as the CMakeLists section of the _.md_ file, otherwise, fill in the *VTKCMakelists* template, including the files listed in the optional _src/Cxx/_**TOPIC**/**EXAMPLE**_.extras_ file.

5. For each Cxx source file, create a _src/Tarballs/_**EXAMPLE**_.tar_ file containing the source and _CMakeLists.txt_ file.

### [SyncSiteWithRepo.sh](__BLOB__/src/SyncSiteWithRepo.sh)

1. Checks for a virtual environment called `vtk-examples-web` and activates it. This environment contains `mkdocs-material` and `htmlmin`. Installing `mkdocs-material` will automatically install compatible versions of all dependencies: `MkDocs`, `Markdown`, `Pygments` and `Python Markdown Extensions`.

    **Note:** There are instructions for how to setup the virtual environment in the header comments of the script.

2. Copies across all needed files such as baseline images and other essential files like `mkdocs.yml` using rsync.

3. Creates the coverage files

    The [VTKClassesUsedInExamples.py](__BLOB__/src/Admin/VTKClassesUsedInExamples.py) python script generates two tables for each language. One table lists each class and what classes it uses. The second table lists the classes that are not used in any example.

4. Wipes the *docs* and *site* directory

    The *docs* directory contains all of the md and HTML files for the site. A clean directory prevents old files from being used.

    The *site* directory contains all of the static html files for the site. A clean directory prevents old files from being used.

5. Runs the [ScrapeRepo.py](__BLOB__/src/Admin/ScrapeRepo.py) python script.

    This script populates the *docs* directory.

6. Checks for a successful scrape

     This sanity check sees if a reasonable number of files have been updated.

7. Copies the style sheets and runs mkdocs to build the static website in folder called `site`.

8. Minifies the HTML
