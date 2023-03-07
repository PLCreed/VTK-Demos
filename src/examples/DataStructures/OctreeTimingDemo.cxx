#include <vector>

#include <time.h>
#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkIdList.h>
#include <vtkMath.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOctreePointLocator.h>
#include <vtkPlot.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTable.h>
#include <vtkTimerLog.h>
#include <vtkXMLPolyDataReader.h>

namespace
{
// void RandomPointInBounds(vtkPolyData* polydata, double p[3]);
void RandomPointInBounds(vtkPolyData *polydata, double p[3], vtkMinimalStandardRandomSequence *rng);

double TimeOctree(vtkPolyData *polydata, int maxPoints, int numberOfTrials, vtkMinimalStandardRandomSequence *rng);
} // namespace

int main(int, char *[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkSphereSource> reader;
    reader->SetThetaResolution(30);
    reader->SetPhiResolution(30);
    reader->Update();

    vtkNew<vtkMinimalStandardRandomSequence> rng;
    rng->SetSeed(8775070);
    // rng->SetSeed(0);

    std::cout << "Timing octree..." << std::endl;
    std::vector<std::pair<int, double>> results;
    int numberOfTrials = 1000;
    for (int i = 1; i < 100; i++) {
        double t = TimeOctree(reader->GetOutput(), i, numberOfTrials, rng.Get());
        std::pair<int, double> result(i, t);
        results.push_back(result);
    }

    // Create a table with some points in it
    vtkNew<vtkTable> table;

    vtkNew<vtkFloatArray> maxPointsPerRegion;
    maxPointsPerRegion->SetName("MaxPointsPerRegion");
    table->AddColumn(maxPointsPerRegion.Get());

    vtkNew<vtkFloatArray> runtime;
    runtime->SetName("Run time");
    table->AddColumn(runtime.Get());

    // Fill in the table with some example values
    size_t numPoints = results.size();
    table->SetNumberOfRows(static_cast<vtkIdType>(numPoints));
    for (size_t i = 0; i < numPoints; ++i) {
        table->SetValue(static_cast<vtkIdType>(i), 0, results[i].first);
        table->SetValue(static_cast<vtkIdType>(i), 1, results[i].second);
        std::cout << "Put " << results[i].first << " " << results[i].second << " in the table." << std::endl;
    }

    // Set up the view
    vtkNew<vtkContextView> view;
    view->GetRenderer()->SetBackground(colors->GetColor3d("White").GetData());
    view->GetRenderWindow()->SetWindowName("OctreeTimingDemo");

    // Add multiple line plots, setting the colors etc
    vtkNew<vtkChartXY> chart;
    view->GetScene()->AddItem(chart.Get());
    vtkPlot *line = chart->AddPlot(vtkChart::LINE);
    line->SetInputData(table.Get(), 0, 1);
    auto lineColor = colors->HTMLColorToRGBA("Lime").GetData();
    line->SetColor(lineColor[0], lineColor[1], lineColor[2], lineColor[3]);
    line->SetWidth(3.0);
    line->GetXAxis()->SetTitle("Max Points Per Region");
    line->GetYAxis()->SetTitle("Run time");

    // Set up an interactor and start
    view->Render();
    view->GetInteractor()->SetRenderWindow(view->GetRenderWindow());
    view->GetInteractor()->Initialize();
    view->GetInteractor()->Start();

    return EXIT_SUCCESS;
}

namespace
{
void RandomPointInBounds(vtkPolyData *polydata, double p[3], vtkMinimalStandardRandomSequence *rng)
{
    double bounds[6];
    polydata->GetBounds(bounds);

    for (auto i = 0; i < 3; ++i) {
        p[i] = bounds[i * 2] + (bounds[i * 2 + 1] - bounds[i * 2]) * rng->GetRangeValue(0.0, 1.0);
        rng->Next();
    }
}

double TimeOctree(vtkPolyData *polydata, int maxPoints, int numberOfTrials, vtkMinimalStandardRandomSequence *rng)
{
    vtkNew<vtkTimerLog> timer;
    timer->StartTimer();

    // Create the tree
    vtkNew<vtkOctreePointLocator> octree;
    octree->SetDataSet(polydata);
    octree->SetMaximumPointsPerRegion(maxPoints);
    octree->BuildLocator();

    std::cout << "With maxPoints = " << maxPoints << " there are " << octree->GetNumberOfLeafNodes() << " leaf nodes."
              << std::endl;

    for (int i = 0; i < numberOfTrials; i++) {
        double p[3];
        RandomPointInBounds(polydata, p, rng);
        // vtkIdType iD = octree->FindClosestPoint(p);
        octree->FindClosestPoint(p);
    }

    timer->StopTimer();

    std::cout << "Octree took " << timer->GetElapsedTime() << std::endl;

    return timer->GetElapsedTime();
}

} // namespace
