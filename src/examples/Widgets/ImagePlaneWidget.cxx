#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkImagePlaneWidget.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->Update();

    // Create a mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper.Get());
    actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    // A renderer and render window
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer.Get());
    renderWindow->SetWindowName("ImagePlaneWidget");

    renderer->AddActor(actor.Get());
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    // An interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());

    // vtkNew<vtkInteractorStyleTrackballActor> style;
    vtkNew<vtkInteractorStyleTrackballCamera> style;

    renderWindowInteractor->SetInteractorStyle(style.Get());

    vtkNew<vtkImagePlaneWidget> planeWidget;
    planeWidget->SetInteractor(renderWindowInteractor.Get());
    planeWidget->TextureVisibilityOff();

    double origin[3] = {0, 1, 0};
    planeWidget->SetOrigin(origin);
    planeWidget->UpdatePlacement();

    // Render
    renderWindow->Render();

    renderer->GetActiveCamera()->Azimuth(-45);
    renderer->GetActiveCamera()->Zoom(0.85);

    renderWindowInteractor->Initialize();
    renderWindow->Render();
    planeWidget->On();

    // Begin mouse interaction
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
