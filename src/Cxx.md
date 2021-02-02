# C++ Examples

These are fully independent, compilable examples. There is significant overlap in the examples, but they are each intended to illustrate a different concept and be fully stand alone compilable.

## VTK Classes Summary

This Python script, [SelectExamples](../Python/Utilities/SelectExamples), will let you select examples based on a VTK Class and language. It requires Python 3.7 or later. The following tables will produce similar information.

- [VTK Classes with Examples](/Coverage/CxxVTKClassesUsed.md), this table is really useful when searching for example(s) using a particular class.

- [VTK Classes with No Examples](/Coverage/CxxVTKClassesNotUsed.md), please add examples in your area of expertise!

## Tutorials

If you are new to VTK then these [tutorials](#tutorial) will help to get you started.

## Hello World

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[A hello world example](/Cxx/GeometricObjects/CylinderExample) | Cylinder example from the VTK Textbook and source code. A hello world example.

## Simple Operations

| Example Name | Description | Image |
| -------------- | ------------- | -------- |
[DistanceBetweenPoints](/Cxx/SimpleOperations/DistanceBetweenPoints) | Distance between two points.
[DistancePointToLine](/Cxx/SimpleOperations/DistancePointToLine) | Distance between a point and a line.
[FloatingPointExceptions](/Cxx/SimpleOperations/FloatingPointExceptions) | Floating Point Exceptions
[GaussianRandomNumber](/Cxx/SimpleOperations/GaussianRandomNumber) | Generates Gaussian random numbers.
[PerspectiveTransform](/Cxx/SimpleOperations/PerspectiveTransform) | Apply a perspective transformation to a point.
[ProjectPointPlane](/Cxx/SimpleOperations/ProjectPointPlane) | Project a point onto a plane.
[RandomSequence](/Cxx/SimpleOperations/RandomSequence) | This is the preferred way to generate random numbers.
[UniformRandomNumber](/Cxx/SimpleOperations/UniformRandomNumber) | Generates uniform random numbers.

## Input and Output

### Graph Formats

### 3D File Formats

| Example Name | Description | Image |
| -------------- | ------------- | -------- |
[ConvertFile](/Cxx/IO/ConvertFile) | Convert a file from one type to another
[XGMLReader](/Cxx/InfoVis/XGMLReader) | Read a .gml file

#### Standard Formats

##### Input

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[DEMReader](/Cxx/IO/DEMReader) | Read DEM (elevation map) files
[DelimitedTextReader](/Cxx/InfoVis/DelimitedTextReader) | Read a delimited file into a polydata.
[DumpXMLFile](/Cxx/IO/DumpXMLFile) | Report the contents of a VTK XML or legacy file
[IndividualVRML](/Cxx/IO/IndividualVRML) | This example is used to get each actor and object from a scene and verify axes correspondence
[ParticleReader](/Cxx/IO/ParticleReader) | This example reads ASCII files where each line consists of points with its position (x,y,z) and (optionally) one scalar or binary files in RAW 3d file format.
[ReadAllPolyDataTypes](/Cxx/IO/ReadAllPolyDataTypes) | Read any VTK polydata file.
[ReadAllPolyDataTypesDemo](/Cxx/IO/ReadAllPolyDataTypesDemo) | Read all VTK polydata file types.
[ReadAllUnstructuredGridTypes](/Cxx/IO/ReadAllUnstructuredGridTypes) | Read any VTK unstructuredgrid file.
[ReadCML](/Cxx/IO/ReadCML) | Read Chemistry Markup Language files.
[ReadExodusData](/Cxx/IO/ReadExodusData) | Read and view ExodusII data.
[ReadOBJ](/Cxx/IO/ReadOBJ) | Read an OBJ (.obj) file.
[ReadPDB](/Cxx/IO/ReadPDB) | Read Protein Data Bank Files.
[ReadPLOT3D](/Cxx/IO/ReadPLOT3D) | Read CFD (computational fluid dynamics) data produced by PLOT3D.
[ReadPLY](/Cxx/IO/ReadPLY) | Read PLY (.ply) files
[ReadPlainTextTriangles](/Cxx/IO/ReadPlainTextTriangles) | Read a plain text file into a polydata.
[ReadSLC](/Cxx/IO/ReadSLC) | Read VolVis file.
[ReadSTL](/Cxx/IO/ReadSTL) | Read stereo lithography STL (.stl) files
[ReadTextFile](/Cxx/IO/ReadTextFile) | Read a plain text file into a polydata.
[SimplePointsReader](/Cxx/IO/SimplePointsReader) | Read a simple "xyz" file of points.

###### Importers

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[3DSImporter](/Cxx/IO/3DSImporter) | Import a 3D Studio scene that includes multiple actors.
[GLTFImporter](/Cxx/IO/GLTFImporter) | Import a glTF scene.
[ImportPolyDataScene](/Cxx/IO/ImportPolyDataScene) | Import a polydata scene using multiblock datasets.
[OBJImporter](/Cxx/IO/OBJImporter) | Import an OBJ scene that includes actors, materials and textures.
[VRMLImporter](/Cxx/IO/VRMLImporter) | Import a VRML 2.0 (WRL) file.
[VRMLImporterDemo](/Cxx/IO/VRMLImporterDemo) | Import a VRML 2.0 (WRL) file and add normals if necessary.

##### Output

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[DelimitedTextWriter](/Cxx/InfoVis/DelimitedTextWriter) | Write data to a delimited file
[SimplePointsWriter](/Cxx/IO/SimplePointsWriter) | Write a simple ".xyz" file
[WritePLY](/Cxx/IO/WritePLY) | Write PLY (.ply) files
[WriteSTL](/Cxx/IO/WriteSTL) | Write stereo lithography STL (.stl) files

###### Exporters

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ExportPolyDataScene](/Cxx/IO/ExportPolyDataScene) | Export a polydata scene using multiblock datasets.

#### VTK Formats

##### Input

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ReadImageData](/Cxx/IO/ReadImageData) | Read a image data (.vti) file
[ReadPolyData](/Cxx/IO/ReadPolyData) | Read a polygonal data (.vtp) file
[ReadRectilinearGrid](/Cxx/IO/ReadRectilinearGrid) | Read a rectilinear grid (.vtr) file
[ReadStructuredGrid](/Cxx/IO/ReadStructuredGrid) | Read a structured grid (.vts) file
[ReadUnknownTypeXMLFile](/Cxx/IO/ReadUnknownTypeXMLFile) | Read an XML file and later decide what kind it is
[ReadUnstructuredGrid](/Cxx/IO/ReadUnstructuredGrid) | Read an unstructured grid (.vtu) file

##### Output

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ExodusIIWriter](/Cxx/Parallel/ExodusIIWriter) | Write a time varying ExodusII file.
[WriteVTI](/Cxx/IO/WriteVTI) | Write a .vti file. VTI is an "ImageData".
[WriteVTP](/Cxx/IO/WriteVTP) | Write a .vtp file. VTP is a "PolyData". This format allows for the most complex geometric objects to be stored.
[WriteVTU](/Cxx/IO/WriteVTU) | Write a .vtu file. VTU is an "Unstructured Grid". This format allows for 3D data to be stored.
[XMLPImageDataWriter](/Cxx/IO/XMLPImageDataWriter) | Write a .pvti file with 4 .vti pieces.
[XMLPUnstructuredGridWriter](/Cxx/IO/XMLPUnstructuredGridWriter) | Write a .pvtu with 4 .vtu pieces.
[XMLStructuredGridWriter](/Cxx/IO/XMLStructuredGridWriter) | Write a .vts file from a vtkStructuredGrid.

#### Legacy VTK Formats

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[GenericDataObjectReader](/Cxx/IO/GenericDataObjectReader) | Read any type of legacy .vtk file.
[ReadLegacyUnstructuredGrid](/Cxx/IO/ReadLegacyUnstructuredGrid) | Read an unstructured grid that contains 11 linear cells.
[StructuredGridReader](/Cxx/IO/StructuredGridReader) | Read a structured grid (.vtk) file.
[StructuredPointsReader](/Cxx/IO/StructuredPointsReader) | Read a structured points (.vtk) file.
[WriteLegacyLinearCells](/Cxx/IO/WriteLegacyLinearCells) | Write each linear cell into a legacy UnstructuredGrid file.
[WriteXMLLinearCells](/Cxx/IO/WriteXMLLinearCells) | Write each linear cell into an XML UnstructuredGrid file (.vtu).

### Image Format

#### Input

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ImageReader2Factory](/Cxx/IO/ImageReader2Factory) | This class tries to find an appropriate reader. It is very convenient to use this when you don't know what kind of file to expect.
[JPEGReader](/Cxx/IO/JPEGReader) | Read a JPEG image.
[MetaImageReader](/Cxx/IO/MetaImageReader) | Read .mha files.
[PNGReader](/Cxx/IO/PNGReader) | Read a PNG image.
[ReadBMP](/Cxx/IO/ReadBMP) | Read BMP (.bmp) files.
[ReadDICOM](/Cxx/IO/ReadDICOM) | Read DICOM file
[ReadDICOMSeries](/Cxx/IO/ReadDICOMSeries) | This example demonstrates how to read a series of DICOM images and scroll through slices
[ReadPNM](/Cxx/IO/ReadPNM) | Read PNM (.pnm) files.
[ReadTIFF](/Cxx/IO/ReadTIFF) | Read TIFF (.tif) files.

#### Output

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ImageWriter](/Cxx/IO/ImageWriter) | Write an imagefile based on the file extension.
[JPEGWriter](/Cxx/IO/JPEGWriter) | Write a JPEG (.jpg) file.
[MetaImageWriter](/Cxx/IO/MetaImageWriter) | Write a .mha/.mhd + .raw file.
[PNGWriter](/Cxx/IO/PNGWriter) | Write a PNG (.png) file.
[WriteBMP](/Cxx/IO/WriteBMP) | Write a BMP (.bmp) file.
[WritePNM](/Cxx/IO/WritePNM) | Write a PNM (.pnm) file.
[WriteTIFF](/Cxx/IO/WriteTIFF) | Write a TIFF (.tif) file.

## Geometric Objects

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Axes](/Cxx/GeometricObjects/Axes) |
[ColoredLines](/Cxx/GeometricObjects/ColoredLines) |
[Cube](/Cxx/GeometricObjects/Cube) | Manually build a polygonal cube.
[Cube1](/Cxx/GeometricObjects/Cube1) | A nice simple example that demonstrates the operation of the VTK pipeline.
[Dodecahedron](/Cxx/GeometricObjects/Dodecahedron) | Create a dodecahedron using vtkPolyhedron.
[EllipticalCylinder](/Cxx/GeometricObjects/EllipticalCylinder) | Create an elliptical cylinder using extrusion.
[EllipticalCylinderDemo](/Cxx/GeometricObjects/EllipticalCylinderDemo) | Show the base and extrusion vector.
[GeometricObjectsDemo](/Cxx/GeometricObjects/GeometricObjectsDemo) | A demonstration of all geometric objects that work the same way.
[OpenVRFrustum](/Cxx/GeometricObjects/OpenVRFrustum) | VTK + OpenVR Example.
[OpenVROrientedArrow](/Cxx/GeometricObjects/OpenVROrientedArrow) | VTK + OpenVR Example.
[OpenVROrientedCylinder](/Cxx/GeometricObjects/OpenVROrientedCylinder) | VTK + OpenVR Example.
[OpenVRTessellatedBoxSource](/Cxx/GeometricObjects/OpenVRTessellatedBoxSource) | VTK + OpenVR Example.
[PlaneSourceDemo](/Cxx/GeometricObjects/PlaneSourceDemo) | Display the instance variables that define a vtkPlaneSource.
[Planes](/Cxx/GeometricObjects/Planes) |
[PlanesIntersection](/Cxx/GeometricObjects/PlanesIntersection) |
[PolygonIntersection](/Cxx/GeometricObjects/PolygonIntersection) | Compute the intersection of a line segment with a polygon.
[ShrinkCube](/Cxx/GeometricObjects/ShrinkCube) | Applying shrink filter to an object.
[SourceObjectsDemo](/Cxx/GeometricObjects/SourceObjectsDemo) | Examples of source objects that procedurally generate polygonal models.  These nine images represent just some of the capability of VTK. From upper left in reading order: sphere, cone, cylinder, cube, plane, text, random point cloud, disk (with or without hole), and line source. Other polygonal source objects are available; check subclasses of vtkPolyDataAlgorithm.

### Cells

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Cell3DDemonstration](/Cxx/GeometricObjects/Cell3DDemonstration) | Demonstrate the use of vtkCell3D to construct geometric objects. Sample objects are generated from the classes derived from vtkCell3D and displayed.
[CellTypeSource](/Cxx/GeometricObjects/CellTypeSource) | Generate tessellated cells.
[ConvexPointSet](/Cxx/GeometricObjects/ConvexPointSet) | Generate a ConvexPointSetCell.
[Hexahedron](/Cxx/GeometricObjects/Hexahedron) |
[LinearCellDemo](/Cxx/GeometricObjects/LinearCellDemo) | Linear cell types in VTK.
[LongLine](/Cxx/GeometricObjects/LongLine) | Manually create a polyline.
[Point](/Cxx/GeometricObjects/Point) |
[PolyLine](/Cxx/GeometricObjects/PolyLine) |
[Polygon](/Cxx/GeometricObjects/Polygon) |
[Polyhedron](/Cxx/GeometricObjects/Polyhedron) | Create an unstructured grid representation of a polyhedron (cube) and write it out to a file.
[Pyramid](/Cxx/GeometricObjects/Pyramid) |
[Quad](/Cxx/GeometricObjects/Quad) |
[Tetrahedron](/Cxx/GeometricObjects/Tetrahedron) |
[Triangle](/Cxx/GeometricObjects/Triangle) |
[Triangle strip](/Cxx/GeometricObjects/TriangleStrip) |
[Vertex](/Cxx/GeometricObjects/Vertex) |

### Sources

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Arrow](/Cxx/GeometricObjects/Arrow) |
[Cone](/Cxx/GeometricObjects/Cone) | Display a cone
[ConeDemo](/Cxx/GeometricObjects/ConeDemo) | Disolay four different resolution cones.
[Cylinder](/Cxx/GeometricObjects/Cylinder) |
[Disk](/Cxx/GeometricObjects/Disk) | A circle with a hole in it.
[EarthSource](/Cxx/GeometricObjects/EarthSource) | Create the Earth.
[Frustum](/Cxx/GeometricObjects/Frustum) |
[Line](/Cxx/GeometricObjects/Line) |
[OrientedArrow](/Cxx/GeometricObjects/OrientedArrow) | Orient an arrow along an arbitrary vector.
[OrientedCylinder](/Cxx/GeometricObjects/OrientedCylinder) | Orient a cylinder along an arbitrary vector.
[Plane](/Cxx/GeometricObjects/Plane) |
[PlatonicSolids](/Cxx/GeometricObjects/PlatonicSolids) |
[PolyLine1](/Cxx/GeometricObjects/PolyLine1) | This example demonstrates how to create a polygon through several ordered points.
[RegularPolygonSource](/Cxx/GeometricObjects/RegularPolygonSource) |
[Sphere](/Cxx/GeometricObjects/Sphere) |
[TessellatedBoxSource](/Cxx/GeometricObjects/TessellatedBoxSource) | Generate a box with tessellated sides.
[TextActor](/Cxx/GeometricObjects/TextActor) | 2D "HUD-type" text

### Non Linear

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[IsoparametricCellsDemo](/Cxx/GeometricObjects/IsoparametricCellsDemo) | Nonlinear isoparametric cell types in VTK.
[QuadraticHexahedron](/Cxx/GeometricObjects/QuadraticHexahedron) | Create and tessellate a nonlinear cell.
[QuadraticHexahedronDemo](/Cxx/GeometricObjects/QuadraticHexahedronDemo) | Interactively adjust chord error.
[QuadraticTetra](/Cxx/GeometricObjects/QuadraticTetra) | Create and tessellate a nonlinear cell.
[QuadraticTetraDemo](/Cxx/GeometricObjects/QuadraticTetraDemo) | Interactively adjust chord error.

