#ifndef INTERVAL_H
#define INTERVAL_H

#include <QMetaType>

/*!
   \brief A class representing an interval

   The interval is represented by 2 doubles, the lower and the upper limit.
 */

class Interval
{
public:
    /*!
       Flag indicating if a border is included or excluded
       \sa setBorderFlags(), borderFlags()
     */
    enum BorderFlag
    {
        // ! Min/Max values are inside the interval
        IncludeBorders = 0x00,

        // ! Min value is not included in the interval
        ExcludeMinimum = 0x01,

        // ! Max value is not included in the interval
        ExcludeMaximum = 0x02,

        // ! Min/Max values are not included in the interval
        ExcludeBorders = ExcludeMinimum | ExcludeMaximum
    };

    // ! Border flags
    Q_DECLARE_FLAGS(BorderFlags, BorderFlag)

    Interval();
    Interval(double minValue, double maxValue, BorderFlags = IncludeBorders);

    void setInterval(double minValue, double maxValue, BorderFlags = IncludeBorders);

    Interval normalized() const;
    Interval inverted() const;
    Interval limited(double lowerBound, double upperBound) const;

    bool operator==(const Interval &) const;
    bool operator!=(const Interval &) const;

    void setBorderFlags(BorderFlags);
    BorderFlags borderFlags() const;

    double minValue() const;
    double maxValue() const;

    double width() const;
    long double widthL() const;

    void setMinValue(double);
    void setMaxValue(double);

    bool contains(double value) const;
    bool contains(const Interval &) const;

    bool intersects(const Interval &) const;
    Interval intersect(const Interval &) const;
    Interval unite(const Interval &) const;

    Interval operator|(const Interval &) const;
    Interval operator&(const Interval &) const;

    Interval &operator|=(const Interval &);
    Interval &operator&=(const Interval &);

    Interval extend(double value) const;
    Interval operator|(double) const;
    Interval &operator|=(double);

    bool isValid() const;
    bool isNull() const;
    void invalidate();

    Interval symmetrize(double value) const;

private:
    double m_minValue;
    double m_maxValue;
    BorderFlags m_borderFlags;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Interval::BorderFlags)
Q_DECLARE_METATYPE(Interval)
Q_DECLARE_TYPEINFO(Interval, Q_MOVABLE_TYPE);

/*!
   \brief Default Constructor

   Creates an invalid interval [0.0, -1.0]
   \sa setInterval(), isValid()
 */
inline Interval::Interval() : m_minValue(0.0), m_maxValue(-1.0), m_borderFlags(IncludeBorders) {}

/*!
   Constructor

   Build an interval with from min/max values

   \param minValue Minimum value
   \param maxValue Maximum value
   \param borderFlags Include/Exclude borders
 */
inline Interval::Interval(double minValue, double maxValue, BorderFlags borderFlags) :
    m_minValue(minValue), m_maxValue(maxValue), m_borderFlags(borderFlags)
{}

/*!
   Assign the limits of the interval

   \param minValue Minimum value
   \param maxValue Maximum value
   \param borderFlags Include/Exclude borders
 */
inline void Interval::setInterval(double minValue, double maxValue, BorderFlags borderFlags)
{
    m_minValue = minValue;
    m_maxValue = maxValue;
    m_borderFlags = borderFlags;
}

/*!
   Change the border flags

   \param borderFlags Or'd BorderMode flags
   \sa borderFlags()
 */
inline void Interval::setBorderFlags(BorderFlags borderFlags)
{
    m_borderFlags = borderFlags;
}

/*!
   \return Border flags
   \sa setBorderFlags()
 */
inline Interval::BorderFlags Interval::borderFlags() const
{
    return m_borderFlags;
}

/*!
   Assign the lower limit of the interval

   \param minValue Minimum value
 */
inline void Interval::setMinValue(double minValue)
{
    m_minValue = minValue;
}

/*!
   Assign the upper limit of the interval

   \param maxValue Maximum value
 */
inline void Interval::setMaxValue(double maxValue)
{
    m_maxValue = maxValue;
}

// ! \return Lower limit of the interval
inline double Interval::minValue() const
{
    return m_minValue;
}

// ! \return Upper limit of the interval
inline double Interval::maxValue() const
{
    return m_maxValue;
}

/*!
   A interval is valid when minValue() <= maxValue().
   In case of Interval::ExcludeBorders it is true
   when minValue() < maxValue()

   \return True, when the interval is valid
 */
inline bool Interval::isValid() const
{
    if ((m_borderFlags & ExcludeBorders) == 0) {
        return m_minValue <= m_maxValue;
    } else {
        return m_minValue < m_maxValue;
    }
}

/*!
   \brief Return the width of an interval

   The width of invalid intervals is 0.0, otherwise the result is
   maxValue() - minValue().

   \return Interval width
   \sa isValid()
 */
inline double Interval::width() const
{
    return isValid() ? (m_maxValue - m_minValue) : 0.0;
}

/*!
   \brief Return the width of an interval as long double

   The width of invalid intervals is 0.0, otherwise the result is
   maxValue() - minValue().

   \return Interval width
   \sa isValid()
 */
inline long double Interval::widthL() const
{
    if (!isValid()) {
        return 0.0;
    }

    return static_cast<long double>(m_maxValue) - static_cast<long double>(m_minValue);
}

/*!
   \brief Intersection of two intervals

   \param other Interval to intersect with
   \return Intersection of this and other

   \sa intersect()
 */
inline Interval Interval::operator&(const Interval &other) const
{
    return intersect(other);
}

/*!
   Union of two intervals

   \param other Interval to unite with
   \return Union of this and other

   \sa unite()
 */
inline Interval Interval::operator|(const Interval &other) const
{
    return unite(other);
}

/*!
   \brief Compare two intervals

   \param other Interval to compare with
   \return True, when this and other are equal
 */
inline bool Interval::operator==(const Interval &other) const
{
    return (m_minValue == other.m_minValue) && (m_maxValue == other.m_maxValue)
        && (m_borderFlags == other.m_borderFlags);
}

/*!
   \brief Compare two intervals

   \param other Interval to compare with
   \return True, when this and other are not equal
 */
inline bool Interval::operator!=(const Interval &other) const
{
    return (!(*this == other));
}

/*!
   Extend an interval

   \param value Value
   \return Extended interval
   \sa extend()
 */
inline Interval Interval::operator|(double value) const
{
    return extend(value);
}

// ! \return true, if isValid() && (minValue() >= maxValue())
inline bool Interval::isNull() const
{
    return isValid() && m_minValue >= m_maxValue;
}

/*!
   Invalidate the interval

   The limits are set to interval [0.0, -1.0]
   \sa isValid()
 */
inline void Interval::invalidate()
{
    m_minValue = 0.0;
    m_maxValue = -1.0;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug, const Interval &);
#endif

#endif
