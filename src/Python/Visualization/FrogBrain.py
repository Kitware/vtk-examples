#!/usr/bin/env python

"""
"""

import vtk


def main():
    colors = vtk.vtkNamedColors()

    file_name, tissue = get_program_parameters()

    tissue_map = create_tissue_map()
    lut = create_frog_lut()

    # Setup render window, renderer, and interactor.
    renderer_left = vtk.vtkRenderer()
    renderer_left.SetViewport(0, 0, .5, 1)
    renderer_right = vtk.vtkRenderer()
    renderer_right.SetViewport(.5, 0, 1, 1)
    render_window = vtk.vtkRenderWindow()
    render_window.AddRenderer(renderer_left)
    render_window.AddRenderer(renderer_right)
    render_window_interactor = vtk.vtkRenderWindowInteractor()
    render_window_interactor.SetRenderWindow(render_window)

    actor = create_frog_actor(file_name, tissue_map[tissue])
    actor.GetProperty().SetDiffuseColor(lut.GetTableValue(tissue_map[tissue])[:3])
    renderer_left.AddActor(actor)

    actorSmooth = create_smooth_frog_actor(file_name, tissue_map[tissue])
    actorSmooth.GetProperty().SetDiffuseColor(lut.GetTableValue(tissue_map[tissue])[:3])
    actorSmooth.GetProperty().SetDiffuse(1.0)
    actorSmooth.GetProperty().SetSpecular(.5)
    actorSmooth.GetProperty().SetSpecularPower(100)

    renderer_right.AddActor(actorSmooth)

    renderer_left.ResetCamera()
    renderer_left.GetActiveCamera().SetViewUp(-1, 0, 0)
    renderer_left.GetActiveCamera().Azimuth(180)
    renderer_left.ResetCameraClippingRange()

    renderer_left.SetBackground(colors.GetColor3d('LightSteelBlue'))
    renderer_right.SetBackground(colors.GetColor3d('LightSteelBlue'))
    renderer_right.SetActiveCamera(renderer_left.GetActiveCamera())

    render_window.SetSize(640, 480)
    render_window.SetWindowName('FrogBrain')
    render_window.Render()

    render_window_interactor.Start()


