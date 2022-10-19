# Python Examples

Please see [this page](https://www.vtk.org/Wiki/VTK/PythonDevelopment) to learn how to setup your environment to use VTK in Python.

It would be appreciated if there are any Python VTK experts who could convert any of the [c++ examples](/Cxx) to Python!

## VTK Classes Summary

This Python script, [SelectExamples](../Python/Utilities/SelectExamples), will let you select examples based on a VTK Class and language. It requires Python 3.7 or later. The following tables will produce similar information.

- [VTK Classes with Examples](/Coverage/PythonVTKClassesUsed.md), this table is really useful when searching for example(s) using a particular class.

- [VTK Classes with No Examples](/Coverage/PythonVTKClassesNotUsed.md), please add examples in your area of expertise!

## Tutorials

If you are new to VTK then these [tutorials](#tutorial) will help to get you started.

## Hello World

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[A hello world example](/Python/GeometricObjects/CylinderExample) | Cylinder example from the VTK Textbook and source code. A hello world example.

## Simple Operations

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[DistanceBetweenPoints](/Python/SimpleOperations/DistanceBetweenPoints) |

## Input and Output

### Graph Formats

### 3D File Formats

#### Standard Formats

##### Input

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ReadExodusData](/Python/IO/ReadExodusData) | A simple script for reading and viewing ExodusII data interactively.
[ReadSLC](/Python/IO/ReadSLC) | Read an SLC file.
[ReadSTL](/Python/IO/ReadSTL) | Read an STL file.
[ReadUnstructuredGrid](/Python/IO/ReadUnstructuredGrid) | Using vtkUnstructuredGridReader to read a (legacy) *.vtk file
[ReadVTP](/Python/IO/ReadVTP) | Read a VTK XML PolyData file.

###### Importers

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[3DSImporter](/Python/IO/3DSImporter) | Import a 3D Studio scene that includes multiple actors.

##### Output

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[WritePLY](/Python/IO/WritePLY) |
[WriteSTL](/Python/IO/WriteSTL) |

#### VTK Formats

##### Input

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ReadImageData](/Python/IO/ReadImageData) | Read an image data (.vti) file
[ReadPolyData](/Python/IO/ReadPolyData) | Read a polygonal data (.vtp) file

##### Output

#### Legacy VTK Formats

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ReadLegacyUnstructuredGrid](/Python/IO/ReadLegacyUnstructuredGrid) | Read an unstructured grid that contains 11 linear cells.
[WriteLegacyLinearCells](/Python/IO/WriteLegacyLinearCells) | Write each linear cell into a legacy UnstructuredGrid file.
[WriteXMLLinearCells](/Python/IO/WriteXMLLinearCells) | Write each linear cell into an XML UnstructuredGrid file (.vtu).

### Image Format

#### Input

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[HDRReader](/Python/IO/HDRReader) | Read a high-dynamic-range imaging file.

#### Output

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ImageWriter](/Python/IO/ImageWriter) | Write an imagefile based on the file extension.

## Geometric Objects

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Axes](/Python/GeometricObjects/Axes) |
[Circle](/Python/GeometricObjects/Circle) | A disk is shown however this can be modified to show a circle.
[ColoredLines](/Python/GeometricObjects/ColoredLines) |
[Dodecahedron](/Python/GeometricObjects/Dodecahedron) | Create a dodecahedron using vtkPolyhedron.
[EllipticalCylinder](/Python/GeometricObjects/EllipticalCylinder) | Create an elliptical cylinder using extrusion.
[EllipticalCylinderDemo](/Python/GeometricObjects/EllipticalCylinderDemo) | Show the base and extrusion vector.
[GeometricObjectsDemo](/Python/GeometricObjects/GeometricObjectsDemo) |
[Planes](/Python/GeometricObjects/Planes) | We create a convex hull of the planes for display purposes.
[PlanesIntersection](/Python/GeometricObjects/PlanesIntersection) |
[PolygonIntersection](/Python/GeometricObjects/PolygonIntersection) | IntersectWithLine()
[SourceObjectsDemo](/Python/GeometricObjects/SourceObjectsDemo) | Examples of source objects that procedurally generate polygonal models.  These nine images represent just some of the capability of VTK. From upper left in reading order: sphere, cone, cylinder, cube, plane, text, random point cloud, disk (with or without hole), and line source. Other polygonal source objects are available; check subclasses of vtkPolyDataAlgorithm.

### Cells

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Cell3DDemonstration](/Python/GeometricObjects/Cell3DDemonstration) | Sample objects are generated from the classes derived from vtkCell3D and displayed.
[CellTypeSource](/Python/GeometricObjects/CellTypeSource) | Generate tessellated cells.
[ConvexPointSet](/Python/GeometricObjects/ConvexPointSet) | Generate a ConvexPointSetCell.
[Hexahedron](/Python/GeometricObjects/Hexahedron) |
[LinearCellDemo](/Python/GeometricObjects/LinearCellDemo) | Linear cell types in VTK.
[LongLine](/Python/GeometricObjects/LongLine) | Manually create a polyline.
[Point](/Python/GeometricObjects/Point) |
[PolyLine](/Python/GeometricObjects/PolyLine) |
[Polygon](/Python/GeometricObjects/Polygon) |
[Polyhedron](/Python/GeometricObjects/Polyhedron) | Create an unstructured grid representation of a polyhedron (cube) and write it out to a file.
[Pyramid](/Python/GeometricObjects/Pyramid) |
[Quad](/Python/GeometricObjects/Quad) |
[Tetrahedron](/Python/GeometricObjects/Tetrahedron) |
[Triangle](/Python/GeometricObjects/Triangle) |
[TriangleStrip](/Python/GeometricObjects/TriangleStrip) |
[Vertex](/Python/GeometricObjects/Vertex) |

### Sources

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Arrow](/Python/GeometricObjects/Arrow) |
[Cone](/Python/GeometricObjects/Cone) |
[Cube](/Python/GeometricObjects/Cube) | Manually build a polygonal cube.
[Cube1](/Python/GeometricObjects/Cube1) | A nice simple example that demonstrates the operation of the VTK pipeline.
[Cylinder](/Python/GeometricObjects/Cylinder) |
[Disk](/Python/GeometricObjects/Disk) | A circle with a hole in it.
[EarthSource](/Python/GeometricObjects/EarthSource) | Create the Earth.
[Frustum](/Python/GeometricObjects/Frustum) |
[Line](/Python/GeometricObjects/Line) |
[OrientedArrow](/Python/GeometricObjects/OrientedArrow) | Orient an arrow along an arbitrary vector.
[OrientedCylinder](/Python/GeometricObjects/OrientedCylinder) | Orient a cylinder along an arbitrary vector.
[Plane](/Python/GeometricObjects/Plane) |
[PlatonicSolids](/Python/GeometricObjects/PlatonicSolids) | All five platonic solids are displayed.
[PolyLine1](/Python/GeometricObjects/PolyLine1) | This example demonstrates how to create a polygon through several ordered points.
[RegularPolygonSource](/Python/GeometricObjects/RegularPolygonSource) |
[ShrinkCube](/Python/GeometricObjects/ShrinkCube) | Applying shrink filter to an object.
[Sphere](/Python/GeometricObjects/Sphere) |
[TessellatedBoxSource](/Python/GeometricObjects/TessellatedBoxSource) | Generate a box with tessellated sides.
[TextActor](/Python/GeometricObjects/TextActor) | 2D "HUD-type" text

### Non Linear

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[IsoparametricCellsDemo](/Python/GeometricObjects/IsoparametricCellsDemo) | Nonlinear isoparametric cell types in VTK.
[QuadraticHexahedron](/Python/GeometricObjects/QuadraticHexahedron) | Create and tessellate a nonlinear cell.
[QuadraticHexahedronDemo](/Python/GeometricObjects/QuadraticHexahedronDemo) | Interactively adjust chord error.
[QuadraticTetra](/Python/GeometricObjects/QuadraticTetra) | Create and tessellate a nonlinear cell.
[QuadraticTetraDemo](/Python/GeometricObjects/QuadraticTetraDemo) | Interactively adjust chord error.

### Parametric Objects

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ParametricKuenDemo](/Python/GeometricObjects/ParametricKuenDemo) | Interactively change the parameters for a Kuen Surface.
[ParametricObjectsDemo](/Python/GeometricObjects/ParametricObjectsDemo) | Demonstrates the Parametric classes added by Andrew Maclean and additional classes added by Tim Meehan. The parametric spline is also included. Options are provided to display single objects, add backface, add normals and print out an image.
[ParametricSuperEllipsoidDemo](/Python/GeometricObjects/ParametricSuperEllipsoidDemo) | Interactively change the parameters for a SuperEllipsoid Surface.
[ParametricSuperToroidDemo](/Python/GeometricObjects/ParametricSuperToroidDemo) | Interactively change the parameters for a SuperToroid Surface.

## Implicit Functions and Iso-surfaces

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[BooleanOperationImplicitFunctions](/Python/ImplicitFunctions/BooleanOperationImplicitFunctions) | Demonstrate booleans of two different implicit functions
[CutWithScalars](/Python/VisualizationAlgorithms/CutWithScalars) | Cut a surface with scalars.
[DiscreteMarchingCubes](/Cxx/Python/DiscreteMarchingCubes) | Generate surfaces from labeled data.
[ExtractData](/Python/VisualizationAlgorithms/ExtractData) | Implicit functions used to select data: Two ellipsoids are combined using the union operation used to select voxels from a volume. Voxels are shrunk 50 percent.
[ExtractLargestIsosurface](/Python/Modelling/ExtractLargestIsosurface) | Extract largest isosurface.
[Hello](/Python/VisualizationAlgorithms/Hello) | Implicit modelling used to thicken a stroked font.
[IceCream](/Python/VisualizationAlgorithms/IceCream) | How to use boolean combinations of implicit functions to create a model of an ice cream cone.
[ImplicitQuadric](/Python/ImplicitFunctions/ImplicitQuadric) | Create an ellipsoid using an implicit quadric
[ImplicitSphere](/Python/ImplicitFunctions/ImplicitSphere) | Demonstrate sampling of a sphere implicit function
[ImplicitSphere1](/Python/ImplicitFunctions/ImplicitSphere1) | Demonstrate sampling of a sphere implicit function
[Lorenz](/Python/Visualization/Lorenz) | Visualizing a Lorenz strange attractor by integrating the Lorenz equations in a volume.
[MarchingCases](/Python/VisualizationAlgorithms/MarchingCases) | Explore the Marching Cubes cases.
[MarchingCasesA](/Python/VisualizationAlgorithms/MarchingCasesA) | The 256 possible cases have been reduced to 15 cases using symmetry.
[MarchingCasesB](/Python/VisualizationAlgorithms/MarchingCasesB) | Marching cubes complementary cases. Cases 3c, 6c, 7c, 10c, 12c and 13c are displayed.
[MarchingCasesC](/Python/VisualizationAlgorithms/MarchingCasesC) | Marching cubes, case 3 is rotated 90 degrees about the y-axis with no label.
[MarchingCasesD](/Python/VisualizationAlgorithms/MarchingCasesD) | Marching cubes, case 7 is rotated 180 degrees about the y-axis with no label.
[MarchingCubes](/Python/Modelling/MarchingCubes) | Create a voxelized sphere.
[SampleFunction](/Python/ImplicitFunctions/SampleFunction) | Sample and visualize an implicit function.
[SmoothDiscreteMarchingCubes](/Python/Modelling/SmoothDiscreteMarchingCubes) | Generate smooth surfaces from labeled data.

