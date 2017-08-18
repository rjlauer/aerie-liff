/*!
 * @file Prescale.h
 * @brief Prescale data for (possibly untriggered) simulated hits.
 * @author Segev BenZvi
 * @ingroup event_data
 * @version $Id: Prescale.h 18879 2014-02-19 21:41:29Z sybenzvi $
 */

#ifndef DATACLASSES_PRESCALE_H_INCLUDED
#define DATACLASSES_PRESCALE_H_INCLUDED

#include <data-structures/event/Event.h>
#include <hawcnest/processing/Module.h>
#include <hawcnest/Configuration.h>
#include <hawcnest/RegisterService.h>

/*!
 * @class Prescale
 * @author Segev BenZvi
 * @ingroup data_io
 * @date 2 Sep 2010
 * @brief Store prescale information for a simulated event to allow corrections
 *        for spectrum and throwing geometry
 */

namespace evt {

class Prescale : public Baggable {

  public:

    Prescale() : value_(1),
                 maxHit_(0) { }

    Prescale(const int& i) : value_(i),
                             maxHit_(0) { }

    Prescale(const int& i,
             const int& m) : value_(i),
                             maxHit_(m) { }

    /// Allow automatic casting to an integer
    operator int() const { return value_; }

    unsigned GetMaxWindowHits() const {return maxHit_;}
    void SetMaxWindowHits(unsigned maxHit) {maxHit_ = maxHit;}

  private:

    int value_;
    int maxHit_;

};

SHARED_POINTER_TYPEDEFS(Prescale);

}

#endif // DATACLASSES_PRESCALE_H_INCLUDED
