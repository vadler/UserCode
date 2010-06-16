//
// $Id: TriggerObject.cc,v 1.6 2010/04/20 21:39:46 vadler Exp $
//

#include "DataFormats/PatCandidates/interface/TriggerObject.h"
#include <iostream> // DEBUG


using namespace pat;


/// default constructor

TriggerObject::TriggerObject() :
  reco::LeafCandidate()
{
  filterIds_.clear();
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
  reco::LeafCandidate( 0, trigObj.particle().p4(), reco::Particle::Point( 0., 0., 0. ), trigObj.id() )
{
  filterIds_.clear();
}
TriggerObject::TriggerObject( const reco::LeafCandidate & leafCand ) :
  reco::LeafCandidate( leafCand )
{
  filterIds_.clear();
}

/// getters

bool TriggerObject::hasCollection( const std::string & coll ) const
{
//   std::cout << "hasCollection(): trying strings '" << coll << "' and '" << collection() << "'" << std::endl; // DEBUG
  if ( collection() == coll ) return true;
  const edm::InputTag collectionTag( collection() );
  const edm::InputTag collTag( coll );
  std::cout << "hasCollection(): trying process '" << collTag.process() << "'" << std::endl; // DEBUG
  if ( collTag.process().empty() ) {
    std::cout << "hasCollection(): trying instances '" << collTag.instance() << "' and '" << collectionTag.instance() << "'" << std::endl; // DEBUG
    if ( ( collTag.instance().empty() && collectionTag.instance().empty() ) || collTag.instance() == collectionTag.instance() ) {
      std::cout << "hasCollection(): trying labels " << collTag.label() << "' and '" << collectionTag.label() << "'" << std::endl; // DEBUG
// DEBUG      if ( collTag.label() == collectionTag.label() ) return true;
      if ( collTag.label() == collectionTag.label() ) { // DEBUG
        std::cout << "hasCollection(): fine!" << std::endl; // DEBUG
        return true; // DEBUG
      } // DEBUG
    }
  }
  return false;
}

bool TriggerObject::hasFilterId( int filterId ) const
{
  for ( size_t iF = 0; iF < filterIds().size(); ++iF ) {
    if ( filterIds().at( iF ) == filterId ) {
      return true;
    }
  }
  return false;
}
