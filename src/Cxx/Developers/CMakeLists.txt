project (${WIKI}Developers)

if(NOT VTK_BINARY_DIR)
  set(VTK_LIBRARIES "")
  find_package(VTK COMPONENTS
    CommonCore
    CommonDataModel
    CommonExecutionModel
    FiltersSources
    InfovisCore
    QUIET
    )
endif()

set(KIT_LIBS ${VTK_LIBRARIES})

add_executable(AlgorithmFilter AlgorithmFilter.cxx vtkTestAlgorithmFilter.cxx vtkTest.cxx)
target_link_libraries(AlgorithmFilter ${VTK_LIBRARIES})
vtk_module_autoinit(
  TARGETS AlgorithmFilter
  MODULES ${VTK_LIBRARIES}
  )

add_executable(AlgorithmSource AlgorithmSource.cxx vtkTestAlgorithmSource.cxx vtkTest1.cxx)
target_link_libraries(AlgorithmSource ${VTK_LIBRARIES})
vtk_module_autoinit(
  TARGETS AlgorithmSource
  MODULES ${VTK_LIBRARIES}
  )

add_executable(ProgressReport ProgressReport.cxx vtkTestProgressReportFilter.cxx)
target_link_libraries(ProgressReport ${VTK_LIBRARIES})
vtk_module_autoinit(
  TARGETS ProgressReport
  MODULES ${VTK_LIBRARIES}
  )

add_executable(FilterSelfProgress FilterSelfProgress.cxx vtkTestFilterSelfProgressFilter.cxx)
target_link_libraries(FilterSelfProgress ${VTK_LIBRARIES})
vtk_module_autoinit(
  TARGETS FilterSelfProgress
  MODULES ${VTK_LIBRARIES}
  )

add_executable(GraphAlgorithmFilter GraphAlgorithmFilter.cxx vtkTestGraphAlgorithmFilter.cxx)
target_link_libraries(GraphAlgorithmFilter ${VTK_LIBRARIES})
vtk_module_autoinit(
  TARGETS GraphAlgorithmFilter
  MODULES ${VTK_LIBRARIES}
  )

add_executable(GraphAlgorithmSource GraphAlgorithmSource.cxx vtkTestGraphAlgorithmSource.cxx)
target_link_libraries(GraphAlgorithmSource ${VTK_LIBRARIES})
vtk_module_autoinit(
  TARGETS GraphAlgorithmSource
  MODULES ${VTK_LIBRARIES}
  )

add_executable(ImageAlgorithmFilter ImageAlgorithmFilter.cxx vtkImageAlgorithmFilter.cxx)
target_link_libraries(ImageAlgorithmFilter ${VTK_LIBRARIES})
vtk_module_autoinit(
  TARGETS ImageAlgorithmFilter
  MODULES ${VTK_LIBRARIES}
  )

add_executable(MultipleInputPorts MultipleInputPorts.cxx vtkTestMultipleInputPortsFilter.cxx)
target_link_libraries(MultipleInputPorts ${VTK_LIBRARIES})
vtk_module_autoinit(
  TARGETS MultipleInputPorts
  MODULES ${VTK_LIBRARIES}
  )
