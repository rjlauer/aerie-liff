/*!
 * @file ScalerChannel.h
 * @brief Scaler count data in a physical channel.
 * @author Segev BenZvi
 * @ingroup event_data
 * @version $Id: ScalerChannel.h 18879 2014-02-19 21:41:29Z sybenzvi $
 */

#ifndef DATACLASSES_EVENT_SCALERCHANNEL_H_INCLUDED
#define DATACLASSES_EVENT_SCALERCHANNEL_H_INCLUDED

#include <hawcnest/PointerTypedefs.h>
#include <hawcnest/processing/Bag.h>

/*!
 * @class ScalerChannel
 * @author Segev BenZvi
 * @date 24 Jul 2013
 * @ingroup event_data
 * @brief Storage of scaler counts from a physical channel.
 */
class ScalerChannel : public Baggable {

  public:

    ScalerChannel();

    ScalerChannel(int chId, int tkId, int tkChId, double count);

    /// Global physical Channel ID
    int GetChannelId() const { return channelId_; }

    /// Physical Tank ID
    int GetTankId() const { return tankId_; }

    /// Physical channel ID within the tank
    int GetTankChannelId() const { return tankChannelId_;}

    /// Scaler counts
    double GetCount() const { return count_; }

    void SetCount(const double count) { count_ = count; }

  private:

    int channelId_;     ///< Global physical Channel ID at Detector level
    int tankId_;        ///< Physical Tank ID
    int tankChannelId_; ///< Physical Channel ID within Tank
    double count_;      ///< Number of counts above low ToT

};

SHARED_POINTER_TYPEDEFS(ScalerChannel);

#endif // DATACLASSES_EVENT_SCALERCHANNEL_H_INCLUDED

