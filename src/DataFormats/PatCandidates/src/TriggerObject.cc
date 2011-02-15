//
// $Id: TriggerObject.cc,v 1.12 2010/12/20 20:05:52 vadler Exp $
//

#include "DataFormats/PatCandidates/interface/TriggerObject.h"

#include "FWCore/Utilities/interface/EDMException.h"


using namespace pat;


// Constructors and Destructor


// Default constructor
TriggerObject::TriggerObject() :
  reco::LeafCandidate()
{
  triggerObjectTypes_.clear();
}


// Constructor from trigger::TriggerObject
TriggerObject::TriggerObject( const trigger::TriggerObject & trigObj ) :
  reco::LeafCandidate( 0, trigObj.particle().p4(), reco::Particle::Point( 0., 0., 0. ), trigObj.id() ),
  refToOrig_()
{
  triggerObjectTypes_.clear();
}


// Constructors from base class object
TriggerObject::TriggerObject( const reco::LeafCandidate & leafCand ) :
  reco::LeafCandidate( leafCand ),
  refToOrig_()
{
  triggerObjectTypes_.clear();
}


// Constructors from base candidate reference (for 'l1extra' particles)
TriggerObject::TriggerObject( const reco::CandidateBaseRef & candRef ) :
  reco::LeafCandidate( *candRef ),
  refToOrig_( candRef )
{
  triggerObjectTypes_.clear();
}


// Constructors from Lorentz-vectors and (optional) PDG ID
TriggerObject::TriggerObject( const reco::Particle::LorentzVector & vec, int id ) :
  reco::LeafCandidate( 0, vec, reco::Particle::Point( 0., 0., 0. ), id ),
  refToOrig_()
{
  triggerObjectTypes_.clear();
}
TriggerObject::TriggerObject( const reco::Particle::PolarLorentzVector & vec, int id ) :
  reco::LeafCandidate( 0, vec, reco::Particle::Point( 0., 0., 0. ), id ),
  refToOrig_()
{
  triggerObjectTypes_.clear();
}


// Methods


// Add a new trigger object type identifier
bool TriggerObject::addTriggerObjectType( trigger::TriggerObjectType triggerObjectType )
{
  // Add trigger object type in any case, if not present yet
  if ( ! hasTriggerObjectType( triggerObjectType ) ) triggerObjectTypes_.push_back( triggerObjectType );
  // Checks for consistency of HLT object type
  if ( triggerObjectType > 0 && isHltObject() > 0 ) return true;
  // Checks for consistency of L1 object type
  if ( triggerObjectType < 0 && isL1Object()  > 0 ) return true;
  // Trigger object type found to be inconsistent
  return false;
}


// Get all trigger object type identifiers
std::vector< int > TriggerObject::triggerObjectTypes() const
{
  std::vector< int > triggerObjectTypes;
  std::copy( triggerObjectTypes_.begin(), triggerObjectTypes_.end(), triggerObjectTypes.begin() );
  return triggerObjectTypes;
}


// Check for the "generic" trigger object type (HLT or L1)
int TriggerObject::isHltObject() const
{
  int reply( 2 );
  if ( triggerObjectTypes_.size() == 0 ) return reply;
  for ( size_t iTo = 0; iTo < triggerObjectTypes_.size(); ++iTo ) {
    if ( triggerObjectTypes_.at( iTo ) > 0 ) {
      if ( reply == 0 ) return -1;
      reply = 1;
    } else if ( triggerObjectTypes_.at( iTo ) < 0 ) {
      if ( reply == 1 ) return -1;
      reply = 0;
    }
  }
  return reply;
}
int TriggerObject::isL1Object() const
{
  int reply( 2 );
  if ( triggerObjectTypes_.size() == 0 ) return reply;
  for ( size_t iTo = 0; iTo < triggerObjectTypes_.size(); ++iTo ) {
    if ( triggerObjectTypes_.at( iTo ) < 0 ) {
      if ( reply == 0 ) return -1;
      reply = 1;
    } else if ( triggerObjectTypes_.at( iTo ) > 0 ) {
      if ( reply == 1 ) return -1;
      reply = 0;
    }
  }
  return reply;
}


// Checks, if a certain label of original collection is assigned
bool TriggerObject::hasCollection( const std::string & collName ) const
{
  // True, if collection name is simply fine
  if ( collName == collection_ ) return true;
  // Check, if collection name possibly fits in an edm::InputTag approach
  const edm::InputTag collectionTag( collection_ );
  const edm::InputTag collTag( collName );
  // If evaluated collection tag contains a process name, it must have been found already by identity check
  if ( collTag.process().empty() ) {
    // Check instance ...
    if ( ( collTag.instance().empty() && collectionTag.instance().empty() ) || collTag.instance() == collectionTag.instance() ) {
      // ... and label
      return ( collTag.label() == collectionTag.label() );
    }
  }
  return false;
}


// Checks, if a certain trigger object type identifier is assigned
bool TriggerObject::hasTriggerObjectType( trigger::TriggerObjectType triggerObjectType ) const
{
  for ( size_t iF = 0; iF < triggerObjectTypes_.size(); ++iF ) {
    if ( triggerObjectType == triggerObjectTypes_.at( iF ) ) return true;
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
