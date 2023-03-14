#ifndef PLOT_RASTERITEM_BASE_H
#define PLOT_RASTERITEM_BASE_H

#include <QPainter>
#include <QString>

#include "scalemap.h"

class Interval;

/*!
   \brief A class, which displays raster data

   Raster data is a grid of pixel values, that can be represented
   as a QImage. It is used for many types of information like
   spectrograms, cartograms, geographical maps ...

   Often a plot has several types of raster data organized in layers.
   ( f.e a geographical map, with weather statistics ).
   Using setAlpha() raster items can be stacked easily.

   PlotRasterItemBase is only implemented for images of the following formats:
   QImage::Format_Indexed8, QImage::Format_ARGB32.

   \sa PlotSpectrogram
 */

class PlotRasterItemBase
{
public:
    /*!
       \brief Cache policy
       The default policy is NoCache
     */
    enum CachePolicy
    {
        /*!
           renderImage() is called each time the item has to be repainted
         */
        NoCache,

        /*!
           renderImage() is called, whenever the image cache is not valid,
           or the scales, or the size of the canvas has changed.

           This type of cache is useful for improving the performance
           of hide/show operations or manipulations of the alpha value.
           All other situations are handled by the canvas backing store.
         */
        PaintCache
    };

    /*!
        Attributes to modify the drawing algorithm.
        \sa setPaintAttribute(), testPaintAttribute()
     */
    enum PaintAttribute
    {
        /*!
           When the image is rendered according to the data pixels
           ( RasterData::pixelHint() ) it can be expanded to paint
           device resolution before it is passed to QPainter.
           The expansion algorithm rounds the pixel borders in the same
           way as the axis ticks, what is usually better than the
           scaling algorithm implemented in Qt.
           Disabling this flag might make sense, to reduce the size of a
           document/file. If this is possible for a document format
           depends on the implementation of the specific QPaintEngine.
         */

        PaintInDeviceResolution = 1
    };

    Q_DECLARE_FLAGS(PaintAttributes, PaintAttribute)

    explicit PlotRasterItemBase(const QString &title = QString());
    virtual ~PlotRasterItemBase();

    void setPaintAttribute(PaintAttribute, bool on = true);
    bool testPaintAttribute(PaintAttribute) const;

    void setAlpha(int alpha);
    int alpha() const;

    void setCachePolicy(CachePolicy);
    CachePolicy cachePolicy() const;

    void invalidateCache();

    virtual void draw(QPainter *, const ScaleMap &xMap, const ScaleMap &yMap, const QRectF &canvasRect) const;

    virtual QRectF pixelHint(const QRectF &) const;

    virtual Interval interval(Qt::Axis) const;
    virtual QRectF boundingRect() const;

    void setRenderThreadCount(uint numThreads);
    uint renderThreadCount() const;

protected:
    /*!
       \brief Render an image

       An implementation of render() might iterate over all
       pixels of imageRect. Each pixel has to be translated into
       the corresponding position in scale coordinates using the maps.
       This position can be used to look up a value in a implementation
       specific way and to map it into a color.

       \param xMap X-Scale Map
       \param yMap Y-Scale Map
       \param area Requested area for the image in scale coordinates
       \param imageSize Requested size of the image

       \return Rendered image
     */
    virtual QImage renderImage(const ScaleMap &xMap, const ScaleMap &yMap, const QRectF &area,
                               const QSize &imageSize) const = 0;

    virtual ScaleMap imageMap(Qt::Orientation, const ScaleMap &map, const QRectF &area, const QSize &imageSize,
                              double pixelSize) const;

private:
    explicit PlotRasterItemBase(const PlotRasterItemBase &);
    PlotRasterItemBase &operator=(const PlotRasterItemBase &);

    void init();

    QImage compose(const ScaleMap &, const ScaleMap &, const QRectF &imageArea, const QRectF &paintRect,
                   const QSize &imageSize, bool doCache) const;

    class PrivateData;
    PrivateData *m_data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PlotRasterItemBase::PaintAttributes)

#endif
