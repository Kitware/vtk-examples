#!/usr/bin/env python

from pathlib import Path
from pathlib import PurePath

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonComputationalGeometry import (
    vtkParametricBoy,
    vtkParametricMobius,
    vtkParametricRandomHills,
    vtkParametricTorus
)
from vtkmodules.vtkCommonCore import (
    VTK_VERSION_NUMBER,
    vtkCommand,
    vtkFloatArray,
    vtkVersion
)
from vtkmodules.vtkCommonTransforms import vtkTransform
from vtkmodules.vtkFiltersCore import (
    vtkPolyDataTangents,
    vtkTriangleFilter
)
from vtkmodules.vtkFiltersGeneral import vtkTransformPolyDataFilter
from vtkmodules.vtkFiltersModeling import vtkLinearSubdivisionFilter
from vtkmodules.vtkFiltersSources import (
    vtkCubeSource,
    vtkParametricFunctionSource,
    vtkTexturedSphereSource
)
from vtkmodules.vtkIOImage import (
    vtkHDRReader,
    vtkImageReader2Factory
)
from vtkmodules.vtkImagingCore import vtkImageFlip
from vtkmodules.vtkInteractionWidgets import (
    vtkCameraOrientationWidget,
    vtkOrientationMarkerWidget,
    vtkSliderRepresentation2D,
    vtkSliderWidget
)
from vtkmodules.vtkRenderingAnnotation import vtkAxesActor
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkPolyDataMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkSkybox,
    vtkTexture
)
from vtkmodules.vtkRenderingOpenGL2 import (
    vtkCameraPass,
    vtkEquirectangularToCubeMapTexture,
    vtkLightsPass,
    vtkOpaquePass,
    vtkOpenGLRenderer,
    vtkOverlayPass,
    vtkRenderPassCollection,
    vtkSequencePass,
    vtkToneMappingPass
)


