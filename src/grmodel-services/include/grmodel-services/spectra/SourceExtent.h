/*!
 * @file SourceExtent.h
 * @brief Simple parameterizations of source extent
 * @author Segev BenZvi
 * @date 15 Feb 2013
 * @version $Id: SourceExtent.h 14868 2013-04-27 14:16:27Z sybenzvi $
 */

#ifndef GRMODEL_SERVICES_SPECTRA_SOURCEEXTENT_H_INCLUDED
#define GRMODEL_SERVICES_SPECTRA_SOURCEEXTENT_H_INCLUDED

/*!
 * @class SourceExtent
 * @author Segev BenZvi
 * @date 15 Feb 2013
 * @ingroup gr_models
 * @brief Simple parameterizations of source extent
 */
class SourceExtent {

  public:

    enum Shape { NONE, BOX, ELLIPSE };
    
    SourceExtent();

    SourceExtent(const Shape s, const double l, const double w, const double a);

    /// Long axis/length of source region
    double GetLength() const { return length_; }

    /// Short axis/width of source region
    double GetWidth() const { return width_; }

    /// Angle of long axis w.r.t. coordinate x-axis
    double GetAngle() const { return angle_; }

    /// Shape of the source (NONE if pointlike)
    Shape GetShape() const { return shape_; }

    /// Return true if source is not pointlike
    bool IsExtended() const { return shape_ != NONE; }

  private:

    Shape shape_;       ///< Shape of source region
    double length_;     ///< Long axis/length of source region
    double width_;      ///< Short axis/width of source region
    double angle_;      ///< Angle of long axis w.r.t. coordinate x-axis

};

#endif // GRMODEL_SERVICES_SPECTRA_SOURCEEXTENT_H_INCLUDED

