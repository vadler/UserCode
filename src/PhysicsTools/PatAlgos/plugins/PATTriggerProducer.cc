//
// $Id$
//


#include "PhysicsTools/PatAlgos/plugins/PATTriggerProducer.h"


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

  produces< TriggerPathCollection >();
  produces< TriggerFilterCollection >();
  produces< TriggerObjectCollection >();
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
}

void PATTriggerProducer::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
  if ( hltConfig_.size() <= 0 ) {
    edm::LogError( "errorHltConfigSize" ) << "HLT config size error" << "\n"
                                          << "Check for occurence of an \"errorHltConfigExtraction\" from beginRun()";
    return;
  }
  edm::Handle< edm::TriggerResults > handleTriggerResults;
  iEvent.getByLabel( tagTriggerResults_, handleTriggerResults );
  if ( ! handleTriggerResults.isValid() ) {
    edm::LogError( "errorTriggerResultsValid" ) << "edm::TriggerResults product with InputTag " << tagTriggerResults_.encode() << " not in event";
    return;
  }
  edm::Handle< trigger::TriggerEvent > handleTriggerEvent;
  iEvent.getByLabel( tagTriggerEvent_, handleTriggerEvent );
  if ( ! handleTriggerEvent.isValid() ) {
    edm::LogError( "errorTriggerEventValid" ) << "trigger::TriggerEvent product with InputTag " << tagTriggerEvent_.encode() << " not in event";
    return;
  }

  // produce trigger paths and determine status of modules
  
  const unsigned int sizePaths( hltConfig_.size() );
  
  std::auto_ptr< TriggerPathCollection > triggerPaths( new TriggerPathCollection() );
  triggerPaths->reserve( sizePaths );
  
  std::map< std::string, int > statesModules;

  for ( unsigned int iP = 0; iP < sizePaths; ++iP ) {
    // initialize path
    const std::string namePath( hltConfig_.triggerName( iP ) );
    const unsigned int indexPath( hltConfig_.triggerIndex( namePath ) );
    TriggerPath triggerPath( namePath, 1, handleTriggerResults->wasrun( indexPath ), handleTriggerResults->accept( indexPath ), handleTriggerResults->error( indexPath ), handleTriggerResults->index( indexPath ) );
    // add module names to path and states' map
    const unsigned int indexLastModule( handleTriggerResults->index( indexPath ) );
    const unsigned int sizeModules( hltConfig_.size( namePath ) );
    assert( indexLastModule < sizeModules );
    std::map< unsigned int, std::string > indicesModules;
    for ( unsigned int iM = 0; iM < sizeModules; ++iM ) {
      const std::string nameModule( hltConfig_.moduleLabel( indexPath, iM ) );
      const unsigned int indexModule( hltConfig_.moduleIndex( indexPath, nameModule ) );
      triggerPath.addModule( nameModule );
      indicesModules.insert( std::pair< unsigned int, std::string >( indexModule, nameModule ) );
    }
    // store path
    triggerPaths->push_back( triggerPath );
    // store module states to be used for the filters
    for ( std::map< unsigned int, std::string >::const_iterator iM = indicesModules.begin(); iM != indicesModules.end(); ++iM ) {
      if ( iM->first < indexLastModule ) {
        statesModules[ iM->second ] = 1;
      } else if ( iM->first == indexLastModule ) {
        statesModules[ iM->second ] = handleTriggerResults->accept( indexPath );
      } else if ( statesModules.find( iM->second ) == statesModules.end() ) {
        statesModules[ iM->second ] = -1;
      }
    }
  }
  
  iEvent.put( triggerPaths );
  
  // produce trigger filters and store used trigger object types
  // (trigger filters from TriggerEvent, which contains only the "last active filter" (doesn't help for x-channels))
  
  const unsigned int sizeFilters( handleTriggerEvent->sizeFilters() );
  
  std::auto_ptr< TriggerFilterCollection > triggerFilters( new TriggerFilterCollection() );
  triggerFilters->reserve( sizeFilters );
  
  std::multimap< trigger::size_type, int > filterIds;
  
  for ( unsigned int iF = 0; iF < sizeFilters; ++iF ) {
    const std::string nameFilter( handleTriggerEvent->filterTag( iF ).label() );
    TriggerFilter triggerFilter( nameFilter );
    // set filter type
    const std::string typeFilter( hltConfig_.moduleType( nameFilter ) );
    triggerFilter.setType( typeFilter );
    // set filter IDs of used objects
    const trigger::Keys & keys = handleTriggerEvent->filterKeys( iF );
    const trigger::Vids & ids = handleTriggerEvent->filterIds( iF );   
    const trigger::size_type sizeKeys( keys.size() );
    const trigger::size_type sizeIds( ids.size() );
    for ( unsigned int iK = 0; iK < sizeKeys; ++iK ) {
      const unsigned int objectKey( keys[ iK ] );
      if ( triggerFilter.hasObjectKey( objectKey ) == triggerFilter.objectKeys().size() ) {
        triggerFilter.addObjectKey( objectKey );
      }
    }
    for ( unsigned int iI = 0; iI < sizeIds; ++iI ) {
      const unsigned int objectId( ids[ iI ] );
      if ( ! triggerFilter.hasObjectId( objectId ) ) {
        triggerFilter.addObjectId( objectId );
      }
    }
    // set status from path info
    std::map< std::string, int >::iterator iS( statesModules.find( nameFilter ) );
    if ( iS != statesModules.end() ) {
      if ( ! triggerFilter.setStatus( iS->second ) ) {
        triggerFilter.setStatus( -1 ); // different code for "unvalid status determined" needed?
      }
    } else {
      triggerFilter.setStatus( -1 ); // different code for "unknown" needed?
    }
    // store filter
    triggerFilters->push_back( triggerFilter );
    // store used trigger object types to be used with the objects
    assert( sizeIds == sizeKeys );
    for ( unsigned int iK = 0; iK < sizeKeys; ++iK ) {
      filterIds.insert( std::pair< trigger::size_type, int >( keys[ iK ], ids[ iK ] ) );
    }
  }

  iEvent.put( triggerFilters );
  
  // produce trigger objects
  
  const unsigned int sizeObjects( handleTriggerEvent->sizeObjects() );
  
  std::auto_ptr< TriggerObjectCollection > triggerObjects( new TriggerObjectCollection() );
  triggerObjects->reserve( sizeObjects );
  
  const trigger::Keys & collectionKeys( handleTriggerEvent->collectionKeys() );
  for ( unsigned int iO = 0, iC = 0; iO < sizeObjects && iC < handleTriggerEvent->sizeCollections(); ++iO ) {
    TriggerObject triggerObject( handleTriggerEvent->getObjects().at( iO ) );
    // set collection
    while ( iO >= collectionKeys[ iC ] ) {
      ++iC;
    }
    const std::string collectionTag( handleTriggerEvent->collectionTag( iC ).encode() );
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
DEFINE_FWK_MODULE( PATTriggerProducer );