def get_program_parameters():
    import argparse
    description = 'Demonstrates physically based rendering, image based lighting, anisotropic texturing and a skybox.'
    epilogue = '''
Physically based rendering sets color, metallicity and roughness of the object.
Image based lighting uses a cubemap texture to specify the environment.
Texturing is used to generate lighting effects.
A Skybox is used to create the illusion of distant three-dimensional surroundings.

The parameter order is:
 skybox, base_color, normal_texture, material_texture, anisotropic_texture, surface
    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('path', help='The path to the cubemap files e.g. Skyboxes/skybox2/ or to a\n'
                                     ' .hdr, .png, or .jpg equirectangular file')
    parser.add_argument('base_fn',
                        help='The path to the base colour (albedo) texture file'
                             ' e.g. Textures/Anisotropic/CarbonFiberAniso_albedo.png')
    parser.add_argument('normal_fn', help='The path to the normal texture file'
                                          ' e.g. Textures/Anisotropic/CarbonFiberAniso_normal.png')
    parser.add_argument('material_fn',
                        help='The path to the material (orm) texture file'
                             ' e.g. Textures/Anisotropic/CarbonFiberAniso_orm.png')
    parser.add_argument('anisotropy_fn',
                        help='The path to the anisotropy texture file'
                             ' e.g. Textures/Anisotropic/CarbonFiberAniso_anisotropyAngle')
    parser.add_argument('surface', nargs='?', default='Boy', help="The surface to use. Boy's surface is the default.")
    args = parser.parse_args()
    return args.path, args.base_fn, args.normal_fn, args.material_fn, args.anisotropy_fn, args.surface


def main():
    if not vtk_version_ok(9, 0, 0):
        print('You need VTK version 9.0 or greater to run this program.')
        return
    path, base_fn, normal_fn, material_fn, anisotropy_fn, surface = get_program_parameters()

    # A dictionary of the skybox folder name and the skybox files in it.
    skybox_files = {
        'skybox0':
            ['right.jpg', 'left.jpg', 'top.jpg', 'bottom.jpg', 'front.jpg',
             'back.jpg'],
        'skybox1':
            ['skybox-px.jpg', 'skybox-nx.jpg', 'skybox-py.jpg', 'skybox-ny.jpg',
             'skybox-pz.jpg', 'skybox-nz.jpg'],
        'skybox2':
            ['posx.jpg', 'negx.jpg', 'posy.jpg', 'negy.jpg', 'posz.jpg',
             'negz.jpg']}

    # Load the skybox or cube map.
    if Path(path).is_dir():
        skybox = read_cubemap(Path(path), skybox_files[PurePath(Path(path)).name])
    elif Path(path).is_file():
        skybox = read_environment_map(Path(path))
    else:
        print('Unable to read:', path)
        return

    if skybox is None:
        return

    # Get the textures
    base_color = read_texture(base_fn)
    base_color.SetColorModeToDirectScalars()
    base_color.UseSRGBColorSpaceOn()
    normal = read_texture(normal_fn)
    normal.SetColorModeToDirectScalars()
    material = read_texture(material_fn)
    material.SetColorModeToDirectScalars()
    anisotropy = read_texture(anisotropy_fn)
    material.SetColorModeToDirectScalars()

    # Get the surface
    surface = surface.lower()
    available_surfaces = {'boy', 'mobius', 'randomhills', 'torus', 'sphere', 'cube'}
    if surface not in available_surfaces:
        surface = 'boy'
    if surface == 'mobius':
        source = get_mobius()
    elif surface == 'randomhills':
        source = get_random_hills()
    elif surface == 'torus':
        source = get_torus()
    elif surface == 'sphere':
        source = get_sphere()
    elif surface == 'cube':
        source = get_cube()
    else:
        source = get_boy()

    colors = vtkNamedColors()

    # Set the background color.
    colors.SetColor('BkgColor', [26, 51, 102, 255])
    colors.SetColor('VTKBlue', [6, 79, 141, 255])
    # Let's make a complementary colour to VTKBlue
    colors.SetColor('VTKBlueComp', [249, 176, 114, 255])

    renderer = vtkOpenGLRenderer()
    render_window = vtkRenderWindow()
    render_window.AddRenderer(renderer)
    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(render_window)

    # Turn off the default lighting and use image based lighting.
    renderer.AutomaticLightCreationOff()
    renderer.UseImageBasedLightingOn()
    renderer.SetEnvironmentTexture(skybox)
    renderer.UseSphericalHarmonicsOff()
    renderer.SetBackground(colors.GetColor3d('BkgColor'))

    # Set up tone mapping so we can vary the exposure.
    #
    # Custom Passes.
    camera_p = vtkCameraPass()
    seq = vtkSequencePass()
    opaque = vtkOpaquePass()
    lights = vtkLightsPass()
    overlay = vtkOverlayPass()

    passes = vtkRenderPassCollection()
    passes.AddItem(lights)
    passes.AddItem(opaque)
    passes.AddItem(overlay)
    seq.SetPasses(passes)
    camera_p.SetDelegatePass(seq)

    tone_mapping_p = vtkToneMappingPass()
    tone_mapping_p.SetToneMappingType(vtkToneMappingPass().GenericFilmic)
    tone_mapping_p.SetGenericFilmicUncharted2Presets()
    tone_mapping_p.SetExposure(1.0)
    tone_mapping_p.SetUseACES(True)
    tone_mapping_p.SetDelegatePass(camera_p)

    renderer.SetPass(tone_mapping_p)
    slw_p = SliderProperties()
    slw_p.initial_value = 1.0
    slw_p.maximum_value = 5.0
    slw_p.title = 'Exposure'

    slider_widget_exposure = make_slider_widget(slw_p)
    slider_widget_exposure.SetInteractor(interactor)
    slider_widget_exposure.SetAnimationModeToAnimate()
    slider_widget_exposure.EnabledOn()

    # Lets use a rough metallic surface.
    diffuse_coefficient = 1.0
    roughness_coefficient = 1.0
    metallic_coefficient = 1.0
    # Other parameters.
    occlusion_strength = 1.0
    normal_scale = 1.0
    anisotropy_coefficient = 1.0
    anisotropy_rotation = 0.0

    # Build the pipeline
    mapper = vtkPolyDataMapper()
    mapper.SetInputData(source)

    actor = vtkActor()
    actor.SetMapper(mapper)
    # Enable PBR on the model.
    actor.GetProperty().SetInterpolationToPBR()
    # Configure the basic properties.
    # Set the model colour.
    actor.GetProperty().SetColor(colors.GetColor3d('White'))
    actor.GetProperty().SetDiffuse(diffuse_coefficient)
    actor.GetProperty().SetRoughness(roughness_coefficient)
    actor.GetProperty().SetMetallic(metallic_coefficient)
    # Configure textures (needs tcoords on the mesh).
    actor.GetProperty().SetBaseColorTexture(base_color)
    actor.GetProperty().SetORMTexture(material)
    actor.GetProperty().SetOcclusionStrength(occlusion_strength)
    # Needs tcoords, normals and tangents on the mesh.
    actor.GetProperty().SetNormalTexture(normal)
    actor.GetProperty().SetNormalScale(normal_scale)
    actor.GetProperty().SetAnisotropyTexture(anisotropy)
    actor.GetProperty().SetAnisotropy(anisotropy_coefficient)
    actor.GetProperty().SetAnisotropyRotation(anisotropy_rotation)

    skybox_actor = vtkSkybox()
    skybox_actor.SetTexture(skybox)
    skybox_actor.GammaCorrectOn()

    renderer.AddActor(actor)
    # Comment out if you don't want a skybox.
    renderer.AddActor(skybox_actor)

    render_window.SetSize(800, 500)
    render_window.Render()
    render_window.SetWindowName('PBR_Skybox_Anisotropy')

    axes = vtkAxesActor()

    widget = vtkOrientationMarkerWidget()
    rgba = [0.0, 0.0, 0.0, 0.0]
    colors.GetColor('Carrot', rgba)
    widget.SetOutlineColor(rgba[0], rgba[1], rgba[2])
    widget.SetOrientationMarker(axes)
    widget.SetInteractor(interactor)
    widget.SetViewport(0.0, 0.0, 0.2, 0.2)
    widget.EnabledOn()
    widget.InteractiveOn()

    interactor.SetRenderWindow(render_window)

    if vtk_version_ok(9, 0, 20210718):
        try:
            cam_orient_manipulator = vtkCameraOrientationWidget()
            cam_orient_manipulator.SetParentRenderer(renderer)
            # Enable the widget.
            cam_orient_manipulator.On()
        except AttributeError:
            pass

    # Create the slider callback to manipulate exposure.
    slider_widget_exposure.AddObserver(vtkCommand.InteractionEvent, SliderCallbackExposure(tone_mapping_p))

    render_window.Render()
    interactor.Start()


def vtk_version_ok(major, minor, build):
    """
    Check the VTK version.

    :param major: Major version.
    :param minor: Minor version.
    :param build: Build version.
    :return: True if the requested VTK version is greater or equal to the actual VTK version.
    """
    needed_version = 10000000000 * int(major) + 100000000 * int(minor) + int(build)
    try:
        vtk_version_number = VTK_VERSION_NUMBER
    except AttributeError:  # as error:
        ver = vtkVersion()
        vtk_version_number = 10000000000 * ver.GetVTKMajorVersion() + 100000000 * ver.GetVTKMinorVersion() \
                             + ver.GetVTKBuildVersion()
    if vtk_version_number >= needed_version:
        return True
    else:
        return False


def read_cubemap(folder_root, file_names):
    """
    Read six images forming a cubemap.

    :param folder_root: The folder where the cube maps are stored.
    :param file_names: The names of the cubemap files.
    :return: The cubemap texture.
    """
    texture = vtkTexture()
    texture.CubeMapOn()
    # Build the file names.
    fns = list()
    for fn in file_names:
        fns.append(folder_root.joinpath(fn))
        if not fns[-1].is_file():
            print('Nonexistent texture file:', fns[-1])
            return None
    i = 0
    for fn in fns:
        # Read the images.
        reader_factory = vtkImageReader2Factory()
        img_reader = reader_factory.CreateImageReader2(str(fn))
        img_reader.SetFileName(str(fn))

        flip = vtkImageFlip()
        flip.SetInputConnection(img_reader.GetOutputPort())
        flip.SetFilteredAxis(1)  # flip y axis
        texture.SetInputConnection(i, flip.GetOutputPort(0))
        i += 1
    texture.MipmapOn()
    texture.InterpolateOn()
    return texture


def read_environment_map(fn):
    """
    Read an equirectangular environment file and convert it to a cube map.

    :param fn: The equirectangular file.
    :return: The cubemap texture.
    """
    if not fn.is_file():
        print('Nonexistent texture file:', fn)
        return None
    suffix = Path(fn).suffix
    if suffix in ['.jpg', '.png']:
        reader_factory = vtkImageReader2Factory()
        img_reader = reader_factory.CreateImageReader2(str(fn))
        img_reader.SetFileName(str(fn))

        texture = vtkTexture()
        texture.SetInputConnection(img_reader.GetOutputPort())
    else:
        reader = vtkHDRReader()
        extensions = reader.GetFileExtensions()
        # Check the image can be read.
        if not reader.CanReadFile(str(fn)):
            print('CanReadFile failed for ', fn)
            return None
        if suffix not in extensions:
            print('Unable to read this file extension: ', suffix)
            return None
        reader.SetFileName(str(fn))
        reader.Update()

        texture = vtkTexture()
        texture.SetColorModeToDirectScalars()
        texture.SetInputConnection(reader.GetOutputPort())

    # Convert to a cube map.
    tcm = vtkEquirectangularToCubeMapTexture()
    tcm.SetInputTexture(texture)
    # Enable mipmapping to handle HDR image.
    tcm.MipmapOn()
    tcm.InterpolateOn()

    return tcm


def read_texture(image_path):
    """
    Read an image and convert it to a texture
    :param image_path: The image path.
    :return: The texture.
    """
    # Read the image which will be the texture
    path = Path(image_path)
    if not path.is_file():
        print('Nonexistent texture file:', path)
        return None
    extension = path.suffix.lower()
    valid_extensions = ['.jpg', '.png', '.bmp', '.tiff', '.pnm', '.pgm', '.ppm']
    if extension not in valid_extensions:
        print('Unable to read the texture file (wrong extension):', path)
        return None

    # Read the images
    reader_factory = vtkImageReader2Factory()
    img_reader = reader_factory.CreateImageReader2(str(path))
    img_reader.SetFileName(str(path))

    texture = vtkTexture()
    texture.InterpolateOn()
    texture.SetInputConnection(img_reader.GetOutputPort())
    texture.Update()

    return texture


def get_boy():
    u_resolution = 51
    v_resolution = 51
    surface = vtkParametricBoy()

    source = vtkParametricFunctionSource()
    source.SetUResolution(u_resolution)
    source.SetVResolution(v_resolution)
    source.GenerateTextureCoordinatesOn()
    source.SetParametricFunction(surface)
    source.Update()

    # Build the tangents
    tangents = vtkPolyDataTangents()
    tangents.SetInputConnection(source.GetOutputPort())
    tangents.Update()
    return tangents.GetOutput()


def get_mobius():
    u_resolution = 51
    v_resolution = 51
    surface = vtkParametricMobius()
    surface.SetMinimumV(-0.25)
    surface.SetMaximumV(0.25)

    source = vtkParametricFunctionSource()
    source.SetUResolution(u_resolution)
    source.SetVResolution(v_resolution)
    source.GenerateTextureCoordinatesOn()
    source.SetParametricFunction(surface)
    source.Update()

    # Build the tangents
    tangents = vtkPolyDataTangents()
    tangents.SetInputConnection(source.GetOutputPort())
    tangents.Update()

    transform = vtkTransform()
    transform.RotateX(-90.0)
    transform_filter = vtkTransformPolyDataFilter()
    transform_filter.SetInputConnection(tangents.GetOutputPort())
    transform_filter.SetTransform(transform)
    transform_filter.Update()

    return transform_filter.GetOutput()


def get_random_hills():
    u_resolution = 51
    v_resolution = 51
    surface = vtkParametricRandomHills()
    surface.SetRandomSeed(1)
    surface.SetNumberOfHills(30)
    # If you want a plane
    # surface.SetHillAmplitude(0)

    source = vtkParametricFunctionSource()
    source.SetUResolution(u_resolution)
    source.SetVResolution(v_resolution)
    source.GenerateTextureCoordinatesOn()
    source.SetParametricFunction(surface)
    source.Update()

    # Build the tangents
    tangents = vtkPolyDataTangents()
    tangents.SetInputConnection(source.GetOutputPort())
    tangents.Update()

    transform = vtkTransform()
    transform.Translate(0.0, 5.0, 15.0)
    transform.RotateX(-90.0)
    transform_filter = vtkTransformPolyDataFilter()
    transform_filter.SetInputConnection(tangents.GetOutputPort())
    transform_filter.SetTransform(transform)
    transform_filter.Update()

    return transform_filter.GetOutput()


def get_torus():
    u_resolution = 51
    v_resolution = 51
    surface = vtkParametricTorus()

    source = vtkParametricFunctionSource()
    source.SetUResolution(u_resolution)
    source.SetVResolution(v_resolution)
    source.GenerateTextureCoordinatesOn()
    source.SetParametricFunction(surface)
    source.Update()

    # Build the tangents
    tangents = vtkPolyDataTangents()
    tangents.SetInputConnection(source.GetOutputPort())
    tangents.Update()

    transform = vtkTransform()
    transform.RotateX(-90.0)
    transform_filter = vtkTransformPolyDataFilter()
    transform_filter.SetInputConnection(tangents.GetOutputPort())
    transform_filter.SetTransform(transform)
    transform_filter.Update()

    return transform_filter.GetOutput()


def get_sphere():
    theta_resolution = 32
    phi_resolution = 32
    surface = vtkTexturedSphereSource()
    surface.SetThetaResolution(theta_resolution)
    surface.SetPhiResolution(phi_resolution)

    # Now the tangents
    tangents = vtkPolyDataTangents()
    tangents.SetInputConnection(surface.GetOutputPort())
    tangents.Update()
    return tangents.GetOutput()


def get_cube():
    surface = vtkCubeSource()

    # Triangulate
    triangulation = vtkTriangleFilter()
    triangulation.SetInputConnection(surface.GetOutputPort())
    # Subdivide the triangles
    subdivide = vtkLinearSubdivisionFilter()
    subdivide.SetInputConnection(triangulation.GetOutputPort())
    subdivide.SetNumberOfSubdivisions(3)
    # Now the tangents
    tangents = vtkPolyDataTangents()
    tangents.SetInputConnection(subdivide.GetOutputPort())
    tangents.Update()
    return tangents.GetOutput()


def uv_tcoords(u_resolution, v_resolution, pd):
    """
    Generate u, v texture coordinates on a parametric surface.
    :param u_resolution: u resolution
    :param v_resolution: v resolution
    :param pd: The polydata representing the surface.
    :return: The polydata with the texture coordinates added.
    """
    u0 = 1.0
    v0 = 0.0
    du = 1.0 / (u_resolution - 1)
    dv = 1.0 / (v_resolution - 1)
    num_pts = pd.GetNumberOfPoints()
    t_coords = vtkFloatArray()
    t_coords.SetNumberOfComponents(2)
    t_coords.SetNumberOfTuples(num_pts)
    t_coords.SetName('Texture Coordinates')
    pt_id = 0
    u = u0
    for i in range(0, u_resolution):
        v = v0
        for j in range(0, v_resolution):
            tc = [u, v]
            t_coords.SetTuple(pt_id, tc)
            v += dv
            pt_id += 1
        u -= du
    pd.GetPointData().SetTCoords(t_coords)
    return pd


class SliderProperties:
    tube_width = 0.008
    slider_length = 0.008
    title_height = 0.025
    label_height = 0.025

    minimum_value = 0.0
    maximum_value = 1.0
    initial_value = 1.0

    p1 = [0.2, 0.1]
    p2 = [0.8, 0.1]

    title = None

    title_color = 'MistyRose'
    value_color = 'Cyan'
    slider_color = 'Coral'
    selected_color = 'Lime'
    bar_color = 'PeachPuff'
    bar_ends_color = 'Thistle'


def make_slider_widget(properties):
    colors = vtkNamedColors()

    slider = vtkSliderRepresentation2D()

    slider.SetMinimumValue(properties.minimum_value)
    slider.SetMaximumValue(properties.maximum_value)
    slider.SetValue(properties.initial_value)
    slider.SetTitleText(properties.title)

    slider.GetPoint1Coordinate().SetCoordinateSystemToNormalizedDisplay()
    slider.GetPoint1Coordinate().SetValue(properties.p1[0], properties.p1[1])
    slider.GetPoint2Coordinate().SetCoordinateSystemToNormalizedDisplay()
    slider.GetPoint2Coordinate().SetValue(properties.p2[0], properties.p2[1])

    slider.SetTubeWidth(properties.tube_width)
    slider.SetSliderLength(properties.slider_length)
    slider.SetTitleHeight(properties.title_height)
    slider.SetLabelHeight(properties.label_height)

    # Set the color properties
    # Change the color of the bar.
    slider.GetTubeProperty().SetColor(colors.GetColor3d(properties.bar_color))
    # Change the color of the ends of the bar.
    slider.GetCapProperty().SetColor(colors.GetColor3d(properties.bar_ends_color))
    # Change the color of the knob that slides.
    slider.GetSliderProperty().SetColor(colors.GetColor3d(properties.slider_color))
    # Change the color of the knob when the mouse is held on it.
    slider.GetSelectedProperty().SetColor(colors.GetColor3d(properties.selected_color))
    # Change the color of the text displaying the value.
    slider.GetLabelProperty().SetColor(colors.GetColor3d(properties.value_color))

    slider_widget = vtkSliderWidget()
    slider_widget.SetRepresentation(slider)

    return slider_widget


class SliderCallbackExposure:
    def __init__(self, tone_mapping_property):
        self.tone_mapping_property = tone_mapping_property

    def __call__(self, caller, ev):
        slider_widget = caller
        value = slider_widget.GetRepresentation().GetValue()
        self.tone_mapping_property.SetExposure(value)


if __name__ == '__main__':
    main()
