#!/usr/bin/env python

import math

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonCore import VTK_UNSIGNED_CHAR
from vtkmodules.vtkCommonDataModel import (
    vtkImageData,
    vtkPlane
)
from vtkmodules.vtkFiltersCore import (
    vtkCutter,
    vtkStripper
)
from vtkmodules.vtkFiltersModeling import vtkLinearExtrusionFilter
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkIOImage import (
    vtkMetaImageWriter,
    vtkPNGWriter
)
from vtkmodules.vtkIOXML import vtkXMLPolyDataWriter
from vtkmodules.vtkImagingStencil import (
    vtkImageStencil,
    vtkPolyDataToImageStencil
)


def main():
    # 3D source sphere
    sphereSource = vtkSphereSource()
    sphereSource.SetPhiResolution(30)
    sphereSource.SetThetaResolution(30)
    sphereSource.SetCenter(40, 40, 0)
    sphereSource.SetRadius(20)

    # generate circle by cutting the sphere with an implicit plane
    # (through its center, axis-aligned)
    circleCutter = vtkCutter()
    circleCutter.SetInputConnection(sphereSource.GetOutputPort())
    cutPlane = vtkPlane()
    cutPlane.SetOrigin(sphereSource.GetCenter())
    cutPlane.SetNormal(0, 0, 1)
    circleCutter.SetCutFunction(cutPlane)

    stripper = vtkStripper()
    stripper.SetInputConnection(circleCutter.GetOutputPort())  # valid circle
    stripper.Update()

    # that's our circle
    circle = stripper.GetOutput()

    # write circle out
    polyDataWriter = vtkXMLPolyDataWriter()
    polyDataWriter.SetInputData(circle)

    polyDataWriter.SetFileName('circle.vtp')
    polyDataWriter.SetCompressorTypeToNone()
    polyDataWriter.SetDataModeToAscii()
    polyDataWriter.Write()

    # prepare the binary image's voxel grid
    whiteImage = vtkImageData()
    bounds = [0] * 6
    circle.GetBounds(bounds)
    spacing = [0] * 3  # desired volume spacing
    spacing[0] = 0.5
    spacing[1] = 0.5
    spacing[2] = 0.5
    whiteImage.SetSpacing(spacing)

    # compute dimensions
    dim = [0] * 3
    for i in range(3):
        dim[i] = int(math.ceil((bounds[i * 2 + 1] - bounds[i * 2]) / spacing[i])) + 1
        if dim[i] < 1:
            dim[i] = 1
    whiteImage.SetDimensions(dim)
    whiteImage.SetExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1)
    origin = [0] * 3
    # NOTE: I am not sure whether or not we had to add some offset!
    origin[0] = bounds[0]  # + spacing[0] / 2
    origin[1] = bounds[2]  # + spacing[1] / 2
    origin[2] = bounds[4]  # + spacing[2] / 2
    whiteImage.SetOrigin(origin)
    whiteImage.AllocateScalars(VTK_UNSIGNED_CHAR, 1)

    # fill the image with foreground voxels:
    inval = 255
    outval = 0
    count = whiteImage.GetNumberOfPoints()
    # for (vtkIdType i = 0 i < count ++i)
    for i in range(count):
        whiteImage.GetPointData().GetScalars().SetTuple1(i, inval)

    # sweep polygonal data (this is the important thing with contours!)
    extruder = vtkLinearExtrusionFilter()
    extruder.SetInputData(circle)
    extruder.SetScaleFactor(1.0)
    # extruder.SetExtrusionTypeToNormalExtrusion()
    extruder.SetExtrusionTypeToVectorExtrusion()
    extruder.SetVector(0, 0, 1)
    extruder.Update()

    # polygonal data -. image stencil:
    pol2stenc = vtkPolyDataToImageStencil()
    pol2stenc.SetTolerance(0)  # important if extruder.SetVector(0, 0, 1) !!!
    pol2stenc.SetInputConnection(extruder.GetOutputPort())
    pol2stenc.SetOutputOrigin(origin)
    pol2stenc.SetOutputSpacing(spacing)
    pol2stenc.SetOutputWholeExtent(whiteImage.GetExtent())
    pol2stenc.Update()

    # cut the corresponding white image and set the background:
    imgstenc = vtkImageStencil()
    imgstenc.SetInputData(whiteImage)
    imgstenc.SetStencilConnection(pol2stenc.GetOutputPort())
    imgstenc.ReverseStencilOff()
    imgstenc.SetBackgroundValue(outval)
    imgstenc.Update()

    imageWriter = vtkMetaImageWriter()
    imageWriter.SetFileName('labelImage.mhd')
    imageWriter.SetInputConnection(imgstenc.GetOutputPort())
    imageWriter.Write()

    imageWriter = vtkPNGWriter()
    imageWriter.SetFileName('labelImage.png')
    imageWriter.SetInputConnection(imgstenc.GetOutputPort())
    imageWriter.Write()


if __name__ == '__main__':
    main()
