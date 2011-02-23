// -*- C++ -*-
//
// Package:    PhysicsTools/PatAlgos
// Class:      pat::TestTriggerInfo
//
// $Id: TestTriggerInfo.cc,v 1.1 2010/03/03 18:59:27 vadler Exp $
//
/**
  \class TestTriggerInfo TestTriggerInfo.cc "PhysicsTools/PatAlgos/test/private/TestTriggerInfo.cc"
  \brief

   [...]

  \author   Volker Adler
  \version  $Id: TestTriggerInfo.cc,v 1.1 2010/03/03 18:59:27 vadler Exp $
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
      edm::InputTag tagPatTriggerObjectStandAlones_
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
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "PhysicsTools/PatUtils/interface/TriggerHelper.h"


using namespace pat;
using namespace pat::helper;


TestTriggerInfo::TestTriggerInfo( const edm::ParameterSet & iConfig ) :
: tagPatTriggerEvent_( iConfig.getParameter< edm::InputTag >( "patTriggerEvent" ) )
, tagPatTriggerObjectStandAlones_( iConfig.getParameter< edm::InputTag >( "tagPatTriggerObjectStandAlones" ) )
, histos1D_()
, histos2D_()
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
    edm::LogError( "patTriggerEventInvalid" ) << " pat::TriggerEvent with InputTag '" << tagPatTriggerEvent_.encode() << "' not valid";
    return;
  }

  const TriggerAlgorithmCollection  * patTriggerAlgorithms( patTriggerEvent->algorithms() );
  const TriggerConditionCollection  * patTriggerConditions( patTriggerEvent->conditions() );
  const TriggerPathCollection       * patTriggerPaths( patTriggerEvent->paths() );
  const TriggerFilterCollection     * patTriggerFilters( patTriggerEvent->filters() );
  const TriggerObjectCollection     * patTriggerObjects( patTriggerEvent->objects() );
  if ( ! patTriggerAlgorithms ) {
    edm::LogError( "patTriggerAlgorithmsInvalid" ) << "pat::TriggerAlgorithmCollection not found";
    return;
  }
  if ( ! patTriggerConditions ) {
    edm::LogError( "patTriggerConditionsInvalid" ) << "pat::TriggerConditionCollection not found";
    return;
  }
  if ( ! patTriggerPaths ) {
    edm::LogError( "patTriggerPathsInvalid" ) << "pat::TriggerPathCollection not found";
    return;
  }
  if ( ! patTriggerFilters ) {
    edm::LogError( "patTriggerFiltersInvalid" ) << "pat::TriggerFilterCollection not found";
    return;
  }
  if ( ! patTriggerObjects ) {
    edm::LogError( "patTriggerObjectsInvalid" ) << "pat::TriggerObjectCollection not found";
    return;
  }

  const TriggerObjectMatchContainer * patTriggerMatches( patTriggerEvent->triggerObjectMatchResults() );
  TriggerMatchHelper triggerMatchHelper;

  edm::Handle< TriggerObjectStandAloneCollection > patTriggerObjectStandAlones;
  iEvent.getByLabel( tagPatTriggerObjectStandAlones_, patTriggerObjectStandAlones );
  if ( ! patTriggerObjectStandAlones.isValid() ) {
    edm::LogError( "patTriggerObjectStandAlonesInvalid" ) << " pat::TriggerObjectStandAloneCollection with InputTag '" << tagPatTriggerObjectStandAlones_.encode() << "' not valid";
    return;
  }

}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( TestTriggerInfo );
