#include <array>

#include <vtkActor2D.h>
#include <vtkImageData.h>
#include <vtkImageMapper.h>
#include <vtkImageSlice.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace
{
void CreateColorImage(vtkImageData *);
}

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkImageData> colorImage;
    CreateColorImage(colorImage.Get());

    vtkNew<vtkImageMapper> imageMapper;
    imageMapper->SetInputData(colorImage.Get());
    imageMapper->SetColorWindow(255);
    imageMapper->SetColorLevel(127.5);

    vtkNew<vtkActor2D> imageActor;
    imageActor->SetMapper(imageMapper.Get());
    imageActor->SetPosition(20, 20);

    // Setup renderers
    vtkNew<vtkRenderer> renderer;

    // Setup render window
    vtkNew<vtkRenderWindow> renderWindow;

    renderWindow->AddRenderer(renderer.Get());
    renderWindow->SetWindowName("ImageMapper");

    // Setup render window interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

    vtkNew<vtkInteractorStyleImage> style;

    renderWindowInteractor->SetInteractorStyle(style.Get());

    // Render and start interaction
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());

    // renderer->AddViewProp(imageActor.Get());
    renderer->AddActor2D(imageActor.Get());
    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

namespace
{
void CreateColorImage(vtkImageData *image)
{
    vtkNew<vtkNamedColors> colors;

    std::array<unsigned char, 3> drawColor1 {0, 0, 0};
    std::array<unsigned char, 3> drawColor2 {0, 0, 0};
    auto color1 = colors->GetColor3ub("HotPink").GetData();
    auto color2 = colors->GetColor3ub("Chartreuse").GetData();
    for (auto i = 0; i < 3; ++i) {
        drawColor1[i] = color1[i];
        drawColor2[i] = color2[i];
    }

    unsigned int dim = 20;

    image->SetDimensions(dim, dim, 1);
    image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

    for (unsigned int x = 0; x < dim; x++) {
        for (unsigned int y = 0; y < dim; y++) {
            auto pixel = static_cast<unsigned char *>(image->GetScalarPointer(x, y, 0));
            if (x < dim / 2) {
                for (auto i = 0; i < 3; ++i) {
                    pixel[i] = drawColor1[i];
                }
            } else {
                for (auto i = 0; i < 3; ++i) {
                    pixel[i] = drawColor2[i];
                }
            }
        }
    }

    image->Modified();
}
} // namespace