### Parametric Objects

These examples demonstrate how to create an display one of the many vtkParametric objects. All of the classes listed can be used in an identical fashion.

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ParametricKuenDemo](/Cxx/GeometricObjects/ParametricKuenDemo) | Interactively change the parameters for a Kuen Surface.
[ParametricObjectsDemo](/Cxx/GeometricObjects/ParametricObjectsDemo) | Demonstrates the Parametric classes added by Andrew Maclean and additional classes added by Tim Meehan. The parametric spline is also included. Options are provided to display single objects, add backface, add normals and print out an image.
[ParametricSuperEllipsoidDemo](/Cxx/GeometricObjects/ParametricSuperEllipsoidDemo) | Interactively change the parameters for a SuperEllipsoid Surface.
[ParametricSuperToroidDemo](/Cxx/GeometricObjects/ParametricSuperToroidDemo) | Interactively change the parameters for a SuperToroid Surface.

## Implicit Functions and Iso-surfaces

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[BandedPolyDataContourFilter](/Cxx/VisualizationAlgorithms/BandedPolyDataContourFilter) | Create filled contours.
[BooleanOperationImplicitFunctions](/Cxx/ImplicitFunctions/BooleanOperationImplicitFunctions) | Demonstrate booleans of two different implicit functions
[ContourTriangulator](/Cxx/Modelling/ContourTriangulator) | Create a contour from a structured point set (image) and triangulate it.
[CutWithCutFunction](/Cxx/VisualizationAlgorithms/CutWithCutFunction) | Cut a surface with an implicit plane using vtkCutter.
[CutWithScalars](/Cxx/VisualizationAlgorithms/CutWithScalars) | Cut a surface with scalars.
[DiscreteMarchingCubes](/Cxx/Modelling/DiscreteMarchingCubes) | Generate surfaces from labeled data.
[ExtractData](/Cxx/VisualizationAlgorithms/ExtractData) | Implicit functions used to select data: Two ellipsoids are combined using the union operation used to select voxels from a volume. Voxels are shrunk 50 percent.
[ExtractLargestIsosurface](/Cxx/Modelling/ExtractLargestIsosurface) | Extract largest isosurface.
[FilledContours](/Cxx/VisualizationAlgorithms/FilledContours) | Create filled contours (using vtkClipPolyData).
[Hello](/Cxx/VisualizationAlgorithms/Hello) | Implicit modelling used to thicken a stroked font.
[IceCream](/Cxx/VisualizationAlgorithms/IceCream) | How to use boolean combinations of implicit functions to create a model of an ice cream cone.
[ImplicitDataSet](/Cxx/ImplicitFunctions/ImplicitDataSet) | Convert an imagedata to an implicit function.
[ImplicitQuadric](/Cxx/ImplicitFunctions/ImplicitQuadric) | Create an ellipsoid using an implicit quadric.
[ImplicitSphere](/Cxx/ImplicitFunctions/ImplicitSphere) | An implicit representation of a sphere.
[ImplicitSphere1](/Cxx/ImplicitFunctions/ImplicitSphere1) | Demonstrate sampling of a sphere implicit function.
[IsoContours](/Cxx/ImplicitFunctions/IsoContours) | Visualize different isocontours using a slider.
[Lorenz](/Cxx/Visualization/Lorenz) | Visualizing a Lorenz strange attractor by integrating the Lorenz equations in a volume.
[MarchingCases](/Cxx/VisualizationAlgorithms/MarchingCases) | Explore the Marching Cubes cases.
[MarchingCasesA](/Cxx/VisualizationAlgorithms/MarchingCasesA) | The 256 possible cases have been reduced to 15 cases using symmetry.
[MarchingCasesB](/Cxx/VisualizationAlgorithms/MarchingCasesB) | Marching cubes complementary cases. Cases 3c, 6c, 7c, 10c, 12c and 13c are displayed.
[MarchingCasesC](/Cxx/VisualizationAlgorithms/MarchingCasesC) | Marching cubes, case 3 is rotated 90 degrees about the y-axis with no label.
[MarchingCasesD](/Cxx/VisualizationAlgorithms/MarchingCasesD) | Marching cubes. Case 7 is rotated 180 degrees about the y-axis with no label.
[MarchingCubes](/Cxx/Modelling/MarchingCubes) | Create a voxelized sphere.
[MarchingSquares](/Cxx/Modelling/MarchingSquares) | Create a contour from a structured point set (image).
[SampleFunction](/Cxx/ImplicitFunctions/SampleFunction) | Sample and visualize an implicit function.
[ShepardInterpolation](/Cxx/Visualization/ShepardInterpolation) | Interpolate scalar data.
[SmoothDiscreteMarchingCubes](/Cxx/Modelling/SmoothDiscreteMarchingCubes) | Generate smooth surfaces from labeled data.

## Working with 3D Data

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AlignFrames](/Cxx/PolyData/AlignFrames) | Align coordinate frames.
[AlignTwoPolyDatas](/Cxx/PolyData/AlignTwoPolyDatas) | Align two vtkPolyData's.
[AppendFilter](/Cxx/Filtering/AppendFilter) | Append different types of data,
[BooleanOperationPolyDataFilter](/Cxx/PolyData/BooleanOperationPolyDataFilter) | Perform boolean operations on two vtkPolyData objects.
[Bottle](/Cxx/Modelling/Bottle) | Rotationally symmetric objects.
[CappedSphere](/Cxx/Modelling/CappedSphere) | Rotate an arc to create a capped sphere.
[CellCenters](/Cxx/PolyData/CellCenters) | Compute points at the center of every cell.
[CellCentersDemo](/Cxx/PolyData/CellCentersDemo) | Visualize points at the center of every cell.
[CellEdgeNeighbors](/Cxx/PolyData/CellEdgeNeighbors) | Determine which cells share an edge with a specific cell.
[CellPointNeighbors](/Cxx/PolyData/CellPointNeighbors) | Determine which cells share a single point with a specific cell.
[CellsInsideObject](/Cxx/PolyData/CellsInsideObject) | Extract cells inside a closed surface.
[CenterOfMass](/Cxx/PolyData/CenterOfMass) | Compute the center of mass of the points.
[Circle](/Cxx/GeometricObjects/Circle) | Create a circle by cutting through a sphere.
[CleanPolyData](/Cxx/PolyData/CleanPolyData) | Remove coincident points.
[ColorCells](/Cxx/PolyData/ColorCells) | Color individual cells of a polydata with scalar index.
[ColorCellsWithRGB](/Cxx/PolyData/ColorCellsWithRGB) | Color individual cells of a polydata with rgb colors.
[ColoredPoints](/Cxx/PolyData/ColoredPoints) | Add three points to a polydata and associate a color with each of them.
[CombinePolyData](/Cxx/Filtering/CombinePolyData) | Combine/Append PolyData.
[ConnectivityFilter](/Cxx/Filtering/ConnectivityFilter) | Color any dataset type based on connectivity.
[ConnectivityFilterDemo](/Cxx/Filtering/ConnectivityFilterDemo) | Color any dataset type based on connectivity.
[ContoursFromPolyData](/Cxx/Filtering/ContoursFromPolyData) | Create contours from PolyData.
[ContoursToSurface](/Cxx/PolyData/ContoursToSurface) | Convert contours to a surface.
[ConvexHull](/Cxx/PolyData/ConvexHull) | Convex hull using vtkHull.
[ConvexHullShrinkWrap](/Cxx/PolyData/ConvexHullShrinkWrap) | Convex hull using shrink wrapping.
[CopyAllArrays](/Cxx/PolyData/CopyAllArrays) | Copy all arrays from one vtkPolyData to another.
[CurvaturesDemo](/Cxx/PolyData/CurvaturesDemo) | Demonstrates how to get the Gaussian and Mean curvatures of a surface.
[DataBounds](/Cxx/PolyData/DataBounds) | Get the minimum and maximum value in each dimension. (Axis aligned bounding box)
[DataSetSurfaceFilter](/Cxx/PolyData/DataSetSurfaceFilter) | Convert vtkUnstructuredGrid to vtkPolyData.
[DecimatePolyline](/Cxx/PolyData/DecimatePolyline) | Decimate polyline.
[DeleteCells](/Cxx/PolyData/DeleteCells) | Delete a cell from a vtkPolyData
[DeletePoint](/Cxx/PolyData/DeletePoint) |
[DetermineArrayDataTypes](/Cxx/PolyData/DetermineArrayDataTypes) | Determine data types of arrays.
[DistancePolyDataFilter](/Cxx/PolyData/DistancePolyDataFilter) | Compute the distance function from one vtkPolyData to another.
[EmbedPointsIntoVolume](/Cxx/PolyData/EmbedPointsIntoVolume) | Embed points into a volume.
[ExternalContour](/Cxx/PolyData/ExternalContour) | Get the External Contour from Polydata.
[ExtractCellsUsingPoints](/Cxx/PolyData/ExtractCellsUsingPoints) | Extract points but bring cells that are still complete with them.
[ExtractOutsideSurface](/Cxx/PolyData/ExtractOutsideSurface) | Extract the outer surface of a multiple surface polydata.
[ExtractPolyLinesFromPolyData](/Cxx/PolyData/ExtractPolyLinesFromPolyData) | Extract polylines from polydata.
[ExtractSelectedIds](/Cxx/PolyData/ExtractSelectedIds) | Extract selected points.
[ExtractSelection](/Cxx/PolyData/ExtractSelection) | Extract selection points.
[ExtractSelectionCells](/Cxx/PolyData/ExtractSelectionCells) | Extract selection cells.
[ExtractSelectionOriginalId](/Cxx/PolyData/ExtractSelectionOriginalId) | Extract selection and find correspondence between new and original Id.
[ExtractVisibleCells](/Cxx/Filtering/ExtractVisibleCells) | Extract and highlight visible cells.
[FieldData](/Cxx/PolyData/FieldData) | Add Global Miscellaneous Data (FieldData) to a Polydata.
[Finance](/Cxx/Modelling/Finance) | Visualization of multidimensional financial data. The gray/wireframe surface represents the total data population. The red surface represents data points delinquent on loan payment.
[FinanceFieldData](/Cxx/Modelling/FinanceFieldData) | Visualization of multidimensional financial data. The yellow surface represents the total data population. The red surface represents data points delinquent on loan payment.
[FindAllArrayNames](/Cxx/IO/FindAllArrayNames) | Get the names of all of the data arrays.
[FitSplineToCutterOutput](/Cxx/PolyData/FitSplineToCutterOutput) | Fit a spline to cutter output.
[GeometryFilter](/Cxx/PolyData/GeometryFilter) | Convert vtkUnstructuredGrid to vtkPolyData (another way).
[GetMiscCellData](/Cxx/PolyData/GetMiscCellData) | Get Miscellaneous Data from Cells in a Polydata.
[GetMiscPointData](/Cxx/PolyData/GetMiscPointData) | Get Miscellaneous Data from Points in a Polydata.
[GradientFilter](/Cxx/PolyData/GradientFilter) | Compute the gradient of a scalar field on a data set.
[ImplicitBoolean](/Cxx/Filtering/ImplicitBoolean) | Operations include intersection and union.
[ImplicitBooleanDemo](/Cxx/Filtering/ImplicitBooleanDemo) | Demo Union, Difference and Intersection.
[ImplicitModeller](/Cxx/PolyData/ImplicitModeller) | Compute the distance from an object to every point on a uniform grid.
[ImplicitPolyDataDistance](/Cxx/PolyData/ImplicitPolyDataDistance) | Compute the distance function in a space around a vtkPolyData.
[InterpolateMeshOnGrid](/Cxx/PolyData/InterpolateMeshOnGrid) | Interpolate a mesh over a grid.
[InterpolateTerrain](/Cxx/PolyData/InterpolateTerrain) | vtkProbeFilter Interpolate terrain.
[IntersectionPolyDataFilter](/Cxx/PolyData/IntersectionPolyDataFilter) | Compute the intersection of two vtkPolyData objects.
[IterateOverLines](/Cxx/PolyData/IterateOverLines) | Iterate through the lines of a PolyData.
[KMeansClustering](/Cxx/InfoVis/KMeansClustering) | KMeans Clustering
[KochanekSpline](/Cxx/PolyData/KochanekSpline) | Create an Kochanek spline on a set of points.
[KochanekSplineDemo](/Cxx/PolyData/KochanekSplineDemo) | Interactively change the parameters of the Kochanek spline.
[LinearExtrusion](/Cxx/PolyData/LinearExtrusion) | Extrude a shape.
[LoopBooleanPolyDataFilter](/Cxx/PolyData/LoopBooleanPolyDataFilter) | Perform boolean operations on two vtkPolyData objects.
[MaskPoints](/Cxx/PolyData/MaskPoints) | Select a subset (mask) of a point set.
[MergePoints](/Cxx/PolyData/MergePoints) | Remove duplicate (merge) points.
[MergeSelections](/Cxx/PolyData/MergeSelections) | Merge selected points.
[MiscCellData](/Cxx/PolyData/MiscCellData) | Add Miscellaneous Data to Cells in a Polydata.
[MiscPointData](/Cxx/PolyData/MiscPointData) | Add Miscellaneous Data to Points in a Polydata.
[MultiBlockMergeFilter](/Cxx/PolyData/MultiBlockMergeFilter) | Combine MultiBlockDataSets.
[NullPoint](/Cxx/PolyData/NullPoint) | Set everything in PointData at a specified index to NULL
[Outline](/Cxx/PolyData/Outline) | Draw the bounding box of the data
[PKMeansClustering](/Cxx/InfoVis/PKMeansClustering) | Parallel KMeans Clustering.
[ParametricSpline](/Cxx/PolyData/ParametricSpline) | Create a Cardinal spline on a set of points.
[PerlinNoise](/Cxx/Filtering/PerlinNoise) |
[PointCellIds](/Cxx/PolyData/PointCellIds) | Generate point and cell id arrays.
[PointInsideObject](/Cxx/PolyData/PointInsideObject) | Check if a point is inside an object.
[PointInsideObject2](/Cxx/PolyData/PointInsideObject2) | This uses a Delaunay triangulation to compute a volume. This gives more of an "is inside convex hull" effect than an "is inside object".
[PointsProjectedHull](/Cxx/PolyData/PointsProjectedHull) | Convex hull of points projected onto the coordinate planes.
[PolyDataCellNormals](/Cxx/PolyData/PolyDataCellNormals) | Add/Get Normals to/from cells in a Polydata.
[PolyDataContourToImageData](/Cxx/PolyData/PolyDataContourToImageData) | Generate a binarized image from a closed contour.
[PolyDataExtractNormals](/Cxx/PolyData/PolyDataExtractNormals) | Extract Normals from a Polydata.
[PolyDataGetPoint](/Cxx/PolyData/PolyDataGetPoint) | Get point locations/coordinates from a vtkPolyData.
[PolyDataIsoLines](/Cxx/PolyData/PolyDataIsoLines) | Iso lines on the surface of a polydata
[PolyDataPointNormals](/Cxx/PolyData/PolyDataPointNormals) | Add/Get Normals to/from points in a Polydata.
[PolyDataPointSampler](/Cxx/PolyData/PolyDataPointSampler) | Sample the edges or surfaces of a polydata.
[PolyDataToImageData](/Cxx/PolyData/PolyDataToImageData) | Generate a binarized volume from a closed surface.
[ProcrustesAlignmentFilter](/Cxx/PolyData/ProcrustesAlignmentFilter) | Align point sets.
[ProgrammableFilter](/Cxx/Filtering/ProgrammableFilter) | Create a custom filter without subclassing.
[ProgrammableSource](/Cxx/Filtering/ProgrammableSource) | Create points using a programmable source. Generates points for a strange attractor.
[ProjectSphere](/Cxx/Visualization/ProjectSphere) | Unroll a sphere or spherical-like model.
[QuantizePolyDataPoints](/Cxx/PolyData/QuantizePolyDataPoints) | Snap (quantize) points to a grid.
[Reflection](/Cxx/PolyData/Reflection) | Mirror a DataSet.
[RemoveOutsideSurface](/Cxx/PolyData/RemoveOutsideSurface) | Remove the outer surface of a multiple surface polydata.
[RemoveVertices](/Cxx/PolyData/RemoveVertices) |
[ResampleAppendedPolyData](/Cxx/PolyData/ResampleAppendedPolyData) | Resample a flat terrain containing multiple objects.
[ResamplePolyLine](/Cxx/PolyData/ResamplePolyLine) |
[ReverseSense](/Cxx/PolyData/ReverseSense) | Flip normals.
[RibbonFilter](/Cxx/PolyData/RibbonFilter) |
[RotationAroundLine](/Cxx/PolyData/RotationAroundLine) | Rotation around a line.
[RuledSurfaceFilter](/Cxx/PolyData/RuledSurfaceFilter) | Create a surface between lines.
[SCurveSpline](/Cxx/InfoVis/SCurveSpline) | Create an SCurve spline on a set of points.
[SelectVisiblePoints](/Cxx/PolyData/SelectVisiblePoints) | Select visible points.
[SelectionSource](/Cxx/Filtering/SelectionSource) | Specify a selection.
[ShrinkPolyData](/Cxx/PolyData/ShrinkPolyData) | Move all items in a PolyData towards their centroid.
[Silhouette](/Cxx/PolyData/Silhouette) |
[Spring](/Cxx/Modelling/Spring) | Rotation in combination with linear displacement and radius variation.
[Stripper](/Cxx/PolyData/Stripper) | Convert triangles to triangle strips.
[ThinPlateSplineTransform](/Cxx/PolyData/ThinPlateSplineTransform) |
[ThresholdCells](/Cxx/PolyData/ThresholdCells) | Thresholding Cells.
[ThresholdPoints](/Cxx/PolyData/ThresholdPoints) | Thresholding Points.
[TransformFilter](/Cxx/PolyData/TransformFilter) | Transform a data set.
[TransformOrderDemo](/Cxx/PolyData/TransformOrderDemo) | Demonstrate how the order of applying transforms affects the result.
[TransformPipeline](/Cxx/PolyData/TransformPipeline) | Combining the transformation, in order to manipulate a group of vtkActor. Shown in the example robot arm motion.
[TransformPolyData](/Cxx/Filtering/TransformPolyData) | Apply a Transformation to a PolyData.
[TriangleArea](/Cxx/PolyData/TriangleArea) | Compute the area of all triangles in a polydata.
[TriangleColoredPoints](/Cxx/PolyData/TriangleColoredPoints) | Set the color of each point of a triangle. You will be able to interpolate the colors across the triangle.
[TriangleSolidColor](/Cxx/PolyData/TriangleSolidColor) | Create a solid colored triangle.
[TubeFilter](/Cxx/PolyData/TubeFilter) | Give lines a thickness (produce a cylinder around lines.
[VectorFieldNonZeroExtraction](/Cxx/Filtering/VectorFieldNonZeroExtraction) | Extract non-zero vectors from a vtkImageData.
[VertexConnectivity](/Cxx/PolyData/VertexConnectivity) | Get a list of vertices attached (through an edge) to a vertex.
[WarpScalar](/Cxx/PolyData/WarpScalar) | Move vertices along normals.
[WarpSurface](/Cxx/PolyData/WarpSurface) | Warp a surface along its normal.
[WarpTo](/Cxx/Filtering/WarpTo) | Bend an object.
[WarpVector](/Cxx/PolyData/WarpVector) |

### Data Types

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[CompositePolyDataMapper](/Cxx/CompositeData/CompositePolyDataMapper) |
[Generate2DAMRDataSetWithPulse](/Cxx/CompositeData/Generate2DAMRDataSetWithPulse) | Generates sample 2-D AMR dataset.
[Generate3DAMRDataSetWithPulse](/Cxx/CompositeData/Generate3DAMRDataSetWithPulse) | Generates sample 3-D AMR dataset.
[MultiBlockDataSet](/Cxx/CompositeData/MultiBlockDataSet) | Demonstrates how to make and use VTK's MultiBlock type data
[OverlappingAMR](/Cxx/CompositeData/OverlappingAMR) | Demonstrates how to create and populate a VTK's Overlapping AMR Grid type Data

### Data Type Conversions

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[PolyDataToUnstructuredGrid](/Cxx/PolyData/PolyDataToUnstructuredGrid) | Convert a vtkPolyData to a vtkUnstructuredGrid.

### Point Cloud Operations

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ColorIsosurface](/Cxx/VisualizationAlgorithms/ColorIsosurface) | Color an isosurface with a data array.
[CompareExtractSurface](/Cxx/Points/CompareExtractSurface) | Compare three extract surface algorithms.
[DensifyPoints](/Cxx/Points/DensifyPoints) | Add points to a point cloud.
[DownsamplePointCloud](/Cxx/PolyData/DownsamplePointCloud) | Down sample a point cloud. Remove points so that there are no points within a tolerance of any point.
[ExtractClusters](/Cxx/Points/ExtractClusters) | From a set of randomly distributed spheres, extract connected clusters.
[ExtractEnclosedPoints](/Cxx/Points/ExtractEnclosedPoints) | Extract points inside a vtkPolyData surface.
[ExtractPointsDemo](/Cxx/Points/ExtractPointsDemo) | Extract points inside an implicit function.
[ExtractSurface](/Cxx/Points/ExtractSurface) | Create a surface from Unorganized Points using Point filters.
[ExtractSurfaceDemo](/Cxx/Points/ExtractSurfaceDemo) | Create a surface from Unorganized Points using Point filters (DEMO).
[FitImplicitFunction](/Cxx/Points/FitImplicitFunction) | Extract points within a distance to an implicit function.
[MaskPointsFilter](/Cxx/Points/MaskPointsFilter) | Extract points within an image mask.
[NormalEstimation](/Cxx/Points/NormalEstimation) | Estimate the normals of a random points that lie on a sphere.
[PointOccupancy](/Cxx/Points/PointOccupancy) | Show which voxels contain points.
[PointSource](/Cxx/PolyData/PointSource) | Generate a random point cloud.
[PoissonExtractSurface](/Cxx/Points/PoissonExtractSurface) | Create a surface from Unorganized Points using the PoissonReconstruction algorithm.
[PowercrustExtractSurface](/Cxx/Points/PowercrustExtractSurface) | Create a surface from Unorganized Points using the Powercrust algorithm.
[RadiusOutlierRemoval](/Cxx/Points/RadiusOutlierRemoval) | Remove outliers.
[SignedDistance](/Cxx/Points/SignedDistance) | Compute signed distance to a point cloud.
[UnsignedDistance](/Cxx/Points/UnsignedDistance) | Compute unsigned distance to a point cloud.

