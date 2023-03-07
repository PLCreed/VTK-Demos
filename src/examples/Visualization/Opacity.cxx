#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    // Cube
    vtkNew<vtkCubeSource> cubeSource;
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cubeSource->GetOutputPort());

    vtkNew<vtkActor> cubeActor;
    cubeActor->GetProperty()->SetOpacity(0.5);
    cubeActor->SetMapper(cubeMapper.Get());
    cubeActor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    // Sphere
    vtkNew<vtkSphereSource> sphereSource;
    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkNew<vtkActor> sphereActor;
    sphereActor->GetProperty()->SetColor(colors->GetColor3d("LightGreen").GetData());
    sphereActor->GetProperty()->SetOpacity(0.7);
    sphereActor->SetMapper(sphereMapper.Get());

    // Create renderers and add actors of plane and cube
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(cubeActor.Get());
    renderer->AddActor(sphereActor.Get());

    // Add renderer to renderwindow and render
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer.Get());
    renderWindow->SetWindowName("Opacity");

    renderer->SetBackground(colors->GetColor3d("DimGray").GetData());
    renderWindow->Render();
    renderer->GetActiveCamera()->Azimuth(-22.5);
    renderer->GetActiveCamera()->Elevation(30);

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow.Get());
    renderWindow->Render();

    interactor->Start();
    return EXIT_SUCCESS;
}
