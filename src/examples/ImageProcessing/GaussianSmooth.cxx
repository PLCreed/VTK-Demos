#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageMapper3D.h>
#include <vtkImageNoiseSource.h>
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
    // Verify input arguments
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " Filename e.g. Gourds.png" << std::endl;
        return EXIT_FAILURE;
    }

    // Read the image
    vtkNew<vtkImageReader2Factory> readerFactory;
    vtkSmartPointer<vtkImageReader2> reader;
    reader.TakeReference(readerFactory->CreateImageReader2(argv[1]));
    reader->SetFileName(argv[1]);
    reader->Update();

    // Process the
    vtkNew<vtkImageCast> cast;
    cast->SetInputConnection(reader->GetOutputPort());
    cast->SetOutputScalarTypeToFloat();

    vtkNew<vtkImageGaussianSmooth> filter;
    filter->SetDimensionality(2);
    filter->SetInputConnection(cast->GetOutputPort());
    filter->SetStandardDeviations(4.0, 4.0);
    filter->SetRadiusFactors(2.0, 2.0);

    // Create actors
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkImageActor> originalActor;
    originalActor->GetMapper()->SetInputConnection(reader->GetOutputPort());

    vtkNew<vtkImageActor> filteredActor;
    filteredActor->GetMapper()->SetInputConnection(filter->GetOutputPort());

    // Define viewport ranges
    // (xmin, ymin, xmax, ymax)
    double originalViewport[4] = {0.0, 0.0, 0.5, 1.0};
    double filteredViewport[4] = {0.5, 0.0, 1.0, 1.0};

    // Setup renderers
    vtkNew<vtkRenderer> originalRenderer;
    originalRenderer->SetViewport(originalViewport);
    originalRenderer->AddActor(originalActor.Get());
    originalRenderer->ResetCamera();
    originalRenderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    vtkNew<vtkRenderer> filteredRenderer;
    filteredRenderer->SetViewport(filteredViewport);
    filteredRenderer->AddActor(filteredActor.Get());
    filteredRenderer->ResetCamera();
    filteredRenderer->SetBackground(colors->GetColor3d("LightSlateGray").GetData());

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(600, 300);
    renderWindow->SetWindowName("GaussianSmooth");
    renderWindow->AddRenderer(originalRenderer.Get());
    renderWindow->AddRenderer(filteredRenderer.Get());

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkNew<vtkInteractorStyleImage> style;

    renderWindowInteractor->SetInteractorStyle(style.Get());

    renderWindowInteractor->SetRenderWindow(renderWindow.Get());
    renderWindow->Render();
    renderWindowInteractor->Initialize();

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
