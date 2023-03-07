#include <array>
#include <sstream>
#include <vector>

#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkCylinderSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

namespace
{
std::vector<vtkSmartPointer<vtkPolyDataAlgorithm>> GetSources();

// Define interaction style
class KeyPressInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static KeyPressInteractorStyle *New();
    vtkTypeMacro(KeyPressInteractorStyle, vtkInteractorStyleTrackballCamera);

    virtual void OnKeyPress()
    {
        // Get the keypress
        vtkRenderWindowInteractor *rwi = this->Interactor;
        std::string key = rwi->GetKeySym();

        if (key == "e" || key == "q" || key == "E" || key == "Q") {
            *status = false;
        }

        // Forward events
        vtkInteractorStyleTrackballCamera::OnKeyPress();
    }

public:
    bool *status = nullptr;
};

vtkStandardNewMacro(KeyPressInteractorStyle);

} // namespace

int main(int argc, char *argv[])
{
    auto simultaneousUpdate = false;
    if ((argc > 1) && (std::string(argv[1]) == "s" || std::string(argv[1]) == "S")) {
        simultaneousUpdate = true;
    }

    vtkNew<vtkNamedColors> colors;

    // Have some fun with colors
    std::vector<std::string> renBkg {"AliceBlue", "GhostWhite", "WhiteSmoke", "Seashell"};
    std::vector<std::string> actorColor {"Bisque", "RosyBrown", "Goldenrod", "Chocolate"};
    // Window sizes and spacing.
    auto width = 300;
    auto height = 300;
    // Add extra space around each window.
    auto dx = 20;
    auto dy = 40;
    auto w = width + dx;
    auto h = height + dy;

    std::vector<vtkSmartPointer<vtkRenderWindowInteractor>> interactors;
    std::vector<vtkSmartPointer<KeyPressInteractorStyle>> styles;
    std::array<bool, 4> running {true, true, true, true};

    vtkCamera *camera = nullptr;
    auto sources = GetSources();

    for (unsigned int i = 0; i < 4; i++) {
        vtkNew<vtkRenderWindow> renderWindow;
        renderWindow->SetSize(width, height);

        vtkNew<vtkRenderer> renderer;

        // Use the same initial camera for each renderer.
        if (i == 0) {
            camera = renderer->GetActiveCamera();
            camera->Azimuth(30);
            camera->Elevation(30);
        } else {
            renderer->SetActiveCamera(camera);
        }

        renderWindow->AddRenderer(renderer.Get());

        vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

        interactors.push_back(renderWindowInteractor.Get());

        renderWindowInteractor->SetRenderWindow(renderWindow.Get());
        renderWindow->Render();
        std::stringstream ss;
        ss << "MultipleRenderWindows " << i;
        renderWindow->SetWindowName(ss.str().c_str());
        renderWindow->SetPosition((i % 2) * w, h - (i / 2) * h);

        // Create a mapper and actor.
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputConnection(sources[i]->GetOutputPort());

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper.Get());
        actor->GetProperty()->SetColor(colors->GetColor3d(actorColor[i]).GetData());

        renderer->AddActor(actor.Get());
        renderer->SetBackground(colors->GetColor3d(renBkg[i]).GetData());

        renderer->ResetCamera();

        running[i] = true;
        vtkNew<KeyPressInteractorStyle> style;
        styles.push_back(style.Get());
        styles[i]->status = &running[i];
        interactors[i]->SetInteractorStyle(styles[i]);
        styles[i]->SetCurrentRenderer(renderer.Get());
    }

    // Start the interaction.
    if (simultaneousUpdate) {
        // Changes in any window will be simultaneously reflected in the other
        // windows.
        interactors[0]->Initialize();
        // If all are running then process the commands.
        while (std::all_of(running.begin(), running.end(),
                           [](bool i)
                           {
                               return i == true;
                           })) {
            for (unsigned int i = 0; i < 4; i++) {
                if (running[i]) {
                    // interactors[i]->ProcessEvents();
                    interactors[i]->Render();
                } else {
                    interactors[i]->TerminateApp();
                    std::cout << "Window " << i << " has stopped running." << std::endl;
                }
            }
        }
    } else {
        // Changes in any window will be reflected in the other
        // windows when you click in the window.
        interactors[0]->Start();
    }

    return EXIT_SUCCESS;
}

namespace
{
std::vector<vtkSmartPointer<vtkPolyDataAlgorithm>> GetSources()
{
    std::vector<vtkSmartPointer<vtkPolyDataAlgorithm>> sources;
    for (unsigned i = 0; i < 4; i++) {
        // Create a sphere.
        vtkNew<vtkSphereSource> sphere;
        sphere->SetCenter(0.0, 0.0, 0.0);
        sphere->Update();
        sources.push_back(sphere.Get());
        // Create a cone.
        vtkNew<vtkConeSource> cone;
        cone->SetCenter(0.0, 0.0, 0.0);
        cone->SetDirection(0, 1, 0);
        cone->Update();
        sources.push_back(cone.Get());
        // Create a cube.
        vtkNew<vtkCubeSource> cube;
        cube->SetCenter(0.0, 0.0, 0.0);
        cube->Update();
        sources.push_back(cube.Get());
        // Create a cylinder.
        vtkNew<vtkCylinderSource> cylinder;
        cylinder->SetCenter(0.0, 0.0, 0.0);
        cylinder->Update();
        sources.push_back(cylinder.Get());
    }
    return sources;
}

} // namespace
