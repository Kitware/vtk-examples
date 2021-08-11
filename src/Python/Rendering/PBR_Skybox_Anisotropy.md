### Description

Demonstrates physically based rendering (PBR) using image based lighting, anisotropic texturing and a skybox.

Physically based rendering sets metallicity, roughness, occlusion strength and normal scaling of the object. Textures are used to set base color,  ORM, anisotropy and normals. Textures for the image based lighting and the skymap are supplied from a cubemap.

Image based lighting uses a cubemap texture to specify the environment.

Textures are provided for:

- setting the base colour
- normals  
- **O**cclusion factor, the **R**oughness factor, and the **M**etalness factor (ORM)
- anosotropy

A Skybox is used to create the illusion of distant three-dimensional surroundings.

You can select different cubemaps and different surfaces to visualise.

Additionally HDR or JPEG equirectangular Environment maps can be used to generate the cubemaps. A good source for Skybox HDRSs and Textures is [Poly Haven](https://polyhaven.com/all). Start with the 4K HDR versions of Skyboxes.

The parameters for this program are rather complex. Here are the parameters used to make the example image:

``` text
<DATA>/Skyboxes/spiaggia_di_mondello_4k.hdr  <DATA1>/CarbonFiberAniso_albedo.png <DATA1>/CarbonFiberAniso_normal.png <DATA1>/CarbonFiberAniso_orm.png <DATA1>/CarbonFiberAniso_anisotropyAngle.png Boy
```

We can also use the six individual cubemap files by specifying the folder where the cubemap files are:

``` text
<DATA>/Skyboxes/skybox2/    <DATA1>/CarbonFiberAniso_albedo.png <DATA1>/CarbonFiberAniso_normal.png <DATA1>/CarbonFiberAniso_orm.png <DATA1>/CarbonFiberAniso_anisotropyAngle.png Boy
```

We also provide an equirectangular file (useful when using ParaView): `<DATA>/Skyboxes/skybox2/skybox.jpg`.

Where `<DATA>` is the path to `?vtk-?examples/src/Testing/Data` and `<DATA1>` is the path to `?vtk-?examples/src/Testing/Data/Textures/Anisotropic`.

Note that three cubemap skyboxes are available in `<DATA>/Skyboxes/` namely `skybox0`, `skybox1` and `skybox2`.

#### Further Reading

- [Introducing Physically Based Rendering with VTK](https://blog.kitware.com/vtk-pbr/)
- [PBR Journey Part 1: High Dynamic Range Image Based Lighting with VTK](https://blog.kitware.com/pbrj1/)
- [PBR Journey Part 2 : Anisotropy model with VTK](https://blog.kitware.com/pbr-journey-part-2-anisotropy-model-with-vtk/)
- [PBR Journey Part 3 : Clear Coat Model with VTK](https://blog.kitware.com/pbr-journey-part-3-clear-coat-model-with-vtk/)

!!! note
    - Support was added for [HDR images in VTK 9.0](https://blog.kitware.com/pbrj1/), Thus for better performance, the skybox texture should set `MipmapOn` as well as `InterpolateOn` to exploit hardware capabilities when using HDR images.
    - We are using a cubemap texture (and not a traditional texture), so one should set `UseSphericalHarmonicsOff` on the texture as well. This means that we need to use vtkOpenGLRenderer instead of vtkRenderer as the renderer.
    - Occlusion is subtle, you will see it in the shadowing around objects on the surface.
    - Physically Based Rendering (PBR) is only available in VTK 9.0+
    - The cubemap/skybox files are found in `?vtk-?examples/src/Testing/Data/skyboxes/`.
