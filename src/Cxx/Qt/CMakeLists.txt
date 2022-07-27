#-----------------------------------------------------------------------------
# Set policies as needed.
foreach(p
    CMP0020 # CMake 2.8.11
    CMP0022 # CMake 2.8.12
    CMP0025 # CMake 3.0
    CMP0043 # CMake 3.0
    CMP0053 # CMake 3.1
    CMP0071 # Cmake 3.10
    CMP0084 # Cmake 3.14
    )
  if(POLICY ${p})
    cmake_policy(SET ${p} NEW)
  endif()
endforeach()
#-----------------------------------------------------------------------------

project (QtVTKWikiExamples)

if(NOT VTK_BINARY_DIR)
  set(VTK_LIBRARIES "")
  find_package(VTK COMPONENTS
    ChartsCore
    CommonColor
    CommonCore
    CommonDataModel
    FiltersCore
    FiltersSources
    GUISupportQt
    InfovisCore
    InteractionStyle
    InteractionWidgets
    RenderingContext2D
    RenderingContextOpenGL2
    RenderingCore
    RenderingFreeType
    RenderingGL2PSOpenGL2
    RenderingOpenGL2
    RenderingQt
    ViewsContext2D
    ViewsQt
    QUIET
    )
    if (VTK_VERSION VERSION_LESS "8.90.0")
      include(${VTK_USE_FILE})
    endif()
endif()

set(KIT_LIBS ${VTK_LIBRARIES})

# Let Qt find its MOCed headers in the build directory.
include_directories(${CMAKE_CURRENT_BINARY_DIR})

#
# For all [example].ui files, build them together with their corresponding
# [example]Driver.cxx and [example].cxx. Subtract these cxx files from a
# list of all cxx files in the directory - we will build the rest in the next block.
file(GLOB ALL_UI_FILES *.ui)
file(GLOB ALL_FILES *.cxx)

Requires_Module(ShareCameraQt ViewsQt)
Requires_Module(ShareCameraQtDriver ViewsQt)
Requires_Module(SideBySideRenderWindowsQt ViewsQt)
Requires_Module(SideBySideRenderWindowsQtDriver ViewsQt)
Requires_Module(QImageToImageSource RenderingQt)

find_package(Qt6Widgets QUIET)
set(qt_version 6)
if(NOT Qt6Widgets_FOUND)
  find_package(Qt5Widgets QUIET)
  if(NOT Qt5Widgets_FOUND)
    message(STATUS "VTKWikiExamples: Not building Qt examples, could not find a valid Qt installation.")
    return()
  endif()
  set(qt_version 5)
endif()

if (NOT DEFINED VTK_QT_VERSION)
  set(VTK_QT_VERSION 5)
endif()

if(NOT qt_version STREQUAL VTK_QT_VERSION)
    message(STATUS "VTKWikiExamples: Not building Qt examples, VTK not built with the same Qt version.")
    return()
endif()
message(STATUS "VTKWikiExamples: Building Qt examples")

set(qt_components Core Gui Widgets)
if(${qt_version} VERSION_GREATER_EQUAL 6)
  list(APPEND qt_components OpenGLWidgets)
endif()
list(SORT qt_components)
# We have ui files, so this will also bring in the macro:
#   qt5_wrap_ui or qt_wrap_ui from Widgets.
find_package(Qt${qt_version} QUIET
  REQUIRED COMPONENTS ${qt_components}
)

foreach(_qt_comp IN LISTS qt_components)
  list(APPEND qt_modules "Qt${qt_version}::${_qt_comp}")
endforeach()

if (VTK_VERSION VERSION_LESS "8.90.0")
# Instruct CMake to run moc automatically when needed.
  set(CMAKE_AUTOMOC ON)
else()
# Instruct CMake to run moc and uic automatically when needed.
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTOUIC ON)
endif()

foreach(EXAMPLE_FILE ${ALL_UI_FILES})
  string(REPLACE ".ui" "" TMP ${EXAMPLE_FILE})
  string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/ "" EXAMPLE ${TMP})
  set(UISrcs)
  set(MOCSrcs)
  # For VTK versions greater than or equal to 8.90.0:
  #  CMAKE_AUTOUIC is ON so we handle uic automatically for Qt targets.
  #  CMAKE_AUTOMOC is ON so we handle moc automatically for Qt targets.
  # However we have to do the following for VTK versions less than 8.90.0,
  #  and we also assume, in this case, that Qt5 is being used.
  add_executable(${WIKI}${EXAMPLE} ${EXECUTABLE_FLAG} ${EXAMPLE}Driver.cxx ${EXAMPLE}.cxx ${UISrcs} ${EXAMPLE}.h)
  if (Qt${qt_version}Widgets_VERSION VERSION_LESS "5.11.0")
    qt5_use_modules(${WIKI}${EXAMPLE} ${qt_components})
  else()
    target_link_libraries(${WIKI}${EXAMPLE} ${qt_modules})
  endif()
  target_link_libraries(${WIKI}${EXAMPLE} ${KIT_LIBS})
    vtk_module_autoinit(
      TARGETS ${WIKI}${EXAMPLE}
      MODULES ${VTK_LIBRARIES}
      )
  list(REMOVE_ITEM ALL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE}Driver.cxx ${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE}.cxx)
endforeach()

# Build all remaining .cxx files.
foreach(EXAMPLE_FILE ${ALL_FILES})
  string(REPLACE ".cxx" "" TMP ${EXAMPLE_FILE})
  string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/ "" EXAMPLE ${TMP})
  add_executable(${WIKI}${EXAMPLE} ${EXECUTABLE_FLAG} ${EXAMPLE}.cxx)
  if (Qt${qt_version}Widgets_VERSION VERSION_LESS "5.11.0")
    qt5_use_modules(${WIKI}${EXAMPLE} ${qt_components})
  else()
    target_link_libraries(${WIKI}${EXAMPLE} ${qt_modules})
  endif()
  target_link_libraries(${WIKI}${EXAMPLE} ${KIT_LIBS})
  vtk_module_autoinit(
    TARGETS ${WIKI}${EXAMPLE}
    MODULES ${VTK_LIBRARIES}
    )
endforeach()
