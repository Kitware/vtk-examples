#include <vtkActor.h>
#include <vtkBooleanTexture.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkImplicitTextureCoords.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkQuadric.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTexture.h>

namespace {

double positions[][3]{{-4, 4, 0},  {-2, 4, 0},  {0, 4, 0},  {2, 4, 0},
                      {-4, 2, 0},  {-2, 2, 0},  {0, 2, 0},  {2, 2, 0},
                      {-4, 0, 0},  {-2, 0, 0},  {0, 0, 0},  {2, 0, 0},
                      {-4, -2, 0}, {-2, -2, 0}, {0, -2, 0}, {2, -2, 0}};

unsigned char solid[]{255, 255};
unsigned char clear[]{255, 0};
unsigned char edge[]{0, 255};

vtkSmartPointer<vtkBooleanTexture> MakeBooleanTexture(int, int, int);
} // namespace

int main(int /* argc */, char* /* argv */[])
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderWindow> renWin;

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  vtkNew<vtkRenderer> aren;

  // define two elliptical cylinders
  vtkNew<vtkQuadric> quadric1;
  quadric1->SetCoefficients(1, 2, 0, 0, 0, 0, 0, 0, 0, -.07);

  vtkNew<vtkQuadric> quadric2;
  quadric2->SetCoefficients(2, 1, 0, 0, 0, 0, 0, 0, 0, -.07);

  // create a sphere for all to use
  vtkNew<vtkSphereSource> aSphere;
  aSphere->SetPhiResolution(21);
  aSphere->SetThetaResolution(21);

  // create texture coordinates for all
  vtkNew<vtkImplicitTextureCoords> tcoords;
  tcoords->SetInputConnection(aSphere->GetOutputPort());
  tcoords->SetRFunction(quadric1);
  tcoords->SetSFunction(quadric2);

  vtkNew<vtkDataSetMapper> aMapper;
  aMapper->SetInputConnection(tcoords->GetOutputPort());

  // create a mapper, sphere and texture map for each case
  for (int i = 0; i < 16; i++)
  {
    auto aBoolean = MakeBooleanTexture(i, 64, 0);

    vtkNew<vtkTexture> aTexture2;
    aTexture2->SetInputConnection(aBoolean->GetOutputPort());
    aTexture2->InterpolateOff();
    aTexture2->RepeatOff();

    vtkNew<vtkActor> anActor2;

    anActor2->SetMapper(aMapper);
    anActor2->SetTexture(aTexture2);
    anActor2->SetPosition(&positions[i][0]);
    anActor2->SetScale(2.0, 2.0, 2.0);
    anActor2->GetProperty()->SetColor(
        colors->GetColor3d("MistyRose").GetData());

    aren->AddActor(anActor2);
  }

  aren->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renWin->SetSize(500, 500);
  renWin->AddRenderer(aren);
  renWin->SetWindowName("TextureCutQuadric");

  // interact with data
  renWin->Render();

  iren->Start();

  return EXIT_SUCCESS;
}

