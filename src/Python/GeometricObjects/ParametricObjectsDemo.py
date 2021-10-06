#!/usr/bin/env python

"""
    Demonstrate all the parametric objects.
"""

import collections
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
    vtkBMPWriter,
    vtkJPEGWriter,
    vtkPNGWriter,
    vtkPNMWriter,
    vtkPostScriptWriter,
    vtkTIFFWriter
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
    parser.add_argument('-w', '--write_image', action='store_true', help='Write out the the image.')
    parser.add_argument('-b', '--back_face', action='store_true', help='Color the back-face.')
    parser.add_argument('-n', '--normals', action='store_true', help='Display normals.')
    args = parser.parse_args()
    return args.back_face, args.normals, args.surface_name, args.write_image


def main():
    back_face, normals, surface_name, write_out_image = get_program_parameters()

    colors = vtkNamedColors()

    if surface_name:
        rendererSize = 800
        gridColumnDimensions = 1
        gridRowDimensions = 1
    else:
        rendererSize = 200
        gridColumnDimensions = 5
        gridRowDimensions = 5

    # Create one text property for all
    textProperty = vtkTextProperty()
    textProperty.SetJustificationToCentered()
    textProperty.SetFontSize(int(rendererSize / 12))
    textProperty.SetColor(colors.GetColor3d("LavenderBlush"))

    # Create a parametric function source, renderer, mapper, and actor
    # for each object
    pfnSrcs = []
    renderers = []
    mappers = []
    actors = []
    textmappers = []
    textactors = []

    # Glyph the normals
    maskPts = []
    arrow = []
    glyph = []
    glyphMapper = []
    glyphActor = []

    backProperty = vtkProperty()
    if back_face:
        backProperty.SetColor(colors.GetColor3d("Peru"))

    boundingBox = []

    # Get the parametric functions and build the pipeline
    pfn = get_parametric_functions()

    if surface_name:
        # is the surface in the map?
        surface_exists = list()
        for t in pfn.keys():
            for obj in pfn[t].keys():
                if surface_name == obj:
                    surface_exists.append(True)
                else:
                    surface_exists.append(False)
        if not any(surface_exists):
            print('Nonexistent object:', surface_name)
            return

    single_surface = list()
    obj_count = 0
    sorted_names = list()
    for t in sorted(pfn.keys()):
        for obj in sorted(pfn[t].keys()):
            sorted_names.append(obj)
            if surface_name:
                if obj == surface_name:
                    single_surface = [surface_name, obj_count]
            pfnSrcs.append(vtkParametricFunctionSource())
            pfnSrcs[obj_count].SetParametricFunction(pfn[t][obj])
            pfnSrcs[obj_count].SetUResolution(51)
            pfnSrcs[obj_count].SetVResolution(51)
            pfnSrcs[obj_count].SetWResolution(51)
            pfnSrcs[obj_count].Update()

            mappers.append(vtkPolyDataMapper())
            mappers[obj_count].SetInputConnection(pfnSrcs[obj_count].GetOutputPort())

            actors.append(vtkActor())
            actors[obj_count].SetMapper(mappers[obj_count])
            actors[obj_count].GetProperty().SetColor(colors.GetColor3d("NavajoWhite"))
            if back_face:
                actors[obj_count].SetBackfaceProperty(backProperty)

            textmappers.append(vtkTextMapper())
            textmappers[obj_count].SetInput(obj)
            textmappers[obj_count].SetTextProperty(textProperty)

            textactors.append(vtkActor2D())
            textactors[obj_count].SetMapper(textmappers[obj_count])
            textactors[obj_count].SetPosition(rendererSize / 2.0, 8)

            renderers.append(vtkRenderer())

            bounds = pfnSrcs[obj_count].GetOutput().GetBounds()
            boundingBox.append(bounds)
            # display_bounding_box_and_center(obj, obj_count, bounds)

            if normals:
                # Glyphing
                maskPts.append(vtkMaskPoints())
                maskPts[obj_count].RandomModeOn()
                maskPts[obj_count].SetMaximumNumberOfPoints(150)
                maskPts[obj_count].SetInputConnection(pfnSrcs[obj_count].GetOutputPort())

                arrow.append(vtkArrowSource())
                arrow[obj_count].SetTipResolution(16)
                arrow[obj_count].SetTipLength(0.3)
                arrow[obj_count].SetTipRadius(0.1)

                glyphScale = get_maximum_length(boundingBox[obj_count])

                glyph.append(vtkGlyph3D())
                glyph[obj_count].SetSourceConnection(arrow[obj_count].GetOutputPort())
                glyph[obj_count].SetInputConnection(maskPts[obj_count].GetOutputPort())
                glyph[obj_count].SetVectorModeToUseNormal()
                glyph[obj_count].SetScaleFactor(glyphScale / 10.0)
                glyph[obj_count].OrientOn()
                glyph[obj_count].Update()

                glyphMapper.append(vtkPolyDataMapper())
                glyphMapper[obj_count].SetInputConnection(
                    glyph[obj_count].GetOutputPort())

                glyphActor.append(vtkActor())
                glyphActor[obj_count].SetMapper(glyphMapper[obj_count])
                glyphActor[obj_count].GetProperty().SetColor(colors.GetColor3d("GreenYellow"))

            obj_count += 1
    # Need a renderer even if there is no actor
    for i in range(obj_count, gridColumnDimensions * gridRowDimensions):
        renderers.append(vtkRenderer())
        renderers[i].SetBackground(colors.GetColor3d("MidnightBlue"))
        sorted_names.append(None)

    renderWindow = vtkRenderWindow()
    renderWindow.SetSize(rendererSize * gridColumnDimensions, rendererSize * gridRowDimensions)

    for row in range(0, gridRowDimensions):
        for col in range(0, gridColumnDimensions):
            # (xmin, ymin, xmax, ymax)
            viewport = [
                float(col) * rendererSize / (gridColumnDimensions * rendererSize),
                float(gridRowDimensions - (row + 1)) * rendererSize / (gridRowDimensions * rendererSize),
                float(col + 1) * rendererSize / (gridColumnDimensions * rendererSize),
                float(gridRowDimensions - row) * rendererSize / (gridRowDimensions * rendererSize)]
            if not surface_name:
                index = row * gridColumnDimensions + col
                renderWindow.AddRenderer(renderers[index])
                renderers[index].SetViewport(viewport)
                if index > obj_count - 1:
                    continue
                renderers[index].AddActor(actors[index])
                # Normals can only be computed for polygons and triangle strips.
                # The Spline is a line.
                if normals and sorted_names[index] != 'Spline':
                    renderers[index].AddActor(glyphActor[index])
                renderers[index].AddActor(textactors[index])
                renderers[index].SetBackground(
                    colors.GetColor3d("MidnightBlue"))
                renderers[index].ResetCamera()
                renderers[index].GetActiveCamera().Azimuth(30)
                renderers[index].GetActiveCamera().Elevation(-30)
                renderers[index].GetActiveCamera().Zoom(0.9)
                renderers[index].ResetCameraClippingRange()
            else:
                index = single_surface[1]
                if index != -1:
                    renderWindow.AddRenderer(renderers[index])
                    renderers[index].SetViewport(viewport)
                    renderers[index].AddActor(actors[index])
                    # Normals can only be computed for polygons and triangle strips.
                    # The Spline is a line.
                    if normals and sorted_names[index] != 'Spline':
                        renderers[index].AddActor(glyphActor[index])
                    renderers[index].AddActor(textactors[index])
                    renderers[index].SetBackground(colors.GetColor3d("MidnightBlue"))
                    renderers[index].ResetCamera()
                    renderers[index].GetActiveCamera().Azimuth(30)
                    renderers[index].GetActiveCamera().Elevation(-30)
                    renderers[index].GetActiveCamera().Zoom(0.9)
                    renderers[index].ResetCameraClippingRange()

    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(renderWindow)

    renderWindow.Render()
    if surface_name:
        renderWindow.SetWindowName(single_surface[0])
    else:
        renderWindow.SetWindowName("ParametricObjectsDemo")
    renderWindow.Render()
    if write_out_image:
        # -------------------------------
        # Save the image
        # -------------------------------
        if surface_name:
            write_image(single_surface[0], renderWindow, rgba=False)
        else:
            write_image('ParametricObjectsDemo', renderWindow, rgba=False)
    interactor.Start()


