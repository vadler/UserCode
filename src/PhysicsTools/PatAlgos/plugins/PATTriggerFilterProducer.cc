//
// $Id$
//


#include "PhysicsTools/PatAlgos/plugins/PATTriggerFilterProducer.h"


using namespace pat;

PATTriggerFilterProducer::PATTriggerFilterProducer( const edm::ParameterSet & iConfig ) :
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

  produces< TriggerFilterCollection >();
}

PATTriggerFilterProducer::~PATTriggerFilterProducer()
{
}

void PATTriggerFilterProducer::beginRun( edm::Run & iRun, const edm::EventSetup & iSetup )
{
  if ( ! hltConfig_.init( nameProcess_ ) ) {
    edm::LogError( "errorHltConfigExtraction" ) << "HLT config extraction error with process name " << nameProcess_;
    return;
  }                          
}

void PATTriggerFilterProducer::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
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
  const unsigned int sizeFilters( handleTriggerEvent->sizeFilters() );
  
  // determine status of modules
  
  std::map< std::string, int > moduleStates;
  
  for ( unsigned int iP = 0; iP < hltConfig_.size(); ++iP ) {
    const std::string pathName( hltConfig_.triggerName( iP ) );
    const unsigned int pathIndex( hltConfig_.triggerIndex( pathName ) );
    const unsigned int lastModuleIndex( handleTriggerResults->index( pathIndex ) );
    const unsigned int sizeModules( hltConfig_.size( pathName ) );
    assert( lastModuleIndex < sizeModules );
    std::map< unsigned int, std::string > moduleIndices;
    for ( unsigned int iM = 0; iM < sizeModules; ++iM ) {
      const std::string moduleName( hltConfig_.moduleLabel( pathIndex, iM ) );
      const unsigned int moduleIndex( hltConfig_.moduleIndex( pathIndex, moduleName ) );
      moduleIndices.insert( std::pair< unsigned int, std::string >( moduleIndex, moduleName ) );
    }
    for ( std::map< unsigned int, std::string >::iterator iM = moduleIndices.begin(); iM != moduleIndices.end(); ++iM ) {
      if ( iM->first < lastModuleIndex ) {
        moduleStates[ iM->second ] = 1;
      } else if ( iM->first == lastModuleIndex ) {
        moduleStates[ iM->second ] = handleTriggerResults->accept( pathIndex );
      } else if ( moduleStates.find( iM->second ) == moduleStates.end() ) {
        moduleStates[ iM->second ] = -1;
      }
    }
  }
  
  // produce trigger filters
  // from TriggerEvent, which contains only the "last active filter" (doesn't help for x-channels)
  
  std::auto_ptr< TriggerFilterCollection > triggerFilters( new TriggerFilterCollection() );
  triggerFilters->reserve( sizeFilters );
  
  for ( unsigned int iF = 0; iF < sizeFilters; ++iF ) {
    std::string filterLabel( handleTriggerEvent->filterTag( iF ).label() );
    TriggerFilter triggerFilter( filterLabel );
    // set filter type
    std::string filterType( hltConfig_.moduleType( filterLabel ) );
    triggerFilter.setType( filterType );
    // set input collection
//     triggerFilter.setInputCollection(  ); // to be figured out
    // set filter IDs of used objects
    const trigger::Vids & ids = handleTriggerEvent->filterIds( iF );   
    for ( unsigned int iI = 0; iI < ids.size(); ++iI ) {
      const unsigned int objectId( ids[ iI ] );
      if ( ! triggerFilter.hasObjectId( objectId ) ) {
        triggerFilter.addObjectId( objectId );
      }
    }
    // set status
    std::map< std::string, int >::iterator iS( moduleStates.find( filterLabel ) );
    if ( iS != moduleStates.end() ) {
      if ( ! triggerFilter.setStatus( iS->second ) ) {
        triggerFilter.setStatus( -1 ); // different code for "unvalid status determined" needed?
      }
    } else {
      triggerFilter.setStatus( -1 ); // different code for "unknown" needed?
    }

    triggerFilters->push_back( triggerFilter );
  }
  
  iEvent.put( triggerFilters );
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( PATTriggerFilterProducer );
