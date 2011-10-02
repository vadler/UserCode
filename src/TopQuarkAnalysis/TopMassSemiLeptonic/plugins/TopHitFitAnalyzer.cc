// -*- C++ -*-
//
// Package:    TopMassSemiLeptonic
// Class:      TopHitFitAnalyzer
//
// $Id: TopHitFitAnalyzer.cc,v 1.1 2011/08/31 14:45:53 vadler Exp $
//
/**
  \class    TopHitFitAnalyzer TopHitFitAnalyzer.cc "TopQuarkAnalysis/TopMassSemiLeptonic/plugins/TopHitFitAnalyzer.cc"
  \brief    Analyses HitFit performance



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

#include "AnalysisDataFormats/TopObjects/interface/TtSemiLeptonicEvent.h"


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
    // TQAF semi-leptonic event
    edm::Handle< TtSemiLeptonicEvent > ttSemiLeptonicEvent_;
    // TQAF MC event
    TtGenEvent * ttGenEvent_;
    // decay channel in real (MC) TTbar events
    unsigned decayChn_;
    bool     isSignal_;

    /// Configuration parameters
    // TQAF semi-leptonic event input tag
    edm::InputTag ttSemiLeptonicEventTag_;
    // HitFit validity
    unsigned binsHitFitNValid_;
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
    // GenMatch sum p_t
    unsigned binsGenMatchSumPt_;
    double   maxGenMatchSumPt_;
    // GenMatch sum Delta R
    unsigned binsGenMatchSumDR_;
    double   maxGenMatchSumDR_;
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
    std::map< std::string, TH1D* > hist1D_GenMatch_;
    std::map< std::string, TH2D* > hist2D_GenMatch_;
    std::map< std::string, TH1D* > hist1D_GenMatch_Sig_;
    std::map< std::string, TH2D* > hist2D_GenMatch_Sig_;
    std::map< std::string, TH1D* > hist1D_GenMatch_Tau_;
    std::map< std::string, TH2D* > hist2D_GenMatch_Tau_;
    std::map< std::string, TH1D* > hist1D_GenMatch_SigTau_;
    std::map< std::string, TH2D* > hist2D_GenMatch_SigTau_;
    std::map< std::string, TH1D* > hist1D_GenMatch_Bkg_;
    std::map< std::string, TH2D* > hist2D_GenMatch_Bkg_;
    std::map< std::string, TH1D* > hist1D_GenMatch_BkgNoTau_;
    std::map< std::string, TH2D* > hist2D_GenMatch_BkgNoTau_;
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
    std::map< std::string, TH2D* > hist2D_GenGenMatch_;

    /// Constants
    std::vector< std::string > decayChnNames_;

    /// Private functions

    void assignDecayChnNames( TAxis * iAxis );
    // Fill histograms
    void fill1D_GenMatch( std::map< std::string, TH1D* > & hist1D );
    void fill2D_GenMatch( std::map< std::string, TH2D* > & hist2D );
    void fill1D_GenMatch_Valid( std::map< std::string, TH1D* > & hist1D );
    void fill2D_GenMatch_Valid( std::map< std::string, TH2D* > & hist2D );
    void fill1D_Gen( std::map< std::string, TH1D* > & hist1D );
    void fill2D_GenHitFit( std::map< std::string, TH2D* > & hist2D );
    void fill2D_GenHitFit_Valid( std::map< std::string, TH2D* > & hist2D );
    void fill2D_GenGenMatch( std::map< std::string, TH2D* > & hist2D );
    void fill2D_GenGenMatch_Valid( std::map< std::string, TH2D* > & hist2D );
    void fill1D_Gen_Signal( std::map< std::string, TH1D* > & hist1D );
    void fill1D_GenHitFitDiff_SignalValid( std::map< std::string, TH1D* > & hist1D );
    void fill2D_Gen_Signal( std::map< std::string, TH2D* > & hist2D );
    void fill1D_HitFit( std::map< std::string, TH1D* > & hist1D );
    void fill2D_HitFit( std::map< std::string, TH2D* > & hist2D );
    void fill1D_HitFit_Valid( std::map< std::string, TH1D* > & hist1D );
    void fill2D_HitFit_Valid( std::map< std::string, TH2D* > & hist2D );

};


#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"


// Default constructor
TopHitFitAnalyzer::TopHitFitAnalyzer( const edm::ParameterSet & iConfig )
: ttSemiLeptonicEvent_()
, ttGenEvent_()
, decayChn_( WDecay::kNone )
, isSignal_( false )
  // get configuration parameters
, ttSemiLeptonicEventTag_( iConfig.getParameter< edm::InputTag >( "ttSemiLeptonicEvent" ) )
, binsHitFitNValid_( iConfig.getParameter< unsigned >( "binsHitFitNValid" ) )
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
, binsGenMatchSumPt_( iConfig.getParameter< unsigned >( "binsGenMatchSumPt" ) )
, maxGenMatchSumPt_( iConfig.getParameter< double >( "maxGenMatchSumPt" ) )
, binsGenMatchSumDR_( iConfig.getParameter< unsigned >( "binsGenMatchSumDR" ) )
, maxGenMatchSumDR_( iConfig.getParameter< double >( "maxGenMatchSumDR" ) )
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
, hist1D_GenMatch_()
, hist2D_GenMatch_()
, hist1D_GenMatch_Sig_()
, hist2D_GenMatch_Sig_()
, hist1D_GenMatch_Tau_()
, hist2D_GenMatch_Tau_()
, hist1D_GenMatch_SigTau_()
, hist2D_GenMatch_SigTau_()
, hist1D_GenMatch_Bkg_()
, hist2D_GenMatch_Bkg_()
, hist1D_GenMatch_BkgNoTau_()
, hist2D_GenMatch_BkgNoTau_()
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
, hist2D_GenGenMatch_()
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
  hist1D_HitFit_[ "nValid" ] = fileService->make< TH1D >( "HitFit_nValid", "HitFit valid hypotheses", binsHitFitNValid_ + 1, -0.5, binsHitFitNValid_ + 0.5 );
  hist1D_HitFit_[ "nValid" ]->SetXTitle( "valid hypotheses^{HitFit}" );
  hist1D_HitFit_[ "nValid" ]->SetYTitle( "events" );
  hist1D_HitFit_[ "Valid" ] = fileService->make< TH1D >( "HitFit_valid", "HitFit valid hypothesis", 2, -0.5, 1.5 );
  hist1D_HitFit_[ "Valid" ]->SetXTitle( "valid^{HitFit}" );
  hist1D_HitFit_[ "Valid" ]->SetYTitle( "events" );
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
  hist1D_HitFit_[ "TopLepM" ] = fileService->make< TH1D >( "HitFit_TopLepM", "HitFit leptonic top mass", binsHitFitMT_, minHitFitMT_, maxHitFitMT_ );
  hist1D_HitFit_[ "TopLepM" ]->SetXTitle( "m_{t, l}^{HitFit} (GeV)" );
  hist1D_HitFit_[ "TopLepM" ]->SetYTitle( "events" );
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
  hist1D_HitFit_[ "TopHadM" ] = fileService->make< TH1D >( "HitFit_TopHadM", "HitFit hadronic top mass", binsHitFitMT_, minHitFitMT_, maxHitFitMT_ );
  hist1D_HitFit_[ "TopHadM" ]->SetXTitle( "m_{t, h}^{HitFit} (GeV)" );
  hist1D_HitFit_[ "TopHadM" ]->SetYTitle( "events" );
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
  hist2D_HitFit_[ "Prob_Prob1" ] = fileService->make< TH2D >( "HitFit_Prob_Prob1", "HitFit probabilty(1) vs. HitFit probabilty", binsHitFitProb_, 0., 1., binsHitFitProb_, 0., 1. );
  hist2D_HitFit_[ "Prob_Prob1" ]->SetXTitle( "P^{HitFit}" );
  hist2D_HitFit_[ "Prob_Prob1" ]->SetYTitle( "P_{1}^{HitFit}" );
  hist2D_HitFit_[ "Prob_Prob1" ]->SetZTitle( "events" );
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
  hist1D_GenMatch_[ "Valid" ] = fileService->make< TH1D >( "GenMatch_Valid", "GenMatch valid hypothesis", 2, -0.5, 1.5 );
  hist1D_GenMatch_[ "Valid" ]->SetXTitle( "valid^{GenMatch}" );
  hist1D_GenMatch_[ "Valid" ]->SetYTitle( "events" );
  hist1D_GenMatch_[ "SumPt" ] = fileService->make< TH1D >( "GenMatch_SumPt", "GenMatch sum of transverse momenta", binsGenMatchSumPt_, 0., maxGenMatchSumPt_ );
  hist1D_GenMatch_[ "SumPt" ]->SetXTitle( "#sum p_{t}^{GenMatch}" );
  hist1D_GenMatch_[ "SumPt" ]->SetYTitle( "events" );
  hist1D_GenMatch_[ "SumDR" ] = fileService->make< TH1D >( "GenMatch_SumDR", "GenMatch sum of spacial deviations", binsGenMatchSumDR_, 0., maxGenMatchSumDR_ );
  hist1D_GenMatch_[ "SumDR" ]->SetXTitle( "#sum #Delta R^{GenMatch}" );
  hist1D_GenMatch_[ "SumDR" ]->SetYTitle( "events" );
  hist1D_GenMatch_[ "TopLepM" ] = fileService->make< TH1D >( "GenMatch_TopLepM", "GenMatch muonic top mass", binsTopM_, minTopM_, maxTopM_ );
  hist1D_GenMatch_[ "TopLepM" ]->SetXTitle( "m_{t_{l}}^{gen.} (GeV)" );
  hist1D_GenMatch_[ "TopLepM" ]->SetYTitle( "events" );
  hist1D_GenMatch_[ "LepM" ] = fileService->make< TH1D >( "GenMatch_LepM", "GenMatch lepton mass", binsGenLepM_, 0., maxGenLepM_ );
  hist1D_GenMatch_[ "LepM" ]->SetXTitle( "m_{l}^{gen.} (GeV)" );
  hist1D_GenMatch_[ "LepM" ]->SetYTitle( "events" );
  hist1D_GenMatch_[ "LepPt" ] = fileService->make< TH1D >( "GenMatch_LepPt", "GenMatch lepton transverse momentum", binsLepPt_, 0., maxLepPt_ );
  hist1D_GenMatch_[ "LepPt" ]->SetXTitle( "p_{t, l}^{gen.} (GeV)" );
  hist1D_GenMatch_[ "LepPt" ]->SetYTitle( "events" );
  hist1D_GenMatch_[ "LepEta" ] = fileService->make< TH1D >( "GenMatch_LepEta", "GenMatch lepton pseudo-rapidity", binsLepEta_, -maxLepEta_, maxLepEta_ );
  hist1D_GenMatch_[ "LepEta" ]->SetXTitle( "#eta_{l}^{gen.}" );
  hist1D_GenMatch_[ "LepEta" ]->SetYTitle( "events" );
  hist1D_GenMatch_[ "LepPhi" ] = fileService->make< TH1D >( "GenMatch_LepPhi", "GenMatch lepton azimutal angle", binsLepPhi_, -TMath::Pi(), TMath::Pi() );
  hist1D_GenMatch_[ "LepPhi" ]->SetXTitle( "#phi_{l}^{gen.}" );
  hist1D_GenMatch_[ "LepPhi" ]->SetYTitle( "events" );
  hist1D_GenMatch_[ "LepPz" ] = fileService->make< TH1D >( "GenMatch_LepPz", "GenMatch lepton longitudinal momentum", binsLepPz_, -maxLepPz_, maxLepPz_ );
  hist1D_GenMatch_[ "LepPz" ]->SetXTitle( "p_{z, l}^{gen.} (GeV)" );
  hist1D_GenMatch_[ "LepPz" ]->SetYTitle( "events" );
  hist1D_GenMatch_[ "NuM" ] = fileService->make< TH1D >( "GenMatch_NuM", "GenMatch neutrino mass", binsNuM_, -maxNuM_, maxNuM_ );
  hist1D_GenMatch_[ "NuM" ]->SetXTitle( "m_{#nu}^{gen.} (GeV)" );
  hist1D_GenMatch_[ "NuM" ]->SetYTitle( "events" );
  hist1D_GenMatch_[ "NuPt" ] = fileService->make< TH1D >( "GenMatch_NuPt", "GenMatch neutrino transverse momentum", binsNuPt_, 0., maxNuPt_ );
  hist1D_GenMatch_[ "NuPt" ]->SetXTitle( "p_{t, #nu}^{gen.} (GeV)" );
  hist1D_GenMatch_[ "NuPt" ]->SetYTitle( "events" );
  hist1D_GenMatch_[ "NuEta" ] = fileService->make< TH1D >( "GenMatch_NuEta", "GenMatch neutrino pseudo-rapidity", binsNuEta_, -maxNuEta_, maxNuEta_ );
  hist1D_GenMatch_[ "NuEta" ]->SetXTitle( "#eta_{#nu}^{gen.}" );
  hist1D_GenMatch_[ "NuEta" ]->SetYTitle( "events" );
  hist1D_GenMatch_[ "NuPhi" ] = fileService->make< TH1D >( "GenMatch_NuPhi", "GenMatch neutrino azimutal angle", binsNuPhi_, -TMath::Pi(), TMath::Pi() );
  hist1D_GenMatch_[ "NuPhi" ]->SetXTitle( "#phi_{#nu}^{gen.}" );
  hist1D_GenMatch_[ "NuPhi" ]->SetYTitle( "events" );
  hist1D_GenMatch_[ "NuPz" ] = fileService->make< TH1D >( "GenMatch_NuPz", "GenMatch neutrino longitudinal momentum", binsNuPz_, -maxNuPz_, maxNuPz_ );
  hist1D_GenMatch_[ "NuPz" ]->SetXTitle( "p_{z, #nu}^{gen.} (GeV)" );
  hist1D_GenMatch_[ "NuPz" ]->SetYTitle( "events" );
  hist1D_GenMatch_[ "TopHadM" ] = fileService->make< TH1D >( "GenMatch_TopHadM", "GenMatch hadronic top mass", binsTopM_, minTopM_, maxTopM_ );
  hist1D_GenMatch_[ "TopHadM" ]->SetXTitle( "m_{t_{h}}^{gen.} (GeV)" );
  hist1D_GenMatch_[ "TopHadM" ]->SetYTitle( "events" );
  hist1D_Gen_[ "decayChn_" ] = fileService->make< TH1D >( "Gen_DecayChn", "Decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1. );
  hist1D_Gen_[ "decayChn_" ]->SetXTitle( "" );
  assignDecayChnNames( hist1D_Gen_[ "decayChn_" ]->GetXaxis() );
  hist1D_Gen_[ "decayChn_" ]->SetYTitle( "events" );
  hist1D_Gen_[ "TopLepM" ] = fileService->make< TH1D >( "Gen_TopLepM", "Generated muonic top mass", binsTopM_, minTopM_, maxTopM_ );
  hist1D_Gen_[ "TopLepM" ]->SetXTitle( "m_{t_{l}}^{gen.} (GeV)" );
  hist1D_Gen_[ "TopLepM" ]->SetYTitle( "events" );
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
  hist1D_Gen_[ "TopHadM" ] = fileService->make< TH1D >( "Gen_TopHadM", "Generated hadronic top mass", binsTopM_, minTopM_, maxTopM_ );
  hist1D_Gen_[ "TopHadM" ]->SetXTitle( "m_{t_{h}}^{gen.} (GeV)" );
  hist1D_Gen_[ "TopHadM" ]->SetYTitle( "events" );
  hist1D_Diff_[ "TopLepM" ] = fileService->make< TH1D >( "Diff_TopLepM", "HitFit leptonic top mass deviation", binsDiffTopM_, -maxDiffTopM_, maxDiffTopM_ );
  hist1D_Diff_[ "TopLepM" ]->SetXTitle( "#Delta m_{t_{l}} (GeV)" );
  hist1D_Diff_[ "TopLepM" ]->SetYTitle( "events" );
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
  hist1D_Diff_[ "TopHadM" ] = fileService->make< TH1D >( "Diff_TopHadM", "HitFit hadronic top mass deviation", binsDiffTopM_, -maxDiffTopM_, maxDiffTopM_ );
  hist1D_Diff_[ "TopHadM" ]->SetXTitle( "#Delta m_{t_{h}} (GeV)" );
  hist1D_Diff_[ "TopHadM" ]->SetYTitle( "events" );
  for ( std::map< std::string, TH1D* >::const_iterator iHist = hist1D_HitFit_.begin(); iHist != hist1D_HitFit_.end(); ++iHist ) {
    std::string nameSig( iHist->second->GetName() );
    hist1D_HitFit_Sig_[ iHist->first ] = fileService->make< TH1D >( *( ( TH1D* )( iHist->second->Clone( nameSig.replace( 6, 0, "_Sig" ).c_str() ) ) ) );
    std::string nameTau( iHist->second->GetName() );
    hist1D_HitFit_Tau_[ iHist->first ] = fileService->make< TH1D >( *( ( TH1D* )( iHist->second->Clone( nameTau.replace( 6, 0, "_Tau" ).c_str() ) ) ) );
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
    hist2D_HitFit_Tau_[ iHist->first ] = fileService->make< TH2D >( *( ( TH2D* )( iHist->second->Clone( nameTau.replace( 6, 0, "_Tau" ).c_str() ) ) ) );
    std::string nameSigTau( iHist->second->GetName() );
    hist2D_HitFit_SigTau_[ iHist->first ] = fileService->make< TH2D >( *( ( TH2D* )( iHist->second->Clone( nameSigTau.replace( 6, 0, "_SigTau" ).c_str() ) ) ) );
    std::string nameBkg( iHist->second->GetName() );
    hist2D_HitFit_Bkg_[ iHist->first ] = fileService->make< TH2D >( *( ( TH2D* )( iHist->second->Clone( nameBkg.replace( 6, 0, "_Bkg" ).c_str() ) ) ) );
    std::string nameBkgNoTau( iHist->second->GetName() );
    hist2D_HitFit_BkgNoTau_[ iHist->first ] = fileService->make< TH2D >( *( ( TH2D* )( iHist->second->Clone( nameBkgNoTau.replace( 6, 0, "_BkgNoTau" ).c_str() ) ) ) );
  }
  for ( std::map< std::string, TH1D* >::const_iterator iHist = hist1D_GenMatch_.begin(); iHist != hist1D_GenMatch_.end(); ++iHist ) {
    std::string nameSig( iHist->second->GetName() );
    hist1D_GenMatch_Sig_[ iHist->first ] = fileService->make< TH1D >( *( ( TH1D* )( iHist->second->Clone( nameSig.replace( 8, 0, "_Sig" ).c_str() ) ) ) );
    std::string nameTau( iHist->second->GetName() );
    hist1D_GenMatch_Tau_[ iHist->first ] = fileService->make< TH1D >( *( ( TH1D* )( iHist->second->Clone( nameTau.replace( 8, 0, "_Tau" ).c_str() ) ) ) );
    std::string nameSigTau( iHist->second->GetName() );
    hist1D_GenMatch_SigTau_[ iHist->first ] = fileService->make< TH1D >( *( ( TH1D* )( iHist->second->Clone( nameSigTau.replace( 8, 0, "_SigTau" ).c_str() ) ) ) );
    std::string nameBkg( iHist->second->GetName() );
    hist1D_GenMatch_Bkg_[ iHist->first ] = fileService->make< TH1D >( *( ( TH1D* )( iHist->second->Clone( nameBkg.replace( 8, 0, "_Bkg" ).c_str() ) ) ) );
    std::string nameBkgNoTau( iHist->second->GetName() );
    hist1D_GenMatch_BkgNoTau_[ iHist->first ] = fileService->make< TH1D >( *( ( TH1D* )( iHist->second->Clone( nameBkgNoTau.replace( 8, 0, "_BkgNoTau" ).c_str() ) ) ) );
  }
  // 2-dim
  for ( std::map< std::string, TH2D* >::const_iterator iHist = hist2D_GenMatch_.begin(); iHist != hist2D_GenMatch_.end(); ++iHist ) {
    std::string nameSig( iHist->second->GetName() );
    hist2D_GenMatch_Sig_[ iHist->first ] = fileService->make< TH2D >( *( ( TH2D* )( iHist->second->Clone( nameSig.replace( 8, 0, "_Sig" ).c_str() ) ) ) );
    std::string nameTau( iHist->second->GetName() );
    hist2D_GenMatch_Tau_[ iHist->first ] = fileService->make< TH2D >( *( ( TH2D* )( iHist->second->Clone( nameTau.replace( 8, 0, "_Tau" ).c_str() ) ) ) );
    std::string nameSigTau( iHist->second->GetName() );
    hist2D_GenMatch_SigTau_[ iHist->first ] = fileService->make< TH2D >( *( ( TH2D* )( iHist->second->Clone( nameSigTau.replace( 8, 0, "_SigTau" ).c_str() ) ) ) );
    std::string nameBkg( iHist->second->GetName() );
    hist2D_GenMatch_Bkg_[ iHist->first ] = fileService->make< TH2D >( *( ( TH2D* )( iHist->second->Clone( nameBkg.replace( 8, 0, "_Bkg" ).c_str() ) ) ) );
    std::string nameBkgNoTau( iHist->second->GetName() );
    hist2D_GenMatch_BkgNoTau_[ iHist->first ] = fileService->make< TH2D >( *( ( TH2D* )( iHist->second->Clone( nameBkgNoTau.replace( 8, 0, "_BkgNoTau" ).c_str() ) ) ) );
  }
  hist2D_GenHitFit_[ "DecayChn_nValid" ] = fileService->make< TH2D >( "DecayChn_HitFit_nValid", "HitFit valid hypotheses vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsHitFitNValid_ + 1, -0.5, binsHitFitNValid_ + 0.5 );
  hist2D_GenHitFit_[ "DecayChn_nValid" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_nValid" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_nValid" ]->SetYTitle( "valid hypotheses^{HitFit}" );
  hist2D_GenHitFit_[ "DecayChn_nValid" ]->SetZTitle( "events" );
  hist2D_GenHitFit_[ "DecayChn_Valid" ] = fileService->make< TH2D >( "DecayChn_HitFit_valid", "HitFit valid hypothesis vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., 2, -0.5, 1.5 );
  hist2D_GenHitFit_[ "DecayChn_Valid" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_Valid" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_Valid" ]->SetYTitle( "valid^{HitFit}" );
  hist2D_GenHitFit_[ "DecayChn_Valid" ]->SetZTitle( "events" );
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
  hist2D_GenHitFit_[ "DecayChn_TopLepM" ] = fileService->make< TH2D >( "DecayChn_TopLepM", "HitFit leptonic top mass vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsHitFitMT_, minHitFitMT_, maxHitFitMT_ );
  hist2D_GenHitFit_[ "DecayChn_TopLepM" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_TopLepM" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_TopLepM" ]->SetYTitle( "m_{t, l}^{HitFit} (GeV)" );
  hist2D_GenHitFit_[ "DecayChn_TopLepM" ]->SetZTitle( "events" );
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
  hist2D_GenHitFit_[ "DecayChn_TopHadM" ] = fileService->make< TH2D >( "DecayChn_TopHadM", "HitFit hadronic top mass vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsHitFitMT_, minHitFitMT_, maxHitFitMT_ );
  hist2D_GenHitFit_[ "DecayChn_TopHadM" ]->SetXTitle( "" );
  assignDecayChnNames( hist2D_GenHitFit_[ "DecayChn_TopHadM" ]->GetXaxis() );
  hist2D_GenHitFit_[ "DecayChn_TopHadM" ]->SetYTitle( "m_{t, h}^{HitFit} (GeV)" );
  hist2D_GenHitFit_[ "DecayChn_TopHadM" ]->SetZTitle( "events" );
  hist2D_GenGenMatch_[ "DecayChn_Valid" ] = fileService->make< TH2D >( "DecayChn_GenMatch_Valid", "GenMatch valid hypothesis vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., 2, -0.5, 1.5 );
  assignDecayChnNames( hist2D_GenGenMatch_[ "DecayChn_Valid" ]->GetXaxis() );
  hist2D_GenGenMatch_[ "DecayChn_Valid" ]->SetYTitle( "valid^{GenMatch}" );
  hist2D_GenGenMatch_[ "DecayChn_Valid" ]->SetZTitle( "events" );
  hist2D_GenGenMatch_[ "DecayChn_SumPt" ] = fileService->make< TH2D >( "DecayChn_GenMatch_SumPt", "GenMatch sum of transverse momenta vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsGenMatchSumPt_, 0., maxGenMatchSumPt_ );
  assignDecayChnNames( hist2D_GenGenMatch_[ "DecayChn_SumPt" ]->GetXaxis() );
  hist2D_GenGenMatch_[ "DecayChn_SumPt" ]->SetYTitle( "#sum p_{t}^{GenMatch}" );
  hist2D_GenGenMatch_[ "DecayChn_SumPt" ]->SetZTitle( "events" );
  hist2D_GenGenMatch_[ "DecayChn_SumDR" ] = fileService->make< TH2D >( "DecayChn_GenMatch_SumDR", "GenMatch sum of spacial deviations vs. decay channel", decayChnNames_.size()-1, 0., decayChnNames_.size()-1., binsGenMatchSumDR_, 0., maxGenMatchSumDR_ );
  assignDecayChnNames( hist2D_GenGenMatch_[ "DecayChn_SumDR" ]->GetXaxis() );
  hist2D_GenGenMatch_[ "DecayChn_SumDR" ]->SetYTitle( "#sum #Delta R^{GenMatch}" );
  hist2D_GenGenMatch_[ "DecayChn_SumDR" ]->SetZTitle( "events" );

}


// Event loop
void TopHitFitAnalyzer::analyze( const edm::Event & iEvent, const edm::EventSetup & iSetup )
{

  // Reset
  ttGenEvent_ = 0;
  isSignal_   = false;

  // TQAF semi-leptonic event
  iEvent.getByLabel( ttSemiLeptonicEventTag_, ttSemiLeptonicEvent_ );

  // HitFit event hypothesis
  unsigned HitFitNHyposValid( 0 );
  for ( unsigned iHypo = 0; iHypo < ttSemiLeptonicEvent_->numberOfAvailableHypos( TtEvent::kHitFit ); ++iHypo ) {
    if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit, iHypo ) )
      ++HitFitNHyposValid;
  }
  hist1D_HitFit_[ "nValid" ]->Fill( HitFitNHyposValid );

  fill1D_HitFit( hist1D_HitFit_ );

  // Fill histograms only for converged fits
  if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit ) ) {

    fill1D_HitFit_Valid( hist1D_HitFit_ );
    fill2D_HitFit_Valid( hist2D_HitFit_ );

  }

  // MC specific (using true information)

  if ( ! iEvent.isRealData() ) {

    fill1D_GenMatch( hist1D_GenMatch_ );

    if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kGenMatch ) ) {

      fill1D_GenMatch_Valid( hist1D_GenMatch_ );

    }

    // Generated event
    ttGenEvent_ = const_cast< TtGenEvent* >( ttSemiLeptonicEvent_->genEvent().get() );
    // Proceed only for ttbar events
    if ( ttGenEvent_->isTtBar() ) {

      // Decay channels
      // FIXME: This is still muon-specific
      unsigned nTrueOtherMuon( 0 );
      if ( ttGenEvent_->isSemiLeptonic( WDecay::kElec ) ) {
        decayChn_ = ttGenEvent_->semiLeptonicChannel();
      }
      else if ( ttGenEvent_->isSemiLeptonic( WDecay::kMuon ) ) {
        decayChn_ = ttGenEvent_->semiLeptonicChannel();
        isSignal_ = true;
      }
      else if ( ttGenEvent_->isSemiLeptonic( WDecay::kTau ) ) {
        decayChn_ = ttGenEvent_->semiLeptonicChannel();
        const reco::GenParticle * genSemiTau( ttGenEvent_->singleLepton() );
        for ( size_t iD = 0; iD < genSemiTau->numberOfDaughters(); ++iD ) {
          const reco::Candidate * genTauDaughter( genSemiTau->daughter( iD ) );
          // First level daughter is a copy: need to go one level deeper
          for ( size_t iDD = 0; iDD < genTauDaughter->numberOfDaughters(); ++iDD ) {
            const reco::Candidate * genTauGrandDaughter( genTauDaughter->daughter( iDD ) );
            if ( fabs( genTauGrandDaughter->pdgId() ) == 13 ) ++nTrueOtherMuon;
          }
        }
        decayChn_ += nTrueOtherMuon;
      }
      else if ( ttGenEvent_->isFullLeptonic() ) {
        decayChn_ = WDecay::kTau + 2;
        const reco::GenParticle * genFullLep( ttGenEvent_->lepton() );
        if ( fabs( genFullLep->pdgId() ) == 15 ) {
          for ( size_t iD = 0; iD < genFullLep->numberOfDaughters(); ++iD ) {
            const reco::Candidate * genTauDaughter( genFullLep->daughter( iD ) );
            for ( size_t iDD = 0; iDD < genTauDaughter->numberOfDaughters(); ++iDD ) {
              const reco::Candidate * genTauGrandDaughter( genTauDaughter->daughter( iDD ) );
              if ( fabs( genTauGrandDaughter->pdgId() ) == 13 ) ++nTrueOtherMuon;
            }
          }
        }
        else if ( fabs( genFullLep->pdgId() ) == 13 ) {
           ++nTrueOtherMuon;
        }
        const reco::GenParticle * genFullLepBar( ttGenEvent_->leptonBar() );
        if ( fabs( genFullLep->pdgId() ) == 15 ) {
          for ( size_t iD = 0; iD < genFullLepBar->numberOfDaughters(); ++iD ) {
            const reco::Candidate * genTauDaughter( genFullLepBar->daughter( iD ) );
            for ( size_t iDD = 0; iDD < genTauDaughter->numberOfDaughters(); ++iDD ) {
              const reco::Candidate * genTauGrandDaughter( genTauDaughter->daughter( iDD ) );
              if ( fabs( genTauGrandDaughter->pdgId() ) == 13 ) ++nTrueOtherMuon;
            }
          }
        }
        else if ( fabs( genFullLepBar->pdgId() ) == 13 ) {
           ++nTrueOtherMuon;
        }
        decayChn_ += nTrueOtherMuon;
      }

      fill1D_Gen( hist1D_Gen_ );
      fill2D_GenHitFit( hist2D_GenHitFit_ );

      // Fill histograms only for converged fits
      if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit ) ) {

        fill2D_GenHitFit_Valid( hist2D_GenHitFit_ );

      }

      fill2D_GenGenMatch( hist2D_GenGenMatch_ );

      if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kGenMatch ) ) {

        fill2D_GenGenMatch_Valid( hist2D_GenGenMatch_ );

      }

    }

    // Signal
    if ( isSignal_ ) {

      fill1D_HitFit( hist1D_HitFit_Sig_ );
      fill1D_HitFit( hist1D_HitFit_SigTau_ );

      // Fill histograms only for converged fits
      if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit ) ) {

        fill1D_HitFit_Valid( hist1D_HitFit_Sig_ );
        fill2D_HitFit_Valid( hist2D_HitFit_Sig_ );

        fill1D_HitFit_Valid( hist1D_HitFit_SigTau_ );
        fill2D_HitFit_Valid( hist2D_HitFit_SigTau_ );

        fill1D_GenHitFitDiff_SignalValid( hist1D_Diff_ );

      }

      fill1D_GenMatch( hist1D_GenMatch_Sig_ );

      fill1D_GenMatch( hist1D_GenMatch_SigTau_ );

      if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kGenMatch ) ) {

        fill1D_GenMatch_Valid( hist1D_GenMatch_Sig_ );

        fill1D_GenMatch_Valid( hist1D_GenMatch_SigTau_ );

      }

      fill1D_Gen_Signal( hist1D_Gen_ );

    }
    // Background
    else {

      fill1D_HitFit( hist1D_HitFit_Bkg_ );

      if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit ) ) {

        fill1D_HitFit_Valid( hist1D_HitFit_Bkg_ );
        fill2D_HitFit_Valid( hist2D_HitFit_Bkg_ );

      }

      fill1D_GenMatch( hist1D_GenMatch_Bkg_ );

      if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kGenMatch ) ) {

        fill1D_GenMatch_Valid( hist1D_GenMatch_Bkg_ );

      }

      // Semi-tau to mu
      if ( decayChn_ == WDecay::kTau + 1 ) {

        fill1D_HitFit( hist1D_HitFit_Tau_ );
        fill1D_HitFit( hist1D_HitFit_SigTau_ );

        if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit ) ) {

          fill1D_HitFit_Valid( hist1D_HitFit_Tau_ );
          fill2D_HitFit_Valid( hist2D_HitFit_Tau_ );

          fill1D_HitFit_Valid( hist1D_HitFit_SigTau_ );
          fill2D_HitFit_Valid( hist2D_HitFit_SigTau_ );

        }

        fill1D_GenMatch( hist1D_GenMatch_Tau_ );

        fill1D_GenMatch( hist1D_GenMatch_SigTau_ );

        if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kGenMatch ) ) {

          fill1D_GenMatch_Valid( hist1D_GenMatch_Tau_ );

          fill1D_GenMatch_Valid( hist1D_GenMatch_SigTau_ );

        }

      }
      else {

        fill1D_HitFit( hist1D_HitFit_BkgNoTau_ );

        if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit ) ) {

          fill1D_HitFit_Valid( hist1D_HitFit_BkgNoTau_ );
          fill2D_HitFit_Valid( hist2D_HitFit_BkgNoTau_ );

        }

        fill1D_GenMatch( hist1D_GenMatch_BkgNoTau_ );

        if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kGenMatch ) ) {

          fill1D_GenMatch_Valid( hist1D_GenMatch_BkgNoTau_ );

        }

      }

      // Fill histograms only for converged fits
      if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit ) ) {

        fill1D_HitFit_Valid( hist1D_HitFit_Bkg_ );
        fill2D_HitFit_Valid( hist2D_HitFit_Bkg_ );

        // Semi-tau to mu
        if ( decayChn_ == WDecay::kTau + 1 ) {

          fill1D_HitFit_Valid( hist1D_HitFit_Tau_ );
          fill2D_HitFit_Valid( hist2D_HitFit_Tau_ );

          fill1D_HitFit_Valid( hist1D_HitFit_SigTau_ );
          fill2D_HitFit_Valid( hist2D_HitFit_SigTau_ );

        }
        else {

          fill1D_HitFit_Valid( hist1D_HitFit_BkgNoTau_ );
          fill2D_HitFit_Valid( hist2D_HitFit_BkgNoTau_ );

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


void TopHitFitAnalyzer::fill1D_GenMatch( std::map< std::string, TH1D* > & hist1D )
{

  hist1D[ "Valid" ]->Fill( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kGenMatch ) );

}


void TopHitFitAnalyzer::fill2D_GenMatch( std::map< std::string, TH2D* > & hist2D )
{

  return;

}


void TopHitFitAnalyzer::fill1D_GenMatch_Valid( std::map< std::string, TH1D* > & hist1D )
{

  hist1D[ "SumPt" ]->Fill( ttSemiLeptonicEvent_->genMatchSumPt() );
  hist1D[ "SumDR" ]->Fill( ttSemiLeptonicEvent_->genMatchSumDR() );

}


void TopHitFitAnalyzer::fill2D_GenMatch_Valid( std::map< std::string, TH2D* > & hist2D )
{

  return;

}


void TopHitFitAnalyzer::fill1D_Gen( std::map< std::string, TH1D* > & hist1D )
{

  hist1D[ "decayChn_" ]->Fill( decayChn_ );

}


void TopHitFitAnalyzer::fill2D_GenHitFit( std::map< std::string, TH2D* > & hist2D )
{

  hist2D[ "DecayChn_Valid" ]->Fill( decayChn_, ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit ) );

}


void TopHitFitAnalyzer::fill2D_GenHitFit_Valid( std::map< std::string, TH2D* > & hist2D )
{

  hist2D[ "DecayChn_nRealNuSol" ]->Fill( decayChn_, ttSemiLeptonicEvent_->numberOfRealNeutrinoSolutions( TtEvent::kHitFit ) );
  hist2D[ "DecayChn_Prob" ]->Fill( decayChn_, ttSemiLeptonicEvent_->hitFitProb() );
  hist2D[ "DecayChn_ProbLow" ]->Fill( decayChn_, ttSemiLeptonicEvent_->hitFitProb() );
  hist2D[ "DecayChn_ProbLog" ]->Fill( decayChn_, log10( ttSemiLeptonicEvent_->hitFitProb() ) );
  hist2D[ "DecayChn_Chi2" ]->Fill( decayChn_, ttSemiLeptonicEvent_->hitFitChi2() );
  hist2D[ "DecayChn_MT" ]->Fill( decayChn_, ttSemiLeptonicEvent_->hitFitMT() );
  hist2D[ "DecayChn_SigMT" ]->Fill( decayChn_, ttSemiLeptonicEvent_->hitFitSigMT() );
  hist2D[ "DecayChn_SigMTRel" ]->Fill( decayChn_, ttSemiLeptonicEvent_->hitFitMT(), ttSemiLeptonicEvent_->hitFitSigMT()/ttSemiLeptonicEvent_->hitFitMT() );

  hist2D[ "DecayChn_TopLepM" ]->Fill( decayChn_, ttSemiLeptonicEvent_->leptonicDecayTop( TtEvent::kHitFit )->mass() );
  hist2D[ "DecayChn_LepM" ]->Fill( decayChn_, ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit )->mass() );
  hist2D[ "DecayChn_LepPt" ]->Fill( decayChn_, ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit )->pt() );
  hist2D[ "DecayChn_LepEta" ]->Fill( decayChn_, ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit )->eta() );
  hist2D[ "DecayChn_LepPhi" ]->Fill( decayChn_, ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit )->phi() );
  hist2D[ "DecayChn_LepPz" ]->Fill( decayChn_, ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit )->pz() );
  hist2D[ "DecayChn_NuM" ]->Fill( decayChn_, ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit )->mass() );
  hist2D[ "DecayChn_NuPt" ]->Fill( decayChn_, ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit )->pt() );
  hist2D[ "DecayChn_NuEta" ]->Fill( decayChn_, ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit )->eta() );
  hist2D[ "DecayChn_NuPhi" ]->Fill( decayChn_, ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit )->phi() );
  hist2D[ "DecayChn_NuPz" ]->Fill( decayChn_, ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit )->pz() );
  hist2D[ "DecayChn_TopHadM" ]->Fill( decayChn_, ttSemiLeptonicEvent_->hadronicDecayTop( TtEvent::kHitFit )->mass() );

}


void TopHitFitAnalyzer::fill2D_GenGenMatch( std::map< std::string, TH2D* > & hist2D )
{

  hist2D[ "DecayChn_Valid" ]->Fill( decayChn_, ttSemiLeptonicEvent_->isHypoValid( TtEvent::kGenMatch ) );

}


void TopHitFitAnalyzer::fill2D_GenGenMatch_Valid( std::map< std::string, TH2D* > & hist2D )
{

  hist2D[ "DecayChn_SumPt" ]->Fill( decayChn_, ttSemiLeptonicEvent_->genMatchSumPt() );
  hist2D[ "DecayChn_SumDR" ]->Fill( decayChn_, ttSemiLeptonicEvent_->genMatchSumDR() );

}


void TopHitFitAnalyzer::fill1D_Gen_Signal( std::map< std::string, TH1D* > & hist1D )
{

  // FIXME: This is still muon-specific
  hist1D[ "TopLepM" ]->Fill( ttGenEvent_->leptonicDecayTop()->mass() );
  hist1D[ "LepM" ]->Fill( ttGenEvent_->singleLepton()->mass() );
  hist1D[ "LepPt" ]->Fill( ttGenEvent_->singleLepton()->pt() );
  hist1D[ "LepEta" ]->Fill( ttGenEvent_->singleLepton()->eta() );
  hist1D[ "LepPhi" ]->Fill( ttGenEvent_->singleLepton()->phi() );
  hist1D[ "LepPz" ]->Fill( ttGenEvent_->singleLepton()->pz() );
  hist1D[ "NuM" ]->Fill( ttGenEvent_->singleNeutrino()->mass() );
  hist1D[ "NuPt" ]->Fill( ttGenEvent_->singleNeutrino()->pt() );
  hist1D[ "NuEta" ]->Fill( ttGenEvent_->singleNeutrino()->eta() );
  hist1D[ "NuPhi" ]->Fill( ttGenEvent_->singleNeutrino()->phi() );
  hist1D[ "NuPz" ]->Fill( ttGenEvent_->singleNeutrino()->pz() );
  hist1D[ "TopHadM" ]->Fill( ttGenEvent_->hadronicDecayTop()->mass() );

}


void TopHitFitAnalyzer::fill1D_GenHitFitDiff_SignalValid( std::map< std::string, TH1D* > & hist1D )
{

  hist1D[ "TopLepM" ]->Fill( ttSemiLeptonicEvent_->hitFitMT() - ttGenEvent_->leptonicDecayTop()->mass() );
  hist1D[ "LepM" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit )->mass() - ttGenEvent_->singleLepton()->mass() );
  hist1D[ "LepPt" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit )->pt() - ttGenEvent_->singleLepton()->pt() );
  hist1D[ "LepEta" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit )->eta() - ttGenEvent_->singleLepton()->eta() );
  hist1D[ "LepPhi" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit )->phi() - ttGenEvent_->singleLepton()->phi() );
  hist1D[ "LepPz" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit )->pz() - ttGenEvent_->singleLepton()->pz() );
  hist1D[ "NuM" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit )->mass() - ttGenEvent_->singleNeutrino()->mass() );
  hist1D[ "NuPt" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit )->pt() - ttGenEvent_->singleNeutrino()->pt() );
  hist1D[ "NuEta" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit )->eta() - ttGenEvent_->singleNeutrino()->eta() );
  hist1D[ "NuPhi" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit )->phi() - ttGenEvent_->singleNeutrino()->phi() );
  hist1D[ "NuPz" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit )->pz() - ttGenEvent_->singleNeutrino()->pz() );
  hist1D[ "TopHadM" ]->Fill( ttSemiLeptonicEvent_->hitFitMT() - ttGenEvent_->hadronicDecayTop()->mass() );

}


void TopHitFitAnalyzer::fill2D_Gen_Signal( std::map< std::string, TH2D* > & hist2D )
{

  return;

}


void TopHitFitAnalyzer::fill1D_HitFit( std::map< std::string, TH1D* > & hist1D )
{

  hist1D[ "Valid" ]->Fill( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit ) );

}


void TopHitFitAnalyzer::fill2D_HitFit( std::map< std::string, TH2D* > & hist2D )
{

  return;

}


void TopHitFitAnalyzer::fill1D_HitFit_Valid( std::map< std::string, TH1D* > & hist1D )
{

  hist1D[ "nRealNuSol" ]->Fill( ttSemiLeptonicEvent_->numberOfRealNeutrinoSolutions( TtEvent::kHitFit ) );
  hist1D[ "Prob" ]->Fill( ttSemiLeptonicEvent_->hitFitProb() );
  hist1D[ "ProbLow" ]->Fill( ttSemiLeptonicEvent_->hitFitProb() );
  hist1D[ "ProbLog" ]->Fill( log10( ttSemiLeptonicEvent_->hitFitProb() ) );
  hist1D[ "Chi2" ]->Fill( ttSemiLeptonicEvent_->hitFitChi2() );
  hist1D[ "MT" ]->Fill( ttSemiLeptonicEvent_->hitFitMT() );
  hist1D[ "SigMT" ]->Fill( ttSemiLeptonicEvent_->hitFitSigMT() );
  hist1D[ "SigMTRel" ]->Fill( ttSemiLeptonicEvent_->hitFitSigMT()/ttSemiLeptonicEvent_->hitFitMT() );

  hist1D[ "TopLepM" ]->Fill( ttSemiLeptonicEvent_->leptonicDecayTop( TtEvent::kHitFit )->mass() );
  hist1D[ "LepM" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit )->mass() );
  hist1D[ "LepPt" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit )->pt() );
  hist1D[ "LepEta" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit )->eta() );
  hist1D[ "LepPhi" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit )->phi() );
  hist1D[ "LepPz" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit )->pz() );
  hist1D[ "NuM" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit )->mass() );
  hist1D[ "NuPt" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit )->pt() );
  hist1D[ "NuEta" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit )->eta() );
  hist1D[ "NuPhi" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit )->phi() );
  hist1D[ "NuPz" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit )->pz() );
  hist1D[ "TopHadM" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayTop( TtEvent::kHitFit )->mass() );

}


void TopHitFitAnalyzer::fill2D_HitFit_Valid( std::map< std::string, TH2D* > & hist2D )
{

  hist2D[ "Prob_MT" ]->Fill( ttSemiLeptonicEvent_->hitFitProb(), ttSemiLeptonicEvent_->hitFitMT() );
  hist2D[ "Prob_SigMT" ]->Fill( ttSemiLeptonicEvent_->hitFitProb(), ttSemiLeptonicEvent_->hitFitSigMT() );
  hist2D[ "Prob_SigMTRel" ]->Fill( ttSemiLeptonicEvent_->hitFitProb(), ttSemiLeptonicEvent_->hitFitSigMT()/ttSemiLeptonicEvent_->hitFitMT() );
  hist2D[ "MT_SigMT" ]->Fill( ttSemiLeptonicEvent_->hitFitMT(), ttSemiLeptonicEvent_->hitFitSigMT() );
  hist2D[ "MT_SigMTRel" ]->Fill( ttSemiLeptonicEvent_->hitFitMT(), ttSemiLeptonicEvent_->hitFitSigMT()/ttSemiLeptonicEvent_->hitFitMT() );
  hist2D[ "Prob_Prob1" ]->Fill( ttSemiLeptonicEvent_->hitFitProb(), ttSemiLeptonicEvent_->hitFitProb( 1 ) );

}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( TopHitFitAnalyzer );
