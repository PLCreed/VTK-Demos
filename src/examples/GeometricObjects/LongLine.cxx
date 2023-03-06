#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkLine.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char *[])
{
    // Create five points.
    double origin[3] = {0.0, 0.0, 0.0};
    double p0[3] = {1.0, 0.0, 0.0};
    double p1[3] = {0.0, 1.0, 0.0};
    double p2[3] = {0.0, 1.0, 2.0};
    double p3[3] = {1.0, 2.0, 3.0};

    // Create a vtkPoints object and store the points in it
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(origin);
    points->InsertNextPoint(p0);
    points->InsertNextPoint(p1);
    points->InsertNextPoint(p2);
    points->InsertNextPoint(p3);

    // Create a cell array to store the lines in and add the lines to it
    vtkNew<vtkCellArray> lines;

    for (unsigned int i = 0; i < 3; i++) {
        vtkNew<vtkLine> line;
        line->GetPointIds()->SetId(0, i);
        line->GetPointIds()->SetId(1, i + 1);
        lines->InsertNextCell(line.Get());
    }

    // Create a polydata to store everything in
    vtkNew<vtkPolyData> linesPolyData;

    // Add the points to the dataset
    linesPolyData->SetPoints(points.Get());

    // Add the lines to the dataset
    linesPolyData->SetLines(lines.Get());

    // Setup actor and mapper
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(linesPolyData.Get());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper.Get());
    actor->GetProperty()->SetLineWidth(4);
    actor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());

    // Setup render window, renderer, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetWindowName("LongLine");
    renderWindow->AddRenderer(renderer.Get());
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());
    renderer->AddActor(actor.Get());

    renderer->ResetCamera();
    renderer->GetActiveCamera()->Azimuth(30);
    renderer->GetActiveCamera()->Elevation(30);
    renderer->ResetCameraClippingRange();

    renderer->SetBackground(colors->GetColor3d("Silver").GetData());
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
