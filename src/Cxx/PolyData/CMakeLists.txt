project (${WIKI}PolyData)

if(NOT VTK_BINARY_DIR)
  set(optional "")
  Add_Optional_If_In_Library(Boolean "${VTK_AVAILABLE_COMPONENTS}" optional)
  set(VTK_LIBRARIES "")
  find_package(VTK COMPONENTS
    CommonColor
    CommonComputationalGeometry
    CommonCore
    CommonDataModel
    CommonExecutionModel
    CommonTransforms
    FiltersCore
    FiltersExtraction
    FiltersGeneral
    FiltersGeometry
    FiltersHybrid
    FiltersModeling
    FiltersPoints
    FiltersSources
    FiltersVerdict
    IOGeometry
    IOImage
    IOImport
    IOLegacy
    IOPLY
    IOXML
    ImagingCore
    ImagingHybrid
    ImagingSources
    ImagingStencil
    InteractionStyle
    InteractionWidgets
    RenderingAnnotation
    RenderingCore
    RenderingFreeType
    RenderingGL2PSOpenGL2
    RenderingLOD
    RenderingOpenGL2
    ${optional}
    OPTIONAL_COMPONENTS
    #    FiltersParallelStatistics
    TestingRendering
    QUIET
    )
endif()

set(KIT_LIBS ${VTK_LIBRARIES})

#
# Build all .cxx files in the directory
file(GLOB ALL_FILES *.cxx)

Requires_Class(AlignTwoPolyDatas HausdorffDistancePointSetFilter)

foreach(SOURCE_FILE ${ALL_FILES})
  string(REPLACE ".cxx" "" TMP ${SOURCE_FILE})
  string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/ "" EXAMPLE ${TMP})
  add_executable(${WIKI}${EXAMPLE} ${EXECUTABLE_FLAG} ${EXAMPLE}.cxx)
  target_link_libraries(${WIKI}${EXAMPLE} ${KIT_LIBS})
  vtk_module_autoinit(
    TARGETS ${WIKI}${EXAMPLE}
    MODULES ${VTK_LIBRARIES}
    )
endforeach()

#Requires_Target(PolyDataBooleanOperationFilter vtkPolyDataBooleanFilter)

