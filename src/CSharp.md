# CSharp Examples

## VTK Classes Summary

- [VTK Classes with Examples](/Coverage/CSharpVTKClassesUsed.md), this table is really useful when searching for example(s) using a particular class.

- [VTK Classes with No Examples](/Coverage/CSharpVTKClassesNotUsed.md), please add examples in your area of expertise!

## Simple Operations

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[DistanceBetweenPoints](/CSharp/SimpleOperations/DistanceBetweenPoints) | Computes the distance between two point Distance between two points
[DistancePointToLine](/CSharp/SimpleOperations/DistancePointToLine) | Distance between a point and a line
[GaussianRandomNumber](/CSharp/SimpleOperations/GaussianRandomNumber) | Generates Gaussian random number Random number (Gaussian distribution)
[PerspectiveTransform](/CSharp/SimpleOperations/PerspectiveTransform) | Apply a perspective transformation to a point
[ProjectPointPlane](/CSharp/SimpleOperations/ProjectPointPlane) | Project a point onto a plan Project a point onto a plane
[RandomSequence](/CSharp/SimpleOperations/RandomSequence) | This is the preferred way to generate random numbers Random sequence
[UniformRandomNumber](/CSharp/SimpleOperations/UniformRandomNumber) | Generates uniform random number Random number (uniform distribution)

## Input and Output

### Graph Formats

### 3D File Formats

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ConvertFile](/CSharp/IO/ConvertFile) | Convert a file from one type to another
[XGMLReader](/CSharp/InfoVis/XGMLReader) | Read a .gml file

#### Standard Formats

##### Input

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[DEMReader](/CSharp/IO/DEMReader) | Read DEM (elevation map) files
[ReadDICOMSeries](/CSharp/IO/ReadDICOMSeries) | This example demonstates how to read a series of DICOM images and scroll through slice Convert a series of DICOM files into a VTI File
[ReadPDB](/CSharp/IO/ReadPDB) | Read Protein Data Bank Files Read Protein Data Bank files (.pdb) files
[ReadPLOT3D](/CSharp/IO/ReadPLOT3D) | Read CFD (computational fluid dynamics) data produced by PLOT3D Read PLOT3D (.bin) files
[ReadPLY](/CSharp/IO/ReadPLY) | Read PLY (.ply) files
[ReadSTL](/CSharp/IO/ReadSTL) | Read stereo lithography STL (.stl) files
[SimplePointsReader](/CSharp/IO/SimplePointsReader) | Read a simple "xyz" file of points
[VRML](/CSharp/IO/VRML) | Read a VRML (WRL) file

#### VTK Formats

##### Input

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ParticleReader](/CSharp/IO/ParticleReader) | Read an ASCII file
[ReadImageData](/CSharp/IO/ReadImageData) | Read a image data (.vti) file
[ReadOBJ](/CSharp/IO/ReadOBJ) | Read a Wavefront OBJ file
[ReadPlainText](/CSharp/IO/ReadPlainText) | Read a plain text file
[ReadPolyData](/CSharp/IO/ReadPolyData) | Read a polygonal data (.vtp) file
[ReadRectilinearGrid](/CSharp/IO/ReadRectilinearGrid) | Read a rectilinear grid (.vtr) file
[ReadStructuredGrid](/CSharp/IO/ReadStructuredGrid) | Read a structured grid (.vts) file
[ReadUnknownTypeXMLFile](/CSharp/IO/ReadUnknownTypeXMLFile) | Read an XML file and later decide what kind it is
[ReadUnstructuredGrid](/CSharp/IO/ReadUnstructuredGrid) | Demonstrates how to read a .vtu fil Read an unstructured grid (.vtu) file

##### Output

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[WriteVTI](/CSharp/IO/WriteVTI) | VTI is an "ImageData" Write a .vti file
[WriteVTP](/CSharp/IO/WriteVTP) | VTP is a "PolyData". This format allows for the most complex geometric objects to be stored Write a .vtp file
[WriteVTU](/CSharp/IO/WriteVTU) | VTU is an "Unstructured Grid". This format allows for 3D data to be stored Write a .vtu file
[XMLStructuredGridWriter](/CSharp/IO/XMLStructuredGridWriter) | Write a .vts file from a vtkStructuredGrid

### Image Formats

#### Input

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ImageReader2Factory](/CSharp/IO/ImageReader2Factory) | This class tries to find an appropriate reader. It is very convenient to use this when you don't know what kind of file to expect Read most standard image formats.
[JPEGReader](/CSharp/IO/JPEGReader) | Read a JPG image
[MetaImageReader](/CSharp/IO/MetaImageReader) | Read .mha files
[PNGReader](/CSharp/IO/PNGReader) | Read a PNG image
[ReadBMP](/CSharp/IO/ReadBMP) | Read BMP (.bmp) files
[ReadPNM](/CSharp/IO/ReadPNM) | Read PNM (.pnm) files
[ReadTIFF](/CSharp/IO/ReadTIFF) | Read TIFF (.tif) files

