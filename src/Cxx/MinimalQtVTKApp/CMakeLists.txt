cmake_minimum_required(VERSION 3.23)
project(${WIKI}minimalQtVTK)

if(NOT VTK_BINARY_DIR)
  set(optional "")
message(STATUS "optional: ${optional}")
  set(VTK_LIBRARIES "")
  find_package(VTK COMPONENTS 
    REQUIRED
    CommonColor
    CommonCore
    CommonDataModel
    CommonSystem
    FiltersCore
    FiltersGeneral
    FiltersPoints
    FiltersSources
    InteractionStyle
    RenderingAnnotation
    RenderingCore
    RenderingOpenGL2
    GUISupportQt
    QUIET
    )
endif()
set(KIT_LIBS ${VTK_LIBRARIES} VTK::RenderingOpenGL2 VTK::GUISupportQt)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set(ALL_FILES minimalQtVTK.cxx)

set(CMAKE_AUTORCC ON)

foreach(SOURCE_FILE ${ALL_FILES})
  string(REPLACE ".cxx" "" EXAMPLE ${SOURCE_FILE})
  string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/ "" EXAMPLE ${EXAMPLE})
  add_executable(${WIKI}${EXAMPLE} ${EXECUTABLE_FLAG} ${SOURCE_FILE} resources/resources.qrc)
  target_link_libraries(${WIKI}${EXAMPLE} ${KIT_LIBS} Qt5::Core Qt5::Widgets)
  vtk_module_autoinit(
    TARGETS ${WIKI}${EXAMPLE}
    MODULES ${VTK_LIBRARIES}
    )
endforeach()
