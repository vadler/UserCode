//
// $Id:$
//


#include "PhysicsTools/PatAlgos/interface/PATHLTProducer.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/HLTFilterObject.h"

#include "FWCore/Framework/interface/TriggerNames.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "FWCore/Utilities/interface/Exception.h"

using namespace pat;

PATHLTProducer::PATHLTProducer( const edm::ParameterSet& iConfig ) :
  // initialize
  triggerResults_ (iConfig.getParameter<edm::InputTag>( "triggerResults" ) ),
  triggerName_    (iConfig.getParameter<std::string>  ( "triggerName" ) ),
  filterName_     (iConfig.getParameter<edm::InputTag>( "filterName" ) )
{
  produces<reco::CandidateCollection>();
}


pat::PATHLTProducer::~PATHLTProducer()
{
}


void PATHLTProducer::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
  // create (already) object to be written to the event
  std::auto_ptr<reco::CandidateCollection> patHltCandidates( new reco::CandidateCollection );
  
  // get trigger results and trigger module info from event
  edm::Handle<edm::TriggerResults> triggerResults;
  iEvent.getByLabel( triggerResults_, triggerResults );
  edm::Handle<reco::HLTFilterObjectWithRefs> hltFilter;
  iEvent.getByLabel( filterName_, hltFilter );
//   if ( ! hltFilter.isValid() ) {
//     throw cms::Exception( "notpresentTriggerModule" ) << "PATHLTProducer: The filter module " << filterName_.label() << " is not present in this event " << triggerName_ << "!\n";
//     iEvent.put( patHltCandidates );
//     return;
//   }
  
  // check trigger requested module and path
  edm::TriggerNames triggerNames( *triggerResults );
  unsigned int triggerIndex = triggerNames.triggerIndex( triggerName_ );
  if ( triggerIndex == triggerNames.size() )
    throw cms::Exception( "wrongTriggerPath" ) << "PATHLTProducer: The trigger path " << triggerName_ << " is not known in this event!\n";
  if ( ! triggerResults->wasrun( triggerIndex ) )
    throw cms::Exception( "notrunTriggerPath" ) << "PATHLTProducer: The trigger path " << triggerName_ << " was not run in this event!\n";
  if ( ! triggerResults->accept( triggerIndex ) )
    throw cms::Exception( "notacceptTriggerPath" ) << "PATHLTProducer: The trigger path " << triggerName_ << " did not accept this event!\n";
  if (   triggerResults->error ( triggerIndex ) )
    throw cms::Exception( "errorTriggerPath" ) << "PATHLTProducer: The trigger path " << triggerName_ << " had an error in this event!\n";
  if ( triggerIndex != hltFilter->path() )
    throw cms::Exception( "wrongTriggerModule" ) 
    << "PATHLTProducer: The filter module " << filterName_.label() << " does not belong to the trigger path " << triggerName_ << "!\n";
  
  // loop over trigger objects and store trigger candidates
  for ( unsigned int iTriggerObject = 0; iTriggerObject < hltFilter->size(); ++iTriggerObject ) {
    const reco::Candidate * patHltCandidate( &(hltFilter->at(iTriggerObject)) );
    std::auto_ptr<reco::Candidate> ptr( patHltCandidate->clone() );
    patHltCandidates->push_back( ptr );        
  }
  iEvent.put( patHltCandidates );
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(PATHLTProducer);
