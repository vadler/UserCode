//
// $Id: PATTriggerProducer.cc,v 1.35 2010/06/27 18:09:25 vadler Exp $
//


#include "PhysicsTools/PatAlgos/plugins/PATTriggerProducer.h"

#include <vector>
#include <map>
#include <cassert>

#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

#include "DataFormats/PatCandidates/interface/TriggerAlgorithm.h"
#include "DataFormats/PatCandidates/interface/TriggerPath.h"
#include "DataFormats/PatCandidates/interface/TriggerFilter.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"


using namespace pat;
using namespace edm;


PATTriggerProducer::PATTriggerProducer( const ParameterSet & iConfig ) :
  onlyStandAlone_( iConfig.getParameter< bool >( "onlyStandAlone" ) ),
  // L1 configuration parameters
  addL1Algos_( false ),
  tagL1ExtraMu_(),
  tagL1ExtraNoIsoEG_(),
  tagL1ExtraIsoEG_(),
  tagL1ExtraCenJet_(),
  tagL1ExtraForJet_(),
  tagL1ExtraTauJet_(),
  tagL1ExtraETM_(),
  tagL1ExtraHTM_(),
  saveL1Refs_( false ),
  // HLT configuration parameters
  nameProcess_( iConfig.getParameter< std::string >( "processName" ) ),
  tagTriggerResults_( "TriggerResults" ),
  tagTriggerEvent_( "hltTriggerSummaryAOD" ),
  hltPrescaleLabel_(),
  labelHltPrescaleTable_(),
  hltPrescaleTableRun_(),
  hltPrescaleTableLumi_(),
  addPathModuleLabels_( false )
{

  // L1 configuration parameters
  if ( iConfig.exists( "addL1Algos" ) ) addL1Algos_ = iConfig.getParameter< bool >( "addL1Algos" );
  if ( iConfig.exists( "l1ExtraMu" ) ) {
    tagL1ExtraMu_ = iConfig.getParameter< InputTag >( "l1ExtraMu" );
    if ( tagL1ExtraMu_.process().empty() ) tagL1ExtraMu_ = InputTag( tagL1ExtraMu_.label(), tagL1ExtraMu_.instance(), nameProcess_ );
  }
  if ( iConfig.exists( "l1ExtraNoIsoEG" ) ) {
    tagL1ExtraNoIsoEG_ = iConfig.getParameter< InputTag >( "l1ExtraNoIsoEG" );
    if ( tagL1ExtraNoIsoEG_.process().empty() ) tagL1ExtraNoIsoEG_ = InputTag( tagL1ExtraNoIsoEG_.label(), tagL1ExtraNoIsoEG_.instance(), nameProcess_ );
  }
  if ( iConfig.exists( "l1ExtraIsoEG" ) ) {
    tagL1ExtraIsoEG_ = iConfig.getParameter< InputTag >( "l1ExtraIsoEG" );
    if ( tagL1ExtraIsoEG_.process().empty() ) tagL1ExtraIsoEG_ = InputTag( tagL1ExtraIsoEG_.label(), tagL1ExtraIsoEG_.instance(), nameProcess_ );
  }
  if ( iConfig.exists( "l1ExtraCenJet" ) ) {
    tagL1ExtraCenJet_ = iConfig.getParameter< InputTag >( "l1ExtraCenJet" );
    if ( tagL1ExtraCenJet_.process().empty() ) tagL1ExtraCenJet_ = InputTag( tagL1ExtraCenJet_.label(), tagL1ExtraCenJet_.instance(), nameProcess_ );
  }
  if ( iConfig.exists( "l1ExtraForJet" ) ) {
    tagL1ExtraForJet_ = iConfig.getParameter< InputTag >( "l1ExtraForJet" );
    if ( tagL1ExtraForJet_.process().empty() ) tagL1ExtraForJet_ = InputTag( tagL1ExtraForJet_.label(), tagL1ExtraForJet_.instance(), nameProcess_ );
  }
  if ( iConfig.exists( "l1ExtraTauJet" ) ) {
    tagL1ExtraTauJet_ = iConfig.getParameter< InputTag >( "l1ExtraTauJet" );
    if ( tagL1ExtraTauJet_.process().empty() ) tagL1ExtraTauJet_ = InputTag( tagL1ExtraTauJet_.label(), tagL1ExtraTauJet_.instance(), nameProcess_ );
  }
  if ( iConfig.exists( "l1ExtraETM" ) ) {
    tagL1ExtraETM_ = iConfig.getParameter< InputTag >( "l1ExtraETM" );
    if ( tagL1ExtraETM_.process().empty() ) tagL1ExtraETM_ = InputTag( tagL1ExtraETM_.label(), tagL1ExtraETM_.instance(), nameProcess_ );
  }
  if ( iConfig.exists( "l1ExtraHTM" ) ) {
    tagL1ExtraHTM_ = iConfig.getParameter< InputTag >( "l1ExtraHTM" );
    if ( tagL1ExtraHTM_.process().empty() ) tagL1ExtraHTM_ = InputTag( tagL1ExtraHTM_.label(), tagL1ExtraHTM_.instance(), nameProcess_ );
  }
  if ( iConfig.exists( "saveL1Refs" ) ) saveL1Refs_ = iConfig.getParameter< bool >( "saveL1Refs" );
  // HLT configuration parameters
  if ( iConfig.exists( "triggerResults" ) )      tagTriggerResults_      = iConfig.getParameter< InputTag >( "triggerResults" );
  if ( iConfig.exists( "triggerEvent" ) )        tagTriggerEvent_        = iConfig.getParameter< InputTag >( "triggerEvent" );
  if ( iConfig.exists( "hltPrescaleLabel" ) )    hltPrescaleLabel_       = iConfig.getParameter< std::string >( "hltPrescaleLabel" );
  if ( iConfig.exists( "hltPrescaleTable" ) )    labelHltPrescaleTable_  = iConfig.getParameter< std::string >( "hltPrescaleTable" );
  if ( iConfig.exists( "addPathModuleLabels" ) ) addPathModuleLabels_    = iConfig.getParameter< bool >( "addPathModuleLabels" );
  if ( tagTriggerResults_.process().empty() ) tagTriggerResults_ = InputTag( tagTriggerResults_.label(), tagTriggerResults_.instance(), nameProcess_ );
  if ( tagTriggerEvent_.process().empty() )   tagTriggerEvent_   = InputTag( tagTriggerEvent_.label(), tagTriggerEvent_.instance(), nameProcess_ );

  if ( ! onlyStandAlone_ ) {
    produces< TriggerAlgorithmCollection >();
    produces< TriggerPathCollection >();
    produces< TriggerFilterCollection >();
    produces< TriggerObjectCollection >();
  }
  produces< TriggerObjectStandAloneCollection >();

}


