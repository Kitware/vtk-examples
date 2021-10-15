#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    vtkLookupTable,
    vtkVersion
)
from vtkmodules.vtkFiltersCore import (
    vtkPolyDataNormals,
    vtkWindowedSincPolyDataFilter
)
from vtkmodules.vtkFiltersGeneral import (
    vtkDiscreteFlyingEdges3D,
    vtkDiscreteMarchingCubes
)
from vtkmodules.vtkIOImage import vtkMetaImageReader
from vtkmodules.vtkImagingCore import vtkExtractVOI
from vtkmodules.vtkRenderingCore import (
    vtkColorTransferFunction,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)
from vtkmodules.vtkRenderingLOD import vtkLODActor


def main():
    # vtkFlyingEdges3D was introduced in VTK >= 8.2
    use_flying_edges = vtk_version_ok(8, 2, 0)

    colors = vtkNamedColors()
    ifn, index = get_program_parameters()

    # Prepare to read the file.
    reader_volume = vtkMetaImageReader()
    reader_volume.SetFileName(ifn)
    reader_volume.Update()

    # Extract the region of interest.
    voi = vtkExtractVOI()
    voi.SetInputConnection(reader_volume.GetOutputPort())
    voi.SetVOI(0, 517, 0, 228, 0, 392)
    voi.SetSampleRate(1, 1, 1)
    voi.Update()  # Necessary for GetScalarRange().
    srange = voi.GetOutput().GetScalarRange()  # Needs Update() before!
    print('Range', srange)

    # Prepare surface generation.
    # For label images.
    if use_flying_edges:
        try:
            contour = vtkDiscreteFlyingEdges3D()
        except AttributeError:
            contour = vtkDiscreteMarchingCubes()
    else:
        contour = vtkDiscreteMarchingCubes()
    contour.SetInputConnection(voi.GetOutputPort())
    # contour.ComputeNormalsOn()

    print('Doing label', index)

    contour.SetValue(0, index)
    contour.Update()  # Needed for GetNumberOfPolys()!!!

    smoother = vtkWindowedSincPolyDataFilter()
    smoother.SetInputConnection(contour.GetOutputPort())
    smoother.SetNumberOfIterations(30)  # This has little effect on the error!
    # smoother.BoundarySmoothingOff()
    # smoother.FeatureEdgeSmoothingOff()
    # smoother.SetFeatureAngle(120.0)
    # smoother.SetPassBand(0.001)        # This increases the error a lot!
    smoother.NonManifoldSmoothingOn()
    smoother.NormalizeCoordinatesOn()
    smoother.GenerateErrorScalarsOn()
    # smoother.GenerateErrorVectorsOn()
    smoother.Update()

    # Find min and max of the smoother error.
    se_range = smoother.GetOutput().GetPointData().GetScalars().GetRange()
    print('Smoother error range:', se_range)
    if se_range[1] > 1:
        print('Big smoother error: min/max:', se_range[0], se_range[1])

    lut = get_diverging_lut(4)

    # Calculate cell normals.
    normals = vtkPolyDataNormals()
    normals.SetInputConnection(smoother.GetOutputPort())
    normals.ComputeCellNormalsOn()
    normals.ComputePointNormalsOff()
    normals.ConsistencyOn()
    normals.AutoOrientNormalsOn()
    normals.Update()  # Creates vtkPolyData.
    normals.SetFeatureAngle(60.0)

    mapper = vtkPolyDataMapper()
    # mapper.SetInputConnection(smoother.GetOutputPort()) # This has no normals.
    mapper.SetInputConnection(normals.GetOutputPort())  # This is better for visibility.)
    mapper.ScalarVisibilityOn()  # Show colour.
    mapper.SetScalarRange(se_range)
    # mapper.SetScalarModeToUseCellData() # Contains the label eg. 31
    mapper.SetScalarModeToUsePointData()  # The smoother error relates to the verts.
    mapper.SetLookupTable(lut)

    # Take the isosurface data and create geometry.
    actor = vtkLODActor()
    actor.SetNumberOfCloudPoints(100000)
    actor.SetMapper(mapper)

    # Create the renderer.
    ren = vtkRenderer()
    ren.SetBackground(colors.GetColor3d('DimGray'))
    ren.AddActor(actor)

    # Create a window for the renderer of size 600X600
    ren_win = vtkRenderWindow()
    ren_win.AddRenderer(ren)
    ren_win.SetSize(600, 600)
    ren_win.SetWindowName('MeshLabelImageColor')
    ren_win.Render()

    # Set a user interface interactor for the render window.
    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    # Start the initialization and rendering.
    iren.Initialize()
    iren.Start()


