// -*- C++ -*-
//
// Package:    TopMassSemiLeptonic
// Class:      TopHitFitAnalyzer
//
// $Id: TopHitFitAnalyzer.cc,v 1.1 2011/08/31 14:45:53 vadler Exp $
//
/**
  \class    TopHitFitAnalyzer TopHitFitAnalyzer.cc "TopQuarkAnalysis/TopMassSemiLeptonic/plugins/TopHitFitAnalyzer.cc"
  \brief    Analyzes



  \author   Volker Adler
  \version  $Id: TopHitFitAnalyzer.cc,v 1.1 2011/08/31 14:45:53 vadler Exp $
*/


#include <map>
#include <string>

#include "TMath.h"
#include "TH1D.h"
#include "TH2D.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"


class TopHitFitAnalyzer : public edm::EDAnalyzer {

  public:

    /// Constructors and Desctructor

    /// Default constructor
    explicit TopHitFitAnalyzer( const edm::ParameterSet & iConfig );

    /// Destructor
    virtual ~TopHitFitAnalyzer();

    /// Methods

    /// Begin job
    virtual void beginJob();

    /// Event loop
    virtual void analyze( const edm::Event & iEvent, const edm::EventSetup & iSetup );

    /// End job
    virtual void endJob();

  private:

    /// Data members

    /// Configuration parameters
    edm::InputTag ttSemiLeptonicEvent_;

    /// Histograms
    std::map< std::string, TH1D* > histos1D_;
    std::map< std::string, TH2D* > histos2D_;

};


#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "AnalysisDataFormats/TopObjects/interface/TtSemiLeptonicEvent.h"


// Default constructor
TopHitFitAnalyzer::TopHitFitAnalyzer( const edm::ParameterSet & iConfig )
: ttSemiLeptonicEvent_( iConfig.getParameter< edm::InputTag >( "ttSemiLeptonicEvent" ) )
, histos1D_()
, histos2D_()
{
}


// Destructor
TopHitFitAnalyzer::~TopHitFitAnalyzer()
{
}


