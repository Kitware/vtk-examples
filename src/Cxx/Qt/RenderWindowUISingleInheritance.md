### Description

Using a QVTKOpenGLNativeWidget with the Qt Single Inheritance model. This cleanly separates the public interface of the class from its internals.

The main widget is derived from from `QMainWindow` and the class `Ui::RenderWindowUISingleInheritance` becomes a variable in the class.

The procedure is:

- In the header file `RenderWindowUISingleInheritance.h` forward declare the class `Ui_RenderWindowUISingleInheritance` and create a private variable in the class:

``` cxx
private:
  // Designer form
  Ui_RenderWindowUISingleInheritance* ui;
```

- In the source file `RenderWindowUISingleInheritance.cxx`, assign and initialise in the constructor:

``` cxx
  this->ui = new Ui_RenderWindowUISingleInheritance;
  this->ui->setupUi(this);
```

This approach also ensures that when `CMAKE_AUTOUIC` is set to `ON` and subfolders are used we get a build with no errors.
