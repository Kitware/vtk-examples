project (${WIKI}Graphs)

if(NOT VTK_BINARY_DIR)
  set(VTK_LIBRARIES "")
  find_package(VTK COMPONENTS
    vtkCommonCore
    vtkCommonDataModel
    vtkFiltersCore
    vtkFiltersGeneral
    vtkFiltersModeling
    vtkFiltersSources
    vtkIOLegacy
    vtkIOXML
    vtkInfovisCore
    vtkInfovisLayout
    vtkInteractionStyle
    vtkRenderingCore
    vtkRenderingFreeType
    vtkRenderingOpenGL2
    vtkViewsCore
    vtkViewsInfovis
    OPTIONAL_COMPONENTS
    vtkTestingRendering
    QUIET
    )
  if (VTK_VERSION VERSION_LESS "8.90.0")
    include(${VTK_USE_FILE})
  endif()
endif()

set(KIT_LIBS ${VTK_LIBRARIES})

#
# Build all .cxx files in the directory
file(GLOB ALL_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cxx)

set(BOOST_SRCS)
if(NOT VTK_USE_BOOST)
  list(APPEND BOOST_SRCS
    AdjacentVertexIterator.cxx
    BreadthFirstDistance.cxx
    BoostBreadthFirstSearchTree.cxx
    ConnectedComponents.cxx
    DepthFirstSearchIterator.cxx
    MinimumSpanningTree.cxx
    )
endif()

if (BOOST_SRCS)
  list(REMOVE_ITEM ALL_FILES ${BOOST_SRCS})
endif()

if(VTK_USE_BOOST)
  Requires_Module(AdjacentVertexIterator vtkInfovisBoostGraphAlgorithms)
  Requires_Module(BreadthFirstDistance vtkInfovisBoostGraphAlgorithms)
  Requires_Module(BoostBreadthFirstSearchTree vtkInfovisBoostGraphAlgorithms)
  Requires_Module(ConnectedComponents vtkInfovisBoostGraphAlgorithms)
  Requires_Module(DepthFirstSearchIterator vtkInfovisBoostGraphAlgorithms)
  Requires_Module(MinimumSpanningTree vtkInfovisBoostGraphAlgorithms)
endif()

foreach(SOURCE_FILE ${ALL_FILES})
  string(REPLACE ".cxx" "" TMP ${SOURCE_FILE})
  string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/ "" EXAMPLE ${TMP})
  add_executable(${WIKI}${EXAMPLE} ${EXECUTABLE_FLAG} ${EXAMPLE}.cxx)
  target_link_libraries(${WIKI}${EXAMPLE} ${KIT_LIBS})
  if (NOT VTK_VERSION VERSION_LESS "8.90.0")
    vtk_module_autoinit(
      TARGETS ${WIKI}${EXAMPLE}
      MODULES ${VTK_LIBRARIES}
      )
  endif()
endforeach()

if (BUILD_TESTING)
  # Testing
  set(KIT Graphs)
  set(NEEDS_ARGS
    )
  endif()
  include(${WikiExamples_SOURCE_DIR}/CMake/ExamplesTesting.cmake)
