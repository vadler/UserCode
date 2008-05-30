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
  triggerEvent_ (iConfig.getParameter<InputTag>( "triggerEvent" ) ),
  filterName_   (iConfig.getParameter<InputTag>( "filterName" ) )
{
  produces<TriggerPrimitiveCollection>();
}


PATTrigProducer::~PATTrigProducer()
{
}


void PATTrigProducer::produce( Event& iEvent, const EventSetup& iSetup )
{
  auto_ptr<TriggerPrimitiveCollection> patTrigCandidates( new TriggerPrimitiveCollection );
  Handle<TriggerEvent> triggerEvent;
  iEvent.getByLabel( triggerEvent_, triggerEvent );
  size_type nFilters = triggerEvent->sizeFilters();
  if ( nFilters == 0 ) {
    LogDebug( "noTriggerFilters" ) << "PATTrigProducer: The TriggerEvent of this event contains no filter informationat all!";
  } else {
    size_type iFilter = triggerEvent->filterIndex( filterName_.label() );
    if ( iFilter == nFilters ) {
      LogDebug( "TriggerFilterNotFound" ) << "PATTrigProducer: The TriggerEvent of this event contains no filter information on filter " << filterName_.label() << "!";
    } else {
      const Vids &                    triggerIds     = triggerEvent->filterIds( iFilter );
      const Keys &                    triggerKeys    = triggerEvent->filterKeys( iFilter );
      const TriggerObjectCollection & triggerObjects = triggerEvent->getObjects();
      assert( triggerIds.size() == triggerKeys.size() );
      for ( size_type idx = 0; idx < triggerKeys.size(); ++idx ) {
        const TriggerObject triggerObject = triggerObjects.at( triggerKeys.at( idx ) );
        auto_ptr<TriggerPrimitive> ptr( new TriggerPrimitive( triggerObject.particle().p4(), filterName_.label(), triggerIds.at( idx ), triggerObject.id() ) );
        patTrigCandidates->push_back( ptr );
      }
    } 
  }
  iEvent.put( patTrigCandidates );
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(PATTrigProducer);
