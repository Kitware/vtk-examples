#include <vtkActor.h>
#include <vtkGeoGraticule.h>
#include <vtkGeoProjection.h>
#include <vtkGeoTransform.h>
#include <vtkNew.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransformFilter.h>
#include <vtkXMLPolyDataReader.h>

int main( int argc, char* argv[] )
{
  vtkNew<vtkNamedColors> colors;

  int latLevel = 2;
  int lngLevel = 2;
  const char* pname = "rouss";

  vtkNew<vtkGeoGraticule> geoGraticle;
  vtkNew<vtkGeoTransform> transformProjection;
  vtkNew<vtkGeoProjection> destinationProjection;
  vtkNew<vtkGeoProjection> sourceProjection;
  vtkNew<vtkTransformFilter> transformGraticle;
  vtkNew<vtkXMLPolyDataReader> reader;
  vtkNew<vtkTransformFilter> transformReader;
  vtkNew<vtkPolyDataMapper> graticleMapper;
  vtkNew<vtkPolyDataMapper> readerMapper;
  vtkNew<vtkActor> graticleActor;
  vtkNew<vtkActor> readerActor;

  geoGraticle->SetGeometryType( vtkGeoGraticule::POLYLINES );
  geoGraticle->SetLatitudeLevel( latLevel );
  geoGraticle->SetLongitudeLevel( lngLevel );
  geoGraticle->SetLongitudeBounds( -180, 180 );
  geoGraticle->SetLatitudeBounds( -90, 90 );

  // destinationProjection defaults to latlong.
  destinationProjection->SetName( pname );
  destinationProjection->SetCentralMeridian( 0. );
  transformProjection->SetSourceProjection( sourceProjection );
  transformProjection->SetDestinationProjection( destinationProjection );
  transformGraticle->SetInputConnection( geoGraticle->GetOutputPort() );
  transformGraticle->SetTransform( transformProjection );
  graticleMapper->SetInputConnection( transformGraticle->GetOutputPort() );
  graticleActor->SetMapper( graticleMapper );

  reader->SetFileName(argv[1]);
  transformReader->SetTransform( transformProjection );
  transformReader->SetInputConnection( reader->GetOutputPort() );
  readerMapper->SetInputConnection( transformReader->GetOutputPort() );
  readerActor->SetMapper( readerMapper );

  vtkNew<vtkRenderWindow> renderWindow;
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindowInteractor> interactor;
  renderWindow->SetInteractor( interactor );
  renderWindow->AddRenderer( renderer );
  renderWindow->SetSize(640, 480);
  renderer->SetBackground(colors->GetColor3d("BurlyWood").GetData());

  renderer->AddActor( readerActor );
  renderer->AddActor( graticleActor );

  renderWindow->Render();

  interactor->Initialize();
  interactor->Start();

  return EXIT_SUCCESS;
}
