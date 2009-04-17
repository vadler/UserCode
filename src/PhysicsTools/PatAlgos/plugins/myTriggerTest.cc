//
// $Id: myTriggerTest.cc,v 1.1.2.12 2009/03/27 21:08:46 vadler Exp $
//


#include "PhysicsTools/PatAlgos/plugins/myTriggerTest.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/Common/interface/AssociativeIterator.h"
#include "DataFormats/Candidate/interface/Candidate.h"

#include "TMath.h"

using namespace pat;
using namespace TMath;

myTriggerTest::myTriggerTest( const edm::ParameterSet & iConfig ) :
  nameHLTProcess_( iConfig.getParameter< std::string >( "hltProcessName" ) ),
  tagTriggerResults_( iConfig.getParameter< edm::InputTag >( "triggerResults" ) ),
  tagTriggerEvent_( iConfig.getParameter< edm::InputTag >( "triggerEvent" ) ),
  namePATProcess_( iConfig.getParameter< std::string >( "patProcessName" ) ),
  tagPatTrigger_( iConfig.getParameter< edm::InputTag >( "patTrigger" ) ),
  tagPatTriggerEvent_( iConfig.getParameter< edm::InputTag >( "patTriggerEvent" ) ),
  testPathModuleTags_( iConfig.getParameter< bool >( "testPathModuleTags" ) ),
  displayNumbers_( iConfig.getParameter< bool >( "displayNumbers" ) ),
  displayObjects_( iConfig.getParameter< bool >( "displayObjects" ) ),
  displayObjectsStandAlone_( iConfig.getParameter< bool >( "displayObjectsStandAlone" ) ),
  displayFilters_( iConfig.getParameter< bool >( "displayFilters" ) ),
  displayPaths_( iConfig.getParameter< bool >( "displayPaths" ) ),
  displayEvent_( iConfig.getParameter< bool >( "displayEvent" ) ),
  displayMatches_( iConfig.getParameter< bool >( "displayMatches" ) ),
  displayEmbedding_( iConfig.getParameter< bool >( "displayEmbedding" ) ),
  tagPatPhotons_( iConfig.getParameter< edm::InputTag >( "patPhotons" ) ),
  tagPatElectrons_( iConfig.getParameter< edm::InputTag >( "patElectrons" ) ),
  tagPatMuons_( iConfig.getParameter< edm::InputTag >( "patMuons" ) ),
  tagPatTaus_( iConfig.getParameter< edm::InputTag >( "patTaus" ) ),
  tagPatJets_( iConfig.getParameter< edm::InputTag >( "patJets" ) ),
  tagPatMETs_( iConfig.getParameter< edm::InputTag >( "patMETs" ) ),
  tagPatPhotonsEmbedding_( iConfig.getParameter< edm::InputTag >( "patPhotonsEmbedding" ) ),
  tagPatElectronsEmbedding_( iConfig.getParameter< edm::InputTag >( "patElectronsEmbedding" ) ),
  tagPatMuonsEmbedding_( iConfig.getParameter< edm::InputTag >( "patMuonsEmbedding" ) ),
  tagPatTausEmbedding_( iConfig.getParameter< edm::InputTag >( "patTausEmbedding" ) ),
  tagPatJetsEmbedding_( iConfig.getParameter< edm::InputTag >( "patJetsEmbedding" ) ),
  tagPatMETsEmbedding_( iConfig.getParameter< edm::InputTag >( "patMETsEmbedding" ) ),
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
  if ( testPathModuleTags_ ) {
    histos2D_[ "nModules" ] = fileService->make< TH2D >( "nModules", "Number of modules per trigger path", 121, -0.5, 120.5, 121, -0.5, 120.5 );
    histos2D_[ "nModules" ]->SetXTitle( "HLTConfigProvider" );
    histos2D_[ "nModules" ]->SetYTitle( "pat::TriggerPath->modules()" );
  }
  histos2D_[ "nFilters" ] = fileService->make< TH2D >( "nFilters", "Number of active filters per trigger path", 6, -0.5, 5.5, 6, -0.5, 5.5 );
  histos2D_[ "nFilters" ]->SetXTitle( "pat::TriggerEvent->pathFilters" );
  histos2D_[ "nFilters" ]->SetYTitle( "pat::TriggerPath->filterIndices()" );
  histos2D_[ "iLastFilter" ] = fileService->make< TH2D >( "iLastFilter", "Index of last active module per trigger path", 121, -0.5, 120.5, 121, -0.5, 120.5 );
  histos2D_[ "iLastFilter" ]->SetXTitle( "HLTConfigProvider.size()" );
  histos2D_[ "iLastFilter" ]->SetYTitle( "pat::TriggerPath->lastActiveFilterSlot()" );
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
  histos1D_[ "pathAcceptedAccept" ] = fileService->make< TH1D >( "pathAcceptedAccept", "Accepted path accepted?", 2, -0.5, 1.5 );
  histos1D_[ "pathAcceptedAccept" ]->SetXTitle( "accepted" );
  histos1D_[ "pathAcceptedAccept" ]->SetYTitle( "entries" );
  histos1D_[ "filterAcceptedStatus" ] = fileService->make< TH1D >( "filterAcceptedStatus", "Accepted filter status", 3, -1.5, 1.5 );
  histos1D_[ "filterAcceptedStatus" ]->SetXTitle( "status" );
  histos1D_[ "filterAcceptedStatus" ]->SetYTitle( "entries" );
  histos2D_[ "objectId" ] = fileService->make< TH2D >( "objectId", "Object filter IDs vs. filter object IDs", 25, 80.5, 105.5, 25, 80.5, 105.5 );
  histos2D_[ "objectId" ]->SetXTitle( "filter object ID" );
  histos2D_[ "objectId" ]->SetYTitle( "object filter ID" );
  histos2D_[ "ptObjCand" ] = fileService->make< TH2D >( "ptObjCand", "Object vs. candidate p_{T}", 60, 0., 300., 60, 0., 300. );
  histos2D_[ "ptObjCand" ]->SetXTitle( "candidate" );
  histos2D_[ "ptObjCand" ]->SetYTitle( "object" );
  histos2D_[ "etaObjCand" ] = fileService->make< TH2D >( "etaObjCand", "Object vs. candidate #eta", 50, -2.5, 2.5, 50, -2.5, 2.5 );
  histos2D_[ "etaObjCand" ]->SetXTitle( "candidate" );
  histos2D_[ "etaObjCand" ]->SetYTitle( "object" );
  histos2D_[ "phiObjCand" ] = fileService->make< TH2D >( "phiObjCand", "Object vs. candidate #phi", 60, -Pi(), Pi(), 60, -Pi(), Pi() );
  histos2D_[ "phiObjCand" ]->SetXTitle( "candidate" );
  histos2D_[ "phiObjCand" ]->SetYTitle( "object" );
  
  // This should normally happen in beginRun()
  if ( ! hltConfig_.init( nameHLTProcess_ ) ) {
    edm::LogError( "hltConfigExtraction" ) << "HLT config extraction error with process name " << nameHLTProcess_;
  }
}

