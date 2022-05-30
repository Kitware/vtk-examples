project (${WIKI}DataStructures)

if(NOT VTK_BINARY_DIR)
  set(VTK_LIBRARIES "")
  find_package(VTK COMPONENTS
    ChartsCore
    CommonColor
    CommonCore
    CommonDataModel
    CommonSystem
    FiltersExtraction
    FiltersFlowPaths
    FiltersGeneral
    FiltersSources
    IOGeometry
    IOLegacy
    IOPLY
    IOXML
    InteractionStyle
    InteractionWidgets
    RenderingContext2D
    RenderingContextOpenGL2
    RenderingCore
    RenderingFreeType
    RenderingGL2PSOpenGL2
    RenderingOpenGL2
    ViewsContext2D
    OPTIONAL_COMPONENTS
    TestingRendering
    QUIET
   )
endif()

set(KIT_LIBS ${VTK_LIBRARIES})

#
# Build all .cxx files in the directory
file(GLOB ALL_FILES *.cxx)
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

if (BUILD_TESTING)
  # Testing
  # Note, the following examples are excluded:
  # KDTreeTimingDemo
  # ModifiedBSPTreeTimingDemo
  # OBBTreeTimingDemo
  # OctreeTimingDemo
  # See: ../../CMake/CTestCustom.cmake.in

  set(KIT DataStructures)
  set(NEEDS_ARGS
    DataStructureComparison
    KDTreeFindPointsWithinRadiusDemo
    OctreeFindPointsWithinRadiusDemo
    OctreeVisualize
    PointLocatorFindPointsWithinRadiusDemo
    StaticLocatorFindPointsWithinRadiusDemo
    VisualizeKDTree
    VisualizeModifiedBSPTree
    VisualizeOBBTree
    )
  set(DATA ${WikiExamples_SOURCE_DIR}/src/Testing/Data)

  add_test(${KIT}-DataStructureComparison ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestDataStructureComparison ${DATA}/Bunny.vtp -E 20)

  add_test(${KIT}-VisualizeKDTree ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestVisualizeKDTree ${DATA}/shark.ply)
  add_test(${KIT}-VisualizeOBBTree ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestVisualizeOBBTree ${DATA}/shark.ply)
  add_test(${KIT}-VisualizeModifiedBSPTree ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestVisualizeModifiedBSPTree ${DATA}/shark.ply)

  add_test(${KIT}-KDTreeFindPointsWithinRadiusDemo ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestKDTreeFindPointsWithinRadiusDemo ${DATA}/dragon.ply 10)

  add_test(${KIT}-OctreeFindPointsWithinRadiusDemo ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestOctreeFindPointsWithinRadiusDemo ${DATA}/dragon.ply 10)

  add_test(${KIT}-PointLocatorFindPointsWithinRadiusDemo ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestPointLocatorFindPointsWithinRadiusDemo ${DATA}/dragon.ply 10)

  add_test(${KIT}-StaticLocatorFindPointsWithinRadiusDemo ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestStaticLocatorFindPointsWithinRadiusDemo ${DATA}/dragon.ply 10)

  add_test(${KIT}-OctreeVisualize ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestOctreeVisualize -E 40)

  include(${WikiExamples_SOURCE_DIR}/CMake/ExamplesTesting.cmake)

endif()
