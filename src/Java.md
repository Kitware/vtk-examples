# Java Examples

Click [here](http://www.vtk.org/Wiki/VTK/Tutorials/JavaEnvironmentSetup) for a tutorial on setting up your Java environment.

It would be appreciated if there are any Java VTK experts who could convert any of the [c++ examples](/Cxx) to Java!

## VTK Classes Summary

- [VTK Classes with Examples](/Coverage/JavaVTKClassesUsed.md), this table is really useful when searching for example(s) using a particular class.

- [VTK Classes with No Examples](/Coverage/JavaVTKClassesNotUsed.md), please add examples in your area of expertise!

## Hello World

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[A hello world example](/Java/GeometricObjects/CylinderExample) | Cylinder example from the VTK Textbook and source code. A hello world example.

## Input and Output

### Graph Formats

### 3D File Formats

| Example Name | Description | Image |
| -------------- | ------------- | -------- |
[ConvertFile](/Java/IO/ConvertFile) | Convert a file from one type to another

#### Standard Formats

##### Input

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[DEMReader](/Java/IO/DEMReader) | Read DEM (elevation map) files
[ParticleReader](/Java/IO/ParticleReader) | This example reads ASCII files where each line consists of points with its position (x,y,z) and (optionally) one scalar or binary files in RAW 3d file format.
[ReadCML](/Java/IO/ReadCML) | Read Chemistry Markup Language files.
[ReadOBJ](/Java/IO/ReadOBJ) | Read an OBJ (.obj) file.
[ReadPLOT3D](/Java/IO/ReadPLOT3D) | Read PLOT3D data files.
[ReadPLY](/Java/IO/ReadPLY) | Read PLY (.ply) files
[ReadSLC](/Java/IO/ReadSLC) | Read an SLC volume file.
[ReadSTL](/Java/IO/ReadSTL) | Read stereo lithography STL (.stl) files.
[SimplePointsReader](/Java/IO/SimplePointsReader) | Read a simple "xyz" file of points.

##### Importers

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ThreeDSImporter](/Java/IO/ThreeDSImporter) | Import a 3D Studio scene that includes multiple actors.
[VRMLImporter](/Java/IO/VRMLImporter) | Import a VRML 2.0 (WRL) file.

##### Output

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[SimplePointsWriter](/Java/IO/SimplePointsWriter) | Write a simple ".xyz" file
[WritePLY](/Java/IO/WritePLY) | Write PLY (.ply) files.
[WriteSTL](/Java/IO/WriteSTL) | Write stereo lithography STL (.stl) files.

#### VTK Formats

##### Input

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ReadPolyData](/Java/IO/ReadPolyData) | Read VTK XML PolyData files.
[ReadRectilinearGrid](/Java/IO/ReadRectilinearGrid) | Read VTK XML RectilinearGrid files.
[ReadStructuredGrid](/Java/IO/ReadStructuredGrid) | Read VTK XML StructuredGrid files.
[ReadVTP](/Java/IO/ReadVTP) | Read a VTK XML PolyData file.

##### Output

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[WriteVTP](/Java/IO/WriteVTP) | Write a VTK XML PolyData file.
[WriteVTU](/Java/IO/WriteVTU) | Write a .vtu file. VTU is an "Unstructured Grid". This format allows for 3D data to be stored.
[XMLPImageDataWriter](/Java/IO/XMLPImageDataWriter) | Write a .pvti file with 4 .vti pieces.
[XMLPUnstructuredGridWriter](/Java/IO/XMLPUnstructuredGridWriter) | Write a .pvtu with 4 .vtu pieces.
[XMLStructuredGridWriter](/Java/IO/XMLStructuredGridWriter) | Write a .vts file from a vtkStructuredGrid.

##### Legacy VTK Formats

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[StructuredPointsReader](/Java/IO/StructuredPointsReader) | Read a structured points (.vtk) file.

### Image Format

