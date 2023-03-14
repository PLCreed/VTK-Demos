﻿#include "plotspectrogram.h"

#include <QPaintEngine>
#include <QtConcurrentRun>

#include <qfuture.h>
#include <qimage.h>
#include <qmath.h>
#include <qpainter.h>
#include <qpen.h>
#include <qthread.h>

#include "colormap.h"
#include "interval.h"
#include "scalemap.h"

#define DEBUG_RENDER 0

#if DEBUG_RENDER
#include <qelapsedtimer.h>
#endif

#include <algorithm>

static inline bool qwtIsNaN(double d)
{
    // qt_is_nan is private header and qIsNaN is not inlined
    // so we need these code here too

    const uchar *ch = (const uchar *)&d;
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
        return (ch[0] & 0x7f) == 0x7f && ch[1] > 0xf0;
    } else {
        return (ch[7] & 0x7f) == 0x7f && ch[6] > 0xf0;
    }
}

class PlotSpectrogram::PrivateData
{
public:
    PrivateData() : data(NULL), colorTableSize(0)
    {
        colorMap = new LinearColorMap();
        displayMode = ImageMode;

        conrecFlags = RasterData::IgnoreAllVerticesOnLevel;
#if 0
        conrecFlags |= RasterData::IgnoreOutOfRange;
#endif
    }

    ~PrivateData()
    {
        delete data;
        delete colorMap;
    }

    void updateColorTable()
    {
        if (colorMap->format() == ColorMap::Indexed) {
            colorTable = colorMap->colorTable256();
        } else {
            if (colorTableSize == 0) {
                colorTable.clear();
            } else {
                colorTable = colorMap->colorTable(colorTableSize);
            }
        }
    }

    RasterData *data;
    ColorMap *colorMap;
    DisplayModes displayMode;

    QList<double> contourLevels;
    QPen defaultContourPen;
    RasterData::ConrecFlags conrecFlags;

    int colorTableSize;
    QVector<QRgb> colorTable;
};

/*!
   Sets the following item attributes:
   - QwtPlotItem::AutoScale: true
   - QwtPlotItem::Legend:    false

   The z value is initialized by 8.0.

   \param title Title

   \sa QwtPlotItem::setItemAttribute(), QwtPlotItem::setZ()
 */
PlotSpectrogram::PlotSpectrogram(const QString &title) : PlotRasterItemBase(title)
{
    m_data = new PrivateData();
}

// ! Destructor
PlotSpectrogram::~PlotSpectrogram()
{
    delete m_data;
}

/*!
   The display mode controls how the raster data will be represented.

   \param mode Display mode
   \param on On/Off

   The default setting enables ImageMode.

   \sa DisplayMode, displayMode()
 */
void PlotSpectrogram::setDisplayMode(DisplayMode mode, bool on)
{
    if (on != bool(mode & m_data->displayMode)) {
        if (on) {
            m_data->displayMode |= mode;
        } else {
            m_data->displayMode &= ~mode;
        }
    }
}

/*!
   The display mode controls how the raster data will be represented.

   \param mode Display mode
   \return true if mode is enabled
 */
bool PlotSpectrogram::testDisplayMode(DisplayMode mode) const
{
    return (m_data->displayMode & mode);
}

/*!
   Change the color map

   Often it is useful to display the mapping between intensities and
   colors as an additional plot axis, showing a color bar.

   \param colorMap Color Map

   \sa colorMap(), QwtScaleWidget::setColorBarEnabled(),
      QwtScaleWidget::setColorMap()
 */
void PlotSpectrogram::setColorMap(ColorMap *colorMap)
{
    if (colorMap == NULL) {
        return;
    }

    if (colorMap != m_data->colorMap) {
        delete m_data->colorMap;
        m_data->colorMap = colorMap;
    }

    m_data->updateColorTable();

    invalidateCache();
}

/*!
   \return Color Map used for mapping the intensity values to colors
   \sa setColorMap()
 */
