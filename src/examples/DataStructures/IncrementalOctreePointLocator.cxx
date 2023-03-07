#include <vtkIdList.h>
#include <vtkIncrementalOctreePointLocator.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

int main(int, char *[])
{
    // Setup point coordinates
    double x[3] = {1.0, 0.0, 0.0};
    double y[3] = {0.0, 1.0, 0.0};
    double z[3] = {0.0, 0.0, 1.0};

    vtkNew<vtkPoints> points;
    points->InsertNextPoint(x);
    points->InsertNextPoint(y);
    points->InsertNextPoint(z);

    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points.Get());

    // Create the tree
    vtkNew<vtkIncrementalOctreePointLocator> octree;
    octree->SetDataSet(polydata.Get());
    octree->BuildLocator();

    double testPoint[3] = {2.0, 0.0, 0.0};

    {
        // Find the closest points to TestPoint
        //  double closestPointDist;
        vtkIdType iD = octree->FindClosestPoint(testPoint);
        std::cout << "The closest point is point " << iD << std::endl;

        // Get the coordinates of the closest point
        double closestPoint[3];
        octree->GetDataSet()->GetPoint(iD, closestPoint);
        std::cout << "Coordinates: " << closestPoint[0] << " " << closestPoint[1] << " " << closestPoint[2]
                  << std::endl;
    }

    // Insert a point
    double pnew[3] = {2.1, 0, 0};
    octree->InsertNextPoint(pnew);

    {
        // Find the closest points to TestPoint
        //  double closestPointDist;
        vtkIdType iD = octree->FindClosestPoint(testPoint);
        std::cout << "The closest point is point " << iD << std::endl;

        // Get the coordinates of the closest point
        double closestPoint[3];
        octree->GetDataSet()->GetPoint(iD, closestPoint);
        std::cout << "Coordinates: " << closestPoint[0] << " " << closestPoint[1] << " " << closestPoint[2]
                  << std::endl;
    }

    return EXIT_SUCCESS;
}
