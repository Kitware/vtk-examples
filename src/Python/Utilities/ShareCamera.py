import vtk


def main():
    colors = vtk.vtkNamedColors()

    # We store background colors in a vector. Then we extract the red, green and
    # blue components later when coloring the reneder background.
    rendererColors = list()
    colorSeries = vtk.vtkColorSeries()
    colorSeries.SetColorSchemeByName('Brewer Qualitative Pastel2')
    rendererColors.append(colorSeries.GetColor(0))
    rendererColors.append(colorSeries.GetColor(1))
    rendererColors.append(colorSeries.GetColor(2))
    rendererColors.append(colorSeries.GetColor(3))

    renderWindow = vtk.vtkRenderWindow()

    renderWindowInteractor = vtk.vtkRenderWindowInteractor()

    renderWindowInteractor.SetRenderWindow(renderWindow)

    # Define viewport ranges
    xmins = [0, 0.5, 0, 0.5]
    xmaxs = [0.5, 1, 0.5, 1]
    ymins = [0, 0, 0.5, 0.5]
    ymaxs = [0.5, 0.5, 1, 1]

    # Using the superclass for the sources
    sources = list()

    for i in range(0, 4):
        if i == 0:
            # Create a sphere
            sphereSource = vtk.vtkSphereSource()
            sphereSource.SetCenter(0.0, 0.0, 0.0)
            sphereSource.Update()
            sources.append(sphereSource)
        elif i == 1:
            # Create a cone
            coneSource = vtk.vtkConeSource()
            coneSource.SetCenter(0.0, 0.0, 0.0)
            coneSource.Update()
            sources.append(coneSource)
        elif i == 2:
            # Create a cube
            cubeSource = vtk.vtkCubeSource()
            cubeSource.SetCenter(0.0, 0.0, 0.0)
            cubeSource.Update()
            sources.append(cubeSource)
        else:
            # Create a cylinder
            cylinderSource = vtk.vtkCylinderSource()
            cylinderSource.SetCenter(0.0, 0.0, 0.0)
            cylinderSource.Update()
            sources.append(cylinderSource)

        mapper = vtk.vtkPolyDataMapper()
        mapper.SetInputConnection(sources[i].GetOutputPort())

        actor = vtk.vtkActor()
        actor.SetMapper(mapper)
        actor.GetProperty().SetColor(colors.GetColor3d('Tomato'))

        renderer = vtk.vtkRenderer()
        renderer.AddActor(actor)
        r = rendererColors[i].GetRed() / 255.0
        g = rendererColors[i].GetGreen() / 255.0
        b = rendererColors[i].GetBlue() / 255.0
        renderer.SetBackground(r, g, b)

        renderWindow.AddRenderer(renderer)

        if i == 0:
            camera = renderer.GetActiveCamera()
            camera.Azimuth(30)
            camera.Elevation(30)
        else:
            renderer.SetActiveCamera(camera)

        renderer.SetViewport(xmins[i], ymins[i], xmaxs[i], ymaxs[i])

        renderer.ResetCamera()

    renderWindow.Render()
    renderWindow.SetWindowName('ShareCamera')

    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
