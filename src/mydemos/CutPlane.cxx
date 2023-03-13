#include <vtkCellLocator.h>
#include <vtkCutter.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkGenericDataObjectWriter.h>
#include <vtkImageData.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkImageMapToColors.h>
#include <vtkImageStencil.h>
#include <vtkImageStencilToImage.h>
#include <vtkImplicitModeller.h>
#include <vtkLookupTable.h>
#include <vtkPNGWriter.h>
#include <vtkPlane.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkSampleFunction.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsGeometryFilter.h>
#include <vtkStructuredPointsReader.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

vtkSmartPointer<vtkImageData> polyDataToImageData(vtkPolyData *polyData, double *pixelSize)
{
    // 创建一个包含 vtkPolyData 的 vtkDataSetSurfaceFilter 过滤器对象
    // 将三维流场数据转换为表面网格，以便进行可视化和分析
    vtkSmartPointer<vtkDataSetSurfaceFilter> surfaceFilter = vtkSmartPointer<vtkDataSetSurfaceFilter>::New();
    surfaceFilter->SetInputData(polyData);
    surfaceFilter->Update();

    // 使用 vtkPolyData 的范围（即边界框）计算输出 vtkImageData 的范围
    double bounds[6];
    surfaceFilter->GetOutput()->GetBounds(bounds);

    std::cout << "surfaceFilter Bounds: " << bounds[0] << "," << bounds[1] << "," << bounds[2] << "," << bounds[3]
              << "," << bounds[4] << "," << bounds[5] << "," << std::endl;

    // 计算 vtkImageData 的分辨率和原点。分辨率可以根据所需像素大小和 vtkPolyData 的范围来计算。
    // 原点可以是 vtkPolyData的中心。
    double spacing[3];
    spacing[0] = pixelSize[0];
    spacing[1] = pixelSize[1];
    spacing[2] = pixelSize[2];
    int dimensions[3];
    for (int i = 0; i < 3; i++) {
        dimensions[i] = ceil((bounds[i * 2 + 1] - bounds[i * 2]) / pixelSize[i]);
    }
    double origin[3];
    origin[0] = (bounds[0] + bounds[1]) / 2.0;
    origin[1] = (bounds[2] + bounds[3]) / 2.0;
    origin[2] = (bounds[4] + bounds[5]) / 2.0;
    origin[0] -= (dimensions[0] * pixelSize[0]) / 2.0;
    origin[1] -= (dimensions[1] * pixelSize[1]) / 2.0;
    origin[2] -= (dimensions[2] * pixelSize[2]) / 2.0;

    // 创建 vtkImageData 对象并设置其范围、分辨率和原点
    vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
    imageData->SetDimensions(dimensions);
    imageData->SetSpacing(spacing);
    imageData->SetOrigin(origin);
    imageData->SetExtent(0, dimensions[0] - 1, 0, dimensions[1] - 1, 0, dimensions[2] - 1);

    // 使用 vtkPolyDataToImageStencil 过滤器将 vtkPolyData 转换为 vtkImageStencilData,
    // 然后使用 vtkImageStencil 过滤器将 vtkImageData 与 vtkImageStencilData 结合起来
    vtkSmartPointer<vtkPolyDataToImageStencil> polyToStencil = vtkSmartPointer<vtkPolyDataToImageStencil>::New();
    polyToStencil->SetInputConnection(surfaceFilter->GetOutputPort());
    polyToStencil->SetOutputSpacing(spacing);
    polyToStencil->SetOutputOrigin(origin);
    polyToStencil->SetInputData(polyData);
    polyToStencil->SetOutputWholeExtent(imageData->GetExtent());
    polyToStencil->Update();

    vtkSmartPointer<vtkImageStencil> stencil = vtkSmartPointer<vtkImageStencil>::New();
    stencil->SetInputData(imageData);
    stencil->SetStencilData(polyToStencil->GetOutput());
    stencil->ReverseStencilOn();
    stencil->Update();

    // Set output image data to the stencil output.
    imageData->ShallowCopy(stencil->GetOutput());

    return imageData;
}

vtkSmartPointer<vtkStructuredPoints> ConvertPolyDataToStructuredPoints(vtkPolyData *polyData)
{
    double *bounds = polyData->GetBounds();
    double xMin = bounds[0];
    double xMax = bounds[1];
    double yMin = bounds[2];
    double yMax = bounds[3];
    double zMin = bounds[4];
    double zMax = bounds[5];

    // Compute dimensions of the structured points
    vtkIdType numPts = polyData->GetNumberOfPoints();
    // Compute the optimal dimensions for the structured points
    // based on the number of points in the input data
    int dimX = 1;
    int dimY = 1;
    int dimZ = 1;
    while (dimX * dimY * dimZ < numPts) {
        if (dimX <= dimY && dimX <= dimZ) {
            dimX += 1;
        } else if (dimY <= dimX && dimY <= dimZ) {
            dimY += 1;
        } else {
            dimZ += 1;
        }
    }

    std::cout << "numPts: " << numPts << std::endl;
    std::cout << "dimX: " << dimX << ","
              << "dimY: " << dimX << ","
              << "dimZ: " << dimX << "," << std::endl;

    // Create structured points and set dimensions
    vtkSmartPointer<vtkStructuredPoints> structuredPoints = vtkSmartPointer<vtkStructuredPoints>::New();
    structuredPoints->SetDimensions(dimX, dimY, dimZ);
    structuredPoints->SetOrigin(xMin, yMin, zMin);
    structuredPoints->SetSpacing((xMax - xMin) / dimX, (yMax - yMin) / dimY, (zMax - zMin) / dimZ);

    // Allocate memory for point data
    structuredPoints->AllocateScalars(VTK_FLOAT, 1);

    // Set point data from the input polydata
    vtkSmartPointer<vtkDataArray> scalarArray = polyData->GetPointData()->GetScalars();
    for (vtkIdType i = 0; i < numPts; i++) {
        double *pt = polyData->GetPoint(i);
        int idX = int((pt[0] - xMin) / structuredPoints->GetSpacing()[0]);
        int idY = int((pt[1] - yMin) / structuredPoints->GetSpacing()[1]);
        int idZ = int((pt[2] - zMin) / structuredPoints->GetSpacing()[2]);
        double scalar = scalarArray->GetTuple1(i);
        structuredPoints->GetPointData()->GetScalars()->SetTuple(idZ * dimX * dimY + idY * dimX + idX, &scalar);
    }

    return structuredPoints;
}