## Working with 3D Data

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AlignTwoPolyDatas](/Python/PolyData/AlignTwoPolyDatas) | Align two vtkPolyData's.
[AppendFilter](/Python/Filtering/AppendFilter) | Append different types of data,
[BooleanOperationPolyDataFilter](/Python/PolyData/BooleanOperationPolyDataFilter) | This example performs a boolean operation (Intersection, Union, Difference) of two PolyData
[Bottle](/Python/Modelling/Bottle) | Rotationally symmetric objects.
[CappedSphere](/Python/Modelling/CappedSphere) | Rotate an arc to create a capped sphere.
[CellsInsideObject](/Python/PolyData/CellsInsideObject) | Extract cells inside a closed surface.
[ColoredTriangle](/Python/PolyData/ColoredTriangle) | Creates a file TriangleColored.vtp.
[CombinePolyData](/Python/Filtering/CombinePolyData) | This example combines Polydata objects, and displays the result to the screen.
[ConnectivityFilter](/Python/Filtering/ConnectivityFilter) | Color any dataset type based on connectivity.
[CurvaturesAdjustEdges](/Python/PolyData/CurvaturesAdjustEdges) | Get the Gaussian and Mean curvatures of a surface with adjustments for edge effects.
[CurvaturesDemo](/Python/PolyData/CurvaturesDemo) | Demonstrates how to get the Gaussian and Mean curvatures of a surface.
[ExtractPolyLinesFromPolyData](/Python/PolyData/ExtractPolyLinesFromPolyData) | Extract polylines from polydata.
[ExtractSelection](/Python/PolyData/ExtractSelection) |Extract selected points.
[ExtractSelectionCells](/Python/PolyData/ExtractSelectionCells) | Extract cell, select cell.
[FilledPolygon](/Python/PolyData/FilledPolygon) |
[Finance](/Python/Modelling/Finance) | Visualization of multidimensional financial data. The gray/wireframe surface represents the total data population. The red surface represents data points delinquent on loan payment.
[FinanceFieldData](/Python/Modelling/FinanceFieldData) | Visualization of multidimensional financial data. The yellow surface represents the total data population. The red surface represents data points delinquent on loan payment.
[Glyph2D](/Python/Filtering/Glyph2D) |
[Glyph3D](/Python/Filtering/Glyph3D) |
[ImplicitPolyDataDistance](/Python/PolyData/ImplicitPolyDataDistance) |
[IterateOverLines](/Python/PolyData/IterateOverLines) | Iterate through the lines of a PolyData.
[IterativeClosestPoints](/Python/Filtering/IterativeClosestPoints) |
[LineOnMesh](/Python/DataManipulation/LineOnMesh) |
[MeshLabelImageColor](/Python/DataManipulation/MeshLabelImageColor) |
[Outline](/Python/PolyData/Outline) |
[PerlinNoise](/Python/Filtering/PerlinNoise) |
[PolyDataContourToImageData](/Python/PolyData/PolyDataContourToImageData) |
[PolyDataToImageDataStencil](/Python/PolyData/PolyDataToImageDataStencil) |
[RotationAroundLine](/Python/PolyData/RotationAroundLine) |
[RuledSurfaceFilter](/Python/PolyData/RuledSurfaceFilter) |
[SmoothMeshGrid](/Python/PolyData/SmoothMeshGrid) |
[SolidColoredTriangle](/Python/PolyData/SolidColoredTriangle) | Writes out a file TriangleSolidColor.vtp.
[Spring](/Python/Modelling/Spring) | Rotation in combination with linear displacement and radius variation.
[TransformPolyData](/Python/Filtering/TransformPolyData) | Apply a Transformation to a PolyData.
[TriangleColoredPoints](/Python/PolyData/TriangleColoredPoints) |
[TriangleCornerVertices](/Python/PolyData/TriangleCornerVertices) |
[TriangleCorners](/Python/PolyData/TriangleCorners) |
[TriangulateTerrainMap](/Python/Filtering/TriangulateTerrainMap) | Generate heights (z values) on a 10x10 grid (a terrain map) and then triangulate the points to form a surface.
[TubeFilter](/Python/PolyData/TubeFilter) | Give lines a thickness (produce a cylinder around lines).
[VertexGlyphFilter](/Python/Filtering/VertexGlyphFilter) | Add a vertex to each point.
[WarpTo](/Python/Filtering/WarpTo) | Deform geometry by warping towards a point.
[WarpVector](/Python/PolyData/WarpVector) | This example warps/deflects a line.
[WriteTriangleToFile](/Python/IO/WriteTriangleToFile) |

### Data Types

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[CompositePolyDataMapper](/Python/CompositeData/CompositePolyDataMapper) |
[MultiBlockDataSet](/Python/CompositeData/MultiBlockDataSet) | Demonstrates how to make and use VTK's MultiBlock type data
[OverlappingAMR](/Python/CompositeData/OverlappingAMR) | Demonstrates how to create and populate a VTK's Overlapping AMR Grid type Data

### Data Type Conversions

### Point Cloud Operations

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[PointSource](/Python/PolyData/PointSource) | Generate a random point cloud.

### Working with Meshes

This section includes examples of manipulating meshes.

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[BoundaryEdges](/Python/Meshes/BoundaryEdges) | Find the edges that are used by only one face.
[ClosedSurface](/Python/PolyData/ClosedSurface) | Check if a surface is closed.
[ColoredElevationMap](/Python/Meshes/ColoredElevationMap) | Color a mesh by height.
[Curvatures](/Python/PolyData/Curvatures) | Compute Gaussian, and Mean Curvatures.
[Decimation](/Python/Meshes/Decimation) | Reduce the number of triangles in a mesh.
[DeformPointSet](/Python/Meshes/DeformPointSet) | Use the vtkDeformPointSet filter to deform a vtkSphereSource with arbitrary polydata.
[DelaunayMesh](/Python/Modelling/DelaunayMesh) | Two-dimensional Delaunay triangulation of a random set of points. Points and edges are shown highlighted with sphere glyphs and tubes.
[PointInterpolator](/Python/Meshes/PointInterpolator) | Plot a scalar field of points onto a PolyData surface.

#### Clipping

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[CapClip](/Python/Meshes/CapClip) | Cap a clipped polydata with a polygon.
[ClipDataSetWithPolyData](/Python/Meshes/ClipDataSetWithPolyData) | Clip a vtkRectilinearGrid with arbitrary polydata. In this example, use a vtkConeSource to generate polydata to slice the grid, resulting in an unstructured grid.
[ClipDataSetWithPolyData1](/Python/Meshes/ClipDataSetWithPolyData1) | Clip a vtkRectilinearGrid with arbitrary polydata. In this example, use a vtkConeSource to generate polydata to slice the grid, resulting in an unstructured grid.
[SolidClip](/Python/Meshes/SolidClip) | Create a "solid" clip. The "ghost" of the part clipped away is also shown.

### Working with Structured 3D Data

This section includes vtkImageData, vtkExplicitStructuredGrid, vtkStructuredGrid, vtkStructuredPoints and vtkRectilinearGrid.

"ImageData" is not the traditional "flat, 2D image" you are used to. It is a special VTK data structure in the collection of 3D data structures provided by VTK. Image data can represent at typical 2D image, but also, a 3D volume.

#### vtkImageData

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ImageWeightedSum](/Python/ImageData/ImageWeightedSum) | Add two or more images.

#### vtkExplicitStructuredGrid

| Example Name | Description | Image |
| ------------ | ----------- | ----- |
[CreateESGrid](/Python/ExplicitStructuredGrid/CreateESGrid) | Create an explicit structured grid and convert this to an unstructured grid or vice versa.
[LoadESGrid](/Python/ExplicitStructuredGrid/LoadESGrid) | Load a VTU file and convert the dataset to an explicit structured grid.

