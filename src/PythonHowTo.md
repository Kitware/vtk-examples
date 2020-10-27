# Python How To

Often you just need a "pointer" to an example that shows you how to do something.

Here are some snippets and examples that highlight interesting features that may help you:

## Camera

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[CameraModel1](/Python/Visualization/CameraModel1) | vtkCameraActor | Illustrate camera movement around the focal point.
[CameraModel2](/Python/Visualization/CameraModel2) | vtkCameraActor | camera movement centered at the camera position.
[CameraPosition](/Python/Snippets/CameraPosition) | Get the camera position while moving the image.
[ShareCamera](/Python/Utilities/ShareCamera) | Use the same camera for all renderers.

## Coloring

| Example Name | Comments | Image |
| -------------- | ---------------------- | ------- |
[ShareCamera](/Python/Utilities/ShareCamera) | Store background colors in a vector for later extraction of the red, green and blue components.

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