#### Input

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[JPEGReader](/Java/IO/JPEGReader) | Read a JPEG image.
[ReadBMP](/Java/IO/ReadBMP) | Read Windows BMP files.
[ReadDICOM](/Java/IO/ReadDICOM) | Read DICOM images.
[ReadPNM](/Java/IO/ReadPNM) | Read PNM (.pnm) files.
[ReadTIFF](/Java/IO/ReadTIFF) | Read TIFF (.tif) files.

#### Output

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[JPEGWriter](/Java/IO/JPEGWriter) | Write a JPEG file.
[PNGReader](/Java/IO/PNGReader) | Read a PNG image.
[WritePNM](/Java/IO/WritePNM) | Write a PNM (.pnm) file.
[WriteTIFF](/Java/IO/WriteTIFF) | Write a TIFF (.tif) file.

## Annotation

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[MultiLineText](/Java/Annotation/MultiLineText) | MultiLineText
[TextOrigin](/Java/Annotation/TextOrigin) | TextOrigin

## CompositeData

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[CompositePolyDataMapper](/Java/CompositeData/CompositePolyDataMapper) | CompositePolyDataMapper

## Filtering

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Delaunay2D](/Java/Filtering/Delaunay2D) | An example to generate set of points and to triangulate using Delaunay2D
[Glyph2D](/Java/Filtering/Glyph2D) | Example to copy a polydata to everypoint in the input set.
[Glyph3D](/Java/Filtering/Glyph3D) | This example applies an object at every point using vtkGlyph3D
[PerlinNoise](/Java/Filtering/PerlinNoise) | An implicit function that implements Perlin noise.
[TransformPolyData](/Java/Filtering/TransformPolyData) | Transforms points, associated normals and vectors for polygonal dataset
[VertexGlyphFilter](/Java/Filtering/VertexGlyphFilter) | Creates a set of points and adds a vertex at each point.
[WarpTo](/Java/Filtering/WarpTo) | Deform geometry by warping towards a point

## GeometricObjects

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Arrow](/Java/GeometricObjects/Arrow) | Arrow Visualization
[Circle](/Java/GeometricObjects/Circle) | Circle Visualization
[Cone](/Java/GeometricObjects/Cone) | Cone Visualization
[ConvexPointSet](/Java/GeometricObjects/ConvexPointSet) | ConvexPointSet Visualization
[Disk](/Java/GeometricObjects/Disk) | Disk Visualization
[EllipticalCylinder](/Java/GeometricObjects/EllipticalCylinder) | EllipticalCylinder
[Frustum](/Java/GeometricObjects/Frustum) | Frustum Visualization
[Line](/Java/GeometricObjects/Line) | Line Visualization
[LongLine](/Java/GeometricObjects/LongLine) | A visualization of a long line
[OrientedArrow](/Java/GeometricObjects/OrientedArrow) | Oriented Arrow visualization.
[OrientedCylinder](/Java/GeometricObjects/OrientedCylinder) | Oriented Cylinder visualization
[ParametricObjects](/Java/GeometricObjects/ParametricObjects) | ParametricObjects Visualization
[PlanesIntersection](/Java/GeometricObjects/PlanesIntersection) | PlanesIntersection
[Polygon](/Java/GeometricObjects/Polygon) | Polygon Visualization
[Pyramid](/Java/GeometricObjects/Pyramid) | Pyramid Visualization
[Quad](/Java/GeometricObjects/Quad) | Quad Visualization
[RegularPolygonSource](/Java/GeometricObjects/RegularPolygonSource) | RegularPolygon Visualization
[ShrinkCube](/Java/GeometricObjects/ShrinkCube) | Applying shrink filter to an object.
[TextActor](/Java/GeometricObjects/TextActor) | TextActor Demonstration
[Triangle](/Java/GeometricObjects/Triangle) | Triangle Visualization
[TriangleStrip](/Java/GeometricObjects/TriangleStrip) | Triangle Strip Visualization
[Vertex](/Java/GeometricObjects/Vertex) | Vertex Visualization