if (BUILD_TESTING)
  # Testing
  set(KIT PolyData)
  set(NEEDS_ARGS
    AlignTwoPolyDatas
    BooleanOperationPolyDataFilter
    CellCentersDemo
    CellLocatorVisualization
    CellsInsideObject
    ColorDisconnectedRegionsDemo
    ColoredPoints
    CombineImportedActors
    ConvexHull
    Curvatures
    DataSetSurfaceFilter
    DetermineArrayDataTypes
    DijkstraGraphGeodesicPath
    EmbedPointsIntoVolume
    ExternalContour
    GetMiscCellData
    GetMiscPointData
    GeometryFilter
    GradientFilter
    GreedyTerrainDecimation
    HighlightBadCells
    Hull
    ImplicitModeller
    LoopBooleanPolyDataFilter
    MiscPointData
    PointLocatorVisualization
    PolyDataIsoLines
    PolyDataPointSampler
    PolyDataToImageData
    RemoveVertices
    ResampleAppendedPolyData
    ShrinkPolyData
    Silhouette
    ThinPlateSplineTransform
    TriangleColoredPoints
    TriangleSolidColor
    WarpSurface
    )

  set(DATA ${WikiExamples_SOURCE_DIR}/src/Testing/Data)
  set(TEMP ${WikiExamples_BINARY_DIR}/Testing/Temporary)

  add_test(${KIT}-AlignTwoPolyDatas ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestAlignTwoPolyDatas ${DATA}/thingiverse/Grey_Nurse_Shark.stl ${DATA}/greatWhite.stl)

  add_test(${KIT}-BooleanOperationPolyDataFilter ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestBooleanOperationPolyDataFilter ${DATA}/Torso.vtp difference ${DATA}/ObliqueCone.vtp)

  add_test(${KIT}-LoopBooleanPolyDataFilter ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestLoopBooleanPolyDataFilter ${DATA}/Torso.vtp difference ${DATA}/ObliqueCone.vtp)

  add_test(${KIT}-CellCentersDemo ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestCellCentersDemo ${DATA}/cow.g)

  add_test(${KIT}-CellLocatorVisualization ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestCellLocatorVisualization -E 50)

  add_test(${KIT}-CellsInsideObject ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestCellsInsideObject ${DATA}/cow.g)

  add_test(${KIT}-ColorDisconnectedRegionsDemo ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestColorDisconnectedRegionsDemo ${DATA}/fsu/footbones.ply)

  add_test(${KIT}-ColoredPoints ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestColoredPoints ${TEMP}/coloredpoints.vtp)

  add_test(${KIT}-CombineImportedActors ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestCombineImportedActors ${DATA}/iflamingo.3ds)

  add_test(${KIT}-ConvexHull ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestConvexHull ${DATA}/cowHead.vtp)

  add_test(${KIT}-Curvatures ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestCurvatures ${DATA}/cowHead.vtp)

  add_test(${KIT}-DataSetSurfaceFilter ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestDataSetSurfaceFilter ${DATA}/uGridEx.vtk)

  add_test(${KIT}-DijkstraGraphGeodesicPath ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestDijkstraGraphGeodesicPath	-E 20)

  add_test(${KIT}-EmbedPointsIntoVolume ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestEmbedPointsIntoVolume ${DATA}/Bunny.vtp )

  add_test(${KIT}-ExternalContour ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestExternalContour ${DATA}/Bunny.vtp -E 80 )

  add_test(${KIT}-DetermineArrayDataTypes ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestDetermineArrayDataTypes ${DATA}/filledContours.vtp)

  add_test(${KIT}-GeometryFilter ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestGeometryFilter ${DATA}/uGridEx.vtk)

  add_test(${KIT}-GetMiscCellData ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestGetMiscCellData ${DATA}/SuperQuadric.vtp Normals)

  add_test(${KIT}-GetMiscPointData ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestGetMiscPointData ${DATA}/cowHead.vtp Gauss_Curvature)

  add_test(${KIT}-GradientFilter ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestGradientFilter ${DATA}/uGridEx.vtk)

  add_test(${KIT}-GreedyTerrainDecimation ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestGreedyTerrainDecimation -E 55)

  add_test(${KIT}-HighlightBadCells ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestHighlightBadCells -E 20)

  add_test(${KIT}-ImplicitModeller ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestImplicitModeller ${DATA}/Bunny.vtp)

  add_test(${KIT}-MiscPointData ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestMiscPointData ${DATA}/cowHead.vtp)

  add_test(${KIT}-PointLocatorVisualization ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestPointLocatorVisualization -E 50)

  add_test(${KIT}-PolyDataIsoLines ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestPolyDataIsoLines ${DATA}/cowHead.vtp)

  add_test(${KIT}-PolyDataPointSampler ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestPolyDataPointSampler ${DATA}/Torso.vtp)

  add_test(${KIT}-PolyDataToImageData ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestPolyDataToImageData ${DATA}/Torso.vtp)

  add_test(${KIT}-RemoveVertices ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestRemoveVertices ${DATA}/Bunny.vtp)

  add_test(${KIT}-ResampleAppendedPolyData ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestResampleAppendedPolyData -E 20)

  add_test(${KIT}-Silhouette ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestSilhouette ${DATA}/cowHead.vtp)

  add_test(${KIT}-ShrinkPolyData ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestShrinkPolyData ${DATA}/cowHead.vtp -E 30)

  add_test(${KIT}-ThinPlateSplineTransform ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestThinPlateSplineTransform ${DATA}/masonry.bmp)

  add_test(${KIT}-TriangleColoredPoints ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestTriangleColoredPoints -E 15)

  add_test(${KIT}-TriangleSolidColor ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestTriangleSolidColor -E 25)

  add_test(${KIT}-WarpSurface ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestWarpSurface ${DATA}/cowHead.vtp .1)

  include(${WikiExamples_SOURCE_DIR}/CMake/ExamplesTesting.cmake)

endif()
