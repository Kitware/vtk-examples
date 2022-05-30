project (${WIKI}Graphs)

if(NOT VTK_BINARY_DIR)
  set(VTK_LIBRARIES "")
  find_package(VTK COMPONENTS
    CommonCore
    CommonDataModel
    FiltersCore
    FiltersGeneral
    FiltersModeling
    FiltersSources
    IOLegacy
    IOXML
    InfovisCore
    InfovisLayout
    InteractionStyle
    RenderingCore
    RenderingFreeType
    RenderingOpenGL2
    ViewsCore
    ViewsInfovis
    OPTIONAL_COMPONENTS
    TestingRendering
    QUIET
    )
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
  Requires_Module(AdjacentVertexIterator InfovisBoostGraphAlgorithms)
  Requires_Module(BreadthFirstDistance InfovisBoostGraphAlgorithms)
  Requires_Module(BoostBreadthFirstSearchTree InfovisBoostGraphAlgorithms)
  Requires_Module(ConnectedComponents InfovisBoostGraphAlgorithms)
  Requires_Module(DepthFirstSearchIterator InfovisBoostGraphAlgorithms)
  Requires_Module(MinimumSpanningTree InfovisBoostGraphAlgorithms)
endif()

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
  set(KIT Graphs)
  set(NEEDS_ARGS
    )
  endif()
  include(${WikiExamples_SOURCE_DIR}/CMake/ExamplesTesting.cmake)
