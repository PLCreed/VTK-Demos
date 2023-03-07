#include <vtkCellArray.h>
#include <vtkDataSetCollection.h>
#include <vtkIdList.h>
#include <vtkNew.h>
#include <vtkOctreePointLocator.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkXMLPolyDataReader.h>

int main(int, char *[])
{
    // Setup point coordinates
    double x[3] {1.0, 0.0, 0.0};
    double y[3] {0.0, 1.0, 0.0};
    double z[3] {0.0, 0.0, 1.0};

    vtkNew<vtkPoints> points;

    for (unsigned int i = 0; i < 3; ++i) {
        points->InsertNextPoint(x[i], y[i], z[i]);
    }

    std::cout << "There are " << points->GetNumberOfPoints() << " points." << std::endl;

    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points.Get());

    // Create the tree
    vtkNew<vtkOctreePointLocator> octree;
    octree->SetDataSet(polydata.Get());
    octree->BuildLocator();

    std::cout << "Number of points in tree: " << octree->GetDataSet()->GetNumberOfPoints() << std::endl;

    double p[3];
    octree->GetDataSet()->GetPoint(0, p);
    std::cout << "p: " << p[0] << " " << p[1] << " " << p[2] << std::endl;

    return EXIT_SUCCESS;
}
