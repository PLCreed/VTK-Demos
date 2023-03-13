#include <vtkActor.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkCutter.h>
#include <vtkDataSetWriter.h>
#include <vtkGenericDataObjectWriter.h>
#include <vtkImageReslice.h>
#include <vtkPlane.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataWriter.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsReader.h>

int main()
{
    // 创建测试数据集
    // vtkSmartPointer<vtkStructuredPoints> data = vtkSmartPointer<vtkStructuredPoints>::New();
    // data->SetDimensions(10, 10, 10);
    // data->SetOrigin(0, 0, 0);
    // data->SetSpacing(1, 1, 1);
    // data->AllocateScalars(VTK_DOUBLE, 1);

    vtkSmartPointer<vtkStructuredPointsReader> reader = vtkSmartPointer<vtkStructuredPointsReader>::New();
    reader->SetFileName("/home/guobin/AI.vtk");
    reader->Update();

    vtkSmartPointer<vtkStructuredPoints> data = reader->GetOutput();

    double spacing[3];
    data->GetSpacing(spacing);
    std::cout << "spacing: " << spacing[0] << spacing[1] << spacing[2] << std::endl;

    // 将测试数据集放置在中心
    double bounds[6];
    data->GetBounds(bounds);
    double center[3] = {(bounds[1] + bounds[0]) / 2, (bounds[3] + bounds[2]) / 2, (bounds[5] + bounds[4]) / 2};

    // 创建切割平面
    vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
    plane->SetOrigin(center);
    plane->SetNormal(1, 0, 0);

    // 创建切割器
    vtkSmartPointer<vtkCutter> cutter = vtkSmartPointer<vtkCutter>::New();
    cutter->SetInputData(data);
    cutter->SetCutFunction(plane);
    cutter->Update();

    vtkPoints *points = cutter->GetOutput()->GetPoints();
    if (points) {
        std::cout << "Number of points: " << points->GetNumberOfPoints() << std::endl;
    }

    // // 清理多边形数据
    // vtkSmartPointer<vtkCleanPolyData> cleaner = vtkSmartPointer<vtkCleanPolyData>::New();
    // cleaner->SetInputData(cutter->GetOutput());
    // cleaner->Update();

    // 输出切割数据到 VTK 文件
    vtkSmartPointer<vtkGenericDataObjectWriter> writer = vtkSmartPointer<vtkGenericDataObjectWriter>::New();
    writer->SetInputData(cutter->GetOutput());
    writer->SetFileName("/home/guobin/output_cleaner.vtk");
    writer->SetFileTypeToBinary();
    writer->Write();

    // 显示切割后的数据
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(cutter->GetOutputPort());
    mapper->SetResolveCoincidentTopologyToPolygonOffset();

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);

    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 800);

    vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(renderWindow);
    renderWindow->Render();

    interactor->Start();

    return 0;
}