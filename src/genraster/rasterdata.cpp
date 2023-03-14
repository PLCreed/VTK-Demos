#include "rasterdata.h"

#include <QList>
#include <QMap>
#include <QPolygon>
#include <QRect>
#include <QVector>

#include <qnumeric.h>

#include "interval.h"
#include "point3d.h"

class RasterData::ContourPlane
{
public:
    explicit inline ContourPlane(double z) : m_z(z) {}

    inline bool intersect(const Point3D vertex[3], QPointF line[2], bool ignoreOnPlane) const;

    inline double z() const { return m_z; }

private:
    inline int compare(double z) const;
    inline QPointF intersection(const Point3D &p1, const Point3D &p2) const;

    double m_z;
};

inline bool RasterData::ContourPlane::intersect(const Point3D vertex[3], QPointF line[2], bool ignoreOnPlane) const
{
    bool found = true;

    // Are the vertices below (-1), on (0) or above (1) the plan ?
    const int eq1 = compare(vertex[0].z());
    const int eq2 = compare(vertex[1].z());
    const int eq3 = compare(vertex[2].z());

    /*
        (a) All the vertices lie below the contour level.
        (b) Two vertices lie below and one on the contour level.
        (c) Two vertices lie below and one above the contour level.
        (d) One vertex lies below and two on the contour level.
        (e) One vertex lies below, one on and one above the contour level.
        (f) One vertex lies below and two above the contour level.
        (g) Three vertices lie on the contour level.
        (h) Two vertices lie on and one above the contour level.
        (i) One vertex lies on and two above the contour level.
        (j) All the vertices lie above the contour level.
     */

    static const int tab[3][3][3] = {
  // jump table to avoid nested case statements
        {{0, 0, 8}, {0, 2, 5},  {7, 6, 9}},
        {{0, 3, 4}, {1, 10, 1}, {4, 3, 0}},
        {{9, 6, 7}, {5, 2, 0},  {8, 0, 0}}
    };

    const int edgeType = tab[eq1 + 1][eq2 + 1][eq3 + 1];
    switch (edgeType) {
    case 1 :
        // d(0,0,-1), h(0,0,1)
        line[0] = vertex[0].toPoint();
        line[1] = vertex[1].toPoint();
        break;
    case 2 :
        // d(-1,0,0), h(1,0,0)
        line[0] = vertex[1].toPoint();
        line[1] = vertex[2].toPoint();
        break;
    case 3 :
        // d(0,-1,0), h(0,1,0)
        line[0] = vertex[2].toPoint();
        line[1] = vertex[0].toPoint();
        break;
    case 4 :
        // e(0,-1,1), e(0,1,-1)
        line[0] = vertex[0].toPoint();
        line[1] = intersection(vertex[1], vertex[2]);
        break;
    case 5 :
        // e(-1,0,1), e(1,0,-1)
        line[0] = vertex[1].toPoint();
        line[1] = intersection(vertex[2], vertex[0]);
        break;
    case 6 :
        // e(-1,1,0), e(1,0,-1)
        line[0] = vertex[2].toPoint();
        line[1] = intersection(vertex[0], vertex[1]);
        break;
    case 7 :
        // c(-1,1,-1), f(1,1,-1)
        line[0] = intersection(vertex[0], vertex[1]);
        line[1] = intersection(vertex[1], vertex[2]);
        break;
    case 8 :
        // c(-1,-1,1), f(1,1,-1)
        line[0] = intersection(vertex[1], vertex[2]);
        line[1] = intersection(vertex[2], vertex[0]);
        break;
    case 9 :
        // f(-1,1,1), c(1,-1,-1)
        line[0] = intersection(vertex[2], vertex[0]);
        line[1] = intersection(vertex[0], vertex[1]);
        break;
    case 10 :
        // g(0,0,0)
        // The CONREC algorithm has no satisfying solution for
        // what to do, when all vertices are on the plane.

        if (ignoreOnPlane) {
            found = false;
        } else {
            line[0] = vertex[2].toPoint();
            line[1] = vertex[0].toPoint();
        }
        break;
    default :
        found = false;
    }

    return found;
}

inline int RasterData::ContourPlane::compare(double z) const
{
    if (z > m_z) {
        return 1;
    }

    if (z < m_z) {
        return -1;
    }

    return 0;
}

inline QPointF RasterData::ContourPlane::intersection(const Point3D &p1, const Point3D &p2) const
{
    const double h1 = p1.z() - m_z;
    const double h2 = p2.z() - m_z;

    const double x = (h2 * p1.x() - h1 * p2.x()) / (h2 - h1);
    const double y = (h2 * p1.y() - h1 * p2.y()) / (h2 - h1);

    return QPointF(x, y);
}