const ColorMap *PlotSpectrogram::colorMap() const
{
    return m_data->colorMap;
}

/*!
    Limit the number of colors being used by the color map

    When using a color table the mapping from the value into a color
    is usually faster as it can be done by simple lookups into a
    precalculated color table.

    Setting a table size > 0 enables using a color table, while setting
    the size to 0 disables it.

    The default size = 0, and no color table is used.

    \param numColors Number of colors. 0 means not using a color table
    \note The colorTableSize has no effect when using a color table
          of ColorMap::Indexed, where the size is always 256.


    \sa ColorMap::colorTable(), colorTableSize()
 */
void PlotSpectrogram::setColorTableSize(int numColors)
{
    numColors = qMax(numColors, 0);
    if (numColors != m_data->colorTableSize) {
        m_data->colorTableSize = numColors;
        m_data->updateColorTable();
        invalidateCache();
    }
}

/*!
    \return Size of the color table, 0 means not using a color table
    \sa ColorMap::colorTable(), setColorTableSize()
 */
int PlotSpectrogram::colorTableSize() const
{
    return m_data->colorTableSize;
}

/*!
   Build and assign the default pen for the contour lines

   In Qt5 the default pen width is 1.0 ( 0.0 in Qt4 ) what makes it
   non cosmetic ( see QPen::isCosmetic() ). This method has been introduced
   to hide this incompatibility.

   \param color Pen color
   \param width Pen width
   \param style Pen style

   \sa pen(), brush()
 */
void PlotSpectrogram::setDefaultContourPen(const QColor &color, qreal width, Qt::PenStyle style)
{
    setDefaultContourPen(QPen(color, width, style));
}

/*!
   \brief Set the default pen for the contour lines

   If the spectrogram has a valid default contour pen
   a contour line is painted using the default contour pen.
   Otherwise (pen.style() == Qt::NoPen) the pen is calculated
   for each contour level using contourPen().

   \sa defaultContourPen(), contourPen()
 */
void PlotSpectrogram::setDefaultContourPen(const QPen &pen)
{
    if (pen != m_data->defaultContourPen) {
        m_data->defaultContourPen = pen;
    }
}

/*!
   \return Default contour pen
   \sa setDefaultContourPen()
 */
QPen PlotSpectrogram::defaultContourPen() const
{
    return m_data->defaultContourPen;
}

/*!
   \brief Calculate the pen for a contour line

   The color of the pen is the color for level calculated by the color map

   \param level Contour level
   \return Pen for the contour line
   \note contourPen is only used if defaultContourPen().style() == Qt::NoPen

   \sa setDefaultContourPen(), setColorMap(), setContourLevels()
 */
QPen PlotSpectrogram::contourPen(double level) const
{
    if ((m_data->data == NULL) || (m_data->colorMap == NULL)) {
        return QPen();
    }

    const Interval intensityRange = m_data->data->interval(Qt::ZAxis);
    const QColor c(m_data->colorMap->rgb(intensityRange, level));

    return QPen(c);
}

/*!
   Modify an attribute of the CONREC algorithm, used to calculate
   the contour lines.

   \param flag CONREC flag
   \param on On/Off

   \sa testConrecFlag(), renderContourLines(),
       RasterData::contourLines()
 */
void PlotSpectrogram::setConrecFlag(RasterData::ConrecFlag flag, bool on)
{
    if (bool(m_data->conrecFlags & flag) == on) {
        return;
    }

    if (on) {
        m_data->conrecFlags |= flag;
    } else {
        m_data->conrecFlags &= ~flag;
    }
}

/*!
   Test an attribute of the CONREC algorithm, used to calculate
   the contour lines.

   \param flag CONREC flag
   \return true, is enabled

   The default setting enables RasterData::IgnoreAllVerticesOnLevel

   \sa setConrecClag(), renderContourLines(),
       RasterData::contourLines()
 */
bool PlotSpectrogram::testConrecFlag(RasterData::ConrecFlag flag) const
{
    return m_data->conrecFlags & flag;
}

