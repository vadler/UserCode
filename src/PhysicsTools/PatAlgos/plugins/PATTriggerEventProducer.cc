//
// $Id: PATTriggerEventProducer.cc,v 1.7 2010/03/08 10:50:24 vadler Exp $
//


#include "PhysicsTools/PatAlgos/plugins/PATTriggerEventProducer.h"

#include <cassert>

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h" // new
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h" // new
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"

#include "DataFormats/Common/interface/AssociativeIterator.h"
#include "FWCore/Framework/interface/ESHandle.h" // new
#include "FWCore/MessageLogger/interface/MessageLogger.h"


using namespace pat;
using namespace edm;


PATTriggerEventProducer::PATTriggerEventProducer( const ParameterSet & iConfig ) :
  nameProcess_( iConfig.getParameter< std::string >( "processName" ) ), // required
  tagTriggerResults_( "TriggerResults" ),                               // default
  tagTriggerProducer_( "patTrigger" ),                                  // default
  tagL1Gt_( "gtDigis" ),                                                // default
  tagsTriggerMatcher_()
{

  if ( iConfig.exists( "triggerResults" ) )     tagTriggerResults_  = iConfig.getParameter< InputTag >( "triggerResults" );
  if ( tagTriggerResults_.process().empty() )   tagTriggerResults_  = InputTag( tagTriggerResults_.label(), tagTriggerResults_.instance(), nameProcess_ );
  if ( iConfig.exists( "patTriggerProducer" ) ) tagTriggerProducer_ = iConfig.getParameter< InputTag >( "patTriggerProducer" );
  if ( iConfig.exists( "l1GtTag" ) )            tagL1Gt_            = iConfig.getParameter< InputTag >( "l1GtTag" );
  if ( iConfig.exists( "patTriggerMatches" ) )  tagsTriggerMatcher_ = iConfig.getParameter< std::vector< InputTag > >( "patTriggerMatches" );

  for ( size_t iMatch = 0; iMatch < tagsTriggerMatcher_.size(); ++iMatch ) {
    produces< TriggerObjectMatch >( tagsTriggerMatcher_.at( iMatch ).label() );
  }
  produces< TriggerEvent >();

}


void PATTriggerEventProducer::beginRun( Run & iRun, const EventSetup & iSetup )
{

  // Initialize
  hltConfigInit_ = false;

  // Initialize HLTConfigProvider
  bool changed( true );
  if ( ! hltConfig_.init( iRun, iSetup, nameProcess_, changed ) ) {
    LogError( "errorHltConfigExtraction" ) << "HLT config extraction error with process name " << nameProcess_;
  } else if ( hltConfig_.size() <= 0 ) {
    LogError( "hltConfigSize" ) << "HLT config size error";
  } else hltConfigInit_ = true;

}


