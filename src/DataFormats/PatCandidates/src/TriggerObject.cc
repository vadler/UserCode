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

TriggerObject::TriggerObject( const reco::Particle::LorentzVector & vec, const int id, const int type ) :
  reco::LeafCandidate( 0, vec, reco::Particle::Point( 0., 0., 0. ), id ),
  type_( type )
{
}
TriggerObject::TriggerObject( const reco::Particle::PolarLorentzVector & vec, const int id, const int type ) :
  reco::LeafCandidate( 0, vec, reco::Particle::Point( 0., 0., 0. ), id ),
  type_( type )
{
}
TriggerObject::TriggerObject( const trigger::TriggerObject & trigObj, const int type ) :
  reco::LeafCandidate( trigObj.particle() ),
  type_( type )
{
}

/// destructor

TriggerObject::~TriggerObject()
{
}

/// object related

void TriggerObject::setCollection( std::string & collection )
{
  collection_ = collection;
}

void TriggerObject::setType( int type )
{
  type_ = type;
}

std::string TriggerObject::getCollection()
{
  return collection_;
}

int TriggerObject::getType()
{
  return type_;
}
      
/// filters related
      
/// PATObject match related
