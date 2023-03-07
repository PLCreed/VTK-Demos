#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractSelection.h>
#include <vtkIdTypeArray.h>
#include <vtkInformation.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkSphereSource.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVertexGlyphFilter.h>

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->Update();

    std::cout << "There are " << sphereSource->GetOutput()->GetNumberOfPoints() << " input points." << std::endl;
    std::cout << "There are " << sphereSource->GetOutput()->GetNumberOfCells() << " input cells." << std::endl;

    vtkNew<vtkIdTypeArray> ids;
    ids->SetNumberOfComponents(1);

    // Set values

    for (vtkIdType i = 0; i < sphereSource->GetOutput()->GetNumberOfPoints() - 15; i++) {
        ids->InsertNextValue(i);
    }

    vtkNew<vtkSelectionNode> selectionNode;
    selectionNode->SetFieldType(vtkSelectionNode::POINT);
    selectionNode->SetContentType(vtkSelectionNode::INDICES);
    selectionNode->SetSelectionList(ids.Get());
    selectionNode->GetProperties()->Set(vtkSelectionNode::CONTAINING_CELLS(), 1);

    vtkNew<vtkSelection> selection;
    selection->AddNode(selectionNode.Get());

    vtkNew<vtkExtractSelection> extractSelection;

    extractSelection->SetInputConnection(0, sphereSource->GetOutputPort());
    extractSelection->SetInputData(1, selection.Get());
    extractSelection->Update();

    // In selection
    vtkNew<vtkUnstructuredGrid> selected;
    selected->ShallowCopy(extractSelection->GetOutput());

    std::cout << "There are " << selected->GetNumberOfPoints() << " points in the selection." << std::endl;
    std::cout << "There are " << selected->GetNumberOfCells() << " cells in the selection." << std::endl;

    // Get points that are NOT in the selection
    selectionNode->GetProperties()->Set(vtkSelectionNode::INVERSE(),
                                        1); // invert the selection
    extractSelection->Update();

    vtkNew<vtkUnstructuredGrid> notSelected;
    notSelected->ShallowCopy(extractSelection->GetOutput());

    std::cout << "There are " << notSelected->GetNumberOfPoints() << " points NOT in the selection." << std::endl;
    std::cout << "There are " << notSelected->GetNumberOfCells() << " cells NOT in the selection." << std::endl;

    vtkNew<vtkProperty> backfaces;
    backfaces->SetColor(colors->GetColor3d("Gold").GetData());

    vtkNew<vtkDataSetMapper> inputMapper;
    inputMapper->SetInputConnection(sphereSource->GetOutputPort());
    vtkNew<vtkActor> inputActor;
    inputActor->SetMapper(inputMapper.Get());
    inputActor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    vtkNew<vtkDataSetMapper> selectedMapper;
    selectedMapper->SetInputData(selected.Get());

    vtkNew<vtkActor> selectedActor;
    selectedActor->SetMapper(selectedMapper.Get());
    selectedActor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());
    vtkNew<vtkProperty> backProperty;
    selectedActor->SetBackfaceProperty(backProperty.Get());
    selectedActor->SetBackfaceProperty(backfaces.Get());

    vtkNew<vtkDataSetMapper> notSelectedMapper;
    notSelectedMapper->SetInputData(notSelected.Get());

    vtkNew<vtkActor> notSelectedActor;
    notSelectedActor->SetMapper(notSelectedMapper.Get());
    notSelectedActor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());
    notSelectedActor->SetBackfaceProperty(backfaces.Get());

    // There will be one render window
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(900, 300);
    renderWindow->SetWindowName("ExtractCellsUsingPoints");

    // And one interactor
    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow.Get());

    // Define viewport ranges
    // (xmin, ymin, xmax, ymax)
    double leftViewport[4] = {0.0, 0.0, 0.33, 1.0};
    double centerViewport[4] = {0.33, 0.0, .66, 1.0};
    double rightViewport[4] = {0.66, 0.0, 1.0, 1.0};

    // Create a camera for all renderers
    vtkNew<vtkCamera> camera;

    // Setup the renderers
    vtkNew<vtkRenderer> leftRenderer;
    renderWindow->AddRenderer(leftRenderer.Get());
    leftRenderer->SetViewport(leftViewport);
    leftRenderer->SetBackground(colors->GetColor3d("BurlyWood").GetData());
    leftRenderer->SetActiveCamera(camera.Get());

    vtkNew<vtkRenderer> centerRenderer;
    renderWindow->AddRenderer(centerRenderer.Get());
    centerRenderer->SetViewport(centerViewport);
    centerRenderer->SetBackground(colors->GetColor3d("orchid_dark").GetData());
    centerRenderer->SetActiveCamera(camera.Get());

    vtkNew<vtkRenderer> rightRenderer;
    renderWindow->AddRenderer(rightRenderer.Get());
    rightRenderer->SetViewport(rightViewport);
    rightRenderer->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());
    rightRenderer->SetActiveCamera(camera.Get());

    leftRenderer->AddActor(inputActor.Get());
    centerRenderer->AddActor(selectedActor.Get());
    rightRenderer->AddActor(notSelectedActor.Get());

    leftRenderer->ResetCamera();

    renderWindow->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}
