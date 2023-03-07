#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTriangle.h>
#include <vtkUnsignedCharArray.h>
#include <vtkXMLPolyDataWriter.h>

// For compatibility with new VTK generic data arrays
#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
#endif

int main(int, char *[])
{
    vtkNew<vtkNamedColors> nc;

    // Setup points
    vtkNew<vtkPoints> points;
    // vtkNew<vtkCellArray> vertices;
    points->InsertNextPoint(0.0, 0.0, 0.0);
    points->InsertNextPoint(1.0, 0.0, 0.0);
    points->InsertNextPoint(0.0, 1.0, 0.0);

    vtkNew<vtkUnsignedCharArray> colors;
    colors->SetNumberOfComponents(3);
    colors->SetName("Colors");
    // Pick a color
    colors->InsertNextTupleValue(nc->GetColor3ub("LightSeaGreen").GetData());

    vtkNew<vtkCellArray> triangles;
    vtkNew<vtkTriangle> triangle;
    triangle->GetPointIds()->SetId(0, 0);
    triangle->GetPointIds()->SetId(1, 1);
    triangle->GetPointIds()->SetId(2, 2);
    triangles->InsertNextCell(triangle.Get());

    vtkNew<vtkPolyData> polydata;

    polydata->SetPoints(points.Get());
    polydata->SetPolys(triangles.Get());
    polydata->GetCellData()->SetScalars(colors.Get());

    // Visualize
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polydata.Get());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper.Get());

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer.Get());
    renderWindow->SetWindowName("TriangleSolidColor");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());

    renderer->AddActor(actor.Get());
    renderer->SetBackground(nc->GetColor3d("SlateGray").GetData());
    renderer->ResetCamera();

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
