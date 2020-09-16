import vtk


def main():
    colors = vtk.vtkNamedColors()
    ifn, index = get_program_parameters()

    # Prepare to read the file.
    readerVolume = vtk.vtkMetaImageReader()
    readerVolume.SetFileName(ifn)
    readerVolume.Update()

    # Extract the region of interest.
    voi = vtk.vtkExtractVOI()
    voi.SetInputConnection(readerVolume.GetOutputPort())
    voi.SetVOI(0, 517, 0, 228, 0, 392)
    voi.SetSampleRate(1, 1, 1)
    voi.Update()  # Necessary for GetScalarRange().
    srange = voi.GetOutput().GetScalarRange()  # Needs Update() before!
    print("Range", srange)

    # Prepare surface generation.
    contour = vtk.vtkDiscreteMarchingCubes()  # For label images.
    contour.SetInputConnection(voi.GetOutputPort())
    # contour.ComputeNormalsOn()

    print("Doing label", index)

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
    print("Smoother error range:", se_range)
    minz = se_range[0]  # min(smoother_error)
    maxz = se_range[1]  # max(smoother_error)
    if maxz > 1:
        print("Big smoother error: min/max:", minz, maxz)
    # minz = 0.3  # This way colours of different particles are comparable.
    # maxz = 1
    # minz = 0.3
    # maxz = 0.6
    minz = 3.25
    maxz = 3.85

    # Create the color map.
    colorLookupTable = vtk.vtkLookupTable()
    colorLookupTable.SetTableRange(minz, maxz)  # This does nothing, use mapper.SetScalarRange(minz, maxz).
    colorLookupTable.SetHueRange(2 / 3.0, 1)
    # colorLookupTable.SetSaturationRange(0, 0)
    # colorLookupTable.SetValueRange(1, 0)
    # colorLookupTable.SetNumberOfColors(256) #256 default
    colorLookupTable.Build()

    # Calculate cell normals.
    triangleCellNormals = vtk.vtkPolyDataNormals()
    triangleCellNormals.SetInputData(smoothed_polys)
    triangleCellNormals.ComputeCellNormalsOn()
    triangleCellNormals.ComputePointNormalsOff()
    triangleCellNormals.ConsistencyOn()
    triangleCellNormals.AutoOrientNormalsOn()
    triangleCellNormals.Update()  # Creates vtkPolyData.

    mapper = vtk.vtkPolyDataMapper()
    # mapper.SetInput(smoothed_polys) # This has no normals.
    mapper.SetInputConnection(triangleCellNormals.GetOutputPort())  # this is better for vis;-)
    mapper.ScalarVisibilityOn()  # Show colour.
    mapper.SetScalarRange(minz, maxz)
    # mapper.SetScalarModeToUseCellData() # Contains the label eg. 31
    mapper.SetScalarModeToUsePointData()  # The smoother error relates to the verts.
    mapper.SetLookupTable(colorLookupTable)

    # Take the isosurface data and create geometry.
    actor = vtk.vtkLODActor()
    actor.SetNumberOfCloudPoints(100000)
    actor.SetMapper(mapper)

    # Create the renderer.
    ren = vtk.vtkRenderer()
    ren.SetBackground(colors.GetColor3d("DimGray"))
    ren.AddActor(actor)

    # Create a window for the renderer of size 600X600
    renWin = vtk.vtkRenderWindow()
    renWin.AddRenderer(ren)
    renWin.SetSize(600, 600)

    # Set a user interface interactor for the render window.
    iren = vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)

    # Start the initialization and rendering.
    iren.Initialize()
    ren.GetActiveCamera().SetPosition(-0.004332, -1.771289, -0.754580)
    ren.GetActiveCamera().SetFocalPoint(0.000271, -0.001974, 0.006892)
    ren.GetActiveCamera().SetViewUp(0.790211, -0.243999, 0.562166)
    ren.ResetCameraClippingRange()
    renWin.Render()
    renWin.SetWindowName('MeshLabelImageColor')
    renWin.Render()

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


if __name__ == '__main__':
    main()