class VTKImageSlice
{
public:
    VTKImageSlice(const std::string &filename);
    void getCutPlane(const double *origin, const double *normal, const std::string &outputFilename,
                     vtkLookupTable *lut = nullptr);

private:
    vtkSmartPointer<vtkStructuredPoints> structPointData_;
};

VTKImageSlice::VTKImageSlice(const std::string &filename)
{
    vtkSmartPointer<vtkStructuredPointsReader> reader = vtkSmartPointer<vtkStructuredPointsReader>::New();
    reader->SetFileName(filename.c_str());
    reader->Update();

    structPointData_ = reader->GetOutput();
}

void VTKImageSlice::getCutPlane(const double *origin, const double *normal, const std::string &outputFilename,
                                vtkLookupTable *lut)
{

    // 创建切割平面
    vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
    plane->SetOrigin(origin[0], origin[1], origin[2]);
    plane->SetNormal(normal[0], normal[1], normal[2]);

    // 创建切割器
    vtkSmartPointer<vtkCutter> cutter = vtkSmartPointer<vtkCutter>::New();
    cutter->SetInputData(structPointData_);
    cutter->SetCutFunction(plane);
    cutter->Update(); // 确保 cutPlane 包含有效数据。

    vtkPolyData *polyData = cutter->GetOutput();
    std::cout << "Bounds1: " << polyData->GetBounds()[0] << "," << polyData->GetBounds()[1] << ","
              << polyData->GetBounds()[2] << "," << polyData->GetBounds()[3] << "," << polyData->GetBounds()[4] << ","
              << polyData->GetBounds()[5] << "," << std::endl;
    vtkPoints *points = polyData->GetPoints();
    if (points) {
        std::cout << "Number of points: " << points->GetNumberOfPoints() << std::endl;

        // for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
        //     double p[3];
        //     points->GetPoint(i, p);
        //     std::cout << "Point: " << p[0] << ", " << p[1] << ", " << p[2] << std::endl;
        // }
    }

    // vtkSmartPointer<vtkImageData> cutPlane = polyDataToImageData(polyData, 1);
    // vtkSmartPointer<vtkStructuredPoints> cutPlane = ConvertPolyDataToStructuredPoints(polyData);

    {
        // create transform
        vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
        transform->PostMultiply();
        // transform->Translate(origin[0], origin[1], origin[2]);
        transform->RotateZ(vtkMath::DegreesFromRadians(atan2(normal[1], normal[0])));

        // create transform filter
        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
            vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        transformFilter->SetInputData(polyData);
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        // 输出切割数据到 VTK 文件
        vtkSmartPointer<vtkGenericDataObjectWriter> writer = vtkSmartPointer<vtkGenericDataObjectWriter>::New();
        writer->SetInputData(transformFilter->GetOutput());
        writer->SetFileName("/home/guobin/cut_plane_trans.vtk");
        writer->SetFileTypeToBinary();
        writer->Write();
    }

    // if (lut != nullptr) {
    //     vtkSmartPointer<vtkImageMapToColors> colorMapper = vtkSmartPointer<vtkImageMapToColors>::New();
    //     colorMapper->SetLookupTable(lut);
    //     colorMapper->SetInputData(cutPlane);
    //     colorMapper->Update();
    //     cutPlane = colorMapper->GetOutput();
    // }

    // 输出切割数据到 VTK 文件
    vtkSmartPointer<vtkGenericDataObjectWriter> writer = vtkSmartPointer<vtkGenericDataObjectWriter>::New();
    writer->SetInputData(polyData);
    writer->SetFileName(outputFilename.c_str());
    writer->SetFileTypeToBinary();
    writer->Write();
}

// 该类中的 `getCutPlane` 方法根据传入的平面原点和法向量，计算得到该平面在 vtk 数据集上的切面图片，并将其保存为 PNG
// 格式的图片文件。可选参数 `lut` 是 vtkLookupTable
// 类型的指针，用于指定色标表，如果不指定将使用默认的黑白色表。注意需要在使用切割器之前调用 `vtkCutter::SetInputData`
// 来设置输入数据集。
int main()
{
    std::string filename = "/home/guobin/AI.vtk";
    double origin[3] = {225000, 1215000, 47};                  // 切割平面的原点坐标
    double normal[3] = {1.0, 1.0, 0.0};                        // 切割平面的法向量
    std::string outputFilename = "/home/guobin/cut_plane.vtk"; // 切面图片的输出文件名
    vtkSmartPointer<vtkLookupTable> lut = nullptr;             // 色标表，可选
    VTKImageSlice reader(filename);
    reader.getCutPlane(origin, normal, outputFilename, lut);

    return 0;
}