##!/usr/bin/env python

# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkIOImage import vtkHDRReader
from vtkmodules.vtkInteractionImage import vtkImageViewer
from vtkmodules.vtkRenderingCore import vtkRenderWindowInteractor


def get_program_parameters():
    import argparse
    description = 'Demonstrate the use of HDRReader'
    epilogue = '''
This example shows how to read in an HDR file.
A callback is used to print out the color window (move the mouse horizontally over the image)
 and color level (move the mouse vertically over the image).

   '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('file_name', help='Path to the hdr file e.g. Skyboxes/spiaggia_di_mondello_1k.hdr.')
    args = parser.parse_args()
    return args.file_name


def main():
    file_name = get_program_parameters()

    reader = vtkHDRReader()

    # Check the image can be read
    if not reader.CanReadFile(file_name):
        print('CanReadFile failed for ', file_name)
        return

    reader.SetFileName(file_name)
    reader.UpdateInformation()

    # Whole extent
    we = reader.GetDataExtent()
    extents = [we[0], we[1], we[2], we[3], 0, 0]
    reader.UpdateExtent(extents)
    # Visualize
    image_viewer = vtkImageViewer()
    image_viewer.SetInputData(reader.GetOutput())

    image_viewer.SetColorWindow(1)
    image_viewer.SetColorLevel(1)
    image_viewer.SetPosition(0, 100)

    iren = vtkRenderWindowInteractor()
    image_viewer.SetupInteractor(iren)
    image_viewer.GetRenderWindow().SetWindowName('HDRReader')
    image_viewer.Render()

    iren.AddObserver('EndInteractionEvent', ColorCallback(image_viewer))

    iren.Start()


class ColorCallback(object):
    def __init__(self, image_viewer):
        self.image_viewer = image_viewer

    def __call__(self, caller, ev):
        # Just do this to demonstrate who called callback and the event that triggered it.
        # print(caller.GetClassName(), 'Event Id:', ev)

        res = 'Color window: {} level: {}'.format(self.image_viewer.GetColorWindow(),
                                                  self.image_viewer.GetColorLevel())
        print(res)


if __name__ == '__main__':
    main()