def get_parametric_functions():
    """
    Create a map of the parametric functions and set some parameters.
    The first key groups the parametric functions and the
      second key is the name of the function.

    :return: The map of functions.
    """
    # We could use OrderedDict if Python version >= 3.2
    pfn = collections.defaultdict(collections.defaultdict)
    pfn[0]['Boy'] = vtkParametricBoy()
    pfn[0]['ConicSpiral'] = vtkParametricConicSpiral()
    pfn[0]['CrossCap'] = vtkParametricCrossCap()
    pfn[0]['Dini'] = vtkParametricDini()
    pfn[0]['Ellipsoid'] = vtkParametricEllipsoid()
    pfn[0]['Enneper'] = vtkParametricEnneper()
    pfn[0]['Figure8Klein'] = vtkParametricFigure8Klein()
    pfn[0]['Klein'] = vtkParametricKlein()
    pfn[0]['Mobius'] = vtkParametricMobius()
    pfn[0]['RandomHills'] = vtkParametricRandomHills()
    pfn[0]['Roman'] = vtkParametricRoman()
    pfn[0]['SuperEllipsoid'] = vtkParametricSuperEllipsoid()
    pfn[0]['SuperToroid'] = vtkParametricSuperToroid()
    pfn[0]['Torus'] = vtkParametricTorus()
    pfn[0]['Spline'] = vtkParametricSpline()
    # Extra parametric surfaces.
    pfn[1]['BohemianDome'] = vtkParametricBohemianDome()
    pfn[1]['Bour'] = vtkParametricBour()
    pfn[1]['CatalanMinimal'] = vtkParametricCatalanMinimal()
    pfn[1]['Henneberg'] = vtkParametricHenneberg()
    pfn[1]['Kuen'] = vtkParametricKuen()
    pfn[1]['PluckerConoid'] = vtkParametricPluckerConoid()
    pfn[1]['Pseudosphere'] = vtkParametricPseudosphere()
    # Now set some parameters.
    pfn[0]["Ellipsoid"].SetXRadius(0.5)
    pfn[0]["Ellipsoid"].SetYRadius(2.0)
    pfn[0]["Mobius"].SetRadius(2.0)
    pfn[0]["Mobius"].SetMinimumV(-0.5)
    pfn[0]["Mobius"].SetMaximumV(0.5)
    pfn[0]["RandomHills"].AllowRandomGenerationOn()
    pfn[0]["RandomHills"].SetRandomSeed(1)
    pfn[0]["RandomHills"].SetNumberOfHills(30)
    pfn[0]["SuperEllipsoid"].SetN1(0.5)
    pfn[0]["SuperEllipsoid"].SetN2(0.4)
    pfn[0]["SuperToroid"].SetN1(0.5)
    pfn[0]["SuperToroid"].SetN2(3.0)
    # The spline needs points
    inputPoints = vtkPoints()
    rng = vtkMinimalStandardRandomSequence()
    rng.SetSeed(8775070)
    for p in range(0, 10):
        xyz = [None] * 3
        for idx in range(0, len(xyz)):
            xyz[idx] = rng.GetRangeValue(-1.0, 1.0)
            rng.Next()
        inputPoints.InsertNextPoint(xyz)

    pfn[0]["Spline"].SetPoints(inputPoints)
    # Extra parametric surfaces.
    pfn[1]["BohemianDome"].SetA(5.0)
    pfn[1]["BohemianDome"].SetB(1.0)
    pfn[1]["BohemianDome"].SetC(2.0)
    pfn[1]["Kuen"].SetDeltaV0(0.001)

    return pfn


