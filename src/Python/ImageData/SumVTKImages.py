#!/usr/bin/env python

from vtkmodules.vtkImagingMath import vtkImageWeightedSum


# ImageSets = List of Image sets
# Weights  = List of weights e.g [1, 1, 1]

def SumVTKImages(ImageSets, Weights):
    NumOfImages = len(ImageSets)
    SumFilter = vtkImageWeightedSum()
    for x in range(0, NumOfImages, 1):
        SumFilter.AddInputConnection(ImageSets[x])
        SumFilter.SetWeight(x, Weights[x])
    SumFilter.Update()

    # Return summed Image as vtkImageData
    return SumFilter.GetOutput()
