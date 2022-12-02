# !/usr/bin/env python
# -*- coding: utf-8 -*-

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import vtkPoints
from vtkmodules.vtkCommonDataModel import (
    # VTK_HEXAGONAL_PRISM,
    # VTK_HEXAHEDRON,
    # VTK_LINE,
    # VTK_PENTAGONAL_PRISM,
    # VTK_PIXEL,
    # VTK_POLY_LINE,
    # VTK_POLY_VERTEX,
    # VTK_POLYGON,
    # VTK_PYRAMID,
    # VTK_QUAD,
    VTK_TETRA,
    # VTK_TRIANGLE,
    # VTK_TRIANGLE_STRIP,
    # VTK_VERTEX,
    # VTK_VOXEL,
    # VTK_WEDGE,
    vtkCellArray,
    vtkHexagonalPrism,
    vtkHexahedron,
    vtkLine,
    vtkPentagonalPrism,
    vtkPixel,
    vtkPolyLine,
    vtkPolyVertex,
    vtkPolygon,
    vtkPyramid,
    vtkQuad,
    vtkTetra,
    vtkTriangle,
    vtkTriangleStrip,
    vtkUnstructuredGrid,
    vtkVertex,
    vtkVoxel,
    vtkWedge
)
from vtkmodules.vtkFiltersSources import vtkSphereSource
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkActor2D,
    vtkDataSetMapper,
    vtkGlyph3DMapper,
    vtkProperty,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkTextMapper,
    vtkTextProperty
)
from vtkmodules.vtkRenderingLabel import vtkLabeledDataMapper


