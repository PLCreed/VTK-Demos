#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVertex.h>

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0, 0, 0);

    vtkNew<vtkVertex> vertex;
    vertex->GetPointIds()->SetId(0, 0);

    vtkNew<vtkCellArray> vertices;
    vertices->InsertNextCell(vertex.Get());

    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points.Get());
    polydata->SetVerts(vertices.Get());

    // Setup actor and mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polydata.Get());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper.Get());
    actor->GetProperty()->SetPointSize(30);
    actor->GetProperty()->SetColor(colors->GetColor3d("PeachPuff").GetData());

    // Setup render window, renderer, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetWindowName("Vertex");
    renderWindow->AddRenderer(renderer.Get());
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());
    renderer->AddActor(actor.Get());
    renderer->SetBackground(colors->GetColor3d("DarkGreen").GetData());

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
