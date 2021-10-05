#!/usr/bin/env python
# -*- coding: utf-8 -*-

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingFreeType
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkPoints
from vtkmodules.vtkCommonDataModel import (
    vtkCellArray,
    vtkPolyData
)
from vtkmodules.vtkCommonTransforms import vtkTransform
from vtkmodules.vtkFiltersCore import (
    vtkAppendFilter,
    vtkContourFilter
)
from vtkmodules.vtkFiltersGeneral import (
    vtkTransformFilter,
    vtkTransformPolyDataFilter,
    vtkWarpTo
)
from vtkmodules.vtkFiltersHybrid import vtkImplicitModeller
from vtkmodules.vtkFiltersModeling import vtkRotationalExtrusionFilter
from vtkmodules.vtkFiltersSources import (
    vtkConeSource,
    vtkCubeSource,
    vtkSphereSource
)
from vtkmodules.vtkRenderingCore import (
    vtkDataSetMapper,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkTextActor
)
from vtkmodules.vtkRenderingLOD import vtkLODActor


def main():
    colors = vtkNamedColors()

    # Set the colors.
    colors.SetColor('AzimuthArrowColor', [255, 77, 77, 255])
    colors.SetColor('ElevationArrowColor', [77, 255, 77, 255])
    colors.SetColor('RollArrowColor', [255, 255, 77, 255])
    colors.SetColor('SpikeColor', [255, 77, 255, 255])
    colors.SetColor('BkgColor', [26, 51, 102, 255])

    # Create a rendering window, renderer and interactor.
    ren = vtkRenderer()
    renWin = vtkRenderWindow()
    renWin.AddRenderer(ren)
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Create a camera model.
    camCS = vtkConeSource()
    camCS.SetHeight(1.5)
    camCS.SetResolution(12)
    camCS.SetRadius(0.4)

    camCBS = vtkCubeSource()
    camCBS.SetXLength(1.5)
    camCBS.SetZLength(0.8)
    camCBS.SetCenter(0.4, 0, 0)

    camAPD = vtkAppendFilter()
    camAPD.AddInputConnection(camCBS.GetOutputPort())
    camAPD.AddInputConnection(camCS.GetOutputPort())

    camMapper = vtkPolyDataMapper()
    camMapper.SetInputConnection(camAPD.GetOutputPort())
    camActor = vtkLODActor()
    camActor.SetMapper(camMapper)
    camActor.SetScale(2, 2, 2)

    # Draw the arrows.
    pd = vtkPolyData()
    ca = vtkCellArray()
    fp = vtkPoints()
    fp.InsertNextPoint(0, 1, 0)
    fp.InsertNextPoint(8, 1, 0)
    fp.InsertNextPoint(8, 2, 0)
    fp.InsertNextPoint(10, 0.01, 0)
    fp.InsertNextPoint(8, -2, 0)
    fp.InsertNextPoint(8, -1, 0)
    fp.InsertNextPoint(0, -1, 0)
    ca.InsertNextCell(7)
    ca.InsertCellPoint(0)
    ca.InsertCellPoint(1)
    ca.InsertCellPoint(2)
    ca.InsertCellPoint(3)
    ca.InsertCellPoint(4)
    ca.InsertCellPoint(5)
    ca.InsertCellPoint(6)
    pd.SetPoints(fp)
    pd.SetPolys(ca)

    pd2 = vtkPolyData()
    ca2 = vtkCellArray()
    fp2 = vtkPoints()
    fp2.InsertNextPoint(0, 1, 0)
    fp2.InsertNextPoint(8, 1, 0)
    fp2.InsertNextPoint(8, 2, 0)
    fp2.InsertNextPoint(10, 0.01, 0)
    ca2.InsertNextCell(4)
    ca2.InsertCellPoint(0)
    ca2.InsertCellPoint(1)
    ca2.InsertCellPoint(2)
    ca2.InsertCellPoint(3)
    pd2.SetPoints(fp2)
    pd2.SetLines(ca2)

    arrowIM = vtkImplicitModeller()
    arrowIM.SetInputData(pd)
    arrowIM.SetSampleDimensions(50, 20, 8)

    arrowCF = vtkContourFilter()
    arrowCF.SetInputConnection(arrowIM.GetOutputPort())
    arrowCF.SetValue(0, 0.2)

    arrowWT = vtkWarpTo()
    arrowWT.SetInputConnection(arrowCF.GetOutputPort())
    arrowWT.SetPosition(5, 0, 5)
    arrowWT.SetScaleFactor(0.85)
    arrowWT.AbsoluteOn()

    arrowT = vtkTransform()
    arrowT.RotateY(60)
    arrowT.Translate(-1.33198, 0, -1.479)
    arrowT.Scale(1, 0.5, 1)

    arrowTF = vtkTransformFilter()
    arrowTF.SetInputConnection(arrowWT.GetOutputPort())
    arrowTF.SetTransform(arrowT)

    arrowMapper = vtkDataSetMapper()
    arrowMapper.SetInputConnection(arrowTF.GetOutputPort())
    arrowMapper.ScalarVisibilityOff()

    # Draw the azimuth arrows.
    a1Actor = vtkLODActor()
    a1Actor.SetMapper(arrowMapper)
    a1Actor.RotateZ(180)
    a1Actor.SetPosition(1, 0, -1)
    a1Actor.GetProperty().SetColor(colors.GetColor3d('AzimuthArrowColor'))
    a1Actor.GetProperty().SetSpecularColor(colors.GetColor3d('White'))
    a1Actor.GetProperty().SetSpecular(0.3)
    a1Actor.GetProperty().SetSpecularPower(20)
    a1Actor.GetProperty().SetAmbient(0.2)
    a1Actor.GetProperty().SetDiffuse(0.8)

    a2Actor = vtkLODActor()
    a2Actor.SetMapper(arrowMapper)
    a2Actor.RotateZ(180)
    a2Actor.RotateX(180)
    a2Actor.SetPosition(1, 0, 1)
    a2Actor.GetProperty().SetColor(colors.GetColor3d('AzimuthArrowColor'))
    a2Actor.GetProperty().SetSpecularColor(colors.GetColor3d('White'))
    a2Actor.GetProperty().SetSpecular(0.3)
    a2Actor.GetProperty().SetSpecularPower(20)
    a2Actor.GetProperty().SetAmbient(0.2)
    a2Actor.GetProperty().SetDiffuse(0.8)

    # Draw the elevation arrows.
    a3Actor = vtkLODActor()
    a3Actor.SetMapper(arrowMapper)
    a3Actor.RotateZ(180)
    a3Actor.RotateX(90)
    a3Actor.SetPosition(1, -1, 0)
    a3Actor.GetProperty().SetColor(colors.GetColor3d('ElevationArrowColor'))
    a3Actor.GetProperty().SetSpecularColor(colors.GetColor3d('White'))
    a3Actor.GetProperty().SetSpecular(0.3)
    a3Actor.GetProperty().SetSpecularPower(20)
    a3Actor.GetProperty().SetAmbient(0.2)
    a3Actor.GetProperty().SetDiffuse(0.8)

    a4Actor = vtkLODActor()
    a4Actor.SetMapper(arrowMapper)
    a4Actor.RotateZ(180)
    a4Actor.RotateX(-90)
    a4Actor.SetPosition(1, 1, 0)
    a4Actor.GetProperty().SetColor(colors.GetColor3d('ElevationArrowColor'))
    a4Actor.GetProperty().SetSpecularColor(colors.GetColor3d('White'))
    a4Actor.GetProperty().SetSpecular(0.3)
    a4Actor.GetProperty().SetSpecularPower(20)
    a4Actor.GetProperty().SetAmbient(0.2)
    a4Actor.GetProperty().SetDiffuse(0.8)

    # Draw the DOP.
    arrowT2 = vtkTransform()
    arrowT2.Scale(1, 0.6, 1)
    arrowT2.RotateY(90)

    arrowTF2 = vtkTransformPolyDataFilter()
    arrowTF2.SetInputData(pd2)
    arrowTF2.SetTransform(arrowT2)

    arrowREF = vtkRotationalExtrusionFilter()
    arrowREF.SetInputConnection(arrowTF2.GetOutputPort())
    arrowREF.CappingOff()
    arrowREF.SetResolution(30)

    spikeMapper = vtkPolyDataMapper()
    spikeMapper.SetInputConnection(arrowREF.GetOutputPort())

    a5Actor = vtkLODActor()
    a5Actor.SetMapper(spikeMapper)
    a5Actor.SetScale(.3, .3, .6)
    a5Actor.RotateY(90)
    a5Actor.SetPosition(-2, 0, 0)
    a5Actor.GetProperty().SetColor(colors.GetColor3d('SpikeColor'))
    a5Actor.GetProperty().SetAmbient(0.2)
    a5Actor.GetProperty().SetDiffuse(0.8)

    # Focal point.
    fps = vtkSphereSource()
    fps.SetRadius(0.5)
    fpMapper = vtkPolyDataMapper()
    fpMapper.SetInputConnection(fps.GetOutputPort())
    fpActor = vtkLODActor()
    fpActor.SetMapper(fpMapper)
    fpActor.SetPosition(-9, 0, 0)
    fpActor.GetProperty().SetSpecularColor(colors.GetColor3d('White'))
    fpActor.GetProperty().SetSpecular(0.3)
    fpActor.GetProperty().SetAmbient(0.2)
    fpActor.GetProperty().SetDiffuse(0.8)
    fpActor.GetProperty().SetSpecularPower(20)

    # Create the roll arrows.
    arrowWT2 = vtkWarpTo()
    arrowWT2.SetInputConnection(arrowCF.GetOutputPort())
    arrowWT2.SetPosition(5, 0, 2.5)
    arrowWT2.SetScaleFactor(0.95)
    arrowWT2.AbsoluteOn()

    arrowT3 = vtkTransform()
    arrowT3.Translate(-2.50358, 0, -1.70408)
    arrowT3.Scale(0.5, 0.3, 1)

    arrowTF3 = vtkTransformFilter()
    arrowTF3.SetInputConnection(arrowWT2.GetOutputPort())
    arrowTF3.SetTransform(arrowT3)

    arrowMapper2 = vtkDataSetMapper()
    arrowMapper2.SetInputConnection(arrowTF3.GetOutputPort())
    arrowMapper2.ScalarVisibilityOff()

    # Draw the roll arrows.
    a6Actor = vtkLODActor()
    a6Actor.SetMapper(arrowMapper2)
    a6Actor.RotateZ(90)
    a6Actor.SetPosition(-4, 0, 0)
    a6Actor.SetScale(1.5, 1.5, 1.5)
    a6Actor.GetProperty().SetColor(colors.GetColor3d('RollArrowColor'))
    a6Actor.GetProperty().SetSpecularColor(colors.GetColor3d('White'))
    a6Actor.GetProperty().SetSpecular(0.3)
    a6Actor.GetProperty().SetSpecularPower(20)
    a6Actor.GetProperty().SetAmbient(0.2)
    a6Actor.GetProperty().SetDiffuse(0.8)

    # Add the actors to the renderer, set the background and size.
    ren.AddActor(camActor)
    ren.AddActor(a1Actor)
    ren.AddActor(a2Actor)
    ren.AddActor(a3Actor)
    ren.AddActor(a4Actor)
    ren.AddActor(a5Actor)
    ren.AddActor(a6Actor)
    ren.AddActor(fpActor)
    ren.SetBackground(colors.GetColor3d('BkgColor'))
    ren.SetBackground(colors.GetColor3d('SlateGray'))
    renWin.SetSize(640, 480)
    renWin.SetWindowName('CameraModel1')

    # Render the image.

    cam1 = (ren.GetActiveCamera())
    ren.ResetCamera()
    cam1.Azimuth(150)
    cam1.Elevation(30)
    cam1.Dolly(1.5)
    ren.ResetCameraClippingRange()

    # Create a TextActor for azimuth  (a1 and a2 actor's color).
    text = vtkTextActor()
    text.SetInput('Azimuth')
    tprop = text.GetTextProperty()
    tprop.SetFontFamilyToArial()
    tprop.ShadowOff()
    tprop.SetLineSpacing(1.0)
    tprop.SetFontSize(36)
    tprop.SetColor(a1Actor.GetProperty().GetColor())
    text.SetDisplayPosition(20, 50)
    ren.AddActor2D(text)

    # Create a TextActor for elevation  (a3 and a4 actor's color).
    text2 = vtkTextActor()
    text2.SetInput('Elevation')
    tprop = text2.GetTextProperty()
    tprop.SetFontFamilyToArial()
    tprop.ShadowOff()
    tprop.SetLineSpacing(1.0)
    tprop.SetFontSize(36)
    tprop.SetColor(a3Actor.GetProperty().GetColor())
    text2.SetDisplayPosition(20, 100)
    ren.AddActor2D(text2)

    # Create a TextActor for roll (a6 actor's color).
    text3 = vtkTextActor()
    text3.SetInput('Roll')
    tprop = text3.GetTextProperty()
    tprop.SetFontFamilyToArial()
    tprop.ShadowOff()
    tprop.SetLineSpacing(1.0)
    tprop.SetFontSize(36)
    tprop.SetColor(a6Actor.GetProperty().GetColor())
    text3.SetDisplayPosition(20, 150)
    ren.AddActor2D(text3)

    iren.Initialize()
    iren.Start()


if __name__ == '__main__':
    main()
