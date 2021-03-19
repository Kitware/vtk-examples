#include <vtkDataRepresentation.h>
#include <vtkNew.h>
#include <vtkRegressionTestImage.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTreeMapView.h>
#include <vtkViewTheme.h>
#include <vtkXMLTreeReader.h>

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    cout << "Usage: " << argv[0] << " treeFileName graphFileName" << std::endl;
    cout << "where: treeFileName is Infovis-XML-vtkclasses.xml and "
            "graphFileName is Infovis-XML-vtklibrary.xml"
         << endl;
    return EXIT_FAILURE;
  }

  std::string treeFileName(argv[1]);
  std::string graphFileName(argv[2]);

  // We need to put the graph and tree edges in different domains.
  vtkNew<vtkXMLTreeReader> reader1;
  reader1->SetFileName(treeFileName.c_str());
  reader1->SetEdgePedigreeIdArrayName("tree edge");
  reader1->GenerateVertexPedigreeIdsOff();
  reader1->SetVertexPedigreeIdArrayName("id");

  vtkNew<vtkXMLTreeReader> reader2;
  reader2->SetFileName(graphFileName.c_str());
  reader2->SetEdgePedigreeIdArrayName("graph edge");
  reader2->GenerateVertexPedigreeIdsOff();
  reader2->SetVertexPedigreeIdArrayName("id");

  reader1->Update();
  reader2->Update();

  vtkNew<vtkTreeMapView> view;
  view->DisplayHoverTextOff();
  view->SetGraphFromInputConnection(reader1->GetOutputPort());
  view->SetTreeFromInputConnection(reader2->GetOutputPort());

  view->SetAreaColorArrayName("level");
  view->SetEdgeColorToSplineFraction();
  view->SetColorEdges(true);
  view->SetAreaLabelArrayName("id");
  view->SetAreaHoverArrayName("id");
  view->SetAreaLabelVisibility(true);
  view->SetAreaSizeArrayName("VertexDegree");

  // Apply a theme to the views
  // vtkViewTheme* const theme = vtkViewTheme::CreateMellowTheme();
  // view->ApplyViewTheme(theme);
  // theme->Delete();
  vtkNew<vtkViewTheme> theme;
  view->ApplyViewTheme(theme->CreateMellowTheme());

  view->GetRenderWindow()->SetMultiSamples(0);
  view->GetRenderWindow()->SetAlphaBitPlanes(1);
  view->Update();
  view->ResetCamera();

  view->GetRenderWindow()->SetSize(600, 600);
  view->GetRenderWindow()->SetWindowName("TreeMapView");
  view->GetRenderWindow()->Render();
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
