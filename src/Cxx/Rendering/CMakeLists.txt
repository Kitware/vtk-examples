project (${WIKI}Rendering)

if(NOT VTK_BINARY_DIR)
  set(VTK_LIBRARIES "")
  find_package(VTK COMPONENTS
    CommonColor
    CommonComputationalGeometry
    CommonCore
    CommonDataModel
    CommonSystem
    CommonTransforms
    FiltersCore
    FiltersGeneral
    FiltersGeometry
    FiltersModeling
    FiltersSources
    FiltersTexture
    IOGeometry
    IOImage
    IOLegacy
    IOPLY
    IOParallel
    IOXML
    ImagingCore
    ImagingHybrid
    ImagingSources
    InteractionImage
    InteractionStyle
    InteractionWidgets
    RenderingAnnotation
    RenderingContextOpenGL2
    RenderingCore
    RenderingFreeType
    RenderingGL2PSOpenGL2
    RenderingOpenGL2
    OPTIONAL_COMPONENTS
    cli11
    jsoncpp
    TestingRendering
    QUIET
    )
endif()

set(KIT_LIBS ${VTK_LIBRARIES})

#
# Build all .cxx files in the directory
file(GLOB ALL_FILES *.cxx)

include(${WikiExamples_SOURCE_DIR}/CMake/RequiresCxxVersion.cmake)
set(CXX_VERSION_MIN "17")
Requires_Cxx_Version(PBR_Skybox ${CXX_VERSION_MIN} ALL_FILES)
Requires_Cxx_Version(PBR_Skybox_Texturing ${CXX_VERSION_MIN} ALL_FILES)
Requires_Cxx_Version(PBR_Skybox_Anisotropy ${CXX_VERSION_MIN} ALL_FILES)

Requires_Cxx_Version(PBR_Anisotropy ${CXX_VERSION_MIN} ALL_FILES)
Requires_Cxx_Version(PBR_Clear_Coat ${CXX_VERSION_MIN} ALL_FILES)
Requires_Cxx_Version(PBR_Edge_Tint ${CXX_VERSION_MIN} ALL_FILES)
Requires_Cxx_Version(PBR_HDR_Environment ${CXX_VERSION_MIN} ALL_FILES)
Requires_Cxx_Version(PBR_Mapping ${CXX_VERSION_MIN} ALL_FILES)
Requires_Cxx_Version(PBR_Materials ${CXX_VERSION_MIN} ALL_FILES)
Requires_Cxx_Version(PBR_Materials_Coat ${CXX_VERSION_MIN} ALL_FILES)

include(${WikiExamples_SOURCE_DIR}/CMake/RequiresModule.cmake)
Requires_Module(PBR_Skybox cli11)
Requires_Module(PBR_Skybox_Texturing cli11)
Requires_Module(PBR_Skybox_Anisotropy cli11)

Requires_Module(PBR_Skybox jsoncpp)
Requires_Module(PBR_Skybox_Texturing jsoncpp)
Requires_Module(PBR_Skybox_Anisotropy jsoncpp)

Requires_Module(PBR_Anisotropy cli11)
Requires_Module(PBR_Clear_Coat cli11)
Requires_Module(PBR_Edge_Tint cli11)
Requires_Module(PBR_HDR_Environment cli11)
Requires_Module(PBR_Mapping cli11)
Requires_Module(PBR_Materials cli11)
Requires_Module(PBR_Materials_Coat cli11)

