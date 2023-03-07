#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkQuadricLODActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    // High res sphere
    vtkNew<vtkSphereSource> highResSphereSource;
    highResSphereSource->SetThetaResolution(200);
    highResSphereSource->SetPhiResolution(200);
    highResSphereSource->Update();

    vtkNew<vtkPolyDataMapper> highResMapper;
    highResMapper->SetInputConnection(highResSphereSource->GetOutputPort());

    vtkNew<vtkQuadricLODActor> actor;
    actor->SetMapper(highResMapper.Get());
    actor->GetProperty()->SetInterpolationToFlat();
    actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    // A renderer and render window
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer.Get());
    renderWindow->SetWindowName("QuadricLODActor");

    // An interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());

    renderWindowInteractor->SetDesiredUpdateRate(1e20);

    renderer->AddActor(actor.Get());
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
