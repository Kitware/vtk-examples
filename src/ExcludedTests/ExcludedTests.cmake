# These are tests to be excluded.
# These tests generate random graphs based on timings.
list(APPEND TESTS_TO_IGNORE
  "DataStructures-KDTreeTimingDemo"
  "DataStructures-ModifiedBSPTreeTimingDemo"
  "DataStructures-OBBTreeTimingDemo"
  "DataStructures-OctreeTimingDemo"
  )

# This is dependent upon the screen resolution.
# If you want to test it, set:
# renderWindow->SetFullScreen(false);
list(APPEND TESTS_TO_IGNORE
  "Utilities-FullScreen"
  )
