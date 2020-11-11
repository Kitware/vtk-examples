#!/usr/bin/env python

"""
=========================================================================

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

"""

# First access the VTK module (and any other needed modules) by importing them.
import vtk


def main(argv):
    colors = vtk.vtkNamedColors()

    #
    # The pipeline creation is documented in Tutorial_Step1.
    #
    cone = vtk.vtkConeSource()
    cone.SetHeight(3.0)
    cone.SetRadius(1.0)
    cone.SetResolution(10)

    coneMapper = vtk.vtkPolyDataMapper()
    coneMapper.SetInputConnection(cone.GetOutputPort())
    coneActor = vtk.vtkActor()
    coneActor.SetMapper(coneMapper)
    coneActor.GetProperty().SetColor(colors.GetColor3d('MistyRose'))

    ren1 = vtk.vtkRenderer()
    ren1.AddActor(coneActor)
    ren1.SetBackground(colors.GetColor3d('MidnightBlue'))
    ren1.ResetCamera()

    renWin = vtk.vtkRenderWindow()
    renWin.AddRenderer(ren1)
    renWin.SetSize(300, 300)
    renWin.SetWindowName('Tutorial_Step2')

    # Here is where we setup the observer.
    mo1 = vtkMyCallback(ren1)
    ren1.AddObserver('StartEvent', mo1)

    #
    # Now we loop over 360 degrees and render the cone each time.
    #
    for i in range(0, 360):
        # Render the image.
        renWin.Render()
        # Rotate the active camera by one degree.
        ren1.GetActiveCamera().Azimuth(1)


class vtkMyCallback(object):
    """
    Callback for the interaction.
    """

    def __init__(self, renderer):
        self.renderer = renderer

    def __call__(self, caller, ev):
        position = self.renderer.GetActiveCamera().GetPosition()
        print('({:5.2f}, {:5.2f}, {:5.2f})'.format(*position))


if __name__ == '__main__':
    import sys

    main(sys.argv)
