#ifndef SCALE_MAP_H
#define SCALE_MAP_H

#include <QtGlobal>

class QPointF;
class QRectF;

/*!
   \brief A scale map

   ScaleMap offers transformations from the coordinate system
   of a scale into the linear coordinate system of a paint device
   and vice versa.
 */
class ScaleMap
{
public:
    ScaleMap();
    ScaleMap(const ScaleMap &);

    ~ScaleMap();

    ScaleMap &operator=(const ScaleMap &);

    void setPaintInterval(double p1, double p2);
    void setScaleInterval(double s1, double s2);

    double transform(double s) const;
    double invTransform(double p) const;

    double p1() const;
    double p2() const;

    double s1() const;
    double s2() const;

    double pDist() const;
    double sDist() const;

    static QRectF transform(const ScaleMap &, const ScaleMap &, const QRectF &);

    static QRectF invTransform(const ScaleMap &, const ScaleMap &, const QRectF &);

    static QPointF transform(const ScaleMap &, const ScaleMap &, const QPointF &);

    static QPointF invTransform(const ScaleMap &, const ScaleMap &, const QPointF &);

    bool isInverting() const;

private:
    void updateFactor();

    double m_s1, m_s2; // scale interval boundaries
    double m_p1, m_p2; // paint device interval boundaries

    double m_cnv; // conversion factor
    double m_ts1;
};

/*!
    \return First border of the scale interval
 */
inline double ScaleMap::s1() const
{
    return m_s1;
}

/*!
    \return Second border of the scale interval
 */
inline double ScaleMap::s2() const
{
    return m_s2;
}

/*!
    \return First border of the paint interval
 */
inline double ScaleMap::p1() const
{
    return m_p1;
}

/*!
    \return Second border of the paint interval
 */
inline double ScaleMap::p2() const
{
    return m_p2;
}

/*!
    \return qwtAbs(p2() - p1())
 */
inline double ScaleMap::pDist() const
{
    return qAbs(m_p2 - m_p1);
}

/*!
    \return qwtAbs(s2() - s1())
 */
inline double ScaleMap::sDist() const
{
    return qAbs(m_s2 - m_s1);
}

/*!
   Transform a point related to the scale interval into an point
   related to the interval of the paint device

   \param s Value relative to the coordinates of the scale
   \return Transformed value

   \sa invTransform()
 */
inline double ScaleMap::transform(double s) const
{
    return m_p1 + (s - m_ts1) * m_cnv;
}

/*!
   Transform an paint device value into a value in the
   interval of the scale.

   \param p Value relative to the coordinates of the paint device
   \return Transformed value

   \sa transform()
 */
inline double ScaleMap::invTransform(double p) const
{
    double s = m_ts1 + (p - m_p1) / m_cnv;

    return s;
}

// ! \return True, when ( p1() < p2() ) != ( s1() < s2() )
inline bool ScaleMap::isInverting() const
{
    return ((m_p1 < m_p2) != (m_s1 < m_s2));
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug, const ScaleMap &);
#endif

#endif
