//
// $Id$
//

#include "DataFormats/PatCandidates/interface/TriggerPrimitive.h"


using namespace pat;

/// default constructor
TriggerPrimitive::TriggerPrimitive() :
  reco::LeafCandidate(),
  triggerName_( "" ),
  filterName_( "" ) {
}

/// constructor from values
TriggerPrimitive::TriggerPrimitive( const reco::Particle::LorentzVector & aVec, const std::string aTrig, const std::string aFilt ) :
  reco::LeafCandidate( 0, aVec ),
  triggerName_( aTrig ),
  filterName_( aFilt ) {
}
TriggerPrimitive::TriggerPrimitive( const reco::Particle::PolarLorentzVector & aVec, const std::string aTrig, const std::string aFilt ) :
  reco::LeafCandidate( 0, aVec ),
  triggerName_( aTrig ),
  filterName_( aFilt ) {
}

/// destructor
TriggerPrimitive::~TriggerPrimitive() {
}

/// return trigger name
const std::string & TriggerPrimitive::triggerName() const {
  return triggerName_;
}

/// return filter name
const std::string & TriggerPrimitive::filterName() const {
  return filterName_;
}

/// set trigger name 
void TriggerPrimitive::setTriggerName( const std::string aTrig ) {
  triggerName_ = aTrig;
}

/// set filter name 
void TriggerPrimitive::setFilterName( const std::string aFilt ) {
  filterName_ = aFilt;
}
