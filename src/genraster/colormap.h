#ifndef COLOR_MAP_H
#define COLOR_MAP_H

#include <QColor>

class Interval;

#if QT_VERSION < 0x060000
template <typename T>
class QVector;
#endif

/*!
   \brief ColorMap is used to map values into colors.

   For displaying 3D data on a 2D plane the 3rd dimension is often
   displayed using colors, like f.e in a spectrogram.

   Each color map is optimized to return colors for only one of the
   following image formats:

   - QImage::Format_Indexed8\n
   - QImage::Format_ARGB32\n
 */

class ColorMap
{
public:
    /*!
        Format for color mapping
        \sa rgb(), colorIndex(), colorTable()
     */

    enum Format
    {
        // ! The map is intended to map into RGB values.
        RGB,

        /*!
           Map values into 8 bit values, that
           are used as indexes into the color table.

           Indexed color maps are used to generate QImage::Format_Indexed8
           images. The calculation of the color index is usually faster
           and the resulting image has a lower memory footprint.

           \sa colorIndex(), colorTable()
         */
        Indexed
    };

    explicit ColorMap(Format = ColorMap::RGB);
    virtual ~ColorMap();

    void setFormat(Format);
    Format format() const;

    /*!
       Map a value of a given interval into a RGB value.

       \param interval Range for the values
       \param value Value
       \return RGB value, corresponding to value
     */
    virtual QRgb rgb(const Interval &interval, double value) const = 0;

    virtual uint colorIndex(int numColors, const Interval &interval, double value) const;

    QColor color(const Interval &, double value) const;
    virtual QVector<QRgb> colorTable(int numColors) const;
    virtual QVector<QRgb> colorTable256() const;

private:
    Q_DISABLE_COPY(ColorMap)

    Format m_format;
};

/*!
   \brief LinearColorMap builds a color map from color stops.

   A color stop is a color at a specific position. The valid
   range for the positions is [0.0, 1.0]. When mapping a value
   into a color it is translated into this interval according to mode().
 */
class LinearColorMap : public ColorMap
{
public:
    /*!
       Mode of color map
       \sa setMode(), mode()
     */
    enum Mode
    {
        // ! Return the color from the next lower color stop
        FixedColors,

        // ! Interpolating the colors of the adjacent stops.
        ScaledColors
    };

    explicit LinearColorMap(ColorMap::Format = ColorMap::RGB);

    LinearColorMap(const QColor &from, const QColor &to, ColorMap::Format = ColorMap::RGB);

    virtual ~LinearColorMap();

    void setMode(Mode);
    Mode mode() const;

    void setColorInterval(const QColor &color1, const QColor &color2);
    void addColorStop(double value, const QColor &);
    QVector<double> colorStops() const;

    QColor color1() const;
    QColor color2() const;

    virtual QRgb rgb(const Interval &, double value) const override;

    virtual uint colorIndex(int numColors, const Interval &, double value) const override;

    class ColorStops;

private:
    class PrivateData;
    PrivateData *m_data;
};

/*!
   Map a value into a color

   \param interval Valid interval for values
   \param value Value

   \return Color corresponding to value
 */
inline QColor ColorMap::color(const Interval &interval, double value) const
{
    return QColor::fromRgba(rgb(interval, value));
}

/*!
   \return Intended format of the color map
   \sa Format
 */
inline ColorMap::Format ColorMap::format() const
{
    return m_format;
}

#endif