def get_centre(bounds):
    """
    Get the centre of the object from the bounding box.
    """
    if len(bounds) != 6:
        return [None] * 6
    return [bounds[i] - (bounds[i] - bounds[i - 1]) / 2.0 for i in range(1, len(bounds), 2)]


def get_maximum_length(bounds):
    """
    Calculate the maximum length of the side bounding box.
    """
    if len(bounds) != 6:
        return None
    return max([bounds[i] - bounds[i - 1] for i in range(1, len(bounds), 2)])


def display_bounding_box_and_center(name, index, bounds):
    if len(bounds) != 6:
        return
    max_len = get_maximum_length(bounds)
    centre = get_centre(bounds)
    s = '{:21s}: {:>2d}\n'.format(name, index)
    s += '{:21s}{:1s}'.format('  Bounds (min, max)', ':')
    s += '{:s}({:6.2f}, {:6.2f})'.format(' x:', bounds[0], bounds[1])
    s += '{:s}({:6.2f}, {:6.2f})'.format(' y:', bounds[2], bounds[3])
    s += '{:s}({:6.2f}, {:6.2f})\n'.format(' z:', bounds[4], bounds[5])
    s += '  Maximum side length: {:6.2f}\n'.format(max_len)
    s += '  Centre (x, y, z)   : ({:6.2f}, {:6.2f}, {:6.2f})\n'.format(centre[0], centre[1], centre[2])
    print(s)


