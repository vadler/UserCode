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
#include <iostream>

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
  histos1D_[ "nRealNuSol" ] = fileService->make< TH1D >( "nRealNuSol", "HitFit real #nu solutions", 4, -1.5, 2.5 );
  histos1D_[ "nRealNuSol" ]->SetXTitle( "real #nu sols." );
  histos1D_[ "nRealNuSol" ]->SetYTitle( "events" );
  histos1D_[ "hitFitProb" ] = fileService->make< TH1D >( "hitFitProb", "HitFit probabilty", 25, 0., 1. );
  histos1D_[ "hitFitProb" ]->SetXTitle( "HitFit prob." );
  histos1D_[ "hitFitProb" ]->SetYTitle( "events" );
  histos1D_[ "hitFitProbLow" ] = fileService->make< TH1D >( "hitFitProbLow", "HitFit probabilty", 25, 0., 0.04 );
  histos1D_[ "hitFitProbLow" ]->SetXTitle( "HitFit prob." );
  histos1D_[ "hitFitProbLow" ]->SetYTitle( "events" );
  histos1D_[ "hitFitChi2" ] = fileService->make< TH1D >( "hitFitChi2", "HitFit #chi^{2}", 50, 0., 100. );
  histos1D_[ "hitFitChi2" ]->SetXTitle( "(#chi^{2})^{HitFit}" );
  histos1D_[ "hitFitChi2" ]->SetYTitle( "events" );
  histos1D_[ "hitFitMT" ] = fileService->make< TH1D >( "hitFitMT", "HitFit top mass", 100, 75., 325. );
  histos1D_[ "hitFitMT" ]->SetXTitle( "m_{top}^{HitFit} (GeV)" );
  histos1D_[ "hitFitMT" ]->SetYTitle( "events" );
  histos1D_[ "hitFitSigMT" ] = fileService->make< TH1D >( "hitFitSigMT", "HitFit top mass uncertainty", 25, 0., 50. );
  histos1D_[ "hitFitSigMT" ]->SetXTitle( "#sigma_{m_{top}}^{HitFit} (GeV)" );
  histos1D_[ "hitFitSigMT" ]->SetYTitle( "events" );
  histos1D_[ "hitFitSigMTRel" ] = fileService->make< TH1D >( "hitFitSigMTRel", "Relative HitFit top mass uncertainty", 25, 0., 0.25 );
  histos1D_[ "hitFitSigMTRel" ]->SetXTitle( "#frac{#sigma_{m_{t}}^{HitFit}}{m_{t}^{HitFit}}" );
  histos1D_[ "hitFitSigMTRel" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepPt" ] = fileService->make< TH1D >( "hitFitLepPt", "HitFit lepton transverse momentum", 50, 0., 250. );
  histos1D_[ "hitFitLepPt" ]->SetXTitle( "p_{t, l}^{HitFit} (GeV)" );
  histos1D_[ "hitFitLepPt" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepEta" ] = fileService->make< TH1D >( "hitFitLepEta", "HitFit lepton pseudo-rapidity", 50, -2.5, 2.5 );
  histos1D_[ "hitFitLepEta" ]->SetXTitle( "#eta_{l}^{HitFit}" );
  histos1D_[ "hitFitLepEta" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepPhi" ] = fileService->make< TH1D >( "hitFitLepPhi", "HitFit lepton azimutal angle", 50, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "hitFitLepPhi" ]->SetXTitle( "#phi_{l}^{HitFit}" );
  histos1D_[ "hitFitLepPhi" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepPz" ] = fileService->make< TH1D >( "hitFitLepPz", "HitFit lepton longitudinal momentum", 50, -500., 500. );
  histos1D_[ "hitFitLepPz" ]->SetXTitle( "p_{z, l}^{HitFit} (GeV)" );
  histos1D_[ "hitFitLepPz" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuM" ] = fileService->make< TH1D >( "hitFitNuM", "HitFit neutrino mass", 50, -0.00005, 0.00005 );
  histos1D_[ "hitFitNuM" ]->SetXTitle( "m_{#nu}^{HitFit} (GeV)" );
  histos1D_[ "hitFitNuM" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuPt" ] = fileService->make< TH1D >( "hitFitNuPt", "HitFit neutrino transverse momentum", 50, 0., 250. );
  histos1D_[ "hitFitNuPt" ]->SetXTitle( "p_{t, #nu}^{HitFit} (GeV)" );
  histos1D_[ "hitFitNuPt" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuEta" ] = fileService->make< TH1D >( "hitFitNuEta", "HitFit neutrino pseudo-rapidity", 50, -4., 4. );
  histos1D_[ "hitFitNuEta" ]->SetXTitle( "#eta_{#nu}^{HitFit}" );
  histos1D_[ "hitFitNuEta" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuPhi" ] = fileService->make< TH1D >( "hitFitNuPhi", "HitFit neutrino azimutal angle", 50, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "hitFitNuPhi" ]->SetXTitle( "#phi_{#nu}^{HitFit}" );
  histos1D_[ "hitFitNuPhi" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuPz" ] = fileService->make< TH1D >( "hitFitNuPz", "HitFit neutrino longitudinal momentum", 50, -500., 500. );
  histos1D_[ "hitFitNuPz" ]->SetXTitle( "p_{z, #nu}^{HitFit} (GeV)" );
  histos1D_[ "hitFitNuPz" ]->SetYTitle( "events" );
  // 2-dim
  histos2D_[ "hitFitProb_hitFitMT" ] = fileService->make< TH2D >( "hitFitProb_hitFitMT", "HitFit top mass vs. HitFit probabilty", 25, 0., 1., 100, 75., 325. );
  histos2D_[ "hitFitProb_hitFitMT" ]->SetXTitle( "HitFit prob." );
  histos2D_[ "hitFitProb_hitFitMT" ]->SetYTitle( "m_{top}^{HitFit} (GeV)" );
  histos2D_[ "hitFitProb_hitFitMT" ]->SetZTitle( "events" );
  histos2D_[ "hitFitProb_hitFitSigMT" ] = fileService->make< TH2D >( "hitFitProb_hitFitSigMT", "HitFit top mass uncertainty vs. HitFit probabilty", 25, 0., 1., 25, 0., 50. );
  histos2D_[ "hitFitProb_hitFitSigMT" ]->SetXTitle( "HitFit prob." );
  histos2D_[ "hitFitProb_hitFitSigMT" ]->SetYTitle( "#sigma_{m_{t}}^{HitFit} (GeV)" );
  histos2D_[ "hitFitProb_hitFitSigMT" ]->SetZTitle( "events" );
  histos2D_[ "hitFitProb_hitFitSigMTRel" ] = fileService->make< TH2D >( "hitFitProb_hitFitSigMTRel", "Relative HitFit top mass uncertainty vs. HitFit probabilty", 25, 0., 1., 25, 0., 0.25 );
  histos2D_[ "hitFitProb_hitFitSigMTRel" ]->SetXTitle( "HitFit prob." );
  histos2D_[ "hitFitProb_hitFitSigMTRel" ]->SetYTitle( "#frac{#sigma_{m_{t}}^{HitFit}}{m_{t}^{HitFit}}" );
  histos2D_[ "hitFitProb_hitFitSigMTRel" ]->SetZTitle( "events" );
  histos2D_[ "hitFitMT_hitFitSigMT" ] = fileService->make< TH2D >( "hitFitMT_hitFitSigMT", "HitFit top mass uncertainty vs. HitFit top mass", 100, 75., 325., 25, 0., 50. );
  histos2D_[ "hitFitMT_hitFitSigMT" ]->SetXTitle( "m_{t}^{HitFit} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMT" ]->SetYTitle( "#sigma_{m_{t}}^{HitFit} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMT" ]->SetZTitle( "events" );
  histos2D_[ "hitFitMT_hitFitSigMTRel" ] = fileService->make< TH2D >( "hitFitMT_hitFitSigMTRel", "Relative HitFit top mass uncertainty vs. HitFit top mass", 50, 100., 300., 25, 0., 0.25 );
  histos2D_[ "hitFitMT_hitFitSigMTRel" ]->SetXTitle( "m_{t}^{HitFit} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMTRel" ]->SetYTitle( "#frac{#sigma_{m_{t}}^{HitFit}}{m_{t}^{HitFit}}" );
  histos2D_[ "hitFitMT_hitFitSigMTRel" ]->SetZTitle( "events" );

  // MC specific (using true information)
  // 1-dim
  histos1D_[ "genDecayChn" ] = fileService->make< TH1D >( "genDecayChn", "Decay channel", 8, -0.5, 7.5 );
  histos1D_[ "genDecayChn" ]->SetXTitle( "decay chn." );
  histos1D_[ "genDecayChn" ]->GetXaxis()->SetBinLabel( 1, "full had." );
  histos1D_[ "genDecayChn" ]->GetXaxis()->SetBinLabel( 2, "semi e" );
  histos1D_[ "genDecayChn" ]->GetXaxis()->SetBinLabel( 3, "#it{semi #mu}" );
  histos1D_[ "genDecayChn" ]->GetXaxis()->SetBinLabel( 4, "semi #tau, to other" );
  histos1D_[ "genDecayChn" ]->GetXaxis()->SetBinLabel( 5, "semi #tau, to #mu" );
  histos1D_[ "genDecayChn" ]->GetXaxis()->SetBinLabel( 6, "full lep. 0 #mu or #tau to #mu" );
  histos1D_[ "genDecayChn" ]->GetXaxis()->SetBinLabel( 7, "full lep. 1 #mu or #tau to #mu" );
  histos1D_[ "genDecayChn" ]->GetXaxis()->SetBinLabel( 8, "full lep. 2 #mu or #tau to #mu" );
  histos1D_[ "genDecayChn" ]->SetYTitle( "events" );
  histos1D_[ "genTopLepM" ] = fileService->make< TH1D >( "genTopLepM", "HitFit muonic top mass", 50, 170., 180. );
  histos1D_[ "genTopLepM" ]->SetXTitle( "m_{t_{l}}^{gen.} (GeV)" );
  histos1D_[ "genTopLepM" ]->SetYTitle( "events" );
  histos1D_[ "genTopHadM" ] = fileService->make< TH1D >( "genTopHadM", "HitFit hadronic top mass", 50, 170., 180. );
  histos1D_[ "genTopHadM" ]->SetXTitle( "m_{t_{h}}^{gen.} (GeV)" );
  histos1D_[ "genTopHadM" ]->SetYTitle( "events" );
  histos1D_[ "genMuPt" ] = fileService->make< TH1D >( "genMuPt", "Generated muon transverse momentum", 50, 0., 500. );
  histos1D_[ "genMuPt" ]->SetXTitle( "p_{t, l}^{gen.} (GeV)" );
  histos1D_[ "genMuPt" ]->SetYTitle( "events" );
  histos1D_[ "genMuEta" ] = fileService->make< TH1D >( "genMuEta", "Generated muon pseudo-rapidity", 50, -4., 4. );
  histos1D_[ "genMuEta" ]->SetXTitle( "#eta_{l}^{gen.}" );
  histos1D_[ "genMuEta" ]->SetYTitle( "events" );
  histos1D_[ "genMuPhi" ] = fileService->make< TH1D >( "genMuPhi", "Generated muon azimutal angle", 50, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "genMuPhi" ]->SetXTitle( "#phi_{l}^{gen.}" );
  histos1D_[ "genMuPhi" ]->SetYTitle( "events" );
  histos1D_[ "genMuPz" ] = fileService->make< TH1D >( "genMuPz", "Generated muon longitudinal momentum", 50, -1000., 1000. );
  histos1D_[ "genMuPz" ]->SetXTitle( "p_{z, l}^{gen.} (GeV)" );
  histos1D_[ "genMuPz" ]->SetYTitle( "events" );
  histos1D_[ "genNuM" ] = fileService->make< TH1D >( "genNuM", "Generated neutrino mass", 10, -0.00001, 0.00001 );
  histos1D_[ "genNuM" ]->SetXTitle( "m_{#nu}^{gen.} (GeV)" );
  histos1D_[ "genNuM" ]->SetYTitle( "events" );
  histos1D_[ "genNuPt" ] = fileService->make< TH1D >( "genNuPt", "Generated neutrino transverse momentum", 50, 0., 500. );
  histos1D_[ "genNuPt" ]->SetXTitle( "p_{t, #nu}^{gen.} (GeV)" );
  histos1D_[ "genNuPt" ]->SetYTitle( "events" );
  histos1D_[ "genNuEta" ] = fileService->make< TH1D >( "genNuEta", "Generated neutrino pseudo-rapidity", 50, -4., 4. );
  histos1D_[ "genNuEta" ]->SetXTitle( "#eta_{#nu}^{gen.}" );
  histos1D_[ "genNuEta" ]->SetYTitle( "events" );
  histos1D_[ "genNuPhi" ] = fileService->make< TH1D >( "genNuPhi", "Generated neutrino azimutal angle", 50, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "genNuPhi" ]->SetXTitle( "#phi_{#nu}^{gen.}" );
  histos1D_[ "genNuPhi" ]->SetYTitle( "events" );
  histos1D_[ "genNuPz" ] = fileService->make< TH1D >( "genNuPz", "Generated neutrino longitudinal momentum", 50, -1000., 1000. );
  histos1D_[ "genNuPz" ]->SetXTitle( "p_{z, #nu}^{gen.} (GeV)" );
  histos1D_[ "genNuPz" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenTopLepM" ] = fileService->make< TH1D >( "hitFitDiffGenTopLepM", "HitFit muonic top mass deviation", 50, -75., 75. );
  histos1D_[ "hitFitDiffGenTopLepM" ]->SetXTitle( "m_{t_{l}}^{gen.} (GeV)" );
  histos1D_[ "hitFitDiffGenTopLepM" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenTopHadM" ] = fileService->make< TH1D >( "hitFitDiffGenTopHadM", "HitFit hadronic top mass deviation", 50, -75., 75. );
  histos1D_[ "hitFitDiffGenTopHadM" ]->SetXTitle( "m_{t_{h}}^{gen.} (GeV)" );
  histos1D_[ "hitFitDiffGenTopHadM" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenMuPt" ] = fileService->make< TH1D >( "hitFitDiffGenMuPt", "HitFit muon transverse momentum deviation", 50, -20., 20. );
  histos1D_[ "hitFitDiffGenMuPt" ]->SetXTitle( "p_{t, l}^{gen.} (GeV)" );
  histos1D_[ "hitFitDiffGenMuPt" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenMuEta" ] = fileService->make< TH1D >( "hitFitDiffGenMuEta", "HitFit muon pseudo-rapidity deviation", 50, -0.2, 0.2 );
  histos1D_[ "hitFitDiffGenMuEta" ]->SetXTitle( "#eta_{l}^{gen.}" );
  histos1D_[ "hitFitDiffGenMuEta" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenMuPhi" ] = fileService->make< TH1D >( "hitFitDiffGenMuPhi", "HitFit muon azimutal angle deviation", 50, -2*TMath::Pi(), 2*TMath::Pi() );
  histos1D_[ "hitFitDiffGenMuPhi" ]->SetXTitle( "#phi_{l}^{gen.}" );
  histos1D_[ "hitFitDiffGenMuPhi" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenMuPz" ] = fileService->make< TH1D >( "hitFitDiffGenMuPz", "HitFit muon longitudinal momentum deviation", 50, -100., 100. );
  histos1D_[ "hitFitDiffGenMuPz" ]->SetXTitle( "p_{z, l}^{gen.} (GeV)" );
  histos1D_[ "hitFitDiffGenMuPz" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenNuM" ] = fileService->make< TH1D >( "hitFitDiffGenNuM", "HitFit neutrino mass deviation", 10, -0.00001, 0.00001 );
  histos1D_[ "hitFitDiffGenNuM" ]->SetXTitle( "m_{#nu}^{gen.} (GeV)" );
  histos1D_[ "hitFitDiffGenNuM" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenNuPt" ] = fileService->make< TH1D >( "hitFitDiffGenNuPt", "HitFit neutrino transverse momentum deviation", 50, -100., 100. );
  histos1D_[ "hitFitDiffGenNuPt" ]->SetXTitle( "p_{t, #nu}^{gen.} (GeV)" );
  histos1D_[ "hitFitDiffGenNuPt" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenNuEta" ] = fileService->make< TH1D >( "hitFitDiffGenNuEta", "HitFit neutrino pseudo-rapidity deviation", 50, -4., 4. );
  histos1D_[ "hitFitDiffGenNuEta" ]->SetXTitle( "#eta_{#nu}^{gen.}" );
  histos1D_[ "hitFitDiffGenNuEta" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenNuPhi" ] = fileService->make< TH1D >( "hitFitDiffGenNuPhi", "HitFit neutrino azimutal angle deviation", 50, -2*TMath::Pi(), 2*TMath::Pi() );
  histos1D_[ "hitFitDiffGenNuPhi" ]->SetXTitle( "#phi_{#nu}^{gen.}" );
  histos1D_[ "hitFitDiffGenNuPhi" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenNuPz" ] = fileService->make< TH1D >( "hitFitDiffGenNuPz", "HitFit neutrino longitudinal momentum deviation", 50, -1000., 1000. );
  histos1D_[ "hitFitDiffGenNuPz" ]->SetXTitle( "p_{z, #nu}^{gen.} (GeV)" );
  histos1D_[ "hitFitDiffGenNuPz" ]->SetYTitle( "events" );
  histos1D_[ "nRealNuSolSig" ] = fileService->make< TH1D >( "nRealNuSolSig", "HitFit real #nu solutions", 4, -1.5, 2.5 );
  histos1D_[ "nRealNuSolSig" ]->SetXTitle( "real #nu sols." );
  histos1D_[ "nRealNuSolSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitProbSig" ] = fileService->make< TH1D >( "hitFitProbSig", "HitFit probabilty", 25, 0., 1. );
  histos1D_[ "hitFitProbSig" ]->SetXTitle( "HitFit prob." );
  histos1D_[ "hitFitProbSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitProbLowSig" ] = fileService->make< TH1D >( "hitFitProbLowSig", "HitFit probabilty", 25, 0., 0.04 );
  histos1D_[ "hitFitProbLowSig" ]->SetXTitle( "HitFit prob." );
  histos1D_[ "hitFitProbLowSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitChi2Sig" ] = fileService->make< TH1D >( "hitFitChi2Sig", "HitFit #chi^{2}", 50, 0., 100. );
  histos1D_[ "hitFitChi2Sig" ]->SetXTitle( "(#chi^{2})^{HitFit}" );
  histos1D_[ "hitFitChi2Sig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitMTSig" ] = fileService->make< TH1D >( "hitFitMTSig", "HitFit top mass", 100, 75., 325. );
  histos1D_[ "hitFitMTSig" ]->SetXTitle( "m_{top}^{HitFit} (GeV)" );
  histos1D_[ "hitFitMTSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitSigMTSig" ] = fileService->make< TH1D >( "hitFitSigMTSig", "HitFit top mass uncertainty", 25, 0., 50. );
  histos1D_[ "hitFitSigMTSig" ]->SetXTitle( "#sigma_{m_{top}}^{HitFit} (GeV)" );
  histos1D_[ "hitFitSigMTSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitSigMTRelSig" ] = fileService->make< TH1D >( "hitFitSigMTRelSig", "Relative HitFit top mass uncertainty", 25, 0., 0.25 );
  histos1D_[ "hitFitSigMTRelSig" ]->SetXTitle( "#sigma_{m_{top}}^{HitFit} (GeV)" );
  histos1D_[ "hitFitSigMTRelSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepPtSig" ] = fileService->make< TH1D >( "hitFitLepPtSig", "HitFit lepton transverse momentum", 50, 0., 250. );
  histos1D_[ "hitFitLepPtSig" ]->SetXTitle( "p_{t, l}^{HitFit} (GeV)" );
  histos1D_[ "hitFitLepPtSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepEtaSig" ] = fileService->make< TH1D >( "hitFitLepEtaSig", "HitFit lepton pseudo-rapidity", 50, -2.5, 2.5 );
  histos1D_[ "hitFitLepEtaSig" ]->SetXTitle( "#eta_{l}^{HitFit}" );
  histos1D_[ "hitFitLepEtaSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepPhiSig" ] = fileService->make< TH1D >( "hitFitLepPhiSig", "HitFit lepton azimutal angle", 50, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "hitFitLepPhiSig" ]->SetXTitle( "#phi_{l}^{HitFit}" );
  histos1D_[ "hitFitLepPhiSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepPzSig" ] = fileService->make< TH1D >( "hitFitLepPzSig", "HitFit lepton longitudinal momentum", 50, -500., 500. );
  histos1D_[ "hitFitLepPzSig" ]->SetXTitle( "p_{z, l}^{HitFit} (GeV)" );
  histos1D_[ "hitFitLepPzSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuMSig" ] = fileService->make< TH1D >( "hitFitNuMSig", "HitFit neutrino mass", 50, -0.00005, 0.00005 );
  histos1D_[ "hitFitNuMSig" ]->SetXTitle( "m_{#nu}^{HitFit} (GeV)" );
  histos1D_[ "hitFitNuMSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuPtSig" ] = fileService->make< TH1D >( "hitFitNuPtSig", "HitFit neutrino transverse momentum", 50, 0., 250. );
  histos1D_[ "hitFitNuPtSig" ]->SetXTitle( "p_{t, #nu}^{HitFit} (GeV)" );
  histos1D_[ "hitFitNuPtSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuEtaSig" ] = fileService->make< TH1D >( "hitFitNuEtaSig", "HitFit neutrino pseudo-rapidity", 50, -4., 4. );
  histos1D_[ "hitFitNuEtaSig" ]->SetXTitle( "#eta_{#nu}^{HitFit}" );
  histos1D_[ "hitFitNuEtaSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuPhiSig" ] = fileService->make< TH1D >( "hitFitNuPhiSig", "HitFit neutrino azimutal angle", 50, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "hitFitNuPhiSig" ]->SetXTitle( "#phi_{#nu}^{HitFit}" );
  histos1D_[ "hitFitNuPhiSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuPzSig" ] = fileService->make< TH1D >( "hitFitNuPzSig", "HitFit neutrino longitudinal momentum", 50, -500., 500. );
  histos1D_[ "hitFitNuPzSig" ]->SetXTitle( "p_{z, #nu}^{HitFit} (GeV)" );
  histos1D_[ "hitFitNuPzSig" ]->SetYTitle( "events" );
  histos1D_[ "nRealNuSolBkg" ] = fileService->make< TH1D >( "nRealNuSolBkg", "HitFit real #nu solutions", 4, -1.5, 2.5 );
  histos1D_[ "nRealNuSolBkg" ]->SetXTitle( "real #nu sols." );
  histos1D_[ "nRealNuSolBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitProbBkg" ] = fileService->make< TH1D >( "hitFitProbBkg", "HitFit probabilty", 25, 0., 1. );
  histos1D_[ "hitFitProbBkg" ]->SetXTitle( "HitFit prob." );
  histos1D_[ "hitFitProbBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitProbLowBkg" ] = fileService->make< TH1D >( "hitFitProbLowBkg", "HitFit probabilty", 25, 0., 0.04 );
  histos1D_[ "hitFitProbLowBkg" ]->SetXTitle( "HitFit prob." );
  histos1D_[ "hitFitProbLowBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitChi2Bkg" ] = fileService->make< TH1D >( "hitFitChi2Bkg", "HitFit #chi^{2}", 50, 0., 100. );
  histos1D_[ "hitFitChi2Bkg" ]->SetXTitle( "(#chi^{2})^{HitFit}" );
  histos1D_[ "hitFitChi2Bkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitMTBkg" ] = fileService->make< TH1D >( "hitFitMTBkg", "HitFit top mass", 100, 75., 325. );
  histos1D_[ "hitFitMTBkg" ]->SetXTitle( "m_{top}^{HitFit} (GeV)" );
  histos1D_[ "hitFitMTBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitSigMTBkg" ] = fileService->make< TH1D >( "hitFitSigMTBkg", "HitFit top mass uncertainty", 25, 0., 50. );
  histos1D_[ "hitFitSigMTBkg" ]->SetXTitle( "#sigma_{m_{top}}^{HitFit} (GeV)" );
  histos1D_[ "hitFitSigMTBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitSigMTRelBkg" ] = fileService->make< TH1D >( "hitFitSigMTRelBkg", "Relative HitFit top mass uncertainty", 25, 0., 0.25 );
  histos1D_[ "hitFitSigMTRelBkg" ]->SetXTitle( "#sigma_{m_{top}}^{HitFit} (GeV)" );
  histos1D_[ "hitFitSigMTRelBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepPtBkg" ] = fileService->make< TH1D >( "hitFitLepPtBkg", "HitFit lepton transverse momentum", 50, 0., 250. );
  histos1D_[ "hitFitLepPtBkg" ]->SetXTitle( "p_{t, l}^{HitFit} (GeV)" );
  histos1D_[ "hitFitLepPtBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepEtaBkg" ] = fileService->make< TH1D >( "hitFitLepEtaBkg", "HitFit lepton pseudo-rapidity", 50, -2.5, 2.5 );
  histos1D_[ "hitFitLepEtaBkg" ]->SetXTitle( "#eta_{l}^{HitFit}" );
  histos1D_[ "hitFitLepEtaBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepPhiBkg" ] = fileService->make< TH1D >( "hitFitLepPhiBkg", "HitFit lepton azimutal angle", 50, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "hitFitLepPhiBkg" ]->SetXTitle( "#phi_{l}^{HitFit}" );
  histos1D_[ "hitFitLepPhiBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepPzBkg" ] = fileService->make< TH1D >( "hitFitLepPzBkg", "HitFit lepton longitudinal momentum", 50, -500., 500. );
  histos1D_[ "hitFitLepPzBkg" ]->SetXTitle( "p_{z, l}^{HitFit} (GeV)" );
  histos1D_[ "hitFitLepPzBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuMBkg" ] = fileService->make< TH1D >( "hitFitNuMBkg", "HitFit neutrino mass", 50, -0.00005, 0.00005 );
  histos1D_[ "hitFitNuMBkg" ]->SetXTitle( "m_{#nu}^{HitFit} (GeV)" );
  histos1D_[ "hitFitNuMBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuPtBkg" ] = fileService->make< TH1D >( "hitFitNuPtBkg", "HitFit neutrino transverse momentum", 50, 0., 250. );
  histos1D_[ "hitFitNuPtBkg" ]->SetXTitle( "p_{t, #nu}^{HitFit} (GeV)" );
  histos1D_[ "hitFitNuPtBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuEtaBkg" ] = fileService->make< TH1D >( "hitFitNuEtaBkg", "HitFit neutrino pseudo-rapidity", 50, -4., 4. );
  histos1D_[ "hitFitNuEtaBkg" ]->SetXTitle( "#eta_{#nu}^{HitFit}" );
  histos1D_[ "hitFitNuEtaBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuPhiBkg" ] = fileService->make< TH1D >( "hitFitNuPhiBkg", "HitFit neutrino azimutal angle", 50, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "hitFitNuPhiBkg" ]->SetXTitle( "#phi_{#nu}^{HitFit}" );
  histos1D_[ "hitFitNuPhiBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuPzBkg" ] = fileService->make< TH1D >( "hitFitNuPzBkg", "HitFit neutrino longitudinal momentum", 50, -500., 500. );
  histos1D_[ "hitFitNuPzBkg" ]->SetXTitle( "p_{z, #nu}^{HitFit} (GeV)" );
  histos1D_[ "hitFitNuPzBkg" ]->SetYTitle( "events" );
  // 2-dim
  histos2D_[ "hitFitMT_hitFitSigMTSig" ] = fileService->make< TH2D >( "hitFitMT_hitFitSigMTSig", "HitFit top mass uncertainty vs. HitFit top mass", 100, 75., 325., 25, 0., 50. );
  histos2D_[ "hitFitMT_hitFitSigMTSig" ]->SetXTitle( "m_{t}^{HitFit} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMTSig" ]->SetYTitle( "#sigma_{m_{t}}^{HitFit} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMTSig" ]->SetZTitle( "events" );
  histos2D_[ "hitFitMT_hitFitSigMTRelSig" ] = fileService->make< TH2D >( "hitFitMT_hitFitSigMTRelSig", "Relative HitFit top mass uncertainty vs. HitFit top mass", 50, 100., 300., 25, 0., 0.25 );
  histos2D_[ "hitFitMT_hitFitSigMTRelSig" ]->SetXTitle( "m_{t}^{HitFit} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMTRelSig" ]->SetYTitle( "#frac{#sigma_{m_{t}}^{HitFit}}{m_{t}^{HitFit}}" );
  histos2D_[ "hitFitMT_hitFitSigMTRelSig" ]->SetZTitle( "events" );
  histos2D_[ "hitFitMT_hitFitSigMTBkg" ] = fileService->make< TH2D >( "hitFitMT_hitFitSigMTBkg", "HitFit top mass uncertainty vs. HitFit top mass", 100, 75., 325., 25, 0., 50. );
  histos2D_[ "hitFitMT_hitFitSigMTBkg" ]->SetXTitle( "m_{t}^{HitFit} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMTBkg" ]->SetYTitle( "#sigma_{m_{t}}^{HitFit} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMTBkg" ]->SetZTitle( "events" );
  histos2D_[ "hitFitMT_hitFitSigMTRelBkg" ] = fileService->make< TH2D >( "hitFitMT_hitFitSigMTRelBkg", "Relative HitFit top mass uncertainty vs. HitFit top mass", 50, 100., 300., 25, 0., 0.25 );
  histos2D_[ "hitFitMT_hitFitSigMTRelBkg" ]->SetXTitle( "m_{t}^{HitFit} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMTRelBkg" ]->SetYTitle( "#frac{#sigma_{m_{t}}^{HitFit}}{m_{t}^{HitFit}}" );
  histos2D_[ "hitFitMT_hitFitSigMTRelBkg" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_nRealNuSol" ] = fileService->make< TH2D >( "genDecayChn_nRealNuSol", "HitFit real #nu solutions vs. decay channel", 8, -0.5, 7.5, 4, -1.5, 2.5 );
  histos2D_[ "genDecayChn_nRealNuSol" ]->SetXTitle( "decay chn." );
  histos2D_[ "genDecayChn_nRealNuSol" ]->GetXaxis()->SetBinLabel( 1, "full had." );
  histos2D_[ "genDecayChn_nRealNuSol" ]->GetXaxis()->SetBinLabel( 2, "semi-e" );
  histos2D_[ "genDecayChn_nRealNuSol" ]->GetXaxis()->SetBinLabel( 3, "#it{semi-#mu}" );
  histos2D_[ "genDecayChn_nRealNuSol" ]->GetXaxis()->SetBinLabel( 4, "semi-#tau, to other" );
  histos2D_[ "genDecayChn_nRealNuSol" ]->GetXaxis()->SetBinLabel( 5, "semi-#tau, to #mu" );
  histos2D_[ "genDecayChn_nRealNuSol" ]->GetXaxis()->SetBinLabel( 6, "full lep. 0 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_nRealNuSol" ]->GetXaxis()->SetBinLabel( 7, "full lep. 1 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_nRealNuSol" ]->GetXaxis()->SetBinLabel( 8, "full lep. 2 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_nRealNuSol" ]->SetYTitle( "real #nu sols." );
  histos2D_[ "genDecayChn_nRealNuSol" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitProb" ] = fileService->make< TH2D >( "genDecayChn_hitFitProb", "HitFit probabilty vs. decay channel", 8, -0.5, 7.5, 25, 0., 1. );
  histos2D_[ "genDecayChn_hitFitProb" ]->SetXTitle( "decay chn." );
  histos2D_[ "genDecayChn_hitFitProb" ]->GetXaxis()->SetBinLabel( 1, "full had." );
  histos2D_[ "genDecayChn_hitFitProb" ]->GetXaxis()->SetBinLabel( 2, "semi-e" );
  histos2D_[ "genDecayChn_hitFitProb" ]->GetXaxis()->SetBinLabel( 3, "#it{semi-#mu}" );
  histos2D_[ "genDecayChn_hitFitProb" ]->GetXaxis()->SetBinLabel( 4, "semi-#tau, to other" );
  histos2D_[ "genDecayChn_hitFitProb" ]->GetXaxis()->SetBinLabel( 5, "semi-#tau, to #mu" );
  histos2D_[ "genDecayChn_hitFitProb" ]->GetXaxis()->SetBinLabel( 6, "full lep. 0 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitProb" ]->GetXaxis()->SetBinLabel( 7, "full lep. 1 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitProb" ]->GetXaxis()->SetBinLabel( 8, "full lep. 2 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitProb" ]->SetYTitle( "HitFit prob." );
  histos2D_[ "genDecayChn_hitFitProb" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitProbLow" ] = fileService->make< TH2D >( "genDecayChn_hitFitProbLow", "HitFit probabilty vs. decay channel", 8, -0.5, 7.5, 25, 0., 0.04 );
  histos2D_[ "genDecayChn_hitFitProbLow" ]->SetXTitle( "decay chn." );
  histos2D_[ "genDecayChn_hitFitProbLow" ]->GetXaxis()->SetBinLabel( 1, "full had." );
  histos2D_[ "genDecayChn_hitFitProbLow" ]->GetXaxis()->SetBinLabel( 2, "semi-e" );
  histos2D_[ "genDecayChn_hitFitProbLow" ]->GetXaxis()->SetBinLabel( 3, "#it{semi-#mu}" );
  histos2D_[ "genDecayChn_hitFitProbLow" ]->GetXaxis()->SetBinLabel( 4, "semi-#tau, to other" );
  histos2D_[ "genDecayChn_hitFitProbLow" ]->GetXaxis()->SetBinLabel( 5, "semi-#tau, to #mu" );
  histos2D_[ "genDecayChn_hitFitProbLow" ]->GetXaxis()->SetBinLabel( 6, "full lep. 0 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitProbLow" ]->GetXaxis()->SetBinLabel( 7, "full lep. 1 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitProbLow" ]->GetXaxis()->SetBinLabel( 8, "full lep. 2 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitProbLow" ]->SetYTitle( "HitFit prob." );
  histos2D_[ "genDecayChn_hitFitProbLow" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitChi2" ] = fileService->make< TH2D >( "genDecayChn_hitFitChi2", "HitFit #chi^{2} vs. decay channel", 8, -0.5, 7.5, 50, 0., 100. );
  histos2D_[ "genDecayChn_hitFitChi2" ]->SetXTitle( "decay chn." );
  histos2D_[ "genDecayChn_hitFitChi2" ]->GetXaxis()->SetBinLabel( 1, "full had." );
  histos2D_[ "genDecayChn_hitFitChi2" ]->GetXaxis()->SetBinLabel( 2, "semi-e" );
  histos2D_[ "genDecayChn_hitFitChi2" ]->GetXaxis()->SetBinLabel( 3, "#it{semi-#mu}" );
  histos2D_[ "genDecayChn_hitFitChi2" ]->GetXaxis()->SetBinLabel( 4, "semi-#tau, to other" );
  histos2D_[ "genDecayChn_hitFitChi2" ]->GetXaxis()->SetBinLabel( 5, "semi-#tau, to #mu" );
  histos2D_[ "genDecayChn_hitFitChi2" ]->GetXaxis()->SetBinLabel( 6, "full lep. 0 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitChi2" ]->GetXaxis()->SetBinLabel( 7, "full lep. 1 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitChi2" ]->GetXaxis()->SetBinLabel( 8, "full lep. 2 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitChi2" ]->SetYTitle( "(#chi^{2})^{HitFit}" );
  histos2D_[ "genDecayChn_hitFitChi2" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitMT" ] = fileService->make< TH2D >( "genDecayChn_hitFitMT", "HitFit top mass vs. decay channel", 8, -0.5, 7.5, 100, 75., 325. );
  histos2D_[ "genDecayChn_hitFitMT" ]->SetXTitle( "decay chn." );
  histos2D_[ "genDecayChn_hitFitMT" ]->GetXaxis()->SetBinLabel( 1, "full had." );
  histos2D_[ "genDecayChn_hitFitMT" ]->GetXaxis()->SetBinLabel( 2, "semi-e" );
  histos2D_[ "genDecayChn_hitFitMT" ]->GetXaxis()->SetBinLabel( 3, "#it{semi-#mu}" );
  histos2D_[ "genDecayChn_hitFitMT" ]->GetXaxis()->SetBinLabel( 4, "semi-#tau, to other" );
  histos2D_[ "genDecayChn_hitFitMT" ]->GetXaxis()->SetBinLabel( 5, "semi-#tau, to #mu" );
  histos2D_[ "genDecayChn_hitFitMT" ]->GetXaxis()->SetBinLabel( 6, "full lep. 0 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitMT" ]->GetXaxis()->SetBinLabel( 7, "full lep. 1 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitMT" ]->GetXaxis()->SetBinLabel( 8, "full lep. 2 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitMT" ]->SetYTitle( "m_{top}^{HitFit} (GeV)" );
  histos2D_[ "genDecayChn_hitFitMT" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitSigMT" ] = fileService->make< TH2D >( "genDecayChn_hitFitSigMT", "HitFit top mass uncertainty vs. decay channel", 8, -0.5, 7.5, 25, 0., 50. );
  histos2D_[ "genDecayChn_hitFitSigMT" ]->SetXTitle( "decay chn." );
  histos2D_[ "genDecayChn_hitFitSigMT" ]->GetXaxis()->SetBinLabel( 1, "full had." );
  histos2D_[ "genDecayChn_hitFitSigMT" ]->GetXaxis()->SetBinLabel( 2, "semi-e" );
  histos2D_[ "genDecayChn_hitFitSigMT" ]->GetXaxis()->SetBinLabel( 3, "#it{semi-#mu}" );
  histos2D_[ "genDecayChn_hitFitSigMT" ]->GetXaxis()->SetBinLabel( 4, "semi-#tau, to other" );
  histos2D_[ "genDecayChn_hitFitSigMT" ]->GetXaxis()->SetBinLabel( 5, "semi-#tau, to #mu" );
  histos2D_[ "genDecayChn_hitFitSigMT" ]->GetXaxis()->SetBinLabel( 6, "full lep. 0 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitSigMT" ]->GetXaxis()->SetBinLabel( 7, "full lep. 1 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitSigMT" ]->GetXaxis()->SetBinLabel( 8, "full lep. 2 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitSigMT" ]->SetYTitle( "#sigma_{m_{top}}^{HitFit} (GeV)" );
  histos2D_[ "genDecayChn_hitFitSigMT" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitSigMTRel" ] = fileService->make< TH2D >( "genDecayChn_hitFitSigMTRel", "Relative HitFit top mass uncertainty vs. decay channel", 8, -0.5, 7.5, 25, 0., 0.25 );
  histos2D_[ "genDecayChn_hitFitSigMTRel" ]->SetXTitle( "decay chn." );
  histos2D_[ "genDecayChn_hitFitSigMTRel" ]->GetXaxis()->SetBinLabel( 1, "full had." );
  histos2D_[ "genDecayChn_hitFitSigMTRel" ]->GetXaxis()->SetBinLabel( 2, "semi-e" );
  histos2D_[ "genDecayChn_hitFitSigMTRel" ]->GetXaxis()->SetBinLabel( 3, "#it{semi-#mu}" );
  histos2D_[ "genDecayChn_hitFitSigMTRel" ]->GetXaxis()->SetBinLabel( 4, "semi-#tau, to other" );
  histos2D_[ "genDecayChn_hitFitSigMTRel" ]->GetXaxis()->SetBinLabel( 5, "semi-#tau, to #mu" );
  histos2D_[ "genDecayChn_hitFitSigMTRel" ]->GetXaxis()->SetBinLabel( 6, "full lep. 0 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitSigMTRel" ]->GetXaxis()->SetBinLabel( 7, "full lep. 1 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitSigMTRel" ]->GetXaxis()->SetBinLabel( 8, "full lep. 2 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitSigMTRel" ]->SetYTitle( "#frac{#sigma_{m_{t}}^{HitFit}}{m_{t}^{HitFit}}" );
  histos2D_[ "genDecayChn_hitFitSigMTRel" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitLepPt" ] = fileService->make< TH2D >( "genDecayChn_hitFitLepPt", "HitFit lepton transverse momentum vs. decay channel", 8, -0.5, 7.5, 50, 0., 250. );
  histos2D_[ "genDecayChn_hitFitLepPt" ]->SetXTitle( "decay chn." );
  histos2D_[ "genDecayChn_hitFitLepPt" ]->GetXaxis()->SetBinLabel( 1, "full had." );
  histos2D_[ "genDecayChn_hitFitLepPt" ]->GetXaxis()->SetBinLabel( 2, "semi-e" );
  histos2D_[ "genDecayChn_hitFitLepPt" ]->GetXaxis()->SetBinLabel( 3, "#it{semi-#mu}" );
  histos2D_[ "genDecayChn_hitFitLepPt" ]->GetXaxis()->SetBinLabel( 4, "semi-#tau, to other" );
  histos2D_[ "genDecayChn_hitFitLepPt" ]->GetXaxis()->SetBinLabel( 5, "semi-#tau, to #mu" );
  histos2D_[ "genDecayChn_hitFitLepPt" ]->GetXaxis()->SetBinLabel( 6, "full lep. 0 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitLepPt" ]->GetXaxis()->SetBinLabel( 7, "full lep. 1 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitLepPt" ]->GetXaxis()->SetBinLabel( 8, "full lep. 2 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitLepPt" ]->SetYTitle( "p_{t, l}^{HitFit} (GeV)" );
  histos2D_[ "genDecayChn_hitFitLepPt" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitLepEta" ] = fileService->make< TH2D >( "genDecayChn_hitFitLepEta", "HitFit lepton pseudo-rapidity vs. decay channel", 8, -0.5, 7.5, 50, -2.5, 2.5 );
  histos2D_[ "genDecayChn_hitFitLepEta" ]->SetXTitle( "decay chn." );
  histos2D_[ "genDecayChn_hitFitLepEta" ]->GetXaxis()->SetBinLabel( 1, "full had." );
  histos2D_[ "genDecayChn_hitFitLepEta" ]->GetXaxis()->SetBinLabel( 2, "semi-e" );
  histos2D_[ "genDecayChn_hitFitLepEta" ]->GetXaxis()->SetBinLabel( 3, "#it{semi-#mu}" );
  histos2D_[ "genDecayChn_hitFitLepEta" ]->GetXaxis()->SetBinLabel( 4, "semi-#tau, to other" );
  histos2D_[ "genDecayChn_hitFitLepEta" ]->GetXaxis()->SetBinLabel( 5, "semi-#tau, to #mu" );
  histos2D_[ "genDecayChn_hitFitLepEta" ]->GetXaxis()->SetBinLabel( 6, "full lep. 0 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitLepEta" ]->GetXaxis()->SetBinLabel( 7, "full lep. 1 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitLepEta" ]->GetXaxis()->SetBinLabel( 8, "full lep. 2 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitLepEta" ]->SetYTitle( "#eta_{l}^{HitFit}" );
  histos2D_[ "genDecayChn_hitFitLepEta" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitLepPhi" ] = fileService->make< TH2D >( "genDecayChn_hitFitLepPhi", "HitFit lepton azimutal angle vs. decay channel", 8, -0.5, 7.5, 50, -TMath::Pi(), TMath::Pi() );
  histos2D_[ "genDecayChn_hitFitLepPhi" ]->SetXTitle( "decay chn." );
  histos2D_[ "genDecayChn_hitFitLepPhi" ]->GetXaxis()->SetBinLabel( 1, "full had." );
  histos2D_[ "genDecayChn_hitFitLepPhi" ]->GetXaxis()->SetBinLabel( 2, "semi-e" );
  histos2D_[ "genDecayChn_hitFitLepPhi" ]->GetXaxis()->SetBinLabel( 3, "#it{semi-#mu}" );
  histos2D_[ "genDecayChn_hitFitLepPhi" ]->GetXaxis()->SetBinLabel( 4, "semi-#tau, to other" );
  histos2D_[ "genDecayChn_hitFitLepPhi" ]->GetXaxis()->SetBinLabel( 5, "semi-#tau, to #mu" );
  histos2D_[ "genDecayChn_hitFitLepPhi" ]->GetXaxis()->SetBinLabel( 6, "full lep. 0 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitLepPhi" ]->GetXaxis()->SetBinLabel( 7, "full lep. 1 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitLepPhi" ]->GetXaxis()->SetBinLabel( 8, "full lep. 2 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitLepPhi" ]->SetYTitle( "#phi_{l}^{HitFit}" );
  histos2D_[ "genDecayChn_hitFitLepPhi" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitLepPz" ] = fileService->make< TH2D >( "genDecayChn_hitFitLepPz", "HitFit lepton longitudinal momentum vs. decay channel", 8, -0.5, 7.5, 50, -500., 500. );
  histos2D_[ "genDecayChn_hitFitLepPz" ]->SetXTitle( "decay chn." );
  histos2D_[ "genDecayChn_hitFitLepPz" ]->GetXaxis()->SetBinLabel( 1, "full had." );
  histos2D_[ "genDecayChn_hitFitLepPz" ]->GetXaxis()->SetBinLabel( 2, "semi-e" );
  histos2D_[ "genDecayChn_hitFitLepPz" ]->GetXaxis()->SetBinLabel( 3, "#it{semi-#mu}" );
  histos2D_[ "genDecayChn_hitFitLepPz" ]->GetXaxis()->SetBinLabel( 4, "semi-#tau, to other" );
  histos2D_[ "genDecayChn_hitFitLepPz" ]->GetXaxis()->SetBinLabel( 5, "semi-#tau, to #mu" );
  histos2D_[ "genDecayChn_hitFitLepPz" ]->GetXaxis()->SetBinLabel( 6, "full lep. 0 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitLepPz" ]->GetXaxis()->SetBinLabel( 7, "full lep. 1 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitLepPz" ]->GetXaxis()->SetBinLabel( 8, "full lep. 2 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitLepPz" ]->SetYTitle( "p_{z, l}^{HitFit} (GeV)" );
  histos2D_[ "genDecayChn_hitFitLepPz" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitNuM" ] = fileService->make< TH2D >( "genDecayChn_hitFitNuM", "HitFit neutrino mass vs. decay channel", 8, -0.5, 7.5, 50, -0.0001, 0.0001 );
  histos2D_[ "genDecayChn_hitFitNuM" ]->SetXTitle( "decay chn." );
  histos2D_[ "genDecayChn_hitFitNuM" ]->GetXaxis()->SetBinLabel( 1, "full had." );
  histos2D_[ "genDecayChn_hitFitNuM" ]->GetXaxis()->SetBinLabel( 2, "semi-e" );
  histos2D_[ "genDecayChn_hitFitNuM" ]->GetXaxis()->SetBinLabel( 3, "#it{semi-#mu}" );
  histos2D_[ "genDecayChn_hitFitNuM" ]->GetXaxis()->SetBinLabel( 4, "semi-#tau, to other" );
  histos2D_[ "genDecayChn_hitFitNuM" ]->GetXaxis()->SetBinLabel( 5, "semi-#tau, to #mu" );
  histos2D_[ "genDecayChn_hitFitNuM" ]->GetXaxis()->SetBinLabel( 6, "full lep. 0 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitNuM" ]->GetXaxis()->SetBinLabel( 7, "full lep. 1 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitNuM" ]->GetXaxis()->SetBinLabel( 8, "full lep. 2 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitNuM" ]->SetYTitle( "m_{#nu}^{HitFit} (GeV)" );
  histos2D_[ "genDecayChn_hitFitNuM" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitNuPt" ] = fileService->make< TH2D >( "genDecayChn_hitFitNuPt", "HitFit neutrino transverse momentum vs. decay channel", 8, -0.5, 7.5, 50, 0., 250. );
  histos2D_[ "genDecayChn_hitFitNuPt" ]->SetXTitle( "decay chn." );
  histos2D_[ "genDecayChn_hitFitNuPt" ]->GetXaxis()->SetBinLabel( 1, "full had." );
  histos2D_[ "genDecayChn_hitFitNuPt" ]->GetXaxis()->SetBinLabel( 2, "semi-e" );
  histos2D_[ "genDecayChn_hitFitNuPt" ]->GetXaxis()->SetBinLabel( 3, "#it{semi-#mu}" );
  histos2D_[ "genDecayChn_hitFitNuPt" ]->GetXaxis()->SetBinLabel( 4, "semi-#tau, to other" );
  histos2D_[ "genDecayChn_hitFitNuPt" ]->GetXaxis()->SetBinLabel( 5, "semi-#tau, to #mu" );
  histos2D_[ "genDecayChn_hitFitNuPt" ]->GetXaxis()->SetBinLabel( 6, "full lep. 0 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitNuPt" ]->GetXaxis()->SetBinLabel( 7, "full lep. 1 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitNuPt" ]->GetXaxis()->SetBinLabel( 8, "full lep. 2 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitNuPt" ]->SetYTitle( "p_{t, #nu}^{HitFit} (GeV)" );
  histos2D_[ "genDecayChn_hitFitNuPt" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitNuEta" ] = fileService->make< TH2D >( "genDecayChn_hitFitNuEta", "HitFit neutrino pseudo-rapidity vs. decay channel", 8, -0.5, 7.5, 50, -4., 4. );
  histos2D_[ "genDecayChn_hitFitNuEta" ]->SetXTitle( "decay chn." );
  histos2D_[ "genDecayChn_hitFitNuEta" ]->GetXaxis()->SetBinLabel( 1, "full had." );
  histos2D_[ "genDecayChn_hitFitNuEta" ]->GetXaxis()->SetBinLabel( 2, "semi-e" );
  histos2D_[ "genDecayChn_hitFitNuEta" ]->GetXaxis()->SetBinLabel( 3, "#it{semi-#mu}" );
  histos2D_[ "genDecayChn_hitFitNuEta" ]->GetXaxis()->SetBinLabel( 4, "semi-#tau, to other" );
  histos2D_[ "genDecayChn_hitFitNuEta" ]->GetXaxis()->SetBinLabel( 5, "semi-#tau, to #mu" );
  histos2D_[ "genDecayChn_hitFitNuEta" ]->GetXaxis()->SetBinLabel( 6, "full lep. 0 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitNuEta" ]->GetXaxis()->SetBinLabel( 7, "full lep. 1 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitNuEta" ]->GetXaxis()->SetBinLabel( 8, "full lep. 2 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitNuEta" ]->SetYTitle( "#eta_{#nu}^{HitFit}" );
  histos2D_[ "genDecayChn_hitFitNuEta" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitNuPhi" ] = fileService->make< TH2D >( "genDecayChn_hitFitNuPhi", "HitFit neutrino azimutal angle vs. decay channel", 8, -0.5, 7.5, 50, -TMath::Pi(), TMath::Pi() );
  histos2D_[ "genDecayChn_hitFitNuPhi" ]->SetXTitle( "decay chn." );
  histos2D_[ "genDecayChn_hitFitNuPhi" ]->GetXaxis()->SetBinLabel( 1, "full had." );
  histos2D_[ "genDecayChn_hitFitNuPhi" ]->GetXaxis()->SetBinLabel( 2, "semi-e" );
  histos2D_[ "genDecayChn_hitFitNuPhi" ]->GetXaxis()->SetBinLabel( 3, "#it{semi-#mu}" );
  histos2D_[ "genDecayChn_hitFitNuPhi" ]->GetXaxis()->SetBinLabel( 4, "semi-#tau, to other" );
  histos2D_[ "genDecayChn_hitFitNuPhi" ]->GetXaxis()->SetBinLabel( 5, "semi-#tau, to #mu" );
  histos2D_[ "genDecayChn_hitFitNuPhi" ]->GetXaxis()->SetBinLabel( 6, "full lep. 0 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitNuPhi" ]->GetXaxis()->SetBinLabel( 7, "full lep. 1 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitNuPhi" ]->GetXaxis()->SetBinLabel( 8, "full lep. 2 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitNuPhi" ]->SetYTitle( "#phi_{#nu}^{HitFit}" );
  histos2D_[ "genDecayChn_hitFitNuPhi" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitNuPz" ] = fileService->make< TH2D >( "genDecayChn_hitFitNuPz", "HitFit neutrino longitudinal momentum vs. decay channel", 8, -0.5, 7.5, 50, -500., 500. );
  histos2D_[ "genDecayChn_hitFitNuPz" ]->SetXTitle( "decay chn." );
  histos2D_[ "genDecayChn_hitFitNuPz" ]->GetXaxis()->SetBinLabel( 1, "full had." );
  histos2D_[ "genDecayChn_hitFitNuPz" ]->GetXaxis()->SetBinLabel( 2, "semi-e" );
  histos2D_[ "genDecayChn_hitFitNuPz" ]->GetXaxis()->SetBinLabel( 3, "#it{semi-#mu}" );
  histos2D_[ "genDecayChn_hitFitNuPz" ]->GetXaxis()->SetBinLabel( 4, "semi-#tau, to other" );
  histos2D_[ "genDecayChn_hitFitNuPz" ]->GetXaxis()->SetBinLabel( 5, "semi-#tau, to #mu" );
  histos2D_[ "genDecayChn_hitFitNuPz" ]->GetXaxis()->SetBinLabel( 6, "full lep. 0 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitNuPz" ]->GetXaxis()->SetBinLabel( 7, "full lep. 1 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitNuPz" ]->GetXaxis()->SetBinLabel( 8, "full lep. 2 #mu or #tau to #mu" );
  histos2D_[ "genDecayChn_hitFitNuPz" ]->SetYTitle( "p_{z, #nu}^{HitFit} (GeV)" );
  histos2D_[ "genDecayChn_hitFitNuPz" ]->SetZTitle( "events" );

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
  const reco::Candidate * hitFitLep( ttSemiLeptonicEvent->singleLepton( TtEvent::kHitFit ) );
  const reco::Candidate * hitFitNu( ttSemiLeptonicEvent->singleNeutrino( TtEvent::kHitFit ) );
  const int nRealNuSol( ttSemiLeptonicEvent->numberOfRealNeutrinoSolutions( TtEvent::kHitFit ) );

  histos1D_[ "nRealNuSol" ]->Fill( nRealNuSol );
  histos1D_[ "hitFitProb" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
  histos1D_[ "hitFitProbLow" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );

  // Fill histograms only for converged fits
  if ( ttSemiLeptonicEvent->hitFitProb() >= 0. ) {

    histos1D_[ "hitFitChi2" ]->Fill( ttSemiLeptonicEvent->hitFitChi2() );
    histos1D_[ "hitFitMT" ]->Fill( ttSemiLeptonicEvent->hitFitMT() );
    histos1D_[ "hitFitSigMT" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT() );
    histos1D_[ "hitFitSigMTRel" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );
    histos2D_[ "hitFitProb_hitFitMT" ]->Fill( ttSemiLeptonicEvent->hitFitProb(), ttSemiLeptonicEvent->hitFitMT() );
    histos2D_[ "hitFitProb_hitFitSigMT" ]->Fill( ttSemiLeptonicEvent->hitFitProb(), ttSemiLeptonicEvent->hitFitSigMT() );
    histos2D_[ "hitFitProb_hitFitSigMTRel" ]->Fill( ttSemiLeptonicEvent->hitFitProb(), ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );
    histos2D_[ "hitFitMT_hitFitSigMT" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT() );
    histos2D_[ "hitFitMT_hitFitSigMTRel" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );

    histos1D_[ "hitFitLepPt" ]->Fill( hitFitLep->pt() );
    histos1D_[ "hitFitLepEta" ]->Fill( hitFitLep->eta() );
    histos1D_[ "hitFitLepPhi" ]->Fill( hitFitLep->phi() );
    histos1D_[ "hitFitLepPz" ]->Fill( hitFitLep->pz() );
    histos1D_[ "hitFitNuM" ]->Fill( hitFitNu->mass() );
    histos1D_[ "hitFitNuPt" ]->Fill( hitFitNu->pt() );
    histos1D_[ "hitFitNuEta" ]->Fill( hitFitNu->eta() );
    histos1D_[ "hitFitNuPhi" ]->Fill( hitFitNu->phi() );
    histos1D_[ "hitFitNuPz" ]->Fill( hitFitNu->pz() );

  }

  // MC specific (using true information)
  if ( ! iEvent.isRealData() ) {

    bool kSIGNAL( false );

    // Generated event
    const TtGenEvent * ttGenEvent( ttSemiLeptonicEvent->genEvent().get() );
    // Proceed only for ttbar events
    if ( ttGenEvent->isTtBar() ) {

      // Decay channels
      unsigned trueDecayChn( WDecay::kNone );
      unsigned nTrueOtherMuon( 0 );
      if ( ttGenEvent->isSemiLeptonic( WDecay::kElec ) ) {
        trueDecayChn = ttGenEvent->semiLeptonicChannel();
      }
      else if ( ttGenEvent->isSemiLeptonic( WDecay::kMuon ) ) {
        trueDecayChn = ttGenEvent->semiLeptonicChannel();
        kSIGNAL = true;
      }
      else if ( ttGenEvent->isSemiLeptonic( WDecay::kTau ) ) {
        trueDecayChn = ttGenEvent->semiLeptonicChannel();
        const reco::GenParticle * genSemiTau( ttGenEvent->singleLepton() );
        size_t nD( genSemiTau->numberOfDaughters() );
        for ( size_t iD = 0; iD < nD; ++iD ) {
          const reco::Candidate * genTauDaughter( genSemiTau->daughter( iD ) );
          size_t nDD( genTauDaughter->numberOfDaughters() );
          for ( size_t iDD = 0; iDD < nDD; ++iDD ) {
            const reco::Candidate * genTauGrandDaughter( genTauDaughter->daughter( iDD ) );
            if ( fabs( genTauGrandDaughter->pdgId() ) == 13 ) ++nTrueOtherMuon;
          }
        }
        trueDecayChn += nTrueOtherMuon;
      }
      else if ( ttGenEvent->isFullLeptonic() ) {
        trueDecayChn = WDecay::kTau + 2;
        const reco::GenParticle * genFullLep( ttGenEvent->lepton() );
        if ( fabs( genFullLep->pdgId() ) == 15 ) {
          size_t nD( genFullLep->numberOfDaughters() );
          for ( size_t iD = 0; iD < nD; ++iD ) {
            const reco::Candidate * genTauDaughter( genFullLep->daughter( iD ) );
            size_t nDD( genTauDaughter->numberOfDaughters() );
            for ( size_t iDD = 0; iDD < nDD; ++iDD ) {
              const reco::Candidate * genTauGrandDaughter( genTauDaughter->daughter( iDD ) );
              if ( fabs( genTauGrandDaughter->pdgId() ) == 13 ) ++nTrueOtherMuon;
            }
          }
        }
        else if ( fabs( genFullLep->pdgId() ) == 13 ) {
           ++nTrueOtherMuon;
        }
        const reco::GenParticle * genFullLepBar( ttGenEvent->leptonBar() );
        if ( fabs( genFullLep->pdgId() ) == 15 ) {
          size_t nD( genFullLepBar->numberOfDaughters() );
          for ( size_t iD = 0; iD < nD; ++iD ) {
            const reco::Candidate * genTauDaughter( genFullLepBar->daughter( iD ) );
            size_t nDD( genTauDaughter->numberOfDaughters() );
            for ( size_t iDD = 0; iDD < nDD; ++iDD ) {
              const reco::Candidate * genTauGrandDaughter( genTauDaughter->daughter( iDD ) );
              if ( fabs( genTauGrandDaughter->pdgId() ) == 13 ) ++nTrueOtherMuon;
            }
          }
        }
        else if ( fabs( genFullLepBar->pdgId() ) == 13 ) {
           ++nTrueOtherMuon;
        }
        trueDecayChn += nTrueOtherMuon;
      }

      histos1D_[ "genDecayChn" ]->Fill( trueDecayChn );
      histos2D_[ "genDecayChn_nRealNuSol" ]->Fill( trueDecayChn, nRealNuSol );
      histos2D_[ "genDecayChn_hitFitProb" ]->Fill( trueDecayChn, ttSemiLeptonicEvent->hitFitProb() );
      histos2D_[ "genDecayChn_hitFitProbLow" ]->Fill( trueDecayChn, ttSemiLeptonicEvent->hitFitProb() );

      // Fill histograms only for converged fits
      if ( ttSemiLeptonicEvent->hitFitProb() >= 0. ) {

        histos2D_[ "genDecayChn_hitFitChi2" ]->Fill( trueDecayChn, ttSemiLeptonicEvent->hitFitChi2() );
        histos2D_[ "genDecayChn_hitFitMT" ]->Fill( trueDecayChn, ttSemiLeptonicEvent->hitFitMT() );
        histos2D_[ "genDecayChn_hitFitSigMT" ]->Fill( trueDecayChn, ttSemiLeptonicEvent->hitFitSigMT() );
        histos2D_[ "genDecayChn_hitFitSigMTRel" ]->Fill( trueDecayChn, ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );

        histos2D_[ "genDecayChn_hitFitLepPt" ]->Fill( trueDecayChn, hitFitLep->pt() );
        histos2D_[ "genDecayChn_hitFitLepEta" ]->Fill( trueDecayChn, hitFitLep->eta() );
        histos2D_[ "genDecayChn_hitFitLepPhi" ]->Fill( trueDecayChn, hitFitLep->phi() );
        histos2D_[ "genDecayChn_hitFitLepPz" ]->Fill( trueDecayChn, hitFitLep->pz() );
        histos2D_[ "genDecayChn_hitFitNuM" ]->Fill( trueDecayChn, hitFitNu->mass() );
        histos2D_[ "genDecayChn_hitFitNuPt" ]->Fill( trueDecayChn, hitFitNu->pt() );
        histos2D_[ "genDecayChn_hitFitNuEta" ]->Fill( trueDecayChn, hitFitNu->eta() );
        histos2D_[ "genDecayChn_hitFitNuPhi" ]->Fill( trueDecayChn, hitFitNu->phi() );
        histos2D_[ "genDecayChn_hitFitNuPz" ]->Fill( trueDecayChn, hitFitNu->pz() );

      }

    }

    if ( kSIGNAL ) {

      // Generator particles
      const reco::GenParticle * genTopLep( ttGenEvent->leptonicDecayTop() );
      const reco::GenParticle * genTopHad( ttGenEvent->hadronicDecayTop() );
      const reco::GenParticle * genMu( ttGenEvent->singleLepton() );
      const reco::GenParticle * genNu( ttGenEvent->singleNeutrino() );

      histos1D_[ "genTopLepM" ]->Fill( genTopLep->mass() );
      histos1D_[ "genTopHadM" ]->Fill( genTopHad->mass() );
      histos1D_[ "genMuPt" ]->Fill( genMu->pt() );
      histos1D_[ "genMuEta" ]->Fill( genMu->eta() );
      histos1D_[ "genMuPhi" ]->Fill( genMu->phi() );
      histos1D_[ "genMuPz" ]->Fill( genMu->pz() );
      histos1D_[ "genNuM" ]->Fill( genNu->mass() );
      histos1D_[ "genNuPt" ]->Fill( genNu->pt() );
      histos1D_[ "genNuEta" ]->Fill( genNu->eta() );
      histos1D_[ "genNuPhi" ]->Fill( genNu->phi() );
      histos1D_[ "genNuPz" ]->Fill( genNu->pz() );

      histos1D_[ "nRealNuSolSig" ]->Fill( nRealNuSol );
      histos1D_[ "hitFitProbSig" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
      histos1D_[ "hitFitProbLowSig" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );

      // Fill histograms only for converged fits
      if ( ttSemiLeptonicEvent->hitFitProb() >= 0. ) {

        histos1D_[ "hitFitDiffGenTopLepM" ]->Fill( ttSemiLeptonicEvent->hitFitMT() - genTopLep->mass() );
        histos1D_[ "hitFitDiffGenTopHadM" ]->Fill( ttSemiLeptonicEvent->hitFitMT() - genTopHad->mass() );
        histos1D_[ "hitFitDiffGenMuPt" ]->Fill( hitFitLep->pt() - genMu->pt() );
        histos1D_[ "hitFitDiffGenMuEta" ]->Fill( hitFitLep->eta() - genMu->eta() );
        histos1D_[ "hitFitDiffGenMuPhi" ]->Fill( hitFitLep->phi() - genMu->phi() );
        histos1D_[ "hitFitDiffGenMuPz" ]->Fill( hitFitLep->pz() - genMu->pz() );
        histos1D_[ "hitFitDiffGenNuM" ]->Fill( hitFitNu->mass() - genNu->mass() );
        histos1D_[ "hitFitDiffGenNuPt" ]->Fill( hitFitNu->pt() - genNu->pt() );
        histos1D_[ "hitFitDiffGenNuEta" ]->Fill( hitFitNu->eta() - genNu->eta() );
        histos1D_[ "hitFitDiffGenNuPhi" ]->Fill( hitFitNu->phi() - genNu->phi() );
        histos1D_[ "hitFitDiffGenNuPz" ]->Fill( hitFitNu->pz() - genNu->pz() );

        histos1D_[ "hitFitChi2Sig" ]->Fill( ttSemiLeptonicEvent->hitFitChi2() );
        histos1D_[ "hitFitMTSig" ]->Fill( ttSemiLeptonicEvent->hitFitMT() );
        histos1D_[ "hitFitSigMTSig" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT() );
        histos1D_[ "hitFitSigMTRelSig" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );
        histos2D_[ "hitFitMT_hitFitSigMTSig" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT() );
        histos2D_[ "hitFitMT_hitFitSigMTRelSig" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );

        histos1D_[ "hitFitLepPtSig" ]->Fill( hitFitLep->pt() );
        histos1D_[ "hitFitLepEtaSig" ]->Fill( hitFitLep->eta() );
        histos1D_[ "hitFitLepPhiSig" ]->Fill( hitFitLep->phi() );
        histos1D_[ "hitFitLepPzSig" ]->Fill( hitFitLep->pz() );
        histos1D_[ "hitFitNuMSig" ]->Fill( hitFitNu->mass() );
        histos1D_[ "hitFitNuPtSig" ]->Fill( hitFitNu->pt() );
        histos1D_[ "hitFitNuEtaSig" ]->Fill( hitFitNu->eta() );
        histos1D_[ "hitFitNuPhiSig" ]->Fill( hitFitNu->phi() );
        histos1D_[ "hitFitNuPzSig" ]->Fill( hitFitNu->pz() );

      }

    }
    else {

      histos1D_[ "nRealNuSolBkg" ]->Fill( nRealNuSol );
      histos1D_[ "hitFitProbBkg" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
      histos1D_[ "hitFitProbLowBkg" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );

      // Fill histograms only for converged fits
      if ( ttSemiLeptonicEvent->hitFitProb() >= 0. ) {

        histos1D_[ "hitFitChi2Bkg" ]->Fill( ttSemiLeptonicEvent->hitFitChi2() );
        histos1D_[ "hitFitMTBkg" ]->Fill( ttSemiLeptonicEvent->hitFitMT() );
        histos1D_[ "hitFitSigMTBkg" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT() );
        histos1D_[ "hitFitSigMTRelBkg" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );
        histos2D_[ "hitFitMT_hitFitSigMTBkg" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT() );
        histos2D_[ "hitFitMT_hitFitSigMTRelBkg" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );

        histos1D_[ "hitFitLepPtBkg" ]->Fill( hitFitLep->pt() );
        histos1D_[ "hitFitLepEtaBkg" ]->Fill( hitFitLep->eta() );
        histos1D_[ "hitFitLepPhiBkg" ]->Fill( hitFitLep->phi() );
        histos1D_[ "hitFitLepPzBkg" ]->Fill( hitFitLep->pz() );
        histos1D_[ "hitFitNuMBkg" ]->Fill( hitFitNu->mass() );
        histos1D_[ "hitFitNuPtBkg" ]->Fill( hitFitNu->pt() );
        histos1D_[ "hitFitNuEtaBkg" ]->Fill( hitFitNu->eta() );
        histos1D_[ "hitFitNuPhiBkg" ]->Fill( hitFitNu->phi() );
        histos1D_[ "hitFitNuPzBkg" ]->Fill( hitFitNu->pz() );

      }

    }

  }

}


// End job
void TopHitFitAnalyzer::endJob()
{
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( TopHitFitAnalyzer );
