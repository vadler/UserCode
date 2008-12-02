//
// $Id$
//


#include "PhysicsTools/PatAlgos/plugins/PATTriggerProducer.h"

#include <algorithm>

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
  }                          
  hltConfig_.dump( "ProcessPSet" ); // DEBUG
  hltConfig_.dump( "TableName" );   // DEBUG
  hltConfig_.dump( "Modules" );     // DEBUG
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
  for ( unsigned int i = 0; i < sizePaths; ++i ) {
    std::string namePath( hltConfig_.triggerName( i ) );
    unsigned int indexPath( hltConfig_.triggerIndex( namePath ) );
    TriggerPath triggerPath( namePath, 1, handleTriggerResults->wasrun( indexPath ), handleTriggerResults->accept( indexPath ), handleTriggerResults->error( indexPath ) );
    triggerPaths->push_back( triggerPath );
//     tableTriggerModulesInPaths.push_back( hltConfig_.moduleLabels( i ) );
  }
  
  // produce trigger filters
  // from TriggerEvent, which contains only the "last active filter" (doesn't help for x-channels)
  std::auto_ptr< TriggerFilterCollection > triggerFilters( new TriggerFilterCollection() );
  triggerFilters->reserve( sizeFilters );
  for ( unsigned int i = 0; i < sizeFilters; ++i ) {
     std::string filterLabel( handleTriggerEvent->filterTag( i ).label() );
     std::string filterType( hltConfig_.moduleType( filterLabel ) );
     TriggerFilter triggerFilter( filterLabel, filterType );
     triggerFilter.setWasRun( true ); // applies automatically only for "last active filter" from TriggerEvent
//      triggerFilter.setAccept(  ); // to be figured out
     triggerFilters->push_back( triggerFilter );
  }
  
  // produce trigger objects
  std::auto_ptr< TriggerObjectCollection > triggerObjects( new TriggerObjectCollection() );
  triggerObjects->reserve( sizeObjects );
  const trigger::Keys & collectionKeys( handleTriggerEvent->collectionKeys() );
  for ( unsigned int i = 0, j = 0; i < sizeObjects && j < sizeCollections; ++i ) {
    while ( i >= collectionKeys.at( j ) ) {
      ++j;
    }
    TriggerObject triggerObject( handleTriggerEvent->getObjects().at( i ) );
    std::string collectionTag( handleTriggerEvent->collectionTag( j ).encode() );
    triggerObject.setCollection( collectionTag );
    triggerObjects->push_back( triggerObject );
  }
  
  // connect trigger objects to filters
  for ( unsigned int i = 0; i < triggerFilters->size(); ++i ) {
    const trigger::Vids & vids = handleTriggerEvent->filterIds( i );   
    const trigger::Keys & keys = handleTriggerEvent->filterKeys( i );
    for ( unsigned int j = 0; j < std::max( vids.size(), keys.size() ); ++j ) {
      if ( triggerObjects->at( keys[ j ] ).getType() != 0 && triggerObjects->at( keys[ j ] ).getType() != vids[ j ] ) {
        edm::LogWarning( "warningDiffVid" ) << "TriggerObject " << keys[ j ] << " has differing types " << triggerObjects->at( keys[ j ] ).getType() << " and " << vids[ j ];
      }
      triggerObjects->at( keys[ j ] ).setType( vids[ j ] );
    }   
  }
  
  // connect trigger filters to paths
  
  // put products into the event
  iEvent.put( triggerObjects );
  iEvent.put( triggerFilters );
  iEvent.put( triggerPaths );
  iEvent.put( triggerEvent );
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( PATTriggerProducer );
