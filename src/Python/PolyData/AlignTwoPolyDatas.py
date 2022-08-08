#!/usr/bin/env python
# -*- coding: utf-8 -*-

import math
from pathlib import Path

# noinspection PyUnresolvedReferences
import vtkmodules.vtkInteractionStyle
# noinspection PyUnresolvedReferences
import vtkmodules.vtkRenderingOpenGL2
from vtkmodules.vtkCommonColor import vtkNamedColors
from vtkmodules.vtkCommonCore import (
    VTK_DOUBLE_MAX,
    vtkPoints
)
from vtkmodules.vtkCommonCore import (
    VTK_VERSION_NUMBER,
    vtkVersion
)
from vtkmodules.vtkCommonDataModel import (
    vtkIterativeClosestPointTransform,
    vtkPolyData
)
from vtkmodules.vtkCommonTransforms import (
    vtkLandmarkTransform,
    vtkTransform
)
from vtkmodules.vtkFiltersGeneral import (
    vtkOBBTree,
    vtkTransformPolyDataFilter
)
from vtkmodules.vtkFiltersModeling import vtkHausdorffDistancePointSetFilter
from vtkmodules.vtkIOGeometry import (
    vtkBYUReader,
    vtkOBJReader,
    vtkSTLReader
)
from vtkmodules.vtkIOLegacy import (
    vtkPolyDataReader,
    vtkPolyDataWriter
    )
from vtkmodules.vtkIOPLY import vtkPLYReader
from vtkmodules.vtkIOXML import vtkXMLPolyDataReader
from vtkmodules.vtkInteractionWidgets import (
    vtkCameraOrientationWidget,
    vtkOrientationMarkerWidget
)
from vtkmodules.vtkRenderingAnnotation import vtkAxesActor
from vtkmodules.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer
)


