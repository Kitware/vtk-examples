#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkConeSource.h>
#include <vtkGlyph3D.h>
#include <vtkMaskPoints.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataReader.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

// This example demonstrates the use of glyphing. We also use a mask filter
// to select a subset of points to glyph.

// Read a data file. This originally was a Cyberware laser digitizer scan
// of Fran J.'s face. Surface normals are generated based on local geometry
// (i.e., the polygon normals surrounding each point are averaged). We flip
// the normals because we want them to point out from Fran's face.
//
int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " fran_cut.vtk" << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkPolyDataReader> fran;
  fran->SetFileName(argv[1]);

  vtkNew<vtkPolyDataNormals> normals;
  normals->SetInputConnection(fran->GetOutputPort());
  normals->FlipNormalsOn();

  vtkNew<vtkPolyDataMapper> franMapper;
  franMapper->SetInputConnection(normals->GetOutputPort());

  vtkNew<vtkActor> franActor;
  franActor->SetMapper(franMapper);
  franActor->GetProperty()->SetColor(colors->GetColor3d("Flesh").GetData());

  // We subsample the dataset because we want to glyph just a subset of
  // the points. Otherwise the display is cluttered and cannot be easily
  // read. The RandomModeOn and SetOnRatio combine to random select one out
  // of every 10 points in the dataset.
  //
  vtkNew<vtkMaskPoints> ptMask;
  ptMask->SetInputConnection(normals->GetOutputPort());
  ptMask->SetOnRatio(10);
  ptMask->RandomModeOn();

  // In this case we are using a cone as a glyph. We transform the cone so
  // its base is at 0,0,0. This is the point where glyph rotation occurs.
  vtkNew<vtkConeSource> cone;
  cone->SetResolution(6);

  vtkNew<vtkTransform> transform;
  transform->Translate(0.5, 0.0, 0.0);

  vtkNew<vtkTransformPolyDataFilter> transformF;
  transformF->SetInputConnection(cone->GetOutputPort());
  transformF->SetTransform(transform);

  // vtkGlyph3D takes two inputs: the input point set (SetInputConnection)
  // which can be any vtkDataSet; and the glyph (SetSourceConnection) which
  // must be a vtkPolyData.  We are interested in orienting the glyphs by the
  // surface normals that we previously generated.
  vtkNew<vtkGlyph3D> glyph;
  glyph->SetInputConnection(ptMask->GetOutputPort());
  glyph->SetSourceConnection(transformF->GetOutputPort());
  glyph->SetVectorModeToUseNormal();
  glyph->SetScaleModeToScaleByVector();
  glyph->SetScaleFactor(0.004);

  vtkNew<vtkPolyDataMapper> spikeMapper;
  spikeMapper->SetInputConnection(glyph->GetOutputPort());

  vtkNew<vtkActor> spikeActor;
  spikeActor->SetMapper(spikeMapper);
  spikeActor->GetProperty()->SetColor(
      colors->GetColor3d("Emerald_Green").GetData());

  // Create the RenderWindow, Renderer and both Actors
  //
  vtkNew<vtkRenderer> ren1;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren1);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Add the actors to the renderer, set the background and size
  //
  ren1->AddActor(franActor);
  ren1->AddActor(spikeActor);

  renWin->SetSize(640, 480);
  renWin->SetWindowName("SpikeFran");

  ren1->SetBackground(colors->GetColor3d("SlateGray").GetData());

  // render the image
  //
  renWin->Render();

  ren1->GetActiveCamera()->Zoom(1.4);
  ren1->GetActiveCamera()->Azimuth(110);
  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