### Working with Meshes

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AddCell](/Cxx/Meshes/AddCell) | Add a cell to an existing mesh.
[BoundaryEdges](/Cxx/Meshes/BoundaryEdges) | Find the edges that are used by only one face.
[CellEdges](/Cxx/Meshes/CellEdges) | Get edges of cells.
[ClosedSurface](/Cxx/PolyData/ClosedSurface) | Check if a surface is closed.
[ColorDisconnectedRegions](/Cxx/PolyData/ColorDisconnectedRegions) | Color each disconnected region of a vtkPolyData a different color.
[ColorDisconnectedRegionsDemo](/Cxx/PolyData/ColorDisconnectedRegionsDemo) | Color each disconnected region of a vtkPolyData a different color for any vtkPolyData.
[ColoredElevationMap](/Cxx/Meshes/ColoredElevationMap) | Color a mesh by height.
[Curvatures](/Cxx/PolyData/Curvatures) | Compute Gaussian, Mean, Min, and Max Curvatures.
[Decimation](/Cxx/Meshes/Decimation) | Reduce the number of triangles in a mesh.
[DeformPointSet](/Cxx/Meshes/DeformPointSet) | Deform a point set with a control polyhedra.
[DelaunayMesh](/Cxx/Modelling/DelaunayMesh) | Two-dimensional Delaunay triangulation of a random set of points. Points and edges are shown highlighted with sphere glyphs and tubes.
[DijkstraGraphGeodesicPath](/Cxx/PolyData/DijkstraGraphGeodesicPath) | Find the shortest path between two points on a mesh.
[ElevationFilter](/Cxx/Meshes/ElevationFilter) | Color a mesh by height.
[ExtractEdges](/Cxx/Meshes/ExtractEdges) |
[FillHoles](/Cxx/Meshes/FillHoles) | Close holes in a mesh.
[FitToHeightMap](/Cxx/Meshes/FitToHeightMap) | Drape a polydata over an elevation map.
[GreedyTerrainDecimation](/Cxx/PolyData/GreedyTerrainDecimation) | Create a mesh from an ImageData
[HighlightBadCells](/Cxx/PolyData/HighlightBadCells) |
[IdentifyHoles](/Cxx/Meshes/IdentifyHoles) | Close holes in a mesh and identify the holes.
[ImplicitSelectionLoop](/Cxx/PolyData/ImplicitSelectionLoop) | Select a region of a mesh with an implicit function.
[InterpolateFieldDataDemo](/Cxx/Meshes/InterpolateFieldDataDemo) | Resample a fine grid and interpolate field data.
[LargestRegion](/Cxx/PolyData/PolyDataConnectivityFilter_LargestRegion) | Extract the largest connected region in a polydata.
[MatrixMathFilter](/Cxx/Meshes/MatrixMathFilter) | Compute various quantities on cell and points in a mesh.
[MeshQuality](/Cxx/PolyData/MeshQuality) |
[OBBDicer](/Cxx/Meshes/OBBDicer) | Breakup a mesh into pieces.
[PointInterpolator](/Cxx/Meshes/PointInterpolator) | Plot a scalar field of points onto a PolyData surface.
[PolygonalSurfaceContourLineInterpolator](/Cxx/PolyData/PolygonalSurfaceContourLineInterpolator) | Interactively find the shortest path between two points on a mesh.
[QuadricClustering](/Cxx/Meshes/QuadricClustering) | Reduce the number of triangles in a mesh.
[QuadricDecimation](/Cxx/Meshes/QuadricDecimation) | Reduce the number of triangles in a mesh.
[SelectPolyData](/Cxx/PolyData/SelectPolyData) | Select a region of a mesh.
[SimpleElevationFilter](/Cxx/Meshes/SimpleElevationFilter) | Color a mesh by dotting a vector from the origin to each point with a specified vector.
[SmoothPolyDataFilter](/Cxx/PolyData/SmoothPolyDataFilter) | Laplacian smoothing.
[SpecifiedRegion](/Cxx/PolyData/PolyDataConnectivityFilter_SpecifiedRegion) | Extract a specific (specified) connected region in a polydata.
[SplitPolyData](/Cxx/Meshes/SplitPolyData) | Breakup a mesh into pieces and save the pieces into files
[Subdivision](/Cxx/Meshes/Subdivision) | Increase the number of triangles in a mesh.
[SubdivisionDemo](/Cxx/Meshes/SubdivisionDemo) | Subdivision of any vtkPolyData
[Triangulate](/Cxx/Meshes/Triangulate) | Convert all polygons in a mesh to triangles.
[WeightedTransformFilter](/Cxx/PolyData/WeightedTransformFilter) |
[WindowedSincPolyDataFilter](/Cxx/Meshes/WindowedSincPolyDataFilter) | Smooth a mesh (windowed sinc filter).

#### Clipping

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[BoxClipStructuredPoints](/Cxx/Visualization/BoxClipStructuredPoints) | Clip vtkStructuredPoints with a box. The results are unstructured grids with tetrahedra.
[BoxClipUnstructuredGrid](/Cxx/Visualization/BoxClipUnstructuredGrid) | Clip a vtkUnstructuredGrid with a box. The results are unstructured grids with tetrahedra.
[CapClip](/Cxx/Meshes/CapClip) | Cap a clipped polydata with a polygon.
[ClipClosedSurface](/Cxx/Meshes/ClipClosedSurface) | Clip a surface with multiple planes.
[ClipDataSetWithPolyData](/Cxx/Meshes/ClipDataSetWithPolyData) | Clip a vtkRectilinearGrid with arbitrary polydata. In this example, use a vtkConeSource to generate polydata to slice the grid, resulting in an unstructured grid. Contrast this with the next example.
[ClipFrustum](/Cxx/Meshes/ClipFrustum) | Clip a vtkPolyData with the vtkCamera's view frustum.
[GenericClip](/Cxx/Filtering/GenericClip) |
[ImplicitDataSetClipping](/Cxx/PolyData/ImplicitDataSetClipping) | Clip using an implicit data set.
[SolidClip](/Cxx/Meshes/SolidClip) | Create a "solid" clip. The "ghost" of the part clipped away is also shown.
[TableBasedClipDataSetWithPolyData](/Cxx/Meshes/TableBasedClipDataSetWithPolyData) | Clip a vtkRectilinearGrid with arbitrary polydata. In this example, use a vtkConeSource to generate polydata to slice the grid, resulting in an unstructured grid. Contrast this with the previous example.
[TableBasedClipDataSetWithPolyData2](/Cxx/Meshes/TableBasedClipDataSetWithPolyData2) | Clip a vtkRectilinearGrid with a checkerboard pattern.

### Working with Structured 3D Data

This section includes vtkImageData vtkStructuredGrid and vtkRectilinearGrid.

"ImageData" is not the traditional "flat, 2D image" you are used to. It is a special VTK data structure in the collection of 3D data structures provided by VTK. [Here is an overview of these data structures](http://www.vtk.org/Wiki/VTK/Tutorials/3DDataTypes). Image data can represent at typical 2D image, but also, a 3D volume.

* [Demonstration of structured data types](/Cxx/Visualization/StructuredDataTypes)
* [Get the linear point id of a point in a grid](/Cxx/StructuredGrid/GetLinearPointId)

#### vtkImageData

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[CellIdFromGridCoordinates](/Cxx/ImageData/CellIdFromGridCoordinates) | Get the id of a cell from its grid coordinates.
[ClipVolume](/Cxx/ImageData/ClipVolume) | Clip a volumeand produce a vtkUnhstructuredGrid.
[ExtractVOI](/Cxx/ImageData/ExtractVOI) | Extract a volume of interest (subvolume).
[GetCellCenter](/Cxx/ImageData/GetCellCenter) | Get the coordinates of the center of a cell.
[ImageIterator](/Cxx/ImageData/ImageIterator) |
[ImageIteratorDemo](/Cxx/ImageData/ImageIteratorDemo) | Demonstrate using an iterator to access pixels in a region.
[ImageNormalize](/Cxx/ImageData/ImageNormalize) | Normalize an image.
[ImageReslice](/Cxx/ImageData/ImageReslice) | Resize a vtkImageData.
[ImageTranslateExtent](/Cxx/ImageData/ImageTranslateExtent) | Change the extent of a vtkImageData.
[ImageWeightedSum](/Cxx/ImageData/ImageWeightedSum) | Add two or more images.
[IntersectLine](/Cxx/ImageData/IntersectLine) | Intersect a line with all cells of a vtkImageData.
[IterateImageData](/Cxx/ImageData/IterateImageData) | Iterating over a vtkImageData.

#### Conversions

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ImageDataGeometryFilter](/Cxx/ImageData/ImageDataGeometryFilter) | Convert a vtkImageData to a vtkPolyData
[ImageDataToPointSet](/Cxx/ImageData/ImageDataToPointSet) | Convert a vtkImageData to a vtkStructuredGrid.

#### vtkStructuredGrid

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[BlankPoint](/Cxx/StructuredGrid/BlankPoint) | Blank a point of a vtkStructuredGrid.
[GetLinearPointId](/Cxx/StructuredGrid/GetLinearPointId) | Get the linear point id of a point in a grid.
[SGrid](/Cxx/StructuredGrid/SGrid) | Creating a structured grid dataset of a semi-cylinder. Vectors are created whose magnitude is proportional to radius and oriented in tangential direction.
[StructuredGrid](/Cxx/StructuredGrid/StructuredGrid) | Reads and writes points into a structured grid. The grid is also colored by point data and how to iterate through the structured grid is demonstrated.
[StructuredGridOutline](/Cxx/StructuredGrid/StructuredGridOutline) | Visualize the outline of a structured grid.
[VisualizeStructuredGrid](/Cxx/StructuredGrid/VisualizeStructuredGrid) | Visualize the points of a structured grid.
[VisualizeStructuredGridCells](/Cxx/StructuredGrid/VisualizeStructuredGridCells) | Visualize the cells of a structured grid.

