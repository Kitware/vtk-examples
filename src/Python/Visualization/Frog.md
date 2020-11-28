### Description

Frog organs along with a translucent skin can be viewed.

The dataset is derived from a frog. This data was prepared at Lawrence Berkeley National Laboratories and is included with their permission. The data was acquired by physically slicing the frog and photographing the slices. The original segmented data is in the form of tissue masks with one file per tissue. There are 136 slices per tissue and 15 different tissues. Each slice is 470 by 500 pixels.

This example uses the vtkPolyData file for the skin and files that are generated from the tissue masks. You add tissues by specifying a list of tissue names. Note that the skin is rendered as being translucent so that you can see the internal organs.

The code uses a general way of specifying transformations that can permute image and other geometric data in order to maintain proper orientation regardless of the acquisition order. See the class **SliceOrder**.

!!! info
    [FrogReconstruction](../FrogReconstruction) for an example detailing a general method for reconstructing from the original data.

!!! info
    By specifying the tissues and other parameters, as outlined in the code, you can generate the following figures: [Figure 12-9a](../../../VTKBook/12Chapter12/#Figure%2012-9a), [Figure 12-9b](../../../VTKBook/12Chapter12/#Figure%2012-9b), [Figure 12-9c](../../../VTKBook/12Chapter12/#Figure%2012-9c), and [Figure 12-9d](../../../VTKBook/12Chapter12/#Figure%2012-9d) in [Chapter 12](../../../VTKBook/12Chapter12) of the [VTK Textbook](../../../VTKBook/01Chapter1).
