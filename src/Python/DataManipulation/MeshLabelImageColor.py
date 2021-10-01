import vtkmodules.all as vtk


def main():
    # vtkFlyingEdges3D was introduced in VTK >= 8.2
    use_flying_edges = vtk_version_ok(8, 2, 0)

    colors = vtk.vtkNamedColors()
    ifn, index = get_program_parameters()

    # Prepare to read the file.
    reader_volume = vtk.vtkMetaImageReader()
    reader_volume.SetFileName(ifn)
    reader_volume.Update()

    # Extract the region of interest.
    voi = vtk.vtkExtractVOI()
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
            contour = vtk.vtkDiscreteFlyingEdges3D()
        except AttributeError:
            contour = vtk.vtkDiscreteMarchingCubes()
    else:
        contour = vtk.vtkDiscreteMarchingCubes()
    contour.SetInputConnection(voi.GetOutputPort())
    # contour.ComputeNormalsOn()

    print('Doing label', index)

    contour.SetValue(0, index)
    contour.Update()  # Needed for GetNumberOfPolys()!!!

    smoother = vtk.vtkWindowedSincPolyDataFilter()
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

    smoothed_polys = smoother.GetOutput()
    smoother_error = smoothed_polys.GetPointData().GetScalars()

    # Find min and max z.
    se_range = smoother_error.GetRange()
    print('Smoother error range:', se_range)
    minz = se_range[0]  # min(smoother_error)
    maxz = se_range[1]  # max(smoother_error)
    if maxz > 1:
        print('Big smoother error: min/max:', minz, maxz)
    # minz = 0.3  # This way colours of different particles are comparable.
    # maxz = 1
    # minz = 0.3
    # maxz = 0.6
    minz = 3.25
    maxz = 3.85

    # Create the color map.
    lut = vtk.vtkLookupTable()
    lut.SetTableRange(minz, maxz)  # This does nothing, use mapper.SetScalarRange(minz, maxz).
    lut.SetHueRange(2 / 3.0, 1)
    # lut.SetSaturationRange(0, 0)
    # lut.SetValueRange(1, 0)
    # lut.SetNumberOfColors(256) #256 default
    lut.Build()

    # Calculate cell normals.
    triangle_cell_normals = vtk.vtkPolyDataNormals()
    triangle_cell_normals.SetInputData(smoothed_polys)
    triangle_cell_normals.ComputeCellNormalsOn()
    triangle_cell_normals.ComputePointNormalsOff()
    triangle_cell_normals.ConsistencyOn()
    triangle_cell_normals.AutoOrientNormalsOn()
    triangle_cell_normals.Update()  # Creates vtkPolyData.

    mapper = vtk.vtkPolyDataMapper()
    # mapper.SetInput(smoothed_polys) # This has no normals.
    mapper.SetInputConnection(triangle_cell_normals.GetOutputPort())  # this is better for vis;-)
    mapper.ScalarVisibilityOn()  # Show colour.
    mapper.SetScalarRange(minz, maxz)
    # mapper.SetScalarModeToUseCellData() # Contains the label eg. 31
    mapper.SetScalarModeToUsePointData()  # The smoother error relates to the verts.
    mapper.SetLookupTable(lut)

    # Take the isosurface data and create geometry.
    actor = vtk.vtkLODActor()
    actor.SetNumberOfCloudPoints(100000)
    actor.SetMapper(mapper)

    # Create the renderer.
    ren = vtk.vtkRenderer()
    ren.SetBackground(colors.GetColor3d('DimGray'))
    ren.AddActor(actor)

    # Create a window for the renderer of size 600X600
    ren_win = vtk.vtkRenderWindow()
    ren_win.AddRenderer(ren)
    ren_win.SetSize(600, 600)

    # Set a user interface interactor for the render window.
    iren = vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    # Start the initialization and rendering.
    iren.Initialize()
    ren.GetActiveCamera().SetPosition(-0.004332, -1.771289, -0.754580)
    ren.GetActiveCamera().SetFocalPoint(0.000271, -0.001974, 0.006892)
    ren.GetActiveCamera().SetViewUp(0.790211, -0.243999, 0.562166)
    ren.ResetCameraClippingRange()
    ren_win.Render()
    ren_win.SetWindowName('MeshLabelImageColor')
    ren_win.Render()

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
        vtk_version_number = vtk.VTK_VERSION_NUMBER
    except AttributeError:  # as error:
        ver = vtk.vtkVersion()
        vtk_version_number = 10000000000 * ver.GetVTKMajorVersion() + 100000000 * ver.GetVTKMinorVersion() \
                             + ver.GetVTKBuildVersion()
    if vtk_version_number >= needed_version:
        return True
    else:
        return False


if __name__ == '__main__':
    main()
