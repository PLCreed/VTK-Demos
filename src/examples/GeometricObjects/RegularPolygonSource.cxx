#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkShrinkPolyData.h>

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    // Create a pentagon
    vtkNew<vtkRegularPolygonSource> polygonSource;
    polygonSource->SetNumberOfSides(5);
    polygonSource->SetRadius(5);
    polygonSource->SetCenter(0, 0, 0);

    vtkNew<vtkShrinkPolyData> shrink;
    shrink->SetInputConnection(polygonSource->GetOutputPort());
    shrink->SetShrinkFactor(0.9);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(shrink->GetOutputPort());

    vtkNew<vtkProperty> back;
    back->SetColor(colors->GetColor3d("Tomato").GetData());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper.Get());
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetLineWidth(5);
    actor->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());
    actor->SetBackfaceProperty(back.Get());

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetWindowName("RegularPolygonSource");
    renderWindow->AddRenderer(renderer.Get());
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());

    renderer->AddActor(actor.Get());
    renderer->SetBackground(colors->GetColor3d("Silver").GetData());

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
