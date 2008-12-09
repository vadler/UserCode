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
  
  std::map< trigger::size_type, int > filterIds;
  for ( unsigned int iF = 0; iF < handleTriggerEvent->sizeFilters(); ++iF ) {
    const trigger::Vids & ids = handleTriggerEvent->filterIds( iF );   
    const trigger::Keys & keys = handleTriggerEvent->filterKeys( iF );
    const trigger::size_type sizeIds( ids.size() );
    const trigger::size_type sizeKeys( keys.size() );
    assert( sizeIds == sizeKeys );
    for ( unsigned int iK = 0; iK < sizeKeys; ++iK ) {
      if ( filterIds.find( keys[ iK ] ) == filterIds.end() ) {
        filterIds[ keys[ iK ] ] = ids[ iK ];
      } else if ( filterIds[ keys[ iK ] ] != ids[ iK ] ) {
        edm::LogWarning( "warningDiffVid" ) << "trigger::TriggerObject " << keys[ iK ] << " has differing types " << filterIds[ keys[ iK ] ] << " and " << ids[ iK ] << "\n"
                                            << "Type " << filterIds[ keys[ iK ] ] << " is used for the pat::TriggerObject";
      }
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
    // set type
    if ( filterIds.find( iO ) != filterIds.end() ) {
      triggerObject.setType( filterIds[ iO ] );
    }
    triggerObjects->push_back( triggerObject );
  }
  
  iEvent.put( triggerObjects );
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( PATTriggerObjectProducer );
