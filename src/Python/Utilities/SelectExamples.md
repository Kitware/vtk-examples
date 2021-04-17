### Description

This script allows you to obtain a list of examples corresponding to a particular VTK Class and a given language.

In order to do this a JSON file listing the vtk examples by VTK class is obtained from the gh-pages branch of the vtk-examples GitHub site. When this script runs, it checks for the existence of this JSON file in your temporary folder, downloading it if it doesn't exist. If it already exists, then it is updated if the file is more than ten minutes old.

When you run this script by specifying the VTK Class and Language (one of: `CSharp`, `Cxx`, `Java`, `Python`), a list of links to the respective examples containing that class is returned. By default five or fewer examples are returned. If there are more than five examples for a class then five examples are randomly selected. You can override the maximum number of examples (e.g. `-n20`) or select all examples `-a`.
