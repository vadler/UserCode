//
// $Id$
//


#include "PhysicsTools/PatAlgos/plugins/PATTriggerProducer.h"

#include <vector>
#include <map>
#include <cassert>

#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"


using namespace pat;

PATTriggerProducer::PATTriggerProducer( const edm::ParameterSet & iConfig ) :
  nameProcess_( iConfig.getParameter< std::string >( "processName" ) ),
  tagTriggerResults_( iConfig.getParameter< edm::InputTag >( "triggerResults" ) ),
  tagTriggerEvent_( iConfig.getParameter< edm::InputTag >( "triggerEvent" ) )
{
  if ( tagTriggerResults_.process().empty() ) {
    tagTriggerResults_ = edm::InputTag( tagTriggerResults_.label(), tagTriggerResults_.instance(), nameProcess_ );
  }
  if ( tagTriggerEvent_.process().empty() ) {
    tagTriggerEvent_ = edm::InputTag( tagTriggerEvent_.label(), tagTriggerEvent_.instance(), nameProcess_ );
  }

  produces< TriggerObjectCollection >();
  produces< TriggerFilterCollection >();
  produces< TriggerPathCollection >();
  produces< TriggerEvent >();
}

PATTriggerProducer::~PATTriggerProducer()
{
}

void PATTriggerProducer::beginRun( edm::Run & iRun, const edm::EventSetup & iSetup )
{
  if ( ! hltConfig_.init( nameProcess_ ) ) {
    edm::LogError( "errorHltConfigExtraction" ) << "HLT config extraction error with process name " << nameProcess_;
    return;
  }                          
//   hltConfig_.dump( "ProcessPSet" ); // DEBUG
//   hltConfig_.dump( "TableName" );   // DEBUG
//   hltConfig_.dump( "Modules" );     // DEBUG
}

void PATTriggerProducer::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
  edm::Handle< edm::TriggerResults > handleTriggerResults;
  iEvent.getByLabel( tagTriggerResults_, handleTriggerResults );
  if ( ! handleTriggerResults.isValid() ) {
    edm::LogError( "errorTriggerResultsValid" ) << "TriggerResults product with InputTag " << tagTriggerResults_.encode() << " not in event";
    return;
  }
  edm::Handle< trigger::TriggerEvent > handleTriggerEvent;
  iEvent.getByLabel( tagTriggerEvent_, handleTriggerEvent );
  if ( ! handleTriggerEvent.isValid() ) {
    edm::LogError( "errorTriggerEventValid" ) << "TriggerEvent product with InputTag " << tagTriggerEvent_.encode() << " not in event";
    return;
  }
  const unsigned int sizePaths( hltConfig_.size() );
  const unsigned int sizeFilters( handleTriggerEvent->sizeFilters() );
  const unsigned int sizeObjects( handleTriggerEvent->sizeObjects() );
  const unsigned int sizeCollections( handleTriggerEvent->sizeCollections() );

  // produce trigger event
  
  std::string nameTable( hltConfig_.tableName() );
  std::auto_ptr< TriggerEvent > triggerEvent( new TriggerEvent( nameTable, handleTriggerResults->wasrun(), handleTriggerResults->accept(), handleTriggerResults->error() ) );

  // produce trigger paths
  
  std::auto_ptr< TriggerPathCollection > triggerPaths( new TriggerPathCollection() );
  triggerPaths->reserve( sizePaths );
  
//   std::vector< std::vector< std::string > > tableTriggerModulesInPaths; // transient table of modules in each path; maybe needed later (?)
//   tableTriggerModulesInPaths.reserve( sizePaths );

  for ( unsigned int iP = 0; iP < sizePaths; ++iP ) {
    std::string namePath( hltConfig_.triggerName( iP ) );
    unsigned int indexPath( hltConfig_.triggerIndex( namePath ) );
    TriggerPath triggerPath( namePath, 1, handleTriggerResults->wasrun( indexPath ), handleTriggerResults->accept( indexPath ), handleTriggerResults->error( indexPath ) );
    triggerPaths->push_back( triggerPath );
//     tableTriggerModulesInPaths.push_back( hltConfig_.moduleLabels( iP ) );
  }
  
  iEvent.put( triggerPaths );
  
  // produce trigger filters
  // from TriggerEvent, which contains only the "last active filter" (doesn't help for x-channels)
  
  std::auto_ptr< TriggerFilterCollection > triggerFilters( new TriggerFilterCollection() );
  triggerFilters->reserve( sizeFilters );
  
  std::map< trigger::size_type, int > filterIds;
  
  for ( unsigned int iF = 0; iF < sizeFilters; ++iF ) {
    std::string filterLabel( handleTriggerEvent->filterTag( iF ).label() );
    TriggerFilter triggerFilter( filterLabel );
    std::string filterType( hltConfig_.moduleType( filterLabel ) );
    triggerFilter.setType( filterType );
    triggerFilter.setWasRun( true ); // applies automatically only for "last active filter" from TriggerEvent
//     triggerFilter.setAccept(  ); // to be figured out
    triggerFilters->push_back( triggerFilter );
    // store used trigger object types
    const trigger::Vids & ids = handleTriggerEvent->filterIds( iF );   
    const trigger::Keys & keys = handleTriggerEvent->filterKeys( iF );
    const trigger::size_type sizeIds( ids.size() );
    const trigger::size_type sizeKeys( keys.size() );
    assert( sizeIds == sizeKeys );
    for ( unsigned int iK = 0; iK < sizeKeys; ++iK ) {
      if ( filterIds.find( keys[ iK ] ) == filterIds.end() ) {
        filterIds[ keys[ iK ] ] = ids[ iK ];
      } else if ( filterIds[ keys[ iK ] ] != ids[ iK ] ) {
        edm::LogWarning( "warningDiffVid" ) << "TriggerObject " << keys[ iK ] << " has differing types " << filterIds[ keys[ iK ] ] << " and " << ids[ iK ];
      }
    }
  }
  
  iEvent.put( triggerFilters );
  
  // produce trigger objects
  
  std::auto_ptr< TriggerObjectCollection > triggerObjects( new TriggerObjectCollection() );
  triggerObjects->reserve( sizeObjects );
  
  const trigger::Keys & collectionKeys( handleTriggerEvent->collectionKeys() );
  
  for ( unsigned int iO = 0, iC = 0; iO < sizeObjects && iC < sizeCollections; ++iO ) {
    while ( iO >= collectionKeys[ iC ] ) {
      ++iC;
    }
    TriggerObject triggerObject( handleTriggerEvent->getObjects().at( iO ) );
    std::string collectionTag( handleTriggerEvent->collectionTag( iC ).encode() );
    triggerObject.setCollection( collectionTag );
    // set type
    if ( filterIds.find( iO ) != filterIds.end() ) {
      triggerObject.setType( filterIds[ iO ] );
    }
    triggerObjects->push_back( triggerObject );
  }
  
  iEvent.put( triggerObjects );
  
  // produce inter-product references
  
  iEvent.put( triggerEvent );
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( PATTriggerProducer );
