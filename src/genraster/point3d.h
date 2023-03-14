#ifndef POINT_3D_H
#define POINT_3D_H

#include <qmetatype.h>
#include <qpoint.h>

/*!
   \brief Point3D class defines a 3D point in double coordinates
 */

class Point3D
{
public:
    Point3D();
    Point3D(double x, double y, double z);
    Point3D(const QPointF &);

    bool isNull() const;

    double x() const;
    double y() const;
    double z() const;

    double &rx();
    double &ry();
    double &rz();

    void setX(double x);
    void setY(double y);
    void setZ(double y);

    QPointF toPoint() const;

    bool operator==(const Point3D &) const;
    bool operator!=(const Point3D &) const;

private:
    double m_x;
    double m_y;
    double m_z;
};

Q_DECLARE_TYPEINFO(Point3D, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(Point3D)

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug, const Point3D &);
#endif

/*!
    Constructs a null point.
    \sa isNull()
 */
inline Point3D::Point3D() : m_x(0.0), m_y(0.0), m_z(0.0) {}

// ! Constructs a point with coordinates specified by x, y and z.
inline Point3D::Point3D(double x, double y, double z = 0.0) : m_x(x), m_y(y), m_z(z) {}

/*!
    Constructs a point with x and y coordinates from a 2D point,
    and a z coordinate of 0.
 */
inline Point3D::Point3D(const QPointF &other) : m_x(other.x()), m_y(other.y()), m_z(0.0) {}

/*!
    \return True if the point is null; otherwise returns false.

    A point is considered to be null if x, y and z-coordinates
    are equal to zero.
 */
inline bool Point3D::isNull() const
{
    return m_x == 0.0 && m_y == 0.0 && m_z == 0.0;
}

// ! \return The x-coordinate of the point.
inline double Point3D::x() const
{
    return m_x;
}

// ! \return The y-coordinate of the point.
inline double Point3D::y() const
{
    return m_y;
}

// ! \return The z-coordinate of the point.
inline double Point3D::z() const
{
    return m_z;
}

// ! \return A reference to the x-coordinate of the point.
inline double &Point3D::rx()
{
    return m_x;
}

// ! \return A reference to the y-coordinate of the point.
inline double &Point3D::ry()
{
    return m_y;
}

// ! \return A reference to the z-coordinate of the point.
inline double &Point3D::rz()
{
    return m_z;
}

// ! Sets the x-coordinate of the point to the value specified by x.
inline void Point3D::setX(double x)
{
    m_x = x;
}

// ! Sets the y-coordinate of the point to the value specified by y.
inline void Point3D::setY(double y)
{
    m_y = y;
}

// ! Sets the z-coordinate of the point to the value specified by z.
inline void Point3D::setZ(double z)
{
    m_z = z;
}

/*!
   \return 2D point, where the z coordinate is dropped.
 */
inline QPointF Point3D::toPoint() const
{
    return QPointF(m_x, m_y);
}

// ! \return True, if this point and other are equal; otherwise returns false.
inline bool Point3D::operator==(const Point3D &other) const
{
    return (m_x == other.m_x) && (m_y == other.m_y) && (m_z == other.m_z);
}

// ! \return True if this rect and other are different; otherwise returns false.
inline bool Point3D::operator!=(const Point3D &other) const
{
    return !operator==(other);
}

#endif
