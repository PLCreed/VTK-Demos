#include <vtkImageAccumulate.h>
#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageHybridMedian2D.h>
#include <vtkImageMapper3D.h>
#include <vtkImageThreshold.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPNGReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int argc, char *argv[])
{
    vtkNew<vtkNamedColors> colors;

    // Handle the arguments
    if (argc < 2) {
        std::cout << "Required arguments: filename.png e.g Gourds.png" << std::endl;
        return EXIT_FAILURE;
    }

    // Read the image
    vtkNew<vtkPNGReader> reader;
    reader->SetFileName(argv[1]);
    reader->Update();

    vtkNew<vtkImageHybridMedian2D> hybridMedian;
    hybridMedian->SetInputConnection(reader->GetOutputPort());

    vtkNew<vtkImageActor> originalActor;
    originalActor->GetMapper()->SetInputConnection(reader->GetOutputPort());

    vtkNew<vtkImageActor> hybridMedianActor;
    hybridMedianActor->GetMapper()->SetInputConnection(hybridMedian->GetOutputPort());

    // Define viewport ranges
    // (xmin, ymin, xmax, ymax)
    double originalViewport[4] = {0.0, 0.0, 0.5, 1.0};
    double hybridMedianViewport[4] = {0.5, 0.0, 1.0, 1.0};

    // Setup renderers
    vtkNew<vtkRenderer> originalRenderer;
    originalRenderer->SetViewport(originalViewport);
    originalRenderer->AddActor(originalActor.Get());
    originalRenderer->ResetCamera();
    originalRenderer->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());

    vtkNew<vtkRenderer> hybridMedianRenderer;
    hybridMedianRenderer->SetViewport(hybridMedianViewport);
    hybridMedianRenderer->AddActor(hybridMedianActor.Get());
    hybridMedianRenderer->ResetCamera();
    hybridMedianRenderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(600, 300);
    renderWindow->AddRenderer(originalRenderer.Get());
    renderWindow->AddRenderer(hybridMedianRenderer.Get());
    renderWindow->SetWindowName("ImageHybridMedian2D");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkNew<vtkInteractorStyleImage> style;

    renderWindowInteractor->SetInteractorStyle(style.Get());

    renderWindowInteractor->SetRenderWindow(renderWindow.Get());
    renderWindow->Render();
    renderWindowInteractor->Initialize();

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
