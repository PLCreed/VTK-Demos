#include "interval.h"

namespace
{
static const struct RegisterQwtInterval
{
    inline RegisterQwtInterval() { qRegisterMetaType<Interval>(); }

} qwtRegisterQwtInterval;
} // namespace

/*!
   \brief Normalize the limits of the interval

   If maxValue() < minValue() the limits will be inverted.
   \return Normalized interval

   \sa isValid(), inverted()
 */
Interval Interval::normalized() const
{
    if (m_minValue > m_maxValue) {
        return inverted();
    }
    if ((m_minValue == m_maxValue) && (m_borderFlags == ExcludeMinimum)) {
        return inverted();
    }

    return *this;
}

/*!
   Invert the limits of the interval
   \return Inverted interval
   \sa normalized()
 */
Interval Interval::inverted() const
{
    BorderFlags borderFlags = IncludeBorders;

    if (m_borderFlags & ExcludeMinimum) {
        borderFlags |= ExcludeMaximum;
    }

    if (m_borderFlags & ExcludeMaximum) {
        borderFlags |= ExcludeMinimum;
    }

    return Interval(m_maxValue, m_minValue, borderFlags);
}

/*!
   Test if a value is inside an interval

   \param value Value
   \return true, if value lies inside the boundaries
 */
bool Interval::contains(double value) const
{
    if (!isValid()) {
        return false;
    }

    if ((value < m_minValue) || (value > m_maxValue)) {
        return false;
    }

    if ((value == m_minValue) && (m_borderFlags & ExcludeMinimum)) {
        return false;
    }

    if ((value == m_maxValue) && (m_borderFlags & ExcludeMaximum)) {
        return false;
    }

    return true;
}

/*!
   Test if an interval is inside an interval

   \param interval Interval
   \return true, if interval lies inside the boundaries
 */
bool Interval::contains(const Interval &interval) const
{
    if (!isValid() || !interval.isValid()) {
        return false;
    }

    if ((interval.m_minValue < m_minValue) || (interval.m_maxValue > m_maxValue)) {
        return false;
    }

    if (m_borderFlags) {
        if (interval.m_minValue == m_minValue) {
            if ((m_borderFlags & ExcludeMinimum) && !(interval.m_borderFlags & ExcludeMinimum)) {
                return false;
            }
        }

        if (interval.m_maxValue == m_maxValue) {
            if ((m_borderFlags & ExcludeMaximum) && !(interval.m_borderFlags & ExcludeMaximum)) {
                return false;
            }
        }
    }

    return true;
}

// ! Unite 2 intervals
Interval Interval::unite(const Interval &other) const
{
    /*
       If one of the intervals is invalid return the other one.
       If both are invalid return an invalid default interval
     */
    if (!isValid()) {
        if (!other.isValid()) {
            return Interval();
        } else {
            return other;
        }
    }
    if (!other.isValid()) {
        return *this;
    }

    Interval united;
    BorderFlags flags = IncludeBorders;

    // minimum
    if (m_minValue < other.minValue()) {
        united.setMinValue(m_minValue);
        flags &= m_borderFlags & ExcludeMinimum;
    } else if (other.minValue() < m_minValue) {
        united.setMinValue(other.minValue());
        flags &= other.borderFlags() & ExcludeMinimum;
    } else // m_minValue == other.minValue()
    {
        united.setMinValue(m_minValue);
        flags &= (m_borderFlags & other.borderFlags()) & ExcludeMinimum;
    }

    // maximum
    if (m_maxValue > other.maxValue()) {
        united.setMaxValue(m_maxValue);
        flags &= m_borderFlags & ExcludeMaximum;
    } else if (other.maxValue() > m_maxValue) {
        united.setMaxValue(other.maxValue());
        flags &= other.borderFlags() & ExcludeMaximum;
    } else // m_maxValue == other.maxValue() )
    {
        united.setMaxValue(m_maxValue);
        flags &= m_borderFlags & other.borderFlags() & ExcludeMaximum;
    }

    united.setBorderFlags(flags);
    return united;
}

/*!
   \brief Intersect 2 intervals

   \param other Interval to be intersect with
   \return Intersection
 */
