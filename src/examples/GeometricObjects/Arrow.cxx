#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    // Create an arrow.
    vtkNew<vtkArrowSource> arrowSource;
    // arrowSource->SetShaftRadius(1.0);
    // arrowSource->SetTipLength(1.0);
    arrowSource->Update();

    // Create a mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(arrowSource->GetOutputPort());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper.Get());

    // Visualize
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetWindowName("Arrow");
    renderWindow->AddRenderer(renderer.Get());
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());

    renderer->AddActor(actor.Get());
    renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

    renderWindow->SetWindowName("Arrow");
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
