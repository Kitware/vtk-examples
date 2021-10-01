import vtkmodules.all as vtk


def main():
    colors = vtk.vtkNamedColors()

    sphereSource = vtk.vtkSphereSource()
    sphereSource.Update()

    input_data = vtk.vtkPolyData()
    input_data.ShallowCopy(sphereSource.GetOutput())

    arrowSource = vtk.vtkArrowSource()

    glyph3D = vtk.vtkGlyph3D()
    glyph3D.SetSourceConnection(arrowSource.GetOutputPort())
    glyph3D.SetVectorModeToUseNormal()
    glyph3D.SetInputData(input_data)
    glyph3D.SetScaleFactor(.2)
    glyph3D.Update()

    # Visualize
    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputConnection(glyph3D.GetOutputPort())

    actor = vtk.vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('Gold'))

    renderer = vtk.vtkRenderer()
    renderWindow = vtk.vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindow.SetWindowName('OrientedGlyphs')

    renderWindowInteractor = vtk.vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('DarkGreen'))

    renderWindow.Render()

    camera = renderer.GetActiveCamera()
    camera.SetPosition(-0.399941, -1.070475, 2.931458)
    camera.SetFocalPoint(-0.000000, -0.000000, 0.000000)
    camera.SetViewUp(-0.028450, 0.940195, 0.339448)
    camera.SetDistance(3.146318)
    camera.SetClippingRange(1.182293, 5.626211)

    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
