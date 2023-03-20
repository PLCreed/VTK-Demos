#include <QApplication>
#include <QDebug>
#include <QImage>

#include <vtkCutter.h>
#include <vtkGenericDataObjectWriter.h>
#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include <vtkPlane.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsReader.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

#include "colormap.h"
#include "interval.h"
#include "plotspectrogram.h"
#include "rasterdata.h"

namespace
{

class SliceRasterData : public QwtMatrixRasterData
{
public:
    SliceRasterData()
    {
        std::string filename = "/home/guobin/AI.vtk";
        double origin[3] = {225000, 1215000, 47};                  // 切割平面的原点坐标
        double normal[3] = {1.0, 1.0, 0.0};                        // 切割平面的法向量
        std::string outputFilename = "/home/guobin/cut_plane.vtk"; // 切面图片的输出文件名

        vtkSmartPointer<vtkStructuredPointsReader> reader = vtkSmartPointer<vtkStructuredPointsReader>::New();
        reader->SetFileName(filename.c_str());
        reader->Update();
        vtkSmartPointer<vtkStructuredPoints> structPointData = reader->GetOutput();

        // 创建切割平面
        vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
        plane->SetOrigin(origin[0], origin[1], origin[2]);
        plane->SetNormal(normal[0], normal[1], normal[2]);

        // 创建切割器
        vtkSmartPointer<vtkCutter> cutter = vtkSmartPointer<vtkCutter>::New();
        cutter->SetInputData(structPointData);
        cutter->SetCutFunction(plane);
        cutter->Update(); // 确保 cutPlane 包含有效数据。

        vtkPolyData *polyData = cutter->GetOutput();
        std::cout << "Cut plane Bounds: " << polyData->GetBounds()[0] << "," << polyData->GetBounds()[1] << ","
                  << polyData->GetBounds()[2] << "," << polyData->GetBounds()[3] << "," << polyData->GetBounds()[4]
                  << "," << polyData->GetBounds()[5] << "," << std::endl;
        // vtkPoints *points = polyData->GetPoints();
        // if (points) {
        //     std::cout << "Number of points: " << points->GetNumberOfPoints() << std::endl;

        // // for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
        // //     double p[3];
        // //     points->GetPoint(i, p);
        // //     std::cout << "Point: " << p[0] << ", " << p[1] << ", " << p[2] << std::endl;
        // // }
        // }

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

        vtkPolyData *trnsPolyData = transformFilter->GetOutput();
        std::cout << "Transform plane Bounds: " << trnsPolyData->GetBounds()[0] << "," << trnsPolyData->GetBounds()[1]
                  << "," << trnsPolyData->GetBounds()[2] << "," << trnsPolyData->GetBounds()[3] << ","
                  << trnsPolyData->GetBounds()[4] << "," << trnsPolyData->GetBounds()[5] << "," << std::endl;
        // 获取标量数据
        vtkDataArray *scalars = trnsPolyData->GetPointData()->GetScalars();
        if (scalars) // 确认标量数据存在
        {
            double scalarMin = 0, scalarMax = 0;
            QVector<double> values;
            for (vtkIdType i = 0; i < scalars->GetNumberOfTuples(); i++) {
                double scalarValue = scalars->GetComponent(i, 0);
                // std::cout << "Scalar value at point " << i << " is " << scalarValue << std::endl;
                values.append(scalarValue);

                if (scalarValue < scalarMin) {
                    scalarMin = scalarValue;
                }
                if (scalarValue > scalarMax) {
                    scalarMax = scalarValue;
                }
            }

            qDebug() << scalarMin << scalarMax;

            int numRows = qAbs(trnsPolyData->GetBounds()[5] - trnsPolyData->GetBounds()[4]) + 1; // TODO
            int numColumns = scalars->GetSize() / numRows;

            qDebug() << numRows << numColumns;

            setValueMatrix(values, numColumns);

            setInterval(Qt::XAxis,
                        Interval(trnsPolyData->GetBounds()[0], trnsPolyData->GetBounds()[1], Interval::ExcludeMaximum));
            setInterval(Qt::YAxis,
                        Interval(trnsPolyData->GetBounds()[4], trnsPolyData->GetBounds()[5], Interval::ExcludeMaximum));
            setInterval(Qt::ZAxis, Interval(scalarMin, scalarMax));

        } else {
            std::cerr << "No scalar data found!" << std::endl;
        }
    }
};

} // namespace

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SliceRasterData *rasterData = new SliceRasterData();
    rasterData->setResampleMode(QwtMatrixRasterData::ResampleMode::BilinearInterpolation);

    LinearColorMap *lineColorMap = new LinearColorMap(Qt::darkBlue, Qt::darkRed);
    lineColorMap->addColorStop(0.1, Qt::blue);
    lineColorMap->addColorStop(0.3, Qt::cyan);
    lineColorMap->addColorStop(0.5, Qt::green);
    lineColorMap->addColorStop(0.6, Qt::yellow);
    lineColorMap->addColorStop(0.9, Qt::red);

    PlotSpectrogram *spectrogram = new PlotSpectrogram();
    spectrogram->setRenderThreadCount(4); // use system specific thread count
    spectrogram->setData(rasterData);
    spectrogram->setColorMap(lineColorMap);

    QList<double> contourLevels;
    for (double level = 0.1; level < 1.0; level += 0.1) {
        contourLevels += level;
    }
    spectrogram->setContourLevels(contourLevels);

    // spectrogram->setDisplayMode(PlotSpectrogram::ContourMode, true);
    // spectrogram->setDisplayMode(PlotSpectrogram::ImageMode, true);
    // spectrogram->setDefaultContourPen(true ? QPen(Qt::black, 0) : QPen(Qt::NoPen));

    const Interval xInterval = spectrogram->data()->interval(Qt::XAxis);
    const Interval yInterval = spectrogram->data()->interval(Qt::YAxis);
    const Interval zInterval = spectrogram->data()->interval(Qt::ZAxis);

    QRect rect(QPoint(0, 0), QSize(xInterval.width(), yInterval.width()));

    ScaleMap xMap;
    xMap.setScaleInterval(xInterval.minValue(), xInterval.maxValue());
    xMap.setPaintInterval(rect.left(), rect.right());
    ScaleMap yMap;
    yMap.setScaleInterval(yInterval.minValue(), yInterval.maxValue());
    yMap.setPaintInterval(rect.bottom(), rect.top());

    QImage image(rect.size(), QImage::Format_ARGB32);
    QPainter p;
    p.begin(&image);

    p.drawRect(rect);
    spectrogram->draw(&p, xMap, yMap, rect);

    p.end();

    image.save("../image.png");

    return 0;
}
