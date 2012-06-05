// -*- C++ -*-
//
// Package:    TopTools
// Class:      TtSemiLepHypSelectionFilter
//
// $Id:$
//
/**
  \class    TtSemiLepHypSelectionFilter TtSemiLepHypSelectionFilter.cc "TopQuarkAnalysis/TopTools/plugins/TtSemiLepHypSelectionFilter.cc"
  \brief    Select events with different selection steps for objects, where an event hypothesis holds still after tighter selections



  \author   Volker Adler
  \version  $Id:$
*/


#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include <string>

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"


class TtSemiLepHypSelectionFilter : public edm::EDFilter {

    // Configuration
    edm::InputTag ttSemiLepEvtTag_;
    std::string   ttSemiLepHyp_;
    edm::InputTag ttSemiLepHypLeptonsTag_;
    edm::InputTag ttSemiLepHypJetsTag_;
    std::string   processName_;
    std::string   leptonSelector_;
    std::string   jetSelector_;

    // Provenance
    HLTConfigProvider origConfig_;
    std::string       leptonSelectorCut_;
    std::string       jetSelectorCut_;

  public:

    explicit TtSemiLepHypSelectionFilter( const edm::ParameterSet & iConfig );
    virtual ~TtSemiLepHypSelectionFilter() {};

  private:

    virtual bool beginRun( edm::Run & iRun, const edm::EventSetup & iSetup );
    virtual bool filter( edm::Event & iEvent, const edm::EventSetup & iSetup );

};


#include "AnalysisDataFormats/TopObjects/interface/TtSemiLeptonicEvent.h"
#include "AnalysisDataFormats/TopObjects/interface/TtSemiLepEvtPartons.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"


TtSemiLepHypSelectionFilter::TtSemiLepHypSelectionFilter( const edm::ParameterSet & iConfig )
: ttSemiLepEvtTag_( iConfig.getParameter< edm::InputTag >( "ttSemiLepEvt" ) )
, ttSemiLepHyp_( iConfig.getParameter< std::string >( "ttSemiLepHyp" ) )
, ttSemiLepHypLeptonsTag_( iConfig.getParameter< edm::InputTag >( "ttSemiLepHypLeptons" ) )
, ttSemiLepHypJetsTag_( iConfig.getParameter< edm::InputTag >( "ttSemiLepHypJets" ) )
, processName_( iConfig.getParameter< std::string >( "processName" ) )
, leptonSelector_( iConfig.getParameter< std::string >( "leptonSelector" ) )
, jetSelector_( iConfig.getParameter< std::string >( "jetSelector" ) )
{
}


bool TtSemiLepHypSelectionFilter::beginRun( edm::Run & iRun, const edm::EventSetup & iSetup )
{

  // Initialise HLTConfigProvider
  bool changed( true );
  if ( ! origConfig_.init( iRun, iSetup, processName_, changed ) ) {
    edm::LogError( "hltConfigExtraction" ) << "HLT config extraction error with process name '" << processName_ << "'";
    return false;
  } else if ( origConfig_.size() <= 0 ) {
    edm::LogError( "hltConfigSize" ) << "HLT config size error";
    return false;
  }

  // Get original configuration
  if ( origConfig_.moduleType( leptonSelector_ ) == "PATMuonSelector" ) {
  }
  else if ( origConfig_.moduleType( leptonSelector_ ) == "PATElectronSelector" ) {
  }
  leptonSelectorCut_ = origConfig_.modulePSet( leptonSelector_ ).getParameter< std::string >( "cut" );
  edm::LogInfo( "leptonCut" ) << leptonSelectorCut_;
  jetSelectorCut_    = origConfig_.modulePSet( jetSelector_ ).getParameter< std::string >( "cut" );
  edm::LogInfo( "jetCut" )    << jetSelectorCut_;

  return true;

}


bool TtSemiLepHypSelectionFilter::filter( edm::Event & iEvent, const edm::EventSetup & iSetup )
{

  bool result( true );

  edm::Handle< TtSemiLeptonicEvent > ttSemiLepEvt;
  iEvent.getByLabel( ttSemiLepEvtTag_, ttSemiLepEvt );
  if ( ! ttSemiLepEvt->isHypoValid( ttSemiLepHyp_ ) ) {
    edm::LogInfo( "hypoValid" ) << "Event hypothesis '" << ttSemiLepHyp_ << "' not available/valid.";
    return result;
  }
  const std::vector< int > jetLepCombi( ttSemiLepEvt->jetLeptonCombination( ttSemiLepHyp_ ) );

  if ( origConfig_.moduleType( leptonSelector_ ) == "PATMuonSelector" ) {
    StringCutObjectSelector< pat::Muon > muonSelector( leptonSelectorCut_ );
    edm::Handle< pat::MuonCollection > patMuons;
    iEvent.getByLabel( ttSemiLepHypLeptonsTag_, patMuons );
    const pat::Muon singleMuon( patMuons->at( jetLepCombi.at( TtSemiLepEvtPartons::Lepton ) ) );
    if ( ! muonSelector( singleMuon ) ) {
      edm::LogInfo( "singleMuon" );
      result = false;
    }
  }
  else if ( origConfig_.moduleType( leptonSelector_ ) == "PATElectronSelector" ) {
    StringCutObjectSelector< pat::Electron > electronSelector( leptonSelectorCut_ );
    edm::Handle< pat::ElectronCollection > patElectrons;
    iEvent.getByLabel( ttSemiLepHypLeptonsTag_, patElectrons );
    const pat::Electron singleElectron( patElectrons->at( jetLepCombi.at( TtSemiLepEvtPartons::Lepton ) ) );
    if ( ! electronSelector( singleElectron ) ) {
      edm::LogInfo( "singleElectron" );
      result = false;
    }
  }

  edm::Handle< pat::JetCollection > patJets;
  iEvent.getByLabel( ttSemiLepHypJetsTag_, patJets );
  StringCutObjectSelector< pat::Jet > jetSelector( jetSelectorCut_ );
  const pat::Jet leptonicDecayB( patJets->at( jetLepCombi.at( TtSemiLepEvtPartons::LepB ) ) );
  if ( ! jetSelector( leptonicDecayB ) ) {
    edm::LogInfo( "leptonicDecayB" );
    result = false;
  }
  const pat::Jet hadronicDecayB( patJets->at( jetLepCombi.at( TtSemiLepEvtPartons::HadB ) ) );
  if ( ! jetSelector( hadronicDecayB ) ) {
    edm::LogInfo( "hadronicDecayB" );
    result = false;
  }
  const pat::Jet hadronicDecayQuark( patJets->at( jetLepCombi.at( TtSemiLepEvtPartons::LightQ ) ) );
  if ( ! jetSelector( hadronicDecayQuark ) ) {
    edm::LogInfo( "hadronicDecayQuark" );
    result = false;
  }
  const pat::Jet hadronicDecayQuarkBar( patJets->at( jetLepCombi.at( TtSemiLepEvtPartons::LightQBar ) ) );
  if ( ! jetSelector( hadronicDecayQuarkBar ) ) {
    edm::LogInfo( "hadronicDecayQuarkBar" );
    result = false;
  }

  return result;

}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( TtSemiLepHypSelectionFilter );
