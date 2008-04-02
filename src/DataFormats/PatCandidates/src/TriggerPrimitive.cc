//
// $Id$
//

#include "DataFormats/PatCandidates/interface/TriggerPrimitive.h"


using namespace pat;

/// default constructor
TriggerPrimitive::TriggerPrimitive() :
  lorentzVector_( 0.,0.,0.,0. ),
  triggerName_( "" ),
  filterName_( "" ) {
}

/// constructor from values
TriggerPrimitive::TriggerPrimitive( const reco::Particle::LorentzVector & aVec, const std::string aTrig, const std::string aFilt ) :
  lorentzVector_( aVec ),
  triggerName_( aTrig ),
  filterName_( aFilt ) {
}
// TriggerPrimitive::TriggerPrimitive( const reco::Particle::PolarLorentzVector & aVec, const std::string aTrig, const std::string aFilt ) :
//   lorentzVector_( aVec ),
//   triggerName_( aTrig ),
//   filterName_( aFilt ) {
// }

/// constructor from a TriggerPrimitive
TriggerPrimitive::TriggerPrimitive( const TriggerPrimitive & aPrim ) :
  lorentzVector_( aPrim.lorentzVector() ),
  triggerName_( aPrim.triggerName() ),
  filterName_( aPrim.filterName() ) {
}

/// destructor
TriggerPrimitive::~TriggerPrimitive() {
}

/// return Lorentz vector
const reco::Particle::LorentzVector & TriggerPrimitive::lorentzVector() const {
  return lorentzVector_;
}

/// return trigger name
const std::string & TriggerPrimitive::triggerName() const {
  return triggerName_;
}

/// return filter name
const std::string & TriggerPrimitive::filterName() const {
  return filterName_;
}

/// set Lorentz vector
void TriggerPrimitive::setLorentzVector( const reco::Particle::LorentzVector & aVec ) {
  lorentzVector_ = aVec;
}
// void TriggerPrimitive::setLorentzVector( const reco::Particle::PolarLorentzVector & aVec ) {
//   lorentzVector_ = aVec;
// }

/// set trigger name 
void TriggerPrimitive::setTriggerName( const std::string aTrig ) {
  triggerName_ = aTrig;
}

/// set filter name 
void TriggerPrimitive::setFilterName( const std::string aFilt ) {
  filterName_ = aFilt;
}
