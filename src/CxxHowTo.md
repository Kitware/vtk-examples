# C++ How To

Often you just need a "pointer" to an example that shows you how to do something.

Here are some snippets and examples that highlight interesting features that may help you:

## Callback

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[CallBack](/Cxx/Interaction/CallBack) | Setting up a callback with client data. Two different methods are demonstrated.

## Camera

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[CameraModel1](/Cxx/Visualization/CameraModel1) | Illustrate camera movement around the focal point.
[CameraModel2](/Cxx/Visualization/CameraModel2) | camera movement centered at the camera position.
[CameraOrientationWidget](/Cxx/Widgets/CameraOrientationWidget) | This 3D camera orientation widget can be used in conjunction with [CameraPosition](/Cxx/Snippets/CameraPosition) to get a nice camera position.
[CameraPosition](/Cxx/Snippets/CameraPosition) | Get the camera position while moving the image.
[MergeSelections](/Cxx/PolyData/MergeSelections) | Use the same camera for all renderers.
[ResetCameraOrientation](/Cxx/Utilities/ResetCameraOrientation) | Reset camera orientation to a previously saved orientation.
[ShareCamera](/Cxx/Utilities/ShareCamera) | Use the same camera for all renderers.

## Check the VTK Version

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[CheckVTKVersion-Snippet](/Cxx/Snippets/CheckVTKVersion) | Check the VTK version returning `true` if the requested VTK version is >= the current version.
[CheckVTKVersion-Example](/Cxx/Utilities/CheckVTKVersion) | Check the VTK version and provide alternatives for different VTK versions.

## Coloring

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[BackgroundGradient](/Cxx/Visualization/BackgroundGradient) | Background gradient.
[KochanekSplineDemo](/Cxx/PolyData/KochanekSplineDemo) | How to color the slider components.
[MergeSelections](/Cxx/PolyData/MergeSelections) | A nifty way to get a "harmonious" set of window colors, using vtkNamedColors and vtkColorSeries.
[ShareCamera](/Cxx/Utilities/ShareCamera) | Store background colors in a vector for later extraction of the red, green and blue components.

## Glyphing

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[PointToGlyph](/Cxx/Snippets/PointToGlyph) | How to represent points as glyphs.
[QuantizePolyDataPoints](/Cxx/PolyData/QuantizePolyDataPoints) | Represent a point by a glyph.

## Image

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[WriteImage](/Cxx/Snippets/WriteImage) | Write out an image of various types.

## Multiple view ports and render windows

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[MultipleRenderWindows](/Cxx/Visualization/MultipleRenderWindows) | Multiple Render Windows.
[MultipleViewports](/Cxx/Visualization/MultipleViewports) | Multiple Viewports.

## Physically Based Rendering

Physically based rendering sets metallicity, roughness, occlusion strength and normal scaling of the object. Textures are used to set base color, ORM, anisotropy and normals. Textures for the image based lighting and the skymap are supplied from a cubemap.

Image based lighting uses a cubemap texture to specify the environment. A Skybox is used to create the illusion of distant three-dimensional surroundings.

The results can be quite spectacular, it is hoped that these examples will help you to get started.

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[PBR_Anisotropy](/Cxx/Rendering/PBR_Anisotropy) | Render spheres with different anisotropy values.
[PBR_Clear_Coat](/Cxx/Rendering/PBR_Clear_Coat) | Render a cube with custom texture mapping and a coat normal texture.
[PBR_Edge_Tint](/Cxx/Rendering/PBR_Edge_Tint) | Render spheres with different edge colors using a skybox as image based lighting.
[PBR_HDR_Environment](/Cxx/Rendering/PBR_HDR_Environment) | Renders spheres with different materials using a skybox as image based lighting.
[PBR_Mapping](/Cxx/Rendering/PBR_Mapping) | Render a cube with custom texture mapping.
[PBR_Materials](/Cxx/Rendering/PBR_Materials) | Renders spheres with different materials using a skybox as image based lighting.
[PBR_Materials_Coat](/Cxx/Rendering/PBR_Materials_Coat) | Render spheres with different coat materials using a skybox as image based lighting.
[PBR_Skybox](/Cxx/Rendering/PBR_Skybox) | Demonstrates physically based rendering, a skybox and image based lighting.
[PBR_Skybox_Texturing](/Cxx/Rendering/PBR_Skybox_Texturing) | Demonstrates physically based rendering, a skybox, image based lighting and texturing.
[PBR_Skybox_Anisotropy](/Cxx/Rendering/PBR_Skybox_Anisotropy) | Demonstrates physically based rendering, a skybox, image based lighting, and anisotropic texturing.

## Polydata

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[ReadPolyData](/Cxx/Snippets/ReadPolyData) | This snippet works for most PolyData.

## Random

If you want to ensure that the same random points/colors are used in C++ and other languages then it is best to use vtkMinimalStandardRandomSequence.

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[ColorDisconnectedRegionsDemo](/Cxx/PolyData/ColorDisconnectedRegionsDemo) | A vtkLookupTable is filled with random colors.
[HighlightWithSilhouette](/Cxx/Picking/HighlightWithSilhouette) | Here we use randomly positioned spheres with random colors. A vtkLookupTable is filled with random colors.

## Render Windows

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[Model](/Cxx/Rendering/Model) | Multiple render windows.

## StructuredDataset

How to visualise the information in a structured dataset. All these examples use the combustor dataset.

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[ColorIsosurface](/Cxx/VisualizationAlgorithms/ColorIsosurface) | Color an isosurface with a data array.
[CombustorIsosurface](/Cxx/VisualizationAlgorithms/CombustorIsosurface) | Generate an isosurface of constant flow density.
[CutStructuredGrid](/Cxx/VisualizationAlgorithms/CutStructuredGrid) | Cut through structured grid with plane. The cut plane is shown solid shaded. A computational plane of constant k value is shown in wireframe for comparison. The colors correspond to flow density. Cutting surfaces are not necessarily planes: implicit functions such as spheres, cylinders, and quadrics can also be used.
[ProbeCombustor](/Cxx/VisualizationAlgorithms/ProbeCombustor) | Probing data in a combustor.  Probes are regular arrays of 50 by 50 points that are then passed through a contouring filter.
[PseudoVolumeRendering](/Cxx/VolumeRendering/PseudoVolumeRendering) | Here we use 100 cut planes, each with an opacity of 0.05. They are then rendered back-to-front to simulate volume rendering.
[Rainbow](/Cxx/Rendering/Rainbow) | Using different vtkLookupTables.
[StreamLines](/Cxx/Visualization/StreamLines) | Seed streamlines with vectors from a structured grid.
[StreamlinesWithLineWidget](/Cxx/VisualizationAlgorithms/StreamlinesWithLineWidget) | Interact with the streamlines in the combustor dataset.
[VelocityProfile](/Cxx/VisualizationAlgorithms/VelocityProfile) | Warping the geometry of three planes to show flow momentum.
[WarpCombustor](/Cxx/VisualizationAlgorithms/WarpCombustor) | Carpet plots of combustor flow energy in a structured grid. Colors and plane displacement represent energy values.
[XYPlot](/Cxx/Annotation/XYPlot) | Display line probes.
