#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkStructuredGridReader.h>

int main(int argc, char *argv[])
{
    vtkNew<vtkNamedColors> colors;

    // Verify input arguments
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " Filename(.vtk) e.g. SampleStructGrid.vtk" << std::endl;
        return EXIT_FAILURE;
    }

    std::string inputFilename = argv[1];

    // Read the file
    vtkNew<vtkStructuredGridReader> reader;
    reader->SetFileName(inputFilename.c_str());
    reader->Update();

    vtkNew<vtkStructuredGridGeometryFilter> geometryFilter;
    geometryFilter->SetInputConnection(reader->GetOutputPort());
    geometryFilter->Update();

    // Visualize
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(geometryFilter->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper.Get());

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer.Get());
    renderWindow->SetWindowName("StructuredGridReader");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());

    renderer->AddActor(actor.Get());
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