#### vtkStructuredGrid

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[SGrid](/Python/StructuredGrid/SGrid) | Creating a structured grid dataset of a semi-cylinder. Vectors are created whose magnitude is proportional to radius and oriented in tangential direction.

#### vtkStructuredPoints

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Vol](/Python/StructuredPoints/Vol) | Creating a image data dataset. Scalar data is generated from the equation for a sphere. Volume dimensions are 26 x 26 x 26.

#### vtkRectilinearGrid

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[RGrid](/Python/RectilinearGrid/RGrid) | Creating a rectilinear grid dataset. The coordinates along each axis are defined using an instance of vtkDataArray.
[RectilinearGrid](/Python/RectilinearGrid/RectilinearGrid) | Create a rectilinear grid.
[VisualizeRectilinearGrid](/Python/RectilinearGrid/VisualizeRectilinearGrid) | Visualize the cells of a rectilinear grid.

### Working with Unstructured 3D Data

This section includes vtkUnstructuredGrid.

#### vtkUnstructuredGrid

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ClipUnstructuredGridWithPlane](/Python/UnstructuredGrid/ClipUnstructuredGridWithPlane) | Clip a UGrid with a plane.
[ClipUnstructuredGridWithPlane2](/Python/UnstructuredGrid/ClipUnstructuredGridWithPlane2) | Clip a UGrid with a plane.
[UGrid](/Python/UnstructuredGrid/UGrid) | Creation of an unstructured grid.

### Registration

### Medical

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[GenerateCubesFromLabels](/Python/Medical/GenerateCubesFromLabels) | Create cubes from labeled volume data.
[GenerateModelsFromLabels](/Python/Medical/GenerateModelsFromLabels) | Create models from labeled volume data.
[MedicalDemo1](/Python/Medical/MedicalDemo1) | Create a skin surface from volume data.
[MedicalDemo2](/Python/Medical/MedicalDemo2) | Create a skin and bone surface from volume data.
[MedicalDemo3](/Python/Medical/MedicalDemo3) | Create skin, bone and slices from volume data.
[MedicalDemo4](/Python/Medical/MedicalDemo4) | Create a volume rendering.
[TissueLens](/Python/Medical/TissueLens) | Cut a volume with a sphere.

### Surface reconstruction

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[GaussianSplat](/Python/Filtering/GaussianSplat) | Create a surface from Unorganized Points (Gaussian Splat).

## Utilities

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[CheckVTKVersion](/Python/Utilities/CheckVTKVersion) | Check the VTK version and provide alternatives for different VTK versions.
[ConstrainedDelaunay2D](/Python/Filtering/ConstrainedDelaunay2D) | Perform a 2D Delaunay triangulation on a point set respecting a specified boundary.
[Delaunay2D](/Python/Filtering/Delaunay2D) |
[LUTUtilities](/Python/Utilities/LUTUtilities) | A utility class for vtkLookupTable allowing you to output the table contents or to compare tables.
[MultipleRenderWindows](/Python/Visualization/MultipleRenderWindows) | Multiple Render Windows.
[MultipleViewports](/Python/Visualization/MultipleViewports) | Multiple Viewports.
[ResetCameraOrientation](/Python/Utilities/ResetCameraOrientation) | Reset camera orientation to a previously saved orientation.
[SaveSceneToFieldData](/Python/Utilities/SaveSceneToFieldData) | Save a vtkCamera's state in a vtkDataSet's vtkFieldData and restore it.
[SaveSceneToFile](/Python/Utilities/SaveSceneToFile) | Save a vtkCamera's state in a file and restore it.
[SelectExamples](/Python/Utilities/SelectExamples) | Given a VTK Class and a language, select the matching examples.
[Screenshot](/Python/Utilities/Screenshot) |
[ShareCamera](/Python/Utilities/ShareCamera) | Share a camera between multiple renderers.
[VTKImportsForPython](/Python/Utilities/VTKImportsForPython) | Generate import statements for the VTK classes in your Python code.
[VTKModulesForCxx](/Python/Utilities/VTKModulesForCxx) | Generate a find_package(VTK COMPONENTS ...) command for CMake.
[VTKWithNumpy](/Python/Utilities/VTKWithNumpy) |
[Variant](/Python/Utilities/Variant) |

### Arrays

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[GetValues](/Python/Arrays/GetValues) |
[RenameArray](/Python/Arrays/RenameArray) |

### Events

## Math Operations

## Graphs

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ColorEdges](/Python/Graphs/ColorEdges) | Color edges.
[ColorVertexLabels](/Python/Graphs/ColorVertexLabels) | Set the color of vertex labels.
[ColorVerticesLookupTable](/Python/Graphs/ColorVerticesLookupTable) | Color vertices.
[ConstructGraph](/Python/Graphs/ConstructGraph) | Construct a graph.
[ConstructTree](/Python/Graphs/ConstructTree) | Construct a tree.
[CreateTree](/Python/Graphs/CreateTree) | Create a tree and label the vertices and edges.
[EdgeWeights](/Python/Graphs/EdgeWeights) |
[GraphToPolyData](/Python/Graphs/GraphToPolyData) | Convert a graph to a PolyData.
[LabelVerticesAndEdges](/Python/Graphs/LabelVerticesAndEdges) | Label vertices and edges.
[NOVCAGraph](/Python/Graphs/NOVCAGraph) |
[RandomGraphSource](/Python/Graphs/RandomGraphSource) | Create a random graph.
[ScaleVertices](/Python/Graphs/ScaleVertices) | Size/scale vertices based on a data array.
[SelectedVerticesAndEdges](/Python/Graphs/SelectedVerticesAndEdges) |
[SideBySideGraphs](/Python/Graphs/SideBySideGraphs) | Display two graphs side by side.
[VisualizeDirectedGraph](/Python/Graphs/VisualizeDirectedGraph) | Visualize a directed graph.
[VisualizeGraph](/Python/Graphs/VisualizeGraph) |

### Graph Conversions

## Data Structures

### Timing Demonstrations

### KD-Tree

### Oriented Bounding Box (OBB) Tree

### Octree

### Modified BSP Tree

### HyperTreeGrid

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[HyperTreeGridSource](/Python/HyperTreeGrid/HyperTreeGridSource) | Create a vtkHyperTreeGrid.

## VTK Concepts

