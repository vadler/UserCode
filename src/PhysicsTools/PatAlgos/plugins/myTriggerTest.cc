//
// $Id$
//


#include "PhysicsTools/PatAlgos/plugins/myTriggerTest.h"


using namespace pat;

myTriggerTest::myTriggerTest( const edm::ParameterSet & iConfig ) :
  nameHLTProcess_( iConfig.getParameter< std::string >( "hltProcessName" ) ),
  tagTriggerResults_( iConfig.getParameter< edm::InputTag >( "triggerResults" ) ),
  tagTriggerEvent_( iConfig.getParameter< edm::InputTag >( "triggerEvent" ) ),
  namePATProcess_( iConfig.getParameter< std::string >( "patProcessName" ) ),
  tagLayer0Trigger_( iConfig.getParameter< edm::InputTag >( "layer0Trigger" ) ),
  tagLayer1Trigger_( iConfig.getParameter< edm::InputTag >( "layer1Trigger" ) ),
  histos1D_(),
  histos2D_()
{
  if ( tagTriggerResults_.process().empty() ) {
    tagTriggerResults_ = edm::InputTag( tagTriggerResults_.label(), tagTriggerResults_.instance(), nameHLTProcess_ );
  }
  if ( tagTriggerEvent_.process().empty() ) {
    tagTriggerEvent_ = edm::InputTag( tagTriggerEvent_.label(), tagTriggerEvent_.instance(), nameHLTProcess_ );
  }
  if ( tagLayer0Trigger_.process().empty() ) {
    tagLayer0Trigger_ = edm::InputTag( tagLayer0Trigger_.label(), tagLayer0Trigger_.instance(), namePATProcess_ );
  }
  if ( tagLayer1Trigger_.process().empty() ) {
    tagLayer1Trigger_ = edm::InputTag( tagLayer1Trigger_.label(), tagLayer1Trigger_.instance(), namePATProcess_ );
  }
}

myTriggerTest::~myTriggerTest()
{
}

void myTriggerTest::beginJob( const edm::EventSetup & iSetup )
{
  if ( hltConfig_.init( nameHLTProcess_ ) ) { // This should happen in beginRun()
    edm::LogError( "errorHltConfigExtraction" ) << "HLT config extraction error with process name " << nameHLTProcess_;
  }
                            
  edm::Service< TFileService > fileService;
  histos2D_[ "nPathsComp" ] = fileService->make< TH2D >( "nPathsComp", "Number of paths in pat::TriggerEvent vs. HLTConfigProvider", 201, -0.5, 200.5, 201, -0.5, 200.5 );
}

void myTriggerTest::beginRun( edm::Run & iRun, const edm::EventSetup & iSetup )
{
}

void myTriggerTest::analyze( const edm::Event & iEvent, const edm::EventSetup & iSetup )
{
  edm::Handle< edm::TriggerResults > handleTriggerResults;
  iEvent.getByLabel( tagTriggerResults_, handleTriggerResults );
  edm::Handle< trigger::TriggerEvent > handleTriggerEvent;
  iEvent.getByLabel( tagTriggerEvent_, handleTriggerEvent );
  edm::Handle< TriggerEvent > handlePatTriggerEvent;
  iEvent.getByLabel( tagLayer1Trigger_, handlePatTriggerEvent );
  edm::Handle< TriggerPathCollection > handlePatTriggerPaths;
  iEvent.getByLabel( tagLayer0Trigger_, handlePatTriggerPaths );
  edm::Handle< TriggerFilterCollection > handlePatTriggerFilters;
  iEvent.getByLabel( tagLayer0Trigger_, handlePatTriggerFilters );
  edm::Handle< TriggerObjectCollection > handlePatTriggerObjects;
  iEvent.getByLabel( tagLayer0Trigger_, handlePatTriggerObjects );
  
  histos2D_[ "nPathsComp" ]->Fill( hltConfig_.size(), handlePatTriggerEvent->paths()->size() );
  
  const TriggerPathCollection * myEventPaths( handlePatTriggerEvent->paths() );
  const unsigned int sizePaths( handlePatTriggerPaths->size() );
  const unsigned int sizeEventPaths( myEventPaths->size() );
  if ( sizePaths == sizeEventPaths ) {
    for ( unsigned int iP = 0; iP < sizePaths; ++iP ) {
      if ( handlePatTriggerPaths->at( iP ).name() != myEventPaths->at( iP ).name() ) {
        edm::LogError( "errorNamePath" ) << iP << ": namePath (" << handlePatTriggerPaths->at( iP ).name() << ") and nameEventPath (" << myEventPaths->at( iP ).name() << ") differ";
      } else {
        edm::LogWarning( "fineNamePath" ) << iP << ": path name " << handlePatTriggerPaths->at( iP ).name() << " fine";
      }
    }
  } else {
    edm::LogError( "errorSizePaths" ) << "sizePaths (" << sizePaths << ") and sizeEventPaths (" << sizeEventPaths << ") differ";
  }
}

void myTriggerTest::endRun()
{
}

void myTriggerTest::endJob()
{
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(myTriggerTest);
