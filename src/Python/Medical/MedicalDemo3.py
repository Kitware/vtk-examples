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
from vtkmodules.vtkFiltersCore import (
    vtkFlyingEdges3D,
    vtkMarchingCubes,
    vtkStripper
)
from vtkmodules.vtkFiltersModeling import vtkOutlineFilter
from vtkmodules.vtkIOImage import vtkMetaImageReader
from vtkmodules.vtkImagingCore import vtkImageMapToColors
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkCamera,
    vtkImageActor,
    vtkPolyDataMapper,
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
    colors.SetColor('BkgColor', [51, 77, 102, 255])

    # Create the renderer, the render window, and the interactor. The
    # renderer draws into the render window, the interactor enables
    # mouse- and keyboard-based interaction with the data within the
    # render window.
    #
    a_renderer = vtkRenderer()
    ren_win = vtkRenderWindow()
    ren_win.AddRenderer(a_renderer)
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    # Set a background color for the renderer and set the size of the
    # render window (expressed in pixels).
    a_renderer.SetBackground(colors.GetColor3d('BkgColor'))
    ren_win.SetSize(640, 480)

    # The following reader is used to read a series of 2D slices (images)
    # that compose the volume. The slice dimensions are set, and the
    # pixel spacing. The data Endianness must also be specified. The
    # reader uses the FilePrefix in combination with the slice number to
    # construct filenames using the format FilePrefix.%d. (In this case
    # the FilePrefix is the root name of the file: quarter.)
    reader = vtkMetaImageReader()
    reader.SetFileName(file_name)
    reader.Update()

    # An isosurface, or contour value of 500 is known to correspond to
    # the skin of the patient.
    # The triangle stripper is used to create triangle
    # strips from the isosurface these render much faster on may
    # systems.
    if use_flying_edges:
        try:
            skin_extractor = vtkFlyingEdges3D()
        except AttributeError:
            skin_extractor = vtkMarchingCubes()
    else:
        skin_extractor = vtkMarchingCubes()
    skin_extractor.SetInputConnection(reader.GetOutputPort())
    skin_extractor.SetValue(0, 500)
    skin_extractor.Update()

    skin_stripper = vtkStripper()
    skin_stripper.SetInputConnection(skin_extractor.GetOutputPort())
    skin_stripper.Update()

    skin_mapper = vtkPolyDataMapper()
    skin_mapper.SetInputConnection(skin_stripper.GetOutputPort())
    skin_mapper.ScalarVisibilityOff()

    skin = vtkActor()
    skin.SetMapper(skin_mapper)
    skin.GetProperty().SetDiffuseColor(colors.GetColor3d('SkinColor'))
    skin.GetProperty().SetSpecular(0.3)
    skin.GetProperty().SetSpecularPower(20)

    # An isosurface, or contour value of 1150 is known to correspond to
    # the bone of the patient.
    # The triangle stripper is used to create triangle
    # strips from the isosurface these render much faster on may
    # systems.
    if use_flying_edges:
        try:
            bone_extractor = vtkFlyingEdges3D()
        except AttributeError:
            bone_extractor = vtkMarchingCubes()
    else:
        bone_extractor = vtkMarchingCubes()
    bone_extractor.SetInputConnection(reader.GetOutputPort())
    bone_extractor.SetValue(0, 1150)

    bone_stripper = vtkStripper()
    bone_stripper.SetInputConnection(bone_extractor.GetOutputPort())

    bone_mapper = vtkPolyDataMapper()
    bone_mapper.SetInputConnection(bone_stripper.GetOutputPort())
    bone_mapper.ScalarVisibilityOff()

    bone = vtkActor()
    bone.SetMapper(bone_mapper)
    bone.GetProperty().SetDiffuseColor(colors.GetColor3d('Ivory'))

    # An outline provides context around the data.
    #
    outline_data = vtkOutlineFilter()
    outline_data.SetInputConnection(reader.GetOutputPort())
    outline_data.Update()

    map_outline = vtkPolyDataMapper()
    map_outline.SetInputConnection(outline_data.GetOutputPort())

    outline = vtkActor()
    outline.SetMapper(map_outline)
    outline.GetProperty().SetColor(colors.GetColor3d('Black'))

    # Now we are creating three orthogonal planes passing through the
    # volume. Each plane uses a different texture map and therefore has
    # different coloration.

    # Start by creating a black/white lookup table.
    bw_lut = vtkLookupTable()
    bw_lut.SetTableRange(0, 2000)
    bw_lut.SetSaturationRange(0, 0)
    bw_lut.SetHueRange(0, 0)
    bw_lut.SetValueRange(0, 1)
    bw_lut.Build()  # effective built

    # Now create a lookup table that consists of the full hue circle
    # (from HSV).
    hue_lut = vtkLookupTable()
    hue_lut.SetTableRange(0, 2000)
    hue_lut.SetHueRange(0, 1)
    hue_lut.SetSaturationRange(1, 1)
    hue_lut.SetValueRange(1, 1)
    hue_lut.Build()  # effective built

    # Finally, create a lookup table with a single hue but having a range
    # in the saturation of the hue.
    sat_lut = vtkLookupTable()
    sat_lut.SetTableRange(0, 2000)
    sat_lut.SetHueRange(0.6, 0.6)
    sat_lut.SetSaturationRange(0, 1)
    sat_lut.SetValueRange(1, 1)
    sat_lut.Build()  # effective built

    # Create the first of the three planes. The filter vtkImageMapToColors
    # maps the data through the corresponding lookup table created above.  The
    # vtkImageActor is a type of vtkProp and conveniently displays an image on
    # a single quadrilateral plane. It does this using texture mapping and as
    # a result is quite fast. (Note: the input image has to be unsigned char
    # values, which the vtkImageMapToColors produces.) Note also that by
    # specifying the DisplayExtent, the pipeline requests data of this extent
    # and the vtkImageMapToColors only processes a slice of data.
    sagittal_colors = vtkImageMapToColors()
    sagittal_colors.SetInputConnection(reader.GetOutputPort())
    sagittal_colors.SetLookupTable(bw_lut)
    sagittal_colors.Update()

    sagittal = vtkImageActor()
    sagittal.GetMapper().SetInputConnection(sagittal_colors.GetOutputPort())
    sagittal.SetDisplayExtent(128, 128, 0, 255, 0, 92)
    sagittal.ForceOpaqueOn()

    # Create the second (axial) plane of the three planes. We use the
    # same approach as before except that the extent differs.
    axial_colors = vtkImageMapToColors()
    axial_colors.SetInputConnection(reader.GetOutputPort())
    axial_colors.SetLookupTable(hue_lut)
    axial_colors.Update()

    axial = vtkImageActor()
    axial.GetMapper().SetInputConnection(axial_colors.GetOutputPort())
    axial.SetDisplayExtent(0, 255, 0, 255, 46, 46)
    axial.ForceOpaqueOn()

    # Create the third (coronal) plane of the three planes. We use
    # the same approach as before except that the extent differs.
    coronal_colors = vtkImageMapToColors()
    coronal_colors.SetInputConnection(reader.GetOutputPort())
    coronal_colors.SetLookupTable(sat_lut)
    coronal_colors.Update()

    coronal = vtkImageActor()
    coronal.GetMapper().SetInputConnection(coronal_colors.GetOutputPort())
    coronal.SetDisplayExtent(0, 255, 128, 128, 0, 92)
    coronal.ForceOpaqueOn()

    # It is convenient to create an initial view of the data. The
    # FocalPoint and Position form a vector direction. Later on
    # (ResetCamera() method) this vector is used to position the camera
    # to look at the data in this direction.
    a_camera = vtkCamera()
    a_camera.SetViewUp(0, 0, -1)
    a_camera.SetPosition(0, -1, 0)
    a_camera.SetFocalPoint(0, 0, 0)
    a_camera.ComputeViewPlaneNormal()
    a_camera.Azimuth(30.0)
    a_camera.Elevation(30.0)

    # Actors are added to the renderer.
    a_renderer.AddActor(outline)
    a_renderer.AddActor(sagittal)
    a_renderer.AddActor(axial)
    a_renderer.AddActor(coronal)
    a_renderer.AddActor(skin)
    a_renderer.AddActor(bone)

    # Turn off bone for this example.
    bone.VisibilityOff()

    # Set skin to semi-transparent.
    skin.GetProperty().SetOpacity(0.5)

    # An initial camera view is created.  The Dolly() method moves
    # the camera towards the FocalPoint, thereby enlarging the image.
    a_renderer.SetActiveCamera(a_camera)

    # Calling Render() directly on a vtkRenderer is strictly forbidden.
    # Only calling Render() on the vtkRenderWindow is a valid call.
    ren_win.SetWindowName('MedicalDemo3')
    ren_win.Render()

    a_renderer.ResetCamera()
    a_camera.Dolly(1.5)

    # Note that when camera movement occurs (as it does in the Dolly()
    # method), the clipping planes often need adjusting. Clipping planes
    # consist of two planes: near and far along the view direction. The
    # near plane clips out objects in front of the plane; the far plane
    # clips out objects behind the plane. This way only what is drawn
    # between the planes is actually rendered.
    a_renderer.ResetCameraClippingRange()

    # Interact with the data.
    ren_win.Render()
    iren.Initialize()
    iren.Start()


def get_program_parameters():
    import argparse
    description = 'The skin and bone is extracted from a CT dataset of the head.'
    epilogue = '''
    Derived from VTK/Examples/Cxx/Medical3.cxx
    This example reads a volume dataset, extracts two isosurfaces that
     represent the skin and bone, creates three orthogonal planes
     (sagittal, axial, coronal), and displays them.
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
