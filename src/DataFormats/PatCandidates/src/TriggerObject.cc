//
// $Id:$
//

#include "DataFormats/PatCandidates/interface/TriggerObject.h"


using namespace pat;

/// default constructor
TriggerObject::TriggerObject() :
  reco::LeafCandidate()
{
}

/// constructor from values
TriggerObject::TriggerObject( const reco::Particle::LorentzVector & vec, const int id ) :
  reco::LeafCandidate( 0, vec, reco::Particle::Point( 0., 0., 0. ), id )
{
}
TriggerObject::TriggerObject( const reco::Particle::PolarLorentzVector & vec, const int id ) :
  reco::LeafCandidate( 0, vec, reco::Particle::Point( 0., 0., 0. ), id )
{
}
TriggerObject::TriggerObject( const trigger::TriggerObject & trigObj ) :
  reco::LeafCandidate( trigObj.particle() )
{
}

/// destructor
TriggerObject::~TriggerObject()
{
}
