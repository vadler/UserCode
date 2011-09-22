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
    // TQAF semi-leptonic event
    edm::InputTag ttSemiLeptonicEvent_;
    // HitFit probability
    unsigned binsHitFitProb_;
    unsigned binsHitFitProbLow_;
    double   maxHitFitProbLow_;
    unsigned binsHitFitProbLog_;
    double   minHitFitProbLog_;
    unsigned binsHitFitChi2_;
    // HitFit chi^2
    double   maxHitFitChi2_;
    // HitFit top mass
    unsigned binsHitFitMT_;
    double   minHitFitMT_;
    double   maxHitFitMT_;
    // HitFit top mass uncertainty
    unsigned binsHitFitSigMT_;
    double   maxHitFitSigMT_;
    // Relative HitFit top mass uncertainty
    unsigned binsHitFitSigMTRel_;
    double   maxHitFitSigMTRel_;
    // Top mass
    unsigned binsTopM_;
    double   minTopM_;
    double   maxTopM_;
    // Lepton mass
    unsigned binsGenLepM_;
    double   maxGenLepM_;
    unsigned binsLepM_;
    double   maxLepM_;
    // Lepton p_t
    unsigned binsLepPt_;
    double   maxLepPt_;
    // Lepton eta
    unsigned binsLepEta_;
    double   maxLepEta_;
    // Lepton phi
    unsigned binsLepPhi_;
    // Lepton p_z
    unsigned binsLepPz_;
    double   maxLepPz_;
    // Neutrino mass
    unsigned binsNuM_;
    double   maxNuM_;
    // Neutrino p_t
    unsigned binsNuPt_;
    double   maxNuPt_;
    // Neutrino eta
    unsigned binsNuEta_;
    double   maxNuEta_;
    // Neutrino phi
    unsigned binsNuPhi_;
    // Neutrino p_z
    unsigned binsNuPz_;
    double   maxNuPz_;
    // Top mass deviation
    unsigned binsDiffTopM_;
    double   maxDiffTopM_;
    // Lepton mass deviation
    unsigned binsDiffLepM_;
    double   maxDiffLepM_;
    // Lepton p_t deviation
    unsigned binsDiffLepPt_;
    double   maxDiffLepPt_;
    // Lepton eta deviation
    unsigned binsDiffLepEta_;
    double   maxDiffLepEta_;
    // Lepton phi deviation
    unsigned binsDiffLepPhi_;
    double   maxDiffLepPhi_;
    // Lepton p_z deviation
    unsigned binsDiffLepPz_;
    double   maxDiffLepPz_;
    // Neutrino mass deviation
    unsigned binsDiffNuM_;
    double   maxDiffNuM_;
    // Neutrino p_t  deviation
    unsigned binsDiffNuPt_;
    double   maxDiffNuPt_;
    // Neutrino eta deviation
    unsigned binsDiffNuEta_;
    double   maxDiffNuEta_;
    // Neutrino phi deviation
    unsigned binsDiffNuPhi_;
    double   maxDiffNuPhi_;
    // Neutrino p_z  deviation
    unsigned binsDiffNuPz_;
    double   maxDiffNuPz_;

    /// Histograms
    std::map< std::string, TH1D* > histos1D_;
    std::map< std::string, TH2D* > histos2D_;

    /// Constants
    std::vector< std::string > decayChnNames_;

    /// Private functions

    void assignDecayChnNames( TAxis * iAxis );

};


#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "AnalysisDataFormats/TopObjects/interface/TtSemiLeptonicEvent.h"


