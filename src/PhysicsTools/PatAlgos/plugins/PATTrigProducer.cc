//
// $Id$
//


#include "PhysicsTools/PatAlgos/plugins/PATTrigProducer.h"

#include "DataFormats/HLTReco/interface/TriggerEvent.h"

#include "DataFormats/PatCandidates/interface/TriggerPrimitive.h"


using namespace pat;
using namespace trigger;
using namespace edm;
using namespace std;


PATTrigProducer::PATTrigProducer( const ParameterSet & iConfig ) :
  // initialize
  triggerEvent_ (iConfig.getParameter<InputTag>   ( "triggerEvent" ) ),
  filterName_   (iConfig.getParameter<std::string>( "filterName" ) )
{
  produces<TriggerPrimitiveCollection>();
}


PATTrigProducer::~PATTrigProducer()
{
}


void PATTrigProducer::produce( Event& iEvent, const EventSetup& iSetup )
{
  auto_ptr<TriggerPrimitiveCollection> patHltCandidates( new TriggerPrimitiveCollection );
  Handle<TriggerEvent> triggerEvent;
  iEvent.getByLabel( triggerEvent_, triggerEvent );
  size_type iFilter = triggerEvent->filterIndex( filterName_ );
  const Vids & triggerIds     = triggerEvent->filterIds( iFilter );
  const Keys & triggerObjects = triggerEvent->filterKeys( iFilter );
  assert( triggerIds.size() == triggerObjects.size() );
//   for (  ) {
//     auto_ptr<TriggerPrimitive> ptr( new TriggerPrimitive( (hltFilter->at(iTriggeredObject)).p4(), triggerName_, filterName_.label() ) );
//     patHltCandidates->push_back( ptr );
//   }
  iEvent.put( patHltCandidates );
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(PATTrigProducer);