void PATTriggerEventProducer::produce( Event& iEvent, const EventSetup& iSetup )
{

  if ( ! hltConfigInit_ ) return;

  ESHandle< L1GtTriggerMenu > handleL1GtTriggerMenu;
  iSetup.get< L1GtTriggerMenuRcd >().get( handleL1GtTriggerMenu );
  Handle< TriggerResults > handleTriggerResults;
  iEvent.getByLabel( tagTriggerResults_, handleTriggerResults );
  if ( ! handleTriggerResults.isValid() ) {
    LogError( "triggerResultsValid" ) << "TriggerResults product with InputTag " << tagTriggerResults_.encode() << " not in event";
    return;
  }
  Handle< TriggerPathCollection > handleTriggerPaths;
  iEvent.getByLabel( tagTriggerProducer_, handleTriggerPaths );
  Handle< TriggerFilterCollection > handleTriggerFilters;
  iEvent.getByLabel( tagTriggerProducer_, handleTriggerFilters );
  Handle< TriggerObjectCollection > handleTriggerObjects;
  iEvent.getByLabel( tagTriggerProducer_, handleTriggerObjects );
  Handle< TriggerObjectStandAloneCollection > handleTriggerObjectsStandAlone;
  iEvent.getByLabel( tagTriggerProducer_, handleTriggerObjectsStandAlone );
  assert( handleTriggerObjects->size() == handleTriggerObjectsStandAlone->size() );

  bool physDecl( false );
  if ( iEvent.isRealData() ) {
    Handle< L1GlobalTriggerReadoutRecord > handleL1GlobalTriggerReadoutRecord;
    iEvent.getByLabel( tagL1Gt_, handleL1GlobalTriggerReadoutRecord );
    if ( handleL1GlobalTriggerReadoutRecord.isValid() ) {
      L1GtFdlWord fdlWord = handleL1GlobalTriggerReadoutRecord->gtFdlWord();
      if ( fdlWord.physicsDeclared() == 1 ) {
        physDecl = true;
      }
    } else {
      LogError( "l1GlobalTriggerReadoutRecordValid" ) << "L1GlobalTriggerReadoutRecord product with InputTag " << tagL1Gt_.encode() << " not in event";
    }
  } else {
    physDecl = true;
  }

  // produce trigger event

  std::auto_ptr< TriggerEvent > triggerEvent( new TriggerEvent( handleL1GtTriggerMenu->gtTriggerMenuName(), std::string( hltConfig_.tableName() ), handleTriggerResults->wasrun(), handleTriggerResults->accept(), handleTriggerResults->error(), physDecl ) );
  // set product references to trigger collections
  if ( handleTriggerPaths.isValid() ) {
    triggerEvent->setPaths( handleTriggerPaths );
  } else {
    LogError( "triggerPathsValid" ) << "pat::TriggerPathCollection product with InputTag " << tagTriggerProducer_.encode() << " not in event";
  }
  if ( handleTriggerFilters.isValid() ) {
    triggerEvent->setFilters( handleTriggerFilters );
  } else {
    LogError( "triggerFiltersValid" ) << "pat::TriggerFilterCollection product with InputTag " << tagTriggerProducer_.encode() << " not in event";
  }
  if ( handleTriggerObjects.isValid() ) {
    triggerEvent->setObjects( handleTriggerObjects );
  } else {
    LogError( "triggerObjectsValid" ) << "pat::TriggerObjectCollection product with InputTag " << tagTriggerProducer_.encode() << " not in event";
  }

  // produce trigger match association and set references
  if ( handleTriggerObjects.isValid() ) {
    for ( size_t iMatch = 0; iMatch < tagsTriggerMatcher_.size(); ++iMatch ) {
      const std::string labelTriggerObjectMatcher( tagsTriggerMatcher_.at( iMatch ).label() );
      // copy trigger match association using TriggerObjectStandAlone to those using TriggerObject
      // relying on the fact, that only one candidate collection is present in the association
      Handle< TriggerObjectStandAloneMatch > handleTriggerObjectStandAloneMatch;
      iEvent.getByLabel( labelTriggerObjectMatcher, handleTriggerObjectStandAloneMatch );
      if ( ! handleTriggerObjectStandAloneMatch.isValid() ) {
        LogError( "triggerMatchValid" ) << "pat::TriggerObjectStandAloneMatch product with InputTag " << labelTriggerObjectMatcher << " not in event";
        continue;
      }
      AssociativeIterator< reco::CandidateBaseRef, TriggerObjectStandAloneMatch > it( *handleTriggerObjectStandAloneMatch, EdmEventItemGetter< reco::CandidateBaseRef >( iEvent ) ), itEnd( it.end() );
      Handle< reco::CandidateView > handleCands;
      std::vector< int > indices;
      while ( it != itEnd ) {
        if ( indices.size() == 0 ) {
          iEvent.get( it->first.id(), handleCands );
        }
        indices.push_back( it->second.key() );
        ++it;
      }
      std::auto_ptr< TriggerObjectMatch > triggerObjectMatch( new TriggerObjectMatch( handleTriggerObjects ) );
      TriggerObjectMatch::Filler matchFiller( *triggerObjectMatch );
      if ( handleCands.isValid() ) {
        matchFiller.insert( handleCands, indices.begin(), indices.end() );
      }
      matchFiller.fill();
      OrphanHandle< TriggerObjectMatch > handleTriggerObjectMatch( iEvent.put( triggerObjectMatch, labelTriggerObjectMatcher ) );
      // set product reference to trigger match association
      if ( ! handleTriggerObjectMatch.isValid() ) {
        LogError( "triggerMatchValid" ) << "pat::TriggerObjectMatch product with InputTag " << labelTriggerObjectMatcher << " not in event";
        continue;
      }
      if ( ! ( triggerEvent->addObjectMatchResult( handleTriggerObjectMatch, labelTriggerObjectMatcher ) ) ) {
        LogWarning( "triggerObjectMatchReplication" ) << "pat::TriggerEvent contains already a pat::TriggerObjectMatch from matcher module " << labelTriggerObjectMatcher;
      }
    }
  }

  iEvent.put( triggerEvent );

}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( PATTriggerEventProducer );
