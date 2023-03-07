#include <array>

#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkImageActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageMapper3D.h>
#include <vtkImageTracerWidget.h>
#include <vtkInteractorStyleImage.h>
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
void CallbackFunction(vtkObject *caller, long unsigned int eventId, void *clientData, void *callData);
}

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    std::array<double, 3> drawColor1 {0, 0, 0};
    std::array<double, 3> drawColor2 {0, 0, 0};
    auto color1 = colors->GetColor3ub("SlateGray").GetData();
    auto color2 = colors->GetColor3ub("Tomato").GetData();
    for (auto i = 0; i < 3; ++i) {
        drawColor1[i] = color1[i];
        drawColor2[i] = color2[i];
    }

    vtkNew<vtkImageCanvasSource2D> imageSource;
    imageSource->SetScalarTypeToUnsignedChar();
    imageSource->SetNumberOfScalarComponents(3);
    imageSource->SetExtent(0, 20, 0, 50, 0, 0);
    imageSource->SetDrawColor(drawColor1.data());
    imageSource->FillBox(0, 20, 0, 50);
    imageSource->SetDrawColor(drawColor2.data());
    imageSource->FillBox(0, 10, 0, 30);
    imageSource->Update();

    vtkNew<vtkImageActor> actor;
    actor->GetMapper()->SetInputConnection(imageSource->GetOutputPort());

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor.Get());
    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

    renderer->ResetCamera();
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer.Get());
    renderWindow->SetWindowName("ImageTracerWidget");

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow.Get());

    vtkNew<vtkInteractorStyleImage> style;
    interactor->SetInteractorStyle(style.Get());

    vtkNew<vtkImageTracerWidget> tracer;
    tracer->GetLineProperty()->SetLineWidth(5);
    tracer->SetInteractor(interactor.Get());
    tracer->SetViewProp(actor.Get());
    renderWindow->Render();

    // The observer must be added BEFORE the On() call.
    vtkNew<vtkCallbackCommand> callback;
    callback->SetCallback(CallbackFunction);
    tracer->AddObserver(vtkCommand::EndInteractionEvent, callback.Get());

    tracer->On();
    interactor->Start();

    return EXIT_SUCCESS;
}

namespace
{
void CallbackFunction(vtkObject *caller, long unsigned int vtkNotUsed(eventId), void *vtkNotUsed(clientData),
                      void *vtkNotUsed(callData))
{
    vtkImageTracerWidget *tracerWidget = static_cast<vtkImageTracerWidget *>(caller);

    vtkNew<vtkPolyData> path;

    tracerWidget->GetPath(path.Get());
    std::cout << "There are " << path->GetNumberOfPoints() << " points in the path." << std::endl;
}
} // end anonymous namespace
