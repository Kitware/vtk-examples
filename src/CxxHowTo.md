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
[CameraPosition](/Cxx/Snippets/CameraPosition) | Get the camera position while moving the image.
[MergeSelections](/Cxx/PolyData/MergeSelections) | Use the same camera for all renderers.
[ShareCamera](/Cxx/Utilities/ShareCamera) | Use the same camera for all renderers.

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
