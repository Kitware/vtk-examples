#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    VTK_VERSION_NUMBER,
    vtkVersion
)
from vtkmodules.vtkFiltersSources import vtkArrowSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)
from vtkmodules.vtkRenderingOpenGL2 import (
    vtkOutlineGlowPass,
    vtkRenderStepsPass
)


def get_program_parameters():
    import argparse
    description = 'How to render an object in a scene with a glowing outline.'
    epilogue = '''
Parts of a scene are highlighted by applying the render pass to a layered renderer
 on top of the main scene. For optimal results, actors that form the outline
 should be brightly colored with lighting disabled. The outline will have the
 color of the actors. There is only one outline around all objects rendered by the delegate.

When combined with layered renderers, this creates a very visible highlight without
 altering the highlighted object.
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.parse_args()
    return


def main():
    if not vtk_version_ok(9, 0, 20200909):
        print('You need VTK version 9.0.20200909 or greater to run this program.')
        return
    get_program_parameters()

    colors = vtkNamedColors()

    iren = vtkRenderWindowInteractor()
    renWin = vtkRenderWindow()
    renWin.SetMultiSamples(0)

    iren.SetRenderWindow(renWin)

    # Set up the renderers
    # One for the object and the other for the outline
    renderer = vtkRenderer()
    rendererOutline = vtkRenderer()
    rendererOutline.SetLayer(1)
    renWin.SetNumberOfLayers(2)
    renWin.AddRenderer(rendererOutline)
    renWin.AddRenderer(renderer)

    # Create an arrow.
    arrowSource = vtkArrowSource()
    # arrowSource.SetShaftRadius(1.0)
    # arrowSource.SetTipLength(1.0)
    arrowSource.Update()

    # Create mapper and actor for the main renderer
    coneMapperMain = vtkPolyDataMapper()
    coneMapperMain.SetInputConnection(arrowSource.GetOutputPort())

    coneActorMain = vtkActor()
    coneActorMain.SetMapper(coneMapperMain)
    coneActorMain.GetProperty().SetDiffuseColor(colors.GetColor3d("LimeGreen"))

    renderer.AddActor(coneActorMain)

    # Lets make the outline glow!
    # Create the render pass
    basicPasses = vtkRenderStepsPass()
    glowPass = vtkOutlineGlowPass()
    glowPass.SetDelegatePass(basicPasses)

    # Apply the render pass to the highlight renderer
    rendererOutline.SetPass(glowPass)

    # Create mapper and actor for the outline
    coneMapperOutline = vtkPolyDataMapper()
    coneMapperOutline.SetInputConnection(arrowSource.GetOutputPort())

    coneActorOutline = vtkActor()
    coneActorOutline.SetMapper(coneMapperOutline)
    coneActorOutline.GetProperty().SetColor(colors.GetColor3d("Magenta"))
    coneActorOutline.GetProperty().LightingOff()

    rendererOutline.AddActor(coneActorOutline)

    renWin.SetSize(600, 600)

    renderer.GradientBackgroundOn()
    renderer.SetBackground(colors.GetColor3d("DarkSlateGray"))
    renderer.SetBackground2(colors.GetColor3d("DarkSlateBlue"))

    renderer.ResetCamera()
    camera = renderer.GetActiveCamera()
    camera.Roll(45.0)
    camera.Azimuth(-30.0)
    camera.Elevation(-15.0)
    renderer.ResetCamera()
    # Now set the active camera for the outline
    rendererOutline.SetActiveCamera(camera)

    renWin.SetWindowName('OutlineGlowPass')

    renWin.Render()

    iren.Start()


def vtk_version_ok(major, minor, build):
    """
    Check the VTK version.

    :param major: Major version.
    :param minor: Minor version.
    :param build: Build version.
    :return: True if the requested VTK version is greater or equal to the actual VTK version.
    """
    needed_version = 10000000000 * int(major) + 100000000 * int(minor) + int(build)
    try:
        vtk_version_number = VTK_VERSION_NUMBER
    except AttributeError:  # as error:
        ver = vtkVersion()
        vtk_version_number = 10000000000 * ver.GetVTKMajorVersion() + 100000000 * ver.GetVTKMinorVersion() \
                             + ver.GetVTKBuildVersion()
    if vtk_version_number >= needed_version:
        return True
    else:
        return False


if __name__ == '__main__':
    main()
