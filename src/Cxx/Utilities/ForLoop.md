### Description

Here we demonstrate the various ways of doing a for loop.

As a result of historical and evolving C++ standards there are are multiple ways of doing the same thing with ?vtkDataArrays? in the VTK C++ code base.

The **recommended** way now is to use the **dispatch mechanism** as shown in the example. Four functions called `mag3Displatch*` are provided in the example to illustrate various ways of doing this.

In the example, all the functions operate on a `?vtkTypeFloat64Array?` of vectors to compute a `?vtkTypeFloat64Array?` of magnitudes. We also test to ensure the correct magnitudes are generated.

Another thing, not so well known, is that VTK provides a series of templated data types:

- `?vtkTypeFloat32Array?`, `?vtkTypeFloat64Array?`,
- `?vtkTypeInt8Array?`, `?vtkTypeInt64Array?`,
- `?vtkTypeUInt8Array?`, `?vtkTypeUInt64Array?`

These are especially useful when dealing with data from other sources.

#### Best Practice

**Yohann Bearzi** has provided the following guidelines for best practices:

- Whenever you hold a vtkDataArray for which you don't know the underlying type (i.e. you cannot safely `?vtkArrayDownCast?`), you should use vtkArrayDispatch and write your accesses in a functor. This prevents a bunch of implicit `static_cast`.
- Whenever you hold a vtkDataArray on which you know the underlying type (ghost arrays for instance vtkUnsignedCharArray, or global ids vtkIdTypeArray), you should use `?vtkArrayDownCast?`.
- When you finally hold a typed instance of vtkDataArray:

    - If the array only holds values (tuples with one component), use `GetValue`.
    - If the array holds tuples, you can prevent a copy for each access with AOS arrays by using `vtk::ArrayTupleRange`. In this case, the pointer of the corresponding tuple in the array is directly used for access.

If you want to use STL algorithms, such as `std::transform` or `std::sort`, convert your downcasted array into a range and proceed.

If you know at compile time how many components are in your array, you should template `vtk::ArrayTupleRange` and `vtk::ArrayValueRange` with the number of components (3 for 3D points, for instance).

The functions in the example illustrate various aspects of these guidelines.

It is left as an exercise for the reader to identify best practices in the example.

#### Thanks

Special thanks must go to **wangzhezhe** for developing the source code on which this example is based, see this discourse article: [get the raw pointer from the ?vtkPoints?](https://discourse.vtk.org/t/get-the-raw-pointer-from-the-vtkpoints/4894), the original source code is here: [forloops.cpp](https://github.com/wangzhezhe/5MCST/blob/master/vtk_example/array/forloops.cpp).

#### Further reading

For further reading please see:

- [Working with ?vtkDataArrays?: 2019 Edition](https://blog.kitware.com/working-with-vtkdataarrays-2019-edition/)
- [C++11 for-range support in VTK](https://blog.kitware.com/c11-for-range-support-in-vtk/)
- [New Data Array Layouts in VTK 7.1](https://blog.kitware.com/new-data-array-layouts-in-vtk-7-1/)
