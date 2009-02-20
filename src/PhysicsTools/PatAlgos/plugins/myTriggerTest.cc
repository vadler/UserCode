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
  edm::Service< TFileService > fileService;
  histos2D_[ "nPaths" ] = fileService->make< TH2D >( "nPaths", "Number of paths", 201, -0.5, 200.5, 201, -0.5, 200.5 );
  histos2D_[ "nPaths" ]->SetXTitle( "HLTConfigProvider" );
  histos2D_[ "nPaths" ]->SetYTitle( "pat::TriggerEvent->paths()" );
  histos2D_[ "nModules" ] = fileService->make< TH2D >( "nModules", "Number of modules per trigger path", 121, -0.5, 120.5, 121, -0.5, 120.5 );
  histos2D_[ "nModules" ]->SetXTitle( "HLTConfigProvider" );
  histos2D_[ "nModules" ]->SetYTitle( "pat::TriggerPath->modules()" );
  histos2D_[ "iLastModule" ] = fileService->make< TH2D >( "iLastModule", "Index of last active module per trigger path", 121, -0.5, 120.5, 121, -0.5, 120.5 );
  histos2D_[ "iLastModule" ]->SetXTitle( "HLTConfigProvider" );
  histos2D_[ "iLastModule" ]->SetYTitle( "pat::TriggerPath->lastActiveModule()" );
  histos1D_[ "nFilterIds" ] = fileService->make< TH1D >( "nFilterIds", "Number of filter IDs per trigger object", 6, -0.5, 5.5 );
  histos1D_[ "nFilterIds" ]->SetXTitle( "filter IDs" );
  histos1D_[ "nFilterIds" ]->SetYTitle( "entries" );
  histos1D_[ "filterIds" ] = fileService->make< TH1D >( "filterIds", "Filter IDs per trigger object", 25, 80.5, 106.5 );
  histos1D_[ "filterIds" ]->SetXTitle( "filter ID" );
  histos1D_[ "filterIds" ]->SetYTitle( "entries" );
  histos1D_[ "goodFilterIds" ] = fileService->make< TH1D >( "goodFilterIds", "Existing filter ID found?", 2, -0.5, 1.5 );
  histos1D_[ "goodFilterIds" ]->SetXTitle( "found" );
  histos1D_[ "goodFilterIds" ]->SetYTitle( "entries" );
  histos1D_[ "badFilterIds" ] = fileService->make< TH1D >( "badFilterIds", "Non-existing filter ID found?", 2, -0.5, 1.5 );
  histos1D_[ "badFilterIds" ]->SetXTitle( "found" );
  histos1D_[ "badFilterIds" ]->SetYTitle( "entries" );
  histos1D_[ "objectKeys" ] = fileService->make< TH1D >( "objectKeys", "Object keys per filter", 251, -0.5, 250.5 );
  histos1D_[ "objectKeys" ]->SetXTitle( "object keys" );
  histos1D_[ "objectKeys" ]->SetYTitle( "entries" );
  histos1D_[ "objectIds" ] = fileService->make< TH1D >( "objectIds", "Object IDs per filter", 25, 80.5, 105.5 );
  histos1D_[ "objectIds" ]->SetXTitle( "object IDs" );
  histos1D_[ "objectIds" ]->SetYTitle( "entries" );
  histos2D_[ "nObjectIdsKeys" ] = fileService->make< TH2D >( "nObjectIdsKeys", "Number of trigger object IDs vs. number of trigger object keys per trigger filter", 6, -0.5, 5.5, 6, -0.5, 5.5 );
  histos2D_[ "nObjectIdsKeys" ]->SetXTitle( "object keys" );
  histos2D_[ "nObjectIdsKeys" ]->SetYTitle( "object IDs" );
  
  // This should normally happen in beginRun()
  if ( ! hltConfig_.init( nameHLTProcess_ ) ) {
    edm::LogWarning( "hltConfigExtraction" ) << "HLT config extraction error with process name " << nameHLTProcess_;
  }
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
  
  const TriggerPathCollection   * myEventPaths( handlePatTriggerEvent->paths() );
  const TriggerFilterCollection * myEventFilters( handlePatTriggerEvent->filters() );
  const TriggerObjectCollection * myEventObjects( handlePatTriggerEvent->objects() );
  
  // Number checking
  
  if ( hltConfig_.size() == myEventPaths->size() ) {
    for ( TriggerPathCollection::const_iterator iPath = myEventPaths->begin(); iPath != myEventPaths->end(); ++iPath ) {
      const TriggerFilterRefVector     myFilters( handlePatTriggerEvent->pathFilters( iPath->name(), true ) );
      const std::vector< std::string > names( iPath->modules() );                             
      const std::vector<std::string> & namesHltConfig( hltConfig_.moduleLabels( iPath->name() ) );
      unsigned lastModule( iPath->lastActiveModule() );
      unsigned lastModuleTriggerResults( handleTriggerResults->index( handlePatTriggerEvent->indexPath( iPath->name() ) ) );
      if ( names.size() == namesHltConfig.size() && namesHltConfig.size() == myFilters.size() ) {
        for ( size_t iModule = 0; iModule < names.size(); ++iModule ) {
          if ( names[ iModule ] != namesHltConfig[ iModule ] ) {
            edm::LogWarning( "nameModule" ) << "    Module #" << iModule << " with different names in path " << iPath->name() << ":\n"
                                            << "        HLTConfigProvider: " << namesHltConfig[ iModule ] << "\n"
                                            << "        pat::TriggerPath : " << names[ iModule ];
          }
        }
      } else {
        edm::LogWarning( "nModules" ) << "    Number of HLT modules differ in path " << iPath->name() << ":\n"
                                      << "        HLTConfigProvider          : " << namesHltConfig.size() << "\n"
                                      << "        pat::TriggerPath           : " << names.size() << "\n"
                                      << "        pat::TriggerFilterRefVector: " << myFilters.size();
      }
      histos2D_[ "nModules" ]->Fill( namesHltConfig.size(), names.size() );
      if ( lastModule != lastModuleTriggerResults ) {
        edm::LogWarning( "iLastModule" ) << "    Index of last active HLT module differs in path " << iPath->name() << ":\n"
                                         << "        TriggerResults::HLT: " << lastModuleTriggerResults << "\n"
                                         << "        pat::TriggerPath   : " << lastModule;
      }
      histos2D_[ "iLastModule" ]->Fill( namesHltConfig.size(), names.size() );
    }
  } else {
    edm::LogWarning( "nPaths" ) << "    Number of HLT paths differ:\n"
                                << "        HLTConfigProvider         : " << hltConfig_.size() << "\n"
                                << "        pat::TriggerPathCollection: " << myEventPaths->size();
  }  
  histos2D_[ "nPaths" ]->Fill( hltConfig_.size(), myEventPaths->size() );
  
  // Method checking
  
  // pat::TriggerObject
  for ( TriggerObjectCollection::const_iterator iObject = myEventObjects->begin(); iObject != myEventObjects->end(); ++iObject ) {
    const std::vector< unsigned > ids( iObject->filterIds() );
    histos1D_[ "nFilterIds" ]->Fill( ids.size() );
    for ( size_t iId = 0; iId < ids.size(); ++iId ) {
      histos1D_[ "filterIds" ]->Fill( ids.at( iId ) );
      histos1D_[ "goodFilterIds" ]->Fill( iObject->hasFilterId( ids.at( iId ) ) );
      histos1D_[ "badFilterIds" ]->Fill( iObject->hasFilterId( ids.at( iId ) + 25 ) ); // arbitrarilly set not to interfere with existing IDs
    }
  }
  
  // pat::TriggerFilter
  for ( TriggerFilterCollection::const_iterator iFilter = myEventFilters->begin(); iFilter != myEventFilters->end(); ++iFilter ) {
    const std::vector< unsigned > keys( iFilter->objectKeys() );
    for ( size_t iKey = 0; iKey < keys.size(); ++iKey ) {
      if ( keys.at( iKey ) >= myEventObjects->size() ) {
        edm::LogWarning( "iObjectKey" ) << "    Object key too large in filter " << iFilter->label() << ":\n"
                                        << "        object key " << iKey << ": " << keys.at( iKey ) << "\n"
                                        << "        # of objects:" << myEventObjects->size();
      }
      histos1D_[ "objectKeys" ]->Fill( keys.at( iKey ) );
    }
    const std::vector< unsigned > ids( iFilter->objectIds() );
    for ( size_t iId = 0; iId < ids.size(); ++iId ) {
      histos1D_[ "objectIds" ]->Fill( ids.at( iId ) );
    }
    histos2D_[ "nObjectIdsKeys" ]->Fill( keys.size(), ids.size() );
  }
  
  // pat::TriggerPath
  
  // pat::TriggerEvent
  // trigger matches
//   for ( std::vector< std::string >::const_iterator iMatch = handlePatTriggerEvent->triggerMatchers().begin(); iMatch != handlePatTriggerEvent->triggerMatchers().end(); ++iMatch ) {
//     ;
//   }
  
}

void myTriggerTest::endRun()
{
}

void myTriggerTest::endJob()
{
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(myTriggerTest);
