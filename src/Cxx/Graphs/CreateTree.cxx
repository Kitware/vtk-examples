/*=========================================================================

  Program:   Visualization Toolkit
  Module:    CreateTree.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
//
// This example creates a tree and labels the vertices and edges.
//

#include <vtkDataSetAttributes.h>
#include <vtkGraphLayoutView.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkNew.h>
#include <vtkRandomGraphSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStringArray.h>
#include <vtkTree.h>
#include <vtkViewTheme.h>

int main(int, char*[])
{
  vtkNew<vtkMutableDirectedGraph> graph;
  vtkIdType a = graph->AddVertex();
  vtkIdType b = graph->AddChild(a);
  vtkIdType c = graph->AddChild(a);
  vtkIdType d = graph->AddChild(b);
  vtkIdType e = graph->AddChild(c);
  vtkIdType f = graph->AddChild(c);

  vtkNew<vtkStringArray> vertexLabels;
  vertexLabels->SetName("VertexLabel");
  vertexLabels->InsertValue(a, "a");
  vertexLabels->InsertValue(b, "b");
  vertexLabels->InsertValue(c, "c");
  vertexLabels->InsertValue(d, "d");
  vertexLabels->InsertValue(e, "e");
  vertexLabels->InsertValue(f, "f");
  graph->GetVertexData()->AddArray(vertexLabels);
  vtkNew<vtkStringArray> edgeLabels;
  edgeLabels->SetName("EdgeLabel");
  edgeLabels->InsertValue(graph->GetEdgeId(a, b), "a -> b");
  edgeLabels->InsertValue(graph->GetEdgeId(a, c), "a -> c");
  edgeLabels->InsertValue(graph->GetEdgeId(b, d), "b -> d");
  edgeLabels->InsertValue(graph->GetEdgeId(c, e), "c -> e");
  edgeLabels->InsertValue(graph->GetEdgeId(c, f), "c -> f");
  graph->GetEdgeData()->AddArray(edgeLabels);

  vtkNew<vtkTree> tree;
  bool validTree = tree->CheckedShallowCopy(graph);
  if (!validTree)
  {
    std::cout << "Invalid tree" << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkGraphLayoutView> view;
  view->SetRepresentationFromInput(tree);
  // Apply a theme to the views
  vtkNew<vtkViewTheme> theme;
  view->ApplyViewTheme(theme->CreateMellowTheme());
  view->SetVertexColorArrayName("VertexDegree");
  view->SetColorVertices(true);
  view->SetVertexLabelArrayName("VertexLabel");
  view->SetVertexLabelVisibility(true);
  view->SetEdgeLabelArrayName("EdgeLabel");
  view->SetEdgeLabelVisibility(true);
  view->SetLayoutStrategyToTree();

  view->ResetCamera();
  view->GetRenderWindow()->SetSize(600, 600);
  view->GetRenderWindow()->SetWindowName("CreateTree");
  view->GetRenderWindow()->Render();
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
