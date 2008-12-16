//
// $Id$
//


#include "PhysicsTools/PatAlgos/plugins/PATTriggerEventProducer.h"


using namespace pat;

PATTriggerEventProducer::PATTriggerEventProducer( const edm::ParameterSet & iConfig ) :
  nameProcess_( iConfig.getParameter< std::string >( "processName" ) ),
  tagTriggerResults_( iConfig.getParameter< edm::InputTag >( "triggerResults" ) ),
  tagTriggerEvent_( iConfig.getParameter< edm::InputTag >( "triggerEvent" ) ),
  tagLayer0Trigger_( iConfig.getParameter< edm::InputTag >( "layer0Trigger" ) )
{
  if ( tagTriggerResults_.process().empty() ) {
    tagTriggerResults_ = edm::InputTag( tagTriggerResults_.label(), tagTriggerResults_.instance(), nameProcess_ );
  }
  if ( tagTriggerEvent_.process().empty() ) {
    tagTriggerEvent_ = edm::InputTag( tagTriggerEvent_.label(), tagTriggerEvent_.instance(), nameProcess_ );
  }

  produces< TriggerEvent >();
}

PATTriggerEventProducer::~PATTriggerEventProducer()
{
}

void PATTriggerEventProducer::beginRun( edm::Run & iRun, const edm::EventSetup & iSetup )
{
  if ( ! hltConfig_.init( nameProcess_ ) ) {
    edm::LogError( "errorHltConfigExtraction" ) << "HLT config extraction error with process name " << nameProcess_;
    return;
  }                          
}

void PATTriggerEventProducer::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
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
  edm::Handle< TriggerPathCollection > handleTriggerPaths;
  iEvent.getByLabel( tagLayer0Trigger_, handleTriggerPaths );
  edm::Handle< TriggerFilterCollection > handleTriggerFilters;
  iEvent.getByLabel( tagLayer0Trigger_, handleTriggerFilters );
  edm::Handle< TriggerObjectCollection > handleTriggerObjects;
  iEvent.getByLabel( tagLayer0Trigger_, handleTriggerObjects );

  // produce trigger event
  
  std::string nameTable( hltConfig_.tableName() );
  std::auto_ptr< TriggerEvent > triggerEvent( new TriggerEvent( nameTable, handleTriggerResults->wasrun(), handleTriggerResults->accept(), handleTriggerResults->error() ) );
  // set product references to layer 0 collections
  if ( handleTriggerPaths.isValid() ) {
    triggerEvent->setPaths( handleTriggerPaths );
  } else {
    edm::LogError( "errorLayer0TriggerPathsValid" ) << "pat::TriggerPathCollection product with InputTag " << tagLayer0Trigger_.encode() << " not in event";
  }
  if ( handleTriggerFilters.isValid() ) {
    triggerEvent->setFilters( handleTriggerFilters );
  } else {
    edm::LogError( "errorLayer0TriggerFiltersValid" ) << "pat::TriggerFilterCollection product with InputTag " << tagLayer0Trigger_.encode() << " not in event";
  }
  if ( handleTriggerObjects.isValid() ) {
    triggerEvent->setObjects( handleTriggerObjects );
  } else {
    edm::LogError( "errorLayer0TriggerObjectsValid" ) << "pat::TriggerObjectCollection product with InputTag " << tagLayer0Trigger_.encode() << " not in event";
  }
  
  iEvent.put( triggerEvent );
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( PATTriggerEventProducer );
