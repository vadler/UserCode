//
// $Id: PATTriggerProducer.cc,v 1.8 2010/01/12 19:28:36 vadler Exp $
//


#include "PhysicsTools/PatAlgos/plugins/PATTriggerProducer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <cassert>


using namespace pat;

PATTriggerProducer::PATTriggerProducer( const edm::ParameterSet & iConfig ) :
  // L1 configuration parameters
  tagL1GtReadoutRecord_( iConfig.getParameter< edm::InputTag >( "l1GtReadoutRecord" ) ),
  tagL1ExtraMu_( iConfig.getParameter< edm::InputTag >( "l1ExtraMu" ) ),
  tagL1ExtraNoIsoEG_( iConfig.getParameter< edm::InputTag >( "l1ExtraNoIsoEG" ) ),
  tagL1ExtraIsoEG_( iConfig.getParameter< edm::InputTag >( "l1ExtraIsoEG" ) ),
  tagL1ExtraCenJet_( iConfig.getParameter< edm::InputTag >( "l1ExtraCenJet" ) ),
  tagL1ExtraForJet_( iConfig.getParameter< edm::InputTag >( "l1ExtraForJet" ) ),
  tagL1ExtraTauJet_( iConfig.getParameter< edm::InputTag >( "l1ExtraTauJet" ) ),
  tagL1ExtraETM_( iConfig.getParameter< edm::InputTag >( "l1ExtraETM" ) ),
  tagL1ExtraHTM_( iConfig.getParameter< edm::InputTag >( "l1ExtraHTM" ) ),
  // HLT configuration parameters
  nameProcess_( iConfig.getParameter< std::string >( "processName" ) ),
  tagTriggerResults_( iConfig.getParameter< edm::InputTag >( "triggerResults" ) ),
  tagTriggerEvent_( iConfig.getParameter< edm::InputTag >( "triggerEvent" ) ),
  onlyStandAlone_( iConfig.getParameter< bool >( "onlyStandAlone" ) ),
  addPathModuleLabels_( iConfig.getParameter< bool >( "addPathModuleLabels" ) )
{
  if ( tagTriggerResults_.process().empty() ) {
    tagTriggerResults_ = edm::InputTag( tagTriggerResults_.label(), tagTriggerResults_.instance(), nameProcess_ );
  }
  if ( tagTriggerEvent_.process().empty() ) {
    tagTriggerEvent_ = edm::InputTag( tagTriggerEvent_.label(), tagTriggerEvent_.instance(), nameProcess_ );
  }

  if ( ! onlyStandAlone_ ) {
    produces< TriggerPathCollection >();
    produces< TriggerFilterCollection >();
    produces< TriggerObjectCollection >();
  }
  produces< TriggerObjectStandAloneCollection >();
}

PATTriggerProducer::~PATTriggerProducer()
{
}