def get_program_parameters():
    import argparse
    description = 'Demonstrate the linear cell types found in VTK. Numbers define ordering of the defining points.'
    epilogue = '''
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    group1 = parser.add_mutually_exclusive_group()
    group1.add_argument('-w', '--wireframe', action='store_true',
                        help='Render a wireframe.')
    group1.add_argument('-b', '--backface', action='store_true',
                        help='Display the back face in a different colour.')
    args = parser.parse_args()
    return args.wireframe, args.backface


def main():
    wireframe_on, backface_on = get_program_parameters()

    titles = list()
    text_mappers = list()
    text_actors = list()

    u_grids = list()
    mappers = list()
    actors = list()
    renderers = list()

    u_grids.append(make_vertex())
    titles.append('VTK_VERTEX (=1)')
    u_grids.append(make_poly_vertex())
    titles.append('VTK_POLY_VERTEX (=2)')
    u_grids.append(make_line())
    titles.append('VTK_LINE (=3)')
    u_grids.append(make_polyline())
    titles.append('VTK_POLY_LINE (=4)')
    u_grids.append(make_triangle())
    titles.append('VTK_TRIANGLE (=5)')
    u_grids.append(make_triangle_strip())
    titles.append('VTK_TRIANGLE_STRIP (=6)')
    u_grids.append(make_polygon())
    titles.append('VTK_POLYGON (=7)')
    u_grids.append(make_pixel())
    titles.append('VTK_PIXEL (=8)')
    u_grids.append(make_quad())
    titles.append('VTK_QUAD (=9)')
    u_grids.append(make_tetra())
    titles.append('VTK_TETRA (=10)')
    u_grids.append(make_voxel())
    titles.append('VTK_VOXEL (=11)')
    u_grids.append(make_hexahedron())
    titles.append('VTK_HEXAHEDRON (=12)')
    u_grids.append(make_wedge())
    titles.append('VTK_WEDGE (=13)')
    u_grids.append(make_pyramid())
    titles.append('VTK_PYRAMID (=14)')
    u_grids.append(make_pentagonal_prism())
    titles.append('VTK_PENTAGONAL_PRISM (=15)')
    u_grids.append(make_hexagonal_prism())
    titles.append('VTK_HEXAGONAL_PRISM (=16)')

    colors = vtkNamedColors()

    ren_win = vtkRenderWindow()
    ren_win.SetWindowName('LinearCellDemo')

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    # Create one sphere for all
    sphere = vtkSphereSource()
    sphere.SetPhiResolution(21)
    sphere.SetThetaResolution(21)
    sphere.SetRadius(.08)

    # Create one text property for all
    text_property = vtkTextProperty()
    text_property.SetFontSize(10)
    text_property.SetJustificationToCentered()
    text_property.SetColor(colors.GetColor3d('Black'))

    back_property = vtkProperty()
    back_property.SetColor(colors.GetColor3d('MediumSeaGreen'))

    # Create and link the mappers actors and renderers together.
    for i in range(0, len(u_grids)):
        print('Creating:', titles[i])
        text_mappers.append(vtkTextMapper())
        text_actors.append(vtkActor2D())

        mappers.append(vtkDataSetMapper())
        actors.append(vtkActor())
        renderers.append(vtkRenderer())
        mappers[i].SetInputData(u_grids[i])
        actors[i].SetMapper(mappers[i])
        if wireframe_on:
            actors[i].GetProperty().SetRepresentationToWireframe()
            actors[i].GetProperty().SetLineWidth(2)
            actors[i].GetProperty().SetOpacity(1)
            actors[i].GetProperty().SetColor(colors.GetColor3d('Black'))
        else:
            actors[i].GetProperty().EdgeVisibilityOn()
            actors[i].GetProperty().SetLineWidth(3)
            actors[i].GetProperty().SetColor(colors.GetColor3d('Tomato'))
            if backface_on:
                actors[i].SetBackfaceProperty(back_property)
                actors[i].GetProperty().SetOpacity(1)
            else:
                actors[i].GetProperty().SetOpacity(0.5)
        renderers[i].AddViewProp(actors[i])

        text_mappers[i].SetInput(titles[i])
        text_actors[i].SetMapper(text_mappers[i])
        text_actors[i].SetPosition(50, 10)
        if wireframe_on:
            text_actors[i].GetProperty().SetColor(colors.GetColor3d('Black'))
        renderers[i].AddViewProp(text_actors[i])

        # Label the points
        label_mapper = vtkLabeledDataMapper()
        label_mapper.SetInputData(u_grids[i])
        label_actor = vtkActor2D()
        label_actor.SetMapper(label_mapper)
        if wireframe_on:
            label_actor.GetProperty().SetColor(colors.GetColor3d('Snow'))
        renderers[i].AddViewProp(label_actor)

        # Glyph the points
        point_mapper = vtkGlyph3DMapper()
        point_mapper.SetInputData(u_grids[i])
        point_mapper.SetSourceConnection(sphere.GetOutputPort())
        point_mapper.ScalingOn()
        point_mapper.ScalarVisibilityOff()

        point_actor = vtkActor()
        point_actor.SetMapper(point_mapper)
        if wireframe_on:
            point_actor.GetProperty().SetColor(colors.GetColor3d('Banana'))
        else:
            point_actor.GetProperty().SetColor(colors.GetColor3d('Banana'))
        point_actor.GetProperty().SetSpecular(.6)
        point_actor.GetProperty().SetSpecularColor(1.0, 1.0, 1.0)
        point_actor.GetProperty().SetSpecularPower(100)
        renderers[i].AddViewProp(point_actor)

        ren_win.AddRenderer(renderers[i])

    # Set up the viewports
    grid_dimensions_x = 4
    grid_dimensions_y = 4
    renderer_size = 300
    ren_win.SetSize(renderer_size * grid_dimensions_x, renderer_size * grid_dimensions_y)
    for row in range(0, grid_dimensions_y):
        for col in range(0, grid_dimensions_x):
            index = row * grid_dimensions_x + col

            # (xmin, ymin, xmax, ymax)
            viewport = [float(col) / grid_dimensions_x,
                        float(grid_dimensions_y - (row + 1)) / grid_dimensions_y,
                        float(col + 1) / grid_dimensions_x,
                        float(grid_dimensions_y - row) / grid_dimensions_y]

            if index > (len(actors) - 1):
                # Add a renderer even if there is no actor.
                # This makes the render window background all the same color.
                ren = vtkRenderer()
                if wireframe_on:
                    ren.SetBackground(colors.GetColor3d('LightSlateGray'))
                else:
                    ren.SetBackground(colors.GetColor3d('SlateGray'))
                ren.SetViewport(viewport)
                ren_win.AddRenderer(ren)
                continue

            if wireframe_on:
                renderers[index].SetBackground(colors.GetColor3d('LightSlateGray'))
            else:
                renderers[index].SetBackground(colors.GetColor3d('SlateGray'))
            renderers[index].SetViewport(viewport)
            renderers[index].ResetCamera()
            if index == 0:
                renderers[index].GetActiveCamera().Dolly(0.1)
                renderers[index].GetActiveCamera().Azimuth(30)
                renderers[index].GetActiveCamera().Elevation(-30)
            elif index == 1:
                renderers[index].GetActiveCamera().Dolly(0.8)
                renderers[index].GetActiveCamera().Azimuth(30)
                renderers[index].GetActiveCamera().Elevation(-30)
            elif index == 2:
                renderers[index].GetActiveCamera().Dolly(0.4)
                renderers[index].GetActiveCamera().Azimuth(30)
                renderers[index].GetActiveCamera().Elevation(-30)
            elif index == 4:
                renderers[index].GetActiveCamera().Dolly(0.7)
                renderers[index].GetActiveCamera().Azimuth(30)
                renderers[index].GetActiveCamera().Elevation(-30)
            elif index == 5:
                renderers[index].GetActiveCamera().Dolly(1.1)
                renderers[index].GetActiveCamera().Azimuth(30)
                renderers[index].GetActiveCamera().Elevation(-30)
            elif index == 6:
                renderers[index].GetActiveCamera().Azimuth(0)
                renderers[index].GetActiveCamera().Elevation(-45)
            elif index == 7:
                renderers[index].GetActiveCamera().Azimuth(0)
                renderers[index].GetActiveCamera().Elevation(-45)
            elif index == 8:
                renderers[index].GetActiveCamera().Azimuth(0)
                renderers[index].GetActiveCamera().Elevation(-45)
            elif index == 9:
                renderers[index].GetActiveCamera().Azimuth(0)
                renderers[index].GetActiveCamera().Elevation(-22.5)
            elif index == 10:
                renderers[index].GetActiveCamera().Azimuth(-22.5)
                renderers[index].GetActiveCamera().Elevation(15)
            elif index == 11:
                renderers[index].GetActiveCamera().Azimuth(-22.5)
                renderers[index].GetActiveCamera().Elevation(15)
            elif index == 12:
                renderers[index].GetActiveCamera().Azimuth(-45)
                renderers[index].GetActiveCamera().Elevation(15)
            elif index == 13:
                renderers[index].GetActiveCamera().Azimuth(0)
                renderers[index].GetActiveCamera().Elevation(-30)
            elif index == 14:
                renderers[index].GetActiveCamera().Azimuth(-22.5)
                renderers[index].GetActiveCamera().Elevation(10)
            elif index == 15:
                renderers[index].GetActiveCamera().Azimuth(-30)
                renderers[index].GetActiveCamera().Elevation(15)
            else:
                renderers[index].GetActiveCamera().Azimuth(30)
                renderers[index].GetActiveCamera().Elevation(-30)
            renderers[index].ResetCameraClippingRange()

    ren_win.Render()
    iren.Initialize()
    iren.Start()


# These functions return a vtkUnstructured grid corresponding to the object.

def make_vertex():
    # A vertex is a cell that represents a 3D point
    number_of_vertices = 1

    points = vtkPoints()
    points.InsertNextPoint(0, 0, 0)

    vertex = vtkVertex()
    for i in range(0, number_of_vertices):
        vertex.GetPointIds().SetId(i, i)

    ug = vtkUnstructuredGrid()
    ug.SetPoints(points)
    ug.InsertNextCell(vertex.GetCellType(), vertex.GetPointIds())

    return ug


def make_poly_vertex():
    # A polyvertex is a cell represents a set of 0D vertices
    number_of_vertices = 6

    points = vtkPoints()
    points.InsertNextPoint(0, 0, 0)
    points.InsertNextPoint(1, 0, 0)
    points.InsertNextPoint(0, 1, 0)
    points.InsertNextPoint(0, 0, 1)
    points.InsertNextPoint(1, 0, 0.4)
    points.InsertNextPoint(0, 1, 0.6)

    poly_vertex = vtkPolyVertex()
    poly_vertex.GetPointIds().SetNumberOfIds(number_of_vertices)

    for i in range(0, number_of_vertices):
        poly_vertex.GetPointIds().SetId(i, i)

    ug = vtkUnstructuredGrid()
    ug.SetPoints(points)
    ug.InsertNextCell(poly_vertex.GetCellType(), poly_vertex.GetPointIds())

    return ug


def make_line():
    # A line is a cell that represents a 1D point
    number_of_vertices = 2

    points = vtkPoints()
    points.InsertNextPoint(0, 0, 0)
    points.InsertNextPoint(0.5, 0.5, 0)

    line = vtkLine()
    for i in range(0, number_of_vertices):
        line.GetPointIds().SetId(i, i)

    ug = vtkUnstructuredGrid()
    ug.SetPoints(points)
    ug.InsertNextCell(line.GetCellType(), line.GetPointIds())

    return ug


def make_polyline():
    # A polyline is a cell that represents a set of 1D lines
    number_of_vertices = 5

    points = vtkPoints()
    points.InsertNextPoint(0, 0.5, 0)
    points.InsertNextPoint(0.5, 0, 0)
    points.InsertNextPoint(1, 0.3, 0)
    points.InsertNextPoint(1.5, 0.4, 0)
    points.InsertNextPoint(2.0, 0.4, 0)

    polyline = vtkPolyLine()
    polyline.GetPointIds().SetNumberOfIds(number_of_vertices)

    for i in range(0, number_of_vertices):
        polyline.GetPointIds().SetId(i, i)

    ug = vtkUnstructuredGrid()
    ug.SetPoints(points)
    ug.InsertNextCell(polyline.GetCellType(), polyline.GetPointIds())

    return ug


def make_triangle():
    # A triangle is a cell that represents a 1D point
    number_of_vertices = 3

    points = vtkPoints()
    points.InsertNextPoint(0, 0, 0)
    points.InsertNextPoint(0.5, 0.5, 0)
    points.InsertNextPoint(.2, 1, 0)

    triangle = vtkTriangle()
    for i in range(0, number_of_vertices):
        triangle.GetPointIds().SetId(i, i)

    ug = vtkUnstructuredGrid()
    ug.SetPoints(points)
    ug.InsertNextCell(triangle.GetCellType(), triangle.GetPointIds())

    return ug


def make_triangle_strip():
    # A triangle is a cell that represents a triangle strip
    number_of_vertices = 10

    points = vtkPoints()
    points.InsertNextPoint(0, 0, 0)
    points.InsertNextPoint(1, -.1, 0)
    points.InsertNextPoint(0.5, 1, 0)
    points.InsertNextPoint(2.0, -0.1, 0)
    points.InsertNextPoint(1.5, 0.8, 0)
    points.InsertNextPoint(3.0, 0, 0)
    points.InsertNextPoint(2.5, 0.9, 0)
    points.InsertNextPoint(4.0, -0.2, 0)
    points.InsertNextPoint(3.5, 0.8, 0)
    points.InsertNextPoint(4.5, 1.1, 0)

    trianglestrip = vtkTriangleStrip()
    trianglestrip.GetPointIds().SetNumberOfIds(number_of_vertices)
    for i in range(0, number_of_vertices):
        trianglestrip.GetPointIds().SetId(i, i)

    ug = vtkUnstructuredGrid()
    ug.SetPoints(points)
    ug.InsertNextCell(trianglestrip.GetCellType(), trianglestrip.GetPointIds())

    return ug


def make_polygon():
    # A polygon is a cell that represents a polygon
    number_of_vertices = 6

    points = vtkPoints()
    points.InsertNextPoint(0, 0, 0)
    points.InsertNextPoint(1, -0.1, 0)
    points.InsertNextPoint(0.8, 0.5, 0)
    points.InsertNextPoint(1, 1, 0)
    points.InsertNextPoint(0.6, 1.2, 0)
    points.InsertNextPoint(0, 0.8, 0)

    polygon = vtkPolygon()
    polygon.GetPointIds().SetNumberOfIds(number_of_vertices)
    for i in range(0, number_of_vertices):
        polygon.GetPointIds().SetId(i, i)

    ug = vtkUnstructuredGrid()
    ug.SetPoints(points)
    ug.InsertNextCell(polygon.GetCellType(), polygon.GetPointIds())

    return ug


def make_pixel():
    # A pixel is a cell that represents a pixel
    pixel = vtkPixel()
    pixel.GetPoints().SetPoint(0, 0, 0, 0)
    pixel.GetPoints().SetPoint(1, 1, 0, 0)
    pixel.GetPoints().SetPoint(2, 0, 1, 0)
    pixel.GetPoints().SetPoint(3, 1, 1, 0)

    pixel.GetPointIds().SetId(0, 0)
    pixel.GetPointIds().SetId(1, 1)
    pixel.GetPointIds().SetId(2, 2)
    pixel.GetPointIds().SetId(3, 3)

    ug = vtkUnstructuredGrid()
    ug.SetPoints(pixel.GetPoints())
    ug.InsertNextCell(pixel.GetCellType(), pixel.GetPointIds())

    return ug


def make_quad():
    # A quad is a cell that represents a quad
    quad = vtkQuad()
    quad.GetPoints().SetPoint(0, 0, 0, 0)
    quad.GetPoints().SetPoint(1, 1, 0, 0)
    quad.GetPoints().SetPoint(2, 1, 1, 0)
    quad.GetPoints().SetPoint(3, 0, 1, 0)

    quad.GetPointIds().SetId(0, 0)
    quad.GetPointIds().SetId(1, 1)
    quad.GetPointIds().SetId(2, 2)
    quad.GetPointIds().SetId(3, 3)

    ug = vtkUnstructuredGrid()
    ug.SetPoints(quad.GetPoints())
    ug.InsertNextCell(quad.GetCellType(), quad.GetPointIds())

    return ug


def make_tetra():
    # Make a tetrahedron.
    number_of_vertices = 4

    points = vtkPoints()
    points.InsertNextPoint(0, 0, 0)
    points.InsertNextPoint(1, 0, 0)
    points.InsertNextPoint(1, 1, 0)
    points.InsertNextPoint(0, 1, 1)

    tetra = vtkTetra()
    for i in range(0, number_of_vertices):
        tetra.GetPointIds().SetId(i, i)

    cell_array = vtkCellArray()
    cell_array.InsertNextCell(tetra)

    ug = vtkUnstructuredGrid()
    ug.SetPoints(points)
    ug.SetCells(VTK_TETRA, cell_array)

    return ug


def make_voxel():
    # A voxel is a representation of a regular grid in 3-D space.
    number_of_vertices = 8

    points = vtkPoints()
    points.InsertNextPoint(0, 0, 0)
    points.InsertNextPoint(1, 0, 0)
    points.InsertNextPoint(0, 1, 0)
    points.InsertNextPoint(1, 1, 0)
    points.InsertNextPoint(0, 0, 1)
    points.InsertNextPoint(1, 0, 1)
    points.InsertNextPoint(0, 1, 1)
    points.InsertNextPoint(1, 1, 1)

    voxel = vtkVoxel()
    for i in range(0, number_of_vertices):
        voxel.GetPointIds().SetId(i, i)

    ug = vtkUnstructuredGrid()
    ug.SetPoints(points)
    ug.InsertNextCell(voxel.GetCellType(), voxel.GetPointIds())

    return ug


def make_hexahedron():
    # A regular hexagon (cube) with all faces square and three squares around
    # each vertex is created below.

    # Set up the coordinates of eight points
    # (the two faces must be in counter-clockwise
    # order as viewed from the outside).

    number_of_vertices = 8

    # Create the points
    points = vtkPoints()
    points.InsertNextPoint(0, 0, 0)
    points.InsertNextPoint(1, 0, 0)
    points.InsertNextPoint(1, 1, 0)
    points.InsertNextPoint(0, 1, 0)
    points.InsertNextPoint(0, 0, 1)
    points.InsertNextPoint(1, 0, 1)
    points.InsertNextPoint(1, 1, 1)
    points.InsertNextPoint(0, 1, 1)

    # Create a hexahedron from the points
    hexhedr = vtkHexahedron()
    for i in range(0, number_of_vertices):
        hexhedr.GetPointIds().SetId(i, i)

    # Add the points and hexahedron to an unstructured grid
    ug = vtkUnstructuredGrid()
    ug.SetPoints(points)
    ug.InsertNextCell(hexhedr.GetCellType(), hexhedr.GetPointIds())

    return ug


def make_wedge():
    # A wedge consists of two triangular ends and three rectangular faces.

    number_of_vertices = 6

    points = vtkPoints()

    points.InsertNextPoint(0, 1, 0)
    points.InsertNextPoint(0, 0, 0)
    points.InsertNextPoint(0, 0.5, 0.5)
    points.InsertNextPoint(1, 1, 0)
    points.InsertNextPoint(1, 0.0, 0.0)
    points.InsertNextPoint(1, 0.5, 0.5)

    wedge = vtkWedge()
    for i in range(0, number_of_vertices):
        wedge.GetPointIds().SetId(i, i)

    ug = vtkUnstructuredGrid()
    ug.SetPoints(points)
    ug.InsertNextCell(wedge.GetCellType(), wedge.GetPointIds())

    return ug


def make_pyramid():
    # Make a regular square pyramid.
    number_of_vertices = 5

    points = vtkPoints()

    p0 = [1.0, 1.0, 0.0]
    p1 = [-1.0, 1.0, 0.0]
    p2 = [-1.0, -1.0, 0.0]
    p3 = [1.0, -1.0, 0.0]
    p4 = [0.0, 0.0, 1.0]

    points.InsertNextPoint(p0)
    points.InsertNextPoint(p1)
    points.InsertNextPoint(p2)
    points.InsertNextPoint(p3)
    points.InsertNextPoint(p4)

    pyramid = vtkPyramid()
    for i in range(0, number_of_vertices):
        pyramid.GetPointIds().SetId(i, i)

    ug = vtkUnstructuredGrid()
    ug.SetPoints(points)
    ug.InsertNextCell(pyramid.GetCellType(), pyramid.GetPointIds())

    return ug


def make_pentagonal_prism():
    pentagonal_prism = vtkPentagonalPrism()

    pentagonal_prism.GetPointIds().SetId(0, 0)
    pentagonal_prism.GetPointIds().SetId(1, 1)
    pentagonal_prism.GetPointIds().SetId(2, 2)
    pentagonal_prism.GetPointIds().SetId(3, 3)
    pentagonal_prism.GetPointIds().SetId(4, 4)
    pentagonal_prism.GetPointIds().SetId(5, 5)
    pentagonal_prism.GetPointIds().SetId(6, 6)
    pentagonal_prism.GetPointIds().SetId(7, 7)
    pentagonal_prism.GetPointIds().SetId(8, 8)
    pentagonal_prism.GetPointIds().SetId(9, 9)

    scale = 2.0
    pentagonal_prism.GetPoints().SetPoint(0, 11 / scale, 10 / scale, 10 / scale)
    pentagonal_prism.GetPoints().SetPoint(1, 13 / scale, 10 / scale, 10 / scale)
    pentagonal_prism.GetPoints().SetPoint(2, 14 / scale, 12 / scale, 10 / scale)
    pentagonal_prism.GetPoints().SetPoint(3, 12 / scale, 14 / scale, 10 / scale)
    pentagonal_prism.GetPoints().SetPoint(4, 10 / scale, 12 / scale, 10 / scale)
    pentagonal_prism.GetPoints().SetPoint(5, 11 / scale, 10 / scale, 14 / scale)
    pentagonal_prism.GetPoints().SetPoint(6, 13 / scale, 10 / scale, 14 / scale)
    pentagonal_prism.GetPoints().SetPoint(7, 14 / scale, 12 / scale, 14 / scale)
    pentagonal_prism.GetPoints().SetPoint(8, 12 / scale, 14 / scale, 14 / scale)
    pentagonal_prism.GetPoints().SetPoint(9, 10 / scale, 12 / scale, 14 / scale)

    ug = vtkUnstructuredGrid()
    ug.SetPoints(pentagonal_prism.GetPoints())
    ug.InsertNextCell(pentagonal_prism.GetCellType(), pentagonal_prism.GetPointIds())

    return ug


def make_hexagonal_prism():
    hexagonal_prism = vtkHexagonalPrism()
    hexagonal_prism.GetPointIds().SetId(0, 0)
    hexagonal_prism.GetPointIds().SetId(1, 1)
    hexagonal_prism.GetPointIds().SetId(2, 2)
    hexagonal_prism.GetPointIds().SetId(3, 3)
    hexagonal_prism.GetPointIds().SetId(4, 4)
    hexagonal_prism.GetPointIds().SetId(5, 5)
    hexagonal_prism.GetPointIds().SetId(6, 6)
    hexagonal_prism.GetPointIds().SetId(7, 7)
    hexagonal_prism.GetPointIds().SetId(8, 8)
    hexagonal_prism.GetPointIds().SetId(9, 9)
    hexagonal_prism.GetPointIds().SetId(10, 10)
    hexagonal_prism.GetPointIds().SetId(11, 11)

    scale = 2.0
    hexagonal_prism.GetPoints().SetPoint(0, 11 / scale, 10 / scale, 10 / scale)
    hexagonal_prism.GetPoints().SetPoint(1, 13 / scale, 10 / scale, 10 / scale)
    hexagonal_prism.GetPoints().SetPoint(2, 14 / scale, 12 / scale, 10 / scale)
    hexagonal_prism.GetPoints().SetPoint(3, 13 / scale, 14 / scale, 10 / scale)
    hexagonal_prism.GetPoints().SetPoint(4, 11 / scale, 14 / scale, 10 / scale)
    hexagonal_prism.GetPoints().SetPoint(5, 10 / scale, 12 / scale, 10 / scale)
    hexagonal_prism.GetPoints().SetPoint(6, 11 / scale, 10 / scale, 14 / scale)
    hexagonal_prism.GetPoints().SetPoint(7, 13 / scale, 10 / scale, 14 / scale)
    hexagonal_prism.GetPoints().SetPoint(8, 14 / scale, 12 / scale, 14 / scale)
    hexagonal_prism.GetPoints().SetPoint(9, 13 / scale, 14 / scale, 14 / scale)
    hexagonal_prism.GetPoints().SetPoint(10, 11 / scale, 14 / scale, 14 / scale)
    hexagonal_prism.GetPoints().SetPoint(11, 10 / scale, 12 / scale, 14 / scale)

    ug = vtkUnstructuredGrid()
    ug.SetPoints(hexagonal_prism.GetPoints())
    ug.InsertNextCell(hexagonal_prism.GetCellType(), hexagonal_prism.GetPointIds())

    return ug


if __name__ == '__main__':
    main()
