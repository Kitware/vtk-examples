### Description

This example shows how to add an observer to a C++ program. It extends
the [Tutorial_Step1](../Tutorial_Step1) example (see that example for information on
the basic setup).

VTK uses a command/observer design pattern. That is, observers watch for
particular events that any vtkObject (or subclass) may invoke on
itself. For example, the vtkRenderer invokes a "StartEvent" as it begins
to render. Here we add an observer that invokes a command when this event
is observed.
