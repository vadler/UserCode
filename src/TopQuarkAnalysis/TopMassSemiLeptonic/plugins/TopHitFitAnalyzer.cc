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
    std::map< std::string, TH1D* > hist1D_;
    std::map< std::string, TH2D* > hist2D_;
    std::map< std::string, TH1D* > hist1D_Gen_;
    std::map< std::string, TH2D* > hist2D_Gen_;
    std::map< std::string, TH1D* > hist1D_HitFit_;
    std::map< std::string, TH2D* > hist2D_HitFit_;
    std::map< std::string, TH1D* > hist1D_HitFit_Sig_;
    std::map< std::string, TH2D* > hist2D_HitFit_Sig_;
    std::map< std::string, TH1D* > hist1D_HitFit_Tau_;
    std::map< std::string, TH2D* > hist2D_HitFit_Tau_;
    std::map< std::string, TH1D* > hist1D_HitFit_SigTau_;
    std::map< std::string, TH2D* > hist2D_HitFit_SigTau_;
    std::map< std::string, TH1D* > hist1D_HitFit_Bkg_;
    std::map< std::string, TH2D* > hist2D_HitFit_Bkg_;
    std::map< std::string, TH1D* > hist1D_HitFit_BkgNoTau_;
    std::map< std::string, TH2D* > hist2D_HitFit_BkgNoTau_;
    std::map< std::string, TH1D* > hist1D_Diff_;
    std::map< std::string, TH2D* > hist2D_Diff_;
    std::map< std::string, TH2D* > hist2D_GenHitFit_;

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
// initialise histo maps
, hist1D_()
, hist2D_()
, hist1D_Gen_()
, hist2D_Gen_()
, hist1D_HitFit_()
, hist2D_HitFit_()
, hist1D_HitFit_Sig_()
, hist2D_HitFit_Sig_()
, hist1D_HitFit_Tau_()
, hist2D_HitFit_Tau_()
, hist1D_HitFit_SigTau_()
, hist2D_HitFit_SigTau_()
, hist1D_HitFit_Bkg_()
, hist2D_HitFit_Bkg_()
, hist1D_HitFit_BkgNoTau_()
, hist2D_HitFit_BkgNoTau_()
, hist1D_Diff_()
, hist2D_Diff_()
, hist2D_GenHitFit_()
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
  hist1D_HitFit_[ "nRealNuSol" ] = fileService->make< TH1D >( "HitFit_nRealNuSol", "HitFit real #nu solutions", 4, -1.5, 2.5 );
  hist1D_HitFit_[ "nRealNuSol" ]->SetXTitle( "solutions_{real #nu}^{HitFit}" );
  hist1D_HitFit_[ "nRealNuSol" ]->SetYTitle( "events" );
  hist1D_HitFit_[ "Prob" ] = fileService->make< TH1D >( "HitFit_Prob", "HitFit probabilty", binsHitFitProb_, 0., 1. );
  hist1D_HitFit_[ "Prob" ]->SetXTitle( "P^{HitFit}" );
  hist1D_HitFit_[ "Prob" ]->SetYTitle( "events" );
  hist1D_HitFit_[ "ProbLow" ] = fileService->make< TH1D >( "HitFit_ProbLow", "HitFit probabilty", binsHitFitProbLow_, 0., maxHitFitProbLow_ );
  hist1D_HitFit_[ "ProbLow" ]->SetXTitle( "P^{HitFit}" );
  hist1D_HitFit_[ "ProbLow" ]->SetYTitle( "events" );
  hist1D_HitFit_[ "ProbLog" ] = fileService->make< TH1D >( "HitFit_ProbLog", "HitFit probabilty", binsHitFitProbLog_, minHitFitProbLog_, 0. );
  hist1D_HitFit_[ "ProbLog" ]->SetXTitle( "log_{10} P" );
  hist1D_HitFit_[ "ProbLog" ]->SetYTitle( "events" );
  hist1D_HitFit_[ "Chi2" ] = fileService->make< TH1D >( "HitFit_Chi2", "HitFit #chi^{2}", binsHitFitChi2_, 0., maxHitFitChi2_ );
  hist1D_HitFit_[ "Chi2" ]->SetXTitle( "(#chi^{2})^{HitFit}" );
  hist1D_HitFit_[ "Chi2" ]->SetYTitle( "events" );
  hist1D_HitFit_[ "MT" ] = fileService->make< TH1D >( "HitFit_MT", "HitFit top mass", binsHitFitMT_, minHitFitMT_, maxHitFitMT_ );
  hist1D_HitFit_[ "MT" ]->SetXTitle( "m_{top}^{HitFit} (GeV)" );
  hist1D_HitFit_[ "MT" ]->SetYTitle( "events" );
  hist1D_HitFit_[ "SigMT" ] = fileService->make< TH1D >( "HitFit_SigMT", "HitFit top mass uncertainty", binsHitFitSigMT_, 0., maxHitFitSigMT_ );
  hist1D_HitFit_[ "SigMT" ]->SetXTitle( "#sigma_{m_{top}}^{HitFit} (GeV)" );
  hist1D_HitFit_[ "SigMT" ]->SetYTitle( "events" );
  hist1D_HitFit_[ "SigMTRel" ] = fileService->make< TH1D >( "HitFit_SigMTRel", "Relative HitFit top mass uncertainty", binsHitFitSigMTRel_, 0., maxHitFitSigMTRel_ );
  hist1D_HitFit_[ "SigMTRel" ]->SetXTitle( "#frac{#sigma_{m_{t}}^{HitFit}}{m_{t}^{HitFit}}" );
  hist1D_HitFit_[ "SigMTRel" ]->SetYTitle( "events" );
  hist1D_HitFit_[ "LepM" ] = fileService->make< TH1D >( "HitFit_LepM", "HitFit lepton mass", binsLepM_, -maxLepM_, maxLepM_ );
  hist1D_HitFit_[ "LepM" ]->SetXTitle( "m_{l}^{HitFit} (GeV)" );
  hist1D_HitFit_[ "LepM" ]->SetYTitle( "events" );
  hist1D_HitFit_[ "LepPt" ] = fileService->make< TH1D >( "HitFit_LepPt", "HitFit lepton transverse momentum", binsLepPt_, 0., maxLepPt_ );
  hist1D_HitFit_[ "LepPt" ]->SetXTitle( "p_{t, l}^{HitFit} (GeV)" );
  hist1D_HitFit_[ "LepPt" ]->SetYTitle( "events" );
  hist1D_HitFit_[ "LepEta" ] = fileService->make< TH1D >( "HitFit_LepEta", "HitFit lepton pseudo-rapidity", binsLepEta_, -maxLepEta_, maxLepEta_ );
  hist1D_HitFit_[ "LepEta" ]->SetXTitle( "#eta_{l}^{HitFit}" );
  hist1D_HitFit_[ "LepEta" ]->SetYTitle( "events" );
  hist1D_HitFit_[ "LepPhi" ] = fileService->make< TH1D >( "HitFit_LepPhi", "HitFit lepton azimutal angle", binsLepPhi_, -TMath::Pi(), TMath::Pi() );
  hist1D_HitFit_[ "LepPhi" ]->SetXTitle( "#phi_{l}^{HitFit}" );
  hist1D_HitFit_[ "LepPhi" ]->SetYTitle( "events" );
  hist1D_HitFit_[ "LepPz" ] = fileService->make< TH1D >( "HitFit_LepPz", "HitFit lepton longitudinal momentum", binsLepPz_, -maxLepPz_, maxLepPz_ );
  hist1D_HitFit_[ "LepPz" ]->SetXTitle( "p_{z, l}^{HitFit} (GeV)" );
  hist1D_HitFit_[ "LepPz" ]->SetYTitle( "events" );
  hist1D_HitFit_[ "NuM" ] = fileService->make< TH1D >( "HitFit_NuM", "HitFit neutrino mass", binsNuM_, -maxNuM_, maxNuM_ );
  hist1D_HitFit_[ "NuM" ]->SetXTitle( "m_{#nu}^{HitFit} (GeV)" );
  hist1D_HitFit_[ "NuM" ]->SetYTitle( "events" );
  hist1D_HitFit_[ "NuPt" ] = fileService->make< TH1D >( "HitFit_NuPt", "HitFit neutrino transverse momentum", binsNuPt_, 0., maxNuPt_ );
  hist1D_HitFit_[ "NuPt" ]->SetXTitle( "p_{t, #nu}^{HitFit} (GeV)" );
  hist1D_HitFit_[ "NuPt" ]->SetYTitle( "events" );
  hist1D_HitFit_[ "NuEta" ] = fileService->make< TH1D >( "HitFit_NuEta", "HitFit neutrino pseudo-rapidity", binsNuEta_, -maxNuEta_, maxNuEta_ );
  hist1D_HitFit_[ "NuEta" ]->SetXTitle( "#eta_{#nu}^{HitFit}" );
  hist1D_HitFit_[ "NuEta" ]->SetYTitle( "events" );
  hist1D_HitFit_[ "NuPhi" ] = fileService->make< TH1D >( "HitFit_NuPhi", "HitFit neutrino azimutal angle", binsNuPhi_, -TMath::Pi(), TMath::Pi() );
  hist1D_HitFit_[ "NuPhi" ]->SetXTitle( "#phi_{#nu}^{HitFit}" );
  hist1D_HitFit_[ "NuPhi" ]->SetYTitle( "events" );
  hist1D_HitFit_[ "NuPz" ] = fileService->make< TH1D >( "HitFit_NuPz", "HitFit neutrino longitudinal momentum", binsNuPz_, -maxNuPz_, maxNuPz_ );
  hist1D_HitFit_[ "NuPz" ]->SetXTitle( "p_{z, #nu}^{HitFit} (GeV)" );
  hist1D_HitFit_[ "NuPz" ]->SetYTitle( "events" );
  // 2-dim
  hist2D_HitFit_[ "Prob_MT" ] = fileService->make< TH2D >( "HitFit_Prob_MT", "HitFit top mass vs. HitFit probabilty", binsHitFitProb_, 0., 1., binsHitFitMT_, minHitFitMT_, maxHitFitMT_ );
  hist2D_HitFit_[ "Prob_MT" ]->SetXTitle( "P^{HitFit}" );
  hist2D_HitFit_[ "Prob_MT" ]->SetYTitle( "m_{top}^{HitFit} (GeV)" );
  hist2D_HitFit_[ "Prob_MT" ]->SetZTitle( "events" );
  hist2D_HitFit_[ "Prob_SigMT" ] = fileService->make< TH2D >( "HitFit_Prob_SigMT", "HitFit top mass uncertainty vs. HitFit probabilty", binsHitFitProb_, 0., 1., binsHitFitSigMT_, 0., maxHitFitSigMT_ );
  hist2D_HitFit_[ "Prob_SigMT" ]->SetXTitle( "P^{HitFit}" );
  hist2D_HitFit_[ "Prob_SigMT" ]->SetYTitle( "#sigma_{m_{t}}^{HitFit} (GeV)" );
  hist2D_HitFit_[ "Prob_SigMT" ]->SetZTitle( "events" );
  hist2D_HitFit_[ "Prob_SigMTRel" ] = fileService->make< TH2D >( "HitFit_Prob_SigMTRel", "Relative HitFit top mass uncertainty vs. HitFit probabilty", binsHitFitProb_, 0., 1., binsHitFitSigMTRel_, 0., maxHitFitSigMTRel_ );
  hist2D_HitFit_[ "Prob_SigMTRel" ]->SetXTitle( "P^{HitFit}" );
  hist2D_HitFit_[ "Prob_SigMTRel" ]->SetYTitle( "#frac{#sigma_{m_{t}}^{HitFit}}{m_{t}^{HitFit}}" );
  hist2D_HitFit_[ "Prob_SigMTRel" ]->SetZTitle( "events" );
  hist2D_HitFit_[ "MT_SigMT" ] = fileService->make< TH2D >( "HitFit_MT_SigMT", "HitFit top mass uncertainty vs. HitFit top mass", binsHitFitMT_, minHitFitMT_, maxHitFitMT_, binsHitFitSigMT_, 0., maxHitFitSigMT_ );
  hist2D_HitFit_[ "MT_SigMT" ]->SetXTitle( "m_{t}^{HitFit} (GeV)" );
  hist2D_HitFit_[ "MT_SigMT" ]->SetYTitle( "#sigma_{m_{t}}^{HitFit} (GeV)" );
  hist2D_HitFit_[ "MT_SigMT" ]->SetZTitle( "events" );
  hist2D_HitFit_[ "MT_SigMTRel" ] = fileService->make< TH2D >( "HitFit_MT_SigMTRel", "Relative HitFit top mass uncertainty vs. HitFit top mass", binsHitFitMT_, minHitFitMT_, maxHitFitMT_, binsHitFitSigMTRel_, 0., maxHitFitSigMTRel_ );
  hist2D_HitFit_[ "MT_SigMTRel" ]->SetXTitle( "m_{t}^{HitFit} (GeV)" );
  hist2D_HitFit_[ "MT_SigMTRel" ]->SetYTitle( "#frac{#sigma_{m_{t}}^{HitFit}}{m_{t}^{HitFit}}" );
  hist2D_HitFit_[ "MT_SigMTRel" ]->SetZTitle( "events" );

  // MC specific (using true information)
  // 1-dim
  hist1D_Gen_[ "DecayChn" ] = fileService->make< TH1D >( "Gen_DecayChn", "Decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1. );
  hist1D_Gen_[ "DecayChn" ]->SetXTitle( "" );
  assignDecayChnNames( hist1D_Gen_[ "DecayChn" ]->GetXaxis() );
  hist1D_Gen_[ "DecayChn" ]->SetYTitle( "events" );
  hist1D_Gen_[ "TopLepM" ] = fileService->make< TH1D >( "Gen_TopLepM", "HitFit muonic top mass", binsTopM_, minTopM_, maxTopM_ );
  hist1D_Gen_[ "TopLepM" ]->SetXTitle( "m_{t_{l}}^{gen.} (GeV)" );
  hist1D_Gen_[ "TopLepM" ]->SetYTitle( "events" );
  hist1D_Gen_[ "TopHadM" ] = fileService->make< TH1D >( "Gen_TopHadM", "HitFit hadronic top mass", binsTopM_, minTopM_, maxTopM_ );
  hist1D_Gen_[ "TopHadM" ]->SetXTitle( "m_{t_{h}}^{gen.} (GeV)" );
  hist1D_Gen_[ "TopHadM" ]->SetYTitle( "events" );
  hist1D_Gen_[ "LepM" ] = fileService->make< TH1D >( "Gen_LepM", "Generated lepton mass", binsGenLepM_, 0., maxGenLepM_ );
  hist1D_Gen_[ "LepM" ]->SetXTitle( "m_{l}^{gen.} (GeV)" );
  hist1D_Gen_[ "LepM" ]->SetYTitle( "events" );
  hist1D_Gen_[ "LepPt" ] = fileService->make< TH1D >( "Gen_LepPt", "Generated lepton transverse momentum", binsLepPt_, 0., maxLepPt_ );
  hist1D_Gen_[ "LepPt" ]->SetXTitle( "p_{t, l}^{gen.} (GeV)" );
  hist1D_Gen_[ "LepPt" ]->SetYTitle( "events" );
  hist1D_Gen_[ "LepEta" ] = fileService->make< TH1D >( "Gen_LepEta", "Generated lepton pseudo-rapidity", binsLepEta_, -maxLepEta_, maxLepEta_ );
  hist1D_Gen_[ "LepEta" ]->SetXTitle( "#eta_{l}^{gen.}" );
  hist1D_Gen_[ "LepEta" ]->SetYTitle( "events" );
  hist1D_Gen_[ "LepPhi" ] = fileService->make< TH1D >( "Gen_LepPhi", "Generated lepton azimutal angle", binsLepPhi_, -TMath::Pi(), TMath::Pi() );
  hist1D_Gen_[ "LepPhi" ]->SetXTitle( "#phi_{l}^{gen.}" );
  hist1D_Gen_[ "LepPhi" ]->SetYTitle( "events" );
  hist1D_Gen_[ "LepPz" ] = fileService->make< TH1D >( "Gen_LepPz", "Generated lepton longitudinal momentum", binsLepPz_, -maxLepPz_, maxLepPz_ );
  hist1D_Gen_[ "LepPz" ]->SetXTitle( "p_{z, l}^{gen.} (GeV)" );
  hist1D_Gen_[ "LepPz" ]->SetYTitle( "events" );
  hist1D_Gen_[ "NuM" ] = fileService->make< TH1D >( "Gen_NuM", "Generated neutrino mass", binsNuM_, -maxNuM_, maxNuM_ );
  hist1D_Gen_[ "NuM" ]->SetXTitle( "m_{#nu}^{gen.} (GeV)" );
  hist1D_Gen_[ "NuM" ]->SetYTitle( "events" );
  hist1D_Gen_[ "NuPt" ] = fileService->make< TH1D >( "Gen_NuPt", "Generated neutrino transverse momentum", binsNuPt_, 0., maxNuPt_ );
  hist1D_Gen_[ "NuPt" ]->SetXTitle( "p_{t, #nu}^{gen.} (GeV)" );
  hist1D_Gen_[ "NuPt" ]->SetYTitle( "events" );
  hist1D_Gen_[ "NuEta" ] = fileService->make< TH1D >( "Gen_NuEta", "Generated neutrino pseudo-rapidity", binsNuEta_, -maxNuEta_, maxNuEta_ );
  hist1D_Gen_[ "NuEta" ]->SetXTitle( "#eta_{#nu}^{gen.}" );
  hist1D_Gen_[ "NuEta" ]->SetYTitle( "events" );
  hist1D_Gen_[ "NuPhi" ] = fileService->make< TH1D >( "Gen_NuPhi", "Generated neutrino azimutal angle", binsNuPhi_, -TMath::Pi(), TMath::Pi() );
  hist1D_Gen_[ "NuPhi" ]->SetXTitle( "#phi_{#nu}^{gen.}" );
  hist1D_Gen_[ "NuPhi" ]->SetYTitle( "events" );
  hist1D_Gen_[ "NuPz" ] = fileService->make< TH1D >( "Gen_NuPz", "Generated neutrino longitudinal momentum", binsNuPz_, -maxNuPz_, maxNuPz_ );
  hist1D_Gen_[ "NuPz" ]->SetXTitle( "p_{z, #nu}^{gen.} (GeV)" );
  hist1D_Gen_[ "NuPz" ]->SetYTitle( "events" );
  hist1D_Diff_[ "TopLepM" ] = fileService->make< TH1D >( "Diff_TopLepM", "HitFit leptonic top mass deviation", binsDiffTopM_, -maxDiffTopM_, maxDiffTopM_ );
  hist1D_Diff_[ "TopLepM" ]->SetXTitle( "#Delta m_{t_{l}} (GeV)" );
  hist1D_Diff_[ "TopLepM" ]->SetYTitle( "events" );
  hist1D_Diff_[ "TopHadM" ] = fileService->make< TH1D >( "Diff_TopHadM", "HitFit hadronic top mass deviation", binsDiffTopM_, -maxDiffTopM_, maxDiffTopM_ );
  hist1D_Diff_[ "TopHadM" ]->SetXTitle( "#Delta m_{t_{h}} (GeV)" );
  hist1D_Diff_[ "TopHadM" ]->SetYTitle( "events" );
  hist1D_Diff_[ "LepM" ] = fileService->make< TH1D >( "Diff_LepM", "HitFit lepton mass deviation", binsDiffLepM_, -maxDiffLepM_, maxDiffLepM_ );
  hist1D_Diff_[ "LepM" ]->SetXTitle( "#Delta m_{l} (GeV)" );
  hist1D_Diff_[ "LepM" ]->SetYTitle( "events" );
  hist1D_Diff_[ "LepPt" ] = fileService->make< TH1D >( "Diff_LepPt", "HitFit lepton transverse momentum deviation", binsDiffLepPt_, -maxDiffLepPt_, maxDiffLepPt_ );
  hist1D_Diff_[ "LepPt" ]->SetXTitle( "#Delta p_{t, l} (GeV)" );
  hist1D_Diff_[ "LepPt" ]->SetYTitle( "events" );
  hist1D_Diff_[ "LepEta" ] = fileService->make< TH1D >( "Diff_LepEta", "HitFit lepton pseudo-rapidity deviation", binsDiffLepEta_, -maxDiffLepEta_, maxDiffLepEta_ );
  hist1D_Diff_[ "LepEta" ]->SetXTitle( "#Delta #eta_{l}" );
  hist1D_Diff_[ "LepEta" ]->SetYTitle( "events" );
  hist1D_Diff_[ "LepPhi" ] = fileService->make< TH1D >( "Diff_LepPhi", "HitFit lepton azimutal angle deviation", binsDiffLepPhi_, -maxDiffLepPhi_, maxDiffLepPhi_ );
  hist1D_Diff_[ "LepPhi" ]->SetXTitle( "#Delta #phi_{l}" );
  hist1D_Diff_[ "LepPhi" ]->SetYTitle( "events" );
  hist1D_Diff_[ "LepPz" ] = fileService->make< TH1D >( "Diff_LepPz", "HitFit lepton longitudinal momentum deviation", binsDiffLepPz_, -binsDiffLepPz_, binsDiffLepPz_ );
  hist1D_Diff_[ "LepPz" ]->SetXTitle( "#Delta p_{z, l} (GeV)" );
  hist1D_Diff_[ "LepPz" ]->SetYTitle( "events" );
  hist1D_Diff_[ "NuM" ] = fileService->make< TH1D >( "Diff_NuM", "HitFit neutrino mass deviation", binsDiffNuM_, -maxDiffNuM_, maxDiffNuM_ );
  hist1D_Diff_[ "NuM" ]->SetXTitle( "#Delta m_{#nu} (GeV)" );
  hist1D_Diff_[ "NuM" ]->SetYTitle( "events" );
  hist1D_Diff_[ "NuPt" ] = fileService->make< TH1D >( "Diff_NuPt", "HitFit neutrino transverse momentum deviation", binsDiffNuPt_, -maxDiffNuPt_, maxDiffNuPt_ );
  hist1D_Diff_[ "NuPt" ]->SetXTitle( "#Delta p_{t, #nu} (GeV)" );
  hist1D_Diff_[ "NuPt" ]->SetYTitle( "events" );
  hist1D_Diff_[ "NuEta" ] = fileService->make< TH1D >( "Diff_NuEta", "HitFit neutrino pseudo-rapidity deviation", binsDiffNuEta_, -maxDiffNuEta_, maxDiffNuEta_ );
  hist1D_Diff_[ "NuEta" ]->SetXTitle( "#Delta #eta_{#nu}" );
  hist1D_Diff_[ "NuEta" ]->SetYTitle( "events" );
  hist1D_Diff_[ "NuPhi" ] = fileService->make< TH1D >( "Diff_NuPhi", "HitFit neutrino azimutal angle deviation", binsDiffNuPhi_, -maxDiffNuPhi_, maxDiffNuPhi_ );
  hist1D_Diff_[ "NuPhi" ]->SetXTitle( "#Delta #phi_{#nu}" );
  hist1D_Diff_[ "NuPhi" ]->SetYTitle( "events" );
  hist1D_Diff_[ "NuPz" ] = fileService->make< TH1D >( "Diff_NuPz", "HitFit neutrino longitudinal momentum deviation", binsDiffNuPz_, -maxDiffNuPz_, maxDiffNuPz_ );
  hist1D_Diff_[ "NuPz" ]->SetXTitle( "#Delta p_{z, #nu} (GeV)" );
  hist1D_Diff_[ "NuPz" ]->SetYTitle( "events" );
  for ( std::map< std::string, TH1D* >::const_iterator iHist = hist1D_HitFit_.begin(); iHist != hist1D_HitFit_.end(); ++iHist ) {
    std::string nameSig( iHist->second->GetName() );
    hist1D_HitFit_Sig_[ iHist->first ] = fileService->make< TH1D >( *( ( TH1D* )( iHist->second->Clone( nameSig.replace( 6, 0, "_Sig" ).c_str() ) ) ) );
    std::string nameTau( iHist->second->GetName() );
    hist1D_HitFit_Tau_[ iHist->first ] = fileService->make< TH1D >( *( ( TH1D* )( iHist->second->Clone( nameSig.replace( 6, 4, "_Tau" ).c_str() ) ) ) );
    std::string nameSigTau( iHist->second->GetName() );
    hist1D_HitFit_SigTau_[ iHist->first ] = fileService->make< TH1D >( *( ( TH1D* )( iHist->second->Clone( nameSigTau.replace( 6, 0, "_SigTau" ).c_str() ) ) ) );
    std::string nameBkg( iHist->second->GetName() );
    hist1D_HitFit_Bkg_[ iHist->first ] = fileService->make< TH1D >( *( ( TH1D* )( iHist->second->Clone( nameBkg.replace( 6, 0, "_Bkg" ).c_str() ) ) ) );
    std::string nameBkgNoTau( iHist->second->GetName() );
    hist1D_HitFit_BkgNoTau_[ iHist->first ] = fileService->make< TH1D >( *( ( TH1D* )( iHist->second->Clone( nameBkgNoTau.replace( 6, 0, "_BkgNoTau" ).c_str() ) ) ) );
  }
  // 2-dim
  for ( std::map< std::string, TH2D* >::const_iterator iHist = hist2D_HitFit_.begin(); iHist != hist2D_HitFit_.end(); ++iHist ) {
    std::string nameSig( iHist->second->GetName() );
    hist2D_HitFit_Sig_[ iHist->first ] = fileService->make< TH2D >( *( ( TH2D* )( iHist->second->Clone( nameSig.replace( 6, 0, "_Sig" ).c_str() ) ) ) );
    std::string nameTau( iHist->second->GetName() );
    hist2D_HitFit_Tau_[ iHist->first ] = fileService->make< TH2D >( *( ( TH2D* )( iHist->second->Clone( nameSig.replace( 6, 4, "_Tau" ).c_str() ) ) ) );
    std::string nameSigTau( iHist->second->GetName() );
    hist2D_HitFit_SigTau_[ iHist->first ] = fileService->make< TH2D >( *( ( TH2D* )( iHist->second->Clone( nameSigTau.replace( 6, 0, "_SigTau" ).c_str() ) ) ) );
    std::string nameBkg( iHist->second->GetName() );
    hist2D_HitFit_Bkg_[ iHist->first ] = fileService->make< TH2D >( *( ( TH2D* )( iHist->second->Clone( nameBkg.replace( 6, 0, "_Bkg" ).c_str() ) ) ) );
    std::string nameBkgNoTau( iHist->second->GetName() );
    hist2D_HitFit_BkgNoTau_[ iHist->first ] = fileService->make< TH2D >( *( ( TH2D* )( iHist->second->Clone( nameBkgNoTau.replace( 6, 0, "_BkgNoTau" ).c_str() ) ) ) );
  }
  hist2D_GenHitFit_[ "DecayChn_nRealNuSol" ] = fileService->make< TH2D >( "DecayChn_nRealNuSol", "HitFit real #nu solutions vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., 4, -1.5, 2.5 );
  hist2D_GenHitFit_[ "DecayChn_nRealNuSol" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_nRealNuSol" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_nRealNuSol" ]->SetYTitle( "real #nu sols." );
  hist2D_GenHitFit_[ "DecayChn_nRealNuSol" ]->SetZTitle( "events" );
  hist2D_GenHitFit_[ "DecayChn_Prob" ] = fileService->make< TH2D >( "DecayChn_Prob", "HitFit probabilty vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsHitFitProb_, 0., 1. );
  hist2D_GenHitFit_[ "DecayChn_Prob" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_Prob" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_Prob" ]->SetYTitle( "P^{HitFit}" );
  hist2D_GenHitFit_[ "DecayChn_Prob" ]->SetZTitle( "events" );
  hist2D_GenHitFit_[ "DecayChn_ProbLow" ] = fileService->make< TH2D >( "DecayChn_ProbLow", "HitFit probabilty vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsHitFitProbLow_, 0., maxHitFitProbLow_ );
  hist2D_GenHitFit_[ "DecayChn_ProbLow" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_ProbLow" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_ProbLow" ]->SetYTitle( "P^{HitFit}" );
  hist2D_GenHitFit_[ "DecayChn_ProbLow" ]->SetZTitle( "events" );
  hist2D_GenHitFit_[ "DecayChn_ProbLog" ] = fileService->make< TH2D >( "DecayChn_ProbLog", "HitFit probabilty vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsHitFitProbLog_, minHitFitProbLog_, 0. );
  hist2D_GenHitFit_[ "DecayChn_ProbLog" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_ProbLog" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_ProbLog" ]->SetYTitle( "log_{10} P" );
  hist2D_GenHitFit_[ "DecayChn_ProbLog" ]->SetZTitle( "events" );
  hist2D_GenHitFit_[ "DecayChn_Chi2" ] = fileService->make< TH2D >( "DecayChn_Chi2", "HitFit #chi^{2} vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsHitFitChi2_, 0., maxHitFitChi2_ );
  hist2D_GenHitFit_[ "DecayChn_Chi2" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_Chi2" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_Chi2" ]->SetYTitle( "(#chi^{2})^{HitFit}" );
  hist2D_GenHitFit_[ "DecayChn_Chi2" ]->SetZTitle( "events" );
  hist2D_GenHitFit_[ "DecayChn_MT" ] = fileService->make< TH2D >( "DecayChn_MT", "HitFit top mass vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsHitFitMT_, minHitFitMT_, maxHitFitMT_ );
  hist2D_GenHitFit_[ "DecayChn_MT" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_MT" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_MT" ]->SetYTitle( "m_{top}^{HitFit} (GeV)" );
  hist2D_GenHitFit_[ "DecayChn_MT" ]->SetZTitle( "events" );
  hist2D_GenHitFit_[ "DecayChn_SigMT" ] = fileService->make< TH2D >( "DecayChn_SigMT", "HitFit top mass uncertainty vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsHitFitSigMT_, 0., maxHitFitSigMT_ );
  hist2D_GenHitFit_[ "DecayChn_SigMT" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_SigMT" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_SigMT" ]->SetYTitle( "#sigma_{m_{top}}^{HitFit} (GeV)" );
  hist2D_GenHitFit_[ "DecayChn_SigMT" ]->SetZTitle( "events" );
  hist2D_GenHitFit_[ "DecayChn_SigMTRel" ] = fileService->make< TH2D >( "DecayChn_SigMTRel", "Relative HitFit top mass uncertainty vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsHitFitSigMTRel_, 0., maxHitFitSigMTRel_ );
  hist2D_GenHitFit_[ "DecayChn_SigMTRel" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_SigMTRel" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_SigMTRel" ]->SetYTitle( "#frac{#sigma_{m_{t}}^{HitFit}}{m_{t}^{HitFit}}" );
  hist2D_GenHitFit_[ "DecayChn_SigMTRel" ]->SetZTitle( "events" );
  hist2D_GenHitFit_[ "DecayChn_LepM" ] = fileService->make< TH2D >( "DecayChn_LepM", "HitFit lepton mass vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsLepM_, -maxLepM_, maxLepM_ );
  hist2D_GenHitFit_[ "DecayChn_LepM" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_LepM" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_LepM" ]->SetYTitle( "m_{l}^{HitFit} (GeV)" );
  hist2D_GenHitFit_[ "DecayChn_LepM" ]->SetZTitle( "events" );
  hist2D_GenHitFit_[ "DecayChn_LepPt" ] = fileService->make< TH2D >( "DecayChn_LepPt", "HitFit lepton transverse momentum vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsLepPt_, 0., maxLepPt_ );
  hist2D_GenHitFit_[ "DecayChn_LepPt" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_LepPt" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_LepPt" ]->SetYTitle( "p_{t, l}^{HitFit} (GeV)" );
  hist2D_GenHitFit_[ "DecayChn_LepPt" ]->SetZTitle( "events" );
  hist2D_GenHitFit_[ "DecayChn_LepEta" ] = fileService->make< TH2D >( "DecayChn_LepEta", "HitFit lepton pseudo-rapidity vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsLepEta_, -maxLepEta_, maxLepEta_ );
  hist2D_GenHitFit_[ "DecayChn_LepEta" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_LepEta" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_LepEta" ]->SetYTitle( "#eta_{l}^{HitFit}" );
  hist2D_GenHitFit_[ "DecayChn_LepEta" ]->SetZTitle( "events" );
  hist2D_GenHitFit_[ "DecayChn_LepPhi" ] = fileService->make< TH2D >( "DecayChn_LepPhi", "HitFit lepton azimutal angle vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsLepPhi_, -TMath::Pi(), TMath::Pi() );
  hist2D_GenHitFit_[ "DecayChn_LepPhi" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_LepPhi" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_LepPhi" ]->SetYTitle( "#phi_{l}^{HitFit}" );
  hist2D_GenHitFit_[ "DecayChn_LepPhi" ]->SetZTitle( "events" );
  hist2D_GenHitFit_[ "DecayChn_LepPz" ] = fileService->make< TH2D >( "DecayChn_LepPz", "HitFit lepton longitudinal momentum vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsLepPz_, -maxLepPz_, maxLepPz_ );
  hist2D_GenHitFit_[ "DecayChn_LepPz" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_LepPz" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_LepPz" ]->SetYTitle( "p_{z, l}^{HitFit} (GeV)" );
  hist2D_GenHitFit_[ "DecayChn_LepPz" ]->SetZTitle( "events" );
  hist2D_GenHitFit_[ "DecayChn_NuM" ] = fileService->make< TH2D >( "DecayChn_NuM", "HitFit neutrino mass vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsNuM_, -maxNuM_, maxNuM_ );
  hist2D_GenHitFit_[ "DecayChn_NuM" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_NuM" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_NuM" ]->SetYTitle( "m_{#nu}^{HitFit} (GeV)" );
  hist2D_GenHitFit_[ "DecayChn_NuM" ]->SetZTitle( "events" );
  hist2D_GenHitFit_[ "DecayChn_NuPt" ] = fileService->make< TH2D >( "DecayChn_NuPt", "HitFit neutrino transverse momentum vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsNuPt_, 0., maxNuPt_ );
  hist2D_GenHitFit_[ "DecayChn_NuPt" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_NuPt" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_NuPt" ]->SetYTitle( "p_{t, #nu}^{HitFit} (GeV)" );
  hist2D_GenHitFit_[ "DecayChn_NuPt" ]->SetZTitle( "events" );
  hist2D_GenHitFit_[ "DecayChn_NuEta" ] = fileService->make< TH2D >( "DecayChn_NuEta", "HitFit neutrino pseudo-rapidity vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsNuEta_, -maxNuEta_, maxNuEta_ );
  hist2D_GenHitFit_[ "DecayChn_NuEta" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_NuEta" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_NuEta" ]->SetYTitle( "#eta_{#nu}^{HitFit}" );
  hist2D_GenHitFit_[ "DecayChn_NuEta" ]->SetZTitle( "events" );
  hist2D_GenHitFit_[ "DecayChn_NuPhi" ] = fileService->make< TH2D >( "DecayChn_NuPhi", "HitFit neutrino azimutal angle vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsNuPhi_, -TMath::Pi(), TMath::Pi() );
  hist2D_GenHitFit_[ "DecayChn_NuPhi" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_NuPhi" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_NuPhi" ]->SetYTitle( "#phi_{#nu}^{HitFit}" );
  hist2D_GenHitFit_[ "DecayChn_NuPhi" ]->SetZTitle( "events" );
  hist2D_GenHitFit_[ "DecayChn_NuPz" ] = fileService->make< TH2D >( "DecayChn_NuPz", "HitFit neutrino longitudinal momentum vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsNuPz_, -maxNuPz_, maxNuPz_ );
  hist2D_GenHitFit_[ "DecayChn_NuPz" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_NuPz" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_NuPz" ]->SetYTitle( "p_{z, #nu}^{HitFit} (GeV)" );
  hist2D_GenHitFit_[ "DecayChn_NuPz" ]->SetZTitle( "events" );

}


// Event loop
void TopHitFitAnalyzer::analyze( const edm::Event & iEvent, const edm::EventSetup & iSetup )
{

  // TQAF semi-leptonic event
  edm::Handle< TtSemiLeptonicEvent > ttSemiLeptonicEvent;
  iEvent.getByLabel( ttSemiLeptonicEvent_, ttSemiLeptonicEvent );

  // HitFit event hypothesis
  const reco::CompositeCandidate HitFitHypo( ttSemiLeptonicEvent->eventHypo( TtEvent::kHitFit ) );
  const reco::Candidate * HitFitTopLep( ttSemiLeptonicEvent->leptonicDecayTop( TtEvent::kHitFit ) );
  const reco::Candidate * HitFitTopHad( ttSemiLeptonicEvent->hadronicDecayTop( TtEvent::kHitFit ) );
  const reco::Candidate * HitFitLep( ttSemiLeptonicEvent->singleLepton( TtEvent::kHitFit ) );
  const reco::Candidate * HitFitNu( ttSemiLeptonicEvent->singleNeutrino( TtEvent::kHitFit ) );
  const int HitFitNRealNuSol( ttSemiLeptonicEvent->numberOfRealNeutrinoSolutions( TtEvent::kHitFit ) );

  hist1D_HitFit_[ "nRealNuSol" ]->Fill( HitFitNRealNuSol );
  hist1D_HitFit_[ "Prob" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
  hist1D_HitFit_[ "ProbLow" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
  hist1D_HitFit_[ "ProbLog" ]->Fill( log10( ttSemiLeptonicEvent->hitFitProb() ) );

  // Fill histograms only for converged fits
  if ( ttSemiLeptonicEvent->hitFitProb() >= 0. ) {

    hist1D_HitFit_[ "Chi2" ]->Fill( ttSemiLeptonicEvent->hitFitChi2() );
    hist1D_HitFit_[ "MT" ]->Fill( ttSemiLeptonicEvent->hitFitMT() );
    hist1D_HitFit_[ "SigMT" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT() );
    hist1D_HitFit_[ "SigMTRel" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );
    hist2D_HitFit_[ "Prob_MT" ]->Fill( ttSemiLeptonicEvent->hitFitProb(), ttSemiLeptonicEvent->hitFitMT() );
    hist2D_HitFit_[ "Prob_SigMT" ]->Fill( ttSemiLeptonicEvent->hitFitProb(), ttSemiLeptonicEvent->hitFitSigMT() );
    hist2D_HitFit_[ "Prob_SigMTRel" ]->Fill( ttSemiLeptonicEvent->hitFitProb(), ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );
    hist2D_HitFit_[ "MT_SigMT" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT() );
    hist2D_HitFit_[ "MT_SigMTRel" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );

    hist1D_HitFit_[ "LepM" ]->Fill( HitFitLep->mass() );
    hist1D_HitFit_[ "LepPt" ]->Fill( HitFitLep->pt() );
    hist1D_HitFit_[ "LepEta" ]->Fill( HitFitLep->eta() );
    hist1D_HitFit_[ "LepPhi" ]->Fill( HitFitLep->phi() );
    hist1D_HitFit_[ "LepPz" ]->Fill( HitFitLep->pz() );
    hist1D_HitFit_[ "NuM" ]->Fill( HitFitNu->mass() );
    hist1D_HitFit_[ "NuPt" ]->Fill( HitFitNu->pt() );
    hist1D_HitFit_[ "NuEta" ]->Fill( HitFitNu->eta() );
    hist1D_HitFit_[ "NuPhi" ]->Fill( HitFitNu->phi() );
    hist1D_HitFit_[ "NuPz" ]->Fill( HitFitNu->pz() );

  }

  // MC specific (using true information)
  if ( ! iEvent.isRealData() ) {

    bool     kSIGNAL( false );
    unsigned DecayChn( WDecay::kNone );

    // Generated event
    const TtGenEvent * ttGenEvent( ttSemiLeptonicEvent->genEvent().get() );
    // Proceed only for ttbar events
    if ( ttGenEvent->isTtBar() ) {

      // Decay channels
      // FIXME: This is still muon-specific
      unsigned nTrueOtherMuon( 0 );
      if ( ttGenEvent->isSemiLeptonic( WDecay::kElec ) ) {
        DecayChn = ttGenEvent->semiLeptonicChannel();
      }
      else if ( ttGenEvent->isSemiLeptonic( WDecay::kMuon ) ) {
        DecayChn = ttGenEvent->semiLeptonicChannel();
        kSIGNAL = true;
      }
      else if ( ttGenEvent->isSemiLeptonic( WDecay::kTau ) ) {
        DecayChn = ttGenEvent->semiLeptonicChannel();
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
        DecayChn += nTrueOtherMuon;
      }
      else if ( ttGenEvent->isFullLeptonic() ) {
        DecayChn = WDecay::kTau + 2;
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
        DecayChn += nTrueOtherMuon;
      }

      hist1D_Gen_[ "DecayChn" ]->Fill( DecayChn );
      hist2D_GenHitFit_[ "DecayChn_nRealNuSol" ]->Fill( DecayChn, HitFitNRealNuSol );
      hist2D_GenHitFit_[ "DecayChn_Prob" ]->Fill( DecayChn, ttSemiLeptonicEvent->hitFitProb() );
      hist2D_GenHitFit_[ "DecayChn_ProbLow" ]->Fill( DecayChn, ttSemiLeptonicEvent->hitFitProb() );
      hist2D_GenHitFit_[ "DecayChn_ProbLog" ]->Fill( DecayChn, log10( ttSemiLeptonicEvent->hitFitProb() ) );

      // Fill histograms only for converged fits
      if ( ttSemiLeptonicEvent->hitFitProb() >= 0. ) {

        hist2D_GenHitFit_[ "DecayChn_Chi2" ]->Fill( DecayChn, ttSemiLeptonicEvent->hitFitChi2() );
        hist2D_GenHitFit_[ "DecayChn_MT" ]->Fill( DecayChn, ttSemiLeptonicEvent->hitFitMT() );
        hist2D_GenHitFit_[ "DecayChn_SigMT" ]->Fill( DecayChn, ttSemiLeptonicEvent->hitFitSigMT() );
        hist2D_GenHitFit_[ "DecayChn_SigMTRel" ]->Fill( DecayChn, ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );

        hist2D_GenHitFit_[ "DecayChn_LepM" ]->Fill( DecayChn, HitFitLep->mass() );
        hist2D_GenHitFit_[ "DecayChn_LepPt" ]->Fill( DecayChn, HitFitLep->pt() );
        hist2D_GenHitFit_[ "DecayChn_LepEta" ]->Fill( DecayChn, HitFitLep->eta() );
        hist2D_GenHitFit_[ "DecayChn_LepPhi" ]->Fill( DecayChn, HitFitLep->phi() );
        hist2D_GenHitFit_[ "DecayChn_LepPz" ]->Fill( DecayChn, HitFitLep->pz() );
        hist2D_GenHitFit_[ "DecayChn_NuM" ]->Fill( DecayChn, HitFitNu->mass() );
        hist2D_GenHitFit_[ "DecayChn_NuPt" ]->Fill( DecayChn, HitFitNu->pt() );
        hist2D_GenHitFit_[ "DecayChn_NuEta" ]->Fill( DecayChn, HitFitNu->eta() );
        hist2D_GenHitFit_[ "DecayChn_NuPhi" ]->Fill( DecayChn, HitFitNu->phi() );
        hist2D_GenHitFit_[ "DecayChn_NuPz" ]->Fill( DecayChn, HitFitNu->pz() );

      }

    }

    if ( kSIGNAL ) {

      // Generator particles
      const reco::GenParticle * GenTopLep( ttGenEvent->leptonicDecayTop() );
      const reco::GenParticle * GenTopHad( ttGenEvent->hadronicDecayTop() );
      const reco::GenParticle * GenMu( ttGenEvent->singleLepton() );
      const reco::GenParticle * GenNu( ttGenEvent->singleNeutrino() );

      // FIXME: This is still muon-specific
      hist1D_Gen_[ "TopLepM" ]->Fill( GenTopLep->mass() );
      hist1D_Gen_[ "TopHadM" ]->Fill( GenTopHad->mass() );
      hist1D_Gen_[ "LepM" ]->Fill( GenMu->mass() );
      hist1D_Gen_[ "LepPt" ]->Fill( GenMu->pt() );
      hist1D_Gen_[ "LepEta" ]->Fill( GenMu->eta() );
      hist1D_Gen_[ "LepPhi" ]->Fill( GenMu->phi() );
      hist1D_Gen_[ "LepPz" ]->Fill( GenMu->pz() );
      hist1D_Gen_[ "NuM" ]->Fill( GenNu->mass() );
      hist1D_Gen_[ "NuPt" ]->Fill( GenNu->pt() );
      hist1D_Gen_[ "NuEta" ]->Fill( GenNu->eta() );
      hist1D_Gen_[ "NuPhi" ]->Fill( GenNu->phi() );
      hist1D_Gen_[ "NuPz" ]->Fill( GenNu->pz() );

      hist1D_HitFit_Sig_[ "nRealNuSol" ]->Fill( HitFitNRealNuSol );
      hist1D_HitFit_Sig_[ "Prob" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
      hist1D_HitFit_Sig_[ "ProbLow" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
      hist1D_HitFit_Sig_[ "ProbLog" ]->Fill( log10( ttSemiLeptonicEvent->hitFitProb() ) );
      hist1D_HitFit_SigTau_[ "nRealNuSol" ]->Fill( HitFitNRealNuSol );
      hist1D_HitFit_SigTau_[ "Prob" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
      hist1D_HitFit_SigTau_[ "ProbLow" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
      hist1D_HitFit_SigTau_[ "ProbLog" ]->Fill( log10( ttSemiLeptonicEvent->hitFitProb() ) );

//       // Fill histograms only for converged fits
      if ( ttSemiLeptonicEvent->hitFitProb() >= 0. ) {

        hist1D_Diff_[ "TopLepM" ]->Fill( ttSemiLeptonicEvent->hitFitMT() - GenTopLep->mass() );
        hist1D_Diff_[ "TopHadM" ]->Fill( ttSemiLeptonicEvent->hitFitMT() - GenTopHad->mass() );
        hist1D_Diff_[ "LepM" ]->Fill( HitFitLep->mass() - GenMu->mass() );
        hist1D_Diff_[ "LepPt" ]->Fill( HitFitLep->pt() - GenMu->pt() );
        hist1D_Diff_[ "LepEta" ]->Fill( HitFitLep->eta() - GenMu->eta() );
        hist1D_Diff_[ "LepPhi" ]->Fill( HitFitLep->phi() - GenMu->phi() );
        hist1D_Diff_[ "LepPz" ]->Fill( HitFitLep->pz() - GenMu->pz() );
        hist1D_Diff_[ "NuM" ]->Fill( HitFitNu->mass() - GenNu->mass() );
        hist1D_Diff_[ "NuPt" ]->Fill( HitFitNu->pt() - GenNu->pt() );
        hist1D_Diff_[ "NuEta" ]->Fill( HitFitNu->eta() - GenNu->eta() );
        hist1D_Diff_[ "NuPhi" ]->Fill( HitFitNu->phi() - GenNu->phi() );
        hist1D_Diff_[ "NuPz" ]->Fill( HitFitNu->pz() - GenNu->pz() );

        hist1D_HitFit_Sig_[ "Chi2" ]->Fill( ttSemiLeptonicEvent->hitFitChi2() );
        hist1D_HitFit_Sig_[ "MT" ]->Fill( ttSemiLeptonicEvent->hitFitMT() );
        hist1D_HitFit_Sig_[ "SigMT" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT() );
        hist1D_HitFit_Sig_[ "SigMTRel" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );
        hist2D_HitFit_Sig_[ "MT_SigMT" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT() );
        hist2D_HitFit_Sig_[ "MT_SigMTRel" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );
        hist1D_HitFit_SigTau_[ "Chi2" ]->Fill( ttSemiLeptonicEvent->hitFitChi2() );
        hist1D_HitFit_SigTau_[ "MT" ]->Fill( ttSemiLeptonicEvent->hitFitMT() );
        hist1D_HitFit_SigTau_[ "SigMT" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT() );
        hist1D_HitFit_SigTau_[ "SigMTRel" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );
        hist2D_HitFit_SigTau_[ "MT_SigMT" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT() );
        hist2D_HitFit_SigTau_[ "MT_SigMTRel" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );

        hist1D_HitFit_Sig_[ "LepM" ]->Fill( HitFitLep->mass() );
        hist1D_HitFit_Sig_[ "LepPt" ]->Fill( HitFitLep->pt() );
        hist1D_HitFit_Sig_[ "LepEta" ]->Fill( HitFitLep->eta() );
        hist1D_HitFit_Sig_[ "LepPhi" ]->Fill( HitFitLep->phi() );
        hist1D_HitFit_Sig_[ "LepPz" ]->Fill( HitFitLep->pz() );
        hist1D_HitFit_Sig_[ "NuM" ]->Fill( HitFitNu->mass() );
        hist1D_HitFit_Sig_[ "NuPt" ]->Fill( HitFitNu->pt() );
        hist1D_HitFit_Sig_[ "NuEta" ]->Fill( HitFitNu->eta() );
        hist1D_HitFit_Sig_[ "NuPhi" ]->Fill( HitFitNu->phi() );
        hist1D_HitFit_Sig_[ "NuPz" ]->Fill( HitFitNu->pz() );
        hist1D_HitFit_SigTau_[ "LepM" ]->Fill( HitFitLep->mass() );
        hist1D_HitFit_SigTau_[ "LepPt" ]->Fill( HitFitLep->pt() );
        hist1D_HitFit_SigTau_[ "LepEta" ]->Fill( HitFitLep->eta() );
        hist1D_HitFit_SigTau_[ "LepPhi" ]->Fill( HitFitLep->phi() );
        hist1D_HitFit_SigTau_[ "LepPz" ]->Fill( HitFitLep->pz() );
        hist1D_HitFit_SigTau_[ "NuM" ]->Fill( HitFitNu->mass() );
        hist1D_HitFit_SigTau_[ "NuPt" ]->Fill( HitFitNu->pt() );
        hist1D_HitFit_SigTau_[ "NuEta" ]->Fill( HitFitNu->eta() );
        hist1D_HitFit_SigTau_[ "NuPhi" ]->Fill( HitFitNu->phi() );
        hist1D_HitFit_SigTau_[ "NuPz" ]->Fill( HitFitNu->pz() );

      }

    }
    else {

      hist1D_HitFit_Bkg_[ "nRealNuSol" ]->Fill( HitFitNRealNuSol );
      hist1D_HitFit_Bkg_[ "Prob" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
      hist1D_HitFit_Bkg_[ "ProbLow" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
      hist1D_HitFit_Bkg_[ "ProbLog" ]->Fill( log10( ttSemiLeptonicEvent->hitFitProb() ) );
      if ( DecayChn == WDecay::kTau + 1 ) {
        hist1D_HitFit_Tau_[ "nRealNuSol" ]->Fill( HitFitNRealNuSol );
        hist1D_HitFit_Tau_[ "Prob" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
        hist1D_HitFit_Tau_[ "ProbLow" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
        hist1D_HitFit_Tau_[ "ProbLog" ]->Fill( log10( ttSemiLeptonicEvent->hitFitProb() ) );
        hist1D_HitFit_SigTau_[ "nRealNuSol" ]->Fill( HitFitNRealNuSol );
        hist1D_HitFit_SigTau_[ "Prob" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
        hist1D_HitFit_SigTau_[ "ProbLow" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
        hist1D_HitFit_SigTau_[ "ProbLog" ]->Fill( log10( ttSemiLeptonicEvent->hitFitProb() ) );
      }
      else {
        hist1D_HitFit_BkgNoTau_[ "nRealNuSol" ]->Fill( HitFitNRealNuSol );
        hist1D_HitFit_BkgNoTau_[ "Prob" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
        hist1D_HitFit_BkgNoTau_[ "ProbLow" ]->Fill( ttSemiLeptonicEvent->hitFitProb() );
        hist1D_HitFit_BkgNoTau_[ "ProbLog" ]->Fill( log10( ttSemiLeptonicEvent->hitFitProb() ) );
      }

      // Fill histograms only for converged fits
      if ( ttSemiLeptonicEvent->hitFitProb() >= 0. ) {

        hist1D_HitFit_Bkg_[ "Chi2" ]->Fill( ttSemiLeptonicEvent->hitFitChi2() );
        hist1D_HitFit_Bkg_[ "MT" ]->Fill( ttSemiLeptonicEvent->hitFitMT() );
        hist1D_HitFit_Bkg_[ "SigMT" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT() );
        hist1D_HitFit_Bkg_[ "SigMTRel" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );
        hist2D_HitFit_Bkg_[ "MT_SigMT" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT() );
        hist2D_HitFit_Bkg_[ "MT_SigMTRel" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );

        hist1D_HitFit_Bkg_[ "LepM" ]->Fill( HitFitLep->mass() );
        hist1D_HitFit_Bkg_[ "LepPt" ]->Fill( HitFitLep->pt() );
        hist1D_HitFit_Bkg_[ "LepEta" ]->Fill( HitFitLep->eta() );
        hist1D_HitFit_Bkg_[ "LepPhi" ]->Fill( HitFitLep->phi() );
        hist1D_HitFit_Bkg_[ "LepPz" ]->Fill( HitFitLep->pz() );
        hist1D_HitFit_Bkg_[ "NuM" ]->Fill( HitFitNu->mass() );
        hist1D_HitFit_Bkg_[ "NuPt" ]->Fill( HitFitNu->pt() );
        hist1D_HitFit_Bkg_[ "NuEta" ]->Fill( HitFitNu->eta() );
        hist1D_HitFit_Bkg_[ "NuPhi" ]->Fill( HitFitNu->phi() );
        hist1D_HitFit_Bkg_[ "NuPz" ]->Fill( HitFitNu->pz() );

        if ( DecayChn == WDecay::kTau + 1 ) {

          hist1D_HitFit_Tau_[ "Chi2" ]->Fill( ttSemiLeptonicEvent->hitFitChi2() );
          hist1D_HitFit_Tau_[ "MT" ]->Fill( ttSemiLeptonicEvent->hitFitMT() );
          hist1D_HitFit_Tau_[ "SigMT" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT() );
          hist1D_HitFit_Tau_[ "SigMTRel" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );
          hist2D_HitFit_Tau_[ "MT_SigMT" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT() );
          hist2D_HitFit_Tau_[ "MT_SigMTRel" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );

          hist1D_HitFit_Tau_[ "LepM" ]->Fill( HitFitLep->mass() );
          hist1D_HitFit_Tau_[ "LepPt" ]->Fill( HitFitLep->pt() );
          hist1D_HitFit_Tau_[ "LepEta" ]->Fill( HitFitLep->eta() );
          hist1D_HitFit_Tau_[ "LepPhi" ]->Fill( HitFitLep->phi() );
          hist1D_HitFit_Tau_[ "LepPz" ]->Fill( HitFitLep->pz() );
          hist1D_HitFit_Tau_[ "NuM" ]->Fill( HitFitNu->mass() );
          hist1D_HitFit_Tau_[ "NuPt" ]->Fill( HitFitNu->pt() );
          hist1D_HitFit_Tau_[ "NuEta" ]->Fill( HitFitNu->eta() );
          hist1D_HitFit_Tau_[ "NuPhi" ]->Fill( HitFitNu->phi() );
          hist1D_HitFit_Tau_[ "NuPz" ]->Fill( HitFitNu->pz() );

          hist1D_HitFit_SigTau_[ "Chi2" ]->Fill( ttSemiLeptonicEvent->hitFitChi2() );
          hist1D_HitFit_SigTau_[ "MT" ]->Fill( ttSemiLeptonicEvent->hitFitMT() );
          hist1D_HitFit_SigTau_[ "SigMT" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT() );
          hist1D_HitFit_SigTau_[ "SigMTRel" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );
          hist2D_HitFit_SigTau_[ "MT_SigMT" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT() );
          hist2D_HitFit_SigTau_[ "MT_SigMTRel" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );

          hist1D_HitFit_SigTau_[ "LepM" ]->Fill( HitFitLep->mass() );
          hist1D_HitFit_SigTau_[ "LepPt" ]->Fill( HitFitLep->pt() );
          hist1D_HitFit_SigTau_[ "LepEta" ]->Fill( HitFitLep->eta() );
          hist1D_HitFit_SigTau_[ "LepPhi" ]->Fill( HitFitLep->phi() );
          hist1D_HitFit_SigTau_[ "LepPz" ]->Fill( HitFitLep->pz() );
          hist1D_HitFit_SigTau_[ "NuM" ]->Fill( HitFitNu->mass() );
          hist1D_HitFit_SigTau_[ "NuPt" ]->Fill( HitFitNu->pt() );
          hist1D_HitFit_SigTau_[ "NuEta" ]->Fill( HitFitNu->eta() );
          hist1D_HitFit_SigTau_[ "NuPhi" ]->Fill( HitFitNu->phi() );
          hist1D_HitFit_SigTau_[ "NuPz" ]->Fill( HitFitNu->pz() );

        }
        else {

          hist1D_HitFit_BkgNoTau_[ "Chi2" ]->Fill( ttSemiLeptonicEvent->hitFitChi2() );
          hist1D_HitFit_BkgNoTau_[ "MT" ]->Fill( ttSemiLeptonicEvent->hitFitMT() );
          hist1D_HitFit_BkgNoTau_[ "SigMT" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT() );
          hist1D_HitFit_BkgNoTau_[ "SigMTRel" ]->Fill( ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );
          hist2D_HitFit_BkgNoTau_[ "MT_SigMT" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT() );
          hist2D_HitFit_BkgNoTau_[ "MT_SigMTRel" ]->Fill( ttSemiLeptonicEvent->hitFitMT(), ttSemiLeptonicEvent->hitFitSigMT()/ttSemiLeptonicEvent->hitFitMT() );

          hist1D_HitFit_BkgNoTau_[ "LepM" ]->Fill( HitFitLep->mass() );
          hist1D_HitFit_BkgNoTau_[ "LepPt" ]->Fill( HitFitLep->pt() );
          hist1D_HitFit_BkgNoTau_[ "LepEta" ]->Fill( HitFitLep->eta() );
          hist1D_HitFit_BkgNoTau_[ "LepPhi" ]->Fill( HitFitLep->phi() );
          hist1D_HitFit_BkgNoTau_[ "LepPz" ]->Fill( HitFitLep->pz() );
          hist1D_HitFit_BkgNoTau_[ "NuM" ]->Fill( HitFitNu->mass() );
          hist1D_HitFit_BkgNoTau_[ "NuPt" ]->Fill( HitFitNu->pt() );
          hist1D_HitFit_BkgNoTau_[ "NuEta" ]->Fill( HitFitNu->eta() );
          hist1D_HitFit_BkgNoTau_[ "NuPhi" ]->Fill( HitFitNu->phi() );
          hist1D_HitFit_BkgNoTau_[ "NuPz" ]->Fill( HitFitNu->pz() );

        }

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
