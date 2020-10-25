### Description

The only thing you have to do to enable your class to work with vtkDenseArray is add a single line:

``` C++
operator vtkVariant() const { return vtkVariant(); }
```