#### vtkStructuredPoints

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[StructuredPointsToUnstructuredGrid](/Cxx/StructuredPoints/StructuredPointsToUnstructuredGrid) | Convert a vtkStructuredPoints to a vtkUnstructuredGrid.
[Vol](/Cxx/StructuredPoints/Vol) | Creating a image data dataset. Scalar data is generated from the equation for a sphere. Volume dimensions are 26 x 26 x 26.

#### vtkRectilinearGrid

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[RGrid](/Cxx/RectilinearGrid/RGrid) | Creating a rectilinear grid dataset. The coordinates along each axis are defined using an instance of vtkDataArray.
[RectilinearGrid](/Cxx/RectilinearGrid/RectilinearGrid) | Create a rectilinear grid.
[RectilinearGridToTetrahedra](/Cxx/RectilinearGrid/RectilinearGridToTetrahedra) | Convert a vtkRectilinearGrid to a vtkUnstructuredGrid mesh.
[VisualizeRectilinearGrid](/Cxx/RectilinearGrid/VisualizeRectilinearGrid) | Visualize the cells of a rectilinear grid.

### Working with Unstructured 3D Data

This section includes vtkUnstructuredGrid.

#### vtkUnstructuredGrid

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ClipUnstructuredGridWithPlane](/Cxx/UnstructuredGrid/ClipUnstructuredGridWithPlane) | Clip a UGrid with a plane.
[ClipUnstructuredGridWithPlane2](/Cxx/UnstructuredGrid/ClipUnstructuredGridWithPlane2) | Clip a UGrid with a plane.
[UGrid](/Cxx/UnstructuredGrid/UGrid) | Creation of an unstructured grid.

### Registration

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[IterativeClosestPointsTransform](/Cxx/Filtering/IterativeClosestPointsTransform) | Iterative Closest Points (ICP) Transform.
[LandmarkTransform](/Cxx/Filtering/LandmarkTransform) | Landmark Transform.

### Medical

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[GenerateCubesFromLabels](/Cxx/Medical/GenerateCubesFromLabels) | Create cubes from labeled volume data.
[GenerateModelsFromLabels](/Cxx/Medical/GenerateModelsFromLabels) | Create models from labeled volume data (vtkDiscreteMarchingCubes).
[MedicalDemo1](/Cxx/Medical/MedicalDemo1) | Create a skin surface from volume data
[MedicalDemo2](/Cxx/Medical/MedicalDemo2) | Create a skin and bone surface from volume data
[MedicalDemo3](/Cxx/Medical/MedicalDemo3) | Create skin, bone and slices from volume data
[MedicalDemo4](/Cxx/Medical/MedicalDemo4) | Create a volume rendering
[TissueLens](/Cxx/Medical/TissueLens) | Cut a volume with a sphere

### Surface reconstruction

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Delaunay3D](/Cxx/Modelling/Delaunay3D) | Create a solid mesh from Unorganized Points.
[Delaunay3DDemo](/Cxx/Modelling/Delaunay3DDemo) | Interactively adjust Alpha for Delaunay3D.
[ExtractSurface](/Cxx/Points/ExtractSurface) | Create a surface from Unorganized Points using Point filters.
[ExtractSurfaceDemo](/Cxx/Points/ExtractSurfaceDemo) | Create a surface from Unorganized Points using Point filters (DEMO).
[GaussianSplat](/Cxx/Filtering/GaussianSplat) | Create a surface from Unorganized Points (Gaussian Splat).
[SurfaceFromUnorganizedPoints](/Cxx/Filtering/SurfaceFromUnorganizedPoints) | Create a surface from Unorganized Points.
[SurfaceFromUnorganizedPointsWithPostProc](/Cxx/Filtering/SurfaceFromUnorganizedPointsWithPostProc) | Create a surface from Unorganized Points (with post processing).
[TriangulateTerrainMap](/Cxx/Filtering/TriangulateTerrainMap) | Generate heights (z values) on a 10x10 grid (a terrain map) and then triangulate the points to form a surface.

## Utilities

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[BoundingBox](/Cxx/Utilities/BoundingBox) | Bounding Box construction.
[BoundingBoxIntersection](/Cxx/Utilities/BoundingBoxIntersection) | Box intersection and Inside tests.
[Box](/Cxx/Utilities/Box) | Intersect a box with a ray.
[BrownianPoints](/Cxx/Utilities/BrownianPoints) | Produce a random vector at each point in a dataset.
[CardinalSpline](/Cxx/Utilities/CardinalSpline) | Cardinal spline Interpolation.
[Casting](/Cxx/PolyData/Casting) | Casting VTK objects.
[CheckVTKVersion](/Cxx/Utilities/CheckVTKVersion) | Check VTK Version.
[ColorLookupTable](/Cxx/Utilities/ColorLookupTable) | Color Lookup Table.
[ColorTransferFunction](/Cxx/Utilities/ColorTransferFunction) | Color Transfer Function.
[CommandSubclass](/Cxx/Utilities/CommandSubclass) | Instead of using a callback function, it is more powerful to subclass vtkCommand.
[ConstrainedDelaunay2D](/Cxx/Filtering/ConstrainedDelaunay2D) | Perform a 2D Delaunay triangulation on a point set respecting a specified boundary.
[Coordinate](/Cxx/Utilities/Coordinate) | Coordinate system conversions.
[DeepCopy](/Cxx/Utilities/DeepCopy) | Deep copy a VTK object.
[Delaunay2D](/Cxx/Filtering/Delaunay2D) | Perform a 2D Delaunay triangulation on a point set.
[DetermineActorType](/Cxx/Utilities/DetermineActorType) | Determine the type of an actor.
[DiscretizableColorTransferFunction](/Cxx/Utilities/DiscretizableColorTransferFunction) | Discretizable Color Transfer Function.
[ExtractFaces](/Cxx/Utilities/ExtractFaces) | Extract faces froam vtkUnstructuredGrid.
[FileOutputWindow](/Cxx/Utilities/FileOutputWindow) | Write errors to a log file  instead of the screen.
[FilenameFunctions](/Cxx/Utilities/FilenameFunctions) | Do things like get the file extension, strip the file extension, etc.
[FilterSelfProgress](/Cxx/Developers/FilterSelfProgress) | Monitor a filters progress.
[FrameRate](/Cxx/Utilities/FrameRate) | Get the frame rate of the rendering.
[FullScreen](/Cxx/Utilities/FullScreen) | Maximize/full screen a vtkRenderWindow.
[FunctionParser](/Cxx/Utilities/FunctionParser) | String function parser.
[GetClassName](/Cxx/Utilities/GetClassName) | Determine the type of a VTK variable.
[GetDataRoot](/Cxx/Utilities/GetDataRoot) | Find the path of VTK_DATA_ROOT.
[LUTUtilities](/Cxx/Utilities/LUTUtilities) | A utility class for vtkLookupTable allowing you to output the table contents or to compare tables.
[MassProperties](/Cxx/Utilities/MassProperties) | Compute volume and surface area of a closed, triangulated mesh.
[MultipleRenderWindows](/Cxx/Visualization/MultipleRenderWindows) | Multiple Render Windows
[MultipleViewports](/Cxx/Visualization/MultipleViewports) | Multiple Viewports.
[OffScreenRendering](/Cxx/Utilities/OffScreenRendering) | Off Screen Rendering.
[PCADemo](/Cxx/Utilities/PCADemo) | Project 2D points onto the best 1D subspace (PCA Demo).
[PCAStatistics](/Cxx/Utilities/PCAStatistics) | Compute Principal Component Analysis (PCA) values.
[PassThrough](/Cxx/InfoVis/PassThrough) | Pass input along to outpu.
[PiecewiseFunction](/Cxx/Utilities/PiecewiseFunction) | Interpolation using a piecewise function.
[PointInPolygon](/Cxx/Utilities/PointInPolygon) | Point inside polygon test.
[RenderScalarToFloatBuffer](/Cxx/Utilities/RenderScalarToFloatBuffer) | Demonstrates how to render scalars in a vtkPolyData object into a vtkFloatArray buffer for further processing.
[ReportRenderWindowCapabilities](/Cxx/Utilities/ReportRenderWindowCapabilities) | Report the capabilities of a render window.
[SaveSceneToFieldData](/Cxx/Utilities/SaveSceneToFieldData) | Save a vtkCamera's state in a vtkDataSet's vtkFieldData and restore it.
[SaveSceneToFile](/Cxx/Utilities/SaveSceneToFile) | Save a vtkCamera's state in a file and restore it.
[Screenshot](/Cxx/Utilities/Screenshot) |
[ShallowCopy](/Cxx/Utilities/ShallowCopy) | Shallow copy a VTK object.
[ShareCamera](/Cxx/Utilities/ShareCamera) | Share a camera between multiple renderers.
[ShepardMethod](/Cxx/Utilities/ShepardMethod) | Shepard method interpolation.
[SideBySideViewports](/Cxx/Visualization/SideBySideViewports) | Side by side viewports.
[TimeStamp](/Cxx/Utilities/TimeStamp) | Time stamp.
[Timer](/Cxx/Utilities/Timer) |
[TimerLog](/Cxx/Utilities/TimerLog) | Timer log.
[Variant](/Cxx/Utilities/Variant) | Convert between data types.
[Vector](/Cxx/Utilities/Vector) | Mathematical vector class.
[ViewportBorders](/Cxx/Utilities/ViewportBorders) | Draw a border around each viewport.
[VisualDebugging](/Cxx/Demos/VisualDebugging) | Update the screen from inside an algorithm.
[ZBuffer](/Cxx/Utilities/ZBuffer) | zbuffer.

### Arrays

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[2DArray](/Cxx/Utilities/2DArray) | 2D Array.
[3DArray](/Cxx/Utilities/3DArray) | 3D Array.
[ArrayCalculator](/Cxx/Utilities/ArrayCalculator) | Perform in-place operations on arrays.
[ArrayLookup](/Cxx/Utilities/ArrayLookup) | Find the location of a value in a vtkDataArray.
[ArrayRange](/Cxx/Utilities/ArrayRange) | Get the bounds (min,max) of a vtk array.
[ArrayToTable](/Cxx/InfoVis/ArrayToTable) | Convert a vtkDenseArray to a vtkTable.
[ArrayWriter](/Cxx/Utilities/ArrayWriter) | Write a DenseArray or SparseArray to a file.
[ConstructTable](/Cxx/Utilities/ConstructTable) | A table is a 2D array of any type of elements. They do not all have to be the same type. This is achieved using vtkVariant.
[CustomDenseArray](/Cxx/Utilities/CustomDenseArray) | Custom type Dense (2D) Array.
[DenseArrayRange](/Cxx/Utilities/DenseArrayRange) | Get the bounds of a vtkDenseArray.
[ExtractArrayComponent](/Cxx/Utilities/ExtractArrayComponent) | Extract a component of an array.
[KnownLengthArray](/Cxx/Utilities/KnownLengthArray) | Known Length Array.
[SortDataArray](/Cxx/Utilities/SortDataArray) | Reorder array values based on an ordering (key) array.
[SparseArray](/Cxx/Utilities/SparseArray) | Sparse Array.
[UnknownLengthArray](/Cxx/Utilities/UnknownLengthArray) | Unknown Length Array.
[VectorArrayKnownLength](/Cxx/Utilities/VectorArrayKnownLength) | Array of Vectors (Known Length).
[VectorArrayUnknownLength](/Cxx/Utilities/VectorArrayUnknownLength) | Array of Vectors (Unknown Length).

### Events

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[CameraModifiedEvent](/Cxx/Utilities/CameraModifiedEvent) | Catch the camera modified event.
[ObserveError](/Cxx/Utilities/ObserveError) | Catch errors and warnings.
[WindowModifiedEvent](/Cxx/Utilities/WindowModifiedEvent) | Catch the window modified event.

## CMake Techniques

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Check if a specific module is present](/Cxx/CMakeTechniques/CheckForModule) |

## Math Operations

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[1DTupleInterpolation](/Cxx/Math/1DTupleInterpolation) | A simple example demonstrating how functions defined by sparsely distributed supporting points can be interpolated at arbitrary positions.
[EigenSymmetric](/Cxx/Math/EigenSymmetric) | Compute eigenvalues and eigenvectors of a symmetric matrix.
[HomogeneousLeastSquares](/Cxx/Math/HomogeneousLeastSquares) | Homogeneous Least Squares.
[LUFactorization](/Cxx/Math/LUFactorization) | LU Factorization.
[LeastSquares](/Cxx/Math/LeastSquares) | Least Squares.
[MatrixInverse](/Cxx/Math/MatrixInverse) | Matrix inverse.
[MatrixTranspose](/Cxx/Math/MatrixTranspose) | Matrix transpose.
[NormalizeVector](/Cxx/Math/NormalizeVector) | Normalize a vector.
[PerpendicularVector](/Cxx/Math/PerpendicularVector) | Get a vector perpendicular to another vector.
[VectorDot](/Cxx/Math/VectorDot) |
[VectorNorm](/Cxx/Math/VectorNorm) | Get the lengths of an array of vectors.

## Graphs

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AdjacencyMatrixToEdgeTable](/Cxx/Graphs/AdjacencyMatrixToEdgeTable) | Convert an adjacency matrix to an edge table.
[AdjacentVertexIterator](/Cxx/Graphs/AdjacentVertexIterator) | Get all vertices connected to a specified vertex.
[BoostBreadthFirstSearchTree](/Cxx/Graphs/BoostBreadthFirstSearchTree) | Breadth first search tree. Can also be used to convert a graph to a tree.
[BreadthFirstDistance](/Cxx/Graphs/BreadthFirstDistance) | Distance from origin.
[ColorEdges](/Cxx/Graphs/ColorEdges) | Color edges.
[ColorVertexLabels](/Cxx/Graphs/ColorVertexLabels) | Set the color of vertex labels.
[ColorVerticesLookupTable](/Cxx/Graphs/ColorVerticesLookupTable) | Color vertices.
[ConnectedComponents](/Cxx/Graphs/ConnectedComponents) | Find connected components of a graph.
[ConstructGraph](/Cxx/Graphs/ConstructGraph) | Construct a graph.
[ConstructTree](/Cxx/Graphs/ConstructTree) | Construct a tree.
[DepthFirstSearchAnimation](/Cxx/Graphs/DepthFirstSearchAnimation) | Depth First Search Animation.
[DepthFirstSearchIterator](/Cxx/Graphs/DepthFirstSearchIterator) | Depth First Search iterator.
[EdgeListIterator](/Cxx/Graphs/EdgeListIterator) | Iterate over edges of a graph.
[EdgeWeights](/Cxx/Graphs/EdgeWeights) | Edge weights.
[GraphPoints](/Cxx/InfoVis/GraphPoints) | Manually set coordinates of vertices in a graph.
[GraphToPolyData](/Cxx/Graphs/GraphToPolyData) | Convert a graph to a PolyData.
[InEdgeIterator](/Cxx/Graphs/InEdgeIterator) | Iterate over edges incoming to a vertex.
[LabelVerticesAndEdges](/Cxx/Graphs/LabelVerticesAndEdges) | Label vertices and edges.
[MinimumSpanningTree](/Cxx/Graphs/MinimumSpanningTree) | Minimum spanning tree of a graph.
[MutableGraphHelper](/Cxx/InfoVis/MutableGraphHelper) | Create either a vtkMutableDirectedGraph or vtkMutableUndirectedGraph.
[NOVCAGraph](/Cxx/Graphs/NOVCAGraph) | Create a graph & visualize it in ParaView/VisIt.
[OutEdgeIterator](/Cxx/Graphs/OutEdgeIterator) | Iterate over edges outgoing from a vertex.
[RandomGraphSource](/Cxx/Graphs/RandomGraphSource) | Create a random graph.
[RemoveIsolatedVertices](/Cxx/Graphs/RemoveIsolatedVertices) | Remove vertices of degree 0 from a vtkGraph.
[ScaleVertices](/Cxx/Graphs/ScaleVertices) | Size/scale vertices based on a data array.
[SelectedVerticesAndEdges](/Cxx/Graphs/SelectedVerticesAndEdges) | Get a list of selected vertices and edges.
[SelectedVerticesAndEdgesObserver](/Cxx/Graphs/SelectedVerticesAndEdgesObserver) | Get a list of selected vertices and edges using an observer of AnnotationChangedEvent.
[ShortestPath](/Cxx/Graphs/ShortestPath) | Find the shortest path on a graph.
[SideBySideGraphs](/Cxx/Graphs/SideBySideGraphs) | Display two graphs side by side.
[TreeBFSIterator](/Cxx/Graphs/TreeBFSIterator) | Breadth First Search iterator.
[VertexSize](/Cxx/Graphs/VertexSize) | Set the size of vertices.
[VisualizeDirectedGraph](/Cxx/Graphs/VisualizeDirectedGraph) | Visualize a directed graph.
[VisualizeGraph](/Cxx/Graphs/VisualizeGraph) | Visualize a graph.

