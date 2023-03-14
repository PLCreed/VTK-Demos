#ifndef PLOT_SPECTROGRAM_H
#define PLOT_SPECTROGRAM_H

#include "plotrasteritembase.h"
#include "rasterdata.h"

class ColorMap;
template <typename T>
class QList;

/*!
   \brief A plot item, which displays a spectrogram

   A spectrogram displays 3-dimensional data, where the 3rd dimension
   ( the intensity ) is displayed using colors. The colors are calculated
   from the values using a color map.

   On multi-core systems the performance of the image composition
   can often be improved by dividing the area into tiles - each of them
   rendered in a different thread ( see QwtPlotItem::setRenderThreadCount() ).

   In ContourMode contour lines are painted for the contour levels.

   \sa RasterData, ColorMap, QwtPlotItem::setRenderThreadCount()
 */

class PlotSpectrogram : public PlotRasterItemBase
{
public:
    /*!
       The display mode controls how the raster data will be represented.
       \sa setDisplayMode(), testDisplayMode()
     */

    enum DisplayMode
    {
        // ! The values are mapped to colors using a color map.
        ImageMode = 0x01,

        // ! The data is displayed using contour lines
        ContourMode = 0x02
    };

    Q_DECLARE_FLAGS(DisplayModes, DisplayMode)

    explicit PlotSpectrogram(const QString &title = QString());
    virtual ~PlotSpectrogram();

    void setDisplayMode(DisplayMode, bool on = true);
    bool testDisplayMode(DisplayMode) const;

    void setData(RasterData *data);
    const RasterData *data() const;
    RasterData *data();

    void setColorMap(ColorMap *);
    const ColorMap *colorMap() const;

    void setColorTableSize(int numColors);
    int colorTableSize() const;

    virtual Interval interval(Qt::Axis) const override;
    virtual QRectF pixelHint(const QRectF &) const override;

    void setDefaultContourPen(const QColor &, qreal width = 0.0, Qt::PenStyle = Qt::SolidLine);
    void setDefaultContourPen(const QPen &);
    QPen defaultContourPen() const;

    virtual QPen contourPen(double level) const;

    void setConrecFlag(RasterData::ConrecFlag, bool on);
    bool testConrecFlag(RasterData::ConrecFlag) const;

    void setContourLevels(const QList<double> &);
    QList<double> contourLevels() const;

    virtual void draw(QPainter *, const ScaleMap &xMap, const ScaleMap &yMap, const QRectF &canvasRect) const override;

protected:
    virtual QImage renderImage(const ScaleMap &xMap, const ScaleMap &yMap, const QRectF &area,
                               const QSize &imageSize) const override;

    virtual QSize contourRasterSize(const QRectF &, const QRect &) const;

    virtual RasterData::ContourLines renderContourLines(const QRectF &rect, const QSize &raster) const;

    virtual void drawContourLines(QPainter *, const ScaleMap &xMap, const ScaleMap &yMap,
                                  const RasterData::ContourLines &) const;

    void renderTile(const ScaleMap &xMap, const ScaleMap &yMap, const QRect &tile, QImage *) const;

private:
    class PrivateData;
    PrivateData *m_data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PlotSpectrogram::DisplayModes)

#endif
