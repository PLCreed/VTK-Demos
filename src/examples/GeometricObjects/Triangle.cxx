#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTriangle.h>

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    // Create a triangle
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(1.0, 0.0, 0.0);
    points->InsertNextPoint(0.0, 0.0, 0.0);
    points->InsertNextPoint(0.0, 1.0, 0.0);

    vtkNew<vtkTriangle> triangle;
    triangle->GetPointIds()->SetId(0, 0);
    triangle->GetPointIds()->SetId(1, 1);
    triangle->GetPointIds()->SetId(2, 2);

    vtkNew<vtkCellArray> triangles;
    triangles->InsertNextCell(triangle.Get());

    // Create a polydata object
    vtkNew<vtkPolyData> trianglePolyData;

    // Add the geometry and topology to the polydata
    trianglePolyData->SetPoints(points.Get());
    trianglePolyData->SetPolys(triangles.Get());

    // Create mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(trianglePolyData.Get());

    vtkNew<vtkActor> actor;
    actor->GetProperty()->SetColor(colors->GetColor3d("PeachPuff").GetData());
    actor->SetMapper(mapper.Get());

    // Create a renderer, render window, and an interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetWindowName("Triangle");
    renderWindow->AddRenderer(renderer.Get());
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());

    // Add the actors to the scene
    renderer->AddActor(actor.Get());
    renderer->SetBackground(colors->GetColor3d("DarkGreen").GetData());

    // Render and interact
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
