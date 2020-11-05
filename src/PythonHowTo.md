# Python How To

Often you just need a "pointer" to an example that shows you how to do something.

Here are some snippets and examples that highlight interesting features that may help you:

## Callback

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[CallBack](/Python/Interaction/CallBack) | Setting up a callback with client data. Two different methods are demonstrated.

## Camera

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[CameraModel1](/Python/Visualization/CameraModel1) | Illustrate camera movement around the focal point.
[CameraModel2](/Python/Visualization/CameraModel2) | camera movement centered at the camera position.
[CameraPosition](/Python/Snippets/CameraPosition) | Get the camera position while moving the image.
[ShareCamera](/Python/Utilities/ShareCamera) | Use the same camera for all renderers.

## Coloring

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[ShareCamera](/Python/Utilities/ShareCamera) | Store background colors in a vector for later extraction of the red, green and blue components.

## Glyphing

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[InteractorStyleTrackballCamera](/Python/Interaction/InteractorStyleTrackballCamera) | Here points are represented as glyphs.
[PointToGlyph](/Python/Snippets/PointToGlyph) | How to represent points as glyphs.

## Image

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[WriteImage](/Python/Snippets/WriteImage) | Write out an image of various types.

## Polydata

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[ReadPolyData](/Python/Snippets/ReadPolyData) | This snippet works for most PolyData.

## Random

If you want to ensure that the same random points/colors are used in C++ and other languages then it is best to use vtkMinimalStandardRandomSequence.

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[HighlightWithSilhouette](/Python/Picking/HighlightWithSilhouette) | Here we use randomly positioned spheres with random colors. A vtkLookupTable is filled with random colors.

## Render Windows

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[Model](/Python/Rendering/Model) | Multiple render windows.

## StructuredDataset

How to visualise the information in a structured dataset. All these examples use the combustor dataset.

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[CombustorIsosurface](/Python/VisualizationAlgorithms/CombustorIsosurface) | Generate an isosurface of constant flow density.
[CutStructuredGrid](/Python/VisualizationAlgorithms/CutStructuredGrid) | Cut through structured grid with plane. The cut plane is shown solid shaded. A computational plane of constant k value is shown in wireframe for comparison. The colors correspond to flow density. Cutting surfaces are not necessarily planes: implicit functions such as spheres, cylinders, and quadrics can also be used.
[ProbeCombustor](/Python/VisualizationAlgorithms/ProbeCombustor) | Probing data in a combustor.  Probes are regular arrays of 50 by 50 points that are then passed through a contouring filter.
[PseudoVolumeRendering](/Python/VolumeRendering/PseudoVolumeRendering)  | Here we use 100 cut planes, each with an opacity of 0.05. They are then rendered back-to-front to simulate volume rendering.
[Rainbow](/Python/Rendering/Rainbow) | Using different vtkLookupTables.
[StreamLines](/Python/Visualization/StreamLines) | Seed streamlines with vectors from a structured grid.
[StreamlinesWithLineWidget](/Python/VisualizationAlgorithms/StreamlinesWithLineWidget) | Interact with the streamlines in the combustor dataset.
[VelocityProfile](/Python/VisualizationAlgorithms/VelocityProfile) | Warping the geometry of three planes to show flow momentum.
[WarpCombustor](/Python/VisualizationAlgorithms/WarpCombustor) | Carpet plots of combustor flow energy in a structured grid. Colors and plane displacement represent energy values.
