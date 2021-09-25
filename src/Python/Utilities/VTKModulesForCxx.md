### Description

Use this to generate a `find_package(VTK COMPONENTS ...)` command for CMake.

It requires `modules.json`, found in your VTK build folder, and your source files. After running, it will generate a `find_package(VTK COMPONENTS ...)` command listing all the vtk modules needed by the C++ source and header files in your code.

Paths for more than one source path can be specified. If there are spaces in the paths, enclose the path in quotes.

If it is unable to find modules for your headers then a list of these, along with the files they are in, is produced so you can manually add the corresponding modules or rebuild VTK to include the missing modules.

You will need to manually add any third-party modules (if used) to the find_package command.
