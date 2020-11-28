### Description

Frog organs along with a translucent skin are reconstructed from the original segmented data.

vtkFlyingEdges3D is used by default to take the 3D structured point set and generate the iso-surface. However if desired, you can specify vtkMarchingCubes instead.

The dataset is derived from a frog. This data was prepared at Lawrence Berkeley National Laboratories and is included with their permission. The data was acquired by physically slicing the frog and photographing the slices. The original segmented data is in the form of tissue masks with one file per tissue. There are 136 slices per tissue and 15 different tissues. Each slice is 470 by 500 pixels.

To accommodate the volume readers we have in VTK, we processed the mask files and combined them all into one vtkMetaImageReader *.mhd* file. Integer numbers 1–15 to represent the 15 tissues. A similar process was done for the frog skin.

The code uses a general way of specifying transformations that can permute image and other geometric data in order to maintain proper orientation regardless of the acquisition order. See the class **SliceOrder**.

!!! info
    By specifying the tissues and other parameters, as outlined in the code, you can generate approximations to the following figures: [Figure 12-9a](../../../VTKBook/12Chapter12/#Figure%2012-9a), [Figure 12-9b](../../../VTKBook/12Chapter12/#Figure%2012-9b), [Figure 12-9c](../../../VTKBook/12Chapter12/#Figure%2012-9c), and [Figure 12-9d](../../../VTKBook/12Chapter12/#Figure%2012-9d) in [Chapter 12](../../../VTKBook/12Chapter12) of the [VTK Textbook](../../../VTKBook/01Chapter1).
