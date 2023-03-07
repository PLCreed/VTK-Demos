#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkInteractorStyleUser.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace
{
void ClickCallbackFunction(vtkObject *caller, long unsigned int eventId, void *clientData, void *callData);
}

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->Update();

    // Visualize
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper.Get());
    actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer.Get());
    renderWindow->SetWindowName("InteractorStyleUser");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());

    renderer->AddActor(actor.Get());
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    renderWindow->Render();

    vtkNew<vtkCallbackCommand> clickCallback;
    clickCallback->SetCallback(ClickCallbackFunction);

    renderWindowInteractor->AddObserver(vtkCommand::LeftButtonPressEvent, clickCallback.Get());

    vtkNew<vtkInteractorStyleUser> style;
    renderWindowInteractor->SetInteractorStyle(style.Get());

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

namespace
{
void ClickCallbackFunction(vtkObject *, long unsigned int, void *, void *)
{
    std::cout << "Click callback" << std::endl;

    // We can get the calling object like this:
    // vtkRenderWindowInteractor *iren =
    //  static_cast<vtkRenderWindowInteractor*>(caller);
}

} // namespace
