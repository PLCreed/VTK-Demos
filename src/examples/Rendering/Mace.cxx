#include <vtkConeSource.h>
#include <vtkGlyph3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkRenderer> renderer;

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer.Get());

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin.Get());

    vtkNew<vtkSphereSource> sphere;
    sphere->SetThetaResolution(8);
    sphere->SetPhiResolution(8);

    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(sphere->GetOutputPort());

    vtkNew<vtkActor> sphereActor;
    sphereActor->SetMapper(sphereMapper.Get());
    sphereActor->GetProperty()->SetColor(colors->GetColor3d("Silver").GetData());

    vtkNew<vtkConeSource> cone;
    cone->SetResolution(6);

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
    spikeActor->GetProperty()->SetColor(colors->GetColor3d("Silver").GetData());

    renderer->AddActor(sphereActor.Get());
    renderer->AddActor(spikeActor.Get());
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
    renWin->SetSize(640, 480);
    renWin->SetWindowName("Mace");

    // interact with data
    renWin->Render();

    iren->Start();

    return EXIT_SUCCESS;
}
