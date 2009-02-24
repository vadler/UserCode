//
// $Id: myTriggerTest.cc,v 1.1.2.1 2008/12/18 11:19:38 vadler Exp $
//


#include "PhysicsTools/PatAlgos/plugins/myTriggerTest.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/Common/interface/AssociativeIterator.h"
#include "DataFormats/Candidate/interface/Candidate.h"


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
  histos1D_[ "pathAcceptedAccept" ] = fileService->make< TH1D >( "pathAcceptedAccept", "Accepted path accepted?", 2, -0.5, 1.5 );
  histos1D_[ "pathAcceptedAccept" ]->SetXTitle( "accepted" );
  histos1D_[ "pathAcceptedAccept" ]->SetYTitle( "entries" );
  histos1D_[ "filterAcceptedStatus" ] = fileService->make< TH1D >( "filterAcceptedStatus", "Accepted filter status", 3, -1.5, 1.5 );
  histos1D_[ "filterAcceptedStatus" ]->SetXTitle( "status" );
  histos1D_[ "filterAcceptedStatus" ]->SetYTitle( "entries" );
  histos2D_[ "objectId" ] = fileService->make< TH2D >( "objectId", "Object filter IDs vs. filter object IDs", 25, 80.5, 105.5, 25, 80.5, 105.5 );
  histos2D_[ "objectId" ]->SetXTitle( "filter object ID" );
  histos2D_[ "objectId" ]->SetYTitle( "object filter ID" );
  histos2D_[ "ptObjCand" ] = fileService->make< TH2D >( "ptObjCand", "Object vs. candidate p_T", 100, 0., 1000., 100, 0., 1000. );
  histos2D_[ "ptObjCand" ]->SetXTitle( "candidate" );
  histos2D_[ "ptObjCand" ]->SetYTitle( "object" );
  
  // This should normally happen in beginRun()
  if ( ! hltConfig_.init( nameHLTProcess_ ) ) {
    edm::LogError( "hltConfigExtraction" ) << "HLT config extraction error with process name " << nameHLTProcess_;
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
      const TriggerFilterRefVector       myFilters( handlePatTriggerEvent->pathFilters( iPath->name(), true ) );
      const std::vector< std::string >   names( iPath->modules() );                             
      const std::vector< std::string > & namesHltConfig( hltConfig_.moduleLabels( iPath->name() ) );
      unsigned lastModule( iPath->lastActiveModule() );
      unsigned lastModuleTriggerResults( handleTriggerResults->index( handlePatTriggerEvent->indexPath( iPath->name() ) ) );
      if ( names.size() == namesHltConfig.size() && namesHltConfig.size() == myFilters.size() ) {
        for ( size_t iModule = 0; iModule < names.size(); ++iModule ) {
          if ( names[ iModule ] != namesHltConfig[ iModule ] ) {
            edm::LogError( "nameModule" ) << "    Module #" << iModule << " with different names in path " << iPath->name() << ":\n"
                                          << "        HLTConfigProvider: " << namesHltConfig[ iModule ] << "\n"
                                          << "        pat::TriggerPath : " << names[ iModule ];
          }
        }
      } else {
        edm::LogError( "nModules" ) << "    Number of HLT modules differ in path " << iPath->name() << ":\n"
                                    << "        HLTConfigProvider          : " << namesHltConfig.size() << "\n"
                                    << "        pat::TriggerPath           : " << names.size() << "\n"
                                    << "        pat::TriggerFilterRefVector: " << myFilters.size();
      }
      histos2D_[ "nModules" ]->Fill( namesHltConfig.size(), names.size() );
      if ( lastModule != lastModuleTriggerResults ) {
        edm::LogError( "iLastModule" ) << "    Index of last active HLT module differs in path " << iPath->name() << ":\n"
                                       << "        TriggerResults::HLT: " << lastModuleTriggerResults << "\n"
                                       << "        pat::TriggerPath   : " << lastModule;
      }
      histos2D_[ "iLastModule" ]->Fill( namesHltConfig.size(), names.size() );
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
  const std::vector< std::string > matches( handlePatTriggerEvent->triggerMatchers() );
  for ( unsigned iMatch = 0; iMatch < matches.size(); ++iMatch ) {
    edm::LogWarning( "matcherLabel" ) << "Matcher label: " << matches.at( iMatch );
//     edm::AssociativeIterator< reco::CandidateBaseRef, TriggerObjectMatch > it( *( handlePatTriggerEvent->triggerObjectMatchResult( matches.at( iMatch ) ) ), edm::EdmEventItemGetter< reco::CandidateBaseRef >( iEvent ) ), itEnd( it.end() );
    edm::Handle< TriggerObjectMatch > triggerMatch;
    iEvent.getByLabel( matches.at( iMatch ), triggerMatch );
    if ( triggerMatch->empty() ) {
      break;
    }
    edm::AssociativeIterator< reco::CandidateBaseRef, TriggerObjectMatch > it( *triggerMatch, edm::EdmEventItemGetter< reco::CandidateBaseRef >( iEvent ) ), itEnd( it.end() );
    while ( it != itEnd ) {
      const reco::CandidateBaseRef candRef( it->first );
      const TriggerObjectRef       objRef( it->second );
      if ( candRef.isNull() || ! candRef.isAvailable() ) {
        edm::LogError( "noMatchCandRef" ) << "    Candidate reference cannot be dereferenced:\n"
                                          << "        product ID: " << candRef.id() << "\n"
                                          << "        key       : " << candRef.key() << "\n"
                                          << "        null      : " << candRef.isNull() << "\n"
                                          << "        available : " << candRef.isAvailable();
        break;
      }
      edm::LogWarning( "matchCandRef" ) << "    Candidate reference:\n"
                                        << "        product ID: " << candRef.id() << "\n"
                                        << "        key       : " << candRef.key();
      if ( objRef.isNull() || ! objRef.isAvailable() ) {
        edm::LogError( "noMatchObjRef" ) << "    Object reference cannot be dereferenced:\n"
                                         << "        product ID: " << objRef.id() << "\n"
                                         << "        key       : " << objRef.key() << "\n"
                                         << "        null      : " << objRef.isNull() << "\n"
                                         << "        available : " << objRef.isAvailable();
        break;
      }
      edm::LogWarning( "matchObjRef" ) << "    Object reference:\n"
                                       << "        product ID: " << objRef.id() << "\n"
                                       << "        key       : " << objRef.key();
      histos2D_[ "ptObjCand" ]->Fill( candRef->pt(), objRef->pt() );
      const TriggerObjectRef matchObjRef( handlePatTriggerEvent->triggerMatchObject( candRef, matches.at( iMatch ), iEvent ) );
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
      const reco::CandidateBaseRefVector matchCandRefs( handlePatTriggerEvent->triggerMatchCandidates( objRef, matches.at( iMatch ), iEvent ) );
      bool found( false );
      for ( reco::CandidateBaseRefVector::const_iterator iCand = matchCandRefs.begin(); iCand != matchCandRefs.end(); ++iCand ) {
        if ( *iCand == candRef ) {
          found = true;
          break;
        }
      }
      if ( ! found ) {
        edm::LogError( "matchCandRefs" ) << "    Matching candidate objects do not correspond:\n"
                                         << "        edm::Association:     " << candRef.id()      << " " << candRef.key() << "not found in triggerMatchCandidates()";
      }
      const TriggerObjectMatchMap matchMap( handlePatTriggerEvent->triggerMatchObjects( candRef, iEvent ) );
      if ( matchMap.empty() ) {
        edm::LogError( "emptyMatchMap" ) << "    No entry in match map:\n"
                                         << "        matcher name:  " <<  matches.at( iMatch ) << "\n"
                                         << "        candidate ref: " << candRef.id() << " " << candRef.key();
      }
      ++it;
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
