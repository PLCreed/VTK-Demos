#include <vtkActor.h>
#include <vtkDiskSource.h>
#include <vtkFeatureEdges.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkDiskSource> diskSource;
    diskSource->Update();

    vtkNew<vtkFeatureEdges> featureEdges;
    featureEdges->SetInputConnection(diskSource->GetOutputPort());
    featureEdges->BoundaryEdgesOn();
    featureEdges->FeatureEdgesOff();
    featureEdges->ManifoldEdgesOff();
    featureEdges->NonManifoldEdgesOff();
    featureEdges->ColoringOn();
    featureEdges->Update();

    // Visualize
    vtkNew<vtkPolyDataMapper> edgeMapper;
    edgeMapper->SetInputConnection(featureEdges->GetOutputPort());
    edgeMapper->SetScalarModeToUseCellData();
    vtkNew<vtkActor> edgeActor;
    edgeActor->SetMapper(edgeMapper.Get());

    vtkNew<vtkPolyDataMapper> diskMapper;
    diskMapper->SetInputConnection(diskSource->GetOutputPort());
    vtkNew<vtkActor> diskActor;
    diskActor->SetMapper(diskMapper.Get());
    diskActor->GetProperty()->SetColor(colors->GetColor3d("Gray").GetData());

    // Create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer.Get());
    renderWindow->SetWindowName("BoundaryEdges");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());

    renderer->AddActor(edgeActor.Get());
    renderer->AddActor(diskActor.Get());
    renderer->SetBackground(colors->GetColor3d("DimGray").GetData());

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
