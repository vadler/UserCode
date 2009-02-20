//
// $Id: TriggerObject.cc,v 1.1.2.1 2008/12/18 11:26:16 vadler Exp $
//

#include "DataFormats/PatCandidates/interface/TriggerObject.h"


using namespace pat;

/// default constructor

TriggerObject::TriggerObject() :
  reco::LeafCandidate()
{
}

/// constructors from values

TriggerObject::TriggerObject( const reco::Particle::LorentzVector & vec, int id ) :
  reco::LeafCandidate( 0, vec, reco::Particle::Point( 0., 0., 0. ), id )
{
  filterIds_.clear();
}
TriggerObject::TriggerObject( const reco::Particle::PolarLorentzVector & vec, int id ) :
  reco::LeafCandidate( 0, vec, reco::Particle::Point( 0., 0., 0. ), id )
{
  filterIds_.clear();
}
TriggerObject::TriggerObject( const trigger::TriggerObject & trigObj ) :
  reco::LeafCandidate( trigObj.particle() )
{
  filterIds_.clear();
}

/// getters

bool TriggerObject::hasFilterId( unsigned filterId ) const
{
  for ( std::vector< unsigned >::const_iterator iF = filterIds_.begin(); iF != filterIds_.end(); ++iF ) {
    if ( *iF == filterId ) {
      return true;
    }
  }
  return false;
}
