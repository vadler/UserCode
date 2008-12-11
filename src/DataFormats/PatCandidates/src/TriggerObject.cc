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

TriggerObject::TriggerObject( const reco::Particle::LorentzVector & vec, const int id ) :
  reco::LeafCandidate( 0, vec, reco::Particle::Point( 0., 0., 0. ), id )
{
  filterIds_.clear();
}
TriggerObject::TriggerObject( const reco::Particle::PolarLorentzVector & vec, const int id ) :
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

void TriggerObject::setCollection( std::string & collection )
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

unsigned int TriggerObject::filterId( const unsigned int i ) const
{
  return filterIds_[ i ];
}

unsigned int TriggerObject::sizeFilterIds() const
{
  return filterIds_.size();
}

bool TriggerObject::hasFilterId( const unsigned int filterId ) const
{
  bool found = false;
  for ( std::vector< unsigned int >::const_iterator iF = filterIds_.begin(); iF != filterIds_.end(); ++iF ) {
    if ( *iF == filterId ) {
      found = true;
      break;
    }
  }
  return found;
}