#### Output

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[JPEGWriter](/CSharp/IO/JPEGWriter) | Write a JPEG (.jpg) file
[MetaImageWriter](/CSharp/IO/MetaImageWriter) | Write a .mha/.mhd + .raw file
[WriteBMP](/CSharp/IO/WriteBMP) | Write a BMP (.bmp) file
[WritePNG](/CSharp/IO/WritePNG) | Write a PNG (.png) file
[WritePNM](/CSharp/IO/WritePNM) | Write a PNM (.pnm) file
[WriteTIFF](/CSharp/IO/WriteTIFF) | Write a TIFF (.tif) file

## Geometric Objects

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Arrow](/CSharp/GeometricObjects/Arrow) | Arrow
[Axes](/CSharp/GeometricObjects/Axes) | Axes
[ColoredLines](/CSharp/GeometricObjects/ColoredLines) | Colored Lines
[Cone](/CSharp/GeometricObjects/Cone) | Cone
[Cube](/CSharp/GeometricObjects/Cube) | Cube
[Cylinder](/CSharp/GeometricObjects/Cylinder) | Cylinder
[Disk](/CSharp/GeometricObjects/Disk) | A circle with a hole in it Disk
[Frustum](/CSharp/GeometricObjects/Frustum) | Frustum
[GeometricObjectsDemo](/CSharp/GeometricObjects/GeometricObjectsDemo) | A demonstration of all geometric objects that work the same way
[Hexahedron](/CSharp/GeometricObjects/Hexahedron) | Hexahedron
[Line](/CSharp/GeometricObjects/Line) | Line
[LongLine](/CSharp/GeometricObjects/LongLine) | Manually create a polyline
[OrientedArrow](/CSharp/GeometricObjects/OrientedArrow) | Oriented Arrow
[Plane](/CSharp/GeometricObjects/Plane) | Plane
[Planes](/CSharp/GeometricObjects/Planes) | Planes
[PlanesIntersection](/CSharp/GeometricObjects/PlanesIntersection) | PlanesIntersection
[PlatonicSolid](/CSharp/GeometricObjects/PlatonicSolid) | PlatonicSolid
[Point](/CSharp/GeometricObjects/Point) | Point
[PolyLine](/CSharp/GeometricObjects/PolyLine) | PolyLine
[Polygon](/CSharp/GeometricObjects/Polygon) | Polygon
[PolygonIntersection](/CSharp/GeometricObjects/PolygonIntersection) | IntersectWithLine( Compute the intersection of a line segment with a polygon
[Pyramid](/CSharp/GeometricObjects/Pyramid) | Pyramid
[Quad](/CSharp/GeometricObjects/Quad) | Quad
[RegularPolygonSource](/CSharp/GeometricObjects/RegularPolygonSource) | Regular Polygon
[Sphere](/CSharp/GeometricObjects/Sphere) | Sphere
[Tetrahedron](/CSharp/GeometricObjects/Tetrahedron) | Tetrahedron
[Triangle](/CSharp/GeometricObjects/Triangle) | Triangle
[TriangleStrip](/CSharp/GeometricObjects/TriangleStrip) | Triangle strip
[Vertex](/CSharp/GeometricObjects/Vertex) | Vertex

### Parametric Objects

These examples demonstrate how to create an display one of the many vtkParametric* objects. All of the classes listed can be used in an identical fashion.

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ParametricObjects](/CSharp/GeometricObjects/ParametricObjects) | Create and display a parametric object
[ParametricObjectsDemo](/CSharp/GeometricObjects/ParametricObjectsDemo) | Demonstration of all of the vtkParametric* classes

## Implicit Functions and Iso-surfaces

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[BandedPolyDataContourFilter](/CSharp/VisualizationAlgorithms/BandedPolyDataContourFilter) | BandedPolyDataContourFilter
[ExtractLargestIsosurface](/CSharp/Modelling/ExtractLargestIsosurface) | Extract largest isosurface
[FilledContours](/CSharp/VisualizationAlgorithms/FilledContours) | Create filled contours (vtkClipPolyData)
[ImplicitSphere](/CSharp/ImplicitFunctions/ImplicitSphere) | An implicit representation of a sphere
[IsoContours](/CSharp/ImplicitFunctions/IsoContours) | Visualize different isocontours using a slider
[MarchingCubes](/CSharp/Modelling/MarchingCubes) | Create a voxelized spher Marching cubes
[MarchingSquares](/CSharp/WishList/Modelling/MarchingSquares) | Create a contour from a structured point set (image Marching squares
[SampleFunction](/CSharp/ImplicitFunctions/SampleFunction) | Sample and visualize an implicit function

## Working with 3D Data

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AlignFrames](/CSharp/PolyData/AlignFrames) | Align coordinate frames
[ContoursFromPolyData](/CSharp/Filtering/ContoursFromPolyData) | Create contours from PolyData
[FindAllArrayNames](/CSharp/IO/FindAllArrayNames) | Get the names of all of the data arrays
[ImplicitBoolean](/CSharp/Filters/ImplicitBoolean) | Operations include intersection and union.
[IterateOverLines](/CSharp/PolyData/IterateOverLines) | Iterate through the lines of a PolyData
[MultiBlockMergeFilter](/CSharp/PolyData/MultiBlockMergeFilter) | Combine MultiBlockDataSets
[NullPoint](/CSharp/PolyData/NullPoint) | Set everything in PointData at a specified index to NUL NullPoint
[PolyDataGetPoint](/CSharp/PolyData/PolyDataGetPoint) | Get point locations/coordinates from a vtkPolyData
[ShrinkPolyData](/CSharp/PolyData/ShrinkPolyData) | Move all items in a PolyData towards their centroid
[VectorFieldNonZeroExtraction](/CSharp/PolyData/VectorFieldNonZeroExtraction) | This example shows how to create an imagedata object, associate scalar data with some points and visualize only nonzero scalar value Extract non-zero vectors from a vtkImageData
[WarpVector](/CSharp/PolyData/WarpVector) | vtkWarpVector

