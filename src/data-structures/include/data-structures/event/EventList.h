/*!
 * @file EventList.h
 * @brief A list of Event data.
 * @author Jim Braun
 * @ingroup event_data
 * @version $Id: EventList.h 18879 2014-02-19 21:41:29Z sybenzvi $
 */

#ifndef DATACLASSES_EVENT_EVENTLIST_H_INCLUDED
#define DATACLASSES_EVENT_EVENTLIST_H_INCLUDED

#include <data-structures/event/Event.h>

#include <hawcnest/HAWCNest.h>

#include <vector>

/*!
 * @class EventList
 * @author Jim Braun
 * @date 20 Jun 2011
 * @ingroup raw_data
 * @brief Baggable wrapper for a std::vector of EventPtrs
 */

namespace evt {

class EventList : public Baggable {

  public:

    EventList() { }

    EventList(int nReserve) {
      events_.reserve(nReserve);
    }

    ~EventList() { }

    int GetNEvents() const {return events_.size();}

    void AddEvent(const evt::EventPtr e) {events_.push_back(e);}

    void Clear() {events_.clear();}

    typedef std::vector<evt::EventPtr>::iterator EventIterator;
    typedef std::vector<evt::EventPtr>::const_iterator ConstEventIterator;

    EventIterator EventsBegin() {return events_.begin();}
    EventIterator EventsEnd() {return events_.end();}

    ConstEventIterator EventsBegin() const {return events_.begin();}
    ConstEventIterator EventsEnd() const {return events_.end();}

    const evt::EventPtr& Front() const {return events_.front();}
    const evt::EventPtr& Back() const {return events_.back();}

    evt::EventPtr& Front() {return events_.front();}
    evt::EventPtr& Back() {return events_.back();}

  private:

    std::vector<evt::EventPtr> events_;

};

SHARED_POINTER_TYPEDEFS(EventList);

}

#endif // DATACLASSES_EVENT_EVENTLIST_H_INCLUDED