class RasterData::PrivateData
{
public:
    RasterData::Attributes attributes;
};

// ! Constructor
RasterData::RasterData()
{
    m_data = new PrivateData();
}

// ! Destructor
RasterData::~RasterData()
{
    delete m_data;
}

/*!
   Specify an attribute of the data

   \param attribute Attribute
   \param on On/Off
   /sa Attribute, testAttribute()
 */
void RasterData::setAttribute(Attribute attribute, bool on)
{
    if (on) {
        m_data->attributes |= attribute;
    } else {
        m_data->attributes &= ~attribute;
    }
}

/*!
    \return True, when attribute is enabled
    \sa Attribute, setAttribute()
 */
bool RasterData::testAttribute(Attribute attribute) const
{
    return m_data->attributes & attribute;
}

/*!
   \brief Initialize a raster

   Before the composition of an image PlotSpectrogram calls initRaster(),
   announcing the area and its resolution that will be requested.

   The default implementation does nothing, but for data sets that
   are stored in files, it might be good idea to reimplement initRaster(),
   where the data is resampled and loaded into memory.

   \param area Area of the raster
   \param raster Number of horizontal and vertical pixels

   \sa initRaster(), value()
 */
void RasterData::initRaster(const QRectF &area, const QSize &raster)
{
    Q_UNUSED(area);
    Q_UNUSED(raster);
}

/*!
   \brief Discard a raster

   After the composition of an image PlotSpectrogram calls discardRaster().

   The default implementation does nothing, but if data has been loaded
   in initRaster(), it could deleted now.

   \sa initRaster(), value()
 */
void RasterData::discardRaster() {}

/*!
   \brief Pixel hint

   pixelHint() returns the geometry of a pixel, that can be used
   to calculate the resolution and alignment of the plot item, that is
   representing the data.

   Width and height of the hint need to be the horizontal
   and vertical distances between 2 neighbored points.
   The center of the hint has to be the position of any point
   ( it doesn't matter which one ).

   An empty hint indicates, that there are values for any detail level.

   Limiting the resolution of the image might significantly improve
   the performance and heavily reduce the amount of memory when rendering
   a QImage from the raster data.

   The default implementation returns an empty rectangle recommending
   to render in target device ( f.e. screen ) resolution.

   \param area In most implementations the resolution of the data doesn't
               depend on the requested area.

   \return Bounding rectangle of a pixel
 */
QRectF RasterData::pixelHint(const QRectF &area) const
{
    Q_UNUSED(area);
    return QRectF();
}

/*!
   Calculate contour lines

   \param rect Bounding rectangle for the contour lines
   \param raster Number of data pixels of the raster data
   \param levels List of limits, where to insert contour lines
   \param flags Flags to customize the contouring algorithm

   \return Calculated contour lines

   An adaption of CONREC, a simple contouring algorithm.
   http://local.wasp.uwa.edu.au/~pbourke/papers/conrec/
 */