void PATTriggerProducer::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
  bool changed( true );
  if ( ! hltConfig_.init( iEvent, nameProcess_, changed ) ) {
    edm::LogError( "errorHltConfigExtraction" ) << "HLT config extraction error with process name " << nameProcess_;
    return;
  }
  if ( hltConfig_.size() <= 0 ) {
    edm::LogError( "errorHltConfigSize" ) << "HLT config size error";
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

  // produce HLT paths and determine status of modules

  const unsigned sizePaths( hltConfig_.size() );
  const unsigned sizeFilters( handleTriggerEvent->sizeFilters() );
  const unsigned sizeObjects( handleTriggerEvent->sizeObjects() );

  std::auto_ptr< TriggerPathCollection > triggerPaths( new TriggerPathCollection() );
  triggerPaths->reserve( onlyStandAlone_ ? 0 : sizePaths );
  std::map< std::string, int > moduleStates;
  std::multimap< std::string, std::string > filterPaths;

  for ( size_t iP = 0; iP < sizePaths; ++iP ) {
    const std::string namePath( hltConfig_.triggerName( iP ) );
    const unsigned indexPath( hltConfig_.triggerIndex( namePath ) );
    const unsigned sizeModules( hltConfig_.size( namePath ) );
    for ( size_t iM = 0; iM < sizeModules; ++iM ) {
      const std::string nameModule( hltConfig_.moduleLabel( indexPath, iM ) );
      const unsigned indexFilter( handleTriggerEvent->filterIndex( edm::InputTag( nameModule, "", nameProcess_ ) ) );
      if ( indexFilter < sizeFilters ) {
        filterPaths.insert( std::pair< std::string, std::string >( nameModule, namePath ) );
      }
    }
    if ( ! onlyStandAlone_ ) {
      const unsigned indexLastFilter( handleTriggerResults->index( indexPath ) );
      TriggerPath triggerPath( namePath, indexPath, 0, handleTriggerResults->wasrun( indexPath ), handleTriggerResults->accept( indexPath ), handleTriggerResults->error( indexPath ), indexLastFilter );
      // add module names to path and states' map
      assert( indexLastFilter < sizeModules );
      std::map< unsigned, std::string > indicesModules;
      for ( size_t iM = 0; iM < sizeModules; ++iM ) {
        const std::string nameModule( hltConfig_.moduleLabel( indexPath, iM ) );
        if ( addPathModuleLabels_ ) {
          triggerPath.addModule( nameModule );
        }
        const unsigned indexFilter( handleTriggerEvent->filterIndex( edm::InputTag( nameModule, "", nameProcess_ ) ) );
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
        if ( iM->first < indexLastFilter ) {
          moduleStates[ iM->second ] = 1;
        } else if ( iM->first == indexLastFilter ) {
          moduleStates[ iM->second ] = handleTriggerResults->accept( indexPath );
        } else if ( moduleStates.find( iM->second ) == moduleStates.end() ) {
          moduleStates[ iM->second ] = -1;
        }
      }
    }
  }

  if ( ! onlyStandAlone_ ) {
    iEvent.put( triggerPaths );
  }

  // produce HLT filters and store used trigger object types
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
          triggerFilter.setStatus( -1 ); // different code for "unvalid status determined" needed?
        }
      } else {
        triggerFilter.setStatus( -1 ); // different code for "unknown" needed?
      }
      // store filter
      triggerFilters->push_back( triggerFilter );
    }
    // store used trigger object types to be used with the objects
  }

  if ( ! onlyStandAlone_ ) {
    iEvent.put( triggerFilters );
  }

  // produce trigger objects

  std::auto_ptr< TriggerObjectCollection > triggerObjects( new TriggerObjectCollection() );
//   triggerObjects->reserve( onlyStandAlone_ ? 0 : sizeObjects );
  std::auto_ptr< TriggerObjectStandAloneCollection > triggerObjectsStandAlone( new TriggerObjectStandAloneCollection() );
