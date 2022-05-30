project (${WIKI}Images)

if(NOT VTK_BINARY_DIR)
  set(VTK_LIBRARIES "")
  find_package(VTK COMPONENTS
    CommonColor
    CommonCore
    CommonDataModel
    CommonExecutionModel
    CommonTransforms
    FiltersCore
    FiltersGeneral
    FiltersGeometry
    FiltersHybrid
    FiltersSources
    IOImage
    IOXML
    ImagingColor
    ImagingCore
    ImagingFourier
    ImagingGeneral
    ImagingMath
    ImagingMorphological
    ImagingSources
    ImagingStatistics
    ImagingStencil
    InteractionImage
    InteractionStyle
    InteractionWidgets
    RenderingAnnotation
    RenderingCore
    RenderingFreeType
    RenderingImage
    RenderingOpenGL2
    OPTIONAL_COMPONENTS
    TestingRendering
    QUIET
    )
endif()

set(KIT Images)
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
  set(NEEDS_ARGS
    CannyEdgeDetector
    Cast
    CenterAnImage
    Colored2DImageFusion
    CombineImages
    DrawOnAnImage
    ExtractComponents
    Flip
    ImageAccumulateGreyscale
    ImageAnisotropicDiffusion2D
    ImageCheckerboard
    ImageCityBlockDistance
    ImageContinuousDilate3D
    ImageContinuousErode3D
    ImageDilateErode3D
    ImageHistogram
    ImageHybridMedian2D
    ImageMagnify
    ImageNonMaximumSuppression
    ImageOpenClose3D
    ImageRange3D
    ImageRotate
    ImageSeparableConvolution
    ImageToPolyDataFilter
    ImageToStructuredPoints
    ImageWarp
    InteractWithImage
    PickPixel
    PickPixel2
    ResizeImage
    ResizeImageDemo
    RGBToHSI
    RGBToHSV
    RGBToYIQ
    RTAnalyticSource
    StaticImage
    Transparency
    )

  set(DATA ${WikiExamples_SOURCE_DIR}/src/Testing/Data)
  set(TEMP ${WikiExamples_BINARY_DIR}/Testing/Temporary)

  add_test(${KIT}-CannyEdgeDetector ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestCannyEdgeDetector ${DATA}/Gourds.png)

  add_test(${KIT}-Cast ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestCast  ${DATA}/Ox.jpg ${TEMP}/Cast.jpg)

  add_test(${KIT}-CenterAnImage ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestCenterAnImage  ${DATA}/Ox.jpg)

  add_test(${KIT}-Colored2DImageFusion ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestColored2DImageFusion  ${DATA}/Ox.jpg ${DATA}/Gourds2.jpg)

  add_test(${KIT}-CombineImages ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestCombineImages  ${DATA}/Ox.jpg ${DATA}/Gourds2.jpg)

  add_test(${KIT}-DrawOnAnImage ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestDrawOnAnImage  ${DATA}/Gourds2.jpg)

  add_test(${KIT}-ExtractComponents ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestExtractComponents  ${DATA}/Gourds2.jpg)

  add_test(${KIT}-Flip ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestFlip  ${DATA}/Gourds2.jpg ${TEMP}/Flip.jpg )

  add_test(${KIT}-ImageAccumulateGreyscale ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestImageAccumulateGreyscale  ${DATA}/Ox.jpg)

  add_test(${KIT}-ImageAnisotropicDiffusion2D ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestImageAnisotropicDiffusion2D  ${DATA}/cake_easy.jpg)

  add_test(${KIT}-ImageCheckerboard ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestImageCheckerboard  ${DATA}/Ox.jpg ${DATA}/Gourds2.jpg)

  add_test(${KIT}-ImageCityBlockDistance ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestImageCityBlockDistance ${DATA}/Yinyang.jpg -E 15)

  add_test(${KIT}-ImageContinuousDilate3D ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestImageContinuousDilate3D ${DATA}/Gourds.png)

  add_test(${KIT}-ImageContinuousErode3D ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestImageContinuousErode3D ${DATA}/Gourds.png)

  add_test(${KIT}-ImageDilateErode3D ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestImageDilateErode3D ${DATA}/Yinyang.png -E 25)

  add_test(${KIT}-ImageHistogram ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestImageHistogram ${DATA}/Pileated.jpg)

  add_test(${KIT}-ImageHybridMedian2D ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestImageHybridMedian2D ${DATA}/Gourds.png)

  add_test(${KIT}-ImageMagnify ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestImageMagnify ${DATA}/Gourds.png)

  # The ImageNoiseSource is inherently random
  add_test(${KIT}-ImageNoiseSource ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestImageNoiseSource -E 999999)

  add_test(${KIT}-ImageNonMaximumSuppression ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestImageNonMaximumSuppression -E 20)

  add_test(${KIT}-ImageOpenClose3D ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestImageOpenClose3D ${DATA}/Yinyang.png -E 25)

  add_test(${KIT}-ImageRange3D ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestImageRange3D  ${DATA}/Gourds2.jpg)

  add_test(${KIT}-ImageRotate ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestImageRotate ${DATA}/Gourds.png 45)

  add_test(${KIT}-ImageSeparableConvolution ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestImageSeparableConvolution ${DATA}/Yinyang.png -E 25)

  add_test(${KIT}-ImageToPolyDataFilter ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestImageToPolyDataFilter ${DATA}/Gourds.png)

  add_test(${KIT}-ImageToStructuredPoints ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestImageToStructuredPoints -E 25)

  add_test(${KIT}-ImageWarp ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestImageWarp ${DATA}/masonry.bmp)

  add_test(${KIT}-InteractWithImage ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestInteractWithImage ${DATA}/Ox.jpg)

  add_test(${KIT}-PickPixel ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestPickPixel  ${DATA}/Gourds2.jpg)

  add_test(${KIT}-PickPixel2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestPickPixel2  ${DATA}/ColorCells.tif)

  add_test(${KIT}-ResizeImage ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestResizeImage ${DATA}/Gourds2.jpg 1280 1024 5)

  add_test(${KIT}-ResizeImageDemo ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestResizeImageDemo ${DATA}/Pileated.jpg 3)

  add_test(${KIT}-Transparency ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestTransparency ${DATA}/Gourds2.jpg)

  add_test(${KIT}-RGBToHSI ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestRGBToHSI ${DATA}/Gourds2.jpg)

  add_test(${KIT}-RGBToHSV ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestRGBToHSV ${DATA}/Gourds2.jpg)

  add_test(${KIT}-RGBToYIQ ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestRGBToYIQ ${DATA}/Gourds2.jpg)

  add_test(${KIT}-StaticImage ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestStaticImage ${DATA}/Ox.jpg)

  add_test(${KIT}-RTAnalyticSource ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KIT}CxxTests
    TestRTAnalyticSource -E 20)

  include(${WikiExamples_SOURCE_DIR}/CMake/ExamplesTesting.cmake)
endif()