Requires_Module(PBR_Anisotropy jsoncpp)
Requires_Module(PBR_Clear_Coat jsoncpp)
Requires_Module(PBR_Edge_Tint jsoncpp)
Requires_Module(PBR_HDR_Environment jsoncpp)
Requires_Module(PBR_Mapping jsoncpp)
Requires_Module(PBR_Materials jsoncpp)
Requires_Module(PBR_Materials_Coat jsoncpp)


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
  set(KIT Rendering)
  set(NEEDS_ARGS
    FlatVersusGouraud
    HiddenLineRemoval
    InterpolateCamera
    MotionBlur
    PBR_Anisotropy
    PBR_Clear_Coat
    PBR_Edge_Tint
    PBR_HDR_Environment
    PBR_Mapping
    PBR_Materials
    PBR_Materials_Coat
    PBR_Skybox
    PBR_Skybox_Texturing
    PBR_Skybox_Anisotropy
    Rainbow
    Rotations
    RotationsA
    RotationsB
    RotationsC
    RotationsD
    Shadows
    StripFran
    WalkCow
    WalkCowA
    WalkCowB
    )

  set(DATA ${WikiExamples_SOURCE_DIR}/src/Testing/Data)
  set(TEMP ${WikiExamples_BINARY_DIR}/Testing/Temporary)

  add_test(${KIT}-FlatVersusGouraud ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestFlatVersusGouraud ${DATA}/cow.obj)

  add_test(${KIT}-HiddenLineRemoval ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestHiddenLineRemoval ${DATA}/cow.obj)

  add_test(${KIT}-InterpolateCamera ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestInterpolateCamera ${DATA}/spider.ply)

  add_test(${KIT}-MotionBlur ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestMotionBlur ${DATA}/Armadillo.ply)

  add_test(${KIT}-Rotations ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestRotations ${DATA}/cow.obj)

  add_test(${KIT}-RotationsA ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestRotationsA ${DATA}/cow.obj 1)

  add_test(${KIT}-RotationsB ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestRotationsB ${DATA}/cow.obj 2)

  add_test(${KIT}-RotationsC ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestRotationsC ${DATA}/cow.obj 3)

  add_test(${KIT}-RotationsD ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestRotationsD ${DATA}/cow.obj 4)

  add_test(${KIT}-Rainbow ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestRainbow ${DATA}/combxyz.bin ${DATA}/combq.bin)

  add_test(${KIT}-Shadows ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestShadows ${DATA}/dragon.ply)

  add_test(${KIT}-StripFran ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestStripFran ${DATA}/fran_cut.vtk)

  add_test(${KIT}-WalkCow ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestWalkCow ${DATA}/cow.g)

  add_test(${KIT}-WalkCowA ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestWalkCowA ${DATA}/cow.g 1)

  add_test(${KIT}-WalkCowB ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestWalkCowB ${DATA}/cow.g 2)

  if(TARGET PBR_Anisotropy)
    add_test(${KIT}-PBR_Anisotropy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestPBR_Anisotropy ${DATA}/PBR_Parameters)
  endif()
  if(TARGET PBR_Clear_Coat)
    add_test(${KIT}-PBR_Clear_Coat ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
      TestPBR_Clear_Coat ${DATA}/PBR_Parameters)
  endif()
  if(TARGET PBR_Edge_Tint)
    add_test(${KIT}-PBR_Edge_Tint ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
      TestPBR_Edge_Tint ${DATA}/PBR_Parameters)
  endif()
  if(TARGET PBR_HDR_Environment)
    add_test(${KIT}-PBR_HDR_Environment ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
      TestPBR_HDR_Environment ${DATA}/PBR_Parameters)
  endif()
  if(TARGET PBR_Mapping)
    add_test(${KIT}-PBR_Mapping ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
      TestPBR_Mapping ${DATA}/PBR_Parameters)
  endif()
  if(TARGET PBR_Materials)
    add_test(${KIT}-PBR_Materials ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
      TestPBR_Materials ${DATA}/PBR_Parameters)
  endif()
  if(TARGET PBR_Materials_Coat)
    add_test(${KIT}-PBR_Materials_Coat ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
      TestPBR_Materials_Coat ${DATA}/PBR_Parameters)
  endif()

  if(TARGET PBR_Skybox)
    add_test(${KIT}-PBR_Skybox ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests TestPBR_Skybox
      ${DATA}/PBR_Skybox.json)
  endif()
  if(TARGET PBR_Skybox_Texturing)
    add_test(${KIT}-PBR_Skybox_Texturing ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
      TestPBR_Skybox_Texturing ${DATA}/PBR_Skybox_Texturing)
  endif()
  if(TARGET PBR_Skybox_Anisotropy)
    add_test(${KIT}-PBR_Skybox_Anisotropy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
      TestPBR_Skybox_Anisotropy ${DATA}/PBR_Skybox_Anisotropy -sBoy)
  endif()

  include(${WikiExamples_SOURCE_DIR}/CMake/ExamplesTesting.cmake)

endif()
