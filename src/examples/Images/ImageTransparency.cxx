#include <array>

#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkImageMapper3D.h>
#include <vtkInteractorStyleImage.h>
#include <vtkJPEGReader.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace
{
void CreateColorImage(vtkImageData *image, std::string const &colorNameunsigned, unsigned char const alpha);
}

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkImageData> image;
    // Use this to control the transparency [0 .. 255]
    unsigned char alpha = 50;
    CreateColorImage(image.Get(), "Gold", alpha);

    // Create actor
    vtkNew<vtkImageActor> imageActor;
    imageActor->GetMapper()->SetInputData(image.Get());

    // Visualize
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(imageActor.Get());
    renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer.Get());
    renderWindow->SetWindowName("ImageTransparency");

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
void CreateColorImage(vtkImageData *image, std::string const &colorName, unsigned char const alpha)
{
    vtkNew<vtkNamedColors> colors;

    std::array<unsigned char, 4> drawColor {0, 0, 0};
    auto color = colors->GetColor4ub(colorName).GetData();
    for (auto i = 0; i < 4; ++i) {
        drawColor[i] = color[i];
    }

    unsigned int dim = 10;

    // Specify the size of the image data
    image->SetDimensions(dim, dim, 1);
    image->AllocateScalars(VTK_UNSIGNED_CHAR, 4);

    int *dims = image->GetDimensions();

    std::cout << "Dims: "
              << " x: " << dims[0] << " y: " << dims[1] << " z: " << dims[2] << std::endl;

    for (unsigned char y = 0; y < dims[1]; y++) {
        for (unsigned char x = 0; x < dims[0]; x++) {
            auto pixel = static_cast<unsigned char *>(image->GetScalarPointer(x, y, 0));
            for (auto i = 0; i < 4; ++i) {
                pixel[i] = drawColor[i];
            }
            if (x < 5) {
                pixel[3] = alpha;
            } else {
                pixel[3] = 255;
            }
        }
    }

    image->Modified();
}
} // namespace
