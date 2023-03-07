#include <sstream>

#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVertexGlyphFilter.h>

int main(int argc, char *argv[])
{
    vtkNew<vtkNamedColors> colors;

    // Verify input arguments
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " Filename(.txt) e.g. TeapotPoints.txt" << std::endl;
        return EXIT_FAILURE;
    }
    // Get all data from the file
    std::string filename = argv[1];
    std::ifstream filestream(filename.c_str());

    std::string line;
    vtkNew<vtkPoints> points;

    while (std::getline(filestream, line)) {
        double x, y, z;
        std::stringstream linestream;
        linestream << line;
        linestream >> x >> y >> z;

        points->InsertNextPoint(x, y, z);
    }

    filestream.close();

    vtkNew<vtkPolyData> polyData;

    polyData->SetPoints(points.Get());

    vtkNew<vtkVertexGlyphFilter> glyphFilter;
    glyphFilter->SetInputData(polyData.Get());
    glyphFilter->Update();

    // Visualize

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(glyphFilter->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper.Get());
    actor->GetProperty()->SetColor(colors->GetColor3d("MidnightBlue").GetData());

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor.Get());
    renderer->SetBackground(colors->GetColor3d("Gainsboro").GetData());

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer.Get());
    renderWindow->SetWindowName("ReadTextFile");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