// Begin job
void TopHitFitAnalyzer::beginJob()
{

  edm::Service< TFileService > fileService;

  // 1-dim
  histos1D_[ "hitFitProb" ] = fileService->make< TH1D >( "hitFitProb", "HitFit fit probabilty", 25, 0., 1. );
  histos1D_[ "hitFitProb" ]->SetXTitle( "probability" );
  histos1D_[ "hitFitProb" ]->SetYTitle( "events" );
  histos1D_[ "hitFitChi2" ] = fileService->make< TH1D >( "hitFitChi2", "HitFit fit #chi^{2}", 50, 0., 100. );
  histos1D_[ "hitFitChi2" ]->SetXTitle( "#chi^{2}" );
  histos1D_[ "hitFitChi2" ]->SetYTitle( "events" );
  histos1D_[ "hitFitMT" ] = fileService->make< TH1D >( "hitFitMT", "HitFit top mass", 50, 100., 300. );
  histos1D_[ "hitFitMT" ]->SetXTitle( "m^{t} (GeV)" );
  histos1D_[ "hitFitMT" ]->SetYTitle( "events" );
  histos1D_[ "hitFitSigMT" ] = fileService->make< TH1D >( "hitFitSigMT", "HitFit top mass uncertainty", 25, 0., 50. );
  histos1D_[ "hitFitSigMT" ]->SetXTitle( "#sigma_{m^{t}} (GeV)" );
  histos1D_[ "hitFitSigMT" ]->SetYTitle( "events" );
  histos1D_[ "hitFitTopLepMass" ] = fileService->make< TH1D >( "hitFitTopLepMass", "HitFit leptonic top mass", 50, 100., 300. );
  histos1D_[ "hitFitTopLepMass" ]->SetXTitle( "m^{t_{l}, HitFit} (GeV)" );
  histos1D_[ "hitFitTopLepMass" ]->SetYTitle( "events" );
  histos1D_[ "hitFitTopHadMass" ] = fileService->make< TH1D >( "hitFitTopHadMass", "HitFit hadronic top mass", 50, 100., 300. );
  histos1D_[ "hitFitTopHadMass" ]->SetXTitle( "m^{t_{h}, HitFit} (GeV)" );
  histos1D_[ "hitFitTopHadMass" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLeptonPt" ] = fileService->make< TH1D >( "hitFitLeptonPt", "HitFit lepton transverse momentum", 50, 0., 500. );
  histos1D_[ "hitFitLeptonPt" ]->SetXTitle( "p_{t}^{l, HitFit} (GeV)" );
  histos1D_[ "hitFitLeptonPt" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLeptonEta" ] = fileService->make< TH1D >( "hitFitLeptonEta", "HitFit lepton pseudo-rapidity", 50, -4., 4. );
  histos1D_[ "hitFitLeptonEta" ]->SetXTitle( "#eta^{l, HitFit}" );
  histos1D_[ "hitFitLeptonEta" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLeptonPhi" ] = fileService->make< TH1D >( "hitFitLeptonPhi", "HitFit lepton azimutal angle", 50, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "hitFitLeptonPhi" ]->SetXTitle( "#phi^{l, HitFit}" );
  histos1D_[ "hitFitLeptonPhi" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLeptonPz" ] = fileService->make< TH1D >( "hitFitLeptonPz", "HitFit lepton longitudinal momentum", 50, -1000., 1000. );
  histos1D_[ "hitFitLeptonPz" ]->SetXTitle( "p_{z}^{l, HitFit} (GeV)" );
  histos1D_[ "hitFitLeptonPz" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNeutrinoMass" ] = fileService->make< TH1D >( "hitFitNeutrinoMass", "HitFit neutrino mass", 50, -0.0001, 0.0001 );
  histos1D_[ "hitFitNeutrinoMass" ]->SetXTitle( "m^{#nu, HitFit} (GeV)" );
  histos1D_[ "hitFitNeutrinoMass" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNeutrinoPt" ] = fileService->make< TH1D >( "hitFitNeutrinoPt", "HitFit neutrino transverse momentum", 50, 0., 500. );
  histos1D_[ "hitFitNeutrinoPt" ]->SetXTitle( "p_{t}^{#nu, HitFit} (GeV)" );
  histos1D_[ "hitFitNeutrinoPt" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNeutrinoEta" ] = fileService->make< TH1D >( "hitFitNeutrinoEta", "HitFit neutrino pseudo-rapidity", 50, -4., 4. );
  histos1D_[ "hitFitNeutrinoEta" ]->SetXTitle( "#eta^{#nu, HitFit}" );
  histos1D_[ "hitFitNeutrinoEta" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNeutrinoPhi" ] = fileService->make< TH1D >( "hitFitNeutrinoPhi", "HitFit neutrino azimutal angle", 50, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "hitFitNeutrinoPhi" ]->SetXTitle( "#phi^{#nu, HitFit}" );
  histos1D_[ "hitFitNeutrinoPhi" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNeutrinoPz" ] = fileService->make< TH1D >( "hitFitNeutrinoPz", "HitFit neutrino longitudinal momentum", 50, -1000., 1000. );
  histos1D_[ "hitFitNeutrinoPz" ]->SetXTitle( "p_{z}^{#nu, HitFit} (GeV)" );
  histos1D_[ "hitFitNeutrinoPz" ]->SetYTitle( "events" );
  // 2-dim
  histos2D_[ "hitFitMT_hitFitSigMT" ] = fileService->make< TH2D >( "hitFitMT_hitFitSigMT", "HitFit fit top mass uncertainty vs. HitFit fit top mass", 50, 100., 300., 25, 0., 50. );
  histos2D_[ "hitFitMT_hitFitSigMT" ]->SetXTitle( "m^{t} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMT" ]->SetYTitle( "#sigma_{m^{t}} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMT" ]->SetZTitle( "events" );
  histos2D_[ "hitFitMT_hitFitSigMTRel" ] = fileService->make< TH2D >( "hitFitMT_hitFitSigMTRel", "Relative HitFit fit top mass uncertainty vs. HitFit fit top mass", 50, 100., 300., 25, 0., 0.25 );
  histos2D_[ "hitFitMT_hitFitSigMTRel" ]->SetXTitle( "m^{t} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMTRel" ]->SetYTitle( "#frac{#sigma_{m^{t}}}{m^{t}}" );
  histos2D_[ "hitFitMT_hitFitSigMTRel" ]->SetZTitle( "events" );

  // MC specific (using true information)
  // 1-dim
  histos1D_[ "genSemiLepChn" ] = fileService->make< TH1D >( "genSemiLepChn", "Semi-leptonic decay channel", 8, -4.5, 3.5 );
  histos1D_[ "genSemiLepChn" ]->SetXTitle( "decay channel" );
  histos1D_[ "genSemiLepChn" ]->GetXaxis()->SetBinLabel( 1, "other fully lep." );
  histos1D_[ "genSemiLepChn" ]->GetXaxis()->SetBinLabel( 2, "#mu + #tau" );
  histos1D_[ "genSemiLepChn" ]->GetXaxis()->SetBinLabel( 3, "#mu + #mu" );
  histos1D_[ "genSemiLepChn" ]->GetXaxis()->SetBinLabel( 4, "#mu + e" );
  histos1D_[ "genSemiLepChn" ]->GetXaxis()->SetBinLabel( 5, "fully had." );
  histos1D_[ "genSemiLepChn" ]->GetXaxis()->SetBinLabel( 6, "e + had." );
  histos1D_[ "genSemiLepChn" ]->GetXaxis()->SetBinLabel( 7, "#it{#mu + had.}" );
  histos1D_[ "genSemiLepChn" ]->GetXaxis()->SetBinLabel( 8, "#tau + had." );
  histos1D_[ "genSemiLepChn" ]->SetYTitle( "events" );
  histos1D_[ "genTopLepMass" ] = fileService->make< TH1D >( "genTopLepMass", "Generated leptonic top mass", 50, 170., 180. );
  histos1D_[ "genTopLepMass" ]->SetXTitle( "m^{t_{l}, gen.} (GeV)" );
  histos1D_[ "genTopLepMass" ]->SetYTitle( "events" );
  histos1D_[ "genTopHadMass" ] = fileService->make< TH1D >( "genTopHadMass", "Generated hadronic top mass", 50, 170., 180. );
  histos1D_[ "genTopHadMass" ]->SetXTitle( "m^{t_{h}, gen.} (GeV)" );
  histos1D_[ "genTopHadMass" ]->SetYTitle( "events" );
  histos1D_[ "genNeutrinoMass" ] = fileService->make< TH1D >( "genNeutrinoMass", "Generated neutrino mass", 10, -0.00001, 0.00001 );
  histos1D_[ "genNeutrinoMass" ]->SetXTitle( "m^{#nu, gen.} (GeV)" );
  histos1D_[ "genNeutrinoMass" ]->SetYTitle( "events" );
  histos1D_[ "genLeptonPt" ] = fileService->make< TH1D >( "genLeptonPt", "Generated lepton transverse momentum", 50, 0., 500. );
  histos1D_[ "genLeptonPt" ]->SetXTitle( "p_{t}^{l, gen.} (GeV)" );
  histos1D_[ "genLeptonPt" ]->SetYTitle( "events" );
//   histos1D_[ "hitFitLeptonPtDiff" ] = fileService->make< TH1D >( "hitFitLeptonPtDiff", "HitFit lepton transverse momentum difference", 50, -50., 50. );
//   histos1D_[ "hitFitLeptonPtDiff" ]->SetXTitle( "p_{t}^{l, HitFit} - p_{t}^{l, HitFit} (GeV)" );
//   histos1D_[ "hitFitLeptonPtDiff" ]->SetYTitle( "events" );
//   histos1D_[ "hitFitLeptonPtDiffRel" ] = fileService->make< TH1D >( "hitFitLeptonPtDiffRel", "HitFit lepton transverse momentum rel. difference", 50, -0.5, 0.5 );
//   histos1D_[ "hitFitLeptonPtDiffRel" ]->SetXTitle( "#frac{(p_{t}^{l, HitFit} - p_{t}^{l, gen.})}{p_{t}^{l, gen.}}" );
//   histos1D_[ "hitFitLeptonPtDiffRel" ]->SetYTitle( "events" );
  histos1D_[ "genLeptonEta" ] = fileService->make< TH1D >( "genLeptonEta", "Generated lepton pseudo-rapidity", 50, -4., 4. );
  histos1D_[ "genLeptonEta" ]->SetXTitle( "#eta^{l, gen.}" );
  histos1D_[ "genLeptonEta" ]->SetYTitle( "events" );
  histos1D_[ "genLeptonPhi" ] = fileService->make< TH1D >( "genLeptonPhi", "Generated lepton azimutal angle", 50, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "genLeptonPhi" ]->SetXTitle( "#phi^{l, gen.}" );
  histos1D_[ "genLeptonPhi" ]->SetYTitle( "events" );
  histos1D_[ "genLeptonPz" ] = fileService->make< TH1D >( "genLeptonPz", "Generated lepton longitudinal momentum", 50, -1000., 1000. );
  histos1D_[ "genLeptonPz" ]->SetXTitle( "p_{z}^{l, gen.} (GeV)" );
  histos1D_[ "genLeptonPz" ]->SetYTitle( "events" );
  histos1D_[ "genNeutrinoPt" ] = fileService->make< TH1D >( "genNeutrinoPt", "Generated neutrino transverse momentum", 50, 0., 500. );
  histos1D_[ "genNeutrinoPt" ]->SetXTitle( "p_{t}^{#nu, gen.} (GeV)" );
  histos1D_[ "genNeutrinoPt" ]->SetYTitle( "events" );
  histos1D_[ "genNeutrinoEta" ] = fileService->make< TH1D >( "genNeutrinoEta", "Generated neutrino pseudo-rapidity", 50, -4., 4. );
  histos1D_[ "genNeutrinoEta" ]->SetXTitle( "#eta^{#nu, gen.}" );
  histos1D_[ "genNeutrinoEta" ]->SetYTitle( "events" );
  histos1D_[ "genNeutrinoPhi" ] = fileService->make< TH1D >( "genNeutrinoPhi", "Generated neutrino azimutal angle", 50, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "genNeutrinoPhi" ]->SetXTitle( "#phi^{#nu, gen.}" );
  histos1D_[ "genNeutrinoPhi" ]->SetYTitle( "events" );
  histos1D_[ "genNeutrinoPz" ] = fileService->make< TH1D >( "genNeutrinoPz", "Generated neutrino longitudinal momentum", 50, -1000., 1000. );
  histos1D_[ "genNeutrinoPz" ]->SetXTitle( "p_{z}^{#nu, gen.} (GeV)" );
  histos1D_[ "genNeutrinoPz" ]->SetYTitle( "events" );
  histos1D_[ "genMuonTopLepMass" ] = fileService->make< TH1D >( "genMuonTopLepMass", "Generated leptonic top mass", 50, 170., 180. );
  histos1D_[ "genMuonTopLepMass" ]->SetXTitle( "m^{t_{l}, gen.} (GeV)" );
  histos1D_[ "genMuonTopLepMass" ]->SetYTitle( "events" );
  histos1D_[ "genMuonTopHadMass" ] = fileService->make< TH1D >( "genMuonTopHadMass", "Generated hadronic top mass", 50, 170., 180. );
  histos1D_[ "genMuonTopHadMass" ]->SetXTitle( "m^{t_{h}, gen.} (GeV)" );
  histos1D_[ "genMuonTopHadMass" ]->SetYTitle( "events" );
  histos1D_[ "genMuonNeutrinoMass" ] = fileService->make< TH1D >( "genMuonNeutrinoMass", "Generated muon neutrino mass", 10, -0.00001, 0.00001 );
  histos1D_[ "genMuonNeutrinoMass" ]->SetXTitle( "m^{#nu, gen.} (GeV)" );
  histos1D_[ "genMuonNeutrinoMass" ]->SetYTitle( "events" );
  histos1D_[ "genMuonPt" ] = fileService->make< TH1D >( "genMuonPt", "Generated muon transverse momentum", 50, 0., 500. );
  histos1D_[ "genMuonPt" ]->SetXTitle( "p_{t}^{#mu, gen.} (GeV)" );
  histos1D_[ "genMuonPt" ]->SetYTitle( "events" );
  histos1D_[ "genMuonEta" ] = fileService->make< TH1D >( "genMuonEta", "Generated muon pseudo-rapidity", 50, -4., 4. );
  histos1D_[ "genMuonEta" ]->SetXTitle( "#eta^{#mu, gen.}" );
  histos1D_[ "genMuonEta" ]->SetYTitle( "events" );
  histos1D_[ "genMuonPhi" ] = fileService->make< TH1D >( "genMuonPhi", "Generated muon azimutal angle", 50, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "genMuonPhi" ]->SetXTitle( "#phi^{#mu, gen.}" );
  histos1D_[ "genMuonPhi" ]->SetYTitle( "events" );
  histos1D_[ "genMuonPz" ] = fileService->make< TH1D >( "genMuonPz", "Generated muon longitudinal momentum", 50, -1000., 1000. );
  histos1D_[ "genMuonPz" ]->SetXTitle( "p_{z}^{#mu, gen.} (GeV)" );
  histos1D_[ "genMuonPz" ]->SetYTitle( "events" );
  histos1D_[ "genMuonNeutrinoPt" ] = fileService->make< TH1D >( "genMuonNeutrinoPt", "Generated muon neutrino transverse momentum", 50, 0., 500. );
  histos1D_[ "genMuonNeutrinoPt" ]->SetXTitle( "p_{t}^{#nu, gen.} (GeV)" );
  histos1D_[ "genMuonNeutrinoPt" ]->SetYTitle( "events" );
  histos1D_[ "genMuonNeutrinoEta" ] = fileService->make< TH1D >( "genMuonNeutrinoEta", "Generated muon neutrino pseudo-rapidity", 50, -4., 4. );
  histos1D_[ "genMuonNeutrinoEta" ]->SetXTitle( "#eta^{#nu, gen.}" );
  histos1D_[ "genMuonNeutrinoEta" ]->SetYTitle( "events" );
  histos1D_[ "genMuonNeutrinoPhi" ] = fileService->make< TH1D >( "genMuonNeutrinoPhi", "Generated muon neutrino azimutal angle", 50, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "genMuonNeutrinoPhi" ]->SetXTitle( "#phi^{#nu, gen.}" );
  histos1D_[ "genMuonNeutrinoPhi" ]->SetYTitle( "events" );
  histos1D_[ "genMuonNeutrinoPz" ] = fileService->make< TH1D >( "genMuonNeutrinoPz", "Generated muon neutrino longitudinal momentum", 50, -1000., 1000. );
  histos1D_[ "genMuonNeutrinoPz" ]->SetXTitle( "p_{z}^{#nu, gen.} (GeV)" );
  histos1D_[ "genMuonNeutrinoPz" ]->SetYTitle( "events" );
  histos1D_[ "hitFitProbSemiMu" ] = fileService->make< TH1D >( "hitFitProbSemiMu", "HitFit fit probabilty, true semi-muonic", 25, 0., 1. );
  histos1D_[ "hitFitProbSemiMu" ]->SetXTitle( "probability" );
  histos1D_[ "hitFitProbSemiMu" ]->SetYTitle( "events" );
  histos1D_[ "hitFitProbFullMu" ] = fileService->make< TH1D >( "hitFitProbFullMu", "HitFit fit probabilty, true full-muonic", 25, 0., 1. );
  histos1D_[ "hitFitProbFullMu" ]->SetXTitle( "probability" );
  histos1D_[ "hitFitProbFullMu" ]->SetYTitle( "events" );
  histos1D_[ "hitFitProbNoMu" ] = fileService->make< TH1D >( "hitFitProbNoMu", "HitFit fit probabilty, true non-muonic", 25, 0., 1. );
  histos1D_[ "hitFitProbNoMu" ]->SetXTitle( "probability" );
  histos1D_[ "hitFitProbNoMu" ]->SetYTitle( "events" );
  histos1D_[ "hitFitMTSemiMu" ] = fileService->make< TH1D >( "hitFitMTSemiMu", "HitFit top mass, true semi-muonic", 50, 100., 300. );
  histos1D_[ "hitFitMTSemiMu" ]->SetXTitle( "m^{t} (GeV)" );
  histos1D_[ "hitFitMTSemiMu" ]->SetYTitle( "events" );
  histos1D_[ "hitFitMTFullMu" ] = fileService->make< TH1D >( "hitFitMTFullMu", "HitFit top mass, true full-muonic", 50, 100., 300. );
  histos1D_[ "hitFitMTFullMu" ]->SetXTitle( "m^{t} (GeV)" );
  histos1D_[ "hitFitMTFullMu" ]->SetYTitle( "events" );
  histos1D_[ "hitFitMTNoMu" ] = fileService->make< TH1D >( "hitFitMTNoMu", "HitFit top mass, true non-muonic", 50, 100., 300. );
  histos1D_[ "hitFitMTNoMu" ]->SetXTitle( "m^{t} (GeV)" );
  histos1D_[ "hitFitMTNoMu" ]->SetYTitle( "events" );
  histos1D_[ "hitFitSigMTSemiMu" ] = fileService->make< TH1D >( "hitFitSigMTSemiMu", "HitFit top mass uncertainty, true semi-muonic", 25, 0., 50. );
  histos1D_[ "hitFitSigMTSemiMu" ]->SetXTitle( "#sigma_{m^{t}} (GeV)" );
  histos1D_[ "hitFitSigMTSemiMu" ]->SetYTitle( "events" );
  histos1D_[ "hitFitSigMTFullMu" ] = fileService->make< TH1D >( "hitFitSigMTFullMu", "HitFit top mass uncertainty, true full-muonic", 25, 0., 50. );
  histos1D_[ "hitFitSigMTFullMu" ]->SetXTitle( "#sigma_{m^{t}} (GeV)" );
  histos1D_[ "hitFitSigMTFullMu" ]->SetYTitle( "events" );
  histos1D_[ "hitFitSigMTNoMu" ] = fileService->make< TH1D >( "hitFitSigMTNoMu", "HitFit top mass uncertainty, true non-muonic", 25, 0., 50. );
  histos1D_[ "hitFitSigMTNoMu" ]->SetXTitle( "#sigma_{m^{t}} (GeV)" );
  histos1D_[ "hitFitSigMTNoMu" ]->SetYTitle( "events" );
  // 2-dim
  histos2D_[ "genSemiLepChn_hitFitProb" ] = fileService->make< TH2D >( "genSemiLepChn_hitFitProb", "HitFit fit probability vs. semi-leptonic decay channel", 8, -4.5, 3.5, 25, 0., 1. );
  histos2D_[ "genSemiLepChn_hitFitProb" ]->SetXTitle( "decay channel" );
  histos2D_[ "genSemiLepChn_hitFitProb" ]->GetXaxis()->SetBinLabel( 1, "other fully lep." );
  histos2D_[ "genSemiLepChn_hitFitProb" ]->GetXaxis()->SetBinLabel( 2, "#mu + #tau" );
  histos2D_[ "genSemiLepChn_hitFitProb" ]->GetXaxis()->SetBinLabel( 3, "#mu + #mu" );
  histos2D_[ "genSemiLepChn_hitFitProb" ]->GetXaxis()->SetBinLabel( 4, "#mu + e" );
  histos2D_[ "genSemiLepChn_hitFitProb" ]->GetXaxis()->SetBinLabel( 5, "fully had." );
  histos2D_[ "genSemiLepChn_hitFitProb" ]->GetXaxis()->SetBinLabel( 6, "e + had." );
  histos2D_[ "genSemiLepChn_hitFitProb" ]->GetXaxis()->SetBinLabel( 7, "#it{#mu + had.}" );
  histos2D_[ "genSemiLepChn_hitFitProb" ]->GetXaxis()->SetBinLabel( 8, "#tau + had." );
  histos2D_[ "genSemiLepChn_hitFitProb" ]->SetYTitle( "probability" );
  histos2D_[ "genSemiLepChn_hitFitProb" ]->SetZTitle( "events" );
  histos2D_[ "genSemiLepChn_hitFitProbLow" ] = fileService->make< TH2D >( "genSemiLepChn_hitFitProbLow", "HitFit fit probability vs. semi-leptonic decay channel", 8, -4.5, 3.5, 25, 0., 0.05 );
  histos2D_[ "genSemiLepChn_hitFitProbLow" ]->SetXTitle( "decay channel" );
  histos2D_[ "genSemiLepChn_hitFitProbLow" ]->GetXaxis()->SetBinLabel( 1, "other fully lep." );
  histos2D_[ "genSemiLepChn_hitFitProbLow" ]->GetXaxis()->SetBinLabel( 2, "#mu + #tau" );
  histos2D_[ "genSemiLepChn_hitFitProbLow" ]->GetXaxis()->SetBinLabel( 3, "#mu + #mu" );
  histos2D_[ "genSemiLepChn_hitFitProbLow" ]->GetXaxis()->SetBinLabel( 4, "#mu + e" );
  histos2D_[ "genSemiLepChn_hitFitProbLow" ]->GetXaxis()->SetBinLabel( 5, "fully had." );
  histos2D_[ "genSemiLepChn_hitFitProbLow" ]->GetXaxis()->SetBinLabel( 6, "e + had." );
  histos2D_[ "genSemiLepChn_hitFitProbLow" ]->GetXaxis()->SetBinLabel( 7, "#it{#mu + had.}" );
  histos2D_[ "genSemiLepChn_hitFitProbLow" ]->GetXaxis()->SetBinLabel( 8, "#tau + had." );
  histos2D_[ "genSemiLepChn_hitFitProbLow" ]->SetYTitle( "probability" );
  histos2D_[ "genSemiLepChn_hitFitProbLow" ]->SetZTitle( "events" );
  histos2D_[ "genSemiLepChn_hitFitChi2" ] = fileService->make< TH2D >( "genSemiLepChn_hitFitChi2", "HitFit fit #chi^{2} vs. semi-leptonic decay channel", 8, -4.5, 3.5, 25, 0., 100. );
  histos2D_[ "genSemiLepChn_hitFitChi2" ]->SetXTitle( "decay channel" );
  histos2D_[ "genSemiLepChn_hitFitChi2" ]->GetXaxis()->SetBinLabel( 1, "other fully lep." );
  histos2D_[ "genSemiLepChn_hitFitChi2" ]->GetXaxis()->SetBinLabel( 2, "#mu + #tau" );
  histos2D_[ "genSemiLepChn_hitFitChi2" ]->GetXaxis()->SetBinLabel( 3, "#mu + #mu" );
  histos2D_[ "genSemiLepChn_hitFitChi2" ]->GetXaxis()->SetBinLabel( 4, "#mu + e" );
  histos2D_[ "genSemiLepChn_hitFitChi2" ]->GetXaxis()->SetBinLabel( 5, "fully had." );
  histos2D_[ "genSemiLepChn_hitFitChi2" ]->GetXaxis()->SetBinLabel( 6, "e + had." );
  histos2D_[ "genSemiLepChn_hitFitChi2" ]->GetXaxis()->SetBinLabel( 7, "#it{#mu + had.}" );
  histos2D_[ "genSemiLepChn_hitFitChi2" ]->GetXaxis()->SetBinLabel( 8, "#tau + had." );
  histos2D_[ "genSemiLepChn_hitFitChi2" ]->SetYTitle( "#chi^{2}" );
  histos2D_[ "genSemiLepChn_hitFitChi2" ]->SetZTitle( "events" );
  histos2D_[ "genSemiLepChn_hitFitMT" ] = fileService->make< TH2D >( "genSemiLepChn_hitFitMT", "HitFit fit top mass vs. semi-leptonic decay channel", 8, -4.5, 3.5, 50, 100., 300. );
  histos2D_[ "genSemiLepChn_hitFitMT" ]->SetXTitle( "decay channel" );
  histos2D_[ "genSemiLepChn_hitFitMT" ]->GetXaxis()->SetBinLabel( 1, "other fully lep." );
  histos2D_[ "genSemiLepChn_hitFitMT" ]->GetXaxis()->SetBinLabel( 2, "#mu + #tau" );
  histos2D_[ "genSemiLepChn_hitFitMT" ]->GetXaxis()->SetBinLabel( 3, "#mu + #mu" );
  histos2D_[ "genSemiLepChn_hitFitMT" ]->GetXaxis()->SetBinLabel( 4, "#mu + e" );
  histos2D_[ "genSemiLepChn_hitFitMT" ]->GetXaxis()->SetBinLabel( 5, "fully had." );
  histos2D_[ "genSemiLepChn_hitFitMT" ]->GetXaxis()->SetBinLabel( 6, "e + had." );
  histos2D_[ "genSemiLepChn_hitFitMT" ]->GetXaxis()->SetBinLabel( 7, "#it{#mu + had.}" );
  histos2D_[ "genSemiLepChn_hitFitMT" ]->GetXaxis()->SetBinLabel( 8, "#tau + had." );
  histos2D_[ "genSemiLepChn_hitFitMT" ]->SetYTitle( "m^{t} (GeV)" );
  histos2D_[ "genSemiLepChn_hitFitMT" ]->SetZTitle( "events" );
  histos2D_[ "genSemiLepChn_hitFitSigMT" ] = fileService->make< TH2D >( "genSemiLepChn_hitFitSigMT", "HitFit fit top mass uncertainty vs. semi-leptonic decay channel", 8, -4.5, 3.5, 25, 0., 50. );
  histos2D_[ "genSemiLepChn_hitFitSigMT" ]->SetXTitle( "decay channel" );
  histos2D_[ "genSemiLepChn_hitFitSigMT" ]->GetXaxis()->SetBinLabel( 1, "other fully lep." );
  histos2D_[ "genSemiLepChn_hitFitSigMT" ]->GetXaxis()->SetBinLabel( 2, "#mu + #tau" );
  histos2D_[ "genSemiLepChn_hitFitSigMT" ]->GetXaxis()->SetBinLabel( 3, "#mu + #mu" );
  histos2D_[ "genSemiLepChn_hitFitSigMT" ]->GetXaxis()->SetBinLabel( 4, "#mu + e" );
  histos2D_[ "genSemiLepChn_hitFitSigMT" ]->GetXaxis()->SetBinLabel( 5, "fully had." );
  histos2D_[ "genSemiLepChn_hitFitSigMT" ]->GetXaxis()->SetBinLabel( 6, "e + had." );
  histos2D_[ "genSemiLepChn_hitFitSigMT" ]->GetXaxis()->SetBinLabel( 7, "#it{#mu + had.}" );
  histos2D_[ "genSemiLepChn_hitFitSigMT" ]->GetXaxis()->SetBinLabel( 8, "#tau + had." );
  histos2D_[ "genSemiLepChn_hitFitSigMT" ]->SetYTitle( "#sigma_{m^{t}} (GeV)" );
  histos2D_[ "genSemiLepChn_hitFitSigMT" ]->SetZTitle( "events" );
  histos2D_[ "genSemiLepChn_hitFitSigMTRel" ] = fileService->make< TH2D >( "genSemiLepChn_hitFitSigMTRel", "Relative HitFit fit top mass uncertainty vs. semi-leptonic decay channel", 8, -4.5, 3.5, 25, 0., 0.25 );
  histos2D_[ "genSemiLepChn_hitFitSigMTRel" ]->SetXTitle( "decay channel" );
  histos2D_[ "genSemiLepChn_hitFitSigMTRel" ]->GetXaxis()->SetBinLabel( 1, "other fully lep." );
  histos2D_[ "genSemiLepChn_hitFitSigMTRel" ]->GetXaxis()->SetBinLabel( 2, "#mu + #tau" );
  histos2D_[ "genSemiLepChn_hitFitSigMTRel" ]->GetXaxis()->SetBinLabel( 3, "#mu + #mu" );
  histos2D_[ "genSemiLepChn_hitFitSigMTRel" ]->GetXaxis()->SetBinLabel( 4, "#mu + e" );
  histos2D_[ "genSemiLepChn_hitFitSigMTRel" ]->GetXaxis()->SetBinLabel( 5, "fully had." );
  histos2D_[ "genSemiLepChn_hitFitSigMTRel" ]->GetXaxis()->SetBinLabel( 6, "e + had." );
  histos2D_[ "genSemiLepChn_hitFitSigMTRel" ]->GetXaxis()->SetBinLabel( 7, "#it{#mu + had.}" );
  histos2D_[ "genSemiLepChn_hitFitSigMTRel" ]->GetXaxis()->SetBinLabel( 8, "#tau + had." );
  histos2D_[ "genSemiLepChn_hitFitSigMTRel" ]->SetYTitle( "#frac{#sigma_{m^{t}}}{m^{t}}" );
  histos2D_[ "genSemiLepChn_hitFitSigMTRel" ]->SetZTitle( "events" );
//   histos2D_[ "genLeptonPt_hitFitLeptonPt" ] = fileService->make< TH2D >( "genLeptonPt_hitFitLeptonPt", "HitFit lepton transverse momentum vs. generated lepton transverse momentum", 50, 0., 500., 50, 0., 500. );
//   histos2D_[ "genLeptonPt_hitFitLeptonPt" ]->SetXTitle( "p_{t}^{l, gen.} (GeV)" );
//   histos2D_[ "genLeptonPt_hitFitLeptonPt" ]->SetYTitle( "p_{t}^{l, HitFit} (GeV)" );
//   histos2D_[ "genLeptonPt_hitFitLeptonPt" ]->SetZTitle( "events" );

}


// Event loop
void TopHitFitAnalyzer::analyze( const edm::Event & iEvent, const edm::EventSetup & iSetup )
{

  // TQAF semi-leptonic event
  edm::Handle< TtSemiLeptonicEvent > ttSemiLeptonicEvent;
  iEvent.getByLabel( ttSemiLeptonicEvent_, ttSemiLeptonicEvent );

  // HitFit event hypothesis
  const reco::CompositeCandidate hypoHitFit( ttSemiLeptonicEvent->eventHypo( TtEvent::kHitFit ) );
  const reco::Candidate * hitFitTopLep( ttSemiLeptonicEvent->leptonicDecayTop( TtEvent::kHitFit ) );
  const reco::Candidate * hitFitTopHad( ttSemiLeptonicEvent->hadronicDecayTop( TtEvent::kHitFit ) );
  const reco::Candidate * hitFitLepton( ttSemiLeptonicEvent->singleLepton( TtEvent::kHitFit ) );
  const reco::Candidate * hitFitNeutrino( ttSemiLeptonicEvent->singleNeutrino( TtEvent::kHitFit ) );

  histos1D_[ "hitFitProb" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
  histos1D_[ "hitFitChi2" ]->Fill( ttSemiLeptonicEvent->hitFitChi2() );
  histos1D_[ "hitFitMT" ]->Fill( ttSemiLeptonicEvent->hitFitMT() );
  histos1D_[ "hitFitSigMT" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT() );
  histos2D_[ "hitFitMT_hitFitSigMT" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT() );
  histos2D_[ "hitFitMT_hitFitSigMTRel" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );

  histos1D_[ "hitFitTopLepMass" ]->Fill( hitFitTopLep->mass() );
  histos1D_[ "hitFitTopHadMass" ]->Fill( hitFitTopHad->mass() );
  histos1D_[ "hitFitLeptonPt" ]->Fill( hitFitLepton->pt() );
  histos1D_[ "hitFitLeptonEta" ]->Fill( hitFitLepton->eta() );
  histos1D_[ "hitFitLeptonPhi" ]->Fill( hitFitLepton->phi() );
  histos1D_[ "hitFitLeptonPz" ]->Fill( hitFitLepton->pz() );
  histos1D_[ "hitFitNeutrinoMass" ]->Fill( hitFitNeutrino->mass() );
  histos1D_[ "hitFitNeutrinoPt" ]->Fill( hitFitNeutrino->pt() );
  histos1D_[ "hitFitNeutrinoEta" ]->Fill( hitFitNeutrino->eta() );
  histos1D_[ "hitFitNeutrinoPhi" ]->Fill( hitFitNeutrino->phi() );
  histos1D_[ "hitFitNeutrinoPz" ]->Fill( hitFitNeutrino->pz() );

  // MC specific (using true information)
  if ( ! iEvent.isRealData() ) {

    const TtGenEvent * ttGenEvent( ttSemiLeptonicEvent->genEvent().get() );

    // Decay channels
    const bool trueSemiMu( ttGenEvent->isSemiLeptonic( WDecay::kMuon ) );
    const bool trueFullMu(  ttGenEvent->isFullLeptonic( WDecay::kMuon, WDecay::kElec )
                         || ttGenEvent->isFullLeptonic( WDecay::kMuon, WDecay::kMuon )
                         || ttGenEvent->isFullLeptonic( WDecay::kMuon, WDecay::kTau )
                         );
    int genSemiLepChn( ttGenEvent->semiLeptonicChannel() );
    if ( trueFullMu ) {
      if ( ttGenEvent->isFullLeptonic( WDecay::kMuon, WDecay::kElec ) )      genSemiLepChn = -WDecay::kElec;
      else if ( ttGenEvent->isFullLeptonic( WDecay::kMuon, WDecay::kMuon ) ) genSemiLepChn = -WDecay::kMuon;
      else if ( ttGenEvent->isFullLeptonic( WDecay::kMuon, WDecay::kTau ) )  genSemiLepChn = -WDecay::kTau;
    } else if ( ttGenEvent->isFullLeptonic() ) {
      genSemiLepChn = -4;
    }

    histos1D_[ "genSemiLepChn" ]->Fill( genSemiLepChn );
    histos2D_[ "genSemiLepChn_hitFitProb" ]->Fill( genSemiLepChn, ttSemiLeptonicEvent->hitFitProb() );
    histos2D_[ "genSemiLepChn_hitFitProbLow" ]->Fill( genSemiLepChn, ttSemiLeptonicEvent->hitFitProb() );
    histos2D_[ "genSemiLepChn_hitFitChi2" ]->Fill( genSemiLepChn, ttSemiLeptonicEvent->hitFitChi2() );
    histos2D_[ "genSemiLepChn_hitFitMT" ]->Fill( genSemiLepChn, ttSemiLeptonicEvent->hitFitMT() );
    histos2D_[ "genSemiLepChn_hitFitSigMT" ]->Fill( genSemiLepChn, ttSemiLeptonicEvent->hitFitSigMT() );
    histos2D_[ "genSemiLepChn_hitFitSigMTRel" ]->Fill( genSemiLepChn, ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );

    if ( ttGenEvent->isSemiLeptonic() ) {
      // Generator particles
      const reco::GenParticle * genTopLep( ttGenEvent->leptonicDecayTop() );
      const reco::GenParticle * genTopHad( ttGenEvent->hadronicDecayTop() );
      const reco::GenParticle * genLepton( ttGenEvent->singleLepton() );
      const reco::GenParticle * genNeutrino( ttGenEvent->singleNeutrino() );
      histos1D_[ "genTopLepMass" ]->Fill( genTopLep->mass() );
      histos1D_[ "genTopHadMass" ]->Fill( genTopHad->mass() );
      histos1D_[ "genNeutrinoMass" ]->Fill( genNeutrino->mass() );
      histos1D_[ "genLeptonPt" ]->Fill( genLepton->pt() );
      histos1D_[ "genLeptonEta" ]->Fill( genLepton->eta() );
      histos1D_[ "genLeptonPhi" ]->Fill( genLepton->phi() );
      histos1D_[ "genLeptonPz" ]->Fill( genLepton->pz() );
      histos1D_[ "genNeutrinoPt" ]->Fill( genNeutrino->pt() );
      histos1D_[ "genNeutrinoEta" ]->Fill( genNeutrino->eta() );
      histos1D_[ "genNeutrinoPhi" ]->Fill( genNeutrino->phi() );
      histos1D_[ "genNeutrinoPz" ]->Fill( genNeutrino->pz() );
      if ( trueSemiMu ) {
        histos1D_[ "genMuonTopLepMass" ]->Fill( genTopLep->mass() );
        histos1D_[ "genMuonTopHadMass" ]->Fill( genTopHad->mass() );
        histos1D_[ "genMuonNeutrinoMass" ]->Fill( genNeutrino->mass() );
        histos1D_[ "genMuonPt" ]->Fill( genLepton->pt() );
        histos1D_[ "genMuonEta" ]->Fill( genLepton->eta() );
        histos1D_[ "genMuonPhi" ]->Fill( genLepton->phi() );
        histos1D_[ "genMuonPz" ]->Fill( genLepton->pz() );
        histos1D_[ "genMuonNeutrinoPt" ]->Fill( genNeutrino->pt() );
        histos1D_[ "genMuonNeutrinoEta" ]->Fill( genNeutrino->eta() );
        histos1D_[ "genMuonNeutrinoPhi" ]->Fill( genNeutrino->phi() );
        histos1D_[ "genMuonNeutrinoPz" ]->Fill( genNeutrino->pz() );
//         histos1D_[ "hitFitLeptonPtDiff" ]->Fill( hitFitLepton->pt() - genLepton->pt() );
//         histos1D_[ "hitFitLeptonPtDiffRel" ]->Fill( ( hitFitLepton->pt() - genLepton->pt() ) / genLepton->pt() );
//         histos2D_[ "genLeptonPt_hitFitLeptonPt" ]->Fill( genLepton->pt(), hitFitLepton->pt() );
      }
    }

    if ( trueSemiMu ) {
      histos1D_[ "hitFitProbSemiMu" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
      histos1D_[ "hitFitMTSemiMu" ]->Fill( ttSemiLeptonicEvent->hitFitMT() );
      histos1D_[ "hitFitSigMTSemiMu" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT() );
    } else if ( trueFullMu ) {
      histos1D_[ "hitFitProbFullMu" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
      histos1D_[ "hitFitMTFullMu" ]->Fill( ttSemiLeptonicEvent->hitFitMT() );
      histos1D_[ "hitFitSigMTFullMu" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT() );
    } else {
      histos1D_[ "hitFitProbNoMu" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
      histos1D_[ "hitFitMTNoMu" ]->Fill( ttSemiLeptonicEvent->hitFitMT() );
      histos1D_[ "hitFitSigMTNoMu" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT() );
    }

  }

}


// End job
void TopHitFitAnalyzer::endJob()
{
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( TopHitFitAnalyzer );