namespace {

vtkSmartPointer<vtkBooleanTexture>
MakeBooleanTexture(int caseNumber, int resolution, int thickness)
{
  vtkNew<vtkBooleanTexture> booleanTexture;

  booleanTexture->SetXSize(resolution);
  booleanTexture->SetYSize(resolution);
  booleanTexture->SetThickness(thickness);

  switch (caseNumber)
  {
  case 0:
    booleanTexture->SetInIn(solid);
    booleanTexture->SetOutIn(solid);
    booleanTexture->SetInOut(solid);
    booleanTexture->SetOutOut(solid);
    booleanTexture->SetOnOn(solid);
    booleanTexture->SetOnIn(solid);
    booleanTexture->SetOnOut(solid);
    booleanTexture->SetInOn(solid);
    booleanTexture->SetOutOn(solid);
    break;
  case 1: /* cut inside 1 */
    booleanTexture->SetInIn(clear);
    booleanTexture->SetOutIn(solid);
    booleanTexture->SetInOut(solid);
    booleanTexture->SetOutOut(solid);
    booleanTexture->SetOnOn(edge);
    booleanTexture->SetOnIn(edge);
    booleanTexture->SetOnOut(solid);
    booleanTexture->SetInOn(edge);
    booleanTexture->SetOutOn(solid);
    break;
  case 2: /* cut outside 1 */
    booleanTexture->SetInIn(solid);
    booleanTexture->SetOutIn(clear);
    booleanTexture->SetInOut(solid);
    booleanTexture->SetOutOut(solid);
    booleanTexture->SetOnOn(edge);
    booleanTexture->SetOnIn(edge);
    booleanTexture->SetOnOut(solid);
    booleanTexture->SetInOn(solid);
    booleanTexture->SetOutOn(edge);
    break;
  case 3: /* cut all 1 */
    booleanTexture->SetInIn(clear);
    booleanTexture->SetOutIn(clear);
    booleanTexture->SetInOut(solid);
    booleanTexture->SetOutOut(solid);
    booleanTexture->SetOnOn(edge);
    booleanTexture->SetOnIn(clear);
    booleanTexture->SetOnOut(solid);
    booleanTexture->SetInOn(edge);
    booleanTexture->SetOutOn(edge);
    break;
  case 4:
    booleanTexture->SetInIn(solid);
    booleanTexture->SetOutIn(solid);
    booleanTexture->SetInOut(clear);
    booleanTexture->SetOutOut(solid);
    booleanTexture->SetOnOn(edge);
    booleanTexture->SetOnIn(solid);
    booleanTexture->SetOnOut(edge);
    booleanTexture->SetInOn(edge);
    booleanTexture->SetOutOn(solid);
    break;
  case 5:
    booleanTexture->SetInIn(clear);
    booleanTexture->SetOutIn(solid);
    booleanTexture->SetInOut(clear);
    booleanTexture->SetOutOut(solid);
    booleanTexture->SetOnOn(edge);
    booleanTexture->SetOnIn(edge);
    booleanTexture->SetOnOut(edge);
    booleanTexture->SetInOn(clear);
    booleanTexture->SetOutOn(solid);
    break;
  case 6:
    booleanTexture->SetInIn(solid);
    booleanTexture->SetOutIn(clear);
    booleanTexture->SetInOut(clear);
    booleanTexture->SetOutOut(solid);
    booleanTexture->SetOnOn(edge);
    booleanTexture->SetOnIn(edge);
    booleanTexture->SetOnOut(edge);
    booleanTexture->SetInOn(edge);
    booleanTexture->SetOutOn(edge);
    break;
  case 7:
    booleanTexture->SetInIn(clear);
    booleanTexture->SetOutIn(clear);
    booleanTexture->SetInOut(clear);
    booleanTexture->SetOutOut(solid);
    booleanTexture->SetOnOn(edge);
    booleanTexture->SetOnIn(clear);
    booleanTexture->SetOnOut(edge);
    booleanTexture->SetInOn(clear);
    booleanTexture->SetOutOn(edge);
    break;
  case 8:
    booleanTexture->SetInIn(solid);
    booleanTexture->SetOutIn(solid);
    booleanTexture->SetInOut(solid);
    booleanTexture->SetOutOut(clear);
    booleanTexture->SetOnOn(edge);
    booleanTexture->SetOnIn(solid);
    booleanTexture->SetOnOut(edge);
    booleanTexture->SetInOn(solid);
    booleanTexture->SetOutOn(edge);
    break;
  case 9:
    booleanTexture->SetInIn(clear);
    booleanTexture->SetInOut(solid);
    booleanTexture->SetOutIn(solid);
    booleanTexture->SetOutOut(clear);
    booleanTexture->SetOnOn(edge);
    booleanTexture->SetOnIn(edge);
    booleanTexture->SetOnOut(edge);
    booleanTexture->SetInOn(edge);
    booleanTexture->SetOutOn(edge);
    break;
  case 10:
    booleanTexture->SetInIn(solid);
    booleanTexture->SetInOut(solid);
    booleanTexture->SetOutIn(clear);
    booleanTexture->SetOutOut(clear);
    booleanTexture->SetOnOn(edge);
    booleanTexture->SetOnIn(edge);
    booleanTexture->SetOnOut(edge);
    booleanTexture->SetInOn(solid);
    booleanTexture->SetOutOn(clear);
    break;
  case 11:
    booleanTexture->SetInIn(clear);
    booleanTexture->SetInOut(solid);
    booleanTexture->SetOutIn(clear);
    booleanTexture->SetOutOut(clear);
    booleanTexture->SetOnOn(edge);
    booleanTexture->SetOnIn(clear);
    booleanTexture->SetOnOut(edge);
    booleanTexture->SetInOn(edge);
    booleanTexture->SetOutOn(clear);
    break;
  case 12:
    booleanTexture->SetInIn(solid);
    booleanTexture->SetInOut(clear);
    booleanTexture->SetOutIn(solid);
    booleanTexture->SetOutOut(clear);
    booleanTexture->SetOnOn(edge);
    booleanTexture->SetOnIn(solid);
    booleanTexture->SetOnOut(clear);
    booleanTexture->SetInOn(edge);
    booleanTexture->SetOutOn(edge);
    break;
  case 13:
    booleanTexture->SetInIn(clear);
    booleanTexture->SetInOut(clear);
    booleanTexture->SetOutIn(solid);
    booleanTexture->SetOutOut(clear);
    booleanTexture->SetOnOn(edge);
    booleanTexture->SetOnIn(edge);
    booleanTexture->SetOnOut(clear);
    booleanTexture->SetInOn(clear);
    booleanTexture->SetOutOn(edge);
    break;
  case 14:
    booleanTexture->SetInIn(solid);
    booleanTexture->SetInOut(clear);
    booleanTexture->SetOutIn(clear);
    booleanTexture->SetOutOut(clear);
    booleanTexture->SetOnOn(edge);
    booleanTexture->SetOnIn(edge);
    booleanTexture->SetOnOut(clear);
    booleanTexture->SetInOn(edge);
    booleanTexture->SetOutOn(clear);
    break;
  case 15:
    booleanTexture->SetInIn(clear);
    booleanTexture->SetInOut(clear);
    booleanTexture->SetOutIn(clear);
    booleanTexture->SetOutOut(clear);
    booleanTexture->SetOnOn(clear);
    booleanTexture->SetOnIn(clear);
    booleanTexture->SetOnOut(clear);
    booleanTexture->SetInOn(clear);
    booleanTexture->SetOutOn(clear);
    break;
  }

  return booleanTexture;
}
} // namespace
