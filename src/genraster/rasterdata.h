#ifndef RASTER_DATA_H
#define RASTER_DATA_H

#include <qnamespace.h>

class Interval;
class QPolygonF;
class QRectF;
class QSize;

template <typename T>
class QVector;
template <typename T>
class QList;
template <class Key, class T>
class QMap;

/*!
   \brief RasterData defines an interface to any type of raster data.

   RasterData is an abstract interface, that is used by
   PlotRasterItemBase to find the values at the pixels of its raster.

   Gaps inside the bounding rectangle of the data can be indicated by NaN
   values ( when WithoutGaps is disabled ).

   Often a raster item is used to display values from a matrix. Then the
   derived raster data class needs to implement some sort of resampling,
   that maps the raster of the matrix into the requested raster of
   the raster item ( depending on resolution and scales of the canvas ).

   QwtMatrixRasterData implements raster data, that returns values from
   a given 2D matrix.

   \sa QwtMatrixRasterData
 */
class RasterData
{
public:
    // ! Contour lines
    typedef QMap<double, QPolygonF> ContourLines;

    /*!
       \brief Raster data attributes

       Additional information that is used to improve processing
       of the data.
     */
    enum Attribute
    {
        /*!
           The bounding rectangle of the data is spanned by
           the interval(Qt::XAxis) and interval(Qt::YAxis).

           WithoutGaps indicates, that the data has no gaps
           ( unknown values ) in this area and the result of
           value() does not need to be checked for NaN values.

           Enabling this flag will have an positive effect on
           the performance of rendering a PlotSpectrogram.

           The default setting is false.

           \note NaN values indicate an undefined value
         */
        WithoutGaps = 0x01
    };

    Q_DECLARE_FLAGS(Attributes, Attribute)

    // ! Flags to modify the contour algorithm
    enum ConrecFlag
    {
        // ! Ignore all vertices on the same level
        IgnoreAllVerticesOnLevel = 0x01,

        // ! Ignore all values, that are out of range
        IgnoreOutOfRange = 0x02
    };

    Q_DECLARE_FLAGS(ConrecFlags, ConrecFlag)

    RasterData();
    virtual ~RasterData();

    void setAttribute(Attribute, bool on = true);
    bool testAttribute(Attribute) const;

    /*!
       \return Bounding interval for an axis
       \sa setInterval
     */
    virtual Interval interval(Qt::Axis) const = 0;

    virtual QRectF pixelHint(const QRectF &) const;

    virtual void initRaster(const QRectF &, const QSize &raster);
    virtual void discardRaster();

    /*!
       \return the value at a raster position
       \param x X value in plot coordinates
       \param y Y value in plot coordinates
     */
    virtual double value(double x, double y) const = 0;

    virtual ContourLines contourLines(const QRectF &rect, const QSize &raster, const QList<double> &levels,
                                      ConrecFlags) const;

    class Contour3DPoint;
    class ContourPlane;

private:
    Q_DISABLE_COPY(RasterData)

    class PrivateData;
    PrivateData *m_data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(RasterData::ConrecFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(RasterData::Attributes)

/*!
   \brief A class representing a matrix of values as raster data

   QwtMatrixRasterData implements an interface for a matrix of
   equidistant values, that can be used by a PlotRasterItemBase.
   It implements a couple of resampling algorithms, to provide
   values for positions, that or not on the value matrix.
 */
class QwtMatrixRasterData : public RasterData
{
public:
    /*!
       \brief Resampling algorithm
       The default setting is NearestNeighbour;
     */
    enum ResampleMode
    {
        /*!
           Return the value from the matrix, that is nearest to the
           the requested position.
         */
        NearestNeighbour,

        /*!
           Interpolate the value from the distances and values of the
           4 surrounding values in the matrix,
         */
        BilinearInterpolation,

        /*!
           Interpolate the value from the 16 surrounding values in the
           matrix using hermite bicubic interpolation
         */
        BicubicInterpolation
    };

    QwtMatrixRasterData();
    virtual ~QwtMatrixRasterData();

    void setResampleMode(ResampleMode mode);
    ResampleMode resampleMode() const;

    void setInterval(Qt::Axis, const Interval &);
    virtual Interval interval(Qt::Axis axis) const override final;

    void setValueMatrix(const QVector<double> &values, int numColumns);
    const QVector<double> valueMatrix() const;

    void setValue(int row, int col, double value);

    int numColumns() const;
    int numRows() const;

    virtual QRectF pixelHint(const QRectF &) const override;

    virtual double value(double x, double y) const override;

private:
    void update();

    class PrivateData;
    PrivateData *m_data;
};

#endif