## Rendering

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AmbientSpheres](/Python/Rendering/AmbientSpheres) | Demonstrates the effect of ambient lighting on spheres.
[CameraBlur](/Python/Rendering/CameraBlur) | Example of a scene rendered with focal depth.
[ColoredSphere](/Python/Rendering/ColoredSphere) | A simple sphere.
[Cone3](/Python/Rendering/Cone3) | Four frames of output, based on the VTK example Cone3.py.
[Cone4](/Python/Rendering/Cone4) | Modifying properties and transformation matrix based on the VTK example Cone4.py.
[DiffuseSpheres](/Python/Rendering/DiffuseSpheres) | Demonstrates the effect of diffuse lighting on spheres.
[FlatVersusGouraud](/Python/Rendering/FlatVersusGouraud) | Flat and Gouraud shading. Different shading methods can dramatically improve the look of an object represented with polygons. On the top, flat shading uses a constant surface normal across each polygon. On the bottom, Gouraud shading interpolates normals from polygon vertices to give a smoother look.
[Mace](/Python/Rendering/Mace) | An example of multiple inputs and outputs.
[Model](/Python/Rendering/Model) | Illustrative diagram of graphics objects.
[MotionBlur](/Python/Rendering/MotionBlur) | Example of motion blur.
[OutlineGlowPass](/Python/Rendering/OutlineGlowPass) | Demonstrates how to render a object in a scene with a glowing outline.
[PBR_Skybox](/Python/Rendering/PBR_Skybox) | Demonstrates physically based rendering, a skybox and image based lighting.
[PBR_Skybox_Texturing](/Python/Rendering/PBR_Skybox_Texturing) | Demonstrates physically based rendering, a skybox, image based lighting and texturing.
[PBR_Skybox_Anisotropy](/Python/Rendering/PBR_Skybox_Anisotropy) | Demonstrates physically based rendering, a skybox, image based lighting, and anisotropic texturing.
[Rainbow](/Python/Rendering/Rainbow) | Use and manipulation of vtkLookupTables.
[Rotations](/Python/Rendering/Rotations) | Rotations of a cow about her axes.
[RotationsA](/Python/Rendering/RotationsA) | Perform six rotations of a cow about her x-axis (Figure 3-31a).
[RotationsB](/Python/Rendering/RotationsB) | Perform six rotations of a cow about her y-axis (Figure 3-31b).
[RotationsC](/Python/Rendering/RotationsC) | Perform six rotations of a cow about her z-axis (Figure 3-31c).
[RotationsD](/Python/Rendering/RotationsD) | First a rotation of a cow about her x-axis, then six rotations about her y-axis (Figure 3-31d).
[Shadows](/Python/Rendering/Shadows) | Draw model with shadows.
[SpecularSpheres](/Python/Rendering/SpecularSpheres) | Demonstrates the effect of specular lighting on spheres.
[StippledLine](/Python/Rendering/StippledLine) | Draw a stippled line.
[StripFran](/Python/Rendering/StripFran) | Triangle strip examples. (a) Structured triangle mesh consisting of 134 strips each of 390 triangles. (b) Unstructured triangle mesh consisting of 2227 strips of average length 3.94, longest strip 101 triangles. Images are generated by displaying every other triangle strip.
[TransformSphere](/Python/Rendering/TransformSphere) | The addition of a transform filter to [ColoredSphere](/Python/Rendering/ColoredSphere).
[TransparentBackground](/Python/Rendering/TransparentBackground) | Demonstrates the use of two renderers. Notice that the second (and subsequent) renderers will have a transparent background.
[WalkCow](/Python/Rendering/WalkCow) | This generates Figs. 3-32, 3-33 found in VTKTextbook.pdf.
[WalkCowA](/Python/Rendering/WalkCowA) | This generates Fig. 3-33a found in VTKTextbook.pdf.
[WalkCowB](/Python/Rendering/WalkCowB) | This generates Fig. 3-33b found in VTKTextbook.pdf.

## Lighting

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ShadowsLightsDemo](/Python/Visualization/ShadowsLightsDemo) | Show lights casting shadows.

## Texture Mapping

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AnimateVectors](/Python/Texture/AnimateVectors) | One frame from a vector field animation using texture maps.
[TextureCutQuadric](/Python/Texture/TextureCutQuadric) | Cut a quadric with boolean textures.
[TextureCutSphere](/Python/Texture/TextureCutSphere) | Examples of texture thresholding. (b) Boolean combination of two planes to cut nested spheres.
[TexturePlane](/Python/Texture/TexturePlane) | Example of texture mapping.
[TextureThreshold](/Python/Texture/TextureThreshold) | Demonstrate the use of scalar thresholds to show values of flow density on three planes.

## Tutorial

If you are new to VTK then these tutorials will help to get you started.

| Tutorial | Description | Image |
| -------------- | ------------- | ------- |
[Step 1](/Python/Tutorial/Tutorial_Step1) | Create a cone, render it and rotate it through 360°.
[Step 2](/Python/Tutorial/Tutorial_Step2) | Adding an observer.
[Step 3](/Python/Tutorial/Tutorial_Step3) | Using multiple renderers within a render window.
[Step 4](/Python/Tutorial/Tutorial_Step4) | The creation of multiple actors and the manipulation of their properties and transformations.
[Step 5](/Python/Tutorial/Tutorial_Step5) | Introducing the concept of interaction.
[Step 6](/Python/Tutorial/Tutorial_Step6) | Using a 3D widget.

## Visualization

