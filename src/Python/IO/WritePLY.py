# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkIOPLY import (
    vtkPLYReader,
    vtkPLYWriter
)
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def get_program_parameters():
    import argparse
    description = 'Generate image data, then write a .ply file.'
    epilogue = '''
   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue)
    parser.add_argument('filename', help='A required ply filename.', nargs='?',
                        const='TestWritePLY.ply',
                        type=str, default='TestWritePLY.ply')
    args = parser.parse_args()
    return args.filename


def main():
    colors = vtkNamedColors()

    filename = get_program_parameters()

    sphereSource = vtkSphereSource()
    sphereSource.Update()

    plyWriter = vtkPLYWriter()
    plyWriter.SetFileName(filename)
    plyWriter.SetInputConnection(sphereSource.GetOutputPort())
    plyWriter.Write()

    # Read and display for verification
    reader = vtkPLYReader()
    reader.SetFileName(filename)
    reader.Update()

    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(reader.GetOutputPort())

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(colors.GetColor3d('MistyRose'))

    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindow.SetWindowName('WritePLY')

    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('cobalt_green'))

    renderWindow.Render()
    renderWindowInteractor.Start()


if __name__ == '__main__':
    main()
