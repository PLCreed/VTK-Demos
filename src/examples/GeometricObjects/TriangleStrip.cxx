#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkDataSetMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTriangleStrip.h>

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0, 0, 0);
    points->InsertNextPoint(0, 1, 0);
    points->InsertNextPoint(1, 0, 0);
    points->InsertNextPoint(1.5, 1, 0);

    vtkNew<vtkTriangleStrip> triangleStrip;
    triangleStrip->GetPointIds()->SetNumberOfIds(4);
    triangleStrip->GetPointIds()->SetId(0, 0);
    triangleStrip->GetPointIds()->SetId(1, 1);
    triangleStrip->GetPointIds()->SetId(2, 2);
    triangleStrip->GetPointIds()->SetId(3, 3);

    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell(triangleStrip.Get());

    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points.Get());
    polydata->SetStrips(cells.Get());

    // Create an actor and mapper
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(polydata.Get());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper.Get());
    actor->GetProperty()->SetColor(colors->GetColor3d("PeachPuff").GetData());
    actor->GetProperty()->SetRepresentationToWireframe();

    // Create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetWindowName("TriangleStrip");
    renderWindow->AddRenderer(renderer.Get());
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());

    renderer->AddActor(actor.Get());
    renderer->SetBackground(colors->GetColor3d("DarkGreen").GetData());
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