## HyperTreeGrid

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[HyperTreeGridSource](/Java/HyperTreeGrid/HyperTreeGridSource) | Demonstration of a hyper-tree grid source.

## Geographic Visualization (Geovis)

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[CompassWidget](/Java/Geovis/CompassWidget) | CompassWidget Visualization
[EarthSource](/Java/Geovis/EarthSource) | Earth Source Visualization
[GeoAssignCoordinates](/Java/Geovis/GeoAssignCoordinates) | Convert latitude and longitude coordinates to world coordinates.

## Graphs

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[GraphPoints](/Java/InfoVis/GraphPoints) | Manually set the coordinates of vertices in a graph.
[RandomGraphSource](/Java/Graphs/RandomGraphSource) | A graph with random edges.
[VisualizeGraph](/Java/Graphs/VisualizeGraph) | An example to construct a graph and visualize it.

## Working with Images

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Actor2D](/Java/Images/Actor2D) | A actor that draws 2D data
[Cast](/Java/Images/Cast) | Image Data type Casting Filter.
[ImageSinusoidSource](/Java/Images/ImageSinusoidSource) | Creates an image with sinusoidal pixel values.
[RTAnalyticSource](/Java/Images/RTAnalyticSource) | Creates an image for regression testing
[StaticImage](/Java/Images/StaticImage) | Displays a 2D image

## Image Processing

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Flip](/Java/Images/Flip) | Flips an image.
[ImageFFT](/Java/Images/ImageFFT) | Compute the Fast Fourier Transform of an image.
[ImageTest](/Java/Imaging/ImageTest) | Use Java file dialog to select an image file to display.
[ShotNoise](/Java/ImageProcessing/ShotNoise) | Create an image using vtkImageCanvasSource2D and write it to a file.

## Implicit Functions and Iso-surfaces

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[IceCream](/Java/VisualizationAlgorithms/IceCream) | How to use boolean combinations of implicit functions to create a model of an ice cream cone.
[ImplicitDataSet](/Java/ImplicitFunctions/ImplicitDataSet) | Implicit functions, implicit data set.
[ImplicitSphere](/Java/ImplicitFunctions/ImplicitSphere) | ImplicitFunctions, Append poly data, tube filter, sphere
[SampleFunction](/Java/ImplicitFunctions/SampleFunction) | Sample an implicit function over a structured point set.

## User Interaction

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Assembly](/Java/Interaction/Assembly) | Combine/group actors into an assembly.
[InteractorStyleTerrain](/Java/Interaction/InteractorStyleTerrain) | Manipulate camera in scene with natural view up.
[PickableOff](/Java/Interaction/PickableOff) | Disallow interaction with a specific actor.
[RubberBandPick](/Java/Interaction/RubberBandPick) | Pick props underneath a rubber band selection rectangle.
[RubberBandZoom](/Java/Interaction/RubberBandZoom) | Zoom in by amount indicated by rubber band box.
[StyleSwitch](/Java/Interaction/StyleSwitch) | Choose between multiple interaction modes.
[TrackballActor](/Java/Interaction/TrackballActor) | Trackball actor mode.
[TrackballCamera](/Java/Interaction/TrackballCamera) | Trackball camera mode.

## Working with Meshes

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AddCell](/Java/Meshes/AddCell) | Add a cell to an existing mesh.
[BoundaryEdges](/Java/Meshes/BoundaryEdges) | Find the edges that are used by only one face.
[DelaunayMesh](/Java/Modelling/DelaunayMesh) | Two-dimensional Delaunay triangulation of a random set of points. Points and edges are shown highlighted with sphere glyphs and tubes.

## Working with 3D Data

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Bottle](/Java/Modelling/Bottle) | Sweeps polygonal data creating "skirt" from free edges and lines, and lines from vertices
[CellCenters](/Java/PolyData/CellCenters) | Compute points at the center of every cell.
[CenterOfMass](/Java/PolyData/CenterOfMass) | Compute the center of mass of the points.
[ProjectSphere](/Java/Visualization/ProjectSphere) | Unroll a sphere or spherical-like model.
[Spring](/Java/Modelling/Spring) | Rotation in combination with linear displacement and radius variation.