def write_image(file_name, ren_win, rgba=True):
    """
    Write the render window view to an image file.

    Image types supported are:
     BMP, JPEG, PNM, PNG, PostScript, TIFF.
    The default parameters are used for all writers, change as needed.

    :param file_name: The file name, if no extension then PNG is assumed.
    :param ren_win: The render window.
    :param rgba: Used to set the buffer type.
    :return:
    """

    if file_name:
        valid_suffixes = ['.bmp', '.jpg', '.png', '.pnm', '.ps', '.tiff']
        # Select the writer to use.
        parent = Path(file_name).resolve().parent
        path = Path(parent) / file_name
        if path.suffix:
            ext = path.suffix.lower()
        else:
            ext = '.png'
            path = Path(str(path)).with_suffix(ext)
        if path.suffix not in valid_suffixes:
            print(f'No writer for this file suffix: {ext}')
            return
        if ext == '.bmp':
            writer = vtkBMPWriter()
        elif ext == '.jpg':
            writer = vtkJPEGWriter()
        elif ext == '.pnm':
            writer = vtkPNMWriter()
        elif ext == '.ps':
            if rgba:
                rgba = False
            writer = vtkPostScriptWriter()
        elif ext == '.tiff':
            writer = vtkTIFFWriter()
        else:
            writer = vtkPNGWriter()

        windowto_image_filter = vtkWindowToImageFilter()
        windowto_image_filter.SetInput(ren_win)
        windowto_image_filter.SetScale(1)  # image quality
        if rgba:
            windowto_image_filter.SetInputBufferTypeToRGBA()
        else:
            windowto_image_filter.SetInputBufferTypeToRGB()
            # Read from the front buffer.
            windowto_image_filter.ReadFrontBufferOff()
            windowto_image_filter.Update()

        writer.SetFileName(path)
        writer.SetInputConnection(windowto_image_filter.GetOutputPort())
        writer.Write()
    else:
        raise RuntimeError('Need a filename.')


if __name__ == '__main__':
    main()