RasterData::ContourLines RasterData::contourLines(const QRectF &rect, const QSize &raster, const QList<double> &levels,
                                                  ConrecFlags flags) const
{
    ContourLines contourLines;

    if ((levels.size() == 0) || !rect.isValid() || !raster.isValid()) {
        return contourLines;
    }

    const double dx = rect.width() / raster.width();
    const double dy = rect.height() / raster.height();

    const bool ignoreOnPlane = flags & RasterData::IgnoreAllVerticesOnLevel;

    const Interval range = interval(Qt::ZAxis);
    bool ignoreOutOfRange = false;
    if (range.isValid()) {
        ignoreOutOfRange = flags & IgnoreOutOfRange;
    }

    RasterData *that = const_cast<RasterData *>(this);
    that->initRaster(rect, raster);

    for (int y = 0; y < raster.height() - 1; y++) {
        enum Position
        {
            Center,

            TopLeft,
            TopRight,
            BottomRight,
            BottomLeft,

            NumPositions
        };

        Point3D xy[NumPositions];

        for (int x = 0; x < raster.width() - 1; x++) {
            const QPointF pos(rect.x() + x * dx, rect.y() + y * dy);

            if (x == 0) {
                xy[TopRight].setX(pos.x());
                xy[TopRight].setY(pos.y());
                xy[TopRight].setZ(value(xy[TopRight].x(), xy[TopRight].y()));

                xy[BottomRight].setX(pos.x());
                xy[BottomRight].setY(pos.y() + dy);
                xy[BottomRight].setZ(value(xy[BottomRight].x(), xy[BottomRight].y()));
            }

            xy[TopLeft] = xy[TopRight];
            xy[BottomLeft] = xy[BottomRight];

            xy[TopRight].setX(pos.x() + dx);
            xy[TopRight].setY(pos.y());
            xy[BottomRight].setX(pos.x() + dx);
            xy[BottomRight].setY(pos.y() + dy);

            xy[TopRight].setZ(value(xy[TopRight].x(), xy[TopRight].y()));
            xy[BottomRight].setZ(value(xy[BottomRight].x(), xy[BottomRight].y()));

            double zMin = xy[TopLeft].z();
            double zMax = zMin;
            double zSum = zMin;

            for (int i = TopRight; i <= BottomLeft; i++) {
                const double z = xy[i].z();

                zSum += z;
                if (z < zMin) {
                    zMin = z;
                }
                if (z > zMax) {
                    zMax = z;
                }
            }

            if (qIsNaN(zSum)) {
                // one of the points is NaN
                continue;
            }

            if (ignoreOutOfRange) {
                if (!range.contains(zMin) || !range.contains(zMax)) {
                    continue;
                }
            }

            if ((zMax < levels[0]) || (zMin > levels[levels.size() - 1])) {
                continue;
            }

            xy[Center].setX(pos.x() + 0.5 * dx);
            xy[Center].setY(pos.y() + 0.5 * dy);
            xy[Center].setZ(0.25 * zSum);

            const int numLevels = levels.size();
            for (int l = 0; l < numLevels; l++) {
                const double level = levels[l];
                if ((level < zMin) || (level > zMax)) {
                    continue;
                }
                QPolygonF &lines = contourLines[level];
                const ContourPlane plane(level);

                QPointF line[2];
                Point3D vertex[3];

                for (int m = TopLeft; m < NumPositions; m++) {
                    vertex[0] = xy[m];
                    vertex[1] = xy[0];
                    vertex[2] = xy[m != BottomLeft ? m + 1 : TopLeft];

                    const bool intersects = plane.intersect(vertex, line, ignoreOnPlane);
                    if (intersects) {
                        lines += line[0];
                        lines += line[1];
                    }
                }
            }
        }
    }

    that->discardRaster();

    return contourLines;
}

/// QwtMatrixRasterData
///////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline double qwtHermiteInterpolate(double A, double B, double C, double D, double t)
{
    const double t2 = t * t;
    const double t3 = t2 * t;

    const double a = -A / 2.0 + (3.0 * B) / 2.0 - (3.0 * C) / 2.0 + D / 2.0;
    const double b = A - (5.0 * B) / 2.0 + 2.0 * C - D / 2.0;
    const double c = -A / 2.0 + C / 2.0;
    const double d = B;

    return a * t3 + b * t2 + c * t + d;
}

static inline double qwtBicubicInterpolate(double v00, double v10, double v20, double v30, double v01, double v11,
                                           double v21, double v31, double v02, double v12, double v22, double v32,
                                           double v03, double v13, double v23, double v33, double dx, double dy)
{
    const double v0 = qwtHermiteInterpolate(v00, v10, v20, v30, dx);
    const double v1 = qwtHermiteInterpolate(v01, v11, v21, v31, dx);
    const double v2 = qwtHermiteInterpolate(v02, v12, v22, v32, dx);
    const double v3 = qwtHermiteInterpolate(v03, v13, v23, v33, dx);

    return qwtHermiteInterpolate(v0, v1, v2, v3, dy);
}

class QwtMatrixRasterData::PrivateData
{
public:
    PrivateData() : resampleMode(QwtMatrixRasterData::NearestNeighbour), numColumns(0) {}

    inline double value(int row, int col) const { return values.data()[row * numColumns + col]; }

    Interval intervals[3];
    QwtMatrixRasterData::ResampleMode resampleMode;

    QVector<double> values;
    int numColumns;
    int numRows;

    double dx;
    double dy;
};

// ! Constructor
QwtMatrixRasterData::QwtMatrixRasterData()
{
    m_data = new PrivateData();
    update();
}

// ! Destructor
QwtMatrixRasterData::~QwtMatrixRasterData()
{
    delete m_data;
}

/*!
   \brief Set the resampling algorithm

   \param mode Resampling mode
   \sa resampleMode(), value()
 */
void QwtMatrixRasterData::setResampleMode(ResampleMode mode)
{
    m_data->resampleMode = mode;
}

