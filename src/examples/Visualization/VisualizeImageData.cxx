#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkImageData.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    // Create an image data
    vtkNew<vtkImageData> imageData;

    // Specify the size of the image data
    imageData->SetDimensions(3, 3, 2);
    imageData->SetSpacing(1.0, 1.0, 1.0);
    imageData->SetOrigin(0.0, 0.0, 0.0);

    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(imageData.Get());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper.Get());
    actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    vtkNew<vtkRenderWindow> renderWindow;

    vtkNew<vtkRenderer> renderer;

    // Add both renderers to the window
    renderWindow->AddRenderer(renderer.Get());
    renderWindow->SetWindowName("VisualizeImageData");

    // Add a sphere to the left and a cube to the right
    renderer->AddActor(actor.Get());
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    renderWindow->Render();
    renderer->GetActiveCamera()->Azimuth(-30);
    renderer->GetActiveCamera()->Elevation(30);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
