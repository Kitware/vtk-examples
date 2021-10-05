#!/usr/bin/env python

import collections
from pathlib import Path

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkLookupTable
from vtkmodules.vtkCommonMath import vtkMatrix4x4
from vtkmodules.vtkCommonTransforms import vtkTransform
from vtkmodules.vtkFiltersCore import (
    vtkContourFilter,
    vtkDecimatePro,
    vtkExecutionTimer,
    vtkFlyingEdges3D,
    vtkMarchingCubes,
    vtkPolyDataNormals,
    vtkStripper,
    vtkWindowedSincPolyDataFilter
)
from vtkmodules.vtkFiltersGeneral import vtkTransformPolyDataFilter
from vtkmodules.vtkIOImage import vtkMetaImageReader
from vtkmodules.vtkImagingCore import (
    vtkImageShrink3D,
    vtkImageThreshold
)
from vtkmodules.vtkImagingGeneral import vtkImageGaussianSmooth
from vtkmodules.vtkImagingMorphological import vtkImageIslandRemoval2D
from vtkmodules.vtkInteractionWidgets import vtkOrientationMarkerWidget
from vtkmodules.vtkRenderingAnnotation import vtkAxesActor
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def get_program_parameters(argv):
    import argparse
    description = 'Display all frog parts and translucent skin.'
    epilogue = '''
To specify all the tissues at once:
 blood brain duodenum eye_retna eye_white heart ileum kidney l_intestine liver lung nerve skeleton spleen stomach skin

You can leave out brainbin, it is the brain with no gaussian smoothing.

Here are the parameters used to get the views in the VTK Textbook:
Fig12-9a:
 blood brain duodenum eye_retna eye_white heart ileum kidney l_intestine liver lung nerve skeleton spleen stomach skin -a
Fig12-9b:
 blood brain duodenum eye_retna eye_white heart ileum kidney l_intestine liver lung nerve skeleton spleen stomach -a
Fig12-9c:
 brain duodenum eye_retna eye_white heart ileum kidney l_intestine liver lung nerve spleen stomach -c
Fig12-9c:
 brain duodenum eye_retna eye_white heart ileum kidney l_intestine liver lung nerve spleen stomach -d
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)

    group = parser.add_mutually_exclusive_group()
    group.add_argument('-a', action='store_const', dest='view', const='a',
                       help='The view corresponds to Figs 12-9a and 12-9b in the VTK Textbook')
    group.add_argument('-c', action='store_const', dest='view', const='c',
                       help='The view corresponds to Figs 12-9c in the VTK Textbook')
    group.add_argument('-d', action='store_const', dest='view', const='d',
                       help='The view corresponds to Figs 12-9d in the VTK Textbook')
    parser.set_defaults(type=None)

    parser.add_argument('-m', action='store_false', dest='flying_edges',
                        help='Use flying edges by default, marching cubes if set.')
    parser.add_argument('-t', action='store_true', dest='decimation',
                        help='Decimate if set.')
    parser.add_argument('data_folder', help='The path to the files: frog.mhd and frogtissue.mhd.')
    parser.add_argument('tissues', nargs='+', help='List of one or more tissues.')
    args = parser.parse_args()
    return args.data_folder, args.tissues, args.view, args.flying_edges, args.decimation


def main(data_folder, tissues, view, flying_edges, decimate):
    colors = vtkNamedColors()

    path = Path(data_folder)
    if path.is_dir():
        s = ''
        frog_fn = path.joinpath('frog').with_suffix('.mhd')
        if not frog_fn.is_file():
            s += 'The file: {:s} does not exist.\n'.format(str(frog_fn))
            print(s)
        frog_tissue_fn = path.joinpath('frogtissue').with_suffix('.mhd')
        if not frog_tissue_fn.is_file():
            s += 'The file: {:s} does not exist.'.format(str(frog_tissue_fn))
        if s:
            print(s)
            return
    else:
        print('Expected a path to frog.mhs and frogtissue.mhd')
        return

    # Tissue parameters
    available_tissues = tissue_parameters()
    selected_tissues = {key: available_tissues[key] for key in tissues}
    if not selected_tissues:
        print('No tissues!')
        return

    missing_parameters = False
    for k, v in selected_tissues.items():
        res = check_for_required_parameters(k, v)
        if res:
            print(res)
            missing_parameters = True
    if missing_parameters:
        print('Some required parameters are missing!')
        return

    # Setup render window, renderer, and interactor.
    renderer = vtkRenderer()
    render_window = vtkRenderWindow()
    render_window.AddRenderer(renderer)
    render_window_interactor = vtkRenderWindowInteractor()
    render_window_interactor.SetRenderWindow(render_window)

    lut = create_frog_lut(colors)

    #  Time some filters
    ict = collections.defaultdict(dict)

    for name, tissue in selected_tissues.items():
        print('Tissue: {:>9s}, label: {:2d}'.format(name, tissue['TISSUE']))
        t, actor = create_frog_actor(frog_fn, frog_tissue_fn, tissue, flying_edges, decimate, lut)
        ict[name] = t
        renderer.AddActor(actor)

    # Initial view (looking down on the dorsal surface).
    renderer.GetActiveCamera().Roll(-90)
    renderer.ResetCamera()

    #  Final view
    if view:
        if view == 'a':
            # Figs 12-9a and 12-9b in the VTK Textbook
            camera = renderer.GetActiveCamera()
            camera.SetPosition(-850.073854, 834.142692, 266.017598)
            camera.SetFocalPoint(-72.387897, 109.637349, -306.343185)
            camera.SetViewUp(0.284585, -0.387303, 0.876931)
            camera.SetDistance(1207.186939)
            camera.SetClippingRange(247.737449, 1758.922849)
        elif view == 'c':
            # Figs 12-9c in the VTK Textbook
            camera = renderer.GetActiveCamera()
            camera.SetPosition(-438.993734, 404.715262, 105.797836)
            camera.SetFocalPoint(-254.193794, 245.672169, -95.535892)
            camera.SetViewUp(0.256893, -0.629643, 0.733182)
            camera.SetDistance(316.197712)
            camera.SetClippingRange(0.789810, 789.809963)
        elif view == 'd':
            # Fig 12-9d in the VTK Textbook
            camera = renderer.GetActiveCamera()
            camera.SetPosition(-262.252604, 229.863144, 562.084505)
            camera.SetFocalPoint(-288.693092, 228.870041, -91.185421)
            camera.SetViewUp(0.729526, -0.683360, -0.028488)
            camera.SetDistance(653.805539)
            camera.SetClippingRange(452.459105, 905.003135)

    print('Timings:')
    print('\n'.join(format_timings(ict)))

    renderer.SetBackground(colors.GetColor3d('LightSteelBlue'))

    render_window.SetSize(640, 640)
    render_window.SetWindowName('FrogReconstruction')
    render_window.Render()

    axes = vtkAxesActor()

    widget = vtkOrientationMarkerWidget()
    rgba = [0.0, 0.0, 0.0, 0.0]
    colors.GetColor("Carrot", rgba)
    widget.SetOutlineColor(rgba[0], rgba[1], rgba[2])
    widget.SetOrientationMarker(axes)
    widget.SetInteractor(render_window_interactor)
    widget.SetViewport(0.0, 0.0, 0.2, 0.2)
    widget.SetEnabled(1)
    widget.InteractiveOn()

    render_window.Render()

    render_window_interactor.Start()


def create_frog_actor(frog_fn, frog_tissue_fn, tissue, flying_edges, decimate, lut):
    # Get the tissue parameters
    pixel_size = tissue['PIXEL_SIZE']
    columns = tissue['COLUMNS']
    rows = tissue['ROWS']

    voi = tissue['VOI']
    spacing = float(tissue['SPACING'])
    start_slice = float(tissue['START_SLICE'])
    data_spacing = [pixel_size, pixel_size, spacing]
    data_origin = [-(columns / 2.0) * pixel_size, -(rows / 2.0) * pixel_size, start_slice * spacing]

    #
    # adjust y bounds for PNM coordinate system
    #
    tmp = voi[2]
    voi[2] = rows - voi[3] - 1
    voi[3] = rows - tmp - 1

    if tissue['NAME'] == 'skin':
        fn = frog_fn
    else:
        fn = frog_tissue_fn

    reader = vtkMetaImageReader()
    reader.SetFileName(str(fn))
    reader.SetDataSpacing(data_spacing)
    reader.SetDataOrigin(data_origin)
    reader.SetDataExtent(voi)
    reader.Update()

    last_connection = reader

    if not tissue['NAME'] == 'skin':
        if tissue['ISLAND_REPLACE'] >= 0:
            island_remover = vtkImageIslandRemoval2D()
            island_remover.SetAreaThreshold(tissue['ISLAND_AREA'])
            island_remover.SetIslandValue(tissue['ISLAND_REPLACE'])
            island_remover.SetReplaceValue(tissue['TISSUE'])
            island_remover.SetInput(last_connection.GetOutput())
            island_remover.Update()
            last_connection = island_remover

        select_tissue = vtkImageThreshold()
        select_tissue.ThresholdBetween(tissue['TISSUE'], tissue['TISSUE'])
        select_tissue.SetInValue(255)
        select_tissue.SetOutValue(0)
        select_tissue.SetInputConnection(last_connection.GetOutputPort())
        last_connection = select_tissue

    shrinker = vtkImageShrink3D()
    shrinker.SetInputConnection(last_connection.GetOutputPort())
    shrinker.SetShrinkFactors(tissue['SAMPLE_RATE'])
    shrinker.AveragingOn()
    last_connection = shrinker

    if not all(v == 0 for v in tissue['GAUSSIAN_STANDARD_DEVIATION']):
        gaussian = vtkImageGaussianSmooth()
        gaussian.SetStandardDeviation(*tissue['GAUSSIAN_STANDARD_DEVIATION'])
        gaussian.SetRadiusFactors(*tissue['GAUSSIAN_RADIUS_FACTORS'])
        gaussian.SetInputConnection(shrinker.GetOutputPort())
        last_connection = gaussian
    # Time the isocontouring.
    ict = collections.defaultdict()
    iso_value = tissue['VALUE']
    if flying_edges:
        iso_surface = vtkFlyingEdges3D()
        iso_surface.SetInputConnection(last_connection.GetOutputPort())
        iso_surface.ComputeScalarsOff()
        iso_surface.ComputeGradientsOff()
        iso_surface.ComputeNormalsOff()
        iso_surface.SetValue(0, iso_value)
        timer = vtkExecutionTimer()
        timer.SetFilter(iso_surface)
        iso_surface.Update()
        ict['Flying Edges'] = timer.GetElapsedWallClockTime()
    else:
        iso_surface = vtkMarchingCubes()
        iso_surface.SetInputConnection(last_connection.GetOutputPort())
        iso_surface.ComputeScalarsOff()
        iso_surface.ComputeGradientsOff()
        iso_surface.ComputeNormalsOff()
        iso_surface.SetValue(0, iso_value)
        timer = vtkExecutionTimer()
        timer.SetFilter(iso_surface)
        iso_surface.Update()
        ict['Marching Cubes'] = timer.GetElapsedWallClockTime()

    so = SliceOrder()
    # transform = so.get(tissue['SLICE_ORDER'])
    # Match Frog.py
    transform = so.get('hfap')
    transform.Scale(1, -1, 1)
    tf = vtkTransformPolyDataFilter()
    tf.SetTransform(transform)
    tf.SetInputConnection(iso_surface.GetOutputPort())
    last_connection = tf

    if decimate:
        decimator = vtkDecimatePro()
        decimator.SetInputConnection(last_connection.GetOutputPort())
        decimator.SetFeatureAngle(tissue['DECIMATE_ANGLE'])
        decimator.MaximumIterations = tissue['DECIMATE_ITERATIONS']
        decimator.PreserveTopologyOn()
        decimator.SetErrorIsAbsolute(1)
        decimator.SetAbsoluteError(tissue['DECIMATE_ERROR'])
        decimator.SetTargetReduction(tissue['DECIMATE_REDUCTION'])
        last_connection = decimator

    smoother = vtkWindowedSincPolyDataFilter()
    smoother.SetInputConnection(last_connection.GetOutputPort())
    smoother.SetNumberOfIterations(tissue['SMOOTH_ITERATIONS'])
    smoother.BoundarySmoothingOff()
    smoother.FeatureEdgeSmoothingOff()
    smoother.SetFeatureAngle(tissue['SMOOTH_ANGLE'])
    smoother.SetPassBand(tissue['SMOOTH_FACTOR'])
    smoother.NonManifoldSmoothingOn()
    smoother.NormalizeCoordinatesOff()
    smoother.Update()

    normals = vtkPolyDataNormals()
    normals.SetInputConnection(smoother.GetOutputPort())
    normals.SetFeatureAngle(tissue['FEATURE_ANGLE'])

    stripper = vtkStripper()
    stripper.SetInputConnection(normals.GetOutputPort())

    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(stripper.GetOutputPort())

    # Create iso-surface
    contour = vtkContourFilter()
    contour.SetInputConnection(reader.GetOutputPort())
    contour.SetValue(0, iso_value)

    actor = vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetOpacity(tissue['OPACITY'])
    actor.GetProperty().SetDiffuseColor(lut.GetTableValue(tissue['TISSUE'])[:3])
    actor.GetProperty().SetSpecular(0.5)
    actor.GetProperty().SetSpecularPower(10)

    return ict, actor


class SliceOrder:
    """
    These transformations permute image and other geometric data to maintain proper
     orientation regardless of the acquisition order. After applying these transforms with
    vtkTransformFilter, a view up of 0,-1,0 will result in the body part
    facing the viewer.
    NOTE: some transformations have a -1 scale factor for one of the components.
          To ensure proper polygon orientation and normal direction, you must
          apply the vtkPolyDataNormals filter.

    Naming (the nomenclature is medical):
    si - superior to inferior (top to bottom)
    is - inferior to superior (bottom to top)
    ap - anterior to posterior (front to back)
    pa - posterior to anterior (back to front)
    lr - left to right
    rl - right to left
    """

    def __init__(self):
        self.si_mat = vtkMatrix4x4()
        self.si_mat.Zero()
        self.si_mat.SetElement(0, 0, 1)
        self.si_mat.SetElement(1, 2, 1)
        self.si_mat.SetElement(2, 1, -1)
        self.si_mat.SetElement(3, 3, 1)

        self.is_mat = vtkMatrix4x4()
        self.is_mat.Zero()
        self.is_mat.SetElement(0, 0, 1)
        self.is_mat.SetElement(1, 2, -1)
        self.is_mat.SetElement(2, 1, -1)
        self.is_mat.SetElement(3, 3, 1)

        self.lr_mat = vtkMatrix4x4()
        self.lr_mat.Zero()
        self.lr_mat.SetElement(0, 2, -1)
        self.lr_mat.SetElement(1, 1, -1)
        self.lr_mat.SetElement(2, 0, 1)
        self.lr_mat.SetElement(3, 3, 1)

        self.rl_mat = vtkMatrix4x4()
        self.rl_mat.Zero()
        self.rl_mat.SetElement(0, 2, 1)
        self.rl_mat.SetElement(1, 1, -1)
        self.rl_mat.SetElement(2, 0, 1)
        self.rl_mat.SetElement(3, 3, 1)

        """
        The previous transforms assume radiological views of the slices (viewed from the feet). other
        modalities such as physical sectioning may view from the head. These transforms modify the original
        with a 180° rotation about y
        """

        self.hf_mat = vtkMatrix4x4()
        self.hf_mat.Zero()
        self.hf_mat.SetElement(0, 0, -1)
        self.hf_mat.SetElement(1, 1, 1)
        self.hf_mat.SetElement(2, 2, -1)
        self.hf_mat.SetElement(3, 3, 1)

    def s_i(self):
        t = vtkTransform()
        t.SetMatrix(self.si_mat)
        return t

    def i_s(self):
        t = vtkTransform()
        t.SetMatrix(self.is_mat)
        return t

    @staticmethod
    def a_p():
        t = vtkTransform()
        return t.Scale(1, -1, 1)

    @staticmethod
    def p_a():
        t = vtkTransform()
        return t.Scale(1, -1, -1)

    def l_r(self):
        t = vtkTransform()
        t.SetMatrix(self.lr_mat)
        t.Update()
        return t

    def r_l(self):
        t = vtkTransform()
        t.SetMatrix(self.lr_mat)
        return t

    def h_f(self):
        t = vtkTransform()
        t.SetMatrix(self.hf_mat)
        return t

    def hf_si(self):
        t = vtkTransform()
        t.Concatenate(self.hf_mat)
        t.Concatenate(self.si_mat)
        return t

    def hf_is(self):
        t = vtkTransform()
        t.Concatenate(self.hf_mat)
        t.Concatenate(self.is_mat)
        return t

    def hf_ap(self):
        t = vtkTransform()
        t.Concatenate(self.hf_mat)
        t.Scale(1, -1, 1)
        return t

    def hf_pa(self):
        t = vtkTransform()
        t.Concatenate(self.hf_mat)
        t.Scale(1, -1, -1)
        return t

    def hf_lr(self):
        t = vtkTransform()
        t.Concatenate(self.hf_mat)
        t.Concatenate(self.lr_mat)
        return t

    def hf_rl(self):
        t = vtkTransform()
        t.Concatenate(self.hf_mat)
        t.Concatenate(self.rl_mat)
        return t

    def get(self, order):
        """
        Returns the vtkTransform corresponding to the slice order.

        :param order: The slice order
        :return: The vtkTransform to use
        """
        if order == 'si':
            return self.s_i()
        elif order == 'is':
            return self.i_s()
        elif order == 'ap':
            return self.a_p()
        elif order == 'pa':
            return self.p_a()
        elif order == 'lr':
            return self.l_r()
        elif order == 'rl':
            return self.r_l()
        elif order == 'hf':
            return self.h_f()
        elif order == 'hfsi':
            return self.hf_si()
        elif order == 'hfis':
            return self.hf_is()
        elif order == 'hfap':
            return self.hf_ap()
        elif order == 'hfpa':
            return self.hf_pa()
        elif order == 'hflr':
            return self.hf_lr()
        elif order == 'hfrl':
            return self.hf_rl()
        else:
            s = 'No such transform "{:s}" exists.'.format(order)
            raise Exception(s)


def default_parameters():
    p = dict()
    p['NAME'] = ''
    p['TISSUE'] = '1'
    p['START_SLICE'] = '0'
    p['END_SLICE'] = '255'
    p['STUDY'] = 'frogtissue'
    p['VALUE'] = 127.5
    p['ROWS'] = 470
    p['COLUMNS'] = 500
    p['HEADER_SIZE'] = 0
    p['PIXEL_SIZE'] = 1
    p['SPACING'] = 1.5
    p['START_SLICE'] = 1
    p['END_SLICE'] = 138
    p['REDUCTION'] = 1
    p['FEATURE_ANGLE'] = 60
    p['DECIMATE_ANGLE'] = 60
    p['SMOOTH_ANGLE'] = 60
    p['SMOOTH_ITERATIONS'] = 10
    p['SMOOTH_FACTOR'] = 0.1
    p['DECIMATE_ITERATIONS'] = 1
    p['DECIMATE_REDUCTION'] = 1
    p['DECIMATE_ERROR'] = 0.0002
    p['DECIMATE_ERROR_INCREMENT'] = 0.0002
    p['ISLAND_AREA'] = 4
    p['ISLAND_REPLACE'] = -1
    p['GAUSSIAN_STANDARD_DEVIATION'] = [2, 2, 2]
    p['GAUSSIAN_RADIUS_FACTORS'] = [2, 2, 2]
    p['VOI'] = [0, p['COLUMNS'] - 1, 0, p['ROWS'] - 1, 0, p['END_SLICE']]
    p['SAMPLE_RATE'] = [1, 1, 1]
    p['OPACITY'] = 1.0
    return p


def blood():
    p = frog()
    p['NAME'] = 'blood'
    p['TISSUE'] = 1
    p['START_SLICE'] = 14
    p['END_SLICE'] = 131
    p['VALUE'] = 4
    p['VOI'] = [33, 406, 62, 425, p['START_SLICE'], p['END_SLICE']]
    return p


def brain():
    p = frog()
    p['NAME'] = 'brain'
    p['TISSUE'] = 2
    p['START_SLICE'] = 1
    p['END_SLICE'] = 33
    p['VOI'] = [349, 436, 211, 252, p['START_SLICE'], p['END_SLICE']]
    return p


def brainbin():
    p = frog()
    p['NAME'] = 'brainbin'
    p['TISSUE'] = 2
    p['START_SLICE'] = 1
    p['END_SLICE'] = 33
    p['VOI'] = [349, 436, 211, 252, p['END_SLICE'], p['START_SLICE']]
    p['GAUSSIAN_STANDARD_DEVIATION'] = [0, 0, 0]
    p['DECIMATE_ITERATIONS'] = 0
    return p


def duodenum():
    p = frog()
    p['NAME'] = 'duodenum'
    p['TISSUE'] = 3
    p['START_SLICE'] = 35
    p['END_SLICE'] = 105
    p['VOI'] = [189, 248, 191, 284, p['START_SLICE'], p['END_SLICE']]
    return p


def eye_retna():
    p = frog()
    p['NAME'] = 'eye_retna'
    p['TISSUE'] = 4
    p['START_SLICE'] = 1
    p['END_SLICE'] = 41
    p['VOI'] = [342, 438, 180, 285, p['START_SLICE'], p['END_SLICE']]
    return p


def eye_white():
    p = frog()
    p['NAME'] = 'eye_white'
    p['TISSUE'] = 5
    p['START_SLICE'] = 1
    p['END_SLICE'] = 37
    p['VOI'] = [389, 433, 183, 282, p['START_SLICE'], p['END_SLICE']]
    return p


def frog():
    p = default_parameters()
    p['ROWS'] = 470
    p['COLUMNS'] = 500
    p['STUDY'] = 'frogtissue'
    p['SLICE_ORDER'] = 'si'
    p['PIXEL_SIZE'] = 1
    p['SPACING'] = 1.5
    p['VALUE'] = 127.5
    p['SAMPLE_RATE'] = [1, 1, 1]
    p['GAUSSIAN_STANDARD_DEVIATION'] = [2, 2, 2]
    p['DECIMATE_REDUCTION'] = 0.95
    p['DECIMATE_ITERATIONS'] = 5
    p['DECIMATE_ERROR'] = 0.0002
    p['DECIMATE_ERROR_INCREMENT'] = 0.0002
    p['SMOOTH_FACTOR'] = 0.1
    return p


def heart():
    p = frog()
    p['NAME'] = 'heart'
    p['TISSUE'] = 6
    p['START_SLICE'] = 49
    p['END_SLICE'] = 93
    p['VOI'] = [217, 299, 186, 266, p['START_SLICE'], p['END_SLICE']]
    return p


def ileum():
    p = frog()
    p['NAME'] = 'ileum'
    p['TISSUE'] = 7
    p['START_SLICE'] = 25
    p['END_SLICE'] = 93
    p['VOI'] = [172, 243, 201, 290, p['START_SLICE'], p['END_SLICE']]
    return p


def kidney():
    p = frog()
    p['NAME'] = 'kidney'
    p['TISSUE'] = 8
    p['START_SLICE'] = 24
    p['END_SLICE'] = 78
    p['VOI'] = [116, 238, 193, 263, p['START_SLICE'], p['END_SLICE']]
    return p


def l_intestine():
    p = frog()
    p['NAME'] = 'l_intestine'
    p['TISSUE'] = 9
    p['START_SLICE'] = 56
    p['END_SLICE'] = 106
    p['VOI'] = [115, 224, 209, 284, p['START_SLICE'], p['END_SLICE']]
    return p


def liver():
    p = frog()
    p['NAME'] = 'liver'
    p['TISSUE'] = 10
    p['START_SLICE'] = 25
    p['END_SLICE'] = 126
    p['VOI'] = [167, 297, 154, 304, p['START_SLICE'], p['END_SLICE']]
    return p


def lung():
    p = frog()
    p['NAME'] = 'lung'
    p['TISSUE'] = 11
    p['START_SLICE'] = 24
    p['END_SLICE'] = 59
    p['VOI'] = [222, 324, 157, 291, p['START_SLICE'], p['END_SLICE']]
    return p


def nerve():
    p = frog()
    p['NAME'] = 'nerve'
    p['TISSUE'] = 12
    p['START_SLICE'] = 7
    p['END_SLICE'] = 113
    p['VOI'] = [79, 403, 63, 394, p['START_SLICE'], p['END_SLICE']]
    return p


def skin():
    p = default_parameters()
    p['NAME'] = 'skin'
    p['TISSUE'] = 0
    p['ROWS'] = 470
    p['COLUMNS'] = 500
    p['STUDY'] = 'frog'
    p['SLICE_ORDER'] = 'si'
    p['PIXEL_SIZE'] = 1
    p['SPACING'] = 1.5
    p['START_SLICE'] = 1
    p['END_SLICE'] = 138
    p['VOI'] = [0, 499, 0, 469, p['START_SLICE'], p['END_SLICE']]
    p['VALUE'] = 10.5
    p['SAMPLE_RATE'] = [2, 2, 1]
    p['DECIMATE_REDUCTION'] = 0.95
    p['DECIMATE_ITERATIONS'] = 10
    p['DECIMATE_ERROR'] = 0.0002
    p['DECIMATE_ERROR_INCREMENT'] = 0.0002
    p['FEATURE_ANGLE'] = 60
    p['OPACITY'] = 0.4
    return p


def skeleton():
    p = frog()
    p['STUDY'] = 'frogtissue'
    p['NAME'] = 'skeleton'
    p['TISSUE'] = 13
    p['VALUE'] = 64.5
    p['START_SLICE'] = 1
    p['END_SLICE'] = 136
    p['VOI'] = [23, 479, 8, 469, p['START_SLICE'], p['END_SLICE']]
    p['GAUSSIAN_STANDARD_DEVIATION'] = [1.5, 1.5, 1]
    return p


def spleen():
    p = frog()
    p['NAME'] = 'spleen'
    p['TISSUE'] = 14
    p['START_SLICE'] = 45
    p['END_SLICE'] = 68
    p['VOI'] = [166, 219, 195, 231, p['START_SLICE'], p['END_SLICE']]
    return p


def stomach():
    p = frog()
    p['NAME'] = 'stomach'
    p['TISSUE'] = 15
    p['START_SLICE'] = 26
    p['END_SLICE'] = 119
    p['VOI'] = [143, 365, 158, 297, p['START_SLICE'], p['END_SLICE']]
    return p


def tissue_parameters():
    t = dict()
    t['blood'] = blood()
    t['brain'] = brain()
    t['brainbin'] = brainbin()
    t['duodenum'] = duodenum()
    t['eye_retna'] = eye_retna()
    t['eye_white'] = eye_white()
    t['frog'] = frog()
    t['heart'] = heart()
    t['ileum'] = ileum()
    t['kidney'] = kidney()
    t['l_intestine'] = l_intestine()
    t['liver'] = liver()
    t['lung'] = lung()
    t['nerve'] = nerve()
    t['skin'] = skin()
    t['skeleton'] = skeleton()
    t['spleen'] = spleen()
    t['stomach'] = stomach()

    return t


def create_frog_lut(colors):
    lut = vtkLookupTable()
    lut.SetNumberOfColors(16)
    lut.SetTableRange(0, 15)
    lut.Build()

    lut.SetTableValue(0, colors.GetColor4d('LimeGreen'))  # skin
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


def check_for_required_parameters(tissue, parameters):
    required = {'NAME', 'END_SLICE', 'TISSUE', 'STUDY', 'ROWS',
                'COLUMNS', 'VALUE', 'SPACING',
                'GAUSSIAN_STANDARD_DEVIATION', 'VOI',
                'DECIMATE_ITERATIONS'}
    k = set(parameters.keys())
    s = None
    if len(k) == 0:
        s = 'Missing parameters for {:11s}: {:s}'.format(tissue, ', '.join(map(str, required)))
    else:
        d = required.difference(k)
        if d:
            s = 'Missing parameters for {:11s}: {:s}'.format(tissue, ', '.join(map(str, d)))
    return s


def format_timings(ict):
    res = list()
    total = 0
    sk = sorted(ict.keys())
    for k in sk:
        sigma = 0
        res.append('{:11s}'.format(k))
        skk = sorted(ict[k].keys())
        for kk in skk:
            sigma += ict[k][kk]
            res.append('{:11s}{:13s} {:5.2f}s'.format(' ', kk, ict[k][kk]))
        total += sigma
        res.append('Subtotal: {:5.2f}s'.format(sigma))
    res.append('   Total: {:5.2f}s'.format(total))
    return res


if __name__ == '__main__':
    import sys

    data_folder, tissue, view, flying_edges, decimate = get_program_parameters(sys.argv)
    main(data_folder, tissue, view, flying_edges, decimate)