### Working with Structured 3D Data

This section includes vtkImageData vtkStructuredGrid and vtkRectilinearGrid.

"ImageData" is not the traditional "flat, 2D image" you are used to. It is a special VTK data structure in the collection of 3D data structures provided by VTK. [Here is an overview of these data structures](http://www.vtk.org/Wiki/VTK/Tutorials/3DDataTypes). Image data can represent at typical 2D image, but also, a 3D volume.

#### vtkImageData

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ImageNormalize](/Java/ImageData/ImageNormalize) | Normalizes the scalar components for each point.
[ImageWeightedSum](/Java/ImageData/ImageWeightedSum) | Add two or more images.

#### vtkStructuredPoints

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[StructuredPointsToUnstructuredGrid](/Java/StructuredPoints/StructuredPointsToUnstructuredGrid) | Convert a vtkStructuredPoints to a vtkUnstructuredGrid.
[Vol](/Java/StructuredPoints/Vol) | Creating a image data dataset. Scalar data is generated from the equation for a sphere. Volume dimensions are 26 x 26 x 26.

#### vtkStructuredGrid

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[BlankPoint](/Java/StructuredGrid/BlankPoint) | Blank a point of a vtkStructuredGrid.
[StructuredGridOutline](/Java/StructuredGrid/StructuredGridOutline) | Creates a wireframe outline for structured grid.

This section includes vtkImageData, vtkStructuredGrid and vtkRectilinearGrid.

"ImageData" is not the traditional "flat, 2D image" you are used to. It is a special VTK data structure in the collection of 3D data structures provided by VTK. [Here is an overview of these data structures](http://www.vtk.org/Wiki/VTK/Tutorials/3DDataTypes). Image data can represent at typical 2D image, but also, a 3D volume.

#### vtkRectilinearGrid

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[RectilinearGrid](/Java/RectilinearGrid/RectilinearGrid) | A dataset that is topologically regular with variable spacing in the three coordinate directions
[RectilinearGridToTetrahedra](/Java/RectilinearGrid/RectilinearGridToTetrahedra) | Convert a vtkRectilinearGrid to a vtkUnstructuredGrid mesh.
[VisualizeRectilinearGrid](/Java/RectilinearGrid/VisualizeRectilinearGrid) | Visualize the cells of a rectilinear grid.

### Point Cloud Operations

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[FitImplicitFunction](/Java/Points/FitImplicitFunction) | Extract points within a distance to an implicit function.

## Data Structures

### KD-Tree

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[VisualizeKDTree](/Java/DataStructures/VisualizeKDTree) | Show the levels of a KD Tree.

### Octree

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[BuildOctree](/Java/DataStructures/BuildOctree) | Create an octree.

## Rendering

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AmbientSpheres](/Java/Rendering/AmbientSpheres) | Demonstrates the effect of ambient lighting on spheres.
[ColoredSphere](/Java/Rendering/ColoredSphere) | A simple sphere.
[Cone6](/Java/Rendering/Cone6) | This example introduces 3D widgets. 3D widgets take advantage of the event/observer design pattern. Corresponds to the missing Step 6 Java example from VTK/Examples/Tutorial.
[Mace](/Java/Rendering/Mace) | An example of multiple inputs and outputs.
[SpecularSpheres](/Java/Rendering/SpecularSpheres) | Demonstrates the effect of specular lighting on spheres.

## Lighting

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[LightActor](/Java/Lighting/LightActor) | vtkLightActor Demonstration
[SpotLight](/Java/Lighting/SpotLight) | vtkSpotLight Demonstration

## Math Operations

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[MatrixInverse](/Java/Math/MatrixInverse) | Represents and manipulates  3x3 transformation matrices.
[MatrixTranspose](/Java/Math/MatrixTranspose) | The transpose of a matrix is a new matrix whose rows are the columns of the original.
[NormalizeVector](/Java/Math/NormalizeVector) | Normalize a vector.

