#include <vtkActor.h>
#include <vtkConeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    // Create a cone
    vtkNew<vtkConeSource> coneSource;
    coneSource->Update();

    // Create a mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(coneSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper.Get());
    actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("bisque").GetData());

    // Create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer.Get());
    renderWindow->SetSize(640, 480);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());

    // Add the actors to the scene
    renderer->AddActor(actor.Get());
    renderer->SetBackground(colors->GetColor3d("Salmon").GetData());

    // Render and interact
    renderWindow->SetWindowName("Cone");
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
