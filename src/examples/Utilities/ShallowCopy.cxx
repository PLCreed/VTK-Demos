#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

int main(int, char *[])
{
    vtkNew<vtkPolyData> polydata;

    vtkNew<vtkPolyData> polydataCopy;

    polydataCopy->ShallowCopy(polydata.Get());

    return EXIT_SUCCESS;
}
