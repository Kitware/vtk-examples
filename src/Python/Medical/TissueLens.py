#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    VTK_VERSION_NUMBER,
    vtkLookupTable,
    vtkVersion
)
from vtkmodules.vtkCommonDataModel import vtkSphere
from vtkmodules.vtkFiltersCore import (
    vtkFlyingEdges3D,
    vtkMarchingCubes,
    vtkProbeFilter
)
from vtkmodules.vtkFiltersGeneral import vtkClipDataSet
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkIOImage import vtkMetaImageReader
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkCamera,
    vtkDataSetMapper,
    vtkProperty,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    # vtkFlyingEdges3D was introduced in VTK >= 8.2
    use_flying_edges = vtk_version_ok(8, 2, 0)

    colors = vtkNamedColors()

    file_name = get_program_parameters()

    colors.SetColor('SkinColor', [240, 184, 160, 255])
    colors.SetColor('BackfaceColor', [255, 229, 200, 255])
    colors.SetColor('BkgColor', [51, 77, 102, 255])

    # Read the volume data
    reader = vtkMetaImageReader()
    reader.SetFileName(file_name)
    reader.Update()

    # An isosurface, or contour value of 500 is known to correspond to the
    # skin of the patient.
    if use_flying_edges:
        try:
            skin_extractor = vtkFlyingEdges3D()
        except AttributeError:
            skin_extractor = vtkMarchingCubes()
    else:
        skin_extractor = vtkMarchingCubes()
    skin_extractor.SetInputConnection(reader.GetOutputPort())
    skin_extractor.SetValue(0, 500)

    # Define a spherical clip function to clip the isosurface
    clip_function = vtkSphere()
    clip_function.SetRadius(50)
    clip_function.SetCenter(73, 52, 15)

    # Clip the isosurface with a sphere
    skin_clip = vtkClipDataSet()
    skin_clip.SetInputConnection(skin_extractor.GetOutputPort())
    skin_clip.SetClipFunction(clip_function)
    skin_clip.SetValue(0)
    skin_clip.GenerateClipScalarsOn()
    skin_clip.Update()

    skin_mapper = vtkDataSetMapper()
    skin_mapper.SetInputConnection(skin_clip.GetOutputPort())
    skin_mapper.ScalarVisibilityOff()

    skin = vtkActor()
    skin.SetMapper(skin_mapper)
    skin.GetProperty().SetDiffuseColor(colors.GetColor3d('SkinColor'))

    back_prop = vtkProperty()
    back_prop.SetDiffuseColor(colors.GetColor3d('BackfaceColor'))
    skin.SetBackfaceProperty(back_prop)

    # Define a model for the "lens". Its geometry matches the implicit
    # sphere used to clip the isosurface
    lens_model = vtkSphereSource()
    lens_model.SetRadius(50)
    lens_model.SetCenter(73, 52, 15)
    lens_model.SetPhiResolution(201)
    lens_model.SetThetaResolution(101)

    # Sample the input volume with the lens model geometry
    lens_probe = vtkProbeFilter()
    lens_probe.SetInputConnection(lens_model.GetOutputPort())
    lens_probe.SetSourceConnection(reader.GetOutputPort())

    # Clip the lens data with the isosurface value
    lens_clip = vtkClipDataSet()
    lens_clip.SetInputConnection(lens_probe.GetOutputPort())
    lens_clip.SetValue(500)
    lens_clip.GenerateClipScalarsOff()
    lens_clip.Update()

    # Define a suitable grayscale lut
    bw_lut = vtkLookupTable()
    bw_lut.SetTableRange(0, 2048)
    bw_lut.SetSaturationRange(0, 0)
    bw_lut.SetHueRange(0, 0)
    bw_lut.SetValueRange(0.2, 1)
    bw_lut.Build()

    lens_mapper = vtkDataSetMapper()
    lens_mapper.SetInputConnection(lens_clip.GetOutputPort())
    lens_mapper.SetScalarRange(lens_clip.GetOutput().GetScalarRange())
    lens_mapper.SetLookupTable(bw_lut)

    lens = vtkActor()
    lens.SetMapper(lens_mapper)

    # It is convenient to create an initial view of the data. The FocalPoint
    # and Position form a vector direction. Later on (ResetCamera() method)
    # this vector is used to position the camera to look at the data in
    # this direction.
    a_camera = vtkCamera()
    a_camera.SetViewUp(0, 0, -1)
    a_camera.SetPosition(0, -1, 0)
    a_camera.SetFocalPoint(0, 0, 0)
    a_camera.ComputeViewPlaneNormal()
    a_camera.Azimuth(30.0)
    a_camera.Elevation(30.0)

    # Create the renderer, the render window, and the interactor. The renderer
    # draws into the render window, the interactor enables mouse- and
    # keyboard-based interaction with the data within the render window.
    #
    a_renderer = vtkRenderer()
    ren_win = vtkRenderWindow()
    ren_win.AddRenderer(a_renderer)

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    # Actors are added to the renderer. An initial camera view is created.
    # The Dolly() method moves the camera towards the FocalPoint,
    # thereby enlarging the image.
    a_renderer.AddActor(lens)
    a_renderer.AddActor(skin)
    a_renderer.SetActiveCamera(a_camera)
    a_renderer.ResetCamera()
    a_camera.Dolly(1.5)

    # Set a background color for the renderer and set the size of the
    # render window (expressed in pixels).
    a_renderer.SetBackground(colors.GetColor3d('BkgColor'))
    ren_win.SetSize(640, 480)
    ren_win.SetWindowName('TissueLens')

    # Note that when camera movement occurs (as it does in the Dolly()
    # method), the clipping planes often need adjusting. Clipping planes
    # consist of two planes: near and far along the view direction. The
    # near plane clips out objects in front of the plane the far plane
    # clips out objects behind the plane. This way only what is drawn
    # between the planes is actually rendered.
    a_renderer.ResetCameraClippingRange()

    # Initialize the event loop and then start it.
    ren_win.Render()
    iren.Initialize()
    iren.Start()


def get_program_parameters():
    import argparse
    description = 'The skin and bone is extracted from a CT dataset of the head and a "tissue lens" effect is applied.'
    epilogue = '''
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filename', help='FullHead.mhd.')
    args = parser.parse_args()
    return args.filename


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