/*!
   \return resampling algorithm
   \sa setResampleMode(), value()
 */
QwtMatrixRasterData::ResampleMode QwtMatrixRasterData::resampleMode() const
{
    return m_data->resampleMode;
}

/*!
   \brief Assign the bounding interval for an axis

   Setting the bounding intervals for the X/Y axis is mandatory
   to define the positions for the values of the value matrix.
   The interval in Z direction defines the possible range for
   the values in the matrix, what is f.e used by PlotSpectrogram
   to map values to colors. The Z-interval might be the bounding
   interval of the values in the matrix, but usually it isn't.
   ( f.e a interval of 0.0-100.0 for values in percentage )

   \param axis X, Y or Z axis
   \param interval Interval

   \sa RasterData::interval(), setValueMatrix()
 */
void QwtMatrixRasterData::setInterval(Qt::Axis axis, const Interval &interval)
{
    if ((axis >= 0) && (axis <= 2)) {
        m_data->intervals[axis] = interval;
        update();
    }
}

/*!
   \return Bounding interval for an axis
   \sa setInterval
 */
Interval QwtMatrixRasterData::interval(Qt::Axis axis) const
{
    if ((axis >= 0) && (axis <= 2)) {
        return m_data->intervals[axis];
    }

    return Interval();
}

/*!
   \brief Assign a value matrix

   The positions of the values are calculated by dividing
   the bounding rectangle of the X/Y intervals into equidistant
   rectangles ( pixels ). Each value corresponds to the center of
   a pixel.

   \param values Vector of values
   \param numColumns Number of columns

   \sa valueMatrix(), numColumns(), numRows(), setInterval()()
 */
void QwtMatrixRasterData::setValueMatrix(const QVector<double> &values, int numColumns)
{
    m_data->values = values;
    m_data->numColumns = qMax(numColumns, 0);
    update();
}

/*!
   \return Value matrix
   \sa setValueMatrix(), numColumns(), numRows(), setInterval()
 */
const QVector<double> QwtMatrixRasterData::valueMatrix() const
{
    return m_data->values;
}

/*!
   \brief Change a single value in the matrix

   \param row Row index
   \param col Column index
   \param value New value

   \sa value(), setValueMatrix()
 */
void QwtMatrixRasterData::setValue(int row, int col, double value)
{
    if ((row >= 0) && (row < m_data->numRows) && (col >= 0) && (col < m_data->numColumns)) {
        const int index = row * m_data->numColumns + col;
        m_data->values.data()[index] = value;
    }
}

/*!
   \return Number of columns of the value matrix
   \sa valueMatrix(), numRows(), setValueMatrix()
 */
int QwtMatrixRasterData::numColumns() const
{
    return m_data->numColumns;
}

/*!
   \return Number of rows of the value matrix
   \sa valueMatrix(), numColumns(), setValueMatrix()
 */
int QwtMatrixRasterData::numRows() const
{
    return m_data->numRows;
}

/*!
   \brief Calculate the pixel hint

   pixelHint() returns the geometry of a pixel, that can be used
   to calculate the resolution and alignment of the plot item, that is
   representing the data.

   - NearestNeighbour\n
     pixelHint() returns the surrounding pixel of the top left value
     in the matrix.

   - BilinearInterpolation\n
     Returns an empty rectangle recommending
     to render in target device ( f.e. screen ) resolution.

   \param area Requested area, ignored
   \return Calculated hint

   \sa ResampleMode, setMatrix(), setInterval()
 */
QRectF QwtMatrixRasterData::pixelHint(const QRectF &area) const
{
    Q_UNUSED(area)

    QRectF rect;
    if (m_data->resampleMode == NearestNeighbour) {
        const Interval intervalX = interval(Qt::XAxis);
        const Interval intervalY = interval(Qt::YAxis);
        if (intervalX.isValid() && intervalY.isValid()) {
            rect = QRectF(intervalX.minValue(), intervalY.minValue(), m_data->dx, m_data->dy);
        }
    }

    return rect;
}

/*!
   \return the value at a raster position

   \param x X value in plot coordinates
   \param y Y value in plot coordinates

   \sa ResampleMode
 */