### Graph Conversions

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[DirectedGraphToMutableDirectedGraph](/Cxx/Graphs/DirectedGraphToMutableDirectedGraph) | vtkDirectedGraph to vtkMutableDirectedGraph.
[MutableDirectedGraphToDirectedGraph](/Cxx/Graphs/MutableDirectedGraphToDirectedGraph) | vtkMutableDirectedGraph to vtkDirectedGraph.
[TreeToMutableDirectedGraph](/Cxx/Graphs/TreeToMutableDirectedGraph) | vtkTree to vtkMutableDirectedGraph

## Data Structures

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AttachAttributes](/Cxx/PolyData/AttachAttributes) | Attach attributes to a VTK array
[CellLocator](/Cxx/PolyData/CellLocator) | Project a point onto a mesh. Closest point on a mesh.Efficient 3D cell query.
[CellLocatorVisualization](/Cxx/PolyData/CellLocatorVisualization) | Visualization of the tree of a vtkCellLocator.
[CellTreeLocator](/Cxx/PolyData/CellTreeLocator) | Points inside an object using vtkCellTreeLocator.
[PointLocator](/Cxx/PolyData/PointLocator) | Efficient 3D point query.
[PointLocatorRadius](/Cxx/PolyData/PointLocatorRadius) | Find all points within a radius of a specified point.
[PointLocatorVisualization](/Cxx/PolyData/PointLocatorVisualization) | Visualization of the tree of a vtkPointLocator.

### Timing Demonstrations

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[KDTreeTimingDemo](/Cxx/DataStructures/KDTreeTimingDemo) | Plot the runtime vs MaxLevel (doesn't seem correct)
[ModifiedBSPTreeTimingDemo](/Cxx/DataStructures/ModifiedBSPTreeTimingDemo) | Plot the runtime vs MaxLevel
[OBBTreeTimingDemo](/Cxx/DataStructures/OBBTreeTimingDemo) | Plot the runtime vs MaxLevel
[OctreeTimingDemo](/Cxx/DataStructures/OctreeTimingDemo) | Plot the runtime vs MaxPointsPerRegionOctree timing demo.

### KD-Tree

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[BuildLocatorFromKClosestPoints](/Cxx/DataStructures/BuildLocatorFromKClosestPoints) |
[ClosestNPoints](/Cxx/DataStructures/ClosestNPoints) | Find the closest N points to a query point.
[DataStructureComparison](/Cxx/DataStructures/DataStructureComparison) | Illustrates, side by side, the differences between several spatial data structures
[KDTreeAccessPoints](/Cxx/DataStructures/KDTreeAccessPoints) | Access the points of a KDTree.
[KDTreeFindPointsWithinRadius](/Cxx/DataStructures/KDTreeFindPointsWithinRadius) | Find points within a specified radius of  a query point.
[KDTreeFindPointsWithinRadiusDemo](/Cxx/DataStructures/KDTreeFindPointsWithinRadiusDemo) | Find points within a range of radii.
[KdTree](/Cxx/DataStructures/KdTree) |
[KdTreePointLocatorClosestPoint](/Cxx/DataStructures/KdTreePointLocatorClosestPoint) | Find the closest point to a query point.
[OctreeFindPointsWithinRadiusDemo](/Cxx/DataStructures/OctreeFindPointsWithinRadiusDemo) | Find points within a range of radii.
[PointLocatorFindPointsWithinRadiusDemo](/Cxx/DataStructures/PointLocatorFindPointsWithinRadiusDemo) | Find points within a range of radii.
[StaticLocatorFindPointsWithinRadiusDemo](/Cxx/DataStructures/StaticLocatorFindPointsWithinRadiusDemo) | Find points within a range of radii.
[VisualizeKDTree](/Cxx/DataStructures/VisualizeKDTree) | Visualize levels of the tree.

### Oriented Bounding Box (OBB) Tree

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[OBBTreeExtractCells](/Cxx/DataStructures/OBBTreeExtractCells) | Intersect a line with an OBB Tree and display all intersected cells.
[OBBTreeIntersectWithLine](/Cxx/DataStructures/OBBTreeIntersectWithLine) | Intersect a line with a vtkOBBTree.
[VisualizeOBBTree](/Cxx/DataStructures/VisualizeOBBTree) | Visualize levels of the tree.

### Octree

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[BuildOctree](/Cxx/DataStructures/BuildOctree) | Create an octree.
[IncrementalOctreePointLocator](/Cxx/DataStructures/IncrementalOctreePointLocator) | Insert points into an octree without rebuilding it.
[OctreeClosestPoint](/Cxx/DataStructures/OctreeClosestPoint) | Find the closest point to a query point.
[OctreeFindPointsWithinRadius](/Cxx/DataStructures/OctreeFindPointsWithinRadius) | Find the points within a sphere of specified radius to a query point.
[OctreeKClosestPoints](/Cxx/DataStructures/OctreeKClosestPoints) | Find the K closest points to a query point.
[OctreeVisualize](/Cxx/DataStructures/OctreeVisualize) | Visualize levels of the tree.

### Modified BSP Tree

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ModifiedBSPTreeExtractCells](/Cxx/DataStructures/ModifiedBSPTreeExtractCells) | Intersect a line with a modified BSP Tree and display all intersected cells.
[ModifiedBSPTreeIntersectWithLine](/Cxx/DataStructures/ModifiedBSPTreeIntersectWithLine) | Intersect a line with a modified BSP Tree.
[VisualizeModifiedBSPTree](/Cxx/DataStructures/VisualizeModifiedBSPTree) | Visualize levels of the tree.

### HyperTreeGrid

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[HyperTreeGridSource](/Cxx/HyperTreeGrid/HyperTreeGridSource) | Create a vtkHyperTreeGrid.

## VTK Concepts

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Scalars](/Cxx/VTKConcepts/Scalars) | Attach a scalar value to each point (PointData) or cell (CellData) in a data set.

## Rendering

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AmbientSpheres](/Cxx/Rendering/AmbientSpheres) | Demonstrates the effect of ambient lighting on spheres.
[CameraBlur](/Cxx/Rendering/CameraBlur) | Example of a scene rendered with focal depth.
[ColoredSphere](/Cxx/Rendering/ColoredSphere) | A simple sphere.
[Cone3](/Cxx/Rendering/Cone3) | Four frames of output, based on the VTK example Cone3.cxx.
[Cone4](/Cxx/Rendering/Cone4) | Modifying properties and transformation matrix based on the VTK example Cone4.cxx.
[DiffuseSpheres](/Cxx/Rendering/DiffuseSpheres) | Demonstrates the effect of diffuse lighting on spheres.
[FlatVersusGouraud](/Cxx/Rendering/FlatVersusGouraud) | Flat and Gouraud shading. Different shading methods can dramatically improve the look of an object represented with polygons. On the top, flat shading uses a constant surface normal across each polygon. On the bottom, Gouraud shading interpolates normals from polygon vertices to give a smoother look.
[HiddenLineRemoval](/Cxx/Rendering/HiddenLineRemoval) | Hidden lines removed.
[Mace](/Cxx/Rendering/Mace) | An example of multiple inputs and outputs.
[Model](/Cxx/Rendering/Model) | Illustrative diagram of graphics objects.
[MotionBlur](/Cxx/Rendering/MotionBlur) | Example of motion blur.
[OutlineGlowPass](/Cxx/Rendering/OutlineGlowPass) | Demonstrates how to render a object in a scene with a glowing outline.
[PhysicallyBasedRendering](/Cxx/Rendering/PhysicallyBasedRendering) | Demonstrates physically based rendering, image based lighting, texturing and a skybox.
[Rainbow](/Cxx/Rendering/Rainbow) | Use and manipulation of vtkLookupTables.
[Rotations](/Cxx/Rendering/Rotations) | Rotations of a cow about her axes.
[RotationsA](/Cxx/Rendering/RotationsA) | Perform six rotations of a cow about her x-axis (Figure 3-31a).
[RotationsB](/Cxx/Rendering/RotationsB) | Perform six rotations of a cow about her y-axis (Figure 3-31b).
[RotationsC](/Cxx/Rendering/RotationsC) | Perform six rotations of a cow about her z-axis (Figure 3-31c).
[RotationsD](/Cxx/Rendering/RotationsD) | First a rotation of a cow about her x-axis, then six rotations about her y-axis (Figure 3-31d).
[Shadows](/Cxx/Rendering/Shadows) | Draw model with shadows.
[Skybox_PBR](/Cxx/Rendering/Skybox_PBR) | Demonstrates physically based rendering, image based lighting and a skybox.
[SpecularSpheres](/Cxx/Rendering/SpecularSpheres) | Demonstrates the effect of specular lighting on spheres.
[StippledLine](/Cxx/Rendering/StippledLine) | Draw a stippled line.
[StringToImageDemo](/Cxx/Rendering/StringToImageDemo) | Demonstrates how to generate images from strings.
[StripFran](/Cxx/Rendering/StripFran) | Triangle strip examples. (a) Structured triangle mesh consisting of 134 strips each of 390 triangles. (b) Unstructured triangle mesh consisting of 2227 strips of average length 3.94, longest strip 101 triangles. Images are generated by displaying every other triangle strip.
[TransformSphere](/Cxx/Rendering/TransformSphere) | The addition of a transform filter to [ColoredSphere](/Cxx/Rendering/ColoredSphere).
[TransparentBackground](/Cxx/Rendering/TransparentBackground) | Demonstrates the use of two renderers. Notice that the second (and subsequent) renderers will have a transparent background.
[WalkCow](/Cxx/Rendering/WalkCow) | This generates Figs. 3-32, 3-33 found in VTKTextbook.pdf.
[WalkCowA](/Cxx/Rendering/WalkCowA) | This generates Fig. 3-33a found in VTKTextbook.pdf.
[WalkCowB](/Cxx/Rendering/WalkCowB) | This generates Fig. 3-33b found in VTKTextbook.pdf.

## Lighting

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Light](/Cxx/Lighting/Light) | Add a directional light to a scene.
[LightActor](/Cxx/Lighting/LightActor) | Display the location and direction of a light.
[ShadowsLightsDemo](/Cxx/Visualization/ShadowsLightsDemo) | Show lights casting shadows.
[SpotLights](/Cxx/Lighting/SpotLights) | Create two positional(spot) lights.

## Shaders

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[BozoShader](/Cxx/Shaders/BozoShader) | Modify the shader to load and use noise functions for color selection.
[BozoShaderDemo](/Cxx/Shaders/BozoShaderDemo) | Show the bozo shader on a variety of polydata models.
[ColorByNormal](/Cxx/Shaders/ColorByNormal) | Modify the shader to color based on model normal.
[CubeMap](/Cxx/Shaders/CubeMap) | Modify the Vertex and Frament shaders to apply a 6-sided cube map.
[MarbleShader](/Cxx/Shaders/MarbleShader) | Modify the shader to load and use noise functions for color selection.
[MarbleShaderDemo](/Cxx/Shaders/MarbleShaderDemo) | Explore parameter space with sliders.
[SpatterShader](/Cxx/Shaders/SpatterShader) | Modify the shader to load and use noise functions for color selection.
[SphereMap](/Cxx/Shaders/SphereMap) | Modify the Vertex and Frament shaders to apply a spherical map.

## Annotation

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[MultiLineText](/Cxx/Annotation/MultiLineText) | Display multiline text.
[TextOrigin](/Cxx/Annotation/TextOrigin) | The 3D text always faces the active camera.
[XYPlot](/Cxx/Annotation/XYPlot) | Display line probes.

