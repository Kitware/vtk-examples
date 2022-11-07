### Description

Demonstrates physically based rendering (PBR) using image based lighting, texturing and a skybox.

Physically based rendering sets metallicity, roughness, occlusion strength, the emissive factor and normal scaling of the object. Textures are used to set base color, ORM, emissivity and normals. Textures for the image based lighting and the skymap are supplied from a cubemap.

Image based lighting uses a cubemap texture to specify the environment. A Skybox is used to create the illusion of distant three-dimensional surroundings. Textures for the image based lighting and the skybox are supplied from an HDR or JPEG equirectangular Environment map or cubemap consisting of six image files.

A good source for Skybox HDRs and Textures is [Poly Haven](https://polyhaven.com/all). Start with the 4K HDR versions of Skyboxes.

The parameters used to generate the example image are loaded from a JSON file with the same name as the example. In this case:

``` text
<DATA>/PBR_Skybox_Texturing.json
```

Where `<DATA>` is the path to `?vtk-?examples/src/Testing/Data`.

By default we use the equirectangular file to generate the texture for the lighting and skybox. We have optionally provided six individual cubemap files to generate lighting and a skybox.

For information about the parameters in the JSON file, please see [PBR_JSON_format](../../Documentation/PBR_JSON_format.md).

### Options

``` text
Positionals:
 fileName              The path to the JSON file containing the parameters.

Options:
 -h,--help             Print this help message and exit
 -s,--surface          The name of the surface. Overrides the surface entry in the json file.
 -c,--use_cubemap      Build the cubemap from the six cubemap files. Overrides the equirectangular entry in the json file.
 -t, --use_tonemapping Use tone mapping.
```

Additionally, you can save a screenshot by pressing "k".

#### Further Reading

- [Introducing Physically Based Rendering with VTK](https://blog.kitware.com/vtk-pbr/)
- [PBR Journey Part 1: High Dynamic Range Image Based Lighting with VTK](https://blog.kitware.com/pbrj1/)
- [PBR Journey Part 2 : Anisotropy model with VTK](https://blog.kitware.com/pbr-journey-part-2-anisotropy-model-with-vtk/)
- [PBR Journey Part 3 : Clear Coat Model with VTK](https://blog.kitware.com/pbr-journey-part-3-clear-coat-model-with-vtk/)
- [Object Shading Properties](https://gitlab.kitware.com/paraview/paraview-docs/-/blob/master/doc/source/ReferenceManual/objectShadingProperties.rst)

!!! note
    - `<DATA>/PBR_Skybox_Texturing.json` assumes that the skyboxes and textures are in the subfolders `Skyboxes` and `Textures` relative to this file. This allows you to copy this JSON file and the associated subfolders to any other location on your computer.
    - You can turn off the skybox in the JSON file by setting `"skybox":false`. Image based lighting will still be active.

!!! note
    - The C++ example requires C++17 as `std::filesystem` is used. If your compiler does not support C++17 comment out the filesystem stuff.