## Parallel

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ExodusIIWriter](/Java/Parallel/ExodusIIWriter) | Example Program to demonstrates how to use vtkExodusIIWriter

## PolyData

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ColorCells](/Java/PolyData/ColorCells) | ColorCells
[ColorCellsWithRGB](/Java/PolyData/ColorCellsWithRGB) | ColorCellsWithRGB
[ColorDisconnectedRegions](/Java/PolyData/ColorDisconnectedRegions) | ColorDisconnectedRegions
[ColoredPoints](/Java/PolyData/ColoredPoints) | Example to visualize Colored Points.
[ConvexHullShrinkWrap](/Java/PolyData/ConvexHullShrinkWrap) | ConvexHullShrinkWrap
[KochanekSpline](/Java/PolyData/KochanekSpline) | KochanekSpline
[MeshQuality](/Java/PolyData/MeshQuality) | MeshQuality
[Outline](/Java/PolyData/Outline) | Outline
[Reflection](/Java/PolyData/Reflection) | Reflection
[RibbonFilter](/Java/PolyData/RibbonFilter) | RibbonFilter
[RotationAroundLine](/Java/PolyData/RotationAroundLine) | Rotation around a line.
[RuledSurfaceFilter](/Java/PolyData/RuledSurfaceFilter) | Create a surface between lines.
[Stripper](/Java/PolyData/Stripper) | Stripper
[ThinPlateSplineTransform](/Java/PolyData/ThinPlateSplineTransform) | A nonlinear warp transformation.
[TransformFilter](/Java/PolyData/TransformFilter) | Transforms points, associated normals and vectors
[TransformPipeline](/Java/PolyData/TransformPipeline) | Combining the transformation, in order to manipulate a group of vtkActor. Shown in the example robot arm motion.
[TriangleColoredPoints](/Java/PolyData/TriangleColoredPoints) | Adding color to each vertex of a triangle
[TubeFilter](/Java/PolyData/TubeFilter) | TubeFilter

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AVI](/Java/Untested/Video/AVI) | An example to demonstrate how to make an AVI video using vtkAVIWriter

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[RenderView](/Java/Views/RenderView) | An easier way to make objects appear on the screen using RenderView

## Texture Mapping

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[TexturePlane](/Java/Texture/TexturePlane) | Example of texture mapping.
[TexturedSphere](/Java/Texture/TexturedSphere) | Texture a sphere.

## Utilities

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[BrownianPoints](/Java/Utilities/BrownianPoints) | Produce a random vector at each point in a dataset.
[ColorLookupTable](/Java/Utilities/ColorLookupTable) | Color Lookup Table.
[DiscretizableColorTransferFunction](/Java/Utilities/DiscretizableColorTransferFunction) | Discretizable Color Transfer Function.
[FullScreen](/Java/Utilities/FullScreen) | Example to Display Actors in a Full Screen Render Window.
[FunctionParser](/Java/Utilities/FunctionParser) | String function parser.
[GetDataRoot](/Java/Utilities/GetDataRoot) | Find the path of VTK_DATA_ROOT.
[PassThrough](/Java/InfoVis/PassThrough) | Shallow copies the input into the output.
[PiecewiseFunction](/Java/Utilities/PiecewiseFunction) | Interpolation using a piecewise function.
[ReportRenderWindowCapabilities](/Java/Utilities/ReportRenderWindowCapabilities) | Report the capabilities of a render window.
[Screenshot](/Java/Utilities/Screenshot) | Use a vtkWindow as input to image pipeline.
[TimerLog](/Java/Utilities/TimerLog) | Example to Demonstrate Timer support and logging.

### Arrays

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[KnownLengthArray](/Java/Utilities/KnownLengthArray) | Known Length Array
[UnknownLengthArray](/Java/Utilities/UnknownLengthArray) | UnKnown Length Array

