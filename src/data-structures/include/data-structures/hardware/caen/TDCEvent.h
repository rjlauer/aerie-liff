/*!
 * @file TDCEvent.h
 * @brief Data from a triggered TDC event.
 * @author Jim Braun
 * @date 10 Jun 2011
 * @version $Id: TDCEvent.h 18889 2014-02-20 15:22:08Z sybenzvi $
 */

#ifndef DATA_STRUCTURES_HARDWARE_CAEN_TDC_EVENT_H_INCLUDED
#define DATA_STRUCTURES_HARDWARE_CAEN_TDC_EVENT_H_INCLUDED

#include <data-structures/hardware/caen/TDCFormat.h>

#include <hawcnest/Logging.h>
#include <hawcnest/HAWCNest.h>

#include <vector>

namespace caen {

  /*!
   * @class TDCEvent
   * @author Jim Braun
   * @brief Representation of one triggered event from one TDC card
   * @date 10 Jun 2011
   */
  class TDCEvent {

    public:

      virtual ~TDCEvent() { }

      TDCEvent() : haveExtendedTriggerTimeTag_(false),
                   nHeaders_(0),
                   nTrailers_(0),
                   nErrors_(0) { }

      bool HasExtendedTriggerTimeTag()  const {
        return haveExtendedTriggerTimeTag_;
      }

      const ExtendedTriggerTimeTag GetExtendedTriggerTimeTag() const {
        if (!haveExtendedTriggerTimeTag_) {
          log_fatal("Request for unavailable extended trigger time tag");
        }
        return extendedTriggerTimeTag_;
      }

      const GlobalHeader&  GetGlobalHeader()  const { return header_; }
      const GlobalTrailer& GetGlobalTrailer() const { return trailer_; }

      void SetGlobalHeader(const GlobalHeader& header)
      { header_ = header; }

      void SetGlobalTrailer(const GlobalTrailer& trailer) {
        trailer_ = trailer;
        extendedTriggerTimeTag_.extendedTriggerTime |= trailer_.etttLowBits;
      }

      void SetExtendedTriggerTimeTag(const ExtendedTriggerTimeTag &t) {
        extendedTriggerTimeTag_ = t;
        haveExtendedTriggerTimeTag_ = true;
      }

      void SetGlobalHeader(const uint32_t data)
      { header_ = ParseGlobalHeader(data); }

      void SetGlobalTrailer(const uint32_t data) {
        trailer_ = ParseGlobalTrailer(data);
        extendedTriggerTimeTag_.extendedTriggerTime |= trailer_.etttLowBits;
      }

      void SetExtendedTriggerTimeTag(const uint32_t data) {
        extendedTriggerTimeTag_ = ParseExtendedTriggerTimeTag(data);
        haveExtendedTriggerTimeTag_ = true;
      }

      unsigned GetNTDCs() const { return nHeaders_; }
      unsigned GetNTDCHeaders() const { return nHeaders_; }
      unsigned GetNTDCTrailers() const { return nTrailers_; }
      unsigned GetNTDCErrors() const { return nErrors_; }
      unsigned GetNMeasurements() const { return measurements_.size(); }

      void AddTDCHeader(const TDCHeader &h) {
        if (nHeaders_ >= MAX_TDC) {
          log_error("Too many TDCs for event");
        } else if (nHeaders_ != nTrailers_) {
          log_error("Bad TDC data block: No trailer");
        } else {
          tdcHeaders_[nHeaders_++] = h;
        }
      }

      void AddTDCTrailer(const TDCTrailer &t) {
       if (nTrailers_ >= nHeaders_ ) {
          log_error("TDC trailer without header");
        } else {
          tdcTrailers_[nTrailers_++] = t;
        }
      }

      void AddTDCError(const TDCError &e) {
        if (nErrors_ >= nHeaders_ ) {
          log_error("TDC error without header");
        } else {
          tdcErrors_[nErrors_++] = e;
        }
      }

      TDCHeader GetTDCHeader(unsigned iTDC) const {
        if (iTDC >= GetNTDCHeaders()) {
          log_fatal("No such TDC index: " << iTDC);
        }
        return tdcHeaders_[iTDC];
      }

      TDCTrailer GetTDCTrailer(unsigned iTDC) const {
        if (iTDC >= GetNTDCTrailers()) {
          log_fatal("No such TDC index: " << iTDC);
        }
        return tdcTrailers_[iTDC];
      }

      TDCError GetTDCError(unsigned iTDC) const {
        if (iTDC >= GetNTDCErrors()) {
          log_fatal("No such TDC index: " << iTDC);
        }
        return tdcErrors_[iTDC];
      }

      void AddMeasurement(const TDCMeasurement &m)
      { measurements_.push_back(m); }

      void AddMeasurement(const uint32_t data)
      { measurements_.push_back(ParseTDCMeasurement(data)); }

      typedef std::vector<TDCMeasurement>::const_iterator
                                              TDCMeasurementIterator;

      TDCMeasurementIterator TDCMeasurementsBegin() const
      { return measurements_.begin(); }

      TDCMeasurementIterator TDCMeasurementsEnd() const
      { return measurements_.end(); }

      typedef const TDCHeader* TDCHeaderIterator;
      TDCHeaderIterator TDCHeadersBegin() const {return tdcHeaders_;}
      TDCHeaderIterator TDCHeadersEnd() const {return tdcHeaders_+nHeaders_;}

      typedef const TDCTrailer* TDCTrailerIterator;
      TDCTrailerIterator TDCTrailersBegin() const {return tdcTrailers_;}
      TDCTrailerIterator TDCTrailersEnd() const {return tdcTrailers_+nTrailers_;}

      typedef const TDCError* TDCErrorIterator;
      TDCErrorIterator TDCErrorsBegin() const {return tdcErrors_;}
      TDCErrorIterator TDCErrorsEnd() const {return tdcErrors_+nErrors_;}

    private:

      bool haveExtendedTriggerTimeTag_;
      ExtendedTriggerTimeTag  extendedTriggerTimeTag_;

      GlobalHeader  header_;
      GlobalTrailer trailer_;

      int nHeaders_;
      int nTrailers_;
      int nErrors_;

      TDCHeader  tdcHeaders_[MAX_TDC];
      TDCTrailer tdcTrailers_[MAX_TDC];
      TDCError   tdcErrors_[MAX_TDC];

      std::vector<TDCMeasurement> measurements_;

  };

  SHARED_POINTER_TYPEDEFS(TDCEvent);

}

#endif // DATA_STRUCTURES_HARDWARE_CAEN_TDC_EVENT_H_INCLUDED