def get_program_parameters():
    import argparse
    description = 'MeshLabelImageColor.'
    epilogue = '''
        Takes a label image in Meta format and meshes a single label of it.
   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue)
    parser.add_argument('filename', help='labels.mhd')
    parser.add_argument('label', nargs='?', const=1, type=int, default=31, help='The label to use e.g 31')
    args = parser.parse_args()
    return args.filename, args.label


def get_diverging_lut(ct=0):
    """
    See: [Diverging Color Maps for Scientific Visualization](https://www.kennethmoreland.com/color-maps/)

    :param ct: The index of the color map to use.
    :return: The lookup table.
    """

    cm = dict()
    # Start point = 0.0, mid point = 0.5 and end point = 1.0.
    # Each value is a list with three sublists corresponding to the start point,
    # mid point and end point along with the rgb color values for the respective point.
    # cool to warm
    cm[0] = [[0.0, 0.230, 0.299, 0.754], [0.5, 0.865, 0.865, 0.865], [1.0, 0.706, 0.016, 0.150]]
    # purple to orange
    cm[1] = [[0.0, 0.436, 0.308, 0.631], [0.5, 0.865, 0.865, 0.865], [1.0, 0.759, 0.334, 0.046]]
    # green to purple
    cm[2] = [[0.0, 0.085, 0.532, 0.201], [0.5, 0.865, 0.865, 0.865], [1.0, 0.436, 0.308, 0.631]]
    # blue to brown
    cm[3] = [[0.0, 0.217, 0.525, 0.910], [0.5, 0.865, 0.865, 0.865], [1.0, 0.677, 0.492, 0.093]]
    # green to red
    cm[4] = [[0.0, 0.085, 0.532, 0.201], [0.5, 0.865, 0.865, 0.865], [1.0, 0.758, 0.214, 0.233]]

    ct = abs(ct)
    if ct > len(cm) - 1:
        ct = 0
        print('The selected diverging color map is unavailable. Using the default cool to warm one.')

    ctf = vtkColorTransferFunction()
    ctf.SetColorSpaceToDiverging()
    for scheme in cm[ct]:
        ctf.AddRGBPoint(*scheme)

    table_size = 256
    lut = vtkLookupTable()
    lut.SetNumberOfTableValues(table_size)
    lut.Build()

    for i in range(0, table_size):
        rgba = list(ctf.GetColor(float(i) / table_size))
        rgba.append(1)
        lut.SetTableValue(i, rgba)

    return lut


def vtk_version_ok(major, minor, build):
    """
    Check the VTK version.

    :param major: Requested major version.
    :param minor: Requested minor version.
    :param build: Requested build version.
    :return: True if the requested VTK version is >= the actual VTK version.
    """
    requested_version = (100 * int(major) + int(minor)) * 100000000 + int(build)
    ver = vtkVersion()
    actual_version = (100 * ver.GetVTKMajorVersion() + ver.GetVTKMinorVersion()) \
                     * 100000000 + ver.GetVTKBuildVersion()
    if actual_version >= requested_version:
        return True
    else:
        return False


if __name__ == '__main__':
    main()
