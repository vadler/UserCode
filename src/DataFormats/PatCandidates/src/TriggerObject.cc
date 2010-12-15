//
// $Id: TriggerObject.cc,v 1.10 2010/12/14 19:31:36 vadler Exp $
//

#include "DataFormats/PatCandidates/interface/TriggerObject.h"

#include "FWCore/Utilities/interface/EDMException.h"


using namespace pat;


// Constructors and Destructor


// Default constructor
TriggerObject::TriggerObject() :
  reco::LeafCandidate()
{
  filterIds_.clear();
}


// Constructor from trigger::TriggerObject
TriggerObject::TriggerObject( const trigger::TriggerObject & trigObj ) :
  reco::LeafCandidate( 0, trigObj.particle().p4(), reco::Particle::Point( 0., 0., 0. ), trigObj.id() ),
  refToOrig_()
{
  filterIds_.clear();
}


// Constructors from reco::Candidate
TriggerObject::TriggerObject( const reco::LeafCandidate & leafCand ) :
  reco::LeafCandidate( leafCand ),
  refToOrig_()
{
  filterIds_.clear();
}
TriggerObject::TriggerObject( const reco::CandidateBaseRef & candRef ) :
  reco::LeafCandidate( *candRef ),
  refToOrig_( candRef )
{
  filterIds_.clear();
}


// Constructors from Lorentz-vectors and (optional) PDG ID
TriggerObject::TriggerObject( const reco::Particle::LorentzVector & vec, int id ) :
  reco::LeafCandidate( 0, vec, reco::Particle::Point( 0., 0., 0. ), id ),
  refToOrig_()
{
  filterIds_.clear();
}
TriggerObject::TriggerObject( const reco::Particle::PolarLorentzVector & vec, int id ) :
  reco::LeafCandidate( 0, vec, reco::Particle::Point( 0., 0., 0. ), id ),
  refToOrig_()
{
  filterIds_.clear();
}


// Methods


// Checks, if a certain label of original collection is assigned
bool TriggerObject::hasCollection( const std::string & coll ) const
{
  // True, if collection name is simply fine
  if ( collection() == coll ) return true;
  // Check, if collection name possibly fits in an edm::InputTag approach
  const edm::InputTag collectionTag( collection() );
  const edm::InputTag collTag( coll );
  // If evaluated collection tag contains a process name, it must have been found already by identity check
  if ( collTag.process().empty() ) {
    // Check instance ...
    if ( ( collTag.instance().empty() && collectionTag.instance().empty() ) || collTag.instance() == collectionTag.instance() ) {
      // ... and label
      if ( collTag.label() == collectionTag.label() ) return true;
    }
  }
  return false;
}


// Checks, if a certain trigger object type identifier is assigned
bool TriggerObject::hasFilterId( trigger::TriggerObjectType filterId ) const
{
  for ( size_t iF = 0; iF < filterIds().size(); ++iF ) {
    if ( filterIds().at( iF ) == filterId ) {
      return true;
    }
  }
  return false;
}


// Special methods for 'l1extra' particles


// Getters specific to the 'l1extra' particle types
// Exceptions of type 'edm::errors::InvalidReference' are thrown,
// if wrong particle type is requested

// EM
const l1extra::L1EmParticleRef TriggerObject::origL1EmRef() const
{
  l1extra::L1EmParticleRef l1Ref;
  try {
    l1Ref = origObjRef().castTo< l1extra::L1EmParticleRef >();
  } catch ( edm::Exception X ) {
    if ( X.categoryCode() != edm::errors::InvalidReference ) throw X;
  }
  return l1Ref;
}

// EtMiss
const l1extra::L1EtMissParticleRef TriggerObject::origL1EtMissRef() const
{
  l1extra::L1EtMissParticleRef l1Ref;
  try {
    l1Ref = origObjRef().castTo< l1extra::L1EtMissParticleRef >();
  } catch ( edm::Exception X ) {
    if ( X.categoryCode() != edm::errors::InvalidReference ) throw X;
  }
  return l1Ref;
}

// Jet
const l1extra::L1JetParticleRef TriggerObject::origL1JetRef() const
{
  l1extra::L1JetParticleRef l1Ref;
  try {
    l1Ref = origObjRef().castTo< l1extra::L1JetParticleRef >();
  } catch ( edm::Exception X ) {
    if ( X.categoryCode() != edm::errors::InvalidReference ) throw X;
  }
  return l1Ref;
}

// Muon
const l1extra::L1MuonParticleRef TriggerObject::origL1MuonRef() const
{
  l1extra::L1MuonParticleRef l1Ref;
  try {
    l1Ref = origObjRef().castTo< l1extra::L1MuonParticleRef >();
  } catch ( edm::Exception X ) {
    if ( X.categoryCode() != edm::errors::InvalidReference ) throw X;
  }
  return l1Ref;
}