/*!
   Set the levels of the contour lines

   \param levels Values of the contour levels
   \sa contourLevels(), renderContourLines(),
       RasterData::contourLines()

   \note contourLevels returns the same levels but sorted.
 */
void PlotSpectrogram::setContourLevels(const QList<double> &levels)
{
    m_data->contourLevels = levels;
    std::sort(m_data->contourLevels.begin(), m_data->contourLevels.end());
}

/*!
   \return Levels of the contour lines.

   The levels are sorted in increasing order.

   \sa contourLevels(), renderContourLines(),
       RasterData::contourLines()
 */
QList<double> PlotSpectrogram::contourLevels() const
{
    return m_data->contourLevels;
}

/*!
   Set the data to be displayed

   \param data Spectrogram Data
   \sa data()
 */
void PlotSpectrogram::setData(RasterData *data)
{
    if (data != m_data->data) {
        delete m_data->data;
        m_data->data = data;

        invalidateCache();
    }
}

/*!
   \return Spectrogram data
   \sa setData()
 */
const RasterData *PlotSpectrogram::data() const
{
    return m_data->data;
}

/*!
   \return Spectrogram data
   \sa setData()
 */
RasterData *PlotSpectrogram::data()
{
    return m_data->data;
}

/*!
   \return Bounding interval for an axis

   The default implementation returns the interval of the
   associated raster data object.

   \param axis X, Y, or Z axis
   \sa RasterData::interval()
 */
Interval PlotSpectrogram::interval(Qt::Axis axis) const
{
    if (m_data->data == NULL) {
        return Interval();
    }

    return m_data->data->interval(axis);
}

/*!
   \brief Pixel hint

   The geometry of a pixel is used to calculated the resolution and
   alignment of the rendered image.

   The default implementation returns data()->pixelHint( rect );

   \param area In most implementations the resolution of the data doesn't
               depend on the requested area.

   \return Bounding rectangle of a pixel

   \sa PlotRasterItemBase::pixelHint(), RasterData::pixelHint(),
       render(), renderImage()
 */
QRectF PlotSpectrogram::pixelHint(const QRectF &area) const
{
    if (m_data->data == NULL) {
        return QRectF();
    }

    return m_data->data->pixelHint(area);
}

/*!
   \brief Render an image from data and color map.

   For each pixel of area the value is mapped into a color.

   \param xMap X-Scale Map
   \param yMap Y-Scale Map
   \param area Requested area for the image in scale coordinates
   \param imageSize Size of the requested image

   \return A QImage::Format_Indexed8 or QImage::Format_ARGB32 depending
           on the color map.

   \sa RasterData::value(), ColorMap::rgb(),
       ColorMap::colorIndex()
 */
QImage PlotSpectrogram::renderImage(const ScaleMap &xMap, const ScaleMap &yMap, const QRectF &area,
                                    const QSize &imageSize) const
{
    if (imageSize.isEmpty() || (m_data->data == NULL) || (m_data->colorMap == NULL)) {
        return QImage();
    }

    const Interval intensityRange = m_data->data->interval(Qt::ZAxis);
    if (!intensityRange.isValid()) {
        return QImage();
    }

    const QImage::Format format =
        (m_data->colorMap->format() == ColorMap::RGB) ? QImage::Format_ARGB32 : QImage::Format_Indexed8;

    QImage image(imageSize, format);

    if (m_data->colorMap->format() == ColorMap::Indexed) {
        image.setColorTable(m_data->colorMap->colorTable256());
    }

    m_data->data->initRaster(area, image.size());

#if DEBUG_RENDER
    QElapsedTimer time;
    time.start();
#endif

#if !defined(QT_NO_QFUTURE)
    uint numThreads = renderThreadCount();

    if (numThreads <= 0) {
        numThreads = QThread::idealThreadCount();
    }

    if (numThreads <= 0) {
        numThreads = 1;
    }

    const int numRows = imageSize.height() / numThreads;

    QVector<QFuture<void>> futures;
    futures.reserve(numThreads - 1);

    for (uint i = 0; i < numThreads; i++) {
        QRect tile(0, i * numRows, image.width(), numRows);
        if (i == numThreads - 1) {
            tile.setHeight(image.height() - i * numRows);
            renderTile(xMap, yMap, tile, &image);
        } else {
            futures += QtConcurrent::run(
#if QT_VERSION >= 0x060000
                &PlotSpectrogram::renderTile, this,
#else
                this, &PlotSpectrogram::renderTile,
#endif
                xMap, yMap, tile, &image);
        }
    }

    for (int i = 0; i < futures.size(); i++) {
        futures[i].waitForFinished();
    }
#else
    const QRect tile(0, 0, image.width(), image.height());
    renderTile(xMap, yMap, tile, &image);
#endif

#if DEBUG_RENDER
    const qint64 elapsed = time.elapsed();
    qDebug() << "renderImage" << imageSize << elapsed;
#endif

    m_data->data->discardRaster();

    return image;
}

