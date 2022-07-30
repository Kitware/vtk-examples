# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CMakeFiles/MinimalQtVTKApp_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/MinimalQtVTKApp_autogen.dir/ParseCache.txt"
  "MinimalQtVTKApp_autogen"
  )
endif()