See [this tutorial](http://www.vtk.org/Wiki/VTK/Tutorials/3DDataTypes) for a brief explanation of the VTK terminology of mappers, actors, etc.

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AlphaFrequency](/Python/Visualization/AlphaFrequency) | Linearly extrude fonts to show letter frequencies in text.
[AnatomicalOrientation](/Python/VisualizationAlgorithms/AnatomicalOrientation) | Show a labelled set of anatomical planes transecting a human figure.
[AnnotatedCubeActor](/Python/Visualization/AnnotatedCubeActor) | Annotated cube.
[AssignCellColorsFromLUT](/Python/Visualization/AssignCellColorsFromLUT) | Demonstrates how to assign colors to cells in a vtkPolyData structure using lookup tables.
[BillboardTextActor3D](/Python/Visualization/BillboardTextActor3D) | Label points with billboards.
[BlobbyLogo](/Python/Visualization/BlobbyLogo) | A logo created with vtkImplicitModeller.
[Blow](/Python/Visualization/Blow) | Ten frames from a blow molding finite element analysis.
[BluntStreamlines](/Python/VisualizationAlgorithms/BluntStreamlines) | Demonstrates airflow around a blunt fin using streamlines.
[Camera](/Python/Visualization/Camera) | Positioning and aiming the camera.
[CameraModel1](/Python/Visualization/CameraModel1) | Illustrate camera movement around the focal point.
[CameraModel2](/Python/Visualization/CameraModel2) | camera movement centered at the camera position.
[CarotidFlow](/Python/VisualizationAlgorithms/CarotidFlow) | Visualizing blood flow in the human carotid arteries. Streamtubes of flow velocity are generated.
[CarotidFlowGlyphs](/Python/VisualizationAlgorithms/CarotidFlowGlyphs) | Visualizing blood flow in human carotid arteries. Cone glyphs indicate flow direction and magnitude.
[ClampGlyphSizes](/Python/Visualization/ClampGlyphSizes) | Use vtkGlyph3D with ClampingOn to limit glyph sizes
[ClipSphereCylinder](/Python/VisualizationAlgorithms/ClipSphereCylinder) | A plane clipped with a sphere and an ellipse. The two transforms place each implicit function into the appropriate position. Two outputs are generated by the clipper.
[CollisionDetection](/Python/Visualization/CollisionDetection) | Collison between two spheres.
[ColorAnActor](/Python/Visualization/ColorAnActor) | Colour the actor.
[ColorSeriesPatches](/Python/Visualization/ColorSeriesPatches) | Creates a HTML file called [VTKColorSeriesPatches](https://htmlpreview.github.io/?https://github.com/Kitware/vtk-examples/blob/gh-pages/VTKColorSeriesPatches.html)
[ColoredAnnotatedCube](/Python/Visualization/ColoredAnnotatedCube) | How to color the individual faces of an annotated cube.
[CombustorIsosurface](/Python/VisualizationAlgorithms/CombustorIsosurface) | Generate an isosurface of constant flow density.
[ComplexV](/Python/Visualization/ComplexV) | ComplexV from the VTK Textbook.
[ContourQuadric](/Python/VisualizationAlgorithms/ContourQuadric) | Contouring a quadric function.
[CreateBFont](/Python/VisualizationAlgorithms/CreateBFont) | A scanned image clipped with a scalar value of 1/2 its maximum intensity produces a mixture of quadrilaterals and triangles.
[CreateColorSeriesDemo](/Python/Visualization/CreateColorSeriesDemo) | Create a custom vtkColorSeries.
[CubeAxesActor](/Python/Visualization/CubeAxesActor) | Display three orthogonal axes with labels.FWorking with 3D
[CurvatureBandsWithGlyphs](/Python/Visualization/CurvatureBandsWithGlyphs) | Demonstrates the coloring of a surface by partitioning the gaussian curvature of a surface into bands and using arrows to display the normals on the surface.
[CutStructuredGrid](/Python/VisualizationAlgorithms/CutStructuredGrid) | Cut through structured grid with plane. The cut plane is shown solid shaded. A computational plane of constant k value is shown in wireframe for comparison. The colors correspond to flow density. Cutting surfaces are not necessarily planes: implicit functions such as spheres, cylinders, and quadrics can also be used.
[Cutter](/Python/VisualizationAlgorithms/Cutter) | How to use vtkCutter by cutting through a cube.
[DataSetSurface](/Python/VisualizationAlgorithms/DataSetSurface) | Cutting a hexahedron with a plane. The red line on the surface shows the cut.
[DecimateFran](/Python/VisualizationAlgorithms/DecimateFran) | Examples of decimation algorithm. (a) Decimation of laser digitizer data.
[DecimateHawaii](/Python/VisualizationAlgorithms/DecimateHawaii) | Examples of decimation algorithm. (b) Decimation of terrain data.
[DisplacementPlot](/Python/VisualizationAlgorithms/DisplacementPlot) | Show modal lines for a vibrating beam.
[DisplayCoordinateAxes](/Python/Visualization/DisplayCoordinateAxes) | Display coordinate axes.
[DisplayQuadricSurfaces](/Python/Visualization/DisplayQuadricSurfaces) | Create and display a quadratic surface.
[ElevationBandsWithGlyphs](/Python/Visualization/ElevationBandsWithGlyphs) | Demonstrates the coloring of a surface by partitioning the elevation into bands and using arrows to display the normals on the surface.
[ExponentialCosine](/Python/VisualizationAlgorithms/ExponentialCosine) | Carpet plots. Visualization of an exponential cosine function. Function values are indicated by surface displacement. Colors indicate derivative values.
[FlyingHeadSlice](/Python/VisualizationAlgorithms/FlyingHeadSlice) | Flying edges used to generate contour lines.
[Frog](/Python/Visualization/Frog) | View frog organs through a translucent skin. Used to generate the illustrations in the VTK TextBook.
[FrogBrain](/Python/Visualization/FrogBrain) | The frog’s brain. Model extracted without smoothing (left) and with smoothing (right).
[FrogDemo](/Python/Visualization/FrogDemo) | View all or some frog tissues and adjust the translucency of the tissues.
[FrogReconstruction](/Python/Visualization/FrogReconstruction) | Frog organs along with a translucent skin are reconstructed from the original segmented data.
[FrogSlice](/Python/Visualization/FrogSlice) | Photographic slice of frog (upper left), segmented frog (upper right) and composite of photo and segmentation (bottom). The purple color represents the stomach and the kidneys are yellow.
[GlyphTable](/Python/Visualization/GlyphTable) | Vary the shapes of glyphed points using a glyph table
[Hanoi](/Python/Visualization/Hanoi) | Towers of Hanoi.
[HanoiInitial](/Python/Visualization/HanoiInitial) | Towers of Hanoi - Initial configuration.
[HanoiIntermediate](/Python/Visualization/HanoiIntermediate) | Towers of Hanoi - Intermediate configuration.
[HardwareSelector](/Python/Visualization/HardwareSelector) |
[Hawaii](/Python/Visualization/Hawaii) | Visualize elevations by coloring the scalar values with a lookup table.
[HeadBone](/Python/VisualizationAlgorithms/HeadBone) | Marching cubes surface of human bone.
[HeadSlice](/Python/VisualizationAlgorithms/HeadSlice) | Marching squares used to generate contour lines.
[HyperStreamline](/Python/VisualizationAlgorithms/HyperStreamline) | Example of hyperstreamlines, the four hyperstreamlines shown are integrated along the minor principle stress axis. A plane (colored with a different lookup table) is also shown.
[IronIsoSurface](/Python/VisualizationAlgorithms/IronIsoSurface) | Marching cubes surface of iron-protein.
[IsosurfaceSampling](/Python/Visualization/IsosurfaceSampling) | Demonstrates how to create point data on an isosurface.
[Kitchen](/Python/Visualization/Kitchen) | Demonstrates stream tracing in a kitchen.
[KochSnowflake](/Python/Visualization/KochSnowflake) | This example draws a Koch snowflake fractal using recursive functions and some of the convenient tools in the toolkit.
[LOx](/Python/VisualizationAlgorithms/LOx) | Streamtubes created by using the computational grid just in front of the post as a source for seeds.
[LOxGrid](/Python/VisualizationAlgorithms/LOxGrid) | Portion of computational grid for the LOx post.
[LOxSeeds](/Python/VisualizationAlgorithms/LOxSeeds) | Streamlines seeded with spherical cloud of points. Four separate cloud positions are shown.
[LoopShrink](/Python/Visualization/LoopShrink) | A network with a loop. VTK 5.0 does not allow you to execute a looping visualization network; this was possible in previous versions of VTK.
[Motor](/Python/VisualizationAlgorithms/Motor) | Texture cut used to reveal internal structure of a motor. Two cut planes are used in combination with transparent texture.
[NamedColorPatches](/Python/Visualization/NamedColorPatches) | Creates a HTML file called [VTKNamedColorPatches](https://htmlpreview.github.io/?https://github.com/Kitware/vtk-examples/blob/gh-pages/VTKNamedColorPatches.html)
[NamedColors](/Python/Visualization/NamedColors) | Demonstrates how to use the vtkNamedColors class.
[NormalsDemo](/Python/Visualization/NormalsDemo) | Demo different options to generate normals.
[Office](/Python/VisualizationAlgorithms/Office) | Using random point seeds to create streamlines.
[OfficeA](/Python/VisualizationAlgorithms/OfficeA) | Corresponds to Fig 9-47(a) in the VTK textbook.
[OfficeTube](/Python/VisualizationAlgorithms/OfficeTube) | The stream polygon. Sweeping a polygon to form a tube.
[OrientedGlyphs](/Python/Visualization/OrientedGlyphs) | Create oriented glyphs from vector data.
[PineRootConnectivity](/Python/VisualizationAlgorithms/PineRootConnectivity) | Applying the connectivity filter to remove noisy isosurfaces.
[PineRootConnectivityA](/Python/VisualizationAlgorithms/PineRootConnectivityA) | The isosurface, with no connectivity filter applied.
[PineRootDecimation](/Python/VisualizationAlgorithms/PineRootDecimation) | Applying the decimation and connectivity filters to remove noisy isosurfaces and reduce data size.
[PlateVibration](/Python/VisualizationAlgorithms/PlateVibration) | Demonstrates the motion of a vibrating beam.
[PointDataSubdivision](/Python/Visualization/PointDataSubdivision) | Vary the shapes of glyphed points using a programmable glyph filter
[PointSize](/Python/Visualization/PointSize) |
[ProbeCombustor](/Python/VisualizationAlgorithms/ProbeCombustor) | Probing data in a combustor.  Probes are regular arrays of 50 by 50 points that are then passed through a contouring filter.
[ProgrammableGlyphFilter](/Python/Visualization/ProgrammableGlyphFilter) | Generate a custom glyph at each point.
[ProgrammableGlyphs](/Python/Visualization/ProgrammableGlyphs) | Generate programmable glyphs.
[PseudoVolumeRendering](/Python/VolumeRendering/PseudoVolumeRendering) | Here we use 100 cut planes, each with an opacity of 0.05. They are then rendered back-to-front to simulate volume rendering.
[QuadricVisualization](/Python/Visualization/QuadricVisualization) | Visualizing a quadric function.
[SingleSplat](/Python/VisualizationAlgorithms/SingleSplat) | Elliptical splatting. (a) Single elliptical splat with eccentricity E=10.  Cone shows orientation of vector.
[SphereTexture](/Python/Visualization/SphereTexture) | Apply an ImageData texture to an sphere
[SpikeFran](/Python/VisualizationAlgorithms/SpikeFran) | Glyphs indicate surface normals on model of human face. Glyph positions are randomly selected.
[SplatFace](/Python/VisualizationAlgorithms/SplatFace) | Elliptical splatting. (b) Surface reconstructed using elliptical splats into 100^3 volume followed by isosurface extraction. Points regularly subsampled and overlaid on original mesh.
[Stocks](/Python/VisualizationAlgorithms/Stocks) | Two views from the stock visualization script. The top shows closing price over time; the bottom shows volume over time.
[StreamLines](/Python/Visualization/StreamLines) | Seed streamlines with vectors from a structured grid.
[StreamlinesWithLineWidget](/Python/VisualizationAlgorithms/StreamlinesWithLineWidget) | Using the vtkLineWidget to produce streamlines in the combustor dataset.  The StartInteractionEvent turns the visibility of the streamlines on; the InteractionEvent causes the streamlines to regenerate themselves.
[TensorAxes](/Python/VisualizationAlgorithms/TensorAxes) | Display the scaled and oriented principal axes of the stress tensor.
[TensorEllipsoids](/Python/VisualizationAlgorithms/TensorEllipsoids) | Display the scaled and oriented principal axes as tensor ellipsoids representing the stress tensor.
[TextSource](/Python/Visualization/TextSource) |
[VectorText](/Python/Visualization/VectorText) | Display high resolution text.
[VelocityProfile](/Python/VisualizationAlgorithms/VelocityProfile) | Warping the geometry of three planes to show flow momentum.
[WarpCombustor](/Python/VisualizationAlgorithms/WarpCombustor) | Carpet plots of combustor flow energy in a structured grid. Colors and plane displacement represent energy values.
[WindowTitle](/Python/Visualization/WindowTitle) |

## Working with vtkImageData

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ImageDataGeometryFilter](/Python/ImageData/ImageDataGeometryFilter) | Convert a vtkImageData to a vtkPolyData.
[ImageNormalize](/Python/ImageData/ImageNormalize) | Normalize an image.
[SumVTKImages](/Python/ImageData/SumVTKImages) | A function to simplify the summing of images, where you can pass a list of images and weights.
[WriteReadVtkImageData](/Python/ImageData/WriteReadVtkImageData) | Generate, edit and read out vtk image data.

## Volume Rendering

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[SimpleRayCast](/Python/VolumeRendering/SimpleRayCast) | Volume rendering of a high potential iron protein.

## User Interaction

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[CallBack](/Python/Interaction/CallBack) | Setting up a callback with client data. Two different methods are demonstrated.
[CellPicking](/Python/Picking/CellPicking) | Cell Picking.
[HighlightPickedActor](/Python/Picking/HighlightPickedActor) | Pick and highlight an actor based on mouse clicks.
[HighlightWithSilhouette](/Python/Picking/HighlightWithSilhouette) | Highlight a picked actor by adding a silhouette.
[InteractorStyleTrackballActor](/Python/Interaction/InteractorStyleTrackballActor) |
[InteractorStyleTrackballCamera](/Python/Interaction/InteractorStyleTrackballCamera) |
[MouseEvents](/Python/Interaction/MouseEvents) | Subclass the interactor style.
[MouseEventsObserver](/Python/Interaction/MouseEventsObserver) | Use an observer.

## Working with Images

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Actor2D](/Python/Images/Actor2D) | 2D actor and mapper.
[BackgroundImage](/Python/Images/BackgroundImage) | Display an image as the "background" of a scene, and render a superquadric in front of it.
[Cast](/Python/Images/Cast) | Cast an image to a different type.

