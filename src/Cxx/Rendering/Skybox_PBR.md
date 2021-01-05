### Description

Demonstrates physically based rendering using image based lighting and a skybox.

Physically based rendering sets color, metallicity and roughness of the object, sliders are provided for:
- metallicity
- roughness

Image based lighting uses a cubemap texture to specify the environment.

A Skybox is used to create the illusion of distant three-dimensional surroundings.

Textures for the image based lighting and the skymap are supplied from a cubemap.

You can select different cubemaps and different surfaces to visualise.

For more information, see [Introducing Physically Based Rendering with VTK](https://blog.kitware.com/vtk-pbr/).

!!! note
    - Support was added for [HDR images in VTK 9.0](https://blog.kitware.com/pbrj1/), Thus for better performance, the skybox texture should set `MipmapOn` as well as `InterpolateOn` to exploit hardware capabilities when using HDR images.
    - We are using a cubemap texture (and not a traditional texture), so one should set `UseSphericalHarmonicsOff` on the texture as well. This means that we need to use `vtkOpenGLRenderer` instead of `vtkRenderer` as the renderer.
    - Occlusion is subtle, you will see it in the shadowing around the letters VTK on the surface.
    - Physically Based Rendering (PBR) is only available in VTK 9.0+
    - The cubemap/skybox files are found in `VTKExamples/Testing/Data/skyboxes/`.