Interval Interval::intersect(const Interval &other) const
{
    if (!other.isValid() || !isValid()) {
        return Interval();
    }

    Interval i1 = *this;
    Interval i2 = other;

    // swap i1/i2, so that the minimum of i1
    // is smaller then the minimum of i2

    if (i1.minValue() > i2.minValue()) {
        qSwap(i1, i2);
    } else if (i1.minValue() == i2.minValue()) {
        if (i1.borderFlags() & ExcludeMinimum) {
            qSwap(i1, i2);
        }
    }

    if (i1.maxValue() < i2.minValue()) {
        return Interval();
    }

    if (i1.maxValue() == i2.minValue()) {
        if (i1.borderFlags() & ExcludeMaximum || i2.borderFlags() & ExcludeMinimum) {
            return Interval();
        }
    }

    Interval intersected;
    BorderFlags flags = IncludeBorders;

    intersected.setMinValue(i2.minValue());
    flags |= i2.borderFlags() & ExcludeMinimum;

    if (i1.maxValue() < i2.maxValue()) {
        intersected.setMaxValue(i1.maxValue());
        flags |= i1.borderFlags() & ExcludeMaximum;
    } else if (i2.maxValue() < i1.maxValue()) {
        intersected.setMaxValue(i2.maxValue());
        flags |= i2.borderFlags() & ExcludeMaximum;
    } else // i1.maxValue() == i2.maxValue()
    {
        intersected.setMaxValue(i1.maxValue());
        flags |= i1.borderFlags() & i2.borderFlags() & ExcludeMaximum;
    }

    intersected.setBorderFlags(flags);
    return intersected;
}

/*!
   \brief Unite this interval with the given interval.

   \param other Interval to be united with
   \return This interval
 */
Interval &Interval::operator|=(const Interval &other)
{
    *this = *this | other;
    return *this;
}

/*!
   \brief Intersect this interval with the given interval.

   \param other Interval to be intersected with
   \return This interval
 */
Interval &Interval::operator&=(const Interval &other)
{
    *this = *this & other;
    return *this;
}

/*!
   \brief Test if two intervals overlap

   \param other Interval
   \return True, when the intervals are intersecting
 */
bool Interval::intersects(const Interval &other) const
{
    if (!isValid() || !other.isValid()) {
        return false;
    }

    Interval i1 = *this;
    Interval i2 = other;

    // swap i1/i2, so that the minimum of i1
    // is smaller then the minimum of i2

    if (i1.minValue() > i2.minValue()) {
        qSwap(i1, i2);
    } else if ((i1.minValue() == i2.minValue()) && i1.borderFlags() & ExcludeMinimum) {
        qSwap(i1, i2);
    }

    if (i1.maxValue() > i2.minValue()) {
        return true;
    }
    if (i1.maxValue() == i2.minValue()) {
        return !((i1.borderFlags() & ExcludeMaximum) || (i2.borderFlags() & ExcludeMinimum));
    }
    return false;
}

/*!
   Adjust the limit that is closer to value, so that value becomes
   the center of the interval.

   \param value Center
   \return Interval with value as center
 */
Interval Interval::symmetrize(double value) const
{
    if (!isValid()) {
        return *this;
    }

    const double delta = qMax(qAbs(value - m_maxValue), qAbs(value - m_minValue));

    return Interval(value - delta, value + delta);
}

/*!
   Limit the interval, keeping the border modes

   \param lowerBound Lower limit
   \param upperBound Upper limit

   \return Limited interval
 */
Interval Interval::limited(double lowerBound, double upperBound) const
{
    if (!isValid() || (lowerBound > upperBound)) {
        return Interval();
    }

    double minValue = qMax(m_minValue, lowerBound);
    minValue = qMin(minValue, upperBound);

    double maxValue = qMax(m_maxValue, lowerBound);
    maxValue = qMin(maxValue, upperBound);

    return Interval(minValue, maxValue, m_borderFlags);
}

/*!
   \brief Extend the interval

   If value is below minValue(), value becomes the lower limit.
   If value is above maxValue(), value becomes the upper limit.

   extend() has no effect for invalid intervals

   \param value Value
   \return extended interval

   \sa isValid()
 */
Interval Interval::extend(double value) const
{
    if (!isValid()) {
        return *this;
    }

    return Interval(qMin(value, m_minValue), qMax(value, m_maxValue), m_borderFlags);
}

/*!
   Extend an interval

   \param value Value
   \return Reference of the extended interval

   \sa extend()
 */
Interval &Interval::operator|=(double value)
{
    *this = *this | value;
    return *this;
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<(QDebug debug, const Interval &interval)
{
    const int flags = interval.borderFlags();

    debug.nospace() << "Interval(" << ((flags & Interval::ExcludeMinimum) ? "]" : "[") << interval.minValue() << ","
                    << interval.maxValue() << ((flags & Interval::ExcludeMaximum) ? "[" : "]") << ")";

    return debug.space();
}

#endif
