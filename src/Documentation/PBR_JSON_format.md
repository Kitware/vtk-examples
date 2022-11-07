# PBR JSON file format

The following keys and values are recognised when parsing the JSON files used by the PBR examples:

## Keys

### General

- **title**: A title, it could indicate the program using this file.
- **object**: The object name.
- **objcolor**: The colour of the object.
- **bkgcolor**: The background colour of the scene.
- **skybox**: Use the cubemap or equirectangular files as a skybox.

### Environment textures

- **cubemap**: The vector of six paths to the cube maps used to generate the environment texture.
- **equirectangular**: An equirectangular file used to generate the environment texture.

You must specify at least one of these keys. If both are specified then **equirectangular** will be used by default.

### Object Textures

- **albedo**: A file setting the perceived color (base colour texture) of the object, the diffuse color for non-metallic objects or the specular color for metallic objects.
- **normal**: A file setting the normal map (texture). This allows a more precise shading without the need to subdivide the geometry. Stores the normal direction {x,y,z} in the Red/Green/Blue (RGB) channels.
- **material**: A file setting the ambient Occlusion/Roughness/Metallic factors in the RGB channels. Also called ORM texture.
- **coat**: A file containing the Coat Normal Texture, a normal map for the clear coat layer.
- **anisotropy**: A file setting the Anisotropy Texture controlling the anisotropy strength in the red channel, while the green channel holds the anisotropy rotation. The blue channel is discarded.
- **emissive**: A file setting the light-emitting texture or Emissive Texture. Note that the material will not actually emit light, and the texture is ignored by OSPRay and NVidia pathtracing backends.

*Note*: Generally only a subset of the available keys are used in any given application.

## Values

The values consist of strings:

- For the key **object**: The name of the object to be used is specified e.g. **"Boy"**.
- For the key **skybox**: **true** or **false** are the allowed values.
- For the keys **objcolor** or **bkgcolor**: the name of a colour is the value e.g. **"MidnightBlue"**. See: [VTKNamedColorPatches](https://htmlpreview.github.io/?https://github.com/Kitware/vtk-examples/blob/gh-pages/VTKNamedColorPatches.html) for colour names.

The remaining keys have values specifying paths to the needed files. These paths are relative to where the JSON file's location is in the filesystem.

The value for **cubemap** is a vector of six paths with this orientation and ordering:

- [right, left, top, bottom, front, back]

or

- [+x, -x, +y, -y, +z, -z]

The cubemap key has two subkeys:

- **root**: Specifying the path to the cubemaps.
- **files**: The ordered list of six files corresponding to the cubemaps.

## Example

``` json
{
  "title": "Parameters for PBR_Skybox_Anisotropy",
  "object": "Boy",
  "bkgcolor": "MidnightBlue",
  "objcolor": "Seashell",
  "skybox": true,
  "cubemap": {
    "root": "./Skyboxes/skybox0",
    "files": [
      "right.jpg",
      "left.jpg",
      "top.jpg",
      "bottom.jpg",
      "front.jpg",
      "back.jpg"
    ]
  },
  "equirectangular": "./Skyboxes/spiaggia_di_mondello_4k.hdr",
  "xequirectangular": "./Skyboxes/skybox0/skybox.jpg",
  "albedo": "./Textures/Anisotropic/CarbonFiberAniso_albedo.png",
  "normal": "./Textures/Anisotropic/CarbonFiberAniso_normal.png",
  "material": "./Textures/Anisotropic/CarbonFiberAniso_orm.png",
  "anisotropy": "./Textures/Anisotropic/CarbonFiberAniso_anisotropyAngle.png"
}
```

 Note:

- In the above example, the relative paths assume that the folders `Skyboxes` and `Textures` are subfolders relative to the directory where this file is located.
- the key `xequirectangular` will not be loaded, to load it, change it to `equirectangular` and `skybox.jpg` will be used instead of `spiaggia_di_mondello_4k.hd`.
