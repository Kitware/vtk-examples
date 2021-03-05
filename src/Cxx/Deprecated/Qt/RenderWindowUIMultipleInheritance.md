### Description

Using a QVTKOpenGLNativeWidget with the Qt Multiple Inheritance model. In contrast to the single inheritance example the main widget is here derived from QMainWindow as well as the class defining the form `Ui::RenderWindowUIMultipleInheritance`.

This approach will not work if `CMAKE_AUTOUIC` is set to `ON` and subfolders are used. It is a good idea to cleanly separate the public interface of the class from its internals. In the simplest case, only include ui_XXX.h in the .cpp source file. For more information see [AUTOUIC](https://cmake.org/cmake/help/latest/prop_tgt/AUTOUIC.html).
