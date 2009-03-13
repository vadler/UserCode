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
  for ( unsigned iF = 0; iF < filterIds().size(); ++iF ) {
    if ( filterIds().at( iF ) == filterId ) {
      return true;
    }
  }
  return false;
}

/// transient methods

bool TriggerObject::hasFilterLabel( const std::string & filterLabel ) const
{
  for ( unsigned iFilter = 0; iFilter < filterLabels().size(); ++iFilter ) {
    if ( filterLabel == filterLabels().at( iFilter ) ) {
      return true;
    }
  }
  return false;
}

bool TriggerObject::hasPathName( const std::string & pathName ) const
{
  for ( unsigned iPath = 0; iPath < pathNames().size(); ++iPath ) {
    if ( pathName == pathNames().at( iPath ) ) {
      return true;
    }
  }
  return false;
}
