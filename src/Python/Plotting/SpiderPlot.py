#!/usr/bin/env python

import vtk


def main():
    colors = vtk.vtkNamedColors()

    numTuples = 12

    bitter = vtk.vtkFloatArray()
    bitter.SetNumberOfTuples(numTuples)

    crispy = vtk.vtkFloatArray()
    crispy.SetNumberOfTuples(numTuples)

    crunchy = vtk.vtkFloatArray()
    crunchy.SetNumberOfTuples(numTuples)

    salty = vtk.vtkFloatArray()
    salty.SetNumberOfTuples(numTuples)

    oily = vtk.vtkFloatArray()
    oily.SetNumberOfTuples(numTuples)

    rand_seq = vtk.vtkMinimalStandardRandomSequence()
    rand_seq.SetSeed(8775070)

    for i in range(numTuples):
        bitter.SetTuple1(i, rand_seq.GetRangeValue(1, 10))
        rand_seq.Next()
        crispy.SetTuple1(i, rand_seq.GetRangeValue(-1, 1))
        rand_seq.Next()
        crunchy.SetTuple1(i, rand_seq.GetRangeValue(1, 100))
        rand_seq.Next()
        salty.SetTuple1(i, rand_seq.GetRangeValue(0, 10))
        rand_seq.Next()
        oily.SetTuple1(i, rand_seq.GetRangeValue(5, 25))
        rand_seq.Next()

    dobj = vtk.vtkDataObject()
    dobj.GetFieldData().AddArray(bitter)
    dobj.GetFieldData().AddArray(crispy)
    dobj.GetFieldData().AddArray(crunchy)
    dobj.GetFieldData().AddArray(salty)
    dobj.GetFieldData().AddArray(oily)

    actor = vtk.vtkSpiderPlotActor()
    actor.SetInputData(dobj)
    actor.SetTitle("Spider Plot")
    actor.SetIndependentVariablesToColumns()
    actor.GetPositionCoordinate().SetValue(0.05, 0.1, 0.0)
    actor.GetPosition2Coordinate().SetValue(0.95, 0.85, 0.0)
    actor.GetProperty().SetColor(colors.GetColor3d('Red'))

    actor.SetAxisLabel(0, "Bitter")
    actor.SetAxisRange(0, 1, 10)

    actor.SetAxisLabel(1, "Crispy")
    actor.SetAxisRange(1, -1, 1)

    actor.SetAxisLabel(2, "Crunchy")
    actor.SetAxisRange(2, 1, 100)

    actor.SetAxisLabel(3, "Salty")
    actor.SetAxisRange(3, 0, 10)

    actor.SetAxisLabel(4, "Oily")
    actor.SetAxisRange(4, 5, 25)
    actor.GetLegendActor().SetNumberOfEntries(numTuples)

    for i in range(numTuples):
        r = rand_seq.GetRangeValue(0.4, 1.0)
        rand_seq.Next()
        g = rand_seq.GetRangeValue(0.4, 1.0)
        rand_seq.Next()
        b = rand_seq.GetRangeValue(0.4, 1.0)
        rand_seq.Next()
        actor.SetPlotColor(i, r, g, b)

    actor.LegendVisibilityOn()

    actor.GetTitleTextProperty().SetColor(colors.GetColor3d('MistyRose'))
    actor.GetLabelTextProperty().SetColor(colors.GetColor3d('MistyRose'))

    ren1 = vtk.vtkRenderer()
    renWin = vtk.vtkRenderWindow()
    renWin.AddRenderer(ren1)
    iren = vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)
    ren1.AddActor(actor)
    ren1.SetBackground(colors.GetColor3d('DarkSlateGray'))
    renWin.SetSize(600, 500)
    renWin.SetWindowName('SpiderPlot')

    iren.Initialize()
    renWin.Render()
    iren.Start()


if __name__ == '__main__':
    main()
