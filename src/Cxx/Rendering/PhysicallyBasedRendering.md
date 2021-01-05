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

For more information, see [Introducing Physically Based Rendering with VTK](https://blog.kitware.com/vtk-pbr/).

!!! note
    - Support was added for [HDR images in VTK 9.0](https://blog.kitware.com/pbrj1/), Thus for better performance, the skybox texture should set `MipmapOn` as well as `InterpolateOn` to exploit hardware capabilities when using HDR images.
    - We are using a cubemap texture (and not a traditional texture), so one should set `UseSphericalHarmonicsOff` on the texture as well. This means that we need to use `vtkOpenGLRenderer` instead of `vtkRenderer` as the renderer.
    - Occlusion is subtle, you will see it in the shadowing around the letters VTK on the surface.
    - Physically Based Rendering (PBR) is only available in VTK 9.0+
    - The cubemap/skybox files are found in `VTKExamples/Testing/Data/skyboxes/`.