void PATTriggerProducer::beginRun( Run & iRun, const EventSetup & iSetup )
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

  // Extract pre-scales
  if ( hltConfigInit_ ) {
    // Start empty
    hltPrescaleTableRun_ = trigger::HLTPrescaleTable();
    // Try run product, if configured
    if ( ! labelHltPrescaleTable_.empty() ) {
      Handle< trigger::HLTPrescaleTable > handleHltPrescaleTable;
      iRun.getByLabel( InputTag( labelHltPrescaleTable_, "Run", nameProcess_ ), handleHltPrescaleTable );
      if ( handleHltPrescaleTable.isValid() ) {
        hltPrescaleTableRun_ = trigger::HLTPrescaleTable( handleHltPrescaleTable->set(), handleHltPrescaleTable->labels(), handleHltPrescaleTable->table() );
      }
    }
  }

}


void PATTriggerProducer::beginLuminosityBlock( LuminosityBlock & iLuminosityBlock, const EventSetup & iSetup )
{

  // Extract pre-scales
  if ( hltConfigInit_ ) {
    // Start from run
    hltPrescaleTableLumi_ = trigger::HLTPrescaleTable( hltPrescaleTableRun_.set(), hltPrescaleTableRun_.labels(), hltPrescaleTableRun_.table() );
    // Try lumi product, if configured and available
    if ( ! labelHltPrescaleTable_.empty() ) {
      Handle< trigger::HLTPrescaleTable > handleHltPrescaleTable;
      iLuminosityBlock.getByLabel( InputTag( labelHltPrescaleTable_, "Lumi", nameProcess_ ), handleHltPrescaleTable );
      if ( handleHltPrescaleTable.isValid() ) {
        hltPrescaleTableLumi_ = trigger::HLTPrescaleTable( handleHltPrescaleTable->set(), handleHltPrescaleTable->labels(), handleHltPrescaleTable->table() );
      }
    }
  }

}