//   triggerObjectsStandAlone->reserve( sizeObjects );

  // HLT
  const trigger::Keys & collectionKeys( handleTriggerEvent->collectionKeys() );
  for ( size_t iO = 0, iC = 0; iO < sizeObjects && iC < handleTriggerEvent->sizeCollections(); ++iO ) {

    TriggerObject triggerObject( handleTriggerEvent->getObjects().at( iO ) );
    // set collection
    while ( iO >= collectionKeys[ iC ] ) {
      ++iC;
    } // relies on well ordering of trigger objects with respect to the collections
    triggerObject.setCollection( handleTriggerEvent->collectionTag( iC ).encode() );
    // set filter ID
    for ( std::multimap< trigger::size_type, int >::iterator iM = filterIds.begin(); iM != filterIds.end(); ++iM ) {
      if ( iM->first == iO && ! triggerObject.hasFilterId( iM->second ) ) {
        triggerObject.addFilterId( iM->second );
      }
    }
    if ( ! onlyStandAlone_ ) {
      triggerObjects->push_back( triggerObject );
    }
    // stand-alone trigger object
    TriggerObjectStandAlone triggerObjectStandAlone( triggerObject );
    for ( std::multimap< trigger::size_type, std::string >::iterator iM = filterLabels.begin(); iM != filterLabels.end(); ++iM ) {
      if ( iM->first == iO ) {
        triggerObjectStandAlone.addFilterLabel( iM->second );
        for ( std::multimap< std::string, std::string >::iterator iP = filterPaths.begin(); iP != filterPaths.end(); ++iP ) {
          if ( iP->first == iM->second ) {
            triggerObjectStandAlone.addPathName( iP->second );
          }
        }
      }
    }

    triggerObjectsStandAlone->push_back( triggerObjectStandAlone );
  }

  // L1
  edm::Handle< l1extra::L1MuonParticleCollection > handleL1ExtraMu;
  iEvent.getByLabel( tagL1ExtraMu_, handleL1ExtraMu );
  if ( ! handleL1ExtraMu.isValid() ) {
    edm::LogError( "errorL1ExtraMuValid" ) << "l1extra::L1MuonParticleCollection product with InputTag " << tagL1ExtraMu_.encode() << " not in event";
    return;
  }
  edm::Handle< l1extra::L1EmParticleCollection > handleL1ExtraNoIsoEG;
  iEvent.getByLabel( tagL1ExtraNoIsoEG_, handleL1ExtraNoIsoEG );
  if ( ! handleL1ExtraNoIsoEG.isValid() ) {
    edm::LogError( "errorL1ExtraNoIsoEGValid" ) << "l1extra::L1EmParticleCollection product with InputTag " << tagL1ExtraNoIsoEG_.encode() << " not in event";
    return;
  }
  edm::Handle< l1extra::L1EmParticleCollection > handleL1ExtraIsoEG;
  iEvent.getByLabel( tagL1ExtraIsoEG_, handleL1ExtraIsoEG );
  if ( ! handleL1ExtraIsoEG.isValid() ) {
    edm::LogError( "errorL1ExtraisoEGValid" ) << "l1extra::L1EmParticleCollection product with InputTag " << tagL1ExtraIsoEG_.encode() << " not in event";
    return;
  }
  edm::Handle< l1extra::L1JetParticleCollection > handleL1ExtraCenJet;
  iEvent.getByLabel( tagL1ExtraCenJet_, handleL1ExtraCenJet );
  if ( ! handleL1ExtraCenJet.isValid() ) {
    edm::LogError( "errorL1ExtraCenJetValid" ) << "l1extra::L1JetParticleCollection product with InputTag " << tagL1ExtraCenJet_.encode() << " not in event";
    return;
  }
  edm::Handle< l1extra::L1JetParticleCollection > handleL1ExtraForJet;
  iEvent.getByLabel( tagL1ExtraForJet_, handleL1ExtraForJet );
  if ( ! handleL1ExtraForJet.isValid() ) {
    edm::LogError( "errorL1ExtraForJetValid" ) << "l1extra::L1JetParticleCollection product with InputTag " << tagL1ExtraForJet_.encode() << " not in event";
    return;
  }
  edm::Handle< l1extra::L1JetParticleCollection > handleL1ExtraTauJet;
  iEvent.getByLabel( tagL1ExtraTauJet_, handleL1ExtraTauJet );
  if ( ! handleL1ExtraTauJet.isValid() ) {
    edm::LogError( "errorL1ExtraTauJetValid" ) << "l1extra::L1JetParticleCollection product with InputTag " << tagL1ExtraTauJet_.encode() << " not in event";
    return;
  }
  edm::Handle< l1extra::L1EtMissParticleCollection > handleL1ExtraETM;
  iEvent.getByLabel( tagL1ExtraETM_, handleL1ExtraETM );
  if ( ! handleL1ExtraETM.isValid() ) {
    edm::LogError( "errorL1ExtraETMValid" ) << "l1extra::L1EtMissParticleCollection product with InputTag " << tagL1ExtraETM_.encode() << " not in event";
    return;
  }
  edm::Handle< l1extra::L1EtMissParticleCollection > handleL1ExtraHTM;
  iEvent.getByLabel( tagL1ExtraHTM_, handleL1ExtraHTM );
  if ( ! handleL1ExtraHTM.isValid() ) {
    edm::LogError( "errorL1ExtraHTMValid" ) << "l1extra::L1EtMissParticleCollection product with InputTag " << tagL1ExtraHTM_.encode() << " not in event";
    return;
  }
  for ( size_t l1Mu = 0; l1Mu < handleL1ExtraMu->size(); ++l1Mu ) {
    const reco::LeafCandidate * ptrMu( handleL1ExtraMu->at( l1Mu ).clone() );
    TriggerObject triggerObject( *ptrMu );
    triggerObject.setCollection( tagL1ExtraMu_.encode() );
    triggerObject.addFilterId( trigger::TriggerL1Mu );
    if ( ! onlyStandAlone_ ) {
      triggerObjects->push_back( triggerObject );
    }
    TriggerObjectStandAlone triggerObjectStandAlone( triggerObject );
    triggerObjectsStandAlone->push_back( triggerObjectStandAlone );
  }
  for ( size_t l1NoIsoEG = 0; l1NoIsoEG < handleL1ExtraNoIsoEG->size(); ++l1NoIsoEG ) {
    const reco::LeafCandidate * ptrNoIsoEG( handleL1ExtraNoIsoEG->at( l1NoIsoEG ).clone() );
    TriggerObject triggerObject( *ptrNoIsoEG );
    triggerObject.setCollection( tagL1ExtraNoIsoEG_.encode() );
    triggerObject.addFilterId( trigger::TriggerL1NoIsoEG );
    if ( ! onlyStandAlone_ ) {
      triggerObjects->push_back( triggerObject );
    }
    TriggerObjectStandAlone triggerObjectStandAlone( triggerObject );
    triggerObjectsStandAlone->push_back( triggerObjectStandAlone );
  }
  for ( size_t l1IsoEG = 0; l1IsoEG < handleL1ExtraIsoEG->size(); ++l1IsoEG ) {
    const reco::LeafCandidate * ptrIsoEG( handleL1ExtraIsoEG->at( l1IsoEG ).clone() );
    TriggerObject triggerObject( *ptrIsoEG );
    triggerObject.setCollection( tagL1ExtraIsoEG_.encode() );
    triggerObject.addFilterId( trigger::TriggerL1IsoEG );
    if ( ! onlyStandAlone_ ) {
      triggerObjects->push_back( triggerObject );
    }
    TriggerObjectStandAlone triggerObjectStandAlone( triggerObject );
    triggerObjectsStandAlone->push_back( triggerObjectStandAlone );
  }
  for ( size_t l1CenJet = 0; l1CenJet < handleL1ExtraCenJet->size(); ++l1CenJet ) {
    const reco::LeafCandidate * ptrCenJet( handleL1ExtraCenJet->at( l1CenJet ).clone() );
    TriggerObject triggerObject( *ptrCenJet );
    triggerObject.setCollection( tagL1ExtraCenJet_.encode() );
    triggerObject.addFilterId( trigger::TriggerL1CenJet );
    if ( ! onlyStandAlone_ ) {
      triggerObjects->push_back( triggerObject );
    }
    TriggerObjectStandAlone triggerObjectStandAlone( triggerObject );
    triggerObjectsStandAlone->push_back( triggerObjectStandAlone );
  }
  for ( size_t l1ForJet = 0; l1ForJet < handleL1ExtraForJet->size(); ++l1ForJet ) {
    const reco::LeafCandidate * ptrForJet( handleL1ExtraForJet->at( l1ForJet ).clone() );
    TriggerObject triggerObject( *ptrForJet );
    triggerObject.setCollection( tagL1ExtraForJet_.encode() );
    triggerObject.addFilterId( trigger::TriggerL1ForJet );
    if ( ! onlyStandAlone_ ) {
      triggerObjects->push_back( triggerObject );
    }
    TriggerObjectStandAlone triggerObjectStandAlone( triggerObject );
    triggerObjectsStandAlone->push_back( triggerObjectStandAlone );
  }
  for ( size_t l1TauJet = 0; l1TauJet < handleL1ExtraTauJet->size(); ++l1TauJet ) {
    const reco::LeafCandidate * ptrTauJet( handleL1ExtraTauJet->at( l1TauJet ).clone() );
    TriggerObject triggerObject( *ptrTauJet );
    triggerObject.setCollection( tagL1ExtraTauJet_.encode() );
    triggerObject.addFilterId( trigger::TriggerL1TauJet );
    if ( ! onlyStandAlone_ ) {
      triggerObjects->push_back( triggerObject );
    }
    TriggerObjectStandAlone triggerObjectStandAlone( triggerObject );
    triggerObjectsStandAlone->push_back( triggerObjectStandAlone );
  }
  for ( size_t l1ETM = 0; l1ETM < handleL1ExtraETM->size(); ++l1ETM ) {
    const reco::LeafCandidate * ptrETM( handleL1ExtraETM->at( l1ETM ).clone() );
    TriggerObject triggerObject( *ptrETM );
    triggerObject.setCollection( tagL1ExtraETM_.encode() );
    triggerObject.addFilterId( trigger::TriggerL1ETM );
    if ( ! onlyStandAlone_ ) {
      triggerObjects->push_back( triggerObject );
    }
    TriggerObjectStandAlone triggerObjectStandAlone( triggerObject );
    triggerObjectsStandAlone->push_back( triggerObjectStandAlone );
  }
  for ( size_t l1HTM = 0; l1HTM < handleL1ExtraHTM->size(); ++l1HTM ) {
    const reco::LeafCandidate * ptrHTM( handleL1ExtraHTM->at( l1HTM ).clone() );
    TriggerObject triggerObject( *ptrHTM );
    triggerObject.setCollection( tagL1ExtraHTM_.encode() );
    triggerObject.addFilterId( trigger::TriggerL1HTM );
    if ( ! onlyStandAlone_ ) {
      triggerObjects->push_back( triggerObject );
    }
    TriggerObjectStandAlone triggerObjectStandAlone( triggerObject );
    triggerObjectsStandAlone->push_back( triggerObjectStandAlone );
  }

  if ( ! onlyStandAlone_ ) {
    iEvent.put( triggerObjects );
  }
  iEvent.put( triggerObjectsStandAlone );
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( PATTriggerProducer );
