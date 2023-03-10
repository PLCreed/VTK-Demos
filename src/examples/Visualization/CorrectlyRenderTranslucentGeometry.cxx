#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkCamera.h>
#include <vtkDepthSortPolyData.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTimerLog.h>
#include <vtkTransform.h>

namespace
{

/**
 * Generate a bunch of overlapping spheres within one poly data set:
 * one big sphere evenly surrounded by four small spheres that intersect the
 * centered sphere.
 * @param theta sphere sampling resolution (THETA)
 * @param phi sphere sampling resolution (PHI)
 * @return the set of spheres within one logical poly data set
 **/
vtkSmartPointer<vtkAppendPolyData> GenerateOverlappingBunchOfSpheres(int theta, int phi);

/**
 * Setup the rendering environment for depth peeling (general depth peeling
 * support is requested).
 * @see IsDepthPeelingSupported()
 * @param renderWindow a valid openGL-supporting render window
 * @param renderer a valid renderer instance
 * @param maxNoOfPeels maximum number of depth peels (multi-pass rendering)
 * @param occulusionRation the occlusion ration (0.0 means a perfect image,
 * >0.0 means a non-perfect image which in general results in faster rendering)
 * @return TRUE if depth peeling could be set up
 */
bool SetupEnvironmentForDepthPeeling(vtkRenderWindow *renderWindow, vtkRenderer *renderer, int maxNoOfPeels,
                                     double occlusionRatio);

/**
 * Find out whether this box supports depth peeling. Depth peeling requires
 * a variety of openGL extensions and appropriate drivers.
 * @param renderWindow a valid openGL-supporting render window
 * @param renderer a valid renderer instance
 * @param doItOffscreen do the test off screen which means that nothing is
 * rendered to screen (this requires the box to support off screen rendering)
 * @return TRUE if depth peeling is supported, FALSE otherwise (which means
 * that another strategy must be used for correct rendering of translucent
 * geometry, e.g. CPU-based depth sorting)
 */
bool IsDepthPeelingSupported(vtkRenderWindow *renderWindow, vtkRenderer *renderer, bool doItOffScreen);
} // namespace

/**
 * Example application demonstrating correct rendering of translucent geometry.
 * It will automatically detect whether depth peeling is supported by the
 * hardware and software, and will apply depth peeling if possible. Otherwise
 * a fallback strategy is used: depth sorting on the CPU.
 * <br>Usage:
 * [ProgramName] Theta Phi MaximumPeels OcclusionRatio ForceDepthSortingFlag
 * DoNotUseAnyDepthRelatedAlgorithmFlag
 * <br>
 * Theta ... spheres' THETA resolution <br>
 * Phi ... spheres' PHI resolution <br>
 * MaximumPeels ... maximum number of depth peels (multi-pass rendering) for
 * depth peeling mode <br>
 * OcclusionRatio ... occlusion ratio for depth peeling mode (0.0 for a
 * perfect rendered image, >0.0 for a non-perfect image which is expected to
 * be slower) <br>
 * ForceDepthSortingFlag ... force depth sorting even if depth peeling is
 * supported <br>
 * DoNotUseAnyDepthRelatedAlgorithmFlag ... neither use depth peeling nor
 * depth sorting - just render as usual
 */
int main(int argc, char *argv[])
{
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] << " Theta Phi MaximumPeels "
                  << "OcclusionRatio ForceDepthSortingFlag "
                  << "DoNotUseAnyDepthRelatedAlgorithmFlag" << std::endl;
        std::cerr << "100 100 50 0.1 0 0" << std::endl;
        return EXIT_FAILURE;
    }

    vtkNew<vtkNamedColors> colors;

    int theta = atoi(argv[1]);
    int phi = atoi(argv[2]);
    int maxPeels = atoi(argv[3]);
    double occulusionRatio = atof(argv[4]);
    bool forceDepthSort = atoi(argv[5]) == 1;
    bool withoutAnyDepthThings = atoi(argv[6]) == 1;

    // Generate a translucent sphere poly data set that partially overlaps:
    auto translucentGeometry = GenerateOverlappingBunchOfSpheres(theta, phi);

    // generate a basic Mapper and Actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(translucentGeometry->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper.Get());
    actor->GetProperty()->SetOpacity(0.5); // translucent !!!
    actor->GetProperty()->SetColor(colors->GetColor3d("Crimson").GetData());
    actor->RotateX(-72); // put the objects in a position where it is easy to see
                         // different overlapping regions

    // Create the RenderWindow, Renderer and RenderWindowInteractor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(600, 400);
    renderWindow->AddRenderer(renderer.Get());
    renderWindow->SetWindowName("CorrectlyRenderTranslucentGeometry");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());

    // Add the actors to the renderer, set the background and size
    renderer->AddActor(actor.Get());
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    // Setup view geometry
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Zoom(2.2); // so the object is larger
    renderWindow->Render();

    // Answer the key question: Does this box support GPU Depth Peeling?
    bool useDepthPeeling = IsDepthPeelingSupported(renderWindow.Get(), renderer.Get(), true);
    std::cout << "DEPTH PEELING SUPPORT: " << (useDepthPeeling ? "YES" : "NO") << std::endl;

    int success = EXIT_SUCCESS;

    // Use depth peeling if available and not explicitly prohibited, otherwise we
    // use manual depth sorting
    std::cout << std::endl << "CHOSEN MODE: ";
    if (useDepthPeeling && !forceDepthSort && !withoutAnyDepthThings) // GPU
    {
        std::cout << "*** DEPTH PEELING ***" << std::endl;
        // Setup GPU depth peeling with configured parameters
        success = !SetupEnvironmentForDepthPeeling(renderWindow.Get(), renderer.Get(), maxPeels, occulusionRatio);
    } else if (!withoutAnyDepthThings) // CPU
    {
        std::cout << "*** DEPTH SORTING ***" << std::endl;
        // Setup CPU depth sorting filter
        vtkNew<vtkDepthSortPolyData> depthSort;
        depthSort->SetInputConnection(translucentGeometry->GetOutputPort());
        depthSort->SetDirectionToBackToFront();
        depthSort->SetVector(1, 1, 1);
        depthSort->SetCamera(renderer->GetActiveCamera());
        depthSort->SortScalarsOff(); // do not really need this here
        // Bring it to the mapper's input
        mapper->SetInputConnection(depthSort->GetOutputPort());
        depthSort->Update();
    } else {
        std::cout << "*** NEITHER DEPTH PEELING NOR DEPTH SORTING ***" << std::endl;
    }

    // Initialize interaction
    renderWindowInteractor->Initialize();

    // Check the average frame rate when rotating the actor
    int endCount = 100;
    vtkNew<vtkTimerLog> clock;
    // Set a user transform for successively rotating the camera position
    vtkNew<vtkTransform> transform;
    transform->Identity();
    transform->RotateY(2.0); // rotate 2 degrees around Y-axis at each iteration
    auto camera = renderer->GetActiveCamera();
    double camPos[3]; // camera position
    // Start test
    clock->StartTimer();
    for (int i = 0; i < endCount; i++) {
        camera->GetPosition(camPos);
        transform->TransformPoint(camPos, camPos);
        camera->SetPosition(camPos);
        renderWindow->Render();
    }
    clock->StopTimer();
    double frameRate = (double)endCount / clock->GetElapsedTime();
    std::cout << "AVERAGE FRAME RATE: " << frameRate << " fps" << std::endl;

    // Start interaction
    renderWindowInteractor->Start();

    return success;
}