/*!
    \brief Render a tile of an image.

    Rendering in tiles can be used to composite an image in parallel
    threads.

    \param xMap X-Scale Map
    \param yMap Y-Scale Map
    \param tile Geometry of the tile in image coordinates
    \param image Image to be rendered
 */
void PlotSpectrogram::renderTile(const ScaleMap &xMap, const ScaleMap &yMap, const QRect &tile, QImage *image) const
{
    const Interval range = m_data->data->interval(Qt::ZAxis);
    if (range.width() <= 0.0) {
        return;
    }

    const bool hasGaps = !m_data->data->testAttribute(RasterData::WithoutGaps);

    if (m_data->colorMap->format() == ColorMap::RGB) {
        const int numColors = m_data->colorTable.size();
        const QRgb *rgbTable = m_data->colorTable.constData();
        const ColorMap *colorMap = m_data->colorMap;

        for (int y = tile.top(); y <= tile.bottom(); y++) {
            const double ty = yMap.invTransform(y);

            QRgb *line = reinterpret_cast<QRgb *>(image->scanLine(y));
            line += tile.left();

            for (int x = tile.left(); x <= tile.right(); x++) {
                const double tx = xMap.invTransform(x);

                const double value = m_data->data->value(tx, ty);

                if (hasGaps && qwtIsNaN(value)) {
                    *line++ = 0u;
                } else if (numColors == 0) {
                    *line++ = colorMap->rgb(range, value);
                } else {
                    const uint index = colorMap->colorIndex(numColors, range, value);
                    *line++ = rgbTable[index];
                }
            }
        }
    } else if (m_data->colorMap->format() == ColorMap::Indexed) {
        for (int y = tile.top(); y <= tile.bottom(); y++) {
            const double ty = yMap.invTransform(y);

            unsigned char *line = image->scanLine(y);
            line += tile.left();

            for (int x = tile.left(); x <= tile.right(); x++) {
                const double tx = xMap.invTransform(x);

                const double value = m_data->data->value(tx, ty);

                if (hasGaps && qwtIsNaN(value)) {
                    *line++ = 0;
                } else {
                    const uint index = m_data->colorMap->colorIndex(256, range, value);
                    *line++ = static_cast<unsigned char>(index);
                }
            }
        }
    }
}

/*!
   \brief Return the raster to be used by the CONREC contour algorithm.

   A larger size will improve the precision of the CONREC algorithm,
   but will slow down the time that is needed to calculate the lines.

   The default implementation returns rect.size() / 2 bounded to
   the resolution depending on pixelSize().

   \param area Rectangle, where to calculate the contour lines
   \param rect Rectangle in pixel coordinates, where to paint the contour lines
   \return Raster to be used by the CONREC contour algorithm.

   \note The size will be bounded to rect.size().

   \sa drawContourLines(), RasterData::contourLines()
 */
