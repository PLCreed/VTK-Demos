#include <sstream>

#include <vtkActor.h>
#include <vtkBillboardTextActor3D.h>
#include <vtkCallbackCommand.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTextProperty.h>

namespace
{
void ActorCallback(vtkObject *caller, long unsigned int vtkNotUsed(eventId), void *clientData,
                   void *vtkNotUsed(callData))
{
    auto textActor = static_cast<vtkBillboardTextActor3D *>(clientData);
    auto actor = static_cast<vtkActor *>(caller);
    std::ostringstream label;
    label << std::setprecision(3) << actor->GetPosition()[0] << ", " << actor->GetPosition()[1] << ", "
          << actor->GetPosition()[2] << std::endl;
    textActor->SetPosition(actor->GetPosition());
    textActor->SetInput(label.str().c_str());
}

void RandomPosition(double p[3], double const &min_r, double const &max_r, vtkMinimalStandardRandomSequence *rng);

} // namespace

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    // For testing
    vtkNew<vtkMinimalStandardRandomSequence> rng;
    // rng->SetSeed(8775070);
    rng->SetSeed(5127);

    // Create a renderer
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

    // Create a render window
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer.Get());
    renderWindow->SetWindowName("BillboardTextActor3D");

    // Create an interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());

    // Create a sphere
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetCenter(0.0, 0.0, 0.0);
    sphereSource->SetRadius(1.0);

    auto min_r = -10.0;
    auto max_r = 10.0;

    for (auto i = 0; i < 10; ++i) {
        if (i == 0) {
            // Create an actor representing the origin
            vtkNew<vtkPolyDataMapper> mapper;
            mapper->SetInputConnection(sphereSource->GetOutputPort());

            vtkNew<vtkActor> actor;
            actor->SetMapper(mapper.Get());
            actor->SetPosition(0, 0, 0);
            actor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());

            renderer->AddActor(actor.Get());
        }
        // Create a mapper
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputConnection(sphereSource->GetOutputPort());

        // Create an actor
        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper.Get());
        actor->SetPosition(0, 0, 0);
        actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

        // Setup the text and add it to the renderer
        vtkNew<vtkBillboardTextActor3D> textActor;
        textActor->SetInput("");
        textActor->SetPosition(actor->GetPosition());
        textActor->GetTextProperty()->SetFontSize(12);
        textActor->GetTextProperty()->SetColor(colors->GetColor3d("Gold").GetData());
        textActor->GetTextProperty()->SetJustificationToCentered();

        renderer->AddActor(actor.Get());
        renderer->AddActor(textActor.Get());

        // If you want to use a callback, do this:
        // vtkNew<vtkCallbackCommand> actorCallback;
        // actorCallback->SetCallback(ActorCallback.Get());
        // actorCallback->SetClientData(textActor.Get());
        // actor->AddObserver(vtkCommand::ModifiedEvent, actorCallback.Get());
        // Otherwise do this:
        double position[3];
        RandomPosition(position, min_r, max_r, rng.Get());
        actor->SetPosition(position);
        std::ostringstream label;
        label << std::setprecision(3) << actor->GetPosition()[0] << ", " << actor->GetPosition()[1] << ", "
              << actor->GetPosition()[2] << std::endl;
        textActor->SetPosition(actor->GetPosition());
        textActor->SetInput(label.str().c_str());
    }
    renderWindow->Render();
    renderWindow->SetWindowName("BillboardTextActor3D");
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

namespace
{

void RandomPosition(double p[3], double const &min_r, double const &max_r, vtkMinimalStandardRandomSequence *rng)
{
    for (auto i = 0; i < 3; ++i) {
        p[i] = rng->GetRangeValue(min_r, max_r);
        rng->Next();
    }
}

} // namespace
