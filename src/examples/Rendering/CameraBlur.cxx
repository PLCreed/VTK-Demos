#include <array>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkConeSource.h>
#include <vtkGlyph3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    // Set the background color.
    std::array<unsigned char, 4> bkg {
        {26, 51, 102, 255}
    };
    colors->SetColor("Bkg", bkg.data());

    // Create the rendering objects.
    vtkNew<vtkRenderer> ren1;
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren1.Get());
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin.Get());

    // Create the pipeline, ball and spikes.
    vtkNew<vtkSphereSource> sphere;
    sphere->SetPhiResolution(7);
    sphere->SetThetaResolution(7);
    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(sphere->GetOutputPort());
    vtkNew<vtkActor> sphereActor;
    sphereActor->SetMapper(sphereMapper.Get());
    vtkNew<vtkActor> sphereActor2;
    sphereActor2->SetMapper(sphereMapper.Get());

    vtkNew<vtkConeSource> cone;
    cone->SetResolution(5);
    vtkNew<vtkGlyph3D> glyph;
    glyph->SetInputConnection(sphere->GetOutputPort());
    glyph->SetSourceConnection(cone->GetOutputPort());
    glyph->SetVectorModeToUseNormal();
    glyph->SetScaleModeToScaleByVector();
    glyph->SetScaleFactor(0.25);
    vtkNew<vtkPolyDataMapper> spikeMapper;
    spikeMapper->SetInputConnection(glyph->GetOutputPort());
    vtkNew<vtkActor> spikeActor;
    spikeActor->SetMapper(spikeMapper.Get());
    vtkNew<vtkActor> spikeActor2;
    spikeActor2->SetMapper(spikeMapper.Get());

    spikeActor->SetPosition(0, 0.7, 0);
    sphereActor->SetPosition(0, 0.7, 0);
    spikeActor2->SetPosition(0, -1.0, -10);
    sphereActor2->SetPosition(0, -1.0, -10);
    spikeActor2->SetScale(1.5, 1.5, 1.5);
    sphereActor2->SetScale(1.5, 1.5, 1.5);

    ren1->AddActor(sphereActor.Get());
    ren1->AddActor(spikeActor.Get());
    ren1->AddActor(sphereActor2.Get());
    ren1->AddActor(spikeActor2.Get());
    ren1->SetBackground(colors->GetColor3d("Bkg").GetData());
    renWin->SetSize(300, 300);
    renWin->SetWindowName("CameraBlur");
    // renWin->DoubleBufferOff();

    // Do the first render and then zoom in a little.
    renWin->Render();
    ren1->GetActiveCamera()->SetFocalPoint(0, 0, 0.0);
    ren1->GetActiveCamera()->Zoom(1.8);
    ren1->GetActiveCamera()->SetFocalDisk(0.05);

    renWin->Render();

    iren->Start();

    return EXIT_SUCCESS;
}
