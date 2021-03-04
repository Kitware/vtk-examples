### Description

Using a QVTKOpenGLNativeWidget with the Qt Single Inheritance model.

It is a good idea to cleanly separate the public interface of the class from its internals.

This approach derives the main widget from QMainWindow and the class defining the form `Ui::RenderWindowUISingleInheritance` is a variable in the class.

The procedure is:

- In the header file `RenderWindowUISingleInheritance.h` forward declare the class `Ui_RenderWindowUISingleInheritance`, and define a private variable in the main widget derived from `QMainWindow`, called `ui`.
- In the source file `RenderWindowUISingleInheritance.cxx` assign and initialise:
  ``` cxx
    this->ui = new Ui_RenderWindowUISingleInheritance;
    this->ui->setupUi(this);
  ```
This approach also ensures that when `CMAKE_AUTOUIC` is set to `ON` and subfolders are used we get a build with no errors.