## Texture Mapping

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AnimateVectors](/Cxx/Texture/AnimateVectors) | One frame from a vector field animation using texture maps ([animVectors.tcl](https://gitlab.kitware.com/vtk/vtk/blob/da314198bb8fbdab6f59e8a69b2d97f1611a05bd/graphics/examplesTcl/animVectors.tcl)).
[BackgroundTexture](/Cxx/Visualization/BackgroundTexture) | Use a texture for the background of a vtkRenderer.
[ClipArt](/Cxx/Visualization/ClipArt) | Generate 3D clip art from an image.
[ProjectedTexture](/Cxx/Texture/ProjectedTexture) | Project a texture onto ovtkPolyData.
[TextureCutQuadric](/Cxx/Texture/TextureCutQuadric) | Cut a quadric with boolean textures.
[TextureCutSphere](/Cxx/Texture/TextureCutSphere) | Cut a sphere using texture coordinates.
[TextureMapImageData](/Cxx/Visualization/TextureMapImageData) | Texture map an ImageData.
[TextureMapPlane](/Cxx/Visualization/TextureMapPlane) | Texture map a plane.
[TextureMapQuad](/Cxx/Visualization/TextureMapQuad) | Texture map a quad.
[TexturePlane](/Cxx/Texture/TexturePlane) | Example of texture mapping.
[TextureThreshold](/Cxx/Texture/TextureThreshold) | Demonstrate the use of scalar thresholds to show values of flow density on three planes.
[TexturedSphere](/Cxx/Texture/TexturedSphere) | Texture a sphere.

## Tutorial

If you are new to VTK then these tutorials will help to get you started.

| Tutorial | Description | Image |
| -------------- | ------------- | ------- |
[Step 1](/Cxx/Tutorial/Tutorial_Step1) | Create a cone, render it and rotate it through 360°.
[Step 2](/Cxx/Tutorial/Tutorial_Step2) | Adding an observer.
[Step 3](/Cxx/Tutorial/Tutorial_Step3) | Using multiple renderers within a render window.
[Step 4](/Cxx/Tutorial/Tutorial_Step4) | The creation of multiple actors and the manipulation of their properties and transformations.
[Step 5](/Cxx/Tutorial/Tutorial_Step5) | Introducing the concept of interaction.
[Step 6](/Cxx/Tutorial/Tutorial_Step6) | Using a 3D widget.

## Visualization

See [this tutorial](http://www.vtk.org/Wiki/VTK/Tutorials/3DDataTypes) for a brief explanation of the VTK terminology of mappers, actors, etc.

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AlphaFrequency](/Cxx/Visualization/AlphaFrequency) | Linearly extrude fonts to show letter frequencies in text.
[AnatomicalOrientation](/Cxx/VisualizationAlgorithms/AnatomicalOrientation) | Show a labelled set of anatomical planes transecting a human figure.
[AnnotatedCubeActor](/Cxx/Visualization/AnnotatedCubeActor) | Annotated cube.
[Arbitrary3DCursor](/Cxx/Visualization/Arbitrary3DCursor) | Track a 3D cursor.
[AssignCellColorsFromLUT](/Cxx/Visualization/AssignCellColorsFromLUT) | Demonstrates how to assign colors to cells in a vtkPolyData structure using lookup tables.
[AxisActor](/Cxx/Visualization/AxisActor) | Generate a single axis.
[BackfaceCulling](/Cxx/Visualization/BackfaceCulling) | Backface culling.
[BackgroundColor](/Cxx/Visualization/BackgroundColor) | Background color.
[BackgroundGradient](/Cxx/Visualization/BackgroundGradient) | Background gradient.
[BillboardTextActor3D](/Cxx/Visualization/BillboardTextActor3D) | Label points with billboards.
[BlobbyLogo](/Cxx/Visualization/BlobbyLogo) | Blobby logo from VTK textbook.
[Blow](/Cxx/Visualization/Blow) | Ten frames from a blow molding finite element analysis.
[BluntStreamlines](/Cxx/VisualizationAlgorithms/BluntStreamlines) | Demonstrates airflow around a blunt fin using streamlines.
[Camera](/Cxx/Visualization/Camera) | Positioning and aiming the camera.
[CameraActor](/Cxx/Visualization/CameraActor) | Visualize a camera (frustum) in a scene.
[CameraModel1](/Cxx/Visualization/CameraModel1) | Illustrate camera movement around the focal point.
[CameraModel2](/Cxx/Visualization/CameraModel2) | Illustrate camera movement centered at the camera position.
[CaptionActor2D](/Cxx/Visualization/CaptionActor2D) | Draw a caption/bubble pointing to a particular point.
[CarotidFlow](/Cxx/VisualizationAlgorithms/CarotidFlow) | Visualizing blood flow in the human carotid arteries. Streamtubes of flow velocity are generated.
[CarotidFlowGlyphs](/Cxx/VisualizationAlgorithms/CarotidFlowGlyphs) | Visualizing blood flow in human carotid arteries. Cone glyphs indicate flow direction and magnitude.
[ChooseTextColor](/Cxx/Visualization/ChooseTextColor) | Choose a text color that contrasts with a background color.
[ChooseTextColorDemo](/Cxx/Visualization/ChooseTextColorDemo) | Create a grid of random colored viewpports and pick a good text color.
[ClipSphereCylinder](/Cxx/VisualizationAlgorithms/ClipSphereCylinder) | A plane clipped with a sphere and an ellipse. The two transforms place each implicit function into the appropriate position. Two outputs are generated by the clipper.
[CloseWindow](/Cxx/Visualization/CloseWindow) | Close a render window.
[CollisionDetection](/Cxx/Visualization/CollisionDetection) | Collison between two spheres.
[ColorActorEdges](/Cxx/Visualization/ColorActorEdges) | Color the edges of an Actor.
[ColorAnActor](/Cxx/Visualization/ColorAnActor) | Color an Actor.
[ColorGlyphs](/Cxx/Visualization/ColorGlyphs) | Color glyphs.
[ColorSeriesPatches](/Cxx/Visualization/ColorSeriesPatches) | Creates a HTML file called [VTKColorSeriesPatches](https://htmlpreview.github.io/?https://github.com/Kitware/vtk-examples/blob/gh-pages/VTKColorSeriesPatches.html)
[ColoredAnnotatedCube](/Cxx/VisualizationAlgorithms/ColoredAnnotatedCube) | How to color the individual faces of an annotated cube.
[CombustorIsosurface](/Cxx/VisualizationAlgorithms/CombustorIsosurface) | Generate an isosurface of constant flow density.
[ComplexV](/Cxx/Visualization/ComplexV) | ComplexV from the VTK Textbook.
[ContourQuadric](/Cxx/VisualizationAlgorithms/ContourQuadric) | Contouring a quadric function.
[CornerAnnotation](/Cxx/Visualization/CornerAnnotation) | Write text in the corners of a window.
[CorrectlyRenderTranslucentGeometry](/Cxx/Visualization/CorrectlyRenderTranslucentGeometry) | Correctly Rendering Translucent Geometry.
[CreateBFont](/Cxx/VisualizationAlgorithms/CreateBFont) | A scanned image clipped with a scalar value of 1/2 its maximum intensity produces a mixture of quadrilaterals and triangles.
[CreateColorSeriesDemo](/Cxx/Visualization/CreateColorSeriesDemo) | Create a custom vtkColorSeries.
[CubeAxesActor](/Cxx/Visualization/CubeAxesActor) | Display three orthogonal axes with labels.
[CubeAxesActor2D](/Cxx/Visualization/CubeAxesActor2D) | This example uses the vtkCubeAxesActor2D to show your scene with axes to indicate the spatial extent of your data.
[Cursor3D](/Cxx/Visualization/Cursor3D) |
[CursorShape](/Cxx/Visualization/CursorShape) | Change the shape of the cursor.
[CurvatureBandsWithGlyphs](/Cxx/Visualization/CurvatureBandsWithGlyphs) | Demonstrates the coloring of a surface by partitioning the gaussian curvature of a surface into bands and using arrows to display the normals on the surface.
[CurvedReformation](/Cxx/Visualization/CurvedReformation) | Sample a volume with a curved surface. In medical imaging, this is often called curved multi planar reformation.
[CutStructuredGrid](/Cxx/VisualizationAlgorithms/CutStructuredGrid) | Cut through structured grid with plane. The cut plane is shown solid shaded. A computational plane of constant k value is shown in wireframe for comparison. The colors correspond to flow density. Cutting surfaces are not necessarily planes: implicit functions such as spheres, cylinders, and quadrics can also be used.
[Cutter](/Cxx/VisualizationAlgorithms/Cutter) | How to use vtkCutter by cutting through a cube.
[DataSetSurface](/Cxx/VisualizationAlgorithms/DataSetSurface) | Cutting a hexahedron with a plane. The red line on the surface shows the cut.
[DecimateFran](/Cxx/VisualizationAlgorithms/DecimateFran) | Examples of decimation algorithm. (a) Decimation of laser digitizer data.
[DecimateHawaii](/Cxx/VisualizationAlgorithms/DecimateHawaii) | Examples of decimation algorithm. (b) Decimation of terrain data.
[DepthSortPolyData](/Cxx/Visualization/DepthSortPolyData) | Poly Data Depth Sorting.
[DisplacementPlot](/Cxx/VisualizationAlgorithms/DisplacementPlot) | Show modal lines for a vibrating beam.
[DisplayCoordinateAxes](/Cxx/Visualization/DisplayCoordinateAxes) | Display coordinate axes.
[DisplayQuadricSurfaces](/Cxx/Visualization/DisplayQuadricSurfaces) | Display Quadric Surfaces.
[DistanceToCamera](/Cxx/Visualization/DistanceToCamera) |
[DrawText](/Cxx/Visualization/DrawText) | Display Text.
[EdgePoints](/Cxx/Visualization/EdgePoints) | Generate points along an edge.
[ElevationBandsWithGlyphs](/Cxx/Visualization/ElevationBandsWithGlyphs) | Demonstrates the coloring of a surface by partitioning the elevation into bands and using arrows to display the normals on the surface.
[ExponentialCosine](/Cxx/VisualizationAlgorithms/ExponentialCosine) | Carpet plots. Visualization of an exponential cosine function. Function values are indicated by surface displacement. Colors indicate derivative values.
[ExtrudePolyDataAlongLine](/Cxx/Visualization/ExtrudePolyDataAlongLine) | Extrude a 2D polydata along a line in 3D space.
[FastSplatter](/Cxx/Visualization/FastSplatter) | Convolve a splat image with every point in an input image.
[FindCellIntersections](/Cxx/VisualizationAlgorithms/FindCellIntersections) | Find the intersection of a line and the cells in an unstructured dataset
[FireFlow](/Cxx/VisualizationAlgorithms/FireFlow) | Combine geometry from a VRML file and a solution from a vtkUnstructuredGrid file.
[FireFlowDemo](/Cxx/VisualizationAlgorithms/FireFlowDemo) | Combine geometry from a VRML file and a solution from a vtkUnstructuredGrid file (interactive).
[FlatShading](/Cxx/Visualization/FlatShading) |
[FlyingHeadSlice](/Cxx/VisualizationAlgorithms/FlyingHeadSlice) | Flying edges used to generate contour lines.
[Follower](/Cxx/Visualization/Follower) | Draw text that stays right side up.
[FontFile](/Cxx/Visualization/FontFile) | Use an external font.
[FrogBrain](/Cxx/Visualization/FrogBrain) | The frog’s brain. Model extracted without smoothing (left) and with smoothing (right).
[FrogSlice](/Cxx/Visualization/FrogSlice) | Photographic slice of frog (upper left), segmented frog (upper right) and composite of photo and segmentation (bottom). The purple color represents the stomach and the kidneys are yellow.
[Glyph2D](/Cxx/Filtering/Glyph2D) |
[Glyph3D](/Cxx/Filtering/Glyph3D) |
[Glyph3DImage](/Cxx/Visualization/Glyph3DImage) | Glyph the points in a vtkImageData.
[Glyph3DMapper](/Cxx/Visualization/Glyph3DMapper) |
[Hanoi](/Cxx/Visualization/Hanoi) | Towers of Hanoi.
[HanoiInitial](/Cxx/Visualization/HanoiInitial) | Towers of Hanoi - Initial configuration.
[HanoiIntermediate](/Cxx/Visualization/HanoiIntermediate) | Towers of Hanoi - Intermediate configuration.
[Hawaii](/Cxx/Visualization/Hawaii) | Visualize elevations by coloring the scalar values with a lookup table.
[HeadBone](/Cxx/VisualizationAlgorithms/HeadBone) | Marching cubes surface of human bone.
[HeadSlice](/Cxx/VisualizationAlgorithms/HeadSlice) | Marching squares used to generate contour lines.
[HedgeHog](/Cxx/Visualization/HedgeHog) | Create oriented lines (hedgehogs) from vector data.
[HideActor](/Cxx/Visualization/HideActor) | visible
[HideAllActors](/Cxx/Visualization/HideAllActors) | Hide all actors.
[HyperStreamline](/Cxx/VisualizationAlgorithms/HyperStreamline) | Example of hyperstreamlines, the four hyperstreamlines shown are integrated along the minor principle stress axis. A plane (colored with a different lookup table) is also shown.
[IronIsoSurface](/Cxx/VisualizationAlgorithms/IronIsoSurface) | Marching cubes surface of iron-protein.
[IsosurfaceSampling](/Cxx/Visualization/IsosurfaceSampling) | Demonstrates how to create point data on an isosurface.
[Kitchen](/Cxx/Visualization/Kitchen) | Demonstrates stream tracing in a kitchen.
[KochSnowflake](/Cxx/Visualization/KochSnowflake) | Use recursion to represent a Koch snowflake fractal.
[LODProp3D](/Cxx/Visualization/LODProp3D) | Level of detail rendering.
[LOx](/Cxx/VisualizationAlgorithms/LOx) | Streamtubes created by using the computational grid just in front of the post as a source for seeds.
[LOxGrid](/Cxx/VisualizationAlgorithms/LOxGrid) | Portion of computational grid for the LOx post.
[LOxSeeds](/Cxx/VisualizationAlgorithms/LOxSeeds) | Streamlines seeded with spherical cloud of points. Four separate cloud positions are shown.
[LabelContours](/Cxx/Visualization/LabelContours) | Label Contours.
[LabelPlacementMapper](/Cxx/Visualization/LabelPlacementMapper) | Display a non-overlapping text label at each point.
[LabeledDataMapper](/Cxx/Visualization/LabeledDataMapper) | Display the point ID at each point.
[LabeledMesh](/Cxx/Visualization/LabeledMesh) | Label Mesh.
[Legend](/Cxx/Visualization/Legend) |
[LegendScaleActor](/Cxx/Visualization/LegendScaleActor) | Display the scale of a scene.
[LineWidth](/Cxx/Visualization/LineWidth) | Change the width/thickness of lines in an actor.
[LoopShrink](/Cxx/Visualization/LoopShrink) | A network with a loop. VTK 5.0 does not allow you to execute a looping visualization network; this was possible in previous versions of VTK.
[Morph3D](/Cxx/Visualization/Morph3D) | Interpolate between datasets.
[Motor](/Cxx/VisualizationAlgorithms/Motor) | Texture cut used to reveal internal structure of a motor. Two cut planes are used in combination with transparent texture.
[MovableAxes](/Cxx/Visualization/MovableAxes) | Movable axes.
[MoveActor](/Cxx/Visualization/MoveActor) | Moving an Actor.
[MoveCamera](/Cxx/Visualization/MoveCamera) | Moving the Camera.
[MultipleActors](/Cxx/Visualization/MultipleActors) | Multiple Actors.
[NamedColorPatches](/Cxx/Visualization/NamedColorPatches) | Creates a HTML file called [VTKNamedColorPatches](https://htmlpreview.github.io/?https://github.com/Kitware/vtk-examples/blob/gh-pages/VTKNamedColorPatches.html)
[NamedColors](/Cxx/Visualization/NamedColors) | Demonstrate the use of the vtkNamedColors class.
[NoShading](/Cxx/Visualization/NoShading) |
[NormalsDemo](/Cxx/Visualization/NormalsDemo) | Demo different options to generate normals.
[Office](/Cxx/VisualizationAlgorithms/Office) | Using random point seeds to create streamlines.
[OfficeA](/Cxx/VisualizationAlgorithms/OfficeA) | Corresponds to Fig 9-47(a) in the VTK textbook.
[OfficeTube](/Cxx/VisualizationAlgorithms/OfficeTube) | The stream polygon. Sweeping a polygon to form a tube.
[Opacity](/Cxx/Visualization/Opacity) | Transparency, transparent.
[OrientedGlyphs](/Cxx/Visualization/OrientedGlyphs) | Create oriented glyphs from vector data.
[PineRootConnectivity](/Cxx/VisualizationAlgorithms/PineRootConnectivity) | Applying the connectivity filter to remove noisy isosurfaces.
[PineRootConnectivityA](/Cxx/VisualizationAlgorithms/PineRootConnectivityA) | The isosurface, with no connectivity filter applied.
[PineRootDecimation](/Cxx/VisualizationAlgorithms/PineRootDecimation) | Applying the decimation and connectivity filters to remove noisy isosurfaces and reduce data size.
[PlateVibration](/Cxx/VisualizationAlgorithms/PlateVibration) | Demonstrates the motion of a vibrating beam.
[PointDataSubdivision](/Cxx/Visualization/PointDataSubdivision) | Demonstrates the effect of applying these filters on various sources.
[PointSize](/Cxx/Visualization/PointSize) |
[ProbeCombustor](/Cxx/VisualizationAlgorithms/ProbeCombustor) | Probing data in a combustor.  Probes are regular arrays of 50 by 50 points that are then passed through a contouring filter.
[ProgrammableGlyphFilter](/Cxx/Visualization/ProgrammableGlyphFilter) | Generate a custom glyph at each point.
[ProteinRibbons](/Cxx/Visualization/ProteinRibbons) | Display pdb ribbons.
[PseudoVolumeRendering](/Cxx/VolumeRendering/PseudoVolumeRendering) | Here we use 100 cut planes, each with an opacity of 0.05. They are then rendered back-to-front to simulate volume rendering.
[QuadraticSurface](/Cxx/Visualization/QuadraticSurface) | Display a quadratic surface.
[QuadricLODActor](/Cxx/Visualization/QuadricLODActor) | Level of detail adjustment.
[QuadricVisualization](/Cxx/Visualization/QuadricVisualization) | Visualizing a quadric function.
[RandomProbe](/Cxx/Visualization/RandomProbe) | Demonstrates how to probe a dataset with random points and select points inside the data set.
[RenderLargeImage](/Cxx/Visualization/RenderLargeImage) | Render a large image, larger than a window.
[RenderView](/Cxx/Views/RenderView) | A little bit easier rendering.
[ReverseAccess](/Cxx/Visualization/ReverseAccess) | Demonstrates how to access the source (e.g. vtkSphereSource) of an actor reversely.
[RotateActor](/Cxx/Visualization/RotateActor) | Rotate an Actor.
[ScalarBarActor](/Cxx/Visualization/ScalarBarActor) | Display a color bar.
[ScalarBarActorColorSeries](/Cxx/Visualization/ScalarBarActorColorSeries) | Display a color bar with a color series lookup table.
[ScalarVisibility](/Cxx/Visualization/ScalarVisibility) | Switch between coloring the actor and coloring the data.
[ScaleGlyphs](/Cxx/Visualization/ScaleGlyphs) | Scale glyphs.
[SceneBounds](/Cxx/Visualization/SceneBounds) | Get the bounds of the whole scene.
[SelectWindowRegion](/Cxx/Visualization/SelectWindowRegion) | Select a region of a window.
[SingleSplat](/Cxx/VisualizationAlgorithms/SingleSplat) | Elliptical splatting. (a) Single elliptical splat with eccentricity E=10.  Cone shows orientation of vector.
[SpikeFran](/Cxx/VisualizationAlgorithms/SpikeFran) | Glyphs indicate surface normals on model of human face. Glyph positions are randomly selected.
[SplatFace](/Cxx/VisualizationAlgorithms/SplatFace) | Elliptical splatting. (b) Surface reconstructed using elliptical splats into 100^3 volume followed by isosurface extraction. Points regularly subsampled and overlaid on original mesh.
[Stocks](/Cxx/VisualizationAlgorithms/Stocks) | Two views from the stock visualization script. The top shows closing price over time; the bottom shows volume over time.
[StreamLines](/Cxx/Visualization/StreamLines) | Seed streamlines with vectors from a structured grid.
[StreamlinesWithLineWidget](/Cxx/VisualizationAlgorithms/StreamlinesWithLineWidget) | Using the vtkLineWidget to produce streamlines in the combustor dataset.  The StartInteractionEvent turns the visibility of the streamlines on; the InteractionEvent causes the streamlines to regenerate themselves.
[StructuredDataTypes](/Cxx/Visualization/StructuredDataTypes) | Demonstration of structured data types.
[TensorAxes](/Cxx/VisualizationAlgorithms/TensorAxes) | Display the scaled and oriented principal axes of the stress tensor.
[TensorEllipsoids](/Cxx/VisualizationAlgorithms/TensorEllipsoids) | Display the scaled and oriented principal axes as tensor ellipsoids representing the stress tensor.
[TensorGlyph](/Cxx/Visualization/TensorGlyph) | Draw a rotated/scaled glyph at each point.
[TextSource](/Cxx/Visualization/TextSource) | Display text.
[TransformActor](/Cxx/Visualization/TransformActor) | Transform an Actor.
[TransformActorCollection](/Cxx/Visualization/TransformActorCollection) | Transform an actor collection.
[TubesFromSplines](/Cxx/VisualizationAlgorithms/TubesFromSplines) | Create tubes from interpolated points and scalars.
[TubesWithVaryingRadiusAndColors](/Cxx/VisualizationAlgorithms/TubesWithVaryingRadiusAndColors) | Create tubes with varying radius and colors.
[VectorField](/Cxx/Visualization/VectorField) |
[VectorOfActors](/Cxx/Visualization/VectorOfActors) | Multiple Actors in a Vector.
[VectorText](/Cxx/Visualization/VectorText) | Display high resolution text.
[VelocityProfile](/Cxx/VisualizationAlgorithms/VelocityProfile) | Warping the geometry of three planes to show flow momentum.
[VertexGlyphFilter](/Cxx/Filtering/VertexGlyphFilter) | Add a vertex to each point.
[Visualize2DPoints](/Cxx/Visualization/Visualize2DPoints) | Visualize a 2D Set of Points.
[VisualizeImageData](/Cxx/Visualization/VisualizeImageData) | Visualize the points of an ImageData.
[VisualizeVTP](/Cxx/Visualization/VisualizeVTP) | Visualize a VTP File.
[VoxelsOnBoundary](/Cxx/ImageData/VoxelsOnBoundary) | Extract voxels on the border of an isosurface.
[WarpCombustor](/Cxx/VisualizationAlgorithms/WarpCombustor) | Carpet plots of combustor flow energy in a structured grid. Colors and plane displacement represent energy values.
[WindowSize](/Cxx/Visualization/WindowSize) | Change the size of a window.
[WindowTitle](/Cxx/Visualization/WindowTitle) | Change the title of a window.
[Wireframe](/Cxx/Visualization/Wireframe) |

## Volume Rendering

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[FixedPointVolumeRayCastMapperCT](/Cxx/VolumeRendering/FixedPointVolumeRayCastMapperCT) | Volume render DICOM or Meta volumes with various vtkColorTransferFunction's.
[HAVS](/Cxx/VolumeRendering/HAVSVolumeMapper) |
[IntermixedUnstructuredGrid](/Cxx/VolumeRendering/IntermixedUnstructuredGrid) | mix of poly data and unstructured grid volume mapper.
[MinIntensityRendering](/Cxx/VolumeRendering/MinIntensityRendering) | Min intensity rendering.
[RayCastIsosurface](/Cxx/VolumeRendering/RayCastIsosurface) | Isosufaces produced by volume rendering.
[SimpleRayCast](/Cxx/VolumeRendering/SimpleRayCast) | Volume rendering of a high potential iron protein.
[SmartVolumeMapper](/Cxx/VolumeRendering/SmartVolumeMapper) | Smart volume mapper.

## User Interaction

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AreaPicking](/Cxx/Picking/AreaPicking) | Area Picking.
[Assembly](/Cxx/Interaction/Assembly) | Combine/group actors into an assembly.
[CallBack](/Cxx/Interaction/CallBack) | Setting up a callback with client data. Two different methods are demonstrated.
[CallData](/Cxx/Interaction/CallData) | Pass an observer a value (via CallData).
[CellPicking](/Cxx/Picking/CellPicking) | Cell Picking.
[ClientData](/Cxx/Interaction/ClientData) | Give an observer access to an object (via ClientData).
[DoubleClick](/Cxx/Interaction/DoubleClick) | Catch a double click.
[EllipticalButton](/Cxx/Interaction/EllipticalButton) | Create an elliptical button.
[Game](/Cxx/Interaction/Game) | Move a cube into a sphere.
[HighlightPickedActor](/Cxx/Picking/HighlightPickedActor) | Highlight a picked actor by changing its color.
[HighlightSelectedPoints](/Cxx/Picking/HighlightSelectedPoints) | Highlight Selected Points.
[HighlightSelection](/Cxx/Picking/HighlightSelection) | Highlight selection.
[HighlightWithSilhouette](/Cxx/Picking/HighlightWithSilhouette) | Highlight a picked actor by adding a silhouette.
[ImageClip](/Cxx/Interaction/ImageClip) | Demonstrates how to interactively select and display a region of an image.
[ImageRegion](/Cxx/Interaction/ImageRegion) | Select a region of an image.
[InteractorStyleTerrain](/Cxx/Interaction/InteractorStyleTerrain) | Terrain mode.
[InteractorStyleUser](/Cxx/Interaction/InteractorStyleUser) | Create a completely custom interactor style (no default functionality is provided)
[KeypressEvents](/Cxx/Interaction/KeypressEvents) | Handle keypress events.
[KeypressObserver](/Cxx/Interaction/KeypressObserver) | This uses a callback function rather than a derived interactor class. Handle keypress events (lightweight).
[MouseEvents](/Cxx/Interaction/MouseEvents) | Subclass the interactor style. Handle mouse events.
[MouseEventsObserver](/Cxx/Interaction/MouseEventsObserver) | Use an observer. Handle mouse events (light weight).
[MoveAGlyph](/Cxx/Interaction/MoveAGlyph) | Drag a glyph around.
[MoveAVertexUnstructuredGrid](/Cxx/Interaction/MoveAVertexUnstructuredGrid) | Click and drag a vertex of a vtkUnstructuredGrid.
[ObserverMemberFunction](/Cxx/Interaction/ObserverMemberFunction) | Set observers to be class member functions.
[PickableOff](/Cxx/Interaction/PickableOff) | Disallow interaction with a specific actor.
[Picking](/Cxx/Interaction/Picking) | Get the world coordinate of the mouse click (if you clicked an actor)
[PointPicker](/Cxx/Interaction/PointPicker) | Get the coordinates of the closest point in the data set to the mouse click.
[RubberBand2D](/Cxx/Interaction/RubberBand2D) |
[RubberBand2DObserver](/Cxx/Interaction/RubberBand2DObserver) | RubberBand2D Observer.
[RubberBand3D](/Cxx/Interaction/RubberBand3D) |
[RubberBandPick](/Cxx/Interaction/RubberBandPick) |
[RubberBandZoom](/Cxx/Interaction/RubberBandZoom) |
[SelectAVertex](/Cxx/Interaction/SelectAVertex) | Click and drag a vertex of a vtkPolyData.
[SelectAnActor](/Cxx/Interaction/SelectAnActor) | Select an actor.
[ShiftAndControl](/Cxx/Interaction/ShiftAndControl) | Hold/holding down a key. Check if shift or control is being held.
[StyleSwitch](/Cxx/Interaction/StyleSwitch) | Choose between multiple interaction modes.
[TrackballActor](/Cxx/Interaction/TrackballActor) | Trackball actor mode.
[TrackballCamera](/Cxx/Interaction/TrackballCamera) | Trackball camera mode.
[UserEvent](/Cxx/Interaction/UserEvent) | Simple observer. Create, invoke, and handle a custom event.
[WorldPointPicker](/Cxx/Interaction/WorldPointPicker) | Get world coordinates of mouse click.

## Working with Images

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Actor2D](/Cxx/Images/Actor2D) | 2D actor and mapper.
[BackgroundImage](/Cxx/Images/BackgroundImage) | Display an image as the background of a scene.
[BorderPixelSize](/Cxx/Images/BorderPixelSize) | Set the size of border pixels.
[CannyEdgeDetector](/Cxx/Images/CannyEdgeDetector) | Perform Canny edge detection on an image.
[Cast](/Cxx/Images/Cast) | Cast an image to a different type.
[DotProduct](/Cxx/Images/DotProduct) | Compute the pixel-wise dot product of two vector images.
[DrawOnAnImage](/Cxx/Images/DrawOnAnImage) | Drawing on an image.
[DrawShapes](/Cxx/Images/DrawShapes) | Drawing shapes in an image.
[ExtractComponents](/Cxx/Images/ExtractComponents) | Extract components of an image. This can be used to get, for example, the red channel of an image.
[FillWindow](/Cxx/Images/FillWindow) | Fit imageSetup the camera to fill the window with an image.
[ImageAccumulateGreyscale](/Cxx/Images/ImageAccumulateGreyscale) | Display a grey scale histogram.
[ImageCheckerboard](/Cxx/Images/ImageCheckerboard) | Visually Compare Two Images.
[ImageCityBlockDistance](/Cxx/Images/ImageCityBlockDistance) | Compute the Manhattan distance from every point to every black point in a binary image.
[ImageDilateErode3D](/Cxx/Images/ImageDilateErode3D) | Dilate or erode an image.
[ImageExport](/Cxx/Images/ImageExport) | Export an image to a C array.
[ImageGridSource](/Cxx/Images/ImageGridSource) | Create a image of a grid.
[ImageHistogram](/Cxx/Images/ImageHistogram) | Compute the histogram of an image.
[ImageHybridMedian2D](/Cxx/Images/ImageHybridMedian2D) | Median filter an image.
[ImageIdealHighPass](/Cxx/Images/ImageIdealHighPass) | High pass filter an image.
[ImageImport](/Cxx/Images/ImageImport) | Import an image from a C array.
[ImageIslandRemoval2D](/Cxx/Images/ImageIslandRemoval2D) | Remove small patches from an image.
[ImageMagnify](/Cxx/Images/ImageMagnify) | Supersample and stretch an image.
[ImageMandelbrotSource](/Cxx/Images/ImageMandelbrotSource) | Create a Mandelbrot image.
[ImageMapper](/Cxx/Images/ImageMapper) | Display an image in 2D.
[ImageMask](/Cxx/Images/ImageMask) | Mask a region of an image.
[ImageMathematics](/Cxx/Images/ImageMathematics) | Perform mathematical operations on an image.
[ImageMedian3D](/Cxx/Images/ImageMedian3D) | Median filter a 3d or 2d image.
[ImageMirrorPad](/Cxx/Images/ImageMirrorPad) | Pad the edges of an extended image by mirror existing pixels.
[ImageNonMaximumSuppression](/Cxx/Images/ImageNonMaximumSuppression) | Find peaks in an image using non maximum suppression.
[ImageOpenClose3D](/Cxx/Images/ImageOpenClose3D) | Open or close (morphologically) an image.
[ImageOrder](/Cxx/Images/ImageOrder) | Determine the display order of a stack of images.
[ImageOrientation](/Cxx/Images/ImageOrientation) | Orientation of the view of an image.
[ImageRFFT](/Cxx/Images/ImageRFFT) | Inverse FFT.
[ImageRange3D](/Cxx/Images/ImageRange3D) | Replace every pixel with the range of its neighbors according to a kernel.
[ImageRotate](/Cxx/Images/ImageRotate) | Rotate a 2D image. This is even more powerful than vtkImageSliceMapper. It can also do oblique slices.
[ImageSeparableConvolution](/Cxx/Images/ImageSeparableConvolution) | Convolve a separable kernel with an image.
[ImageShiftScale](/Cxx/Images/ImageShiftScale) | Shift and scale an image.
[ImageSinusoidSource](/Cxx/Images/ImageSinusoidSource) | Create a sinusoidal image.
[ImageSlice](/Cxx/Images/ImageSlice) | Visualize and interact with an image. This is even more powerful than vtkImageSliceMapper. It can also do oblique slices.
[ImageSliceMapper](/Cxx/Images/ImageSliceMapper) | Visualize and interact with an image. This is the new way to do this. It is much more powerful.
[ImageStack](/Cxx/Images/ImageStack) | Display layers of images.
[ImageStencil](/Cxx/Images/ImageStencil) | Copy one image into another image.
[ImageText](/Cxx/Images/ImageText) | Draw text in an image.
[ImageThreshold](/Cxx/Images/ImageThreshold) | Threshold an image.
[ImageToPolyDataFilter](/Cxx/Images/ImageToPolyDataFilter) | Convert a vtkImageData to a vtkPolyData.
[ImageToStructuredPoints](/Cxx/Images/ImageToStructuredPoints) | Convert a vtkImageData to a vtkStructuredPoints.
[ImageTransparency](/Cxx/Images/ImageTransparency) | Set transparency of image pixels.
[InteractWithImage](/Cxx/Images/InteractWithImage) | Visualize and interact with an image.
[Interpolation](/Cxx/Images/Interpolation) | Set the interpolation type for the display of an image. If pixels look blurry instead of sharp when zoomed in, change this.
[MarkKeypoints](/Cxx/Images/MarkKeypoints) | Mark keypoints in an image.
[NegativeIndices](/Cxx/Images/NegativeIndices) | A very powerful feature of vtkImageData is that you can use negative indices.
[PickPixel](/Cxx/Images/PickPixel) | Picking a pixel.
[PickPixel2](/Cxx/Images/PickPixel2) | Picking a pixel 2 - modified version for exact pixel values.
[RTAnalyticSource](/Cxx/Images/RTAnalyticSource) | An image source that can be used for regression testing
[StaticImage](/Cxx/Images/StaticImage) | This will display the image, but not allow you to interact with it.
[Transparency](/Cxx/Images/Transparency) | Make part of an image transparent.

## Image Processing

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[Attenuation](/Cxx/ImageProcessing/Attenuation) | This MRI image illustrates attenuation that can occur due to sensor position.  The artifact is removed by dividing by the attenuation profile determined manually.
[CenterAnImage](/Cxx/Images/CenterAnImage) | Center an image.
[Colored2DImageFusion](/Cxx/Images/Colored2DImageFusion) | Blending 2D images with different color maps.
[CombineImages](/Cxx/Images/CombineImages) | Combine two images.
[CombiningRGBChannels](/Cxx/Images/CombiningRGBChannels) | Combine layers into an RGB image.
[EnhanceEdges](/Cxx/ImageProcessing/EnhanceEdges) | High-pass filters can extract and enhance edges in an image. Subtraction of the Laplacian (middle) from the original image (left) results in edge enhancement or a sharpening operation (right).
[Flip](/Cxx/Images/Flip) | Flip an image.
[GaussianSmooth](/Cxx/ImageProcessing/GaussianSmooth) | Low-pass filters can be implemented as convolution with a Gaussian kernel.
[Gradient](/Cxx/Images/Gradient) | Compute the gradient vector at every pixel.
[HybridMedianComparison](/Cxx/ImageProcessing/HybridMedianComparison) | Comparison of median and hybrid-median filters. The hybrid filter preserves corners and thin lines, better than the median filter.
[IdealHighPass](/Cxx/ImageProcessing/IdealHighPass) | This figure shows two high-pass filters in the frequency domain. The Butterworth high-pass filter has a gradual attenuation that avoids ringing produced by the ideal high-pass filter with an abrupt transition.
[ImageAccumulate](/Cxx/Images/ImageAccumulate) | Histogram
[ImageAnisotropicDiffusion2D](/Cxx/Images/ImageAnisotropicDiffusion2D) | Anisotropic diffusion (2D).
[ImageContinuousDilate3D](/Cxx/Images/ImageContinuousDilate3D) | Dilate an image.
[ImageContinuousErode3D](/Cxx/Images/ImageContinuousErode3D) | Erode an image.
[ImageConvolve](/Cxx/Images/ImageConvolve) | Convolve an image with a kernel.
[ImageCorrelation](/Cxx/Images/ImageCorrelation) | Correlate two images.
[ImageDifference](/Cxx/Images/ImageDifference) | Compute the difference image of two images.
[ImageDivergence](/Cxx/Images/ImageDivergence) | Divergence of a vector field.
[ImageEllipsoidSource](/Cxx/Images/ImageEllipsoidSource) | Create an image of an ellipsoid.
[ImageFFT](/Cxx/Images/ImageFFT) | Compute the FFT of an image.
[ImageGaussianSmooth](/Cxx/Images/ImageGaussianSmooth) | Smooth an image.
[ImageGradient](/Cxx/VisualizationAlgorithms/ImageGradient) | Create an imaging pipeline to visualize gradient information.
[ImageGradientMagnitude](/Cxx/Images/ImageGradientMagnitude) | Compute the magnitude of the gradient at each pixel of an image.
[ImageLaplacian](/Cxx/Images/ImageLaplacian) | Compute the Laplacian of an image.
[ImageLuminance](/Cxx/Images/ImageLuminance) | Convert RGB image to greyscale (luminance).
[ImageMagnitude](/Cxx/Images/ImageMagnitude) | grayscaleConvert RGB image to grey scale.
[ImageMapToColors](/Cxx/Images/ImageMapToColors) | Use a lookup table to map scalar (gray scale) values to colorsConvert grey scale image to RGB.
[ImageNoiseSource](/Cxx/Images/ImageNoiseSource) | Create an image of noise.
[ImagePermute](/Cxx/Images/ImagePermute) | Switch axes of an image.
[ImageShrink3D](/Cxx/Images/ImageShrink3D) | Resample an image.
[ImageSobel2D](/Cxx/Images/ImageSobel2D) | Sobel edge detection 2D.
[ImageValueRange](/Cxx/Images/ImageValueRange) | Get the range of image pixel values (min/max).
[ImageVariance3D](/Cxx/Images/ImageVariance3D) | Construct a new image consisting of the variance of the input image at each pixel.
[ImageWarp](/Cxx/Images/ImageWarp) | Combine the imaging and visualization pipelines to deform an image in the z-direction. The vtkMergeFilter is used to combine the warped surface with the original color data.
[IsoSubsample](/Cxx/ImageProcessing/IsoSubsample) | This figure demonstrates aliasing that occurs when a high-frequency signal is subsampled. High frequencies appear as low frequency artifacts. The left image is an isosurface of a skull after subsampling. The right image used a low-pass filter before subsampling to reduce aliasing.
[MedianComparison](/Cxx/ImageProcessing/MedianComparison) | Comparison of Gaussian and Median smoothing for reducing low-probability high-amplitude noise.
[MorphologyComparison](/Cxx/ImageProcessing/MorphologyComparison) | This figure demonstrates various binary filters that can alter the shape of segmented regions.
[Pad](/Cxx/ImageProcessing/Pad) | Convolution in frequency space treats the image as a periodic function. A large kernel can pick up features from both sides of the image. The lower-left image has been padded with zeros to eliminate wraparound during convolution. On the right, mirror padding has been used to remove artificial edges introduced by borders.
[RGBToHSI](/Cxx/Images/RGBToHSI) | Convert RGB to HSI.
[RGBToHSV](/Cxx/Images/RGBToHSV) | Convert RGB to HSV.
[RGBToYIQ](/Cxx/Images/RGBToYIQ) | Convert RGB to YIQ.
[RescaleAnImage](/Cxx/ImageProcessing/RescaleAnImage) | Rescale an image
[ResizeImage](/Cxx/Images/ResizeImage) | Resize an image using a sinc interpolator.
[ResizeImageDemo](/Cxx/Images/ResizeImageDemo) | Demonstrate allsinc interpolators to resize an image.
[VTKSpectrum](/Cxx/ImageProcessing/VTKSpectrum) | The discrete Fourier transform changes an image from the spatial domain into the frequency domain, where each pixel represents a sinusoidal function. This figure shows an image and its power spectrum displayed using a logarithmic transfer function.

## Widgets

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AffineWidget](/Cxx/Widgets/AffineWidget) | Apply an affine transformation interactively.
[AngleWidget](/Cxx/Widgets/AngleWidget) |
[AngleWidget2D](/Cxx/Widgets/AngleWidget2D) | vtkAngleWidget + vtkAngleRepresentation2D.
[BalloonWidget](/Cxx/Widgets/BalloonWidget) |
[BiDimensionalWidget](/Cxx/Widgets/BiDimensionalWidget) | When would you use this?
[BorderWidget](/Cxx/Widgets/BorderWidget) | 2D selection, 2D box.
[BoxWidget](/Cxx/Widgets/BoxWidget) |
[BoxWidget2](/Cxx/Widgets/BoxWidget2) |
[CaptionWidget](/Cxx/Widgets/CaptionWidget) |
[CheckerboardWidget](/Cxx/Widgets/CheckerboardWidget) | Compare two images using a checkerboard.
[CompassWidget](/Cxx/Widgets/CompassWidget) | Draws an interactive compass.
[ContourWidget](/Cxx/Widgets/ContourWidget) | Create a contour from a set of points and dynamically change the contour using the points as control points.
[Cursor2D](/Cxx/Visualization/Cursor2D) |
[DistanceWidget](/Cxx/Widgets/DistanceWidget) |
[HoverWidget](/Cxx/Widgets/HoverWidget) | How to detect a hover?
[ImagePlaneWidget](/Cxx/Widgets/ImagePlaneWidget) |
[ImageTracerWidget](/Cxx/Widgets/ImageTracerWidget) | Scribble on an image.
[ImageTracerWidgetInsideContour](/Cxx/Widgets/ImageTracerWidgetInsideContour) | Highlight pixels inside a non-regular region scribbled on an image.
[ImageTracerWidgetNonPlanar](/Cxx/Widgets/ImageTracerWidgetNonPlanar) | Draw on a non-planar surface.
[ImplicitPlaneWidget2](/Cxx/Widgets/ImplicitPlaneWidget2) | Clip polydata with an implicit plane.
[LineWidget2](/Cxx/Widgets/LineWidget2) |
[LogoWidget](/Cxx/Widgets/LogoWidget) | Logo widget.
[OrientationMarkerWidget](/Cxx/Widgets/OrientationMarkerWidget) | Draws two cubes. One of them can be clicked to be rotated, and will rotate the second one. The second one has annotations on it, and can also be moved.
[OrientationMarkerWidget1](/Cxx/Widgets/OrientationMarkerWidget1) | Display a polydata as an orientation icon.
[PlaneWidget](/Cxx/Widgets/PlaneWidget) | Interact with a plane.
[PolygonalSurfacePointPlacer](/Cxx/PolyData/PolygonalSurfacePointPlacer) | Used in conjunction with vtkContourWidget to draw curves on a surface.
[RectilinearWipeWidget](/Cxx/Widgets/RectilinearWipeWidget) | Compare two images.
[ScalarBarWidget](/Cxx/Widgets/ScalarBarWidget) | The ScalarBarWidget displays a scalar bar that is movable and changes orientation automatically when close to the borders of the image. It needs a ScalarBarActor
[SeedWidget](/Cxx/Widgets/SeedWidget) | Seed widget.
[SeedWidgetImage](/Cxx/Widgets/SeedWidgetImage) | How to start the interaction?
[SeedWidgetWithCustomCallback](/Cxx/Widgets/SeedWidgetWithCustomCallback) | How to place points in a scene using a custom callback.
[Slider](/Cxx/Widgets/Slider) | 3D Slider.
[Slider2D](/Cxx/Widgets/Slider2D) | 2D Slider.
[SphereWidget](/Cxx/Widgets/SphereWidget) | Sphere widget.
[SphereWidget2](/Cxx/Widgets/SphereWidget2) |
[SphereWidgetEvents](/Cxx/Widgets/SphereWidgetEvents) | Sphere widget events.
[SplineWidget](/Cxx/Widgets/SplineWidget) |
[TextWidget](/Cxx/Widgets/TextWidget) | Draw movable text.
[TexturedButtonWidget](/Cxx/Widgets/TexturedButtonWidget) | Create a textured 2D button.

## Plotting

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AreaPlot](/Cxx/Plotting/AreaPlot) | Plot area between two curves.
[BarChart](/Cxx/Plotting/BarChart) | Bar chart.
[BoxChart](/Cxx/Plotting/BoxChart) | Box plot.
[ChartMatrix](/Cxx/Plotting/ChartMatrix) | Create a marix of plots.
[ChartsOn3DScene](/Cxx/Plotting/ChartsOn3DScene) | Draw a chart in a 3D scene.
[CompareRandomGeneratorsCxx](/Cxx/Plotting/CompareRandomGeneratorsCxx) | Compare STL random number generators.
[Diagram](/Cxx/Plotting/Diagram) | Draw a custom diagram.
[FunctionalBagPlot](/Cxx/Plotting/FunctionalBagPlot) | Functional Bag Plot.
[Histogram2D](/Cxx/Plotting/Histogram2D) | 2D Histogram of a vtkImageData.
[HistogramBarChart](/Cxx/Plotting/HistogramBarChart) | Histogram using bar chart.
[LinePlot](/Cxx/Plotting/LinePlot) | Line plot.
[LinePlot3D](/Cxx/Plotting/PlotLine3D) | Line plot of 3d data..
[ParallelCoordinates](/Cxx/Plotting/ParallelCoordinates) | Parallel coordinates.
[PieChart](/Cxx/Plotting/PieChart) | Pie chart.
[PieChartActor](/Cxx/Plotting/PieChartActor) | Pie chart.
[ScatterPlot](/Cxx/Plotting/ScatterPlot) | Scatter plot.
[SpiderPlot](/Cxx/Plotting/SpiderPlot) | Spider plot.
[StackedBar](/Cxx/Plotting/StackedBar) | Stacked bar.
[StackedPlot](/Cxx/Plotting/StackedPlot) | Stacked plot.
[SurfacePlot](/Cxx/Plotting/SurfacePlot) | SurfacePlot.