## Video

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[OggTheora](/Java/Video/OggTheora) | Uses the ogg and theora libraries to write video files.

## Visualization

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AnimDataCone](/Java/Visualization/AnimDataCone) | Animate a cone by modifying the polydata points - uses Interaction M key (Motion) to toggle animation.
[Animation](/Java/Visualization/Animation) | Move a sphere across a scene
[AnnotatedCubeActor](/Java/Visualization/AnnotatedCubeActor) | A 3D cube with face labels.
[BackfaceCulling](/Java/Visualization/BackfaceCulling) | Turn on/off fast culling of polygons based on orientation of normal with respect to camera.
[BackgroundColor](/Java/Visualization/BackgroundColor) | Example program to set the background color of the render window
[BackgroundGradient](/Java/Visualization/BackgroundGradient) | viewport having gradient background using the Background (bottom) and Background2 (top) colors.
[BlobbyLogo](/Java/Visualization/BlobbyLogo) | Blobby logo from VTK textbook.
[Camera](/Java/Visualization/Camera) | A virtual camera for 3D rendering.
[CameraActor](/Java/Visualization/CameraActor) | A frustum to represent a camera.
[ColorActorEdges](/Java/Visualization/ColorActorEdges) | Color the edges of an Actor.
[ColorAnActor](/Java/Visualization/ColorAnActor) | Switch style interaction adds C and A key actions for selecting Camera or Actor interaction, and T and J key actions for Trackball or Joystick interaction mode. Adds charEvent observer callback to restore the missing E and Q events to quit.
[ComplexV](/Java/Visualization/ComplexV) | ComplexV from the VTK Textbook.
[CornerAnnotation](/Java/Visualization/CornerAnnotation) | Write text in the corners of a window.
[CubeAxesActor](/Java/Visualization/CubeAxesActor) | Display three orthogonal axes with labels.
[CubeAxesActor2D](/Java/Visualization/CubeAxesActor2D) | This example uses the vtkCubeAxesActor2D to show your scene with axes to indicate the spatial extent of your data.
[Cursor2D](/Java/Visualization/Cursor2D) | Generates a 2D Cursor Representation.
[Cursor3D](/Java/Visualization/Cursor3D) | Generates a 3D Cursor Representation
[CursorShape](/Java/Visualization/CursorShape) | Change the shape of the cursor.
[Cutter](/Java/VisualizationAlgorithms/Cutter) | How to use vtkCutter by cutting through a cube.
[DisplayCoordinateAxes](/Java/Visualization/DisplayCoordinateAxes) | Display coordinate axes.
[DistanceToCamera](/Java/Visualization/DistanceToCamera) | Calculate distance from points to the camera.
[DrawText](/Java/Visualization/DrawText) | Display Text.
[Follower](/Java/Visualization/Follower) | Draw text that stays right side up.
[ImageOrientation](/Java/Visualization/ImageOrientation) | Orientation of the view of an image.
[Legend](/Java/Visualization/Legend) | Draw symbols with text.
[LegendScaleActor](/Java/Visualization/LegendScaleActor) | Display the scale of a scene.
[LineWidth](/Java/Visualization/LineWidth) | Change the width/thickness of lines in an actor.
[LoopShrink](/Java/Visualization/LoopShrink) | A network with a loop.
[MoveActor](/Java/Visualization/MoveActor) | Moving an Actor.
[MoveCamera](/Java/Visualization/MoveCamera) | Moving the Camera.
[MultipleActors](/Java/Visualization/MultipleActors) | Multiple Actors.
[MultipleViewports](/Java/Visualization/MultipleViewports) | Multiple Viewports.
[NoShading](/Java/Visualization/NoShading) |
[Opacity](/Java/Visualization/Opacity) | Transparency, transparent.
[OrientedGlyphs](/Java/Visualization/OrientedGlyphs) | Copies oriented and scaled glyph geometry to every input point
[ProteinRibbons](/Java/Visualization/ProteinRibbons) | Display pdb ribbons.
[RandomProbe](/Java/Visualization/RandomProbe) | Demonstrates how to probe a dataset with random points and select points inside the data set.
[ScalarBarActor](/Java/Visualization/ScalarBarActor) | Display a color bar.
[ScalarBarActorColorSeries](/Java/Visualization/ScalarBarActorColorSeries) | Display a color bar with a color series lookup table.
[ScalarVisibility](/Java/Visualization/ScalarVisibility) | Switch between coloring the actor and coloring the data.
[SideBySideViewports](/Java/Visualization/SideBySideViewports) | Side by side viewports.
[SpikeFran](/Java/VisualizationAlgorithms/SpikeFran) | Glyphs indicate surface normals on model of human face. Glyph positions are randomly selected.
[VectorFieldExample](/Java/Visualization/VectorFieldExample) | A vector field visualisation.
[VisualizeImageData](/Java/Visualization/VisualizeImageData) | Visualize the points of an ImageData.
[VisualizeVTP](/Java/Visualization/VisualizeVTP) | Visualize a VTP File.
[WindowSize](/Java/Visualization/WindowSize) | Change the size of a window.
[WireframeSphere](/Java/Visualization/WireframeSphere) | A Wireframe Sphere Visualization.

