macro(Requires_Cxx_Version example version_min)
  if(${CMAKE_CXX_STANDARD} VERSION_LESS "${version_min}")
  message(STATUS "VTKWikiExamples: ${example} requires C++${version_min} or newer but the current version is C++${CMAKE_CXX_STANDARD}")
  string(REGEX REPLACE "[^;]*${example}.cxx"
         "" ALL_FILES "${ALL_FILES}")
  endif()
endmacro()