### Working with Meshes

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[BoundaryEdges](/CSharp/Meshes/BoundaryEdges) | Find the edges that are used by only one face Finding Boundary Edges
[CapClip](/CSharp/Meshes/CapClip) | Cap a clipped polydata with a polygon
[CellEdges](/CSharp/Meshes/CellEdges) | Get edges of cells
[ClosedSurface](/CSharp/PolyData/ClosedSurface) | Check if a surface is closed
[ColorDisconnectedRegions](/CSharp/PolyData/ColorDisconnectedRegions) | Color each disconnected region of a vtkPolyData a different color
[ColoredElevationMap](/CSharp/Meshes/ColoredElevationMap) | Colormap, color ma Color a mesh by height
[Curvature](/CSharp/PolyData/Curvature) | Compute Gaussian, Mean, Min, and Max Curvatures
[Decimate](/CSharp/Meshes/Decimate) | Reduce the number of triangles in a mesh Decimation
[DijkstraGraphGeodesicPath](/CSharp/PolyData/DijkstraGraphGeodesicPath) | Find the shortest path between two points on a mesh
[ElevationFilter](/CSharp/Meshes/ElevationFilter) | Color a mesh by height
[ExtractEdges](/CSharp/Meshes/ExtractEdges) | Extract Edges
[FillHoles](/CSharp/Meshes/FillHoles) | Close holes in a mesh Fill Holes
[GreedyTerrainDecimation](/CSharp/PolyData/GreedyTerrainDecimation) | Create a mesh from an ImageData
[HighlightBadCells](/CSharp/PolyData/HighlightBadCells) | Highlight Bad Cells
[LargestRegion](/CSharp/PolyData/PolyDataConnectivityFilter_LargestRegion) | Extract the largest connected region in a polydata
[MatrixMathFilter](/CSharp/Meshes/MatrixMathFilter) | Compute various quantities on cell and points in a mesh Matrix math filter
[OBBDicer](/CSharp/Meshes/OBBDicer) | Breakup a mesh into pieces OBBDicer
[PolygonalSurfaceContourLineInterpolator](/CSharp/PolyData/PolygonalSurfaceContourLineInterpolator) | Interactively find the shortest path between two points on a mesh
[QuadricClustering](/CSharp/Meshes/QuadricClustering) | Reduce the number of triangles in a mesh Decimation (quadric clustering)
[QuadricDecimation](/CSharp/Meshes/QuadricDecimation) | Reduce the number of triangles in a mesh Decimation (quadric decimation)
[SelectPolyData](/CSharp/PolyData/SelectPolyData) | Select a region of a mesh
[SimpleElevationFilter](/CSharp/Meshes/SimpleElevationFilter) | Color a mesh by dotting a vector from the origin to each point with a specified vector
[SolidClip](/CSharp/Meshes/SolidClip) | Create a "solid" clip
[SpecifiedRegion](/CSharp/PolyData/PolyDataConnectivityFilter_SpecifiedRegion) | Extract a specific (specified) connected region in a polydata
[Subdivision](/CSharp/Meshes/Subdivision) | Increase the number of triangles in a mesh Subdivision
[Triangulate](/CSharp/Meshes/Triangulate) | Convert all polygons in a mesh to triangles Triangulate a mesh
[WeightedTransformFilter](/CSharp/PolyData/WeightedTransformFilter) | vtkWeightedTransformFilter
[WindowedSincPolyDataFilter](/CSharp/Meshes/WindowedSincPolyDataFilter) | Smooth a mesh.

### Working with Structured 3D Data

This section includes vtkImageData, vtkStructuredGrid, and vtkRectilinearGrid.

"ImageData" is not the traditional "flat, 2D image" you are used to. It is a special VTK data structure in the collection of 3D data structures provided by VTK. Here is an overview of these data structures: [http://vtk.org/Wiki/VTK/Tutorials/3DDataTypes]. Image data can represent at typical 2D image, but also, a 3D volume.
