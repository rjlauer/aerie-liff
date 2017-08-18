/*!
 * @file MuonTagResult.h
 * @brief MuonTag of a reconstructed air shower.
 * @author Harm Schoorlemmer
 * @version $Id: MuonTagResult.h 24730 2015-04-04 19:35:57Z sybenzvi $
 */

#ifndef DATA_STRUCTURES_RECO_MUONTAG_RESULT_H_INCLUDED
#define DATA_STRUCTURES_RECO_MUONTAG_RESULT_H_INCLUDED

#include <data-structures/reconstruction/RecoResult.h>
#include <hawcnest/HAWCNest.h>

/*!
 * @class MuonTagResult
 * @author Harm Schoorlemmer
 * @ingroup muon_tagging
 * @brief MuonTag calculated per tank.
 */
class MuonTagResult : public RecoResult {
  
public:
  
  struct MuonTag {
    float llhIsolatedMuon_;
    float llhNonIsolatedMuon_;
    float llhNotMuon_;
    //likelihood for normalized for a give sum
    float llhIsolatedMuonNorm_;
    float llhNonIsolatedMuonNorm_;
    float llhNotMuonNorm_;
    float log10Sum_;
    float log10MeanNeigh_;
  };
  std::map<int,MuonTag> muonTagList_;       ///<tank id / muontag
  
  MuonTagResult() : nMuon_(0.),
  nMuonIsolated_(0.),
  nMuonNotIsolated_(0.),
  nNotMuon_(0.),
  tanksExcluded_(0.)
  { }
  
  ///number of tanks for which we could make a valid muon tag
  unsigned GetNumberOfTanks() const
  { return muonTagList_.size(); }

  unsigned GetNumberOfTanksExcluded() const
  { return tanksExcluded_; }

  unsigned GetNumberOfMuons() const
  { return nMuon_; }
  
  unsigned GetNumberOfIsolatedMuons() const
  { return nMuonIsolated_; }

  unsigned GetNumberOfNonIsolatedMuons() const
  { return nMuonNotIsolated_; }

  unsigned GetNumberOfNonMuons() const
  { return nNotMuon_; }

  void SetTanksExcluded(const unsigned tanksExcluded)
  { tanksExcluded_ = tanksExcluded; }

  void SetNumberOfMuons(const unsigned nMuon)
  { nMuon_ = nMuon; }

  void SetNumberOfIsolatedMuons(const unsigned nMuonIsolated)
  { nMuonIsolated_ = nMuonIsolated; }

  void SetNumberOfNonIsolatedMuons(const unsigned nMuonNotIsolated)
  { nMuonNotIsolated_ = nMuonNotIsolated; }

  void SetNumberOfNonMuons(const unsigned nNotMuons)
  { nNotMuon_ = nNotMuons; }

  bool HasTank(unsigned tankId) const {
    std::map<int,MuonTag>::const_iterator found = muonTagList_.find(tankId);
    if (found != muonTagList_.end()) {
      return true;
    } else return false;
  }
  
  bool IsMuon(unsigned tankId) const {
    if (!HasTank(tankId)) {
      log_error("you should check if tank has muon tag results use: MuonTagResult::HasTank(tankId)");
      return false;
    }
    std::map<int,MuonTag>::const_iterator it = muonTagList_.find(tankId);
    if (it->second.llhIsolatedMuon_ > 0.9 * it->second.llhNotMuon_ ||it->second.llhNonIsolatedMuon_ > 0.9 * it->second.llhNotMuon_) {
      return true;
    } else
      return false;
  }

  bool IsIsolatedMuon(unsigned tankId) const {
    if (!HasTank(tankId)) {
      log_error("you should check if tank has muon tag results use: MuonTagResult::HasTank(tankId)");
      return false;
    }
    
    if (!HasTank(tankId)) {
      log_error("you should check if tank has muon tag results use: MuonTagResult::HasTank(tankId)");
      return false;
    }
    std::map<int,MuonTag>::const_iterator it = muonTagList_.find(tankId);
    if (it->second.log10Sum_ > 1 && it->second.log10MeanNeigh_  < 1) {
      return true;
    } else {
      return false;
    }

  }

  bool IsNonIsolatedMuon(unsigned tankId) const {
    return (IsMuon(tankId) && !IsIsolatedMuon(tankId));
  }
  
  
protected:
  
  unsigned nMuon_;
  unsigned nMuonIsolated_;
  unsigned nMuonNotIsolated_;
  unsigned nNotMuon_;
  unsigned tanksExcluded_;             ///< Number of tanks which don't have 4 good channels
};

SHARED_POINTER_TYPEDEFS(MuonTagResult);

#endif // DATA_STRUCTURES_RECO_MuonTag_RESULT_H_INCLUDED

