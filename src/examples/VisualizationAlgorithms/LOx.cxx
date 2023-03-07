#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkLookupTable.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStreamTracer.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkTubeFilter.h>

//// LOx post CFD case study

int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " postxyz.bin postq.bin" << std::endl;
        return EXIT_FAILURE;
    }
    // read data
    //
    vtkNew<vtkMultiBlockPLOT3DReader> pl3d;
    pl3d->AutoDetectFormatOn();
    pl3d->SetXYZFileName(argv[1]);
    pl3d->SetQFileName(argv[2]);
    pl3d->SetScalarFunctionNumber(153);
    pl3d->SetVectorFunctionNumber(200);
    pl3d->Update();

    vtkStructuredGrid *sg = dynamic_cast<vtkStructuredGrid *>(pl3d->GetOutput()->GetBlock(0));

    vtkNew<vtkNamedColors> colors;

    // blue to red lut
    //
    vtkNew<vtkLookupTable> lut;
    lut->SetHueRange(0.667, 0.0);

    // computational planes
    vtkNew<vtkStructuredGridGeometryFilter> floorComp;
    floorComp->SetExtent(0, 37, 0, 75, 0, 0);
    floorComp->SetInputData(sg);
    floorComp->Update();

    vtkNew<vtkPolyDataMapper> floorMapper;
    floorMapper->SetInputConnection(floorComp->GetOutputPort());
    floorMapper->ScalarVisibilityOff();
    floorMapper->SetLookupTable(lut.Get());

    vtkNew<vtkActor> floorActor;
    floorActor->SetMapper(floorMapper.Get());
    floorActor->GetProperty()->SetRepresentationToWireframe();
    floorActor->GetProperty()->SetColor(colors->GetColor3d("Beige").GetData());
    floorActor->GetProperty()->SetLineWidth(2);

    vtkNew<vtkStructuredGridGeometryFilter> subFloorComp;

    subFloorComp->SetExtent(0, 37, 0, 15, 22, 22);
    subFloorComp->SetInputData(sg);

    vtkNew<vtkPolyDataMapper> subFloorMapper;
    subFloorMapper->SetInputConnection(subFloorComp->GetOutputPort());
    subFloorMapper->SetLookupTable(lut.Get());
    subFloorMapper->SetScalarRange(sg->GetScalarRange());

    vtkNew<vtkActor> subFloorActor;

    subFloorActor->SetMapper(subFloorMapper.Get());

    vtkNew<vtkStructuredGridGeometryFilter> subFloor2Comp;
    subFloor2Comp->SetExtent(0, 37, 60, 75, 22, 22);
    subFloor2Comp->SetInputData(sg);

    vtkNew<vtkPolyDataMapper> subFloor2Mapper;
    subFloor2Mapper->SetInputConnection(subFloor2Comp->GetOutputPort());
    subFloor2Mapper->SetLookupTable(lut.Get());
    subFloor2Mapper->SetScalarRange(sg->GetScalarRange());

    vtkNew<vtkActor> subFloor2Actor;

    subFloor2Actor->SetMapper(subFloor2Mapper.Get());

    vtkNew<vtkStructuredGridGeometryFilter> postComp;

    postComp->SetExtent(10, 10, 0, 75, 0, 37);
    postComp->SetInputData(sg);

    vtkNew<vtkPolyDataMapper> postMapper;
    postMapper->SetInputConnection(postComp->GetOutputPort());
    postMapper->SetLookupTable(lut.Get());
    postMapper->SetScalarRange(sg->GetScalarRange());

    vtkNew<vtkActor> postActor;
    postActor->SetMapper(postMapper.Get());
    postActor->GetProperty()->SetColor(colors->GetColor3d("Beige").GetData());

    vtkNew<vtkStructuredGridGeometryFilter> fanComp;
    fanComp->SetExtent(0, 37, 38, 38, 0, 37);
    fanComp->SetInputData(sg);

    vtkNew<vtkPolyDataMapper> fanMapper;
    fanMapper->SetInputConnection(fanComp->GetOutputPort());
    fanMapper->SetLookupTable(lut.Get());
    fanMapper->SetScalarRange(sg->GetScalarRange());

    vtkNew<vtkActor> fanActor;

    fanActor->SetMapper(fanMapper.Get());
    fanActor->GetProperty()->SetColor(colors->GetColor3d("Beige").GetData());

    // streamers
    //
    // spherical seed points
    vtkNew<vtkPointSource> rake;
    rake->SetCenter(-0.74, 0, 0.3);
    rake->SetNumberOfPoints(10);

    // a line of seed points
    vtkNew<vtkStructuredGridGeometryFilter> seedsComp;
    seedsComp->SetExtent(10, 10, 37, 39, 1, 27);
    seedsComp->SetInputData(sg);

    vtkNew<vtkStreamTracer> streamers;
    streamers->SetInputConnection(pl3d->GetOutputPort());

    // streamers SetSource [rake GetOutput]
    streamers->SetSourceConnection(seedsComp->GetOutputPort());
    streamers->SetMaximumPropagation(250);
    streamers->SetInitialIntegrationStep(.2);
    streamers->SetMinimumIntegrationStep(.01);
    streamers->SetIntegratorType(2);
    streamers->Update();

    vtkNew<vtkTubeFilter> tubes;
    tubes->SetInputConnection(streamers->GetOutputPort());
    tubes->SetNumberOfSides(8);
    tubes->SetRadius(0.08);
    tubes->SetVaryRadius(0);

    vtkNew<vtkPolyDataMapper> mapTubes;

    mapTubes->SetInputConnection(tubes->GetOutputPort());
    mapTubes->SetScalarRange(sg->GetScalarRange());

    vtkNew<vtkActor> tubesActor;
    tubesActor->SetMapper(mapTubes.Get());

    // outline
    vtkNew<vtkStructuredGridOutlineFilter> outline;
    outline->SetInputData(sg);

    vtkNew<vtkPolyDataMapper> outlineMapper;
    outlineMapper->SetInputConnection(outline->GetOutputPort());

    vtkNew<vtkActor> outlineActor;
    outlineActor->SetMapper(outlineMapper.Get());
    outlineActor->GetProperty()->SetColor(colors->GetColor3d("Beige").GetData());

    // Create graphics stuff
    vtkNew<vtkRenderer> ren1;
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren1.Get());

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin.Get());

    // Add the actors to the renderer, set the background and size
    //
    ren1->AddActor(outlineActor.Get());
    ren1->AddActor(floorActor.Get());
    // ren1->AddActor(subFloorActor.Get());
    // ren1->AddActor(subFloor2Actor.Get());
    ren1->AddActor(postActor.Get());
    // ren1->AddActor(fanActor.Get());
    ren1->AddActor(tubesActor.Get());

    vtkNew<vtkCamera> aCam;
    aCam->SetFocalPoint(2.47736, -0.150024, 2.42361);
    aCam->SetPosition(1.57547, -13.4601, 5.47872);
    aCam->SetViewUp(0.00197003, 0.223588, 0.974682);
    // aCam->Dolly(4.0);
    aCam->SetClippingRange(1, 100);

    ren1->SetBackground(colors->GetColor3d("SlateGray").GetData());
    ren1->SetActiveCamera(aCam.Get());
    renWin->SetSize(640, 480);
    renWin->SetWindowName("LOx");

    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}
