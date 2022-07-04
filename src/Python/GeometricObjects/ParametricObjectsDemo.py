#!/usr/bin/env python3

"""
    Demonstrate all the parametric objects.
"""

from pathlib import Path

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingFreeType
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonComputationalGeometry import (
    vtkParametricBohemianDome,
    vtkParametricBour,
    vtkParametricBoy,
    vtkParametricCatalanMinimal,
    vtkParametricConicSpiral,
    vtkParametricCrossCap,
    vtkParametricDini,
    vtkParametricEllipsoid,
    vtkParametricEnneper,
    vtkParametricFigure8Klein,
    vtkParametricHenneberg,
    vtkParametricKlein,
    vtkParametricKuen,
    vtkParametricMobius,
    vtkParametricPluckerConoid,
    vtkParametricPseudosphere,
    vtkParametricRandomHills,
    vtkParametricRoman,
    vtkParametricSpline,
    vtkParametricSuperEllipsoid,
    vtkParametricSuperToroid,
    vtkParametricTorus
)
from vtkmodules.vtkCommonCore import (
    vtkMinimalStandardRandomSequence,
    vtkPoints
)
from vtkmodules.vtkFiltersCore import (
    vtkGlyph3D,
    vtkMaskPoints
)
from vtkmodules.vtkFiltersSources import (
    vtkArrowSource,
    vtkParametricFunctionSource
)
from vtkmodules.vtkIOImage import (
    vtkPNGWriter
)
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkActor2D,
    vtkPolyDataMapper,
    vtkProperty,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkTextMapper,
    vtkTextProperty,
    vtkWindowToImageFilter
)