// Default constructor
TopHitFitAnalyzer::TopHitFitAnalyzer( const edm::ParameterSet & iConfig )
: ttSemiLeptonicEvent_( iConfig.getParameter< edm::InputTag >( "ttSemiLeptonicEvent" ) )
, binsHitFitProb_( iConfig.getParameter< unsigned >( "binsHitFitProb" ) )
, binsHitFitProbLow_( iConfig.getParameter< unsigned >( "binsHitFitProbLow" ) )
, maxHitFitProbLow_( iConfig.getParameter< double >( "maxHitFitProbLow" ) )
, binsHitFitProbLog_( iConfig.getParameter< unsigned >( "binsHitFitProbLog" ) )
, minHitFitProbLog_( iConfig.getParameter< double >( "minHitFitProbLog" ) )
, binsHitFitChi2_( iConfig.getParameter< unsigned >( "binsHitFitChi2" ) )
, maxHitFitChi2_( iConfig.getParameter< double >( "maxHitFitChi2" ) )
, binsHitFitMT_( iConfig.getParameter< unsigned >( "binsHitFitMT" ) )
, minHitFitMT_( iConfig.getParameter< double >( "minHitFitMT" ) )
, maxHitFitMT_( iConfig.getParameter< double >( "maxHitFitMT" ) )
, binsHitFitSigMT_( iConfig.getParameter< unsigned >( "binsHitFitSigMT" ) )
, maxHitFitSigMT_( iConfig.getParameter< double >( "maxHitFitSigMT" ) )
, binsHitFitSigMTRel_( iConfig.getParameter< unsigned >( "binsHitFitSigMTRel" ) )
, maxHitFitSigMTRel_( iConfig.getParameter< double >( "maxHitFitSigMTRel" ) )
, binsTopM_( iConfig.getParameter< unsigned >( "binsTopM" ) )
, minTopM_( iConfig.getParameter< double >( "minTopM" ) )
, maxTopM_( iConfig.getParameter< double >( "maxTopM" ) )
, binsGenLepM_( iConfig.getParameter< unsigned >( "binsGenLepM" ) )
, maxGenLepM_( iConfig.getParameter< double >( "maxGenLepM" ) )
, binsLepM_( iConfig.getParameter< unsigned >( "binsLepM" ) )
, maxLepM_( iConfig.getParameter< double >( "maxLepM" ) )
, binsLepPt_( iConfig.getParameter< unsigned >( "binsLepPt" ) )
, maxLepPt_( iConfig.getParameter< double >( "maxLepPt" ) )
, binsLepEta_( iConfig.getParameter< unsigned >( "binsLepEta" ) )
, maxLepEta_( iConfig.getParameter< double >( "maxLepEta" ) )
, binsLepPhi_( iConfig.getParameter< unsigned >( "binsLepPhi" ) )
, binsLepPz_( iConfig.getParameter< unsigned >( "binsLepPz" ) )
, maxLepPz_( iConfig.getParameter< double >( "maxLepPz" ) )
, binsNuM_( iConfig.getParameter< unsigned >( "binsNuM" ) )
, maxNuM_( iConfig.getParameter< double >( "maxNuM" ) )
, binsNuPt_( iConfig.getParameter< unsigned >( "binsNuPt" ) )
, maxNuPt_ ( iConfig.getParameter< double >( "maxNuPt" ) )
, binsNuEta_( iConfig.getParameter< unsigned >( "binsNuEta" ) )
, maxNuEta_( iConfig.getParameter< double >( "maxNuEta" ) )
, binsNuPhi_( iConfig.getParameter< unsigned >( "binsNuPhi" ) )
, binsNuPz_( iConfig.getParameter< unsigned >( "binsNuPz" ) )
, maxNuPz_( iConfig.getParameter< double >( "maxNuPz" ) )
, binsDiffTopM_( iConfig.getParameter< unsigned >( "binsDiffTopM" ) )
, maxDiffTopM_( iConfig.getParameter< double >( "maxDiffTopM" ) )
, binsDiffLepM_( iConfig.getParameter< unsigned >( "binsDiffLepM" ) )
, maxDiffLepM_( iConfig.getParameter< double >( "maxDiffLepM" ) )
, binsDiffLepPt_( iConfig.getParameter< unsigned >( "binsDiffLepPt" ) )
, maxDiffLepPt_( iConfig.getParameter< double >( "maxDiffLepPt" ) )
, binsDiffLepEta_( iConfig.getParameter< unsigned >( "binsDiffLepEta" ) )
, maxDiffLepEta_( iConfig.getParameter< double >( "maxDiffLepEta" ) )
, binsDiffLepPhi_( iConfig.getParameter< unsigned >( "binsDiffLepPhi" ) )
, maxDiffLepPhi_( iConfig.getParameter< double >( "maxDiffLepPhi" ) )
, binsDiffLepPz_( iConfig.getParameter< unsigned >( "binsDiffLepPz" ) )
, maxDiffLepPz_( iConfig.getParameter< double >( "maxDiffLepPz" ) )
, binsDiffNuM_( iConfig.getParameter< unsigned >( "binsDiffNuM" ) )
, maxDiffNuM_( iConfig.getParameter< double >( "maxDiffNuM" ) )
, binsDiffNuPt_( iConfig.getParameter< unsigned >( "binsDiffNuPt" ) )
, maxDiffNuPt_( iConfig.getParameter< double >( "maxDiffNuPt" ) )
, binsDiffNuEta_( iConfig.getParameter< unsigned >( "binsDiffNuEta" ) )
, maxDiffNuEta_( iConfig.getParameter< double >( "maxDiffNuEta" ) )
, binsDiffNuPhi_( iConfig.getParameter< unsigned >( "binsDiffNuPhi" ) )
, maxDiffNuPhi_( iConfig.getParameter< double >( "maxDiffNuPhi" ) )
, binsDiffNuPz_( iConfig.getParameter< unsigned >( "binsDiffNuPz" ) )
, maxDiffNuPz_( iConfig.getParameter< double >( "maxDiffNuPz" ) )
, histos1D_()
, histos2D_()
{

  // FIXME: This is still muon-specific
  decayChnNames_.clear();
  decayChnNames_.push_back( "" ); // 1st histo bin is underflow
  decayChnNames_.push_back( "full had." );
  decayChnNames_.push_back( "semi e" );
  decayChnNames_.push_back( "#it{semi #mu}" );
  decayChnNames_.push_back( "semi #tau, to other" );
  decayChnNames_.push_back( "semi #tau, to #mu" );
  decayChnNames_.push_back( "full lep. 0 #mu or #tau to #mu" );
  decayChnNames_.push_back( "full lep. 1 #mu or #tau to #mu" );
  decayChnNames_.push_back( "full lep. 2 #mu or #tau to #mu" );

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
  histos1D_[ "hitFitProb" ] = fileService->make< TH1D >( "hitFitProb", "HitFit probabilty", binsHitFitProb_, 0., 1. );
  histos1D_[ "hitFitProb" ]->SetXTitle( "P^{HitFit}" );
  histos1D_[ "hitFitProb" ]->SetYTitle( "events" );
  histos1D_[ "hitFitProbLow" ] = fileService->make< TH1D >( "hitFitProbLow", "HitFit probabilty", binsHitFitProbLow_, 0., maxHitFitProbLow_ );
  histos1D_[ "hitFitProbLow" ]->SetXTitle( "P^{HitFit}" );
  histos1D_[ "hitFitProbLow" ]->SetYTitle( "events" );
  histos1D_[ "hitFitProbLog" ] = fileService->make< TH1D >( "hitFitProbLog", "HitFit probabilty", binsHitFitProbLog_, minHitFitProbLog_, 0. );
  histos1D_[ "hitFitProbLog" ]->SetXTitle( "log_{10} P" );
  histos1D_[ "hitFitProbLog" ]->SetYTitle( "events" );
  histos1D_[ "hitFitChi2" ] = fileService->make< TH1D >( "hitFitChi2", "HitFit #chi^{2}", binsHitFitChi2_, 0., maxHitFitChi2_ );
  histos1D_[ "hitFitChi2" ]->SetXTitle( "(#chi^{2})^{HitFit}" );
  histos1D_[ "hitFitChi2" ]->SetYTitle( "events" );
  histos1D_[ "hitFitMT" ] = fileService->make< TH1D >( "hitFitMT", "HitFit top mass", binsHitFitMT_, minHitFitMT_, maxHitFitMT_ );
  histos1D_[ "hitFitMT" ]->SetXTitle( "m_{top}^{HitFit} (GeV)" );
  histos1D_[ "hitFitMT" ]->SetYTitle( "events" );
  histos1D_[ "hitFitSigMT" ] = fileService->make< TH1D >( "hitFitSigMT", "HitFit top mass uncertainty", binsHitFitSigMT_, 0., maxHitFitSigMT_ );
  histos1D_[ "hitFitSigMT" ]->SetXTitle( "#sigma_{m_{top}}^{HitFit} (GeV)" );
  histos1D_[ "hitFitSigMT" ]->SetYTitle( "events" );
  histos1D_[ "hitFitSigMTRel" ] = fileService->make< TH1D >( "hitFitSigMTRel", "Relative HitFit top mass uncertainty", binsHitFitSigMTRel_, 0., maxHitFitSigMTRel_ );
  histos1D_[ "hitFitSigMTRel" ]->SetXTitle( "#frac{#sigma_{m_{t}}^{HitFit}}{m_{t}^{HitFit}}" );
  histos1D_[ "hitFitSigMTRel" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepM" ] = fileService->make< TH1D >( "hitFitLepM", "HitFit lepton mass", binsLepM_, -maxLepM_, maxLepM_ );
  histos1D_[ "hitFitLepM" ]->SetXTitle( "m_{l}^{HitFit} (GeV)" );
  histos1D_[ "hitFitLepM" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepPt" ] = fileService->make< TH1D >( "hitFitLepPt", "HitFit lepton transverse momentum", binsLepPt_, 0., maxLepPt_ );
  histos1D_[ "hitFitLepPt" ]->SetXTitle( "p_{t, l}^{HitFit} (GeV)" );
  histos1D_[ "hitFitLepPt" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepEta" ] = fileService->make< TH1D >( "hitFitLepEta", "HitFit lepton pseudo-rapidity", binsLepEta_, -maxLepEta_, maxLepEta_ );
  histos1D_[ "hitFitLepEta" ]->SetXTitle( "#eta_{l}^{HitFit}" );
  histos1D_[ "hitFitLepEta" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepPhi" ] = fileService->make< TH1D >( "hitFitLepPhi", "HitFit lepton azimutal angle", binsLepPhi_, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "hitFitLepPhi" ]->SetXTitle( "#phi_{l}^{HitFit}" );
  histos1D_[ "hitFitLepPhi" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepPz" ] = fileService->make< TH1D >( "hitFitLepPz", "HitFit lepton longitudinal momentum", binsLepPz_, -maxLepPz_, maxLepPz_ );
  histos1D_[ "hitFitLepPz" ]->SetXTitle( "p_{z, l}^{HitFit} (GeV)" );
  histos1D_[ "hitFitLepPz" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuM" ] = fileService->make< TH1D >( "hitFitNuM", "HitFit neutrino mass", binsNuM_, -maxNuM_, maxNuM_ );
  histos1D_[ "hitFitNuM" ]->SetXTitle( "m_{#nu}^{HitFit} (GeV)" );
  histos1D_[ "hitFitNuM" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuPt" ] = fileService->make< TH1D >( "hitFitNuPt", "HitFit neutrino transverse momentum", binsNuPt_, 0., maxNuPt_ );
  histos1D_[ "hitFitNuPt" ]->SetXTitle( "p_{t, #nu}^{HitFit} (GeV)" );
  histos1D_[ "hitFitNuPt" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuEta" ] = fileService->make< TH1D >( "hitFitNuEta", "HitFit neutrino pseudo-rapidity", binsNuEta_, -maxNuEta_, maxNuEta_ );
  histos1D_[ "hitFitNuEta" ]->SetXTitle( "#eta_{#nu}^{HitFit}" );
  histos1D_[ "hitFitNuEta" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuPhi" ] = fileService->make< TH1D >( "hitFitNuPhi", "HitFit neutrino azimutal angle", binsNuPhi_, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "hitFitNuPhi" ]->SetXTitle( "#phi_{#nu}^{HitFit}" );
  histos1D_[ "hitFitNuPhi" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuPz" ] = fileService->make< TH1D >( "hitFitNuPz", "HitFit neutrino longitudinal momentum", binsNuPz_, -maxNuPz_, maxNuPz_ );
  histos1D_[ "hitFitNuPz" ]->SetXTitle( "p_{z, #nu}^{HitFit} (GeV)" );
  histos1D_[ "hitFitNuPz" ]->SetYTitle( "events" );
  // 2-dim
  histos2D_[ "hitFitProb_hitFitMT" ] = fileService->make< TH2D >( "hitFitProb_hitFitMT", "HitFit top mass vs. HitFit probabilty", binsHitFitProb_, 0., 1., binsHitFitMT_, minHitFitMT_, maxHitFitMT_ );
  histos2D_[ "hitFitProb_hitFitMT" ]->SetXTitle( "P^{HitFit}" );
  histos2D_[ "hitFitProb_hitFitMT" ]->SetYTitle( "m_{top}^{HitFit} (GeV)" );
  histos2D_[ "hitFitProb_hitFitMT" ]->SetZTitle( "events" );
  histos2D_[ "hitFitProb_hitFitSigMT" ] = fileService->make< TH2D >( "hitFitProb_hitFitSigMT", "HitFit top mass uncertainty vs. HitFit probabilty", binsHitFitProb_, 0., 1., binsHitFitSigMT_, 0., maxHitFitSigMT_ );
  histos2D_[ "hitFitProb_hitFitSigMT" ]->SetXTitle( "P^{HitFit}" );
  histos2D_[ "hitFitProb_hitFitSigMT" ]->SetYTitle( "#sigma_{m_{t}}^{HitFit} (GeV)" );
  histos2D_[ "hitFitProb_hitFitSigMT" ]->SetZTitle( "events" );
  histos2D_[ "hitFitProb_hitFitSigMTRel" ] = fileService->make< TH2D >( "hitFitProb_hitFitSigMTRel", "Relative HitFit top mass uncertainty vs. HitFit probabilty", binsHitFitProb_, 0., 1., binsHitFitSigMTRel_, 0., maxHitFitSigMTRel_ );
  histos2D_[ "hitFitProb_hitFitSigMTRel" ]->SetXTitle( "P^{HitFit}" );
  histos2D_[ "hitFitProb_hitFitSigMTRel" ]->SetYTitle( "#frac{#sigma_{m_{t}}^{HitFit}}{m_{t}^{HitFit}}" );
  histos2D_[ "hitFitProb_hitFitSigMTRel" ]->SetZTitle( "events" );
  histos2D_[ "hitFitMT_hitFitSigMT" ] = fileService->make< TH2D >( "hitFitMT_hitFitSigMT", "HitFit top mass uncertainty vs. HitFit top mass", binsHitFitMT_, minHitFitMT_, maxHitFitMT_, binsHitFitSigMT_, 0., maxHitFitSigMT_ );
  histos2D_[ "hitFitMT_hitFitSigMT" ]->SetXTitle( "m_{t}^{HitFit} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMT" ]->SetYTitle( "#sigma_{m_{t}}^{HitFit} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMT" ]->SetZTitle( "events" );
  histos2D_[ "hitFitMT_hitFitSigMTRel" ] = fileService->make< TH2D >( "hitFitMT_hitFitSigMTRel", "Relative HitFit top mass uncertainty vs. HitFit top mass", binsHitFitMT_, minHitFitMT_, maxHitFitMT_, binsHitFitSigMTRel_, 0., maxHitFitSigMTRel_ );
  histos2D_[ "hitFitMT_hitFitSigMTRel" ]->SetXTitle( "m_{t}^{HitFit} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMTRel" ]->SetYTitle( "#frac{#sigma_{m_{t}}^{HitFit}}{m_{t}^{HitFit}}" );
  histos2D_[ "hitFitMT_hitFitSigMTRel" ]->SetZTitle( "events" );

  // MC specific (using true information)
  // 1-dim
  histos1D_[ "genDecayChn" ] = fileService->make< TH1D >( "genDecayChn", "Decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1. );
  histos1D_[ "genDecayChn" ]->SetXTitle( "" );
  assignDecayChnNames( histos1D_[ "genDecayChn" ]->GetXaxis() );
  histos1D_[ "genDecayChn" ]->SetYTitle( "events" );
  histos1D_[ "genTopLepM" ] = fileService->make< TH1D >( "genTopLepM", "HitFit muonic top mass", binsTopM_, minTopM_, maxTopM_ );
  histos1D_[ "genTopLepM" ]->SetXTitle( "m_{t_{l}}^{gen.} (GeV)" );
  histos1D_[ "genTopLepM" ]->SetYTitle( "events" );
  histos1D_[ "genTopHadM" ] = fileService->make< TH1D >( "genTopHadM", "HitFit hadronic top mass", binsTopM_, minTopM_, maxTopM_ );
  histos1D_[ "genTopHadM" ]->SetXTitle( "m_{t_{h}}^{gen.} (GeV)" );
  histos1D_[ "genTopHadM" ]->SetYTitle( "events" );
  histos1D_[ "genLepM" ] = fileService->make< TH1D >( "genLepM", "Generated lepton mass", binsGenLepM_, 0., maxGenLepM_ );
  histos1D_[ "genLepM" ]->SetXTitle( "m_{l}^{gen.} (GeV)" );
  histos1D_[ "genLepM" ]->SetYTitle( "events" );
  histos1D_[ "genLepPt" ] = fileService->make< TH1D >( "genLepPt", "Generated lepton transverse momentum", binsLepPt_, 0., maxLepPt_ );
  histos1D_[ "genLepPt" ]->SetXTitle( "p_{t, l}^{gen.} (GeV)" );
  histos1D_[ "genLepPt" ]->SetYTitle( "events" );
  histos1D_[ "genLepEta" ] = fileService->make< TH1D >( "genLepEta", "Generated lepton pseudo-rapidity", binsLepEta_, -maxLepEta_, maxLepEta_ );
  histos1D_[ "genLepEta" ]->SetXTitle( "#eta_{l}^{gen.}" );
  histos1D_[ "genLepEta" ]->SetYTitle( "events" );
  histos1D_[ "genLepPhi" ] = fileService->make< TH1D >( "genLepPhi", "Generated lepton azimutal angle", binsLepPhi_, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "genLepPhi" ]->SetXTitle( "#phi_{l}^{gen.}" );
  histos1D_[ "genLepPhi" ]->SetYTitle( "events" );
  histos1D_[ "genLepPz" ] = fileService->make< TH1D >( "genLepPz", "Generated lepton longitudinal momentum", binsLepPz_, -maxLepPz_, maxLepPz_ );
  histos1D_[ "genLepPz" ]->SetXTitle( "p_{z, l}^{gen.} (GeV)" );
  histos1D_[ "genLepPz" ]->SetYTitle( "events" );
  histos1D_[ "genNuM" ] = fileService->make< TH1D >( "genNuM", "Generated neutrino mass", binsNuM_, -maxNuM_, maxNuM_ );
  histos1D_[ "genNuM" ]->SetXTitle( "m_{#nu}^{gen.} (GeV)" );
  histos1D_[ "genNuM" ]->SetYTitle( "events" );
  histos1D_[ "genNuPt" ] = fileService->make< TH1D >( "genNuPt", "Generated neutrino transverse momentum", binsNuPt_, 0., maxNuPt_ );
  histos1D_[ "genNuPt" ]->SetXTitle( "p_{t, #nu}^{gen.} (GeV)" );
  histos1D_[ "genNuPt" ]->SetYTitle( "events" );
  histos1D_[ "genNuEta" ] = fileService->make< TH1D >( "genNuEta", "Generated neutrino pseudo-rapidity", binsNuEta_, -maxNuEta_, maxNuEta_ );
  histos1D_[ "genNuEta" ]->SetXTitle( "#eta_{#nu}^{gen.}" );
  histos1D_[ "genNuEta" ]->SetYTitle( "events" );
  histos1D_[ "genNuPhi" ] = fileService->make< TH1D >( "genNuPhi", "Generated neutrino azimutal angle", binsNuPhi_, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "genNuPhi" ]->SetXTitle( "#phi_{#nu}^{gen.}" );
  histos1D_[ "genNuPhi" ]->SetYTitle( "events" );
  histos1D_[ "genNuPz" ] = fileService->make< TH1D >( "genNuPz", "Generated neutrino longitudinal momentum", binsNuPz_, -maxNuPz_, maxNuPz_ );
  histos1D_[ "genNuPz" ]->SetXTitle( "p_{z, #nu}^{gen.} (GeV)" );
  histos1D_[ "genNuPz" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenTopLepM" ] = fileService->make< TH1D >( "hitFitDiffGenTopLepM", "HitFit leptonic top mass deviation", binsDiffTopM_, -maxDiffTopM_, maxDiffTopM_ );
  histos1D_[ "hitFitDiffGenTopLepM" ]->SetXTitle( "#Delta m_{t_{l}} (GeV)" );
  histos1D_[ "hitFitDiffGenTopLepM" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenTopHadM" ] = fileService->make< TH1D >( "hitFitDiffGenTopHadM", "HitFit hadronic top mass deviation", binsDiffTopM_, -maxDiffTopM_, maxDiffTopM_ );
  histos1D_[ "hitFitDiffGenTopHadM" ]->SetXTitle( "#Delta m_{t_{h}} (GeV)" );
  histos1D_[ "hitFitDiffGenTopHadM" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenLepM" ] = fileService->make< TH1D >( "hitFitDiffGenLepM", "HitFit lepton mass deviation", binsDiffLepM_, -maxDiffLepM_, maxDiffLepM_ );
  histos1D_[ "hitFitDiffGenLepM" ]->SetXTitle( "#Delta m_{l} (GeV)" );
  histos1D_[ "hitFitDiffGenLepM" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenLepPt" ] = fileService->make< TH1D >( "hitFitDiffGenLepPt", "HitFit lepton transverse momentum deviation", binsDiffLepPt_, -maxDiffLepPt_, maxDiffLepPt_ );
  histos1D_[ "hitFitDiffGenLepPt" ]->SetXTitle( "#Delta p_{t, l} (GeV)" );
  histos1D_[ "hitFitDiffGenLepPt" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenLepEta" ] = fileService->make< TH1D >( "hitFitDiffGenLepEta", "HitFit lepton pseudo-rapidity deviation", binsDiffLepEta_, -maxDiffLepEta_, maxDiffLepEta_ );
  histos1D_[ "hitFitDiffGenLepEta" ]->SetXTitle( "#Delta #eta_{l}" );
  histos1D_[ "hitFitDiffGenLepEta" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenLepPhi" ] = fileService->make< TH1D >( "hitFitDiffGenLepPhi", "HitFit lepton azimutal angle deviation", binsDiffLepPhi_, -maxDiffLepPhi_, maxDiffLepPhi_ );
  histos1D_[ "hitFitDiffGenLepPhi" ]->SetXTitle( "#Delta #phi_{l}" );
  histos1D_[ "hitFitDiffGenLepPhi" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenLepPz" ] = fileService->make< TH1D >( "hitFitDiffGenLepPz", "HitFit lepton longitudinal momentum deviation", binsDiffLepPz_, -binsDiffLepPz_, binsDiffLepPz_ );
  histos1D_[ "hitFitDiffGenLepPz" ]->SetXTitle( "#Delta p_{z, l} (GeV)" );
  histos1D_[ "hitFitDiffGenLepPz" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenNuM" ] = fileService->make< TH1D >( "hitFitDiffGenNuM", "HitFit neutrino mass deviation", binsDiffNuM_, -maxDiffNuM_, maxDiffNuM_ );
  histos1D_[ "hitFitDiffGenNuM" ]->SetXTitle( "#Delta m_{#nu} (GeV)" );
  histos1D_[ "hitFitDiffGenNuM" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenNuPt" ] = fileService->make< TH1D >( "hitFitDiffGenNuPt", "HitFit neutrino transverse momentum deviation", binsDiffNuPt_, -maxDiffNuPt_, maxDiffNuPt_ );
  histos1D_[ "hitFitDiffGenNuPt" ]->SetXTitle( "#Delta p_{t, #nu} (GeV)" );
  histos1D_[ "hitFitDiffGenNuPt" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenNuEta" ] = fileService->make< TH1D >( "hitFitDiffGenNuEta", "HitFit neutrino pseudo-rapidity deviation", binsDiffNuEta_, -maxDiffNuEta_, maxDiffNuEta_ );
  histos1D_[ "hitFitDiffGenNuEta" ]->SetXTitle( "#Delta #eta_{#nu}" );
  histos1D_[ "hitFitDiffGenNuEta" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenNuPhi" ] = fileService->make< TH1D >( "hitFitDiffGenNuPhi", "HitFit neutrino azimutal angle deviation", binsDiffNuPhi_, -maxDiffNuPhi_, maxDiffNuPhi_ );
  histos1D_[ "hitFitDiffGenNuPhi" ]->SetXTitle( "#Delta #phi_{#nu}" );
  histos1D_[ "hitFitDiffGenNuPhi" ]->SetYTitle( "events" );
  histos1D_[ "hitFitDiffGenNuPz" ] = fileService->make< TH1D >( "hitFitDiffGenNuPz", "HitFit neutrino longitudinal momentum deviation", binsDiffNuPz_, -maxDiffNuPz_, maxDiffNuPz_ );
  histos1D_[ "hitFitDiffGenNuPz" ]->SetXTitle( "#Delta p_{z, #nu} (GeV)" );
  histos1D_[ "hitFitDiffGenNuPz" ]->SetYTitle( "events" );
  histos1D_[ "nRealNuSolSig" ] = fileService->make< TH1D >( "nRealNuSolSig", "HitFit real #nu solutions", 4, -1.5, 2.5 );
  histos1D_[ "nRealNuSolSig" ]->SetXTitle( "real #nu sols." );
  histos1D_[ "nRealNuSolSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitProbSig" ] = fileService->make< TH1D >( "hitFitProbSig", "HitFit probabilty", binsHitFitProb_, 0., 1. );
  histos1D_[ "hitFitProbSig" ]->SetXTitle( "P^{HitFit}" );
  histos1D_[ "hitFitProbSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitProbLowSig" ] = fileService->make< TH1D >( "hitFitProbLowSig", "HitFit probabilty", binsHitFitProbLow_, 0., maxHitFitProbLow_ );
  histos1D_[ "hitFitProbLowSig" ]->SetXTitle( "P^{HitFit}" );
  histos1D_[ "hitFitProbLowSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitProbLogSig" ] = fileService->make< TH1D >( "hitFitProbLogSig", "HitFit probabilty", binsHitFitProbLog_, minHitFitProbLog_, 0. );
  histos1D_[ "hitFitProbLogSig" ]->SetXTitle( "log_{10} P" );
  histos1D_[ "hitFitProbLogSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitChi2Sig" ] = fileService->make< TH1D >( "hitFitChi2Sig", "HitFit #chi^{2}", binsHitFitChi2_, 0., maxHitFitChi2_ );
  histos1D_[ "hitFitChi2Sig" ]->SetXTitle( "(#chi^{2})^{HitFit}" );
  histos1D_[ "hitFitChi2Sig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitMTSig" ] = fileService->make< TH1D >( "hitFitMTSig", "HitFit top mass", binsHitFitMT_, minHitFitMT_, maxHitFitMT_ );
  histos1D_[ "hitFitMTSig" ]->SetXTitle( "m_{top}^{HitFit} (GeV)" );
  histos1D_[ "hitFitMTSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitSigMTSig" ] = fileService->make< TH1D >( "hitFitSigMTSig", "HitFit top mass uncertainty", binsHitFitSigMT_, 0., maxHitFitSigMT_ );
  histos1D_[ "hitFitSigMTSig" ]->SetXTitle( "#sigma_{m_{top}}^{HitFit} (GeV)" );
  histos1D_[ "hitFitSigMTSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitSigMTRelSig" ] = fileService->make< TH1D >( "hitFitSigMTRelSig", "Relative HitFit top mass uncertainty", binsHitFitSigMTRel_, 0., maxHitFitSigMTRel_ );
  histos1D_[ "hitFitSigMTRelSig" ]->SetXTitle( "#sigma_{m_{top}}^{HitFit} (GeV)" );
  histos1D_[ "hitFitSigMTRelSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepMSig" ] = fileService->make< TH1D >( "hitFitLepMSig", "HitFit lepton mass", binsLepM_, -maxLepM_, maxLepM_ );
  histos1D_[ "hitFitLepMSig" ]->SetXTitle( "m_{l}^{HitFit} (GeV)" );
  histos1D_[ "hitFitLepMSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepPtSig" ] = fileService->make< TH1D >( "hitFitLepPtSig", "HitFit lepton transverse momentum", binsLepPt_, 0., maxLepPt_ );
  histos1D_[ "hitFitLepPtSig" ]->SetXTitle( "p_{t, l}^{HitFit} (GeV)" );
  histos1D_[ "hitFitLepPtSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepEtaSig" ] = fileService->make< TH1D >( "hitFitLepEtaSig", "HitFit lepton pseudo-rapidity", binsLepEta_, -maxLepEta_, maxLepEta_ );
  histos1D_[ "hitFitLepEtaSig" ]->SetXTitle( "#eta_{l}^{HitFit}" );
  histos1D_[ "hitFitLepEtaSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepPhiSig" ] = fileService->make< TH1D >( "hitFitLepPhiSig", "HitFit lepton azimutal angle", binsLepPhi_, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "hitFitLepPhiSig" ]->SetXTitle( "#phi_{l}^{HitFit}" );
  histos1D_[ "hitFitLepPhiSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepPzSig" ] = fileService->make< TH1D >( "hitFitLepPzSig", "HitFit lepton longitudinal momentum", binsLepPz_, -maxLepPz_, maxLepPz_ );
  histos1D_[ "hitFitLepPzSig" ]->SetXTitle( "p_{z, l}^{HitFit} (GeV)" );
  histos1D_[ "hitFitLepPzSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuMSig" ] = fileService->make< TH1D >( "hitFitNuMSig", "HitFit neutrino mass", binsNuM_, -maxNuM_, maxNuM_ );
  histos1D_[ "hitFitNuMSig" ]->SetXTitle( "m_{#nu}^{HitFit} (GeV)" );
  histos1D_[ "hitFitNuMSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuPtSig" ] = fileService->make< TH1D >( "hitFitNuPtSig", "HitFit neutrino transverse momentum", binsNuPt_, 0., maxNuPt_ );
  histos1D_[ "hitFitNuPtSig" ]->SetXTitle( "p_{t, #nu}^{HitFit} (GeV)" );
  histos1D_[ "hitFitNuPtSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuEtaSig" ] = fileService->make< TH1D >( "hitFitNuEtaSig", "HitFit neutrino pseudo-rapidity", binsNuEta_, -maxNuEta_, maxNuEta_ );
  histos1D_[ "hitFitNuEtaSig" ]->SetXTitle( "#eta_{#nu}^{HitFit}" );
  histos1D_[ "hitFitNuEtaSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuPhiSig" ] = fileService->make< TH1D >( "hitFitNuPhiSig", "HitFit neutrino azimutal angle", binsNuPhi_, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "hitFitNuPhiSig" ]->SetXTitle( "#phi_{#nu}^{HitFit}" );
  histos1D_[ "hitFitNuPhiSig" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuPzSig" ] = fileService->make< TH1D >( "hitFitNuPzSig", "HitFit neutrino longitudinal momentum", binsNuPz_, -maxNuPz_, maxNuPz_ );
  histos1D_[ "hitFitNuPzSig" ]->SetXTitle( "p_{z, #nu}^{HitFit} (GeV)" );
  histos1D_[ "hitFitNuPzSig" ]->SetYTitle( "events" );
  histos1D_[ "nRealNuSolBkg" ] = fileService->make< TH1D >( "nRealNuSolBkg", "HitFit real #nu solutions", 4, -1.5, 2.5 );
  histos1D_[ "nRealNuSolBkg" ]->SetXTitle( "real #nu sols." );
  histos1D_[ "nRealNuSolBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitProbBkg" ] = fileService->make< TH1D >( "hitFitProbBkg", "HitFit probabilty", binsHitFitProb_, 0., 1. );
  histos1D_[ "hitFitProbBkg" ]->SetXTitle( "P^{HitFit}" );
  histos1D_[ "hitFitProbBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitProbLowBkg" ] = fileService->make< TH1D >( "hitFitProbLowBkg", "HitFit probabilty", binsHitFitProbLow_, 0., maxHitFitProbLow_ );
  histos1D_[ "hitFitProbLowBkg" ]->SetXTitle( "P^{HitFit}" );
  histos1D_[ "hitFitProbLowBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitProbLogBkg" ] = fileService->make< TH1D >( "hitFitProbLogBkg", "HitFit probabilty", binsHitFitProbLog_, minHitFitProbLog_, 0. );
  histos1D_[ "hitFitProbLogBkg" ]->SetXTitle( "log_{10} P" );
  histos1D_[ "hitFitProbLogBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitChi2Bkg" ] = fileService->make< TH1D >( "hitFitChi2Bkg", "HitFit #chi^{2}", binsHitFitChi2_, 0., maxHitFitChi2_ );
  histos1D_[ "hitFitChi2Bkg" ]->SetXTitle( "(#chi^{2})^{HitFit}" );
  histos1D_[ "hitFitChi2Bkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitMTBkg" ] = fileService->make< TH1D >( "hitFitMTBkg", "HitFit top mass", binsHitFitMT_, minHitFitMT_, maxHitFitMT_ );
  histos1D_[ "hitFitMTBkg" ]->SetXTitle( "m_{top}^{HitFit} (GeV)" );
  histos1D_[ "hitFitMTBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitSigMTBkg" ] = fileService->make< TH1D >( "hitFitSigMTBkg", "HitFit top mass uncertainty", binsHitFitSigMT_, 0., maxHitFitSigMT_ );
  histos1D_[ "hitFitSigMTBkg" ]->SetXTitle( "#sigma_{m_{top}}^{HitFit} (GeV)" );
  histos1D_[ "hitFitSigMTBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitSigMTRelBkg" ] = fileService->make< TH1D >( "hitFitSigMTRelBkg", "Relative HitFit top mass uncertainty", binsHitFitSigMTRel_, 0., maxHitFitSigMTRel_ );
  histos1D_[ "hitFitSigMTRelBkg" ]->SetXTitle( "#sigma_{m_{top}}^{HitFit} (GeV)" );
  histos1D_[ "hitFitSigMTRelBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepMBkg" ] = fileService->make< TH1D >( "hitFitLepMBkg", "HitFit lepton mass", binsLepM_, -maxLepM_, maxLepM_ );
  histos1D_[ "hitFitLepMBkg" ]->SetXTitle( "m_{l}^{HitFit} (GeV)" );
  histos1D_[ "hitFitLepMBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepPtBkg" ] = fileService->make< TH1D >( "hitFitLepPtBkg", "HitFit lepton transverse momentum", binsLepPt_, 0., maxLepPt_ );
  histos1D_[ "hitFitLepPtBkg" ]->SetXTitle( "p_{t, l}^{HitFit} (GeV)" );
  histos1D_[ "hitFitLepPtBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepEtaBkg" ] = fileService->make< TH1D >( "hitFitLepEtaBkg", "HitFit lepton pseudo-rapidity", binsLepEta_, -maxLepEta_, maxLepEta_ );
  histos1D_[ "hitFitLepEtaBkg" ]->SetXTitle( "#eta_{l}^{HitFit}" );
  histos1D_[ "hitFitLepEtaBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepPhiBkg" ] = fileService->make< TH1D >( "hitFitLepPhiBkg", "HitFit lepton azimutal angle", binsLepPhi_, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "hitFitLepPhiBkg" ]->SetXTitle( "#phi_{l}^{HitFit}" );
  histos1D_[ "hitFitLepPhiBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitLepPzBkg" ] = fileService->make< TH1D >( "hitFitLepPzBkg", "HitFit lepton longitudinal momentum", binsLepPz_, -maxLepPz_, maxLepPz_ );
  histos1D_[ "hitFitLepPzBkg" ]->SetXTitle( "p_{z, l}^{HitFit} (GeV)" );
  histos1D_[ "hitFitLepPzBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuMBkg" ] = fileService->make< TH1D >( "hitFitNuMBkg", "HitFit neutrino mass", binsNuM_, -maxNuM_, maxNuM_ );
  histos1D_[ "hitFitNuMBkg" ]->SetXTitle( "m_{#nu}^{HitFit} (GeV)" );
  histos1D_[ "hitFitNuMBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuPtBkg" ] = fileService->make< TH1D >( "hitFitNuPtBkg", "HitFit neutrino transverse momentum", binsNuPt_, 0., maxNuPt_ );
  histos1D_[ "hitFitNuPtBkg" ]->SetXTitle( "p_{t, #nu}^{HitFit} (GeV)" );
  histos1D_[ "hitFitNuPtBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuEtaBkg" ] = fileService->make< TH1D >( "hitFitNuEtaBkg", "HitFit neutrino pseudo-rapidity", binsNuEta_, -maxNuEta_, maxNuEta_ );
  histos1D_[ "hitFitNuEtaBkg" ]->SetXTitle( "#eta_{#nu}^{HitFit}" );
  histos1D_[ "hitFitNuEtaBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuPhiBkg" ] = fileService->make< TH1D >( "hitFitNuPhiBkg", "HitFit neutrino azimutal angle", binsNuPhi_, -TMath::Pi(), TMath::Pi() );
  histos1D_[ "hitFitNuPhiBkg" ]->SetXTitle( "#phi_{#nu}^{HitFit}" );
  histos1D_[ "hitFitNuPhiBkg" ]->SetYTitle( "events" );
  histos1D_[ "hitFitNuPzBkg" ] = fileService->make< TH1D >( "hitFitNuPzBkg", "HitFit neutrino longitudinal momentum", binsNuPz_, -maxNuPz_, maxNuPz_ );
  histos1D_[ "hitFitNuPzBkg" ]->SetXTitle( "p_{z, #nu}^{HitFit} (GeV)" );
  histos1D_[ "hitFitNuPzBkg" ]->SetYTitle( "events" );
  // 2-dim
  histos2D_[ "hitFitMT_hitFitSigMTSig" ] = fileService->make< TH2D >( "hitFitMT_hitFitSigMTSig", "HitFit top mass uncertainty vs. HitFit top mass", binsHitFitMT_, minHitFitMT_, maxHitFitMT_, binsHitFitSigMT_, 0., maxHitFitSigMT_ );
  histos2D_[ "hitFitMT_hitFitSigMTSig" ]->SetXTitle( "m_{t}^{HitFit} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMTSig" ]->SetYTitle( "#sigma_{m_{t}}^{HitFit} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMTSig" ]->SetZTitle( "events" );
  histos2D_[ "hitFitMT_hitFitSigMTRelSig" ] = fileService->make< TH2D >( "hitFitMT_hitFitSigMTRelSig", "Relative HitFit top mass uncertainty vs. HitFit top mass", binsHitFitMT_, minHitFitMT_, maxHitFitMT_, binsHitFitSigMTRel_, 0., maxHitFitSigMTRel_ );
  histos2D_[ "hitFitMT_hitFitSigMTRelSig" ]->SetXTitle( "m_{t}^{HitFit} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMTRelSig" ]->SetYTitle( "#frac{#sigma_{m_{t}}^{HitFit}}{m_{t}^{HitFit}}" );
  histos2D_[ "hitFitMT_hitFitSigMTRelSig" ]->SetZTitle( "events" );
  histos2D_[ "hitFitMT_hitFitSigMTBkg" ] = fileService->make< TH2D >( "hitFitMT_hitFitSigMTBkg", "HitFit top mass uncertainty vs. HitFit top mass", binsHitFitMT_, minHitFitMT_, maxHitFitMT_, binsHitFitSigMT_, 0., maxHitFitSigMT_ );
  histos2D_[ "hitFitMT_hitFitSigMTBkg" ]->SetXTitle( "m_{t}^{HitFit} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMTBkg" ]->SetYTitle( "#sigma_{m_{t}}^{HitFit} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMTBkg" ]->SetZTitle( "events" );
  histos2D_[ "hitFitMT_hitFitSigMTRelBkg" ] = fileService->make< TH2D >( "hitFitMT_hitFitSigMTRelBkg", "Relative HitFit top mass uncertainty vs. HitFit top mass", binsHitFitMT_, minHitFitMT_, maxHitFitMT_, binsHitFitSigMTRel_, 0., maxHitFitSigMTRel_ );
  histos2D_[ "hitFitMT_hitFitSigMTRelBkg" ]->SetXTitle( "m_{t}^{HitFit} (GeV)" );
  histos2D_[ "hitFitMT_hitFitSigMTRelBkg" ]->SetYTitle( "#frac{#sigma_{m_{t}}^{HitFit}}{m_{t}^{HitFit}}" );
  histos2D_[ "hitFitMT_hitFitSigMTRelBkg" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_nRealNuSol" ] = fileService->make< TH2D >( "genDecayChn_nRealNuSol", "HitFit real #nu solutions vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., 4, -1.5, 2.5 );
  histos2D_[ "genDecayChn_nRealNuSol" ]->SetXTitle( "" );
  assignDecayChnNames( histos2D_[ "genDecayChn_nRealNuSol" ]->GetXaxis() );
  histos2D_[ "genDecayChn_nRealNuSol" ]->SetYTitle( "real #nu sols." );
  histos2D_[ "genDecayChn_nRealNuSol" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitProb" ] = fileService->make< TH2D >( "genDecayChn_hitFitProb", "HitFit probabilty vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsHitFitProb_, 0., 1. );
  histos2D_[ "genDecayChn_hitFitProb" ]->SetXTitle( "" );
  assignDecayChnNames( histos2D_[ "genDecayChn_hitFitProb" ]->GetXaxis() );
  histos2D_[ "genDecayChn_hitFitProb" ]->SetYTitle( "P^{HitFit}" );
  histos2D_[ "genDecayChn_hitFitProb" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitProbLow" ] = fileService->make< TH2D >( "genDecayChn_hitFitProbLow", "HitFit probabilty vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsHitFitProbLow_, 0., maxHitFitProbLow_ );
  histos2D_[ "genDecayChn_hitFitProbLow" ]->SetXTitle( "" );
  assignDecayChnNames( histos2D_[ "genDecayChn_hitFitProbLow" ]->GetXaxis() );
  histos2D_[ "genDecayChn_hitFitProbLow" ]->SetYTitle( "P^{HitFit}" );
  histos2D_[ "genDecayChn_hitFitProbLow" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitProbLog" ] = fileService->make< TH2D >( "genDecayChn_hitFitProbLog", "HitFit probabilty vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsHitFitProbLog_, minHitFitProbLog_, 0. );
  histos2D_[ "genDecayChn_hitFitProbLog" ]->SetXTitle( "" );
  assignDecayChnNames( histos2D_[ "genDecayChn_hitFitProbLog" ]->GetXaxis() );
  histos2D_[ "genDecayChn_hitFitProbLog" ]->SetYTitle( "log_{10} P" );
  histos2D_[ "genDecayChn_hitFitProbLog" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitChi2" ] = fileService->make< TH2D >( "genDecayChn_hitFitChi2", "HitFit #chi^{2} vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsHitFitChi2_, 0., maxHitFitChi2_ );
  histos2D_[ "genDecayChn_hitFitChi2" ]->SetXTitle( "" );
  assignDecayChnNames( histos2D_[ "genDecayChn_hitFitChi2" ]->GetXaxis() );
  histos2D_[ "genDecayChn_hitFitChi2" ]->SetYTitle( "(#chi^{2})^{HitFit}" );
  histos2D_[ "genDecayChn_hitFitChi2" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitMT" ] = fileService->make< TH2D >( "genDecayChn_hitFitMT", "HitFit top mass vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsHitFitMT_, minHitFitMT_, maxHitFitMT_ );
  histos2D_[ "genDecayChn_hitFitMT" ]->SetXTitle( "" );
  assignDecayChnNames( histos2D_[ "genDecayChn_hitFitMT" ]->GetXaxis() );
  histos2D_[ "genDecayChn_hitFitMT" ]->SetYTitle( "m_{top}^{HitFit} (GeV)" );
  histos2D_[ "genDecayChn_hitFitMT" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitSigMT" ] = fileService->make< TH2D >( "genDecayChn_hitFitSigMT", "HitFit top mass uncertainty vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsHitFitSigMT_, 0., maxHitFitSigMT_ );
  histos2D_[ "genDecayChn_hitFitSigMT" ]->SetXTitle( "" );
  assignDecayChnNames( histos2D_[ "genDecayChn_hitFitSigMT" ]->GetXaxis() );
  histos2D_[ "genDecayChn_hitFitSigMT" ]->SetYTitle( "#sigma_{m_{top}}^{HitFit} (GeV)" );
  histos2D_[ "genDecayChn_hitFitSigMT" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitSigMTRel" ] = fileService->make< TH2D >( "genDecayChn_hitFitSigMTRel", "Relative HitFit top mass uncertainty vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsHitFitSigMTRel_, 0., maxHitFitSigMTRel_ );
  histos2D_[ "genDecayChn_hitFitSigMTRel" ]->SetXTitle( "" );
  assignDecayChnNames( histos2D_[ "genDecayChn_hitFitSigMTRel" ]->GetXaxis() );
  histos2D_[ "genDecayChn_hitFitSigMTRel" ]->SetYTitle( "#frac{#sigma_{m_{t}}^{HitFit}}{m_{t}^{HitFit}}" );
  histos2D_[ "genDecayChn_hitFitSigMTRel" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitLepM" ] = fileService->make< TH2D >( "genDecayChn_hitFitLepM", "HitFit lepton mass vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsLepM_, -maxLepM_, maxLepM_ );
  histos2D_[ "genDecayChn_hitFitLepM" ]->SetXTitle( "" );
  assignDecayChnNames( histos2D_[ "genDecayChn_hitFitLepM" ]->GetXaxis() );
  histos2D_[ "genDecayChn_hitFitLepM" ]->SetYTitle( "m_{l}^{HitFit} (GeV)" );
  histos2D_[ "genDecayChn_hitFitLepM" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitLepPt" ] = fileService->make< TH2D >( "genDecayChn_hitFitLepPt", "HitFit lepton transverse momentum vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsLepPt_, 0., maxLepPt_ );
  histos2D_[ "genDecayChn_hitFitLepPt" ]->SetXTitle( "" );
  assignDecayChnNames( histos2D_[ "genDecayChn_hitFitLepPt" ]->GetXaxis() );
  histos2D_[ "genDecayChn_hitFitLepPt" ]->SetYTitle( "p_{t, l}^{HitFit} (GeV)" );
  histos2D_[ "genDecayChn_hitFitLepPt" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitLepEta" ] = fileService->make< TH2D >( "genDecayChn_hitFitLepEta", "HitFit lepton pseudo-rapidity vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsLepEta_, -maxLepEta_, maxLepEta_ );
  histos2D_[ "genDecayChn_hitFitLepEta" ]->SetXTitle( "" );
  assignDecayChnNames( histos2D_[ "genDecayChn_hitFitLepEta" ]->GetXaxis() );
  histos2D_[ "genDecayChn_hitFitLepEta" ]->SetYTitle( "#eta_{l}^{HitFit}" );
  histos2D_[ "genDecayChn_hitFitLepEta" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitLepPhi" ] = fileService->make< TH2D >( "genDecayChn_hitFitLepPhi", "HitFit lepton azimutal angle vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsLepPhi_, -TMath::Pi(), TMath::Pi() );
  histos2D_[ "genDecayChn_hitFitLepPhi" ]->SetXTitle( "" );
  assignDecayChnNames( histos2D_[ "genDecayChn_hitFitLepPhi" ]->GetXaxis() );
  histos2D_[ "genDecayChn_hitFitLepPhi" ]->SetYTitle( "#phi_{l}^{HitFit}" );
  histos2D_[ "genDecayChn_hitFitLepPhi" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitLepPz" ] = fileService->make< TH2D >( "genDecayChn_hitFitLepPz", "HitFit lepton longitudinal momentum vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsLepPz_, -maxLepPz_, maxLepPz_ );
  histos2D_[ "genDecayChn_hitFitLepPz" ]->SetXTitle( "" );
  assignDecayChnNames( histos2D_[ "genDecayChn_hitFitLepPz" ]->GetXaxis() );
  histos2D_[ "genDecayChn_hitFitLepPz" ]->SetYTitle( "p_{z, l}^{HitFit} (GeV)" );
  histos2D_[ "genDecayChn_hitFitLepPz" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitNuM" ] = fileService->make< TH2D >( "genDecayChn_hitFitNuM", "HitFit neutrino mass vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsNuM_, -maxNuM_, maxNuM_ );
  histos2D_[ "genDecayChn_hitFitNuM" ]->SetXTitle( "" );
  assignDecayChnNames( histos2D_[ "genDecayChn_hitFitNuM" ]->GetXaxis() );
  histos2D_[ "genDecayChn_hitFitNuM" ]->SetYTitle( "m_{#nu}^{HitFit} (GeV)" );
  histos2D_[ "genDecayChn_hitFitNuM" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitNuPt" ] = fileService->make< TH2D >( "genDecayChn_hitFitNuPt", "HitFit neutrino transverse momentum vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsNuPt_, 0., maxNuPt_ );
  histos2D_[ "genDecayChn_hitFitNuPt" ]->SetXTitle( "" );
  assignDecayChnNames( histos2D_[ "genDecayChn_hitFitNuPt" ]->GetXaxis() );
  histos2D_[ "genDecayChn_hitFitNuPt" ]->SetYTitle( "p_{t, #nu}^{HitFit} (GeV)" );
  histos2D_[ "genDecayChn_hitFitNuPt" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitNuEta" ] = fileService->make< TH2D >( "genDecayChn_hitFitNuEta", "HitFit neutrino pseudo-rapidity vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsNuEta_, -maxNuEta_, maxNuEta_ );
  histos2D_[ "genDecayChn_hitFitNuEta" ]->SetXTitle( "" );
  assignDecayChnNames( histos2D_[ "genDecayChn_hitFitNuEta" ]->GetXaxis() );
  histos2D_[ "genDecayChn_hitFitNuEta" ]->SetYTitle( "#eta_{#nu}^{HitFit}" );
  histos2D_[ "genDecayChn_hitFitNuEta" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitNuPhi" ] = fileService->make< TH2D >( "genDecayChn_hitFitNuPhi", "HitFit neutrino azimutal angle vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsNuPhi_, -TMath::Pi(), TMath::Pi() );
  histos2D_[ "genDecayChn_hitFitNuPhi" ]->SetXTitle( "" );
  assignDecayChnNames( histos2D_[ "genDecayChn_hitFitNuPhi" ]->GetXaxis() );
  histos2D_[ "genDecayChn_hitFitNuPhi" ]->SetYTitle( "#phi_{#nu}^{HitFit}" );
  histos2D_[ "genDecayChn_hitFitNuPhi" ]->SetZTitle( "events" );
  histos2D_[ "genDecayChn_hitFitNuPz" ] = fileService->make< TH2D >( "genDecayChn_hitFitNuPz", "HitFit neutrino longitudinal momentum vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsNuPz_, -maxNuPz_, maxNuPz_ );
  histos2D_[ "genDecayChn_hitFitNuPz" ]->SetXTitle( "" );
  assignDecayChnNames( histos2D_[ "genDecayChn_hitFitNuPz" ]->GetXaxis() );
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
  histos1D_[ "hitFitProbLog" ]->Fill( log10( ttSemiLeptonicEvent->hitFitProb() ) );

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

    histos1D_[ "hitFitLepM" ]->Fill( hitFitLep->mass() );
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
      // FIXME: This is still muon-specific
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
      histos2D_[ "genDecayChn_hitFitProbLog" ]->Fill( trueDecayChn, log10( ttSemiLeptonicEvent->hitFitProb() ) );

      // Fill histograms only for converged fits
      if ( ttSemiLeptonicEvent->hitFitProb() >= 0. ) {

        histos2D_[ "genDecayChn_hitFitChi2" ]->Fill( trueDecayChn, ttSemiLeptonicEvent->hitFitChi2() );
        histos2D_[ "genDecayChn_hitFitMT" ]->Fill( trueDecayChn, ttSemiLeptonicEvent->hitFitMT() );
        histos2D_[ "genDecayChn_hitFitSigMT" ]->Fill( trueDecayChn, ttSemiLeptonicEvent->hitFitSigMT() );
        histos2D_[ "genDecayChn_hitFitSigMTRel" ]->Fill( trueDecayChn, ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );

        histos2D_[ "genDecayChn_hitFitLepM" ]->Fill( trueDecayChn, hitFitLep->mass() );
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

      // FIXME: This is still muon-specific
      histos1D_[ "genTopLepM" ]->Fill( genTopLep->mass() );
      histos1D_[ "genTopHadM" ]->Fill( genTopHad->mass() );
      histos1D_[ "genLepM" ]->Fill( genMu->mass() );
      histos1D_[ "genLepPt" ]->Fill( genMu->pt() );
      histos1D_[ "genLepEta" ]->Fill( genMu->eta() );
      histos1D_[ "genLepPhi" ]->Fill( genMu->phi() );
      histos1D_[ "genLepPz" ]->Fill( genMu->pz() );
      histos1D_[ "genNuM" ]->Fill( genNu->mass() );
      histos1D_[ "genNuPt" ]->Fill( genNu->pt() );
      histos1D_[ "genNuEta" ]->Fill( genNu->eta() );
      histos1D_[ "genNuPhi" ]->Fill( genNu->phi() );
      histos1D_[ "genNuPz" ]->Fill( genNu->pz() );

      histos1D_[ "nRealNuSolSig" ]->Fill( nRealNuSol );
      histos1D_[ "hitFitProbSig" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
      histos1D_[ "hitFitProbLowSig" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
      histos1D_[ "hitFitProbLogSig" ]->Fill( log10( ttSemiLeptonicEvent->hitFitProb() ) );

      // Fill histograms only for converged fits
      if ( ttSemiLeptonicEvent->hitFitProb() >= 0. ) {

        histos1D_[ "hitFitDiffGenTopLepM" ]->Fill( ttSemiLeptonicEvent->hitFitMT() - genTopLep->mass() );
        histos1D_[ "hitFitDiffGenTopHadM" ]->Fill( ttSemiLeptonicEvent->hitFitMT() - genTopHad->mass() );
        histos1D_[ "hitFitDiffGenLepM" ]->Fill( hitFitLep->mass() - genMu->mass() );
        histos1D_[ "hitFitDiffGenLepPt" ]->Fill( hitFitLep->pt() - genMu->pt() );
        histos1D_[ "hitFitDiffGenLepEta" ]->Fill( hitFitLep->eta() - genMu->eta() );
        histos1D_[ "hitFitDiffGenLepPhi" ]->Fill( hitFitLep->phi() - genMu->phi() );
        histos1D_[ "hitFitDiffGenLepPz" ]->Fill( hitFitLep->pz() - genMu->pz() );
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

        histos1D_[ "hitFitLepMSig" ]->Fill( hitFitLep->mass() );
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
      histos1D_[ "hitFitProbLogBkg" ]->Fill( log10( ttSemiLeptonicEvent->hitFitProb() ) );

      // Fill histograms only for converged fits
      if ( ttSemiLeptonicEvent->hitFitProb() >= 0. ) {

        histos1D_[ "hitFitChi2Bkg" ]->Fill( ttSemiLeptonicEvent->hitFitChi2() );
        histos1D_[ "hitFitMTBkg" ]->Fill( ttSemiLeptonicEvent->hitFitMT() );
        histos1D_[ "hitFitSigMTBkg" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT() );
        histos1D_[ "hitFitSigMTRelBkg" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );
        histos2D_[ "hitFitMT_hitFitSigMTBkg" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT() );
        histos2D_[ "hitFitMT_hitFitSigMTRelBkg" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );

        histos1D_[ "hitFitLepMBkg" ]->Fill( hitFitLep->mass() );
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


void TopHitFitAnalyzer::assignDecayChnNames( TAxis * iAxis )
{

  for ( unsigned i = 1; i < decayChnNames_.size(); ++i ) {
    iAxis->SetBinLabel( i, decayChnNames_.at( i ).c_str() );
  }

}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( TopHitFitAnalyzer );