namespace
{

vtkSmartPointer<vtkAppendPolyData> GenerateOverlappingBunchOfSpheres(int theta, int phi)
{
    vtkSmartPointer<vtkAppendPolyData> appendData = vtkSmartPointer<vtkAppendPolyData>::New();

    for (int i = 0; i < 5; i++) {
        vtkNew<vtkSphereSource> sphereSource;
        sphereSource->SetThetaResolution(theta);
        sphereSource->SetPhiResolution(phi);
        sphereSource->SetRadius(0.5); // all spheres except the center
                                      // one should have radius = 0.5
        switch (i) {
        case 0 :
            sphereSource->SetRadius(1);
            sphereSource->SetCenter(0, 0, 0);
            break;
        case 1 :
            sphereSource->SetCenter(1, 0, 0);
            break;
        case 2 :
            sphereSource->SetCenter(-1, 0, 0);
            break;
        case 3 :
            sphereSource->SetCenter(0, 1, 0);
            break;
        case 4 :
            sphereSource->SetCenter(0, -1, 0);
            break;
        }
        sphereSource->Update();
        appendData->AddInputConnection(sphereSource->GetOutputPort());
    }

    return appendData;
}

bool SetupEnvironmentForDepthPeeling(vtkRenderWindow *renderWindow, vtkRenderer *renderer, int maxNoOfPeels,
                                     double occlusionRatio)
{
    if (!renderWindow || !renderer) {
        return false;
    }

    // 1. Use a render window with alpha bits (as initial value is 0 (false)):
    renderWindow->SetAlphaBitPlanes(true);

    // 2. Force to not pick a framebuffer with a multisample buffer
    // (as initial value is 8):
    renderWindow->SetMultiSamples(0);

    // 3. Choose to use depth peeling (if supported) (initial value is 0 (false)):
    renderer->SetUseDepthPeeling(true);

    // 4. Set depth peeling parameters
    // - Set the maximum number of rendering passes (initial value is 4):
    renderer->SetMaximumNumberOfPeels(maxNoOfPeels);
    // - Set the occlusion ratio (initial value is 0.0, exact image):
    renderer->SetOcclusionRatio(occlusionRatio);

    return true;
}

bool IsDepthPeelingSupported(vtkRenderWindow *renderWindow, vtkRenderer *renderer, bool doItOffScreen)
{

    bool success = true;

    // Save original renderer / render window state
    bool origOffScreenRendering = renderWindow->GetOffScreenRendering() == 1;
    bool origAlphaBitPlanes = renderWindow->GetAlphaBitPlanes() == 1;
    int origMultiSamples = renderWindow->GetMultiSamples();
    bool origUseDepthPeeling = renderer->GetUseDepthPeeling() == 1;
    int origMaxPeels = renderer->GetMaximumNumberOfPeels();
    double origOcclusionRatio = renderer->GetOcclusionRatio();

    // Activate off screen rendering on demand
    renderWindow->SetOffScreenRendering(doItOffScreen);

    // Setup environment for depth peeling (with some default parametrization)
    success = success && SetupEnvironmentForDepthPeeling(renderWindow, renderer, 100, 0.1);

    // Do a test render
    renderWindow->Render();

    // Check whether depth peeling was used
    success = success && renderer->GetLastRenderingUsedDepthPeeling();

    // recover original state
    renderWindow->SetOffScreenRendering(origOffScreenRendering);
    renderWindow->SetAlphaBitPlanes(origAlphaBitPlanes);
    renderWindow->SetMultiSamples(origMultiSamples);
    renderer->SetUseDepthPeeling(origUseDepthPeeling);
    renderer->SetMaximumNumberOfPeels(origMaxPeels);
    renderer->SetOcclusionRatio(origOcclusionRatio);

    return success;
}

} // namespace
