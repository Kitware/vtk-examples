from time import sleep

# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkFiltersSources import vtkConeSource
# noinspection PyUnresolvedReferences
from vtkmodules.vtkInteractionStyle import (
    vtkInteractorStyleTrackballCamera)
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def comma_separated_list(v, precision=6, width=10):
    """
    Produce a comma-separated string of numbers from a list.
    :param v: The list of floats.
    :param precision: Number of decimal places.
    :param width: The width.
    :return: A comma-separated string.
    """
    res = list()
    for p in v:
        res.append(f'{p:{width}.{precision}f}')
    return ','.join(res)


def get_orientation(ren):
    """
    Get the camera orientation.
    :param ren: The renderer.
    :return: The orientation parameters.
    """
    p = dict()
    camera = ren.GetActiveCamera()
    p['position'] = camera.GetPosition()
    p['focal point'] = camera.GetFocalPoint()
    p['view up'] = camera.GetViewUp()
    p['distance'] = camera.GetDistance()
    p['clipping range'] = camera.GetClippingRange()
    p['orientation'] = camera.GetOrientation()
    return p


def set_orientation(ren, p):
    """
    Set the orientation of the camera.
    :param ren: The renderer.
    :param p: The orientation parameters.
    :return:
    """
    camera = ren.GetActiveCamera()
    camera.SetPosition(p['position'])
    camera.SetFocalPoint(p['focal point'])
    camera.SetViewUp(p['view up'])
    camera.SetDistance(p['distance'])
    camera.SetClippingRange(p['clipping range'])


def main(argv):
    colors = vtkNamedColors()

    cone = vtkConeSource()
    cone.SetHeight(3.0)
    cone.SetRadius(1.0)
    cone.SetResolution(10)

    cone_mapper = vtkPolyDataMapper()
    cone_mapper.SetInputConnection(cone.GetOutputPort())

    cone_actor = vtkActor()
    cone_actor.SetMapper(cone_mapper)
    cone_actor.GetProperty().SetColor(colors.GetColor3d('Bisque'))

    ren = vtkRenderer()
    ren.AddActor(cone_actor)
    ren.SetBackground(colors.GetColor3d('MidnightBlue'))

    ren_win = vtkRenderWindow()
    ren_win.AddRenderer(ren)
    ren_win.SetSize(600, 600)
    ren_win.SetWindowName('ResetCameraOrientation')

    iren = vtkRenderWindowInteractor()
    iren.SetRenderWindow(ren_win)

    style = vtkInteractorStyleTrackballCamera()
    iren.SetInteractorStyle(style)

    camera = ren.GetActiveCamera()
    camera.SetRoll(15)
    camera.Elevation(-15)
    camera.Azimuth(30)
    ren.ResetCamera()

    ren_win.Render()
    original_orient = get_orientation(ren)
    s = f'{"Original orientation:":23s}'
    s += comma_separated_list(original_orient["orientation"])
    print(s)
    sleep(1)

    camera.SetPosition(-3.568189, 5.220048, 2.352639)
    camera.SetFocalPoint(-0.399044, -0.282865, 0.131438)
    camera.SetViewUp(0.623411, 0.573532, -0.531431)
    camera.SetDistance(6.727500)
    camera.SetClippingRange(3.001430, 11.434082)
    # No need to use ren.ResetCamera() as we have all the parameters.
    ren_win.Render()
    new_orient = get_orientation(ren)
    s = f'{"New orientation:":23s}'
    s += comma_separated_list(new_orient["orientation"])
    print(s)
    sleep(1)

    print('Reloading the original orientation.')
    set_orientation(ren, original_orient)
    ren_win.Render()
    check = get_orientation(ren)
    s = f'{"Final orientation:":23s}'
    s += comma_separated_list(check["orientation"])
    print(s)
    sleep(1)

    iren.Initialize()
    iren.Start()


if __name__ == '__main__':
    import sys

    main(sys.argv)