def get_program_parameters():
    import argparse
    description = 'The frog\'s brain.'
    epilogue = '''
    Model extracted without smoothing (left) and with smoothing (right).
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('file_name', help='frog.mhd.')
    parser.add_argument('tissue', default='brain', nargs='?', help='The tissue to use.')
    args = parser.parse_args()
    return args.file_name, args.tissue


def create_frog_lut():
    colors = vtk.vtkNamedColors()

    lut = vtk.vtkLookupTable()
    lut.SetNumberOfColors(17)
    lut.SetTableRange(0, 16)
    lut.Build()

    lut.SetTableValue(0, 0, 0, 0, 0)
    lut.SetTableValue(1, colors.GetColor4d('salmon'))  # blood
    lut.SetTableValue(2, colors.GetColor4d('beige'))  # brain
    lut.SetTableValue(3, colors.GetColor4d('orange'))  # duodenum
    lut.SetTableValue(4, colors.GetColor4d('misty_rose'))  # eye_retina
    lut.SetTableValue(5, colors.GetColor4d('white'))  # eye_white
    lut.SetTableValue(6, colors.GetColor4d('tomato'))  # heart
    lut.SetTableValue(7, colors.GetColor4d('raspberry'))  # ileum
    lut.SetTableValue(8, colors.GetColor4d('banana'))  # kidney
    lut.SetTableValue(9, colors.GetColor4d('peru'))  # l_intestine
    lut.SetTableValue(10, colors.GetColor4d('pink'))  # liver
    lut.SetTableValue(11, colors.GetColor4d('powder_blue'))  # lung
    lut.SetTableValue(12, colors.GetColor4d('carrot'))  # nerve
    lut.SetTableValue(13, colors.GetColor4d('wheat'))  # skeleton
    lut.SetTableValue(14, colors.GetColor4d('violet'))  # spleen
    lut.SetTableValue(15, colors.GetColor4d('plum'))  # stomach

    return lut


def create_tissue_map():
    tissue_map = dict()
    tissue_map['blood'] = 1
    tissue_map['brain'] = 2
    tissue_map['duodenum'] = 3
    tissue_map['eyeRetina'] = 4
    tissue_map['eyeWhite'] = 5
    tissue_map['heart'] = 6
    tissue_map['ileum'] = 7
    tissue_map['kidney'] = 8
    tissue_map['intestine'] = 9
    tissue_map['liver'] = 10
    tissue_map['lung'] = 11
    tissue_map['nerve'] = 12
    tissue_map['skeleton'] = 13
    tissue_map['spleen'] = 14
    tissue_map['stomach'] = 15

    return tissue_map


def create_smooth_frog_actor(file_name, tissue):
    reader = vtk.vtkMetaImageReader()
    reader.SetFileName(file_name)
    reader.Update()

    selectTissue = vtk.vtkImageThreshold()
    selectTissue.ThresholdBetween(tissue, tissue)
    selectTissue.SetInValue(255)
    selectTissue.SetOutValue(0)
    selectTissue.SetInputConnection(reader.GetOutputPort())

    gaussianRadius = 1
    gaussianStandardDeviation = 2.0
    gaussian = vtk.vtkImageGaussianSmooth()
    gaussian.SetStandardDeviations(gaussianStandardDeviation, gaussianStandardDeviation, gaussianStandardDeviation)
    gaussian.SetRadiusFactors(gaussianRadius, gaussianRadius, gaussianRadius)
    gaussian.SetInputConnection(selectTissue.GetOutputPort())

    isoValue = 127.5
    iso_surface = vtk.vtkFlyingEdges3D()
    iso_surface.SetInputConnection(gaussian.GetOutputPort())
    iso_surface.ComputeScalarsOff()
    iso_surface.ComputeGradientsOff()
    iso_surface.ComputeNormalsOff()
    iso_surface.SetValue(0, isoValue)

    smoothing_iterations = 0
    feature_angle = 60.0
    relaxation_factor = 0.1

    smoother = vtk.vtkSmoothPolyDataFilter()
    smoother.SetInputConnection(iso_surface.GetOutputPort())
    smoother.SetNumberOfIterations(smoothing_iterations)
    smoother.SetRelaxationFactor(relaxation_factor)
    smoother.SetFeatureAngle(feature_angle)
    smoother.FeatureEdgeSmoothingOff()
    smoother.BoundarySmoothingOff()
    smoother.SetConvergence(0)

    # smoothing_iterations = 0
    # pass_band = 0.001
    # feature_angle = 60.0
    # smoother = vtk.vtkWindowedSincPolyDataFilter()
    # smoother.SetInputConnection(iso_surface.GetOutputPort())
    # smoother.SetNumberOfIterations(smoothing_iterations)
    # smoother.BoundarySmoothingOff()
    # smoother.FeatureEdgeSmoothingOff()
    # smoother.SetFeatureAngle(feature_angle)
    # smoother.SetPassBand(pass_band)
    # smoother.NonManifoldSmoothingOn()
    # smoother.NormalizeCoordinatesOn()
    # smoother.Update()

    normals = vtk.vtkPolyDataNormals()
    normals.SetInputConnection(smoother.GetOutputPort())
    normals.SetFeatureAngle(feature_angle)

    stripper = vtk.vtkStripper()
    stripper.SetInputConnection(normals.GetOutputPort())

    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputConnection(stripper.GetOutputPort())

    actor = vtk.vtkActor()
    actor.SetMapper(mapper)

    return actor


def create_frog_actor(file_name, tissue):
    reader = vtk.vtkMetaImageReader()
    reader.SetFileName(file_name)
    reader.Update()

    select_tissue = vtk.vtkImageThreshold()
    select_tissue.ThresholdBetween(tissue, tissue)
    select_tissue.SetInValue(255)
    select_tissue.SetOutValue(0)
    select_tissue.SetInputConnection(reader.GetOutputPort())

    iso_value = 63.5
    iso_surface = vtk.vtkFlyingEdges3D()
    iso_surface.SetInputConnection(select_tissue.GetOutputPort())
    iso_surface.ComputeScalarsOff()
    iso_surface.ComputeGradientsOff()
    iso_surface.ComputeNormalsOn()
    iso_surface.SetValue(0, iso_value)

    stripper = vtk.vtkStripper()
    stripper.SetInputConnection(iso_surface.GetOutputPort())

    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputConnection(stripper.GetOutputPort())

    actor = vtk.vtkActor()
    actor.SetMapper(mapper)

    return actor


if __name__ == '__main__':
    main()
