#include <vtkActor.h>
#include <vtkCellCenters.h>
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

    vtkNew<vtkCellCenters> cellCentersFilter;
    cellCentersFilter->SetInputData(imageData.Get());
    cellCentersFilter->VertexCellsOn();
    cellCentersFilter->Update();

    // Access the cell centers
    for (vtkIdType i = 0; i < cellCentersFilter->GetOutput()->GetNumberOfPoints(); i++) {
        double p[3];
        cellCentersFilter->GetOutput()->GetPoint(i, p);
        cout << "Point " << i << " : " << p[0] << " , " << p[1] << " , " << p[2] << endl;
    }

    // Display the cell centers
    vtkNew<vtkDataSetMapper> centerMapper;
    centerMapper->SetInputConnection(cellCentersFilter->GetOutputPort());
    vtkNew<vtkActor> centerActor;
    centerActor->SetMapper(centerMapper.Get());
    centerActor->GetProperty()->SetPointSize(5);
    centerActor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(imageData.Get());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper.Get());
    actor->GetProperty()->SetRepresentationToWireframe();
    actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

    vtkNew<vtkRenderWindow> renderWindow;

    vtkNew<vtkRenderer> renderer;

    renderWindow->AddRenderer(renderer.Get());
    renderWindow->SetWindowName("CellCenters");

    renderer->AddActor(actor.Get());
    renderer->AddActor(centerActor.Get());

    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
    renderer->ResetCamera();

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow.Get());
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
