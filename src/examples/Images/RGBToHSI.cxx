#include <vtkCamera.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageMapper3D.h>
#include <vtkImageRGBToHSI.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int argc, char *argv[])
{
    vtkNew<vtkNamedColors> colors;

    // Verify command line arguments
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << "image e.g. Gourds2.jpg" << std::endl;
        return EXIT_FAILURE;
    }

    // Read the image
    vtkNew<vtkImageReader2Factory> readerFactory;
    vtkSmartPointer<vtkImageReader2> reader;
    reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
    reader->SetFileName(argv[1]);

    vtkNew<vtkImageRGBToHSI> hsiFilter;
    hsiFilter->SetInputConnection(reader->GetOutputPort());

    vtkNew<vtkImageExtractComponents> extractHFilter;
    extractHFilter->SetInputConnection(hsiFilter->GetOutputPort());
    extractHFilter->SetComponents(0);

    vtkNew<vtkImageExtractComponents> extractSFilter;
    extractSFilter->SetInputConnection(hsiFilter->GetOutputPort());
    extractSFilter->SetComponents(1);

    vtkNew<vtkImageExtractComponents> extractIFilter;
    extractIFilter->SetInputConnection(hsiFilter->GetOutputPort());
    extractIFilter->SetComponents(2);

    // Create actors
    vtkNew<vtkImageActor> inputActor;
    inputActor->GetMapper()->SetInputConnection(reader->GetOutputPort());

    vtkNew<vtkImageActor> hActor;
    hActor->GetMapper()->SetInputConnection(extractHFilter->GetOutputPort());

    vtkNew<vtkImageActor> sActor;
    sActor->GetMapper()->SetInputConnection(extractSFilter->GetOutputPort());

    vtkNew<vtkImageActor> iActor;
    iActor->GetMapper()->SetInputConnection(extractIFilter->GetOutputPort());

    // Define viewport ranges
    // (xmin, ymin, xmax, ymax)
    double inputViewport[4] = {0.0, 0.0, 0.25, 1.0};
    double hViewport[4] = {0.25, 0.0, 0.5, 1.0};
    double sViewport[4] = {0.5, 0.0, 0.75, 1.0};
    double iViewport[4] = {0.75, 0.0, 1.0, 1.0};

    // Shared camera
    vtkNew<vtkCamera> sharedCamera;

    // Setup renderers
    vtkNew<vtkRenderer> inputRenderer;
    inputRenderer->SetViewport(inputViewport);
    inputRenderer->AddActor(inputActor.Get());
    inputRenderer->SetActiveCamera(sharedCamera.Get());
    inputRenderer->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());

    vtkNew<vtkRenderer> hRenderer;
    hRenderer->SetViewport(hViewport);
    hRenderer->AddActor(hActor.Get());
    hRenderer->SetActiveCamera(sharedCamera.Get());
    hRenderer->SetBackground(colors->GetColor3d("MistyRose").GetData());

    vtkNew<vtkRenderer> sRenderer;
    sRenderer->SetViewport(sViewport);
    sRenderer->AddActor(sActor.Get());
    sRenderer->SetActiveCamera(sharedCamera.Get());
    sRenderer->SetBackground(colors->GetColor3d("LavenderBlush").GetData());

    vtkNew<vtkRenderer> iRenderer;
    iRenderer->SetViewport(iViewport);
    iRenderer->AddActor(iActor.Get());
    iRenderer->SetActiveCamera(sharedCamera.Get());
    iRenderer->SetBackground(colors->GetColor3d("Lavender").GetData());

    // Setup render window
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(1000, 250);
    renderWindow->SetWindowName("RGBToHSI");
    renderWindow->AddRenderer(inputRenderer.Get());
    renderWindow->AddRenderer(hRenderer.Get());
    renderWindow->AddRenderer(sRenderer.Get());
    renderWindow->AddRenderer(iRenderer.Get());
    inputRenderer->ResetCamera();

    // Setup render window interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkNew<vtkInteractorStyleImage> style;

    renderWindowInteractor->SetInteractorStyle(style.Get());

    // Render and start interaction
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());
    renderWindow->Render();
    renderWindowInteractor->Initialize();

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