def get_program_parameters():
    import argparse
    description = 'Display the parametric surfaces.'
    epilogue = '''
   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('-s', '--surface_name', default=None, help='The name of the surface.')
    parser.add_argument('-b', '--back_face', action='store_true', help='Color the back face.')
    parser.add_argument('-n', '--normals', action='store_true', help='Display normals.')
    parser.add_argument('-l', '--limits', action='store_true', help='Display the geometric bounds of the object..')
    args = parser.parse_args()
    return args.surface_name, args.back_face, args.normals, args.limits


def main():
    surface_name, back_face, normals, limits = get_program_parameters()

    # Get the parametric functions and build the pipeline
    pfn = get_parametric_functions()

    # Check for a single surface.
    single_surface = [None, False]
    if surface_name:
        sn = surface_name.lower()
        for t in pfn.keys():
            if sn == t.lower():
                single_surface[0] = t
                single_surface[1] = True
    if surface_name and not single_surface[1]:
        print('Nonexistent surface:', surface_name)
        return

    if single_surface[1]:
        renderer_size = 1000
        grid_column_dimensions = 1
        grid_row_dimensions = 1
    else:
        renderer_size = 200
        grid_column_dimensions = 5
        grid_row_dimensions = 5

    colors = vtkNamedColors()

    # Create one text property for all.
    text_property = vtkTextProperty()
    text_property.SetJustificationToCentered()
    text_property.SetFontSize(int(renderer_size / 12))
    text_property.SetColor(colors.GetColor3d("LavenderBlush"))

    # Create a parametric function source, renderer, mapper, and actor
    # for each object.
    pfn_srcs = []
    renderers = []
    mappers = []
    actors = []
    text_mappers = []
    text_actors = []

    # Glyph the normals.
    mask_pts = []
    arrow = []
    glyph = []
    glyph_mapper = []
    glyph_actor = []

    back_property = vtkProperty()
    if back_face:
        back_property.SetColor(colors.GetColor3d("Peru"))

    # Now decide on the surfaces to build.
    surfaces = dict()
    if single_surface[1]:
        surfaces[single_surface[0]] = pfn[single_surface[0]]
    else:
        surfaces = pfn

    # The bounding boxes for each object.
    bounding_boxes = dict()
    indexed_names = dict()
    # The index of each parametric object.
    obj_idx = -1
    sorted_names = list()
    for obj in sorted(surfaces.keys()):
        obj_idx += 1
        indexed_names[obj_idx] = obj
        pfn_srcs.append(vtkParametricFunctionSource())
        pfn_srcs[obj_idx].SetParametricFunction(surfaces[obj])
        pfn_srcs[obj_idx].SetUResolution(51)
        pfn_srcs[obj_idx].SetVResolution(51)
        pfn_srcs[obj_idx].SetWResolution(51)
        pfn_srcs[obj_idx].Update()

        mappers.append(vtkPolyDataMapper())
        mappers[obj_idx].SetInputConnection(pfn_srcs[obj_idx].GetOutputPort())

        actors.append(vtkActor())
        actors[obj_idx].SetMapper(mappers[obj_idx])
        actors[obj_idx].GetProperty().SetColor(colors.GetColor3d("NavajoWhite"))
        if back_face:
            actors[obj_idx].SetBackfaceProperty(back_property)

        text_mappers.append(vtkTextMapper())
        text_mappers[obj_idx].SetInput(obj)
        text_mappers[obj_idx].SetTextProperty(text_property)

        text_actors.append(vtkActor2D())
        text_actors[obj_idx].SetMapper(text_mappers[obj_idx])
        text_actors[obj_idx].SetPosition(renderer_size / 2.0, 8)

        renderers.append(vtkRenderer())
        renderers[obj_idx].SetBackground(colors.GetColor3d("MidnightBlue"))

        bounds = pfn_srcs[obj_idx].GetOutput().GetBounds()
        bounding_boxes[obj] = bounds

        if normals:
            # Glyphing
            mask_pts.append(vtkMaskPoints())
            mask_pts[obj_idx].RandomModeOn()
            mask_pts[obj_idx].SetMaximumNumberOfPoints(150)
            mask_pts[obj_idx].SetInputConnection(pfn_srcs[obj_idx].GetOutputPort())

            arrow.append(vtkArrowSource())
            arrow[obj_idx].SetTipResolution(16)
            arrow[obj_idx].SetTipLength(0.3)
            arrow[obj_idx].SetTipRadius(0.1)

            glyph_scale = get_maximum_length(bounding_boxes[obj])

            glyph.append(vtkGlyph3D())
            glyph[obj_idx].SetSourceConnection(arrow[obj_idx].GetOutputPort())
            glyph[obj_idx].SetInputConnection(mask_pts[obj_idx].GetOutputPort())
            glyph[obj_idx].SetVectorModeToUseNormal()
            glyph[obj_idx].SetScaleFactor(glyph_scale / 10.0)
            glyph[obj_idx].OrientOn()
            glyph[obj_idx].Update()

            glyph_mapper.append(vtkPolyDataMapper())
            glyph_mapper[obj_idx].SetInputConnection(
                glyph[obj_idx].GetOutputPort())

            glyph_actor.append(vtkActor())
            glyph_actor[obj_idx].SetMapper(glyph_mapper[obj_idx])
            glyph_actor[obj_idx].GetProperty().SetColor(colors.GetColor3d("GreenYellow"))

    # Need a renderer even if there is no actor.
    for i in range(obj_idx + 1, grid_column_dimensions * grid_row_dimensions):
        renderers.append(vtkRenderer())
        renderers[i].SetBackground(colors.GetColor3d("MidnightBlue"))
        sorted_names.append(None)

    ren_win = vtkRenderWindow()
    ren_win.SetSize(renderer_size * grid_column_dimensions, renderer_size * grid_row_dimensions)

    for row in range(0, grid_row_dimensions):
        for col in range(0, grid_column_dimensions):
            index = row * grid_column_dimensions + col
            # (xmin, ymin, xmax, ymax)
            viewport = [
                float(col) * renderer_size / (grid_column_dimensions * renderer_size),
                float(grid_row_dimensions - (row + 1)) * renderer_size / (grid_row_dimensions * renderer_size),
                float(col + 1) * renderer_size / (grid_column_dimensions * renderer_size),
                float(grid_row_dimensions - row) * renderer_size / (grid_row_dimensions * renderer_size)]
            ren_win.AddRenderer(renderers[index])
            renderers[index].SetViewport(viewport)
            if index > obj_idx:
                continue
            renderers[index].AddActor(actors[index])
            # Normals can only be computed for polygons and triangle strips.
            # The Spline is a line.
            if normals and indexed_names[index] != 'Spline':
                renderers[index].AddActor(glyph_actor[index])
            renderers[index].AddActor(text_actors[index])
            renderers[index].ResetCamera()
            renderers[index].GetActiveCamera().Azimuth(30)
            renderers[index].GetActiveCamera().Elevation(-30)
            renderers[index].GetActiveCamera().Zoom(0.9)
            renderers[index].ResetCameraClippingRange()

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    if limits:
        for k, v in bounding_boxes.items():
            display_bounding_box_and_center(k, v)

    if surface_name:
        fn = single_surface[0]
    else:
        fn = 'ParametricObjectsDemo'
    ren_win.SetWindowName(fn)

    print_callback = PrintCallback(iren, fn, 1, False)
    iren.AddObserver('KeyPressEvent', print_callback)

    iren.Initialize()
    iren.Start()


def get_parametric_functions():
    """
    Create a map of the parametric functions and set some parameters.
    The first key groups the parametric functions and the
      second key is the name of the function.

    :return: The map of functions.
    """
    pfn = dict()
    pfn['Boy'] = vtkParametricBoy()
    pfn['ConicSpiral'] = vtkParametricConicSpiral()
    pfn['CrossCap'] = vtkParametricCrossCap()
    pfn['Dini'] = vtkParametricDini()
    pfn['Ellipsoid'] = vtkParametricEllipsoid()
    pfn['Enneper'] = vtkParametricEnneper()
    pfn['Figure8Klein'] = vtkParametricFigure8Klein()
    pfn['Klein'] = vtkParametricKlein()
    pfn['Mobius'] = vtkParametricMobius()
    pfn['RandomHills'] = vtkParametricRandomHills()
    pfn['Roman'] = vtkParametricRoman()
    pfn['SuperEllipsoid'] = vtkParametricSuperEllipsoid()
    pfn['SuperToroid'] = vtkParametricSuperToroid()
    pfn['Torus'] = vtkParametricTorus()
    pfn['Spline'] = vtkParametricSpline()
    # Extra parametric surfaces.
    pfn['BohemianDome'] = vtkParametricBohemianDome()
    pfn['Bour'] = vtkParametricBour()
    pfn['CatalanMinimal'] = vtkParametricCatalanMinimal()
    pfn['Henneberg'] = vtkParametricHenneberg()
    pfn['Kuen'] = vtkParametricKuen()
    pfn['PluckerConoid'] = vtkParametricPluckerConoid()
    pfn['Pseudosphere'] = vtkParametricPseudosphere()
    # Now set some parameters.
    pfn["Ellipsoid"].SetXRadius(0.5)
    pfn["Ellipsoid"].SetYRadius(2.0)
    pfn["Mobius"].SetRadius(2.0)
    pfn["Mobius"].SetMinimumV(-0.5)
    pfn["Mobius"].SetMaximumV(0.5)
    pfn["RandomHills"].AllowRandomGenerationOn()
    pfn["RandomHills"].SetRandomSeed(1)
    pfn["RandomHills"].SetNumberOfHills(30)
    pfn["SuperEllipsoid"].SetN1(0.5)
    pfn["SuperEllipsoid"].SetN2(0.4)
    pfn["SuperToroid"].SetN1(0.5)
    pfn["SuperToroid"].SetN2(3.0)
    # The spline needs points
    spline_points = vtkPoints()
    rng = vtkMinimalStandardRandomSequence()
    rng.SetSeed(8775070)
    for p in range(0, 10):
        xyz = [None] * 3
        for idx in range(0, len(xyz)):
            xyz[idx] = rng.GetRangeValue(-1.0, 1.0)
            rng.Next()
        spline_points.InsertNextPoint(xyz)

    pfn["Spline"].SetPoints(spline_points)
    # Extra parametric surfaces.
    pfn["BohemianDome"].SetA(5.0)
    pfn["BohemianDome"].SetB(1.0)
    pfn["BohemianDome"].SetC(2.0)
    pfn["Kuen"].SetDeltaV0(0.001)

    return pfn


def get_centre(bounds):
    """
    Get the centre of the object from the bounding box.

    :param bounds: The bounding box of the object.
    :return:
    """
    if len(bounds) != 6:
        return None
    return [bounds[i] - (bounds[i] - bounds[i - 1]) / 2.0 for i in range(1, len(bounds), 2)]


def get_maximum_length(bounds):
    """
    Calculate the maximum length of the side bounding box.

    :param bounds: The bounding box of the object.
    :return:
    """
    if len(bounds) != 6:
        return None
    return max([bounds[i] - bounds[i - 1] for i in range(1, len(bounds), 2)])


def display_bounding_box_and_center(name, bounds):
    """
    Display the dimensions of the bounding box, maximum diagonal length
     and coordinates of the centre.

    :param name: The name of the object.
    :param bounds: The bounding box of the object.
    :return:
    """
    if len(bounds) != 6:
        return
    max_len = get_maximum_length(bounds)
    centre = get_centre(bounds)
    s = '{:21s}\n'.format(name)
    s += '{:21s}{:1s}'.format('  Bounds (min, max)', ':')
    s += '{:s}({:6.2f}, {:6.2f})'.format(' x:', bounds[0], bounds[1])
    s += '{:s}({:6.2f}, {:6.2f})'.format(' y:', bounds[2], bounds[3])
    s += '{:s}({:6.2f}, {:6.2f})\n'.format(' z:', bounds[4], bounds[5])
    if max_len:
        s += '  Maximum side length: {:6.2f}\n'.format(max_len)
    if centre:
        s += '  Centre (x, y, z)   : ({:6.2f}, {:6.2f}, {:6.2f})\n'.format(centre[0], centre[1], centre[2])
    print(s)


class PrintCallback:
    def __init__(self, caller, file_name, image_quality=1, rgba=True):
        self.caller = caller
        self.image_quality = image_quality
        # rgba is the the buffer type,
        #  (if true, there is no background in the screenshot).
        self.rgba = rgba
        parent = Path(file_name).resolve().parent
        pth = Path(parent) / file_name
        self.path = Path(str(pth)).with_suffix('.png')

    def __call__(self, caller, ev):
        # Save the screenshot.
        if caller.GetKeyCode() == "k":
            w2if = vtkWindowToImageFilter()
            w2if.SetInput(caller.GetRenderWindow())
            w2if.SetScale(self.image_quality, self.image_quality)
            if self.rgba:
                w2if.SetInputBufferTypeToRGBA()
            else:
                w2if.SetInputBufferTypeToRGB()
            # Read from the front buffer.
            w2if.ReadFrontBufferOn()
            w2if.Update()
            writer = vtkPNGWriter()
            writer.SetFileName(self.path)
            writer.SetInputData(w2if.GetOutput())
            writer.Write()
            print('Screenshot saved to:', self.path.name)


if __name__ == '__main__':
    main()
