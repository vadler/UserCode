//
// $Id$
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

/// destructor

TriggerObject::~TriggerObject()
{
}

/// setters

void TriggerObject::setCollection( const std::string & collection )
{
  collection_ = collection;
}

void TriggerObject::addFilterId( unsigned int filterId )
{
  filterIds_.push_back( filterId );
}

/// getters

std::string TriggerObject::collection() const
{
  return collection_;
}

std::vector< unsigned int > TriggerObject::filterIds() const
{
  return filterIds_;
}

bool TriggerObject::hasFilterId( unsigned int filterId ) const
{
  for ( std::vector< unsigned int >::const_iterator iF = filterIds_.begin(); iF != filterIds_.end(); ++iF ) {
    if ( *iF == filterId ) {
      return true;
    }
  }
  return false;
}