void myTriggerTest::beginRun( edm::Run & iRun, const edm::EventSetup & iSetup )
{
// FIXME Why does this not work  h e r e , but in the c'tor?
//   if ( ! hltConfig_.init( nameHLTProcess_ ) ) {
//     edm::LogError( "hltConfigExtraction" ) << "HLT config extraction error with process name " << nameHLTProcess_;
//   }
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
  edm::Handle< TriggerObjectStandAloneCollection > handlePatTriggerObjectsStandAlone;
  iEvent.getByLabel( tagPatTrigger_, handlePatTriggerObjectsStandAlone );
  
  const TriggerPathCollection   * myEventPaths( handlePatTriggerEvent->paths() );
  const TriggerFilterCollection * myEventFilters( handlePatTriggerEvent->filters() );
  const TriggerObjectCollection * myEventObjects( handlePatTriggerEvent->objects() );
  
  // Number checking
  
  if ( hltConfig_.size() == myEventPaths->size() ) {
    for ( TriggerPathCollection::const_iterator iPath = myEventPaths->begin(); iPath != myEventPaths->end(); ++iPath ) {
      const TriggerFilterRefVector       myModules( handlePatTriggerEvent->pathModules( iPath->name(), true ) );
      const std::vector< std::string > & namesHltConfig( hltConfig_.moduleLabels( iPath->name() ) );
      unsigned lastFilter( iPath->lastActiveFilterSlot() );
      bool     lastFilterDecision(  );
      unsigned lastModuleTriggerResults( handleTriggerResults->index( handlePatTriggerEvent->indexPath( iPath->name() ) ) );
      if ( testPathModuleTags_ ) {
        const std::vector< std::string > namesModules( iPath->modules() );
        if ( namesModules.size() == namesHltConfig.size() && namesHltConfig.size() == myModules.size() ) {
          for ( size_t iModule = 0; iModule < namesModules.size(); ++iModule ) {
            if ( namesModules[ iModule ] != namesHltConfig[ iModule ] ) {
              edm::LogError( "nameModule" ) << "    Module #" << iModule << " with different namesModules in path " << iPath->name() << ":\n"
                                            << "        HLTConfigProvider: " << namesHltConfig[ iModule ] << "\n"
                                            << "        pat::TriggerPath : " << namesModules[ iModule ];
            }
          }
        } else if ( namesModules.size() > 0 ) {
          edm::LogError( "nModules" ) << "    Number of HLT modules differ in path " << iPath->name() << ":\n"
                                      << "        HLTConfigProvider          : " << namesHltConfig.size() << "\n"
                                      << "        pat::TriggerPath           : " << namesModules.size() << "\n"
                                      << "        pat::TriggerFilterRefVector: " << myModules.size();
        }
        histos2D_[ "nModules" ]->Fill( namesHltConfig.size(), namesModules.size() );
      }
      if ( lastFilter != lastModuleTriggerResults ) {
        edm::LogError( "iLastFilter" ) << "    Index of last active HLT module differs in path " << iPath->name() << ":\n"
                                       << "        TriggerResults::HLT: " << lastModuleTriggerResults << "\n"
                                       << "        pat::TriggerPath   : " << lastFilter;
      }
      const TriggerFilterRefVector  myFilters( handlePatTriggerEvent->pathFilters( iPath->name() ) );
      if ( iPath->filterIndices().size() != myFilters.size() ) {
        edm::LogError( "nFilters" ) << "    Number of HLT active filters differ in path " << iPath->name() << ":\n"
                                    << "        pat::TriggerPath           : " << iPath->filterIndices().size() << "\n"
                                    << "        pat::TriggerFilterRefVector: " << myFilters.size();
      }
      histos2D_[ "nFilters" ]->Fill( myFilters.size(), iPath->filterIndices().size() );
      histos2D_[ "iLastFilter" ]->Fill( namesHltConfig.size(), lastFilter );
    }
  } else {
    edm::LogError( "nPaths" ) << "    Number of HLT paths differ:\n"
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
  
  // pat::TriggerObjectStandAlone
  
  // pat::TriggerFilter
  
  for ( TriggerFilterCollection::const_iterator iFilter = myEventFilters->begin(); iFilter != myEventFilters->end(); ++iFilter ) {
    const std::vector< unsigned > keys( iFilter->objectKeys() );
    for ( size_t iKey = 0; iKey < keys.size(); ++iKey ) {
      if ( keys.at( iKey ) >= myEventObjects->size() ) {
        edm::LogError( "iObjectKey" ) << "    Object key too large in filter " << iFilter->label() << ":\n"
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
  
  for ( TriggerPathCollection::const_iterator iPath = myEventPaths->begin(); iPath != myEventPaths->end(); ++iPath ) {
    if ( ! iPath->wasAccept() ) {
      const std::string labelLastActiveModule( hltConfig_.moduleLabel( iPath->index(), iPath->lastActiveFilterSlot() ) );
      if ( iPath->filterIndices().size() > 0 ) {
        const std::string labelLastActiveFilter( handlePatTriggerFilters->at( iPath->filterIndices().size() - 1 ).label() );
        if ( labelLastActiveModule != labelLastActiveFilter ) {
          if ( ! handlePatTriggerEvent->filter( labelLastActiveModule ) ) {
            if ( displayPaths_ ) edm::LogWarning( "lastActiveLabel" ) << "    Labels for last active module differ in failing path " << iPath->name() << ":\n"
                                                                      << "        from stored last active filter slot   :"                                           << labelLastActiveModule << " --> not found!" <<  "\n"
                                                                      << "        from link to last active filter    (" << iPath->filterIndices().size() - 1 << "):" << labelLastActiveFilter << " --> " << handlePatTriggerEvent->filter( labelLastActiveFilter )->status();
          } else {
            if ( displayPaths_ ) edm::LogWarning( "lastActiveLabel" ) << "    Labels for last active module differ in failing path " << iPath->name() << ":\n"
                                                                      << "        from stored last active filter slot:"                                              << labelLastActiveModule << " --> " << handlePatTriggerEvent->filter( labelLastActiveModule )->status() << "\n"
                                                                      << "        from link to last active filter    (" << iPath->filterIndices().size() - 1 << "):" << labelLastActiveFilter << " --> " << handlePatTriggerEvent->filter( labelLastActiveFilter )->status();
          }
          for ( size_t iFilter = 0; iFilter < iPath->filterIndices().size(); ++iFilter ) {
            if ( myEventFilters->at( iFilter ).label() == labelLastActiveModule ) {
              if ( displayPaths_ ) edm::LogWarning( "lastActiveLabelFound" ) << "        but found it here: " << iFilter;
              break;
            }
          }
        }
      } else {
        if ( displayPaths_ ) edm::LogWarning( "filterIndices" ) << "    Active filters empty in failing path " << iPath->name() << ":\n"
                                                                << "        last active module from last active filter slot: " << labelLastActiveModule;
      }
    }
    if ( testPathModuleTags_ ) {
      const std::vector< std::string > pathModules( iPath->modules() );
      unsigned found( 0 );
      for ( size_t iModule = 0; iModule < pathModules.size(); ++iModule ) {
        if ( handlePatTriggerEvent->indexFilter( pathModules.at( iModule ) ) == myEventFilters->size() ) {
          if ( displayPaths_ ) edm::LogWarning( "moduleNotInFilters" ) << "    Module from path not found in filter collection:\n"
                                                                       << "        path  : " << iPath->name() << "\n"
                                                                       << "        module: " << pathModules.at( iModule );
        } else {
          ++found;
          if ( displayPaths_ ) edm::LogWarning( "moduleInFilters" ) << "    Module from path found in filter collection:\n"
                                                                    << "        path  : " << iPath->name() << "\n"
                                                                    << "        module: " << pathModules.at( iModule );
        }
      }
      if ( displayPaths_ ) edm::LogWarning( "foundFilters" ) << "    Found filters in path " << iPath->name() << ": " << found;
    }
  }
  
  // pat::TriggerEvent
  
  const TriggerPathRefVector pathRefs( handlePatTriggerEvent->acceptedPaths() );
  for ( TriggerPathRefVector::const_iterator iPath = pathRefs.begin(); iPath != pathRefs.end(); ++iPath ) {
    if ( ! ( *iPath )->wasAccept() ) {
      edm::LogError( "pathAccept" ) << "    Not-accepted path in collection of accepted paths:\n"
                                    << "        path name: " << ( *iPath )->name();
    }
    histos1D_[ "pathAcceptedAccept" ]->Fill( ( *iPath )->wasAccept() );
  }
  const TriggerFilterRefVector filterRefs( handlePatTriggerEvent->acceptedFilters() );
  for ( TriggerFilterRefVector::const_iterator iFilter = filterRefs.begin(); iFilter != filterRefs.end(); ++iFilter ) {
    if ( ( *iFilter )->status() != 1 ) {
      edm::LogError( "filterAccept" ) << "    Not-accepted filter in collection of accepted filter:\n"
                                      << "        filter label: " << ( *iFilter )->label() << "\n"
                                      << "        status      : " << ( *iFilter )->status();
    }
    histos1D_[ "filterAcceptedStatus" ]->Fill( ( *iFilter )->status() );
    const std::vector< unsigned > ids( ( *iFilter )->objectIds() );
    for ( size_t iId = 0; iId < ids.size(); ++iId ) {
      const TriggerObjectRefVector objectRefs( handlePatTriggerEvent->objects( ids.at( iId ) ) );
      for ( TriggerObjectRefVector::const_iterator iObject = objectRefs.begin(); iObject != objectRefs.end(); ++iObject ) {
        const std::vector< unsigned > ids2( ( *iObject )->filterIds() );
        if ( ! ( *iObject )->hasFilterId( ids.at( iId ) ) ) {
          edm::LogError( "objectFilterId" ) << "    Wrong filter ID found:\n"
                                            << "        filter           : " << ( *iFilter )->label() << "\n"
                                            << "        filter object ID : " << ids.at( iId ) << "\n"
                                            << "        object key       : " << ( *iObject ).key();
        }
        for ( size_t iId2 = 0; iId2 < ids2.size(); ++iId2 ) {
          histos2D_[ "objectId" ]->Fill( ids.at( iId ), ids2.at( iId2 ) );
        } 
      }
    }
  }
  
  // trigger matches
  
  const TriggerObjectMatchContainer * triggerMatches( handlePatTriggerEvent->triggerObjectMatchResults() );
  TriggerObjectMatchContainer triggerMatchContainer( *triggerMatches ); // cannot be const due to usage of 'operator[]'
  const std::vector< std::string > matches( handlePatTriggerEvent->triggerMatchers() );
  for ( size_t iMatch = 0; iMatch < matches.size(); ++iMatch ) {
    bool continueHere( false );
    const std::string match( matches.at( iMatch ) );
    if ( displayMatches_ ) edm::LogWarning( "matcherLabel" ) << "Matcher label: " << match;
    edm::Handle< TriggerObjectStandAloneMatch > triggerStandAloneMatch;
    iEvent.getByLabel( match, triggerStandAloneMatch );
    if ( ! triggerStandAloneMatch.isValid() ) {
      edm::LogError( "missingStandAloneMatch" ) << "    Stand-alone match " << match << " missing";
      continueHere = true;
    } else if ( triggerStandAloneMatch->empty() ) {
      edm::LogWarning( "emptyStandAloneMatch" ) << "    Stand-alone match " << match << " empty";
    }
    edm::Handle< TriggerObjectMatch > triggerMatch;
    const edm::InputTag matchTag( tagPatTriggerEvent_.label(), match ); // pat::TriggerObjectMatch produced by PATTriggerEventProducer with the matcher label as instance
    iEvent.getByLabel( matchTag, triggerMatch );
    if ( ! triggerMatch.isValid() ) {
      edm::LogError( "missingMatch" ) << "    Match " << matchTag.encode() << " missing";
      continueHere = true;
    } else if ( triggerMatch->empty() ) {
      edm::LogWarning( "emptyMatch" ) << "    Match " << matchTag.encode() << " empty";
    }
    if ( ! continueHere ) {
      if ( triggerStandAloneMatch->size() != triggerMatch->size() ) {
        edm::LogError( "matchSizes" ) << "    Matches differ in size:\n"
                                      << "        Stand-alone match " << match             << ": " << triggerStandAloneMatch->size() << "\n"
                                      << "        Match             " << matchTag.encode() << ": " << triggerMatch->size();
        continueHere = true;
      } else if ( displayMatches_ ) {
        edm::LogWarning( "matchSizes" ) << "    Matches " << match << " size: " << triggerStandAloneMatch->size();
      }
    }
    if ( continueHere ) {
      continue;
    }
    const TriggerObjectMatch * triggerMatchEvent( handlePatTriggerEvent->triggerObjectMatchResult( match ) );
    if ( triggerMatchEvent != triggerMatch.product() ) {
      edm::LogError( "matchPtr" ) << "    Matcher pointers differ:\n"
                                  << "        from edm::Handle::product()                          : " << triggerMatch.product() << "\n"
                                  << "        from pat::TriggerEvent::triggerObjectMatchResult(...): " << triggerMatchEvent;
    } else {
      if ( displayMatches_ ) edm::LogWarning( "matchPtr" ) << "    Matcher pointers ok";
    }
    edm::AssociativeIterator< reco::CandidateBaseRef, TriggerObjectMatch > it( *triggerMatch, edm::EdmEventItemGetter< reco::CandidateBaseRef >( iEvent ) ), itEnd( it.end() );
    edm::AssociativeIterator< reco::CandidateBaseRef, TriggerObjectStandAloneMatch > itSa( *triggerStandAloneMatch, edm::EdmEventItemGetter< reco::CandidateBaseRef >( iEvent ) ), itSaEnd( itSa.end() );
    while ( it != itEnd && itSa != itSaEnd ) {
      bool continueNow( false );
      const reco::CandidateBaseRef candRef( it->first );
      if ( candRef.isNull() || ! candRef.isAvailable() ) {
        if ( candRef.id().id() != 0 ) {
          edm::LogError( "noMatchCandRef" ) << "    Candidate reference cannot be dereferenced:\n"
                                            << "        product ID: " << candRef.id() << "\n"
                                            << "        key       : " << candRef.key() << "\n"
                                            << "        null      : " << candRef.isNull() << "\n"
                                            << "        available : " << candRef.isAvailable();
        }
        continueNow = true;
      }
      if ( displayMatches_ && ! continueNow ) edm::LogWarning( "matchCandRef" ) << "    Candidate reference:\n"
                                                                                << "        product ID: " << candRef.id() << "\n"
                                                                                << "        key       : " << candRef.key();
      const TriggerObjectRef objRef( it->second );
      if ( objRef.isNull() || ! objRef.isAvailable() ) {
        if ( objRef.id().id() != 0 ) {
          edm::LogError( "noMatchObjRef" ) << "    Object reference cannot be dereferenced:\n"
                                           << "        product ID: " << objRef.id() << "\n"
                                           << "        key       : " << objRef.key() << "\n"
                                           << "        null      : " << objRef.isNull() << "\n"
                                           << "        available : " << objRef.isAvailable();
        }
        continueNow = true;
      }
      if ( displayMatches_ && ! continueNow ) edm::LogWarning( "matchObjRef" ) << "    Object reference:\n"
                                                                               << "        product ID: " << objRef.id() << "\n"
                                                                               << "        key       : " << objRef.key();
      const reco::CandidateBaseRef candRefSa( itSa->first );
      if ( candRefSa.isNull() || ! candRefSa.isAvailable() ) {
        if ( candRefSa.id().id() != 0 ) {
          edm::LogError( "noMatchCandRefSa" ) << "    Stand-alone candidate reference cannot be dereferenced:\n"
                                              << "        product ID: " << candRefSa.id() << "\n"
                                              << "        key       : " << candRefSa.key() << "\n"
                                              << "        null      : " << candRefSa.isNull() << "\n"
                                              << "        available : " << candRefSa.isAvailable();
        }
        continueNow = true;
      }
      if ( displayMatches_ && ! continueNow ) edm::LogWarning( "matchCandRefSa" ) << "    Stand-alone candidate reference:\n"
                                                                                  << "        product ID: " << candRefSa.id() << "\n"
                                                                                  << "        key       : " << candRefSa.key();
      const TriggerObjectStandAloneRef objRefSa( itSa->second );
      if ( objRefSa.isNull() || ! objRefSa.isAvailable() ) {
        if ( objRefSa.id().id() != 0 ) {
          edm::LogError( "noMatchObjRefSa" ) << "    Stand-alone object reference cannot be dereferenced:\n"
                                             << "        product ID: " << objRefSa.id() << "\n"
                                             << "        key       : " << objRefSa.key() << "\n"
                                             << "        null      : " << objRefSa.isNull() << "\n"
                                             << "        available : " << objRefSa.isAvailable();
        }
        continueNow = true;
      }
      if ( displayMatches_ && ! continueNow ) edm::LogWarning( "matchObjRefSa" ) << "    Stand-alone object reference:\n"
                                                                                 << "        product ID: " << objRefSa.id() << "\n"
                                                                                 << "        key       : " << objRefSa.key();
      if ( continueNow ) {
        ++it;
        ++itSa;
        continue;
      }
      // check correspondance of match and stand-alone match
      if ( candRef.id() != candRefSa.id() ) {
        edm::LogError( "candRefIds" ) << "    Candidate IDs differ:\n"
                                      << "        candidate            : " << candRef.id() << "\n"
                                      << "        stand-alone candidate: " << candRefSa.id();
        continueNow = true;
      }
      if ( candRef.key() != candRefSa.key() ) {
        edm::LogError( "candRefKeys" ) << "    Candidate keys differ:\n"
                                       << "        candidate            : " << candRef.key() << "\n"
                                       << "        stand-alone candidate: " << candRefSa.key();
        continueNow = true;
      }
      if ( objRef.id() == objRefSa.id() && objRef.id().id() != 0 ) {
        edm::LogError( "objRefIds" ) << "    Object IDs are equal: " << objRef.id();
        continueNow = true;
      }
      if ( objRef.key() != objRefSa.key() ) {
        edm::LogError( "objRefKeys" ) << "    Object keys differ:\n"
                                      << "        object            : " << objRef.key() << "\n"
                                      << "        stand-alone object: " << objRefSa.key();
        continueNow = true;
      }
      if ( continueNow ) {
        ++it;
        ++itSa;
        continue;
      }
      histos2D_[ "ptObjCand" ]->Fill( candRef->pt(), objRef->pt() );
      histos2D_[ "etaObjCand" ]->Fill( candRef->eta(), objRef->eta() );
      histos2D_[ "phiObjCand" ]->Fill( candRef->phi(), objRef->phi() );
      const TriggerObjectRef matchObjRef( handlePatTriggerEvent->triggerMatchObject( candRef, match, iEvent ) );
      if ( matchObjRef.isNonnull() && matchObjRef.isAvailable() ) {
        edm::LogWarning( "matchObjRefEvent" ) << "    Object reference from event:\n"
                                              << "        product ID: " << matchObjRef.id() << "\n"
                                              << "        key       : " << matchObjRef.key();
        if ( matchObjRef != objRef ) {                                          
          edm::LogError( "matchObjRefs" ) << "    Matching trigger objects do not correspond:\n"
                                          << "        edm::Association:     " << objRef.id()      << " " << objRef.key() << "\n"
                                          << "        triggerMatchObject(): " << matchObjRef.id() << " " << matchObjRef.key();
        }
      } else {
        edm::LogError( "noMatchObjRefEvent" ) << "    Object reference from event cannot be dereferenced:\n"
                                              << "        product ID: " << matchObjRef.id() << "\n"
                                              << "        key       : " << matchObjRef.key() << "\n"
                                              << "        null      : " << matchObjRef.isNull() << "\n"
                                              << "        available : " << matchObjRef.isAvailable();
      }
      const TriggerFilterRefVector objFilters( handlePatTriggerEvent->objectFilters( objRef ) );
      for ( TriggerFilterRefVector::const_iterator iFilter = objFilters.begin(); iFilter != objFilters.end(); ++iFilter ) {
        if ( displayMatches_ ) edm::LogWarning( "matchObjFilter" ) << "    Object used in filter:\n"
                                                                   << "        label: " << ( *iFilter )->label() << "\n"
                                                                   << "        type : " << ( *iFilter )->type();
        if ( ! ( *iFilter )->hasObjectKey( objRef.key() ) ) {
          edm::LogError( "wrongObjFilterKey" ) << "    Object key not found in keys from filter:\n"
                                               << "        object key : " << objRef.key() << "\n"
                                               << "        size filter: " << ( *iFilter )->objectKeys().size();
        }
      }
      const reco::CandidateBaseRefVector matchCandRefs( handlePatTriggerEvent->triggerMatchCandidates( objRef, match, iEvent ) );
      bool found( false );
      for ( reco::CandidateBaseRefVector::const_iterator iCand = matchCandRefs.begin(); iCand != matchCandRefs.end(); ++iCand ) {
        if ( *iCand == candRef ) {
          found = true;
          break;
        }
      }
      if ( found ) {
        if ( displayMatches_ ) edm::LogWarning( "matchCandRefs" ) << "    Matching candidate object found";
      } else {
        edm::LogError( "noMatchCandRefs" ) << "    Matching candidate objects do not correspond:\n"
                                           << "        edm::Association:     " << candRef.id()      << " " << candRef.key() << " not found in triggerMatchCandidates()";
      }
      const TriggerObjectMatchMap matchMap( handlePatTriggerEvent->triggerMatchObjects( candRef, iEvent ) );
      if ( matchMap.empty() ) {
        edm::LogError( "emptyMatchMap" ) << "    No entry in match map:\n"
                                         << "        matcher name:  " <<  match << "\n"
                                         << "        candidate ref: " << candRef.id() << " " << candRef.key();
      }
      ++it;
      ++itSa;
    }
  }

  // Checking embedding

  edm::Handle< PhotonCollection > handlePatPhotons;
  iEvent.getByLabel( tagPatPhotons_, handlePatPhotons );
  const unsigned prodIdPhotons( handlePatPhotons.id().id() );
  edm::Handle< ElectronCollection > handlePatElectrons;
  iEvent.getByLabel( tagPatElectrons_, handlePatElectrons );
  const unsigned prodIdElectrons( handlePatElectrons.id().id() );
  edm::Handle< MuonCollection > handlePatMuons;
  iEvent.getByLabel( tagPatMuons_, handlePatMuons );
  const unsigned prodIdMuons( handlePatMuons.id().id() );
  edm::Handle< TauCollection > handlePatTaus;
  iEvent.getByLabel( tagPatTaus_, handlePatTaus );
  const unsigned prodIdTaus( handlePatTaus.id().id() );
  edm::Handle< JetCollection > handlePatJets;
  iEvent.getByLabel( tagPatJets_, handlePatJets );
  const unsigned prodIdJets( handlePatJets.id().id() );
  edm::Handle< METCollection > handlePatMETs;
  iEvent.getByLabel( tagPatMETs_, handlePatMETs );
  const unsigned prodIdMETs( handlePatMETs.id().id() );
  edm::Handle< PhotonCollection > handlePatPhotonsEmbedding;
  iEvent.getByLabel( tagPatPhotonsEmbedding_, handlePatPhotonsEmbedding );
  edm::Handle< ElectronCollection > handlePatElectronsEmbedding;
  iEvent.getByLabel( tagPatElectronsEmbedding_, handlePatElectronsEmbedding );
  edm::Handle< MuonCollection > handlePatMuonsEmbedding;
  iEvent.getByLabel( tagPatMuonsEmbedding_, handlePatMuonsEmbedding );
  edm::Handle< TauCollection > handlePatTausEmbedding;
  iEvent.getByLabel( tagPatTausEmbedding_, handlePatTausEmbedding );
  edm::Handle< JetCollection > handlePatJetsEmbedding;
  iEvent.getByLabel( tagPatJetsEmbedding_, handlePatJetsEmbedding );
  edm::Handle< METCollection > handlePatMETsEmbedding;
  iEvent.getByLabel( tagPatMETsEmbedding_, handlePatMETsEmbedding );

  for ( size_t iMatch = 0; iMatch < matches.size(); ++iMatch ) {
    const std::string match( matches.at( iMatch ) );
    edm::Handle< TriggerObjectStandAloneMatch > triggerStandAloneMatch;
    iEvent.getByLabel( match, triggerStandAloneMatch );
    if ( ! triggerStandAloneMatch.isValid() ) {
      edm::LogError( "missingStandAloneMatch" ) << "    Stand-alone match " << match << " missing";
      continue;
    }
    edm::AssociativeIterator< reco::CandidateBaseRef, TriggerObjectStandAloneMatch > itSa( *triggerStandAloneMatch, edm::EdmEventItemGetter< reco::CandidateBaseRef >( iEvent ) ), itSaEnd( itSa.end() );
    while ( itSa != itSaEnd ) {
      const TriggerObjectStandAloneRef objRefSa( itSa->second );
      if ( objRefSa.isNull() || ! objRefSa.isAvailable() ) {
        ++itSa;
        continue;
      }
      const reco::CandidateBaseRef candRefSa( itSa->first );
      bool found( false );
      if ( candRefSa.id().id() == prodIdPhotons ) {
        const TriggerObjectStandAloneCollection & trigObjColl( handlePatPhotonsEmbedding->at( candRefSa.key() ).triggerObjectMatches() );
        for ( size_t iTrigObj = 0; iTrigObj < trigObjColl.size(); ++iTrigObj ) {
          if ( trigObjColl.at( iTrigObj ).collection() == objRefSa->collection() &
               trigObjColl.at( iTrigObj ).p4()         == objRefSa->p4()         ) {
            found = true;
          }
        }
      } else if ( candRefSa.id().id() == prodIdElectrons ) {
        const TriggerObjectStandAloneCollection & trigObjColl( handlePatElectronsEmbedding->at( candRefSa.key() ).triggerObjectMatches() );
        for ( size_t iTrigObj = 0; iTrigObj < trigObjColl.size(); ++iTrigObj ) {
          if ( trigObjColl.at( iTrigObj ).collection() == objRefSa->collection() &
               trigObjColl.at( iTrigObj ).p4()         == objRefSa->p4()         ) {
            found = true;
          }
        }
      } else if ( candRefSa.id().id() == prodIdMuons ) {
        const TriggerObjectStandAloneCollection & trigObjColl( handlePatMuonsEmbedding->at( candRefSa.key() ).triggerObjectMatches() );
        for ( size_t iTrigObj = 0; iTrigObj < trigObjColl.size(); ++iTrigObj ) {
          if ( trigObjColl.at( iTrigObj ).collection() == objRefSa->collection() &
               trigObjColl.at( iTrigObj ).p4()         == objRefSa->p4()         ) {
            found = true;
          }
        }
      } else if ( candRefSa.id().id() == prodIdTaus ) {
        const TriggerObjectStandAloneCollection & trigObjColl( handlePatTausEmbedding->at( candRefSa.key() ).triggerObjectMatches() );
        for ( size_t iTrigObj = 0; iTrigObj < trigObjColl.size(); ++iTrigObj ) {
          if ( trigObjColl.at( iTrigObj ).collection() == objRefSa->collection() &
               trigObjColl.at( iTrigObj ).p4()         == objRefSa->p4()         ) {
            found = true;
          }
        }
      } else if ( candRefSa.id().id() == prodIdJets ) {
        const TriggerObjectStandAloneCollection & trigObjColl( handlePatJetsEmbedding->at( candRefSa.key() ).triggerObjectMatches() );
        for ( size_t iTrigObj = 0; iTrigObj < trigObjColl.size(); ++iTrigObj ) {
          if ( trigObjColl.at( iTrigObj ).collection() == objRefSa->collection() &
               trigObjColl.at( iTrigObj ).p4()         == objRefSa->p4()         ) {
            found = true;
          }
        }
      } else if ( candRefSa.id().id() == prodIdMETs ) {
        const TriggerObjectStandAloneCollection & trigObjColl( handlePatMETsEmbedding->at( candRefSa.key() ).triggerObjectMatches() );
        for ( size_t iTrigObj = 0; iTrigObj < trigObjColl.size(); ++iTrigObj ) {
          if ( trigObjColl.at( iTrigObj ).collection() == objRefSa->collection() &
               trigObjColl.at( iTrigObj ).p4()         == objRefSa->p4()         ) {
            found = true;
          }
        }
      } else {
        edm::LogError( "missingProductId" ) << "    ProductID " << candRefSa.id().id() << " not present in collection map";
        ++itSa;
        continue;
      }
      if ( ! found ) {
        edm::LogError( "missingMatch" ) << "    Match not found:";
      }
      ++itSa;
    }
  }  

}

void myTriggerTest::endRun()
{
}

void myTriggerTest::endJob()
{
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( myTriggerTest );