## Animation

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[AnimateActors](/Cxx/Animation/AnimateActors) | Animate actors.
[Animation](/Cxx/Utilities/Animation) | Move a sphere across a scene.
[AnimationScene](/Cxx/Utilities/AnimationScene) | Animation (the right way). Zoom in on a sphere.
[DataAnimation](/Cxx/Utilities/DataAnimation) | Data Animation. Update points in a dataset every specified interval.
[DataAnimationSubclass](/Cxx/Utilities/DataAnimationSubclass) | Update points in a dataset every specified interval (using a vtkCommand subclass instead of a callback function.
[RotatingSphere](/Cxx/Animation/RotatingSphere) | A rotating sphere.

## InfoVis

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ParallelCoordinatesView](/Cxx/InfoVis/ParallelCoordinatesView) | Parallel coordinates.
[TreeMapView](/Cxx/InfoVis/TreeMapView) | Tree map.
[WordCloud](/Cxx/InfoVis/WordCloud) | Create a word cloud.

## Qt

Click [here](http://www.vtk.org/Wiki/VTK/Tutorials/QtSetup) for a tutorial on how to setup Qt.

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[BarChartQt](/Cxx/Qt/BarChartQt) | Bar chart.
[BorderWidgetQt](/Cxx/Qt/BorderWidgetQt) |  2D selection, 2D box.
[EventQtSlotConnect](/Cxx/Qt/EventQtSlotConnect) | Connect a VTK event to a Qt slot.
[ImageDataToQImage](/Cxx/Qt/ImageDataToQImage) | Convert a vtkImageData to a QImage.
[QImageToImageSource](/Cxx/Qt/QImageToImageSource) | Convert a QImage to a vtkImageData.
[RenderWindowNoUiFile](/Cxx/Qt/RenderWindowNoUiFile) | This is a very basic example that shows how to create a Qt window. Typically, one would want to design a form in the QtDesigner (this is shown in [RenderWindowUIMultipleInheritance](/Cxx/Qt/RenderWindowUIMultipleInheritance)).
[RenderWindowUIMultipleInheritance](/Cxx/Qt/RenderWindowUIMultipleInheritance) | Using a QVTKOpenGLWidget with the Qt Multiple Inheritance model.
[RenderWindowUISingleInheritance](/Cxx/Qt/RenderWindowUISingleInheritance) | Using a QVTKOpenGLWidget with the Qt Single Inheritance model.
[ShareCameraQt](/Cxx/Qt/ShareCameraQt) | Share the camera between QVTKOpenGLWidgets.
[ShowEvent](/Cxx/Qt/ShowEvent) | Use QMainWindow::showEvent event to do things that you might want to do in the constructor
[SideBySideRenderWindowsQt](/Cxx/Qt/SideBySideRenderWindowsQt) | Side by side render windows.

## Matlab

You must turn on VTK_USE_MATLAB_MEX to use these.

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[MatlabEngineFilter](/Cxx/Matlab/MatlabEngineFilter) |

## Databases

### SQL

If you have any trouble or errors with the following examples, please troubleshoot using [these instructions](http://www.vtk.org/Wiki/Tutorials/SQLSetup).

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[ConnectAndRead](/Cxx/Databases/SQL/MySQL/ConnectAndRead) | Connect to and read a MySQL database.
[CreateDatabase](/Cxx/Databases/SQL/MySQL/CreateDatabase) | Create a MySQL database.
[WriteToDatabase](/Cxx/Databases/SQL/MySQL/WriteToDatabase) | Write to a MySQL database.

## RenderMan

RenderMan is a high quality rendering system created by [Pixar](https://renderman.pixar.com/view/renderman). VTK can export RenderMan RIB files for rendering by [prman](http://renderman.pixar.com/view/renderman-prman-the-rispec-and-renderman-studio). In the spring of 2015, Pixar released a [non-commercial version](http://renderman.pixar.com/view/PR-NCR) of its RenderMan products.

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[PolyDataRIB](/Cxx/RenderMan/PolyDataRIB) | Apply a RenderMan Shader to a PolyData.

## Remote Modules

Remote modules are user contributed code that is not distributed with VTK. A description of a remote module and the location of its repository is listed in the VTK/Remote directory with the extension .remote.cmake. Once enabled, a remote module is treated like any other VTK module. Each of the remote module examples describes how to enable the example. More information about remote modules is [here](http://www.vtk.org/Wiki/VTK/Remote_Modules).

| Example Name | Description | Image |
| -------------- | ------------- | ------- |
[FrenetSerretFrame](/Cxx/Remote/FrenetSerretFrame) | Compute tangent and normal vectors to a polyline.
[FrenetSerretFrameDemo](/Cxx/Remote/FrenetSerretFrameDemo) | Uses vtkSplineWidget to interact with a spline. Shows how to control a pipeline inside a callback.
