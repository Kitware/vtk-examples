!!! info
    The example uses `src/Testing/Data/FullHead.mhd` which references `src/Testing/Data/FullHead.raw.gz`.
### Description

This examples show how volume rendering can produce isosurface-like images. Using vtkOpenGLGPUVolumeRayCastMapper with an isosurface blend mode, two isosurfaces are created using appropriate transfer functions. The effect is similar to what is shown in [MedicalDemo2](/Cxx/Medical/MedicalDemo2). The user can specify the **.mhd** file and the two isosurface values. The example uses 500 and 1150 for the two isosurface values. The volume rendering "surfaces" are fuzzier than the hard surfaces created by vtkMarchingCubes in MedicalDemo3.

!!! example "Usage"

``` C++
    RayCastIsosurface FullHead.mhd 500 1150
```

!!! info
    The example uses `src/Testing/Data/FullHead.mhd` which references `src/Testing/Data/FullHead.raw.gz`.
