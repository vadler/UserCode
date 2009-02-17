//
// $Id: myTriggerTest.cc,v 1.1.2.1 2008/12/18 11:19:38 vadler Exp $
//


#include "PhysicsTools/PatAlgos/plugins/myTriggerTest.h"


using namespace pat;

myTriggerTest::myTriggerTest( const edm::ParameterSet & iConfig ) :
  nameHLTProcess_( iConfig.getParameter< std::string >( "hltProcessName" ) ),
  tagTriggerResults_( iConfig.getParameter< edm::InputTag >( "triggerResults" ) ),
  tagTriggerEvent_( iConfig.getParameter< edm::InputTag >( "triggerEvent" ) ),
  namePATProcess_( iConfig.getParameter< std::string >( "patProcessName" ) ),
  tagPatTrigger_( iConfig.getParameter< edm::InputTag >( "patTrigger" ) ),
  tagPatTriggerEvent_( iConfig.getParameter< edm::InputTag >( "patTriggerEvent" ) ),
  histos1D_(),
  histos2D_()
{
  if ( tagTriggerResults_.process().empty() ) {
    tagTriggerResults_ = edm::InputTag( tagTriggerResults_.label(), tagTriggerResults_.instance(), nameHLTProcess_ );
  }
  if ( tagTriggerEvent_.process().empty() ) {
    tagTriggerEvent_ = edm::InputTag( tagTriggerEvent_.label(), tagTriggerEvent_.instance(), nameHLTProcess_ );
  }
  if ( tagPatTrigger_.process().empty() ) {
    tagPatTrigger_ = edm::InputTag( tagPatTrigger_.label(), tagPatTrigger_.instance(), namePATProcess_ );
  }
  if ( tagPatTriggerEvent_.process().empty() ) {
    tagPatTriggerEvent_ = edm::InputTag( tagPatTriggerEvent_.label(), tagPatTriggerEvent_.instance(), namePATProcess_ );
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
  iEvent.getByLabel( tagPatTriggerEvent_, handlePatTriggerEvent );
  edm::Handle< TriggerPathCollection > handlePatTriggerPaths;
  iEvent.getByLabel( tagPatTrigger_, handlePatTriggerPaths );
  edm::Handle< TriggerFilterCollection > handlePatTriggerFilters;
  iEvent.getByLabel( tagPatTrigger_, handlePatTriggerFilters );
  edm::Handle< TriggerObjectCollection > handlePatTriggerObjects;
  iEvent.getByLabel( tagPatTrigger_, handlePatTriggerObjects );
  
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
