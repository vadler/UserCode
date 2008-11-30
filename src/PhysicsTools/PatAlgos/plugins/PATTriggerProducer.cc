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

  produces< TriggerObjectCollection >();
  produces< TriggerFilterCollection >();
  produces< TriggerPathCollection >();
  produces< TriggerEvent >();
}

PATTriggerProducer::~PATTriggerProducer()
{
}

void PATTriggerProducer::beginRun( const edm::Run & iRun, const edm::EventSetup & iSetup )
{
  if ( ! hltConfig_.init( nameProcess_ ) ) {
    edm::LogError( "errorHltConfigExtraction" ) << "HLT config extraction error with process name " << nameProcess_;
  }
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
  unsigned int sizePaths( hltConfig_.size() );
  unsigned int sizeFilters( handleTriggerEvent->sizeFilters() );
  unsigned int sizeObjects( handleTriggerEvent->sizeObjects() );

  // produce trigger event
  std::string nameTable( hltConfig_.tableName() );
  TriggerEvent * ptrTriggerEvent( new TriggerEvent( nameTable, handleTriggerResults->wasrun(), handleTriggerResults->accept(), handleTriggerResults->error() ) ) ;

  // produce trigger paths
  TriggerPathCollection * ptrTriggerPaths( new TriggerPathCollection() );
  ptrTriggerPaths->reserve( sizePaths );
  std::vector< std::vector< std::string > > tableTriggerModulesInPaths; // transient table of modules in each path; maybe needed later (?)
  tableTriggerModulesInPaths.reserve( sizePaths );
  for ( unsigned int i = 0; i < sizePaths; ++i ) {
    std::string  namePath( hltConfig_.triggerName( i ) );
    unsigned int indexPath( hltConfig_.triggerIndex( namePath ) );
    TriggerPath triggerPath( namePath, 1, handleTriggerResults->wasrun( indexPath ), handleTriggerResults->accept( indexPath ), handleTriggerResults->error( indexPath ) );
    ptrTriggerPaths->push_back( triggerPath );
    tableTriggerModulesInPaths.push_back( hltConfig_.moduleLabels( i ) );
  }
  
  // produce trigger filters
  // from TriggerEvent, which contains only the "last active filter" (doesn't help for x-channels)
  TriggerFilterCollection * ptrTriggerFilters( new TriggerFilterCollection() );
  ptrTriggerFilters->reserve( sizeFilters );
  for ( unsigned int i = 0; i < sizeFilters; ++i ) {
     std::string filterLabel( handleTriggerEvent->filterTag( i ).label() );
     std::string filterType( hltConfig_.moduleType( filterLabel ) );
     TriggerFilter triggerFilter( filterLabel, filterType );
     triggerFilter.setWasRun( true );
//      triggerFilter.setAccept(  ); // to be figured out
     ptrTriggerFilters->push_back( triggerFilter );
  }
  
  // produce trigger objects
  TriggerObjectCollection * ptrTriggerObjects( new TriggerObjectCollection() );
  ptrTriggerObjects->reserve( sizeObjects );
  for ( unsigned int i = 0; i < sizeObjects; ++i ) {
    TriggerObject triggerObject( handleTriggerEvent->getObjects().at( i ) );
    ptrTriggerObjects->push_back( triggerObject );
  }
  
  // connect trigger objects to filters
  
  // connect trigger filters to paths
  
  std::auto_ptr< TriggerObjectCollection > triggerObjects( ptrTriggerObjects );
  iEvent.put( triggerObjects );
  delete ptrTriggerObjects;
  std::auto_ptr< TriggerFilterCollection > triggerFilters( ptrTriggerFilters );
  iEvent.put( triggerFilters );
  delete ptrTriggerFilters;
  std::auto_ptr< TriggerPathCollection > triggerPaths( ptrTriggerPaths );
  iEvent.put( triggerPaths );
  delete ptrTriggerPaths;
  std::auto_ptr< TriggerEvent > triggerEvent( ptrTriggerEvent );
  iEvent.put( triggerEvent );
  delete ptrTriggerEvent;
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( PATTriggerProducer );
