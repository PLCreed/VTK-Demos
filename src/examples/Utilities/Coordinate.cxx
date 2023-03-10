#include <vtkCoordinate.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

int main(int, char *[])
{
    vtkNew<vtkRenderWindow> rendererWindow;

    vtkNew<vtkRenderer> renderer;
    rendererWindow->AddRenderer(renderer.Get());
    rendererWindow->Render();

    vtkNew<vtkCoordinate> coordinate;
    coordinate->SetCoordinateSystemToNormalizedDisplay();
    coordinate->SetValue(.5, .5, 0);
    std::cout << *coordinate.Get() << std::endl;
    std::cout << coordinate->GetCoordinateSystemAsString() << std::endl;

    int *val;
    val = coordinate->GetComputedDisplayValue(renderer.Get());
    std::cout << "Val: " << val[0] << " " << val[1] << std::endl;

    return EXIT_SUCCESS;
}
