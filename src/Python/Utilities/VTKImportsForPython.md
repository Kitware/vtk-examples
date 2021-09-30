### Description

Use this to generate a series of import statements for your Python code.

The imports are generated using the VTK modules, along with the VTK classes and constants in your Python source file(s).
For older versions of VTK, `modules.json` is required, this is found in your VTK build directory.

When this script is run against your code, a series of `from ... import` statements are generated, based on the classes you have used. The result will be output to the console, or, alternatively to a text file with extension `.txt`. The first line is the program name and subsequent lines are the import statements.

At the end of the list there is a series of commented out statements consisting of imports that you may need to enable. Only enable the ones you really need and include the statement `# noinspection PyUnresolvedReferences` for PyCharm users, as this will prevent the statement from being removed.

e.g The most common ones will be:

``` Python
    # noinspection PyUnresolvedReferences
    import ?vtkmodules?.?vtkInteractionStyle?
    # noinspection PyUnresolvedReferences
    import ?vtkmodules?.?vtkRenderingOpenGL2?
```

Make sure that any of these statements are placed after the last `from ... import ...` statement. Also remove any unused ones.

As an example, if you have used `import vtk`, you can replace it with with these statements. This means that only the relevant VTK modules are loaded when the Python program runs.

Of course after adding these statements you may need to edit your code e.g. changing `x = vtk.?vtkSomeClass?()` to `x = ?vtkSomeClass?()`.
