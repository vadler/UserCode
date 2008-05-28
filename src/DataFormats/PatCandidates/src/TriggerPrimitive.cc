//
// $Id$
//

#include "DataFormats/PatCandidates/interface/TriggerPrimitive.h"


using namespace pat;

/// default constructor
TriggerPrimitive::TriggerPrimitive() :
  reco::LeafCandidate(),
  filterName_( "" ),
  triggerObjectType_( 0 ) {
}

/// copy constructor
TriggerPrimitive::TriggerPrimitive( const TriggerPrimitive & aTrigPrim ) :
  reco::LeafCandidate( 0, aTrigPrim.p4() ),
  filterName_( aTrigPrim.filterName() ),
  triggerObjectType_( aTrigPrim.triggerObjectType() ) {
}

/// constructor from values
TriggerPrimitive::TriggerPrimitive( const reco::Particle::LorentzVector & aVec, const std::string aFilt, const int aType ) :
  reco::LeafCandidate( 0, aVec ),
  filterName_( aFilt ),
  triggerObjectType_( aType ) {
}
TriggerPrimitive::TriggerPrimitive( const reco::Particle::PolarLorentzVector & aVec, const std::string aFilt, const int aType ) :
  reco::LeafCandidate( 0, aVec ),
  filterName_( aFilt ),
  triggerObjectType_( aType ) {
}

/// destructor
TriggerPrimitive::~TriggerPrimitive() {
}

/// clone method
TriggerPrimitive * TriggerPrimitive::clone() const {
  return new TriggerPrimitive( * this );
}

/// return filter name
const std::string & TriggerPrimitive::filterName() const {
  return filterName_;
}

/// return trigger object type
const int & TriggerPrimitive::triggerObjectType() const {
  return triggerObjectType_;
}

/// set filter name 
void TriggerPrimitive::setFilterName( const std::string aFilt ) {
  filterName_ = aFilt;
}

/// set trigger object type
void TriggerPrimitive::setTriggerObjectType( const int aType ) {
  triggerObjectType_ = aType;
}
