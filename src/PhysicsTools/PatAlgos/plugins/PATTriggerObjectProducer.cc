//
// $Id$
//


#include "PhysicsTools/PatAlgos/plugins/PATTriggerObjectProducer.h"


using namespace pat;

PATTriggerObjectProducer::PATTriggerObjectProducer( const edm::ParameterSet & iConfig ) :
  nameProcess_( iConfig.getParameter< std::string >( "processName" ) ),
  tagTriggerEvent_( iConfig.getParameter< edm::InputTag >( "triggerEvent" ) )
{
  if ( tagTriggerEvent_.process().empty() ) {
    tagTriggerEvent_ = edm::InputTag( tagTriggerEvent_.label(), tagTriggerEvent_.instance(), nameProcess_ );
  }

  produces< TriggerObjectCollection >();
}

PATTriggerObjectProducer::~PATTriggerObjectProducer()
{
}

void PATTriggerObjectProducer::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
  edm::Handle< trigger::TriggerEvent > handleTriggerEvent;
  iEvent.getByLabel( tagTriggerEvent_, handleTriggerEvent );
  if ( ! handleTriggerEvent.isValid() ) {
    edm::LogError( "errorTriggerEventValid" ) << "trigger::TriggerEvent product with InputTag " << tagTriggerEvent_.encode() << " not in event";
    return;
  }
  const unsigned int sizeObjects( handleTriggerEvent->sizeObjects() );
  
  // store used trigger object types
  
  std::multimap< trigger::size_type, int > filterIds;
  
  for ( unsigned int iF = 0; iF < handleTriggerEvent->sizeFilters(); ++iF ) {
    const trigger::Keys & keys = handleTriggerEvent->filterKeys( iF );
    const trigger::Vids & ids = handleTriggerEvent->filterIds( iF );   
    const trigger::size_type sizeKeys( keys.size() );
    const trigger::size_type sizeIds( ids.size() );
    assert( sizeIds == sizeKeys );
    for ( unsigned int iK = 0; iK < sizeKeys; ++iK ) {
      filterIds.insert( std::pair< trigger::size_type, int >( keys[ iK ], ids[ iK ] ) );
    }
  }
  
  // produce trigger objects
  
  std::auto_ptr< TriggerObjectCollection > triggerObjects( new TriggerObjectCollection() );
  triggerObjects->reserve( sizeObjects );
  
  const trigger::Keys & collectionKeys( handleTriggerEvent->collectionKeys() );
  for ( unsigned int iO = 0, iC = 0; iO < sizeObjects && iC < handleTriggerEvent->sizeCollections(); ++iO ) {
    TriggerObject triggerObject( handleTriggerEvent->getObjects().at( iO ) );
    // set collection
    while ( iO >= collectionKeys[ iC ] ) {
      ++iC;
    }
    std::string collectionTag( handleTriggerEvent->collectionTag( iC ).encode() );
    triggerObject.setCollection( collectionTag );
    // set filter ID
    for ( std::multimap< trigger::size_type, int >::iterator iM = filterIds.begin(); iM != filterIds.end(); ++iM ) {
      if ( iM->first == iO && ! triggerObject.hasFilterId( iM->second ) ) {
        triggerObject.addFilterId( iM->second );
      }
    }
    
    triggerObjects->push_back( triggerObject );
  }
  
  iEvent.put( triggerObjects );
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( PATTriggerObjectProducer );
