#include <array>

#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#define DISPLAY_FIRST_IMAGE
// Comment this out to display the first image.
#undef DISPLAY_FIRST_IMAGE

namespace
{
void CreateColorImage(vtkImageData *image, std::string const &colorName);
}

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    // Image 1
    vtkNew<vtkImageData> firstImage;
    CreateColorImage(firstImage.Get(), "DarkOliveGreen");

    vtkNew<vtkImageActor> firstImageActor;
    firstImageActor->SetInputData(firstImage.Get());

    // Image 2
    vtkNew<vtkImageData> secondImage;
    CreateColorImage(secondImage.Get(), "DarkSalmon");

    vtkNew<vtkImageActor> secondImageActor;
    secondImageActor->SetInputData(secondImage.Get());

    // Visualize
    vtkNew<vtkRenderer> renderer;

#ifdef DISPLAY_FIRST_IMAGE
    // The first image is displayed
    renderer->AddActor(secondImageActor.Get());
    renderer->AddActor(firstImageActor.Get());
#else
    // The second image is displayed
    renderer->AddActor(firstImageActor.Get());
    renderer->AddActor(secondImageActor.Get());
#endif

    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer.Get());
    renderWindow->SetWindowName("ImageOrder");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkNew<vtkInteractorStyleImage> style;

    renderWindowInteractor->SetInteractorStyle(style.Get());

    renderWindowInteractor->SetRenderWindow(renderWindow.Get());
    renderWindow->Render();
    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

namespace
{
void CreateColorImage(vtkImageData *image, std::string const &colorName)
{
    vtkNew<vtkNamedColors> colors;

    std::array<unsigned char, 3> drawColor {0, 0, 0};
    auto color = colors->GetColor3ub(colorName).GetData();
    for (auto i = 0; i < 3; ++i) {
        drawColor[i] = color[i];
    }

    unsigned int dim = 10;

    image->SetDimensions(dim, dim, 1);
    image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

    for (unsigned int x = 0; x < dim; x++) {
        for (unsigned int y = 0; y < dim; y++) {
            auto pixel = static_cast<unsigned char *>(image->GetScalarPointer(x, y, 0));
            for (auto i = 0; i < 3; ++i) {
                pixel[i] = drawColor[i];
            }
        }
    }

    image->Modified();
}
} // namespace
