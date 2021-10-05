#!/usr/bin/env python

from pathlib import Path

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkLookupTable
from vtkmodules.vtkCommonMath import vtkMatrix4x4
from vtkmodules.vtkCommonTransforms import vtkTransform
from vtkmodules.vtkFiltersCore import vtkPolyDataNormals
from vtkmodules.vtkFiltersGeneral import vtkTransformPolyDataFilter
from vtkmodules.vtkFiltersSources import vtkPlaneSource
from vtkmodules.vtkIOImage import vtkMetaImageReader
from vtkmodules.vtkImagingCore import vtkImageConstantPad
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkCamera,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkTexture,
    vtkWindowLevelLookupTable
)


def get_program_parameters(argv):
    import argparse
    description = 'Visualization of a frog.'
    epilogue = '''
Photographic slice of frog (upper left), segmented frog (upper right) and
 composite of photo and segmentation (bottom).
The purple color represents the stomach and the kidneys are yellow.
If slice = 39 it matches Figure 12-6 in the VTK Book.
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('data_folder', help='The path to the files: frog.mhd and frogtissue.mhd.')
    parser.add_argument('slice_number', default=39, type=int, nargs='?', help='Slice number.')
    args = parser.parse_args()
    return args.data_folder, args.slice_number


def main(data_folder, slice_number):
    colors = vtkNamedColors()

    path = Path(data_folder)
    if path.is_dir():
        s = ''
        fn_1 = path.joinpath('frog').with_suffix('.mhd')
        if not fn_1.is_file():
            s += 'The file: {:s} does not exist.\n'.format(str(fn_1))
            print(s)
        fn_2 = path.joinpath('frogtissue').with_suffix('.mhd')
        if not fn_2.is_file():
            s += 'The file: {:s} does not exist.'.format(str(fn_2))
        if s:
            print(s)
            return
    else:
        print('Expected a path to frog.mhs and frogtissue.mhd')
        return

    so = SliceOrder()

    # Now create the RenderWindow, Renderer and Interactor
    #
    ren1 = vtkRenderer()
    ren2 = vtkRenderer()
    ren3 = vtkRenderer()
    ren_win = vtkRenderWindow()
    ren_win.AddRenderer(ren1)
    ren_win.AddRenderer(ren2)
    ren_win.AddRenderer(ren3)
    ren_win.SetWindowName('FrogSlice')

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    grey_reader = vtkMetaImageReader()
    grey_reader.SetFileName(str(fn_1))
    grey_reader.Update()

    grey_padder = vtkImageConstantPad()
    grey_padder.SetInputConnection(grey_reader.GetOutputPort())
    grey_padder.SetOutputWholeExtent(0, 511, 0, 511, slice_number, slice_number)
    grey_padder.SetConstant(0)

    grey_plane = vtkPlaneSource()

    grey_transform = vtkTransformPolyDataFilter()
    grey_transform.SetTransform(so.get('hfsi'))
    grey_transform.SetInputConnection(grey_plane.GetOutputPort())

    grey_normals = vtkPolyDataNormals()
    grey_normals.SetInputConnection(grey_transform.GetOutputPort())
    grey_normals.FlipNormalsOff()

    wllut = vtkWindowLevelLookupTable()
    wllut.SetWindow(255)
    wllut.SetLevel(128)
    wllut.SetTableRange(0, 255)
    wllut.Build()

    grey_mapper = vtkPolyDataMapper()
    grey_mapper.SetInputConnection(grey_plane.GetOutputPort())

    grey_texture = vtkTexture()
    grey_texture.SetInputConnection(grey_padder.GetOutputPort())
    grey_texture.SetLookupTable(wllut)
    grey_texture.SetColorModeToMapScalars()
    grey_texture.InterpolateOn()

    grey_actor = vtkActor()
    grey_actor.SetMapper(grey_mapper)
    grey_actor.SetTexture(grey_texture)

    segment_reader = vtkMetaImageReader()
    segment_reader.SetFileName(str(fn_2))
    segment_reader.Update()

    segment_padder = vtkImageConstantPad()
    segment_padder.SetInputConnection(segment_reader.GetOutputPort())
    segment_padder.SetOutputWholeExtent(0, 511, 0, 511, slice_number, slice_number)
    segment_padder.SetConstant(0)

    segment_plane = vtkPlaneSource()

    segment_transform = vtkTransformPolyDataFilter()
    segment_transform.SetTransform(so.get('hfsi'))
    segment_transform.SetInputConnection(segment_plane.GetOutputPort())

    segment_normals = vtkPolyDataNormals()
    segment_normals.SetInputConnection(segment_transform.GetOutputPort())
    segment_normals.FlipNormalsOn()

    lut = create_frog_lut(colors)

    segment_mapper = vtkPolyDataMapper()
    segment_mapper.SetInputConnection(segment_plane.GetOutputPort())

    segment_texture = vtkTexture()
    segment_texture.SetInputConnection(segment_padder.GetOutputPort())
    segment_texture.SetLookupTable(lut)
    segment_texture.SetColorModeToMapScalars()
    segment_texture.InterpolateOff()

    segment_actor = vtkActor()
    segment_actor.SetMapper(segment_mapper)
    segment_actor.SetTexture(segment_texture)

    segment_overlay_actor = vtkActor()
    segment_overlay_actor.SetMapper(segment_mapper)
    segment_overlay_actor.SetTexture(segment_texture)

    segment_overlay_actor.GetProperty().SetOpacity(.5)
    ren1.SetBackground(0, 0, 0)
    ren1.SetViewport(0, 0.5, 0.5, 1)
    ren_win.SetSize(640, 480)
    ren1.AddActor(grey_actor)

    ren2.SetBackground(0, 0, 0)
    ren2.SetViewport(0.5, 0.5, 1, 1)
    ren2.AddActor(segment_actor)

    cam1 = vtkCamera()
    cam1.SetViewUp(0, -1, 0)
    cam1.SetPosition(0, 0, -1)
    ren1.SetActiveCamera(cam1)
    ren1.ResetCamera()
    cam1.SetViewUp(0, -1, 0)
    cam1.SetPosition(0.0554068, -0.0596001, -0.491383)
    cam1.SetFocalPoint(0.0554068, -0.0596001, 0)
    ren1.ResetCameraClippingRange()

    ren3.AddActor(grey_actor)
    ren3.AddActor(segment_overlay_actor)
    segment_overlay_actor.SetPosition(0, 0, -0.01)

    ren1.SetBackground(colors.GetColor3d('SlateGray'))
    ren2.SetBackground(colors.GetColor3d('SlateGray'))
    ren3.SetBackground(colors.GetColor3d('SlateGray'))

    ren3.SetViewport(0, 0, 1, 0.5)

    ren2.SetActiveCamera(ren1.GetActiveCamera())
    ren3.SetActiveCamera(ren1.GetActiveCamera())

    ren_win.Render()
    iren.Start()


def create_frog_lut(colors):
    lut = vtkLookupTable()
    lut.SetNumberOfColors(16)
    lut.SetTableRange(0, 15)
    lut.Build()

    lut.SetTableValue(0, colors.GetColor4d('Black'))
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


if __name__ == '__main__':
    import sys

    data_folder, slice_number = get_program_parameters(sys.argv)
    main(data_folder, slice_number)