double QwtMatrixRasterData::value(double x, double y) const
{
    const Interval xInterval = interval(Qt::XAxis);
    const Interval yInterval = interval(Qt::YAxis);

    if (!(xInterval.contains(x) && yInterval.contains(y))) {
        return qQNaN();
    }

    double value;

    switch (m_data->resampleMode) {
    case BicubicInterpolation :
    {
        const double colF = (x - xInterval.minValue()) / m_data->dx;
        const double rowF = (y - yInterval.minValue()) / m_data->dy;

        const int col = qRound(colF);
        const int row = qRound(rowF);

        int col0 = col - 2;
        int col1 = col - 1;
        int col2 = col;
        int col3 = col + 1;

        if (col1 < 0) {
            col1 = col2;
        }

        if (col0 < 0) {
            col0 = col1;
        }

        if (col2 >= m_data->numColumns) {
            col2 = col1;
        }

        if (col3 >= m_data->numColumns) {
            col3 = col2;
        }

        int row0 = row - 2;
        int row1 = row - 1;
        int row2 = row;
        int row3 = row + 1;

        if (row1 < 0) {
            row1 = row2;
        }

        if (row0 < 0) {
            row0 = row1;
        }

        if (row2 >= m_data->numRows) {
            row2 = row1;
        }

        if (row3 >= m_data->numRows) {
            row3 = row2;
        }

        // First row
        const double v00 = m_data->value(row0, col0);
        const double v10 = m_data->value(row0, col1);
        const double v20 = m_data->value(row0, col2);
        const double v30 = m_data->value(row0, col3);

        // Second row
        const double v01 = m_data->value(row1, col0);
        const double v11 = m_data->value(row1, col1);
        const double v21 = m_data->value(row1, col2);
        const double v31 = m_data->value(row1, col3);

        // Third row
        const double v02 = m_data->value(row2, col0);
        const double v12 = m_data->value(row2, col1);
        const double v22 = m_data->value(row2, col2);
        const double v32 = m_data->value(row2, col3);

        // Fourth row
        const double v03 = m_data->value(row3, col0);
        const double v13 = m_data->value(row3, col1);
        const double v23 = m_data->value(row3, col2);
        const double v33 = m_data->value(row3, col3);

        value = qwtBicubicInterpolate(v00, v10, v20, v30, v01, v11, v21, v31, v02, v12, v22, v32, v03, v13, v23, v33,
                                      colF - col + 0.5, rowF - row + 0.5);

        break;
    }
    case BilinearInterpolation :
    {
        int col1 = qRound((x - xInterval.minValue()) / m_data->dx) - 1;
        int row1 = qRound((y - yInterval.minValue()) / m_data->dy) - 1;
        int col2 = col1 + 1;
        int row2 = row1 + 1;

        if (col1 < 0) {
            col1 = col2;
        } else if (col2 >= m_data->numColumns) {
            col2 = col1;
        }

        if (row1 < 0) {
            row1 = row2;
        } else if (row2 >= m_data->numRows) {
            row2 = row1;
        }

        const double v11 = m_data->value(row1, col1);
        const double v21 = m_data->value(row1, col2);
        const double v12 = m_data->value(row2, col1);
        const double v22 = m_data->value(row2, col2);

        const double x2 = xInterval.minValue() + (col2 + 0.5) * m_data->dx;
        const double y2 = yInterval.minValue() + (row2 + 0.5) * m_data->dy;

        const double rx = (x2 - x) / m_data->dx;
        const double ry = (y2 - y) / m_data->dy;

        const double vr1 = rx * v11 + (1.0 - rx) * v21;
        const double vr2 = rx * v12 + (1.0 - rx) * v22;

        value = ry * vr1 + (1.0 - ry) * vr2;

        break;
    }
    case NearestNeighbour :
    default :
    {
        int row = int((y - yInterval.minValue()) / m_data->dy);
        int col = int((x - xInterval.minValue()) / m_data->dx);

        // In case of intervals, where the maximum is included
        // we get out of bound for row/col, when the value for the
        // maximum is requested. Instead we return the value
        // from the last row/col

        if (row >= m_data->numRows) {
            row = m_data->numRows - 1;
        }

        if (col >= m_data->numColumns) {
            col = m_data->numColumns - 1;
        }

        value = m_data->value(row, col);
    }
    }

    return value;
}

void QwtMatrixRasterData::update()
{
    m_data->numRows = 0;
    m_data->dx = 0.0;
    m_data->dy = 0.0;

    if (m_data->numColumns > 0) {
        m_data->numRows = m_data->values.size() / m_data->numColumns;

        const Interval xInterval = interval(Qt::XAxis);
        const Interval yInterval = interval(Qt::YAxis);
        if (xInterval.isValid()) {
            m_data->dx = xInterval.width() / m_data->numColumns;
        }
        if (yInterval.isValid()) {
            m_data->dy = yInterval.width() / m_data->numRows;
        }
    }
}
