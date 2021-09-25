### Description

As a result of historical and evolving C++ standards there are are multiple ways of doing the same thing with ?vtkDataArrays? in the VTK C++ code base. This example demonstrates various ways of iterating through a vtkDataArray and operating on the resultant tuple to create a new result. The result of the operation is inserted into a new vtkDataArray.

In this example, all the functions operate on a `?vtkTypeFloat64Array?` of vectors to compute a `?vtkTypeFloat64Array?` of magnitudes. We also test to ensure the correct magnitudes are generated.

This example demonstrates:

- The classic for loop e.g. `for (auto c = 0; c < 3; ++c)` and `static_cast`.

    - `naivemag3`, demonstrates the classic for loop using a counter.
    - `mag3GetPointer`, uses raw pointers, it becomes a little complicated calling `mag3Trampoline` which then calls `mag3Helper`. Lots of `static_cast` has to be used.
    - `mag3Explicit`, instantiate a templated function with explicit types. It is similar to `naivemag3`.

- A dispatcher/worker paradigm. This is the **recommended approach**. In the dispatcher function, we instantiate a worker. Then we use vtkArrayDispatch to generate optimised workers when the arrays are both float or double and fall back to just using the worker for other situations.

    - `mag3Dispatch1` instantiates the struct `Mag3Worker1` where we use accessors, APIType and assume the tuple sizes.
    - `mag3Dispatch2a` instantiates the struct `Mag3Worker2a` where range objects are used.
    - `mag3Dispatch2b` instantiates the struct `Mag3Worker2b` where range objects are used. Here ReferenceType and ConstReferenceType are used. Also elements in the range are accessed using **operator[]** like a STL indexed container.
    - `mag3Dispatch3` instantiates the struct `Mag3Worker3` where range objects are used. Here ValueType and ConstReferenceType are used. We also create a lambda to calculate the magnitude for each tuple. This is then used in `std::transform` to generate the magnitudes.

Refer to [Further reading](#further-reading) for more information.

**Note** that VTK provides a (not so well known) series of templated data types that are especially useful when dealing with data from other sources:

- `?vtkTypeFloat32Array?`, `?vtkTypeFloat64Array?`,
- `?vtkTypeInt8Array?`, `?vtkTypeInt64Array?`,
- `?vtkTypeUInt8Array?`, `?vtkTypeUInt64Array?`

#### Best Practice

**Yohann Bearzi** has provided the following guidelines for best practices:

- Whenever you hold a vtkDataArray for which you don't know the underlying type (i.e. you cannot safely `?vtkArrayDownCast?`), you should use vtkArrayDispatch and write your accesses in a functor. This prevents a bunch of implicit `static_cast`.
- Whenever you hold a vtkDataArray on which you know the underlying type (ghost arrays for instance vtkUnsignedCharArray, or global ids vtkIdTypeArray), you should use `?vtkArrayDownCast?`.
- When you finally hold a typed instance of vtkDataArray:

    - If the array only holds values (tuples with one component), use `GetValue`.
    - If the array holds tuples, you can prevent a copy for each access with AOS arrays by using `vtk::ArrayTupleRange`. In this case, the pointer of the corresponding tuple in the array is directly used for access.

If you want to use STL algorithms, such as `std::transform` or `std::sort`, convert your downcasted array into a range and proceed.

If you know at compile time how many components are in your array, you should template `vtk::ArrayTupleRange` and `vtk::ArrayValueRange` with the number of components (3 for 3D points, for instance).

It is left as an exercise for the reader to identify best practices in the example.

#### Thanks

Special thanks must go to **wangzhezhe** for developing the source code on which this example is based, see this discourse article: [get the raw pointer from the ?vtkPoints?](https://discourse.vtk.org/t/get-the-raw-pointer-from-the-?vtkpoints?/4894), the original source code is here: [forloops.cpp](https://github.com/wangzhezhe/5MCST/blob/master/vtk_example/array/forloops.cpp).

#### Further reading

For further reading please see:

- [Working with ?vtkDataArrays?: 2019 Edition](https://blog.kitware.com/working-with-?vtkdataarrays?-2019-edition/)
- [C++11 for-range support in VTK](https://blog.kitware.com/c11-for-range-support-in-vtk/)
- [New Data Array Layouts in VTK 7.1](https://blog.kitware.com/new-data-array-layouts-in-vtk-7-1/)
