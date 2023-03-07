#include <cstdlib>

#include <vtkActor.h>
#include <vtkBox.h>
#include <vtkCamera.h>
#include <vtkColor.h>
#include <vtkContourFilter.h>
#include <vtkImplicitBoolean.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>
#include <vtkSphere.h>

int main(int argc, char *argv[])
{
    // Define colors
    auto colors = vtkSmartPointer<vtkNamedColors>::New();
    vtkColor3d actorColor = colors->GetColor3d("AliceBlue");
    vtkColor3d EdgeColour = colors->GetColor3d("SteelBlue");
    vtkColor3d BackgroundColour = colors->GetColor3d("Silver");

    // create a sphere
    auto sphere = vtkSmartPointer<vtkSphere>::New();
    sphere->SetCenter(0.0, 0.0, 0.0);
    sphere->SetRadius(0.5);

    /* The sample function generates a distance function from the implicit
           function.This is then contoured to get a polygonal surface.*/
    auto sample = vtkSmartPointer<vtkSampleFunction>::New();
    sample->SetImplicitFunction(sphere.Get());
    sample->SetModelBounds(-.5, .5, -.5, .5, -.5, .5);
    sample->SetSampleDimensions(20, 20, 20);
    sample->ComputeNormalsOff();

    // contour
    auto surface = vtkSmartPointer<vtkContourFilter>::New();
    surface->SetInputConnection(sample->GetOutputPort());
    surface->SetValue(0, 0.0);

    // Create a mapper and an actor
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(surface->GetOutputPort());
    mapper->ScalarVisibilityOff();
    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper.Get());
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetColor(actorColor.GetData());
    actor->GetProperty()->SetEdgeColor(EdgeColour.GetData());

    // A renderer and render window
    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(BackgroundColour.GetData());
    auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer.Get());
    renderWindow->SetWindowName("ImplicitSphere1");

    auto renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());
    // add the actor
    renderer->AddActor(actor.Get());
    // Start
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