def get_program_parameters():
    import argparse
    description = 'How to align two vtkPolyData\'s.'
    epilogue = '''

    '''
    parser = argparse.ArgumentParser(description=description, epilog=epilogue,
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('src_fn', help='The polydata source file name,e.g. thingiverse/Grey_Nurse_Shark.stl.')
    parser.add_argument('tgt_fn', help='The polydata target file name, e.g. greatWhite.stl.')

    args = parser.parse_args()

    return args.src_fn, args.tgt_fn


def main():
    colors = vtkNamedColors()

    src_fn, tgt_fn = get_program_parameters()
    print('Loading source:', src_fn)
    source_polydata = read_poly_data(src_fn)
    # Save the source polydata in case the alignment process does not improve
    # segmentation.
    original_source_polydata = vtkPolyData()
    original_source_polydata.DeepCopy(source_polydata)

    print('Loading target:', tgt_fn)
    target_polydata = read_poly_data(tgt_fn)

    # If the target orientation is markedly different, you may need to apply a
    # transform to orient the target with the source.
    # For example, when using Grey_Nurse_Shark.stl as the source and
    # greatWhite.stl as the target, you need to transform the target.
    trnf = vtkTransform()
    if Path(src_fn).name == 'Grey_Nurse_Shark.stl' and Path(tgt_fn).name == 'greatWhite.stl':
        trnf.RotateY(90)

    tpd = vtkTransformPolyDataFilter()
    tpd.SetTransform(trnf)
    tpd.SetInputData(target_polydata)
    tpd.Update()

    renderer = vtkRenderer()
    render_window = vtkRenderWindow()
    render_window.AddRenderer(renderer)
    interactor = vtkRenderWindowInteractor()
    interactor.SetRenderWindow(render_window)

    distance = vtkHausdorffDistancePointSetFilter()
    distance.SetInputData(0, tpd.GetOutput())
    distance.SetInputData(1, source_polydata)
    distance.Update()

    distance_before_align = distance.GetOutput(0).GetFieldData().GetArray('HausdorffDistance').GetComponent(0, 0)

    # Get initial alignment using oriented bounding boxes.
    align_bounding_boxes(source_polydata, tpd.GetOutput())

    distance.SetInputData(0, tpd.GetOutput())
    distance.SetInputData(1, source_polydata)
    distance.Modified()
    distance.Update()
    distance_after_align = distance.GetOutput(0).GetFieldData().GetArray('HausdorffDistance').GetComponent(0, 0)

    best_distance = min(distance_before_align, distance_after_align)

    if distance_after_align > distance_before_align:
        source_polydata.DeepCopy(original_source_polydata)

    # Refine the alignment using IterativeClosestPoint.
    icp = vtkIterativeClosestPointTransform()
    icp.SetSource(source_polydata)
    icp.SetTarget(tpd.GetOutput())
    icp.GetLandmarkTransform().SetModeToRigidBody()
    icp.SetMaximumNumberOfLandmarks(100)
    icp.SetMaximumMeanDistance(.00001)
    icp.SetMaximumNumberOfIterations(500)
    icp.CheckMeanDistanceOn()
    icp.StartByMatchingCentroidsOn()
    icp.Update()
    icp_mean_distance = icp.GetMeanDistance()

    # print(icp)

    lm_transform = icp.GetLandmarkTransform()
    transform = vtkTransformPolyDataFilter()
    transform.SetInputData(source_polydata)
    transform.SetTransform(lm_transform)
    transform.SetTransform(icp)
    transform.Update()

    distance.SetInputData(0, tpd.GetOutput())
    distance.SetInputData(1, transform.GetOutput())
    distance.Update()

    # Note: If there is an error extracting eigenfunctions, then this will be zero.
    distance_after_icp = distance.GetOutput(0).GetFieldData().GetArray('HausdorffDistance').GetComponent(0, 0)

    # Check if ICP worked.
    if not (math.isnan(icp_mean_distance) or math.isinf(icp_mean_distance)):
        if distance_after_icp < best_distance:
            best_distance = distance_after_icp

    print('Distances:')
    print('  Before aligning:                        {:0.5f}'.format(distance_before_align))
    print('  Aligning using oriented bounding boxes: {:0.5f}'.format(distance_before_align))
    print('  Aligning using IterativeClosestPoint:   {:0.5f}'.format(distance_after_icp))
    print('  Best distance:                          {:0.5f}'.format(best_distance))

    # Select the source to use.
    source_mapper = vtkDataSetMapper()
    if best_distance == distance_before_align:
        source_mapper.SetInputData(original_source_polydata)
        print('Using original alignment')
    elif best_distance == distance_after_align:
        source_mapper.SetInputData(source_polydata)
        print('Using alignment by OBB')
    else:
        source_mapper.SetInputConnection(transform.GetOutputPort())
        print('Using alignment by ICP')
    source_mapper.ScalarVisibilityOff()


    writer = vtkPolyDataWriter()
    if best_distance == distance_before_align:
        writer.SetInputData(original_source_polydata)
    elif best_distance == distance_after_align:
        writer.SetInputData(source_polydata)
    else:
        writer.SetInputData(transform.GetOutput())
    writer.SetFileName('AlignedSource.vtk')
    writer.Write()
    writer.SetInputData(tpd.GetOutput())
    writer.SetFileName('Target.vtk')
    writer.Write()

    source_actor = vtkActor()
    source_actor.SetMapper(source_mapper)
    source_actor.GetProperty().SetOpacity(0.6)
    source_actor.GetProperty().SetDiffuseColor(
        colors.GetColor3d('White'))
    renderer.AddActor(source_actor)

    target_mapper = vtkDataSetMapper()
    target_mapper.SetInputData(tpd.GetOutput())
    target_mapper.ScalarVisibilityOff()

    target_actor = vtkActor()
    target_actor.SetMapper(target_mapper)
    target_actor.GetProperty().SetDiffuseColor(
        colors.GetColor3d('Tomato'))
    renderer.AddActor(target_actor)

    render_window.AddRenderer(renderer)
    renderer.SetBackground(colors.GetColor3d("sea_green_light"))
    renderer.UseHiddenLineRemovalOn()

    if vtk_version_ok(9, 0, 20210718):
        try:
            cam_orient_manipulator = vtkCameraOrientationWidget()
            cam_orient_manipulator.SetParentRenderer(renderer)
            # Enable the widget.
            cam_orient_manipulator.On()
        except AttributeError:
            pass
    else:
        axes = vtkAxesActor()
        widget = vtkOrientationMarkerWidget()
        rgba = [0.0, 0.0, 0.0, 0.0]
        colors.GetColor("Carrot", rgba)
        widget.SetOutlineColor(rgba[0], rgba[1], rgba[2])
        widget.SetOrientationMarker(axes)
        widget.SetInteractor(interactor)
        widget.SetViewport(0.0, 0.0, 0.2, 0.2)
        widget.EnabledOn()
        widget.InteractiveOn()

    render_window.SetSize(640, 480)
    render_window.Render()
    render_window.SetWindowName('AlignTwoPolyDatas')

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


def read_poly_data(file_name):
    import os
    path, extension = os.path.splitext(file_name)
    extension = extension.lower()
    if extension == ".ply":
        reader = vtkPLYReader()
        reader.SetFileName(file_name)
        reader.Update()
        poly_data = reader.GetOutput()
    elif extension == ".vtp":
        reader = vtkXMLPolyDataReader()
        reader.SetFileName(file_name)
        reader.Update()
        poly_data = reader.GetOutput()
    elif extension == ".obj":
        reader = vtkOBJReader()
        reader.SetFileName(file_name)
        reader.Update()
        poly_data = reader.GetOutput()
    elif extension == ".stl":
        reader = vtkSTLReader()
        reader.SetFileName(file_name)
        reader.Update()
        poly_data = reader.GetOutput()
    elif extension == ".vtk":
        reader = vtkPolyDataReader()
        reader.SetFileName(file_name)
        reader.Update()
        poly_data = reader.GetOutput()
    elif extension == ".g":
        reader = vtkBYUReader()
        reader.SetGeometryFileName(file_name)
        reader.Update()
        poly_data = reader.GetOutput()
    else:
        # Return a None if the extension is unknown.
        poly_data = None
    return poly_data


def align_bounding_boxes(source, target):
    # Use OBBTree to create an oriented bounding box for target and source
    source_obb_tree = vtkOBBTree()
    source_obb_tree.SetDataSet(source)
    source_obb_tree.SetMaxLevel(1)
    source_obb_tree.BuildLocator()

    target_obb_tree = vtkOBBTree()
    target_obb_tree.SetDataSet(target)
    target_obb_tree.SetMaxLevel(1)
    target_obb_tree.BuildLocator()

    source_landmarks = vtkPolyData()
    source_obb_tree.GenerateRepresentation(0, source_landmarks)

    target_landmarks = vtkPolyData()
    target_obb_tree.GenerateRepresentation(0, target_landmarks)

    lm_transform = vtkLandmarkTransform()
    lm_transform.SetModeToSimilarity()
    lm_transform.SetTargetLandmarks(target_landmarks.GetPoints())
    best_distance = VTK_DOUBLE_MAX
    best_points = vtkPoints()
    best_distance = best_bounding_box(
        "X",
        target,
        source,
        target_landmarks,
        source_landmarks,
        best_distance,
        best_points)
    best_distance = best_bounding_box(
        "Y",
        target,
        source,
        target_landmarks,
        source_landmarks,
        best_distance,
        best_points)
    best_distance = best_bounding_box(
        "Z",
        target,
        source,
        target_landmarks,
        source_landmarks,
        best_distance,
        best_points)

    lm_transform.SetSourceLandmarks(best_points)
    lm_transform.Modified()

    lm_transform_pd = vtkTransformPolyDataFilter()
    lm_transform_pd.SetInputData(source)
    lm_transform_pd.SetTransform(lm_transform)
    lm_transform_pd.Update()

    source.DeepCopy(lm_transform_pd.GetOutput())

    return


def best_bounding_box(axis, target, source, target_landmarks, source_landmarks, best_distance, best_points):
    distance = vtkHausdorffDistancePointSetFilter()
    test_transform = vtkTransform()
    test_transform_pd = vtkTransformPolyDataFilter()
    lm_transform = vtkLandmarkTransform()
    lm_transform_pd = vtkTransformPolyDataFilter()

    lm_transform.SetModeToSimilarity()
    lm_transform.SetTargetLandmarks(target_landmarks.GetPoints())

    source_center = source_landmarks.GetCenter()

    delta = 90.0
    for i in range(0, 4):
        angle = delta * i
        # Rotate about center
        test_transform.Identity()
        test_transform.Translate(source_center[0], source_center[1], source_center[2])
        if axis == "X":
            test_transform.RotateX(angle)
        elif axis == "Y":
            test_transform.RotateY(angle)
        else:
            test_transform.RotateZ(angle)
        test_transform.Translate(-source_center[0], -source_center[1], -source_center[2])

        test_transform_pd.SetTransform(test_transform)
        test_transform_pd.SetInputData(source_landmarks)
        test_transform_pd.Update()

        lm_transform.SetSourceLandmarks(test_transform_pd.GetOutput().GetPoints())
        lm_transform.Modified()

        lm_transform_pd.SetInputData(source)
        lm_transform_pd.SetTransform(lm_transform)
        lm_transform_pd.Update()

        distance.SetInputData(0, target)
        distance.SetInputData(1, lm_transform_pd.GetOutput())
        distance.Update()

        test_distance = distance.GetOutput(0).GetFieldData().GetArray("HausdorffDistance").GetComponent(0, 0)
        if test_distance < best_distance:
            best_distance = test_distance
            best_points.DeepCopy(test_transform_pd.GetOutput().GetPoints())

    return best_distance


if __name__ == '__main__':
    main()