QSize PlotSpectrogram::contourRasterSize(const QRectF &area, const QRect &rect) const
{
    QSize raster = rect.size() / 2;

    const QRectF pixelRect = pixelHint(area);
    if (!pixelRect.isEmpty()) {
        const QSize res(qCeil(rect.width() / pixelRect.width()), qCeil(rect.height() / pixelRect.height()));
        raster = raster.boundedTo(res);
    }

    return raster;
}

/*!
   Calculate contour lines

   \param rect Rectangle, where to calculate the contour lines
   \param raster Raster, used by the CONREC algorithm
   \return Calculated contour lines

   \sa contourLevels(), setConrecFlag(),
       RasterData::contourLines()
 */
RasterData::ContourLines PlotSpectrogram::renderContourLines(const QRectF &rect, const QSize &raster) const
{
    if (m_data->data == NULL) {
        return RasterData::ContourLines();
    }

    return m_data->data->contourLines(rect, raster, m_data->contourLevels, m_data->conrecFlags);
}

/*!
   Paint the contour lines

   \param painter Painter
   \param xMap Maps x-values into pixel coordinates.
   \param yMap Maps y-values into pixel coordinates.
   \param contourLines Contour lines

   \sa renderContourLines(), defaultContourPen(), contourPen()
 */
void PlotSpectrogram::drawContourLines(QPainter *painter, const ScaleMap &xMap, const ScaleMap &yMap,
                                       const RasterData::ContourLines &contourLines) const
{
    if (m_data->data == NULL) {
        return;
    }

    const int numLevels = m_data->contourLevels.size();
    for (int l = 0; l < numLevels; l++) {
        const double level = m_data->contourLevels[l];

        QPen pen = defaultContourPen();
        if (pen.style() == Qt::NoPen) {
            pen = contourPen(level);
        }

        if (pen.style() == Qt::NoPen) {
            continue;
        }

        painter->setPen(pen);

        const QPolygonF &lines = contourLines[level];
        for (int i = 0; i < lines.size(); i += 2) {
            const QPointF p1(xMap.transform(lines[i].x()), yMap.transform(lines[i].y()));
            const QPointF p2(xMap.transform(lines[i + 1].x()), yMap.transform(lines[i + 1].y()));

            // QwtPainter::drawLine(painter, p1, p2);
            painter->drawLine(p1, p2);
        }
    }
}

/*!
   \brief Draw the spectrogram

   \param painter Painter
   \param xMap Maps x-values into pixel coordinates.
   \param yMap Maps y-values into pixel coordinates.
   \param canvasRect Contents rectangle of the canvas in painter coordinates

   \sa setDisplayMode(), renderImage(),
      PlotRasterItemBase::draw(), drawContourLines()
 */
void PlotSpectrogram::draw(QPainter *painter, const ScaleMap &xMap, const ScaleMap &yMap,
                           const QRectF &canvasRect) const
{
    if (m_data->displayMode & ImageMode) {
        PlotRasterItemBase::draw(painter, xMap, yMap, canvasRect);
    }

    if (m_data->displayMode & ContourMode) {
        // Add some pixels at the borders
        const int margin = 2;
        QRectF rasterRect(canvasRect.x() - margin, canvasRect.y() - margin, canvasRect.width() + 2 * margin,
                          canvasRect.height() + 2 * margin);

        QRectF area = ScaleMap::invTransform(xMap, yMap, rasterRect);

        const QRectF br = boundingRect();
        if (br.isValid()) {
            area &= br;
            if (area.isEmpty()) {
                return;
            }

            rasterRect = ScaleMap::transform(xMap, yMap, area);
        }

        QSize raster = contourRasterSize(area, rasterRect.toRect());
        raster = raster.boundedTo(rasterRect.toRect().size());
        if (raster.isValid()) {
            const RasterData::ContourLines lines = renderContourLines(area, raster);

            drawContourLines(painter, xMap, yMap, lines);
        }
    }
}