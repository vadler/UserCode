// -*- C++ -*-
//
// Package:    PhysicsTools/PatAlgos
// Class:      pat::TestTriggerInfo
//
// $Id: TestTriggerInfo.cc,v 1.15 2010/02/28 13:37:09 vadler Exp $
//
/**
  \class TestTriggerInfo TestTriggerInfo.cc "PhysicsTools/PatAlgos/test/private/TestTriggerInfo.cc"
  \brief

   [...]

  \author   Volker Adler
  \version  $Id:$
*/


#include "FWCore/Framework/interface/EDAnalyzer.h"

#include <string>
#include <map>

#include "FWCore/Utilities/interface/InputTag.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "TH1D.h"
#include "TH2D.h"


namespace pat {

  class TestTriggerInfo : public edm::EDAnalyzer {

      // Data members

      // Configuration parameters
      edm::InputTag tagPatTriggerEvent_;
      // Output histograms
      std::map< std::string, TH1D* > histos1D_;
      std::map< std::string, TH2D* > histos2D_;

    public:

      explicit TestTriggerInfo( const edm::ParameterSet & iConfig );
      ~TestTriggerInfo() {};

    private:

      virtual void beginJob();
      virtual void analyze( const edm::Event & iEvent, const edm::EventSetup & iSetup );

  };

}


#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include "PhysicsTools/PatUtils/interface/TriggerHelper.h"


using namespace pat;
using namespace pat::helper;


TestTriggerInfo::TestTriggerInfo( const edm::ParameterSet & iConfig ) :
  tagPatTriggerEvent_( iConfig.getParameter< edm::InputTag >( "patTriggerEvent" ) ),
  histos1D_(),
  histos2D_()
{



}


void TestTriggerInfo::beginJob()
{

  edm::Service< TFileService > fileService;

}


void TestTriggerInfo::analyze( const edm::Event & iEvent, const edm::EventSetup & iSetup )
{

  edm::Handle< TriggerEvent > patTriggerEvent;
  iEvent.getByLabel( tagPatTriggerEvent_, patTriggerEvent );
  if ( ! patTriggerEvent.isValid() ) {
    edm::LogError( "patTriggerEventInvalid" ) << " pat::TriggerEvent with the tag " << tagPatTriggerEvent_.encode() << " not valid.";
    return;
  }
  const TriggerPathCollection       * patTriggerPaths( patTriggerEvent->paths() );
  const TriggerFilterCollection     * patTriggerFilters( patTriggerEvent->filters() );
  const TriggerObjectCollection     * patTriggerObjects( patTriggerEvent->objects() );
  const TriggerObjectMatchContainer * patTriggerMatches( patTriggerEvent->triggerObjectMatchResults() );
  TriggerMatchHelper triggerMatchHelper;

}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( TestTriggerInfo );
