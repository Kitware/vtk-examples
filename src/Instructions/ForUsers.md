## Users

This assumes you have `VTK`, `CMake`, `git` and `git lfs` installed.

If you want to use the examples repository [VTK Examples](__SITE_REPOSITORY__), you have several options:

- If you want to add/update examples: [go here](../ForDevelopers)
- Users: Read the following instructions.

## Build an example

For C++ examples, go to the bottom of the page for the example that you have selected and read the instructions on how to build the example. A **\*.tar** file is provided with the code, a CMake file and a build directory. Additionally, if there are examples in other languages, these are provided also.

For other languages, you can copy the code to your clipboard and paste into your favourite editor.

## Build all of the examples

If you are getting excited about learning VTK and are tired of
downloading individual examples, you can build them all.

Go to the [VTK Examples](__SITE_REPOSITORY__) and you can: 

- Download the repository and untar or unzip it as needed into a folder called `__REPO_NAME__`. Then go to the folder and:

``` bash
   cd __REPO_NAME__
   cd build
   cmake -DVTK_DIR:PATH=YOUR_VTK_BIN_DIR -DBUILD_TESTING:BOOL=ON ..
   make
```

   where **YOUR_VTK_BIN_DIR** is the location of your VTK build.

- Clone the VTK examples repository:

``` bash
   git clone __GIT_REPO__
   cd __REPO_NAME__
   cd build
   cmake -DVTK_DIR:PATH=YOUR_VTK_BIN_DIR -DBUILD_TESTING:BOOL=ON ..
   make
```

   where **YOUR_VTK_BIN_DIR** is the location of your VTK build.

### Update the examples repository

From now on, if you cloned the examples repository, you can get the latest updates:

``` bash
cd __REPO_NAME__
git pull --rebase
cd build
cmake ../__REPO_NAME__
make
```

### Run all of the examples

``` bash
ctest
```
