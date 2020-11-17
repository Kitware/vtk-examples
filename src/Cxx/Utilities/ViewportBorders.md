### Description

Sometimes multiple vtkRenderer viewports can be difficult to differentiate. This example draws a border around each viewport. The example creates a vtkPolyLine that outlines the viewport. The coordinates for the vtkPoints are specified in normalized viewport coordinates. The coordinate selection is made with a vtkCoordinate. The vtkPolyDataMapper2D and vtkActor2D render the border.

To run the example, provide 1-N vtkPolyDataReader files.

This examples uses the data, `src/Testing/Data/v.vtk,`, `src/Testing/Data/t.vtk` and `src/Testing/Data/k.vtk`.
