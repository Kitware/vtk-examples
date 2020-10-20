import vtk


def main():
    colors = vtk.vtkNamedColors()

    # Create a rendering window and renderer
    ren = vtk.vtkRenderer()
    renWin = vtk.vtkRenderWindow()
    renWin.AddRenderer(ren)
    renWin.SetSize(640, 480)
    renWin.SetWindowName('RotationAroundLine')

    # Create a renderwindowinteractor
    iren = vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Create arrow
    source = vtk.vtkArrowSource()

    # Create a transform that rotates the arrow 45° around the z-axis
    transform = vtk.vtkTransform()
    transform.RotateWXYZ(45, 0, 0, 1)
    transformFilter = vtk.vtkTransformPolyDataFilter()
    transformFilter.SetTransform(transform)
    transformFilter.SetInputConnection(source.GetOutputPort())
    transformFilter.Update()

    # Mapper for the original arrow
    coneMapper1 = vtk.vtkPolyDataMapper()
    coneMapper1.SetInputConnection(source.GetOutputPort())

    # Another mapper for the rotated arrow
    coneMapper2 = vtk.vtkPolyDataMapper()
    coneMapper2.SetInputConnection(transformFilter.GetOutputPort())

    # Actor for original arrow
    actor1 = vtk.vtkActor()
    actor1.SetMapper(coneMapper1)

    # Actor for rotated arrow
    actor2 = vtk.vtkActor()
    actor2.SetMapper(coneMapper2)

    # Color the original arrow
    actor1.GetProperty().SetColor(colors.GetColor3d('LightCoral'))
    # Color rotated arrow
    actor2.GetProperty().SetColor(colors.GetColor3d('PaleTurquoise'))

    # Assign actor to the renderer
    ren.AddActor(actor1)
    ren.AddActor(actor2)
    ren.SetBackground(colors.GetColor3d('SlateGray'));

    # Enable the user interface interactor
    iren.Initialize()
    renWin.Render()
    iren.Start()


if __name__ == '__main__':
    main()
