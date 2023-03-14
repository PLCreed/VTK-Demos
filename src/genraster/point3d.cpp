#include "point3d.h"

#if QT_VERSION >= 0x050200

static Point3D qwtPointToPoint3D(const QPointF &point)
{
    return Point3D(point);
}

#endif

namespace
{
static const struct RegisterQwtPoint3D
{
    inline RegisterQwtPoint3D()
    {
        qRegisterMetaType<Point3D>();

#if QT_VERSION >= 0x050200
        QMetaType::registerConverter<QPointF, Point3D>(qwtPointToPoint3D);
#endif
    }
} qwtRegisterQwtPoint3D;
} // namespace

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<(QDebug debug, const Point3D &point)
{
    debug.nospace() << "Point3D(" << point.x() << "," << point.y() << "," << point.z() << ")";
    return debug.space();
}

#endif
