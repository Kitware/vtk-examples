### Description

This example uses two vtkClipDataSet filters to achieve a "tissue lens" effect. First, a vtkSphere implicit function is used to clip a spherical hole in the isosurface extracted with vtkFlyingEdges3D or vtkMarchingCubes. Then a geometric vtkSphereSource samples the original volume data using a vtkProbeFilter. vtkClipDataSet uses the resulting scalar point data to clip the sphere surface with the isosurface value.

!!! example "Usage"
    TissueLens FullHead.mhd

!!! note
   The skin color was selected from Table 7 in [Improvement of Haar Feature Based Face Detection in OpenCV Incorporating Human Skin Color Characteristic](https://www.researchgate.net/publication/310443424_Improvement_of_Haar_Feature_Based_Face_Detection_in_OpenCV_Incorporating_Human_Skin_Color_Characteristic)

!!! info
    The example uses `src/Testing/Data/FullHead.mhd` which references `src/Testing/Data/FullHead.raw.gz`.
