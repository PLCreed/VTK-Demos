#include <array>

#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkImageAccumulate.h>
#include <vtkImageActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkImageStencilToImage.h>
#include <vtkImageTracerWidget.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNGWriter.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace
{
void CallbackFunction(vtkObject *caller, long unsigned int eventId, void *clientData, void *callData);

void CreateImage1(vtkImageData *);

} // namespace

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkImageData> image;
    CreateImage1(image.Get());

    vtkNew<vtkImageActor> actor;
    actor->GetMapper()->SetInputData(image.Get());

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor.Get());
    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

    renderer->ResetCamera();
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer.Get());
    renderWindow->SetWindowName("ImageTracerWidgetInsideContour");

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow.Get());

    vtkNew<vtkInteractorStyleImage> style;
    interactor->SetInteractorStyle(style.Get());

    vtkNew<vtkImageTracerWidget> tracer;
    tracer->GetLineProperty()->SetLineWidth(5);
    tracer->SetInteractor(interactor.Get());
    tracer->SetViewProp(actor.Get());
    tracer->AutoCloseOn();
    renderWindow->Render();

    // The observer must be added BEFORE the On() call.
    vtkNew<vtkCallbackCommand> callback;
    callback->SetCallback(CallbackFunction);
    callback->SetClientData(image.Get());
    tracer->AddObserver(vtkCommand::EndInteractionEvent, callback.Get());

    tracer->On();
    interactor->Start();

    return EXIT_SUCCESS;
}

namespace
{
void CallbackFunction(vtkObject *caller, long unsigned int vtkNotUsed(eventId), void *clientData,
                      void *vtkNotUsed(callData))
{
    vtkImageTracerWidget *tracerWidget = static_cast<vtkImageTracerWidget *>(caller);

    vtkNew<vtkPolyData> path;

    if (!tracerWidget->IsClosed()) {
        std::cout << "Path not closed!" << std::endl;
        return;
    }

    tracerWidget->GetPath(path.Get());
    std::cout << "There are " << path->GetNumberOfPoints() << " points in the path." << std::endl;

    vtkImageData *image = static_cast<vtkImageData *>(clientData);

    vtkNew<vtkPolyDataToImageStencil> polyDataToImageStencil;
    polyDataToImageStencil->SetTolerance(0);
    polyDataToImageStencil->SetInputData(path.Get());
    polyDataToImageStencil->SetOutputOrigin(image->GetOrigin());
    polyDataToImageStencil->SetOutputSpacing(image->GetSpacing());
    polyDataToImageStencil->SetOutputWholeExtent(image->GetExtent());
    polyDataToImageStencil->Update();

    vtkNew<vtkImageStencilToImage> imageStencilToImage;
    imageStencilToImage->SetInputConnection(polyDataToImageStencil->GetOutputPort());
    imageStencilToImage->SetInsideValue(255);
    imageStencilToImage->Update();

    vtkNew<vtkImageAccumulate> imageAccumulate;
    imageAccumulate->SetStencilData(polyDataToImageStencil->GetOutput());
    imageAccumulate->SetInputData(image);
    imageAccumulate->Update();
    std::cout << "Voxel count: " << imageAccumulate->GetVoxelCount() << std::endl;

    // vtkNew<vtkPNGWriter> writer;
    // writer->SetFileName("selection.png");
    // writer->SetInputConnection(imageStencilToImage->GetOutputPort());
    // writer->Write();
}

void CreateImage1(vtkImageData *image)
{
    vtkNew<vtkNamedColors> colors;

    std::array<double, 3> drawColor1 {0, 0, 0};
    std::array<double, 3> drawColor2 {0, 0, 0};
    auto color1 = colors->GetColor3ub("DarkGray").GetData();
    for (auto i = 0; i < 3; ++i) {
        drawColor1[i] = color1[i];
    }

    vtkNew<vtkImageCanvasSource2D> imageSource;
    imageSource->SetScalarTypeToUnsignedChar();
    imageSource->SetNumberOfScalarComponents(1);
    imageSource->SetExtent(0, 20, 0, 50, 0, 0);
    imageSource->SetDrawColor(drawColor1.data());
    imageSource->FillBox(0, 20, 0, 50);
    imageSource->Update();

    image->ShallowCopy(imageSource->GetOutput());
}
} // end anonymous namespace
