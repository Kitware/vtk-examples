# Supplementary Data

In this folder we store supplementary data/images referenced by the markdown files associated with the examples.

The data is organised by language and area of application.

You will need to manually add the inline HTML links as follows:

1. For an image. Taken from `src/Cxx/Graphs/MinimumSpanningTree.md`:

    ``` text
    <img style="float:middle" src="https://raw.githubusercontent.com/Kitware/vtk-examples/gh-pages/src/SupplementaryData/Cxx/Graphs/3_vertex_fully_connected_graph.svg">
    ```

    This will be displayed as an image on the example page.

2. For a link to data. Taken from `src/Cxx/IO/ReadDICOM.md`:

    ``` text
    <a id="raw-url" href="https://raw.githubusercontent.com/Kitware/vtk-examples/gh-pages/src/SupplementaryData/Cxx/IO/DICOM_Prostate.zip">DICOM_Prostate</a>
    ```

    This will be displayed as a link on the example page.

The supplementary data/images will be copied across to the `gh-pages` branch when `SyncSiteWithRepo.sh` is run.
