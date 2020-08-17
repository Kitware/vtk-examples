## Developers

The success of the VTK Examples depends on the contributions from the VTK user community. If you wish to contribute to this valuable resource, please follow these guidelines. If you are a VTK Example User, [go here](../ForUsers) or an Example Administrator [go here](../ForAdministrators).

C++, C#,  Python, and Java examples are welcome! Examples should illustrate a single concept.

## Follow the Coding Guidelines

When you write an example, please follow the [coding guidelines](../Guidelines). Create the example in your local repository, compile and run it before you generate a pull request.

Some additional steps need to be done for Python C# and Java, see the sections below.

## Setup for Development

This assumes you have `VTK`, `CMake`, `git` and `git lfs` installed.

### Fork the repository

Go to the [VTK Examples Repository](__SITE_REPOSITORY__), sign in/register, and then fork the repository.

A fork is a copy of a project. Forking a repository allows you to make changes without affecting the original project.

### Clone the VTK examples repository onto your local machine and build it

If you are using SSH:

``` bash
    git clone git@gitlab.kitware.com:<username>vtk-examples.git
```

If You are using HTTPS:

``` bash
    git clone https://gitlab.kitware.com/<username>/vtk-examples.git
```

### Add the vtk-examples repository as a *remote* called *upstream*

``` bash
    git remote add upstream __SITE_REPOSITORY__
```

### Now build it

``` bash
    cd __REPO_NAME__
    cd build
    cmake -DVTK_DIR:PATH=YOUR_VTK_BIN_DIR -DBUILD_TESTING:BOOL=ON ..
    make
```

   where **YOUR_VTK_BIN_DIR** is the location of your VTK build.

### Before adding your examples

Sync your repository with the __REPO_NAME__ repository. Remember that to run the following commands, you need to be in the **__REPO_NAME__** directory.

``` bash
    git fetch upstream
    git checkout master
    git merge upstream/master
    git push
```

* Build the __REPO_NAME__ code

``` bash
    cd __REPO_NAME__
    cd build
    cmake -DVTK_DIR:PATH=YOUR_VTK_BIN_DIR -DBUILD_TESTING:BOOL=ON ..
    make
```

where **YOUR_VTK_BIN_DIR** is the location of your VTK build.

### Add the example

Create a branch in your repository

``` bash
    git checkout -b MyNewExample
```

  where **MyNewExample** is the name of your new example.

#### Choose a Topic

The examples are organized by topic. Current topics include Animation,
DataStructures, Filters, GeometricObjects, Images, Meshes, etc.

#### Write the source

* The following snippets may be of use:

  * [Cxx available snippets](../../Cxx/Snippets).
  * [Python available snippets](../../Python/Snippets).
  * [Java available snippets](../../Java/Snippets).

* Save your source code in __REPO_NAME__/src/**LANGUAGE**/**TOPIC**/

    Where **LANGUAGE** is Cxx, Python, CSharp or Java and **TOPIC** is the topic that you have chosen.

#### Steps for C++

* Build and test your example (**NOTE:** only for cxx examples)

``` bash
        cd __REPO_NAME__/build
        cmake ..
        make
        ctest -V -R MyNewExample
```

Note: If **MyNewExample** is not built, then in the directory where you put the file, do:

``` bash
    touch CMakeLists.txt
```

* If your C++ example does any rendering, the test will fail the first time and create an image in __REPO_NAME__/build/Testing/Temporary. The image will be called Test**MyNewExample**.png.

* Copy the image into: __REPO_NAME__/src/Testing/Baseline/**LANG**/**TOPIC**/. For Python and other languages, create an image with the proper name using a screen capture and copy that image into the proper location.

* Rerun ctest and the test should pass.

At this point you are ready to push the changes to GitLab.

#### Steps for Python, Java and C#

* [ ] *TODO: Add in new instructions for* ***Python***.

If you are basing your code on a Cxx example use the same name as the Cxx example. This ensures that automatic cross-referencing works.

Keep the same directory structure as that in Cxx.

* Follow first three steps above.
* Manually create a test image if your example does any rendering. The image should be called Test**MyNewExample**.png.

  The following snippets can be used to write the image out:

  * [WriteImage](../../Python/Snippets/WriteImage/) for Python
  * [WriteImage](../../Java/Snippets/WriteImage/) for Java

#### Add the example to the language page

Depending on the language of your example, edit the file *Cxx.md*, *Python.md*, *CSharp.md*, or *Java.md*.

Find the section for your topic and add a line for your new example. The format of the line is:

\[**MyNewExample**\]\(/**LANG**/**TOPIC**/**MyNewExample**\) | doxygen | short description

where **LANG** is one of Cxx, Python, CSharp, Java.

#### Commit your changes to your topic branch

``` bash
git add MyNewExample.cxx
```

and if you have a baseline image,

``` bash
git add Testing/Baseline/LANG/TOPIC/TestMyNewExample.png
```

finally,

``` bash
git commit
```

### Push the changes to GitLab

``` bash
git push origin MyNewExample
```

#### Go to your GitLab project

Then generate a merge request for review.

See here for [how to generate a merge request](https://docs.gitlab.com/ee/user/project/merge_requests/creating_merge_requests.html).

### Advanced usage

#### Add a description

If your example could benefit from an extended description, you can create a file **MyNewExample.md**. Store the file alongside your source code. Use [markdown](https://guides.github.com/features/mastering-markdown/) to format the description. Also look at existing files for layout ideas.

#### Add arguments to the test

If your example requires arguments, you will need to edit the *CMakeLists.txt* in the topic directory.

* Add the name of your example to the *NEEDS_ARGS* variable

* Add an *ADD_TEST* line. See other *CMakeLists.txt* files for examples.

#### Add extra files to a C++ example

Most C++ examples consist of one file. If other files are required,
place them in the same directory as the example. Then add a file with
the same prefix as the example name and a *.extras* suffix. List each
extra filename in the *.extras* file, one filename per line.

!!! warning
    If you add extra files to the example, but do not add their filenames to the *.extras* file, they will appear in the left-hand file menus **and** will not be included in the tar file for the example.
