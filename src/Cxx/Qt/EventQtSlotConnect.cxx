#include "EventQtSlotConnect.h"
#include "ui_EventQtSlotConnect.h"

#include <vtkEventQtSlotConnect.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlatonicSolidSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVersion.h>

#if VTK_VERSION_NUMBER >= 89000000000ULL
#define VTK890 1
#endif

namespace {
/** Get a specialised lookup table for the platonic solids.
 *
 * Since each face of a vtkPlatonicSolidSource has a different
 * cell scalar, we create a lookup table with a different colour
 * for each face.
 * The colors have been carefully chosen so that adjacent cells
 * are colored distinctly.
 *
 * @return The lookup table.
 */
vtkNew<vtkLookupTable> GetPlatonicLUT();
} // namespace

// Constructor
EventQtSlotConnect::EventQtSlotConnect(QWidget* parent)
  : QMainWindow(parent), ui(new Ui::EventQtSlotConnect)
{
  this->ui->setupUi(this);

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
#if VTK890
  this->ui->qvtkWidget->setRenderWindow(renderWindow);
#else
  this->ui->qvtkWidget->SetRenderWindow(renderWindow);
#endif

  vtkNew<vtkEventQtSlotConnect> slotConnector;
  this->Connections = slotConnector;

  auto lut = GetPlatonicLUT();

  vtkNew<vtkPlatonicSolidSource> source;
  source->SetSolidTypeToOctahedron();

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(source->GetOutputPort());
  mapper->SetLookupTable(lut);
  mapper->SetScalarRange(0, 19);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

#if VTK890
  this->ui->qvtkWidget->renderWindow()->AddRenderer(renderer);
#else
  this->ui->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);
#endif

#if VTK890
  this->Connections->Connect(
      this->ui->qvtkWidget->renderWindow()->GetInteractor(),
      vtkCommand::LeftButtonPressEvent, this,
      SLOT(slot_clicked(vtkObject*, unsigned long, void*, void*)));
#else
  this->Connections->Connect(
      this->qvtkWidget->GetRenderWindow()->GetInteractor(),
      vtkCommand::LeftButtonPressEvent, this,
      SLOT(slot_clicked(vtkObject*, unsigned long, void*, void*)));
#endif
};

EventQtSlotConnect::~EventQtSlotConnect()
{
  delete this->ui;
}

void EventQtSlotConnect::slot_clicked(vtkObject*, unsigned long, void*, void*)
{
  std::cout << "Clicked." << std::endl;
}

namespace {

vtkNew<vtkLookupTable> GetPlatonicLUT()
{
  vtkNew<vtkLookupTable> lut;
  lut->SetNumberOfTableValues(20);
  lut->SetTableRange(0.0, 19.0);
  lut->Build();
  lut->SetTableValue(0, 0.1, 0.1, 0.1);
  lut->SetTableValue(1, 0, 0, 1);
  lut->SetTableValue(2, 0, 1, 0);
  lut->SetTableValue(3, 0, 1, 1);
  lut->SetTableValue(4, 1, 0, 0);
  lut->SetTableValue(5, 1, 0, 1);
  lut->SetTableValue(6, 1, 1, 0);
  lut->SetTableValue(7, 0.9, 0.7, 0.9);
  lut->SetTableValue(8, 0.5, 0.5, 0.5);
  lut->SetTableValue(9, 0.0, 0.0, 0.7);
  lut->SetTableValue(10, 0.5, 0.7, 0.5);
  lut->SetTableValue(11, 0, 0.7, 0.7);
  lut->SetTableValue(12, 0.7, 0, 0);
  lut->SetTableValue(13, 0.7, 0, 0.7);
  lut->SetTableValue(14, 0.7, 0.7, 0);
  lut->SetTableValue(15, 0, 0, 0.4);
  lut->SetTableValue(16, 0, 0.4, 0);
  lut->SetTableValue(17, 0, 0.4, 0.4);
  lut->SetTableValue(18, 0.4, 0, 0);
  lut->SetTableValue(19, 0.4, 0, 0.4);
  return lut;
}

} // namespace
