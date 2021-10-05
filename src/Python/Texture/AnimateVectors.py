#!/usr/bin/env python

from pathlib import Path

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersCore import (
    vtkGlyph3D,
    vtkThresholdPoints
)
from vtkmodules.vtkFiltersModeling import vtkOutlineFilter
from vtkmodules.vtkFiltersSources import vtkLineSource
from vtkmodules.vtkIOLegacy import vtkStructuredPointsReader
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkCamera,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkTexture
)


def main():
    vec_anim_paths = [''] * 2
    fn1, fn2 = get_program_parameters()
    vec_anim_paths[0] = Path(fn1)
    vec_anim_paths[1] = Path(fn2)

    # Generate the other vecAnim file names. There are 8 of them.
    tmp = str(vec_anim_paths[1])
    old_stem = vec_anim_paths[1].stem
    for i in range(2, 9):
        new_stem = old_stem[:-1] + str(i)
        vec_anim_paths.append(Path(tmp.replace(old_stem, new_stem)))

    colors = vtkNamedColors()

    # Setup the render window, renderer, and interactor.
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)

    # Read the data.

    # Create the pipeline.

    reader = vtkStructuredPointsReader()
    reader.SetFileName(vec_anim_paths[0])

    threshold = vtkThresholdPoints()
    threshold.SetInputConnection(reader.GetOutputPort())
    threshold.ThresholdByUpper(200)

    line = vtkLineSource()
    line.SetResolution(1)

    lines = vtkGlyph3D()
    lines.SetInputConnection(threshold.GetOutputPort())
    lines.SetSourceConnection(line.GetOutputPort())
    lines.SetScaleFactor(0.005)
    lines.SetScaleModeToScaleByScalar()
    lines.Update()

    vectorMapper = vtkPolyDataMapper()
    vectorMapper.SetInputConnection(lines.GetOutputPort())
    vectorMapper.SetScalarRange(lines.GetOutput().GetScalarRange())

    vectorActor = vtkActor()
    vectorActor.SetMapper(vectorMapper)
    vectorActor.GetProperty().SetOpacity(0.99)
    vectorActor.GetProperty().SetLineWidth(1.5)

    # Outline
    outline = vtkOutlineFilter()
    outline.SetInputConnection(reader.GetOutputPort())

    outlineMapper = vtkPolyDataMapper()
    outlineMapper.SetInputConnection(outline.GetOutputPort())

    outlineActor = vtkActor()
    outlineActor.SetMapper(outlineMapper)
    outlineActor.GetProperty().SetColor(colors.GetColor3d('Black'))

    #  Texture maps.
    textureMaps = list()
    for i in range(2, len(vec_anim_paths)):
        tmap = vtkStructuredPointsReader()
        tmap.SetFileName(vec_anim_paths[i])

        texture = vtkTexture()
        texture.SetInputConnection(tmap.GetOutputPort())
        texture.InterpolateOff()
        texture.RepeatOff()
        textureMaps.append(texture)

    vectorActor.SetTexture(textureMaps[0])

    # Add the actors to the renderer, set the background and size.

    renderer.AddActor(vectorActor)
    renderer.AddActor(outlineActor)

    cam1 = vtkCamera()
    cam1.SetClippingRange(17.4043, 870.216)
    cam1.SetFocalPoint(136.71, 104.025, 23)
    cam1.SetPosition(204.747, 258.939, 63.7925)
    cam1.SetViewUp(-0.102647, -0.210897, 0.972104)
    cam1.Zoom(1.2)
    renderer.SetActiveCamera(cam1)

    renderer.SetBackground(colors.GetColor3d('Wheat'))
    renderWindow.SetSize(640, 480)
    renderWindow.SetWindowName('AnimateVectors')

    # Go into a loop.
    for j in range(0, 100):
        for i in range(0, len(textureMaps)):
            vectorActor.SetTexture(textureMaps[i])
            renderWindow.Render()
    interactor.Start()


def get_program_parameters():
    import argparse
    description = 'Texture maps can be animated as a function of time.'
    epilogue = '''
    This example uses texture map animation to simulate vector field motion.
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filename1', help='carotid.')
    parser.add_argument('filename2', help='VectorAnimation/vecAnim1.vtk.')
    args = parser.parse_args()
    return args.filename1, args.filename2


if __name__ == '__main__':
    main()
