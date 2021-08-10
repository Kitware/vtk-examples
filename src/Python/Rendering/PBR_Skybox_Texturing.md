### Description

Demonstrates physically based rendering (PBR) using image based lighting, texturing and a skybox.

Physically based rendering sets metallicity, roughness, occlusion strength, the emissive factor and normal scaling of the object. Textures are used to set base color,  ORM, emissivity and normals. Textures for the image based lighting and the skymap are supplied from a cubemap.

Sliders are provided for:

- metallicity
- roughness
- occlusion strength
- normal scaling

Image based lighting uses a cubemap texture to specify the environment.

Textures are provided for:

- setting the base colour
- **O**cclusion factor, the **R**oughness factor, and the **M**etalness factor (ORM)
- emissivity
- normals  

A Skybox is used to create the illusion of distant three-dimensional surroundings.

You can select different cubemaps and different surfaces to visualise.

Additionally HDR or JPEG equirectangular Environment maps can be used to generate the cubemaps. A good source for Skybox HDRSs and Textures is [Poly Haven](https://polyhaven.com/all). Start with the 4K HDR versions of Skyboxes.

The parameters for this program are rather complex. Here are the parameters used to make the example image:

``` text
<DATA>/skyboxes/skybox2/ <DATA1>/vtk_Base_Color.png <DATA1>/vtk_Normal.png <DATA1>/vtk_Material.png <DATA1>/vtk_dark_bkg.png Boy
```

We also provide an equirectangular file (useful when using ParaView): `<DATA>/skyboxes/skybox2/skybox.jpg`.

where `<DATA>` is the path to `?vtk-?examples/src/Testing/Data` and `<DATA1>` is the path to `?vtk-?examples/src/Testing/Data/Textures/Isotropic`. Also note that three skyboxes are available: `skybox0`, `skybox1` and `skybox2`.

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