## Image Processing

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Attenuation](/Python/ImageProcessing/Attenuation) | This MRI image illustrates attenuation that can occur due to sensor position.  The artifact is removed by dividing by the attenuation profile determined manually.
[EnhanceEdges](/Python/ImageProcessing/EnhanceEdges) | High-pass filters can extract and enhance edges in an image. Subtraction of the Laplacian (middle) from the original image (left) results in edge enhancement or a sharpening operation (right).
[GaussianSmooth](/Python/ImageProcessing/GaussianSmooth) | Low-pass filters can be implemented as convolution with a Gaussian kernel.
[HybridMedianComparison](/Python/ImageProcessing/HybridMedianComparison) | Comparison of median and hybrid-median filters. The hybrid filter preserves corners and thin lines, better than the median filter.
[IdealHighPass](/Python/ImageProcessing/IdealHighPass) | This figure shows two high-pass filters in the frequency domain. The Butterworth high-pass filter has a gradual attenuation that avoids ringing produced by the ideal high-pass filter with an abrupt transition.
[ImageGradient](/Python/VisualizationAlgorithms/ImageGradient) | Create an imaging pipeline to visualize gradient information.
[ImageWarp](/Python/Images/ImageWarp) | Combine the imaging and visualization pipelines to deform an image in the z-direction. The vtkMergeFilter is used to combine the warped surface with the original color data.
[IsoSubsample](/Python/ImageProcessing/IsoSubsample) | This figure demonstrates aliasing that occurs when a high-frequency signal is subsampled. High frequencies appear as low frequency artifacts. The left image is an isosurface of a skull after subsampling. The right image used a low-pass filter before subsampling to reduce aliasing.
[MedianComparison](/Python/ImageProcessing/MedianComparison) | Comparison of Gaussian and Median smoothing for reducing low-probability high-amplitude noise.
[MorphologyComparison](/Python/ImageProcessing/MorphologyComparison) | This figure demonstrates various binary filters that can alter the shape of segmented regions.
[Pad](/Python/ImageProcessing/Pad) | Convolution in frequency space treats the image as a periodic function. A large kernel can pick up features from both sides of the image. The lower-left image has been padded with zeros to eliminate wraparound during convolution. On the right, mirror padding has been used to remove artificial edges introduced by borders.
[VTKSpectrum](/Python/ImageProcessing/VTKSpectrum) | The discrete Fourier transform changes an image from the spatial domain into the frequency domain, where each pixel represents a sinusoidal function. This figure shows an image and its power spectrum displayed using a logarithmic transfer function.