## Volume Rendering

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[SimpleRayCast](/Java/VolumeRendering/SimpleRayCast) | Volume rendering of a high potential iron protein.

## Graphs

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[SelectGraphVertices](/Java/Graphs/SelectGraphVertices) | Select Edges and vertices in a graph

## SimpleOperations

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[DistanceBetweenPoints](/Java/SimpleOperations/DistanceBetweenPoints) | Example Program to find squared distance and the Euclidean distance between two 3D points.
[PerspectiveTransform](/Java/SimpleOperations/PerspectiveTransform) | It is used to describe the full range of homogeneous transformations. It was designed in particular to describe a camera-view of a scene
[ProjectPointPlane](/Java/SimpleOperations/ProjectPointPlane) | Performs various plane computations.
[RandomSequence](/Java/SimpleOperations/RandomSequence) | Park and Miller Sequence of pseudo random numbers.

## Swing Integration

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[JFrameRender](/Java/SwingIntegration/JFrameRenderer) | Render a scene in a JFrame using a vtkRenderWindowPanel
[SwingHandleMouseEvent](/Java/SwingIntegration/SwingHandleMouseEvent) | Display in a JLabel the point the mouse is hovering over using a mouse listener

## Widgets

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AngleWidget](/Java/Widgets/AngleWidget) | AngleWidget Demonstration
[AngleWidget2D](/Java/Widgets/AngleWidget2D) | AngleWidget2D Demonstration
[BalloonWidget](/Java/Widgets/BalloonWidget) | The balloon text describes each object when you hover it.
[CaptionWidget](/Java/Widgets/CaptionWidget) | Widget for placing a caption (text plus leader)
[DistanceWidget](/Java/Widgets/DistanceWidget) | Measures the distance between two points.
[ImagePlaneWidget](/Java/Widgets/ImagePlaneWidget) | 3D widget for reslicing image data.
[ImageTracerWidgetNonPlanar](/Java/Widgets/ImageTracerWidgetNonPlanar) | 3D widget for tracing on planar props.
[LogoWidget](/Java/Widgets/LogoWidget) | 2D widget for placing and manipulating a logo
[OrientationMarkerWidget](/Java/Widgets/OrientationMarkerWidget) | Display a polydata as an orientation icon.
[PlaneWidget](/Java/Widgets/PlaneWidget) | A finite (bounded) plane that can be interactively placed in a scene.
[SeedWidget](/Java/Widgets/SeedWidget) | Places multiple seed points, where a user moves the cursor to and clicks it.
[SplineWidget](/Java/Widgets/SplineWidget) | SplineWidget Demonstration
[TextWidget](/Java/Widgets/TextWidget) | Widget for placing text on overlay plane.