void PATTriggerProducer::produce( Event& iEvent, const EventSetup& iSetup )
{

  std::auto_ptr< TriggerObjectCollection > triggerObjects( new TriggerObjectCollection() );
  if ( onlyStandAlone_ ) triggerObjects->reserve( 0 );
  std::auto_ptr< TriggerObjectStandAloneCollection > triggerObjectsStandAlone( new TriggerObjectStandAloneCollection() );

  // HLT

  // Get and check HLT event data

  Handle< TriggerResults > handleTriggerResults;
  iEvent.getByLabel( tagTriggerResults_, handleTriggerResults );
  Handle< trigger::TriggerEvent > handleTriggerEvent;
  iEvent.getByLabel( tagTriggerEvent_, handleTriggerEvent );
  bool goodHlt( hltConfigInit_ );
  if ( goodHlt ) {
    if( ! handleTriggerResults.isValid() ) {
      LogError( "errorTriggerResultsValid" ) << "TriggerResults product with InputTag " << tagTriggerResults_.encode() << " not in event\n"
                                             << "No HLT information produced.";
      goodHlt = false;
    } else if ( ! handleTriggerEvent.isValid() ) {
      LogError( "errorTriggerEventValid" ) << "trigger::TriggerEvent product with InputTag " << tagTriggerEvent_.encode() << " not in event\n"
                                           << "No HLT information produced.";
      goodHlt = false;
    }
  }

  // Produce HLT paths and determine status of modules

  if ( goodHlt ) {

    // Extract pre-scales
    // Start from lumi
    trigger::HLTPrescaleTable hltPrescaleTable( hltPrescaleTableLumi_.set(), hltPrescaleTableLumi_.labels(), hltPrescaleTableLumi_.table() );
    // Try event product, if configured and available
    if ( ! labelHltPrescaleTable_.empty() ) {
      Handle< trigger::HLTPrescaleTable > handleHltPrescaleTable;
      iEvent.getByLabel( InputTag( labelHltPrescaleTable_, "Event", nameProcess_ ), handleHltPrescaleTable );
      if ( handleHltPrescaleTable.isValid() ) {
        hltPrescaleTable = trigger::HLTPrescaleTable( handleHltPrescaleTable->set(), handleHltPrescaleTable->labels(), handleHltPrescaleTable->table() );
      }
    }
    // Try event setup, if no product
    if ( hltPrescaleTable.size() == 0 ) {
      if ( ! labelHltPrescaleTable_.empty() ) {
        LogWarning( "hltPrescaleInputTag" ) << "HLTPrescaleTable product with label '" << labelHltPrescaleTable_ << "' not found in process '" << nameProcess_ << "'; using default from event setup";
      }
      if ( hltConfig_.prescaleSize() > 0 ) {
        if ( hltConfig_.prescaleSet( iEvent, iSetup ) != -1 ) {
          hltPrescaleTable = trigger::HLTPrescaleTable( hltConfig_.prescaleSet( iEvent, iSetup ), hltConfig_.prescaleLabels(), hltConfig_.prescaleTable() );
        } else {
          LogWarning( "hltPrescaleSet" ) << "HLTPrescaleTable from event setup has error";
        }
      }
    }
    unsigned set( hltPrescaleTable.set() );
    if ( hltPrescaleTable.size() > 0 ) {
      if ( hltPrescaleLabel_.size() > 0 ) {
        bool foundPrescaleLabel( false );
        for ( unsigned iLabel = 0; iLabel <  hltPrescaleTable.labels().size(); ++iLabel ) {
          if ( hltPrescaleTable.labels().at( iLabel ) == hltPrescaleLabel_ ) {
            set                = iLabel;
            foundPrescaleLabel = true;
            break;
          }
        }
        if ( ! foundPrescaleLabel ) {
          LogWarning( "hltPrescaleLabel" ) << "HLT prescale label '" << hltPrescaleLabel_ << "' not in prescale table; using default";
        }
      }
    } else if ( iEvent.isRealData() ) LogWarning( "hltPrescaleTable" ) << "No HLT prescale table found; using default empty table with all prescales 1";

    const unsigned sizePaths( hltConfig_.size() );
    const unsigned sizeFilters( handleTriggerEvent->sizeFilters() );
    const unsigned sizeObjects( handleTriggerEvent->sizeObjects() );

    std::auto_ptr< TriggerPathCollection > triggerPaths( new TriggerPathCollection() );
    triggerPaths->reserve( onlyStandAlone_ ? 0 : sizePaths );
    std::map< std::string, int > moduleStates;
    std::multimap< std::string, std::pair< std::string, bool > > filterPaths;

    for ( size_t iP = 0; iP < sizePaths; ++iP ) {
      const std::string namePath( hltConfig_.triggerName( iP ) );
      const unsigned indexPath( hltConfig_.triggerIndex( namePath ) );
      const unsigned sizeModulesPath( hltConfig_.size( namePath ) );
      const unsigned indexLastFilterPath( handleTriggerResults->index( indexPath ) );
      unsigned indexLastFilterPathModules( indexLastFilterPath + 1 );
      unsigned indexLastFilterFilters( sizeFilters );
      while ( indexLastFilterPathModules > 0 ) {
        --indexLastFilterPathModules;
        const std::string labelLastFilterModules( hltConfig_.moduleLabel( indexPath, indexLastFilterPathModules ) );
        indexLastFilterFilters = handleTriggerEvent->filterIndex( InputTag( labelLastFilterModules, "", nameProcess_ ) );
        if ( indexLastFilterFilters < sizeFilters ) break;
      }
      for ( size_t iM = 0; iM < sizeModulesPath; ++iM ) {
        const std::string nameFilter( hltConfig_.moduleLabel( indexPath, iM ) );
        const unsigned indexFilter( handleTriggerEvent->filterIndex( InputTag( nameFilter, "", nameProcess_ ) ) );
        if ( indexFilter < sizeFilters ) {
          std::pair< std::string, bool > pathAndStatus( namePath, handleTriggerResults->wasrun( indexPath ) && handleTriggerResults->accept( indexPath ) && indexFilter == indexLastFilterFilters );
          filterPaths.insert( std::pair< std::string, std::pair< std::string, bool > >( nameFilter, pathAndStatus ) );
        }
      }
      if ( ! onlyStandAlone_ ) {
        TriggerPath triggerPath( namePath, indexPath, hltConfig_.prescaleValue( set, namePath ), handleTriggerResults->wasrun( indexPath ), handleTriggerResults->accept( indexPath ), handleTriggerResults->error( indexPath ), indexLastFilterPath );
        // add module names to path and states' map
        assert( indexLastFilterPath < sizeModulesPath );
        std::map< unsigned, std::string > indicesModules;
        for ( size_t iM = 0; iM < sizeModulesPath; ++iM ) {
          const std::string nameModule( hltConfig_.moduleLabel( indexPath, iM ) );
          if ( addPathModuleLabels_ ) {
            triggerPath.addModule( nameModule );
          }
          const unsigned indexFilter( handleTriggerEvent->filterIndex( InputTag( nameModule, "", nameProcess_ ) ) );
          if ( indexFilter < sizeFilters ) {
            triggerPath.addFilterIndex( indexFilter );
          }
          const unsigned slotModule( hltConfig_.moduleIndex( indexPath, nameModule ) );
          indicesModules.insert( std::pair< unsigned, std::string >( slotModule, nameModule ) );
        }
        // store path
        triggerPaths->push_back( triggerPath );
        // store module states to be used for the filters
        for ( std::map< unsigned, std::string >::const_iterator iM = indicesModules.begin(); iM != indicesModules.end(); ++iM ) {
          if ( iM->first < indexLastFilterPath ) {
            moduleStates[ iM->second ] = 1;
          } else if ( iM->first == indexLastFilterPath ) {
            moduleStates[ iM->second ] = handleTriggerResults->accept( indexPath );
          } else if ( moduleStates.find( iM->second ) == moduleStates.end() ) {
            moduleStates[ iM->second ] = -1;
          }
        }
      }
    }

    // Put HLT paths to event
    if ( ! onlyStandAlone_ ) iEvent.put( triggerPaths );

    // Produce HLT filters and store used trigger object types
    // (only last active filter(s) available from trigger::TriggerEvent)

    std::auto_ptr< TriggerFilterCollection > triggerFilters( new TriggerFilterCollection() );
    triggerFilters->reserve( onlyStandAlone_ ? 0 : sizeFilters );
    std::multimap< trigger::size_type, int >         filterIds;
    std::multimap< trigger::size_type, std::string > filterLabels;

    for ( size_t iF = 0; iF < sizeFilters; ++iF ) {
      const std::string nameFilter( handleTriggerEvent->filterTag( iF ).label() );
      const trigger::Keys & keys = handleTriggerEvent->filterKeys( iF );
      const trigger::Vids & ids  = handleTriggerEvent->filterIds( iF );
      assert( ids.size() == keys.size() );
      for ( size_t iK = 0; iK < keys.size(); ++iK ) {
        filterLabels.insert( std::pair< trigger::size_type, std::string >( keys[ iK ], nameFilter ) ); // only for objects used in last active filter
        filterIds.insert( std::pair< trigger::size_type, int >( keys[ iK ], ids[ iK ] ) );             // only for objects used in last active filter
      }
      if ( ! onlyStandAlone_ ) {
        TriggerFilter triggerFilter( nameFilter );
        // set filter type
        const std::string typeFilter( hltConfig_.moduleType( nameFilter ) );
        triggerFilter.setType( typeFilter );
        // set filter IDs of used objects
        for ( size_t iK = 0; iK < keys.size(); ++iK ) {
          triggerFilter.addObjectKey( keys[ iK ] );
        }
        for ( size_t iI = 0; iI < ids.size(); ++iI ) {
          triggerFilter.addObjectId( ids[ iI ] );
        }
        // set status from path info
        std::map< std::string, int >::iterator iS( moduleStates.find( nameFilter ) );
        if ( iS != moduleStates.end() ) {
          if ( ! triggerFilter.setStatus( iS->second ) ) {
            triggerFilter.setStatus( -1 ); // FIXME different code for "unvalid status determined" needed?
          }
        } else {
          triggerFilter.setStatus( -1 ); // FIXME different code for "unknown" needed?
        }
        // store filter
        triggerFilters->push_back( triggerFilter );
      }
    }

    // Put HLT filters to event
    if ( ! onlyStandAlone_ ) iEvent.put( triggerFilters );

    // HLT objects

    const trigger::Keys & collectionKeys( handleTriggerEvent->collectionKeys() );
    for ( size_t iO = 0, iC = 0; iO < sizeObjects && iC < handleTriggerEvent->sizeCollections(); ++iO ) {

      TriggerObject triggerObject( handleTriggerEvent->getObjects().at( iO ) );
      // set collection
      while ( iO >= collectionKeys[ iC ] ) ++iC; // relies on well ordering of trigger objects with respect to the collections
      triggerObject.setCollection( handleTriggerEvent->collectionTag( iC ) );
      // set filter ID
      for ( std::multimap< trigger::size_type, int >::iterator iM = filterIds.begin(); iM != filterIds.end(); ++iM ) {
        if ( iM->first == iO && ! triggerObject.hasFilterId( iM->second ) ) {
          triggerObject.addFilterId( iM->second );
        }
      }
      if ( ! onlyStandAlone_ ) triggerObjects->push_back( triggerObject );
      // stand-alone trigger object
      TriggerObjectStandAlone triggerObjectStandAlone( triggerObject );
      for ( std::multimap< trigger::size_type, std::string >::iterator iM = filterLabels.begin(); iM != filterLabels.end(); ++iM ) {
        if ( iM->first == iO ) {
          triggerObjectStandAlone.addFilterLabel( iM->second );
          for ( std::multimap< std::string, std::pair< std::string, bool > >::iterator iP = filterPaths.begin(); iP != filterPaths.end(); ++iP ) {
            if ( iP->first == iM->second ) {
              triggerObjectStandAlone.addPathName( iP->second.first, iP->second.second );
            }
          }
        }
      }

      triggerObjectsStandAlone->push_back( triggerObjectStandAlone );
    }

  } // if ( goodHlt )

  // L1 objects
  // (needs to be done after HLT objects, since their x-links with filters rely on their collection keys)

  if ( ! tagL1ExtraMu_.label().empty() ) {
    Handle< l1extra::L1MuonParticleCollection > handleL1ExtraMu;
    iEvent.getByLabel( tagL1ExtraMu_, handleL1ExtraMu );
    if ( handleL1ExtraMu.isValid() ) {
      for ( size_t l1Mu = 0; l1Mu < handleL1ExtraMu->size(); ++l1Mu ) {
        TriggerObject triggerObject;
        if ( saveL1Refs_ ) {
          const reco::CandidateBaseRef leafCandRef( l1extra::L1MuonParticleRef( handleL1ExtraMu, l1Mu ) );
          triggerObject = TriggerObject( leafCandRef );
        } else {
          const reco::LeafCandidate * leafCandidate( handleL1ExtraMu->at( l1Mu ).reco::LeafCandidate::clone() );
          triggerObject = TriggerObject( *leafCandidate );
        }
        triggerObject.setCollection( tagL1ExtraMu_ );
        triggerObject.addFilterId( trigger::TriggerL1Mu );
        if ( ! onlyStandAlone_ ) triggerObjects->push_back( triggerObject );
        triggerObjectsStandAlone->push_back( TriggerObjectStandAlone( triggerObject ) );
      }
    } else LogError( "errorL1ExtraMuValid" ) << "l1extra::L1MuonParticleCollection product with InputTag " << tagL1ExtraMu_.encode() << " not in event";
  }
  if ( ! tagL1ExtraNoIsoEG_.label().empty() ) {
    Handle< l1extra::L1EmParticleCollection > handleL1ExtraNoIsoEG;
    iEvent.getByLabel( tagL1ExtraNoIsoEG_, handleL1ExtraNoIsoEG );
    if ( handleL1ExtraNoIsoEG.isValid() ) {
      for ( size_t l1NoIsoEG = 0; l1NoIsoEG < handleL1ExtraNoIsoEG->size(); ++l1NoIsoEG ) {
        TriggerObject triggerObject;
        if ( saveL1Refs_ ) {
          const reco::CandidateBaseRef leafCandRef( l1extra::L1EmParticleRef( handleL1ExtraNoIsoEG, l1NoIsoEG ) );
          triggerObject = TriggerObject( leafCandRef );
        } else {
          const reco::LeafCandidate * leafCandidate( handleL1ExtraNoIsoEG->at( l1NoIsoEG ).reco::LeafCandidate::clone() );
          triggerObject = TriggerObject( *leafCandidate );
        }
        triggerObject.setCollection( tagL1ExtraNoIsoEG_ );
        triggerObject.addFilterId( trigger::TriggerL1NoIsoEG );
        if ( ! onlyStandAlone_ ) triggerObjects->push_back( triggerObject );
        triggerObjectsStandAlone->push_back( TriggerObjectStandAlone( triggerObject ) );
      }
    } else LogError( "errorL1ExtraNoIsoEGValid" ) << "l1extra::L1EmParticleCollection product with InputTag " << tagL1ExtraNoIsoEG_.encode() << " not in event";
  }
  if ( ! tagL1ExtraIsoEG_.label().empty() ) {
    Handle< l1extra::L1EmParticleCollection > handleL1ExtraIsoEG;
    iEvent.getByLabel( tagL1ExtraIsoEG_, handleL1ExtraIsoEG );
    if ( handleL1ExtraIsoEG.isValid() ) {
      for ( size_t l1IsoEG = 0; l1IsoEG < handleL1ExtraIsoEG->size(); ++l1IsoEG ) {
        TriggerObject triggerObject;
        if ( saveL1Refs_ ) {
          const reco::CandidateBaseRef leafCandRef( l1extra::L1EmParticleRef( handleL1ExtraIsoEG, l1IsoEG ) );
          triggerObject = TriggerObject( leafCandRef );
        } else {
          const reco::LeafCandidate * leafCandidate( handleL1ExtraIsoEG->at( l1IsoEG ).reco::LeafCandidate::clone() );
          triggerObject = TriggerObject( *leafCandidate );
        }
        triggerObject.setCollection( tagL1ExtraIsoEG_ );
        triggerObject.addFilterId( trigger::TriggerL1IsoEG );
        if ( ! onlyStandAlone_ ) triggerObjects->push_back( triggerObject );
        triggerObjectsStandAlone->push_back( TriggerObjectStandAlone( triggerObject ) );
      }
    } else LogError( "errorL1ExtraisoEGValid" ) << "l1extra::L1EmParticleCollection product with InputTag " << tagL1ExtraIsoEG_.encode() << " not in event";
  }
  if ( ! tagL1ExtraCenJet_.label().empty() ) {
    Handle< l1extra::L1JetParticleCollection > handleL1ExtraCenJet;
    iEvent.getByLabel( tagL1ExtraCenJet_, handleL1ExtraCenJet );
    if ( handleL1ExtraCenJet.isValid() ) {
      for ( size_t l1CenJet = 0; l1CenJet < handleL1ExtraCenJet->size(); ++l1CenJet ) {
        TriggerObject triggerObject;
        if ( saveL1Refs_ ) {
          const reco::CandidateBaseRef leafCandRef( l1extra::L1JetParticleRef( handleL1ExtraCenJet, l1CenJet ) );
          triggerObject = TriggerObject( leafCandRef );
        } else {
          const reco::LeafCandidate * leafCandidate( handleL1ExtraCenJet->at( l1CenJet ).reco::LeafCandidate::clone() );
          triggerObject = TriggerObject( *leafCandidate );
        }
        triggerObject.setCollection( tagL1ExtraCenJet_ );
        triggerObject.addFilterId( trigger::TriggerL1CenJet );
        if ( ! onlyStandAlone_ ) triggerObjects->push_back( triggerObject );
        triggerObjectsStandAlone->push_back( TriggerObjectStandAlone( triggerObject ) );
      }
    } else LogError( "errorL1ExtraCenJetValid" ) << "l1extra::L1JetParticleCollection product with InputTag " << tagL1ExtraCenJet_.encode() << " not in event";
  }
  if ( ! tagL1ExtraForJet_.label().empty() ) {
    Handle< l1extra::L1JetParticleCollection > handleL1ExtraForJet;
    iEvent.getByLabel( tagL1ExtraForJet_, handleL1ExtraForJet );
    if ( handleL1ExtraForJet.isValid() ) {
      for ( size_t l1ForJet = 0; l1ForJet < handleL1ExtraForJet->size(); ++l1ForJet ) {
        TriggerObject triggerObject;
        if ( saveL1Refs_ ) {
          const reco::CandidateBaseRef leafCandRef( l1extra::L1JetParticleRef( handleL1ExtraForJet, l1ForJet ) );
          triggerObject = TriggerObject( leafCandRef );
        } else {
          const reco::LeafCandidate * leafCandidate( handleL1ExtraForJet->at( l1ForJet ).reco::LeafCandidate::clone() );
          triggerObject = TriggerObject( *leafCandidate );
        }
        triggerObject.setCollection( tagL1ExtraForJet_ );
        triggerObject.addFilterId( trigger::TriggerL1ForJet );
        if ( ! onlyStandAlone_ ) triggerObjects->push_back( triggerObject );
        triggerObjectsStandAlone->push_back( TriggerObjectStandAlone( triggerObject ) );
      }
    } else LogError( "errorL1ExtraForJetValid" ) << "l1extra::L1JetParticleCollection product with InputTag " << tagL1ExtraForJet_.encode() << " not in event";
  }
  if ( ! tagL1ExtraTauJet_.label().empty() ) {
    Handle< l1extra::L1JetParticleCollection > handleL1ExtraTauJet;
    iEvent.getByLabel( tagL1ExtraTauJet_, handleL1ExtraTauJet );
    if ( handleL1ExtraTauJet.isValid() ) {
      for ( size_t l1TauJet = 0; l1TauJet < handleL1ExtraTauJet->size(); ++l1TauJet ) {
        TriggerObject triggerObject;
        if ( saveL1Refs_ ) {
          const reco::CandidateBaseRef leafCandRef( l1extra::L1JetParticleRef( handleL1ExtraTauJet, l1TauJet ) );
          triggerObject = TriggerObject( leafCandRef );
        } else {
          const reco::LeafCandidate * leafCandidate( handleL1ExtraTauJet->at( l1TauJet ).reco::LeafCandidate::clone() );
          triggerObject = TriggerObject( *leafCandidate );
        }
        triggerObject.setCollection( tagL1ExtraTauJet_ );
        triggerObject.addFilterId( trigger::TriggerL1TauJet );
        if ( ! onlyStandAlone_ ) triggerObjects->push_back( triggerObject );
        triggerObjectsStandAlone->push_back( TriggerObjectStandAlone( triggerObject ) );
      }
    } else LogError( "errorL1ExtraTauJetValid" ) << "l1extra::L1JetParticleCollection product with InputTag " << tagL1ExtraTauJet_.encode() << " not in event";
  }
  if ( ! tagL1ExtraETM_ .label().empty()) {
    Handle< l1extra::L1EtMissParticleCollection > handleL1ExtraETM;
    iEvent.getByLabel( tagL1ExtraETM_, handleL1ExtraETM );
    if ( handleL1ExtraETM.isValid() ) {
      for ( size_t l1ETM = 0; l1ETM < handleL1ExtraETM->size(); ++l1ETM ) {
        TriggerObject triggerObject;
        if ( saveL1Refs_ ) {
          const reco::CandidateBaseRef leafCandRef( l1extra::L1EtMissParticleRef( handleL1ExtraETM, l1ETM ) );
          triggerObject = TriggerObject( leafCandRef );
        } else {
          const reco::LeafCandidate * leafCandidate( handleL1ExtraETM->at( l1ETM ).reco::LeafCandidate::clone() );
          triggerObject = TriggerObject( *leafCandidate );
        }
        triggerObject.setCollection( tagL1ExtraETM_ );
        triggerObject.addFilterId( trigger::TriggerL1ETM );
        if ( ! onlyStandAlone_ ) triggerObjects->push_back( triggerObject );
        triggerObjectsStandAlone->push_back( TriggerObjectStandAlone( triggerObject ) );
      }
    } else LogError( "errorL1ExtraETMValid" ) << "l1extra::L1EtMissParticleCollection product with InputTag " << tagL1ExtraETM_.encode() << " not in event";
  }
  if ( ! tagL1ExtraHTM_.label().empty() ) {
    Handle< l1extra::L1EtMissParticleCollection > handleL1ExtraHTM;
    iEvent.getByLabel( tagL1ExtraHTM_, handleL1ExtraHTM );
    if ( handleL1ExtraHTM.isValid() ) {
      for ( size_t l1HTM = 0; l1HTM < handleL1ExtraHTM->size(); ++l1HTM ) {
        TriggerObject triggerObject;
        if ( saveL1Refs_ ) {
          const reco::CandidateBaseRef leafCandRef( l1extra::L1EtMissParticleRef( handleL1ExtraHTM, l1HTM ) );
          triggerObject = TriggerObject( leafCandRef );
        } else {
          const reco::LeafCandidate * leafCandidate( handleL1ExtraHTM->at( l1HTM ).reco::LeafCandidate::clone() );
          triggerObject = TriggerObject( *leafCandidate );
        }
        triggerObject.setCollection( tagL1ExtraHTM_ );
        triggerObject.addFilterId( trigger::TriggerL1HTM );
        if ( ! onlyStandAlone_ ) triggerObjects->push_back( triggerObject );
        triggerObjectsStandAlone->push_back( TriggerObjectStandAlone( triggerObject ) );
      }
    } else LogError( "errorL1ExtraHTMValid" ) << "l1extra::L1EtMissParticleCollection product with InputTag " << tagL1ExtraHTM_.encode() << " not in event";
  }

  // Put HLT & L1 objects to event
  if ( ! onlyStandAlone_ ) iEvent.put( triggerObjects );
  iEvent.put( triggerObjectsStandAlone );

  // L1 algorithms
  if ( ! onlyStandAlone_ ) {
    std::auto_ptr< TriggerAlgorithmCollection > triggerAlgos( new TriggerAlgorithmCollection() );
    if ( addL1Algos_ ) {
      l1GtUtils_.retrieveL1EventSetup( iSetup );
      ESHandle< L1GtTriggerMenu > handleL1GtTriggerMenu;
      iSetup.get< L1GtTriggerMenuRcd >().get( handleL1GtTriggerMenu );
      const AlgorithmMap l1GtAlgorithms( handleL1GtTriggerMenu->gtAlgorithmMap() );
      const AlgorithmMap l1GtTechTriggers( handleL1GtTriggerMenu->gtTechnicalTriggerMap() );
      triggerAlgos->reserve( l1GtAlgorithms.size() + l1GtTechTriggers.size() );
      // physics algorithms
      for ( AlgorithmMap::const_iterator iAlgo = l1GtAlgorithms.begin(); iAlgo != l1GtAlgorithms.end(); ++iAlgo ) {
        if ( iAlgo->second.algoBitNumber() > 127 ) {
          LogError( "errorL1AlgoBit" ) << "L1 algorithm '" << iAlgo->second.algoName() << "' has bit number " << iAlgo->second.algoBitNumber() << " > 127; skipping";
          continue;
        }
        L1GtUtils::TriggerCategory category;
        int bit;
        if ( ! l1GtUtils_.l1AlgoTechTrigBitNumber( iAlgo->second.algoName(), category, bit ) ) {
          LogError( "errorL1AlgoName" ) << "L1 algorithm '" << iAlgo->second.algoName() << "' not found in the L1 menu; skipping";
          continue;
        }
        bool decisionBeforeMask;
        bool decisionAfterMask;
        int  prescale;
        int  mask;
        int  error( l1GtUtils_.l1Results( iEvent, iAlgo->second.algoName(), decisionBeforeMask, decisionAfterMask, prescale, mask ) );
        if ( error ) {
          LogError( "errorL1AlgoDecision" ) << "L1 algorithm '" << iAlgo->second.algoName() << "' decision has error code " << error << " from 'L1GtUtils'; skipping";
          continue;
        }
        TriggerAlgorithm triggerAlgo( iAlgo->second.algoName(), iAlgo->second.algoAlias(), category == L1GtUtils::TechnicalTrigger, (unsigned)bit, (unsigned)prescale, (bool)mask, decisionBeforeMask, decisionAfterMask );
        triggerAlgos->push_back( triggerAlgo );
      }
      // technical triggers
      for ( AlgorithmMap::const_iterator iAlgo = l1GtTechTriggers.begin(); iAlgo != l1GtTechTriggers.end(); ++iAlgo ) {
        if ( iAlgo->second.algoBitNumber() > 63 ) {
          LogError( "errorL1AlgoBit" ) << "L1 algorithm '" << iAlgo->second.algoName() << "' has bit number " << iAlgo->second.algoBitNumber() << " > 63; skipping";
          continue;
        }
        L1GtUtils::TriggerCategory category;
        int bit;
        if ( ! l1GtUtils_.l1AlgoTechTrigBitNumber( iAlgo->second.algoName(), category, bit ) ) {
          LogError( "errorL1AlgoName" ) << "L1 algorithm '" << iAlgo->second.algoName() << "' not found in the L1 menu; skipping";
          continue;
        }
        bool decisionBeforeMask;
        bool decisionAfterMask;
        int  prescale;
        int  mask;
        int  error( l1GtUtils_.l1Results( iEvent, iAlgo->second.algoName(), decisionBeforeMask, decisionAfterMask, prescale, mask ) );
        if ( error ) {
          LogError( "errorL1AlgoDecision" ) << "L1 algorithm '" << iAlgo->second.algoName() << "' decision has error code " << error << " from 'L1GtUtils'; skipping";
          continue;
        }
        TriggerAlgorithm triggerAlgo( iAlgo->second.algoName(), iAlgo->second.algoAlias(), category == L1GtUtils::TechnicalTrigger, (unsigned)bit, (unsigned)prescale, (bool)mask, decisionBeforeMask, decisionAfterMask );
        triggerAlgos->push_back( triggerAlgo );
      }
    }

    // Put L1 algorithms to event
    iEvent.put( triggerAlgos ); // empty, but in event if 'addL1Algos_' is false for consistency with the PATTriggerEventProducer
  }

}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( PATTriggerProducer );
