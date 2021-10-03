#!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonDataModel import (
    vtkDataObject,
    vtkDataSetAttributes
)
from vtkmodules.vtkCommonTransforms import vtkTransform
from vtkmodules.vtkFiltersCore import vtkThreshold
from vtkmodules.vtkFiltersGeneral import vtkTransformFilter
from vtkmodules.vtkFiltersGeometry import vtkGeometryFilter
from vtkmodules.vtkIOImage import vtkMetaImageReader
from vtkmodules.vtkImagingCore import vtkImageWrapPad
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def main():
    colors = vtkNamedColors()

    file_name, start_label, end_label = get_program_parameters()
    if start_label > end_label:
        end_label, start_label = start_label, end_label

    # Generate cubes from labels
    # 1) Read the meta file
    # 2) Convert point data to cell data
    # 3) Convert to geometry and display

    reader = vtkMetaImageReader()
    reader.SetFileName(file_name)
    reader.Update()

    # Pad the volume so that we can change the point data into cell
    # data.
    extent = reader.GetOutput().GetExtent()
    pad = vtkImageWrapPad()
    pad.SetInputConnection(reader.GetOutputPort())
    pad.SetOutputWholeExtent(extent[0], extent[1] + 1, extent[2], extent[3] + 1, extent[4], extent[5] + 1)
    pad.Update()

    # Copy the scalar point data of the volume into the scalar cell data
    pad.GetOutput().GetCellData().SetScalars(reader.GetOutput().GetPointData().GetScalars())

    selector = vtkThreshold()
    selector.SetInputArrayToProcess(0, 0, 0, vtkDataObject().FIELD_ASSOCIATION_CELLS,
                                    vtkDataSetAttributes().SCALARS)
    selector.SetInputConnection(pad.GetOutputPort())
    selector.SetLowerThreshold(start_label)
    selector.SetUpperThreshold(end_label)
    selector.Update()

    # Shift the geometry by 1/2
    transform = vtkTransform()
    transform.Translate(-0.5, -0.5, -0.5)

    transform_model = vtkTransformFilter()
    transform_model.SetTransform(transform)
    transform_model.SetInputConnection(selector.GetOutputPort())

    geometry = vtkGeometryFilter()
    geometry.SetInputConnection(transform_model.GetOutputPort())

    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(geometry.GetOutputPort())
    mapper.SetScalarRange(start_label, end_label)
    mapper.SetScalarModeToUseCellData()
    mapper.SetColorModeToMapScalars()

    actor = vtkActor()
    actor.SetMapper(mapper)

    renderer = vtkRenderer()
    render_window = vtkRenderWindow()
    render_window.AddRenderer(renderer)
    render_window.SetSize(640, 480)
    render_window.SetWindowName('GenerateCubesFromLabels')

    render_window_interactor = vtkRenderWindowInteractor()
    render_window_interactor.SetRenderWindow(render_window)

    renderer.AddActor(actor)
    renderer.SetBackground(colors.GetColor3d('DarkSlateBlue'))
    render_window.Render()

    camera = renderer.GetActiveCamera()
    camera.SetPosition(42.301174, 939.893457, -124.005030)
    camera.SetFocalPoint(224.697134, 221.301653, 146.823706)
    camera.SetViewUp(0.262286, -0.281321, -0.923073)
    camera.SetDistance(789.297581)
    camera.SetClippingRange(168.744328, 1509.660206)

    render_window_interactor.Start()


def get_program_parameters():
    import argparse
    description = 'Convert the point data from a labeled volume into cell data.'
    epilogue = '''
 The surfaces are displayed as vtkPolydata.
     '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('filename', help='Input volume e.g. Frog/frogtissue.mhd.')
    parser.add_argument('startlabel', type=int, help='The starting label in the input volume, e,g, 1.')
    parser.add_argument('endlabel', type=int, help='The ending label in the input volume e.g. 29')
    args = parser.parse_args()
    return args.filename, args.startlabel, args.endlabel


if __name__ == '__main__':
    main()
