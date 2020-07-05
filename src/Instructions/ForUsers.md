## Users

If you want to use [VTK Ex](https://github.com/ajpmaclean/VTKEx), you have several options. If you are a VTK Example Developer, [go here](../ForDevelopers) or a VTK Example Administrator [go here](../ForAdministrators).

## Build an example

At the bottom of each example page, there are instructions on how to build the example.

## Build all of the examples

If you are getting excited about learning VTK and are tired of
downloading individual examples, you can build them all.

1. As a VTK Remote module

   When you configure your VTK build, set Module_VTKEx:BOOL=ON and rebuild VTK.

2. Download a [zip](https://github.com/ajpmaclean/VTKEx/archive/master.zip) containing the source.

3. Clone the VTK examples repository

``` bash
   git clone https://github.com/ajpmaclean/VTKEx.git
   cd VTKEx
   cd build
   cmake -DVTK_DIR:PATH=YOUR_VTK_BIN_DIR -DBUILD_TESTING:BOOL=ON ..
   make
```

   where **YOUR_VTK_BIN_DIR** is the location of your VTK build.

### Update the examples repository

If you cloned the examples repository, you can get the latest updates:

``` bash
cd VTKEx
git pull
cd build
cmake ../VTKEx
make
```

### Run all of the examples

``` bash
ctest
```
