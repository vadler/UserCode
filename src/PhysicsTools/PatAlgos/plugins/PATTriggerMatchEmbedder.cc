// -*- C++ -*-
//
// Package:    PatAlgos
// Class:      pat::PATTriggerMatchEmbedder
//
/**
  \class    pat::PATTriggerMatchEmbedder PATTriggerMatchEmbedder.cc "PhysicsTools/PatAlgos/plugins/PATTriggerMatchEmbedder.cc"
  \brief

   .

  \author   Volker Adler
  \version  $Id$
*/
//
// $Id$
//


#include <vector>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/RefToBase.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"


namespace pat {

  template< class PATObjectType, class RecoObjectType >
  class PATTriggerMatchEmbedder : public edm::EDProducer {

      edm::InputTag src_;
      std::vector< edm::InputTag > matches_;

    public:

      explicit PATTriggerMatchEmbedder( const edm::ParameterSet & iConfig );
      ~PATTriggerMatchEmbedder() {};

    private:

      virtual void produce( edm::Event & iEvent, const edm::EventSetup & iSetup );

  };

  typedef PATTriggerMatchEmbedder< Electron, ElectronType > PATTriggerMatchElectronEmbedder;
  typedef PATTriggerMatchEmbedder< Jet     , JetType >      PATTriggerMatchJetEmbedder;
  typedef PATTriggerMatchEmbedder< MET     , METType >      PATTriggerMatchMETEmbedder;
  typedef PATTriggerMatchEmbedder< Muon    , MuonType >     PATTriggerMatchMuonEmbedder;
  typedef PATTriggerMatchEmbedder< Photon  , PhotonType >   PATTriggerMatchPhotonEmbedder;
  typedef PATTriggerMatchEmbedder< Tau     , TauType >      PATTriggerMatchTauEmbedder;

}


using namespace pat;

template< class PATObjectType, class RecoObjectType >
PATTriggerMatchEmbedder< PATObjectType, RecoObjectType >::PATTriggerMatchEmbedder( const edm::ParameterSet & iConfig ) :
  src_( iConfig.getParameter< edm::InputTag >( "src" ) ),
  matches_( iConfig.getParameter< std::vector< edm::InputTag > >( "matches" ) )
{
  produces< std::vector< PATObjectType > >();
}

template< class PATObjectType, class RecoObjectType >
void PATTriggerMatchEmbedder< PATObjectType, RecoObjectType >::produce( edm::Event & iEvent, const edm::EventSetup & iSetup )
{  
  std::auto_ptr< std::vector< PATObjectType > > output( new std::vector< PATObjectType >() );
  
  edm::Handle< edm::View< PATObjectType > > candidates;
  iEvent.getByLabel( src_, candidates );
  if ( ! candidates.isValid() ) {
    edm::LogError( "missingInputSource" ) << "Input source with InputTag " << src_.encode() << " not in event.";
    return;
  }

  for ( size_t iMatch = 0; iMatch < matches_.size(); ++iMatch ) {
    edm::Handle< TriggerObjectStandAloneMatch > match;
    iEvent.getByLabel( matches_.at( iMatch ), match );
    if ( ! match.isValid() ) {
      edm::LogError( "missingInputMatch" ) << "Input match with InputTag " << matches_.at( iMatch ).encode() << " not in event.";
      continue;
    }
    for ( typename edm::View< PATObjectType >::const_iterator iCand = candidates->begin(); iCand != candidates->end(); ++iCand ) {
      const unsigned index( iCand - candidates->begin() );
      const edm::RefToBase< RecoObjectType > candRef( candidates->refAt( index ) );
      PATObjectType cand( candRef );
      const TriggerObjectStandAloneRef trigRef( ( *match )[ candRef ] );
      if ( trigRef.isNonnull() && trigRef.isAvailable() ) {
        cand.addTriggerObjectMatch( *trigRef );
      }
      output->push_back( cand );
    }
  }

  iEvent.put( output );
}


#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE( PATTriggerMatchElectronEmbedder );
DEFINE_FWK_MODULE( PATTriggerMatchJetEmbedder );
DEFINE_FWK_MODULE( PATTriggerMatchMETEmbedder );
DEFINE_FWK_MODULE( PATTriggerMatchMuonEmbedder );
DEFINE_FWK_MODULE( PATTriggerMatchPhotonEmbedder );
DEFINE_FWK_MODULE( PATTriggerMatchTauEmbedder );
