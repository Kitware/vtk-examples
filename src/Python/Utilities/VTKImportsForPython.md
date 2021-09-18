### Description

This example lets you generate a series of import statements for your Python code.

The imports are generated using the VTK modules, along with the VTK classes and constants in your Python source file(s).
For older versions of VTK, `modules.json` is required, this is found in your VTK build directory.

For instance if you have:

``` Python
import vtk
```

It will output either a text file, or to the console, a series of `from ... import` statements where the first line is the program name and subsequent lines are the import statements.

You can replace `import vtk` with these statements. This means that only the relevant VTK modules are loaded when the Python program runs.

Of course after adding these statements you may need to edit your code.