## Widgets

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[BalloonWidget](/Python/Widgets/BalloonWidget) | Uses a vtkBalloonWidget to draw labels when the mouse stays above an actor.
[BoxWidget](/Python/Widgets/BoxWidget) | This 3D widget defines a region of interest that is represented by an arbitrarily oriented hexahedron with interior face angles of 90 degrees (orthogonal faces). The object creates 7 handles that can be moused on and manipulated.
[CameraOrientationWidget](/Python/Widgets/CameraOrientationWidget) | Demonstrates a 3D camera orientation widget.
[CompassWidget](/Python/Widgets/CompassWidget) | Draws an interactive compass.
[ContourWidget](/Python/Widgets/ContourWidget) | Draw a contour (line) which can be deformed by the user
[ImplicitPlaneWidget2](/Python/Widgets/ImplicitPlaneWidget2) | Clip polydata with an implicit plane.
[OrientationMarkerWidget](/Python/Widgets/OrientationMarkerWidget) | Draws two cubes. One of them can be clicked to be rotated, and will rotate the second one. The second one has annotations on it, and can also be moved.
[OrientationMarkerWidget1](/Python/Widgets/OrientationMarkerWidget1) | Display a polydata as an orientation icon.
[ScalarBarWidget](/Python/Widgets/ScalarBarWidget) | The ScalarBarWidget displays a scalar bar that is movable and changes orientation automatically when close to the borders of the image. It needs a ScalarBarActor
[SphereWidget](/Python/Widgets/SphereWidget) | This 3D widget defines a sphere that can be interactively placed in a scene.
[SplineWidget](/Python/Widgets/SplineWidget) | This example shows how to use vtkSplineWidget with a callback being used to get the length of the spline widget.
[TextWidget](/Python/Widgets/TextWidget) | Annotate a VTK image with movable text

## Plotting

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[MultiplePlots](/Python/Plotting/MultiplePlots) | Display multiple plots by using viewports in a single render window.
[ScatterPlot](/Python/Plotting/ScatterPlot) | Scatter plot.
[SpiderPlot](/Python/Plotting/SpiderPlot) | Spider plot.
[SurfacePlot](/Python/Plotting/SurfacePlot) | Surface plot.

## Animation

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Animation](/Python/Utilities/Animation) | Move a sphere across a scene.

## Annotation

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[MultiLineText](/Python/Annotation/MultiLineText) | Display multiline text.
[TextOrigin](/Python/Annotation/TextOrigin) | This example demonstrates the use of vtkVectorText and vtkFollower. vtkVectorText is used to create 3D annotation.

## InfoVis

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ParallelCoordinatesExtraction](/Python/InfoVis/ParallelCoordinatesExtraction) | Extract data based on a selection in a Parallel Coordinates View
[SelectedGraphIDs](/Python/InfoVis/SelectedGraphIDs) | Callback on vtkAnnotationLink in a Graph Layout View when selection is changed

## PyQt

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[EmbedInPyQt](/Python/Widgets/EmbedInPyQt) | Shows how to easily embed VTK in PyQt with QVTKRenderWindowInteractor, first example
[EmbedInPyQt2](/Python/Widgets/EmbedInPyQt2) | Shows how to embed VTK in PyQt with QVTKRenderWindowInteractor, second example
