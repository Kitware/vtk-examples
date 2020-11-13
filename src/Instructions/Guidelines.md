## Guidelines

Although education of new users is the main motivation, the VTK Examples should also:

1. Encourage good programming style
2. Promote the proper and modern way to use VTK and write VTK programs
3. Facilitate the nightly compilation and testing of examples that reside in the VTK Examples repository.

These requirements must be met without compromising the main goal of user education.

## Guidelines for each language

All examples should follow the VTK programming style and there should be a single blank line at the end of the example

### C++

* The indentation style can be characterized as the [AllmannStyle](https://en.wikipedia.org/wiki/Indent_style#Allman_style).
The curly brace (scope delimiter) is on a separate line and aligns with the control statement.
The control block is indented by two spaces (**no tabs**).
A suitable `.clang-format` is provided in `src/Cxx`.
Most IDE's can be be configured to use this file for formatting.
See [clang-format](https://clang.llvm.org/docs/ClangFormat.html) for more information.

    Example:

``` c++
    if (this->Locator == locator)
    {
      return;
    }
    for (i = 0; i < this->Source->GetNumberOfPoints(); i++)
    {
      p1 = this->Source->GetPoint(i);
      [...]
    }
```

* Where appropriate, explicitly use the **std::** namespace:

``` c++
    std::cout << "Print something" << std::endl;
```

  rather than

``` c++
    cout << "Print something" << endl;
```

* All includes from the toolkit should use **<...>** notation. This follows C++ programming conventions.

    For example: `#include <vtkContourFilter.h>` is preferred over `#include "vtkContourFilter.h"`

* The main program must have the following signature:

``` c++
    int main (int argc, char *argv[])
```

  or, if argc and argv are not referenced in the code,

``` c++
    int main (int, char *[])
```

* If arguments are required, a check similar to the following should be made at the start of the main program.

``` c++
  float alpha = 0.5;
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " filename.vtp [alpha] e.g. Bunny.vtp"
              << std::endl;
    return EXIT_FAILURE;
  }
  if (argc > 2)
  {
    alpha = atof(argv[2]);
  }
```

* An example should **never call exit()**. If the main program executes successfully, it should

``` c++
    return EXIT_SUCCESS;
```

  otherwise

``` c++
    return EXIT_FAILURE;
```

* The use of SmartPointers is preferred in VTK examples, generally use `vtkNew` and only use `vtkSmartPointer`where appropriate.

``` c++
    vtkNew<vtkCutter> cutter;
```

  or (where needed)

``` c++
    auto cutter = vtkSmartPointer<vtkCutter>::New();
```

* When building pipelines, the new `SetInputConnection()`, `GetOutputPort()` methods should be used.

* Input/Output filenames

    When possible, filenames should be passed on the command line. This gives the examples utility beyond the data that is used in the specific example.

* If there are just a few parameters for the example, these should be passed in as arguments. This increases the utility of the example and facilitates testing.

    For example, this program

``` c++
    Delaunay3D InputPolydataFileName(.vtp) Alpha
```

    would use the arguments in this manner

``` c++
    reader->SetFileName (argv[1]);
    delaunay3D->SetAlpha(atof(argv[2]));
```

* Always provide a background for the renderers. Avoid setting the background to white.

* Use [vtkNamedColors](http://www.vtk.org/doc/nightly/html/classvtkNamedColors.html) for setting colors of actors and renderer backgrounds.
[VTKNamedColorPatches](http://htmlpreview.github.io/?__WEB_BLOB__/VTKNamedColorPatches.html) shows the colors that are available. If you are using a color series, then you can choose what you want from here [VTKColorSeriesPatches](http://htmlpreview.github.io/?__WEB_BLOB__/VTKColorSeriesPatches.html).

  For example,

``` c++
  #include <vtkNamedColors.h>

  int main(int, char*[])
  {

    vtkNew<vtkNamedColors> namedColors;

    renderer->SetBackground(namedColors->GetColor3d("Khaki").GetData());
```

  is preferred over

``` c++
    renderer->SetBackground(0.9412, 0.9020, 0.5490);
```

* Use admonitions to warn/cite/info, etc. [Here is a summary of admonitions](__WEB_SITE_URL__/Instructions/ForAdministrators/#admonition).

### Python

In general, Python submissions should follow the VTK Programming style and the comments outlined for C++ above (with language appropriate modification).

Python code styling follows [PEP 8 -- Style Guide for Python Code](https://www.python.org/dev/peps/pep-0008/)

Python code is tested in a similar manner to C++ code, so it must follow this layout:

``` Python
#!/usr/bin/env python

import vtk

def main(argv):
    """
    Get parameters (if needed)
    Instantiate your classes, call them and any defs.
    """


if __name__ == '__main__':
    import sys
    main(sys.argv)

```

Use `argparse` where needed:

``` Python

#!/usr/bin/env python

import vtk


def main(argv):
    xyz_fn, q_fn = get_program_parameters(argv)
    colors = vtk.vtkNamedColors()

    # Do the work

def get_program_parameters(argv):
    import argparse
    description = 'Some description.'
    epilogue = '''
    Extra information.
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('xyz_filename', help='combxyz.bin.')
    parser.add_argument('q_filename', help='combq.bin.')
    args = parser.parse_args()
    return args.xyz_filename, args.q_filename


if __name__ == '__main__':
    # import sys
    main(sys.argv)

```

For the Input/Output of filenames and parameters. Use this snippet [GetProgramParameters](__WEB_SITE_URL__/Python/Snippets/GetProgramParameters/) 

### Java

In general, Java submissions should follow the VTK Programming style and the comments outlined for C++ above (with language appropriate modification).

For Java code layout, look at [CylinderExample](__WEB_SITE_URL__/Java/GeometricObjects/CylinderExample/)

Java code styling follows the usual style as implemented in the IDEs.

However note:

* No Tabs
