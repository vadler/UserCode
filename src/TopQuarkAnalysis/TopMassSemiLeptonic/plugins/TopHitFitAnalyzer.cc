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
#include <vector>
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
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"


typedef std::map< std::string, TH1D* > MapTH1D;
typedef std::map< std::string, TH2D* > MapTH2D;


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
    // PAT collections
    edm::Handle< pat::MuonCollection > patMuons_;
    edm::Handle< pat::JetCollection >  patJets_;
    edm::Handle< pat::METCollection >  patMETs_;
    // TQAF MC event
    TtGenEvent * ttGenEvent_;
    // decay channel in real (MC) TTbar events
    unsigned decayChn_;
    bool     isSignal_;
    bool     isSignalTau_;
    unsigned nHitFitHyposValid_;
    unsigned nGenMatchHyposValid_;
    int      hitFitHypoFound_1_;
    int      hitFitHypoFound_2_;
    int      hitFitHypoFound_;

    /// Configuration parameters
    unsigned verbosity_;
    // TQAF semi-leptonic event input tag
    edm::InputTag ttSemiLeptonicEventTag_;
    // PAT input tag
    edm::InputTag patMuonsTag_;
    edm::InputTag patJetsTag_;
    edm::InputTag patMETsTag_;
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
    // GenMatch validity
    unsigned binsGenMatchNValid_;
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
    // Generatzor
    MapTH1D hist1D_Generator_;
    MapTH1D hist1D_Generator__Signal_;
    MapTH2D hist2D_Generator_;
    MapTH2D hist2D_Generator__Signal_;
    // HitFit
    MapTH1D hist1D_HitFit_;
    MapTH1D hist1D_HitFit__Signal_;
    MapTH1D hist1D_HitFit__Tau_;
    MapTH1D hist1D_HitFit__SignalTau_;
    MapTH1D hist1D_HitFit__Bkg_;
    MapTH1D hist1D_HitFit__BkgNoTau_;
    MapTH2D hist2D_HitFit_;
    MapTH2D hist2D_HitFit__Signal_;
    MapTH2D hist2D_HitFit__Tau_;
    MapTH2D hist2D_HitFit__SignalTau_;
    MapTH2D hist2D_HitFit__Bkg_;
    MapTH2D hist2D_HitFit__BkgNoTau_;
    MapTH1D hist1D_HitFitFound__Signal_;
    MapTH1D hist1D_HitFitGood__Signal_;
    MapTH2D hist2D_HitFitFound__Signal_;
    MapTH2D hist2D_HitFitGood__Signal_;
    // GenMatch
    MapTH1D hist1D_GenMatch__Signal_;
    MapTH2D hist2D_GenMatch__Signal_;
    // HitFit vs. Generator
    MapTH1D hist1D_Generator_HitFit_;
    MapTH1D hist1D_Generator_HitFit__Signal_;
    MapTH2D hist2D_Generator_HitFit_;
    MapTH2D hist2D_Generator_HitFit__Signal_;
    MapTH1D hist1D_Generator_HitFitFound__Signal_;
    MapTH1D hist1D_Generator_HitFitGood__Signal_;
    MapTH2D hist2D_Generator_HitFitFound__Signal_;
    MapTH2D hist2D_Generator_HitFitGood__Signal_;
    // HitFit vs. GenMatch
    MapTH1D hist1D_GenMatch_HitFitGood__Signal_;
    MapTH2D hist2D_GenMatch_HitFitGood__Signal_;

    /// Constants
    std::vector< std::string > decayChnLabels_;
    std::vector< std::string > patMatchPartonLabels_;
    std::vector< std::string > tqafMatchPartonLabels_;

    /// Private functions

    void  assignDecayChnNames( TAxis * iAxis );
    void  assignPatMatchPartonNames( TAxis * iAxis );
    void  assignTqafMatchPartonNames( TAxis * iAxis );
    float getPatMatchPartonBin( const int pdgId );
    int   correspondingAltHypo( const TtEvent::HypoClassKey key1, const unsigned hyp1, const TtEvent::HypoClassKey key2 );
    // Fill histograms
    void fill1D_Generator( MapTH1D & hist1D );
    void fill2D_Generator( MapTH2D & hist2D );
    void fill1D_GeneratorValid( MapTH1D & hist1D );
    void fill2D_GeneratorValid( MapTH2D & hist2D );
    void fill1D_HitFit( MapTH1D & hist1D, unsigned iCombi = 0 );
    void fill2D_HitFit( MapTH2D & hist2D, unsigned iCombi = 0 );
    void fill1D_HitFitValid( MapTH1D & hist1D, unsigned iCombi = 0 );
    void fill2D_HitFitValid( MapTH2D & hist2D, unsigned iCombi = 0 );
    void fill1D_GenMatch( MapTH1D & hist1D, unsigned iCombi = 0 );
    void fill2D_GenMatch( MapTH2D & hist2D, unsigned iCombi = 0 );
    void fill1D_GenMatchValid( MapTH1D & hist1D, unsigned iCombi = 0 );
    void fill2D_GenMatchValid( MapTH2D & hist2D, unsigned iCombi = 0 );
    void fill2D_Generator_HitFit( MapTH2D & hist2D, unsigned iCombi = 0 );
    void fill1D_Generator_HitFitValid( MapTH1D & hist1D, unsigned iCombi = 0 );
    void fill2D_Generator_HitFitValid( MapTH2D & hist2D, unsigned iCombi = 0 );
    void fill1D_GenMatch_HitFitValid( MapTH1D & hist2D, unsigned iCombi = 0 );
    void fill2D_GenMatch_HitFitValid( MapTH2D & hist2D, unsigned iCombi = 0 );

};


#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "AnalysisDataFormats/TopObjects/interface/TtSemiLepEvtPartons.h"


// Default constructor
TopHitFitAnalyzer::TopHitFitAnalyzer( const edm::ParameterSet & iConfig )
: ttSemiLeptonicEvent_()
, patMuons_()
, patJets_()
, patMETs_()
, ttGenEvent_()
, decayChn_( WDecay::kNone )
, isSignal_( false )
, isSignalTau_( false )
, nHitFitHyposValid_( 0 )
, nGenMatchHyposValid_( 0 )
, hitFitHypoFound_1_( -1 )
, hitFitHypoFound_2_( -1 )
, hitFitHypoFound_( -1 )
  // get configuration parameters
, verbosity_( iConfig.getParameter< unsigned >( "verbosity" ) )
, ttSemiLeptonicEventTag_( iConfig.getParameter< edm::InputTag >( "ttSemiLeptonicEvent" ) )
, patMuonsTag_( iConfig.getParameter< edm::InputTag >( "patMuons" ) )
, patJetsTag_( iConfig.getParameter< edm::InputTag >( "patJets" ) )
, patMETsTag_( iConfig.getParameter< edm::InputTag >( "patMETs" ) )
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
, binsGenMatchNValid_( iConfig.getParameter< unsigned >( "binsGenMatchNValid" ) )
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
, hist1D_Generator_()
, hist1D_Generator__Signal_()
, hist2D_Generator_()
, hist2D_Generator__Signal_()
, hist1D_HitFit_()
, hist1D_HitFit__Signal_()
, hist1D_HitFit__Tau_()
, hist1D_HitFit__SignalTau_()
, hist1D_HitFit__Bkg_()
, hist1D_HitFit__BkgNoTau_()
, hist2D_HitFit_()
, hist2D_HitFit__Signal_()
, hist2D_HitFit__Tau_()
, hist2D_HitFit__SignalTau_()
, hist2D_HitFit__Bkg_()
, hist2D_HitFit__BkgNoTau_()
, hist1D_HitFitFound__Signal_()
, hist1D_HitFitGood__Signal_()
, hist2D_HitFitFound__Signal_()
, hist2D_HitFitGood__Signal_()
, hist1D_GenMatch__Signal_()
, hist2D_GenMatch__Signal_()
, hist1D_Generator_HitFit_()
, hist1D_Generator_HitFit__Signal_()
, hist2D_Generator_HitFit_()
, hist2D_Generator_HitFit__Signal_()
, hist1D_Generator_HitFitFound__Signal_()
, hist1D_Generator_HitFitGood__Signal_()
, hist2D_Generator_HitFitFound__Signal_()
, hist2D_Generator_HitFitGood__Signal_()
, hist1D_GenMatch_HitFitGood__Signal_()
, hist2D_GenMatch_HitFitGood__Signal_()
{

  // Axis labels

  // Decay chanels
  // FIXME: This is still muon-specific
  decayChnLabels_.clear();
  decayChnLabels_.push_back( "undefined" ); // 1st histo bin is underflow
  decayChnLabels_.push_back( "full had." );
  decayChnLabels_.push_back( "semi e" );
  decayChnLabels_.push_back( "#it{semi #mu}" );
  decayChnLabels_.push_back( "semi #tau, to other" );
  decayChnLabels_.push_back( "semi #tau, to #mu" );
  decayChnLabels_.push_back( "full lep. 0 #mu or #tau to #mu" );
  decayChnLabels_.push_back( "full lep. 1 #mu or #tau to #mu" );
  decayChnLabels_.push_back( "full lep. 2 #mu or #tau to #mu" );

  // PAT MC jet matched partons
  patMatchPartonLabels_.clear();
  patMatchPartonLabels_.push_back( "no match" ); // 1st histo bin is underflow
  patMatchPartonLabels_.push_back( "uds" );
  patMatchPartonLabels_.push_back( "c" );
  patMatchPartonLabels_.push_back( "b" );

  // TQAF MC jet matched partons
  tqafMatchPartonLabels_.clear();
  tqafMatchPartonLabels_.push_back( "no match" ); // 1st histo bin is underflow
  tqafMatchPartonLabels_.push_back( "q" );
  tqafMatchPartonLabels_.push_back( "#bar{q}" );
  tqafMatchPartonLabels_.push_back( "b_{h}" );
  tqafMatchPartonLabels_.push_back( "b_{l}" );

}


// Destructor
TopHitFitAnalyzer::~TopHitFitAnalyzer()
{
}


// Begin job
void TopHitFitAnalyzer::beginJob()
{

  const std::string strHitFit( "HitFit_" );
  const std::string strHitFitValid( "HitFitValid_" );

  edm::Service< TFileService > fileService;

  // Generator histograms

  TFileDirectory dir_Generator( fileService->mkdir( "Generator", "Generator histograms" ) );

  TFileDirectory dir_Generator__Signal( fileService->mkdir( "Generator__Signal", "Generator histograms, signal events" ) );

  // 1-dim
  hist1D_Generator_[ "DecayChn" ] = dir_Generator.make< TH1D >( "Generator_DecayChn", "True decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1. );
  hist1D_Generator_[ "DecayChn" ]->SetXTitle( "" );
  assignDecayChnNames( hist1D_Generator_[ "DecayChn" ]->GetXaxis() );
  for ( MapTH1D::const_iterator iHist = hist1D_Generator_.begin(); iHist != hist1D_Generator_.end(); ++iHist ) {
    iHist->second->SetYTitle( "events" );
  }
  // 1-dim
  for ( MapTH1D::const_iterator iHist = hist1D_Generator_.begin(); iHist != hist1D_Generator_.end(); ++iHist ) {
    std::string name__Signal( iHist->second->GetName() );
    hist1D_Generator__Signal_[ iHist->first ] = dir_Generator__Signal.make< TH1D >( *( ( TH1D* )( iHist->second->Clone( name__Signal.append( "__Signal" ).c_str() ) ) ) );
  }
  hist1D_Generator__Signal_[ "TopLepM" ] = dir_Generator__Signal.make< TH1D >( "Gen_TopLepM__Signal", "Generated muonic top mass", binsTopM_, minTopM_, maxTopM_ );
  hist1D_Generator__Signal_[ "TopLepM" ]->SetXTitle( "m_{t_{l}}^{gen.} (GeV)" );
  hist1D_Generator__Signal_[ "LepM" ] = dir_Generator__Signal.make< TH1D >( "Gen_LepM__Signal", "Generated lepton mass", binsGenLepM_, 0., maxGenLepM_ );
  hist1D_Generator__Signal_[ "LepM" ]->SetXTitle( "m_{l}^{gen.} (GeV)" );
  hist1D_Generator__Signal_[ "LepPt" ] = dir_Generator__Signal.make< TH1D >( "Gen_LepPt__Signal", "Generated lepton transverse momentum", binsLepPt_, 0., maxLepPt_ );
  hist1D_Generator__Signal_[ "LepPt" ]->SetXTitle( "p_{t, l}^{gen.} (GeV)" );
  hist1D_Generator__Signal_[ "LepEta" ] = dir_Generator__Signal.make< TH1D >( "Gen_LepEta__Signal", "Generated lepton pseudo-rapidity", binsLepEta_, -maxLepEta_, maxLepEta_ );
  hist1D_Generator__Signal_[ "LepEta" ]->SetXTitle( "#eta_{l}^{gen.}" );
  hist1D_Generator__Signal_[ "LepPhi" ] = dir_Generator__Signal.make< TH1D >( "Gen_LepPhi__Signal", "Generated lepton azimutal angle", binsLepPhi_, -TMath::Pi(), TMath::Pi() );
  hist1D_Generator__Signal_[ "LepPhi" ]->SetXTitle( "#phi_{l}^{gen.}" );
  hist1D_Generator__Signal_[ "LepPz" ] = dir_Generator__Signal.make< TH1D >( "Gen_LepPz__Signal", "Generated lepton longitudinal momentum", binsLepPz_, -maxLepPz_, maxLepPz_ );
  hist1D_Generator__Signal_[ "LepPz" ]->SetXTitle( "p_{z, l}^{gen.} (GeV)" );
  hist1D_Generator__Signal_[ "NuM" ] = dir_Generator__Signal.make< TH1D >( "Gen_NuM__Signal", "Generated neutrino mass", binsNuM_, -maxNuM_, maxNuM_ );
  hist1D_Generator__Signal_[ "NuM" ]->SetXTitle( "m_{#nu}^{gen.} (GeV)" );
  hist1D_Generator__Signal_[ "NuPt" ] = dir_Generator__Signal.make< TH1D >( "Gen_NuPt__Signal", "Generated neutrino transverse momentum", binsNuPt_, 0., maxNuPt_ );
  hist1D_Generator__Signal_[ "NuPt" ]->SetXTitle( "p_{t, #nu}^{gen.} (GeV)" );
  hist1D_Generator__Signal_[ "NuEta" ] = dir_Generator__Signal.make< TH1D >( "Gen_NuEta__Signal", "Generated neutrino pseudo-rapidity", binsNuEta_, -maxNuEta_, maxNuEta_ );
  hist1D_Generator__Signal_[ "NuEta" ]->SetXTitle( "#eta_{#nu}^{gen.}" );
  hist1D_Generator__Signal_[ "NuPhi" ] = dir_Generator__Signal.make< TH1D >( "Gen_NuPhi__Signal", "Generated neutrino azimutal angle", binsNuPhi_, -TMath::Pi(), TMath::Pi() );
  hist1D_Generator__Signal_[ "NuPhi" ]->SetXTitle( "#phi_{#nu}^{gen.}" );
  hist1D_Generator__Signal_[ "NuPz" ] = dir_Generator__Signal.make< TH1D >( "Gen_NuPz__Signal", "Generated neutrino longitudinal momentum", binsNuPz_, -maxNuPz_, maxNuPz_ );
  hist1D_Generator__Signal_[ "NuPz" ]->SetXTitle( "p_{z, #nu}^{gen.} (GeV)" );
  hist1D_Generator__Signal_[ "TopHadM" ] = dir_Generator__Signal.make< TH1D >( "Gen_TopHadM__Signal", "Generated hadronic top mass", binsTopM_, minTopM_, maxTopM_ );
  hist1D_Generator__Signal_[ "TopHadM" ]->SetXTitle( "m_{t_{h}}^{gen.} (GeV)" );
  for ( MapTH1D::const_iterator iHist = hist1D_Generator__Signal_.begin(); iHist != hist1D_Generator__Signal_.end(); ++iHist ) {
    iHist->second->SetYTitle( "events" );
  }

  // HitFit histograms

  TFileDirectory dir_HitFit( fileService->mkdir( "HitFit", "HitFit histograms" ) );
  TFileDirectory dir_HitFitValid( dir_HitFit.mkdir( "HitFitValid", "HitFit histograms for valid hypotheses" ) );

  TFileDirectory dir_HitFit__Signal( fileService->mkdir( "HitFit__Signal", "HitFit histograms, signal events" ) );
  TFileDirectory dir_HitFitValid__Signal( dir_HitFit__Signal.mkdir( "HitFitValid__Signal", "HitFit histograms, signal events for valid hypotheses" ) );

  TFileDirectory dir_HitFit__Tau( fileService->mkdir( "HitFit__Tau", "HitFit histograms, muonic tau + jets events (signal-like)" ) );
  TFileDirectory dir_HitFitValid__Tau( dir_HitFit__Tau.mkdir( "HitFit__TauValid", "HitFit histograms, muonic tau + jets events (signal-like) for valid hypotheses" ) );

  TFileDirectory dir_HitFit__SignalTau( fileService->mkdir( "HitFit__SignalTau", "HitFit histograms, signal + signal-like events" ) );
  TFileDirectory dir_HitFitValid__SignalTau( dir_HitFit__SignalTau.mkdir( "HitFitValid__SignalTau", "HitFit histograms, signal + signal-like events for valid hypotheses" ) );

  TFileDirectory dir_HitFit__Bkg( fileService->mkdir( "HitFit__Bkg", "HitFit histograms, background events" ) );
  TFileDirectory dir_HitFitValid__Bkg( dir_HitFit__Bkg.mkdir( "HitFitValid__Bkg", "HitFit histograms, background events for valid hypotheses" ) );

  TFileDirectory dir_HitFit__BkgNoTau( fileService->mkdir( "HitFit__BkgNoTau", "HitFit histograms, background w/o signal-like events" ) );
  TFileDirectory dir_HitFitValid__BkgNoTau( dir_HitFit__BkgNoTau.mkdir( "HitFitValid__BkgNoTau", "HitFit histograms, background w/o signal-like events for valid hypotheses" ) );

  TFileDirectory dir_HitFitFound__Signal( fileService->mkdir( "HitFitFound__Signal", "HitFit histograms, found HitFit solution in GenMatch" ) );
  TFileDirectory dir_HitFitFoundValid__Signal( dir_HitFitFound__Signal.mkdir( "HitFitValidFound__Signal", "HitFit histograms, found HitFit solution in GenMatch for valid hypotheses" ) );

  TFileDirectory dir_HitFitGood__Signal( fileService->mkdir( "HitFitGood__Signal", "HitFit histograms, good HitFit solution in GenMatch" ) );
  TFileDirectory dir_HitFitGoodValid__Signal( dir_HitFitGood__Signal.mkdir( "HitFitValidGood__Signal", "HitFit histograms, good HitFit solution in GenMatch for valid hypotheses" ) );

  // 1-dim
  hist1D_HitFit_[ "nValid" ] = dir_HitFit.make< TH1D >( "HitFit_nValid", "No. of valid HitFit hypothesis", binsHitFitNValid_ + 1, -0.5, binsHitFitNValid_ + 0.5 );
  hist1D_HitFit_[ "nValid" ]->SetXTitle( "valid HitFit hypos" );
  // 1-dim
  hist1D_HitFit_[ "nRealNuSol" ] = dir_HitFitValid.make< TH1D >( "HitFitValid_nRealNuSol", "No. of real HitFit solutions", 4, -1.5, 2.5 );
  hist1D_HitFit_[ "nRealNuSol" ]->SetXTitle( "solutions_{real #nu}^{HitFit}" );
  hist1D_HitFit_[ "Prob" ] = dir_HitFitValid.make< TH1D >( "HitFitValid_Prob", "HitFit probabilty", binsHitFitProb_, 0., 1. );
  hist1D_HitFit_[ "Prob" ]->SetXTitle( "P^{HitFit}" );
  hist1D_HitFit_[ "ProbLow" ] = dir_HitFitValid.make< TH1D >( "HitFitValid_ProbLow", "HitFit probabilty", binsHitFitProbLow_, 0., maxHitFitProbLow_ );
  hist1D_HitFit_[ "ProbLow" ]->SetXTitle( "P^{HitFit}" );
  hist1D_HitFit_[ "ProbLog" ] = dir_HitFitValid.make< TH1D >( "HitFitValid_ProbLog", "HitFit probabilty", binsHitFitProbLog_, minHitFitProbLog_, 0. );
  hist1D_HitFit_[ "ProbLog" ]->SetXTitle( "log_{10} P" );
  hist1D_HitFit_[ "Chi2" ] = dir_HitFitValid.make< TH1D >( "HitFitValid_Chi2", "HitFit #chi^{2}", binsHitFitChi2_, 0., maxHitFitChi2_ );
  hist1D_HitFit_[ "Chi2" ]->SetXTitle( "(#chi^{2})^{HitFit}" );
  hist1D_HitFit_[ "MT" ] = dir_HitFitValid.make< TH1D >( "HitFitValid_MT", "HitFit top mass", binsHitFitMT_, minHitFitMT_, maxHitFitMT_ );
  hist1D_HitFit_[ "MT" ]->SetXTitle( "m_{top}^{HitFit} (GeV)" );
  hist1D_HitFit_[ "SigMT" ] = dir_HitFitValid.make< TH1D >( "HitFitValid_SigMT", "HitFit top mass uncertainty", binsHitFitSigMT_, 0., maxHitFitSigMT_ );
  hist1D_HitFit_[ "SigMT" ]->SetXTitle( "#sigma_{m_{top}}^{HitFit} (GeV)" );
  hist1D_HitFit_[ "SigMTRel" ] = dir_HitFitValid.make< TH1D >( "HitFitValid_SigMTRel", "Relative HitFit top mass uncertainty", binsHitFitSigMTRel_, 0., maxHitFitSigMTRel_ );
  hist1D_HitFit_[ "SigMTRel" ]->SetXTitle( "#frac{#sigma_{m_{t}}^{HitFit}}{m_{t}^{HitFit}}" );
  hist1D_HitFit_[ "TopLepM" ] = dir_HitFitValid.make< TH1D >( "HitFitValid_TopLepM", "HitFit leptonic top mass", binsHitFitMT_, minHitFitMT_, maxHitFitMT_ );
  hist1D_HitFit_[ "TopLepM" ]->SetXTitle( "m_{t, l}^{HitFit} (GeV)" );
  hist1D_HitFit_[ "LepM" ] = dir_HitFitValid.make< TH1D >( "HitFitValid_LepM", "HitFit lepton mass", binsLepM_, -maxLepM_, maxLepM_ );
  hist1D_HitFit_[ "LepM" ]->SetXTitle( "m_{l}^{HitFit} (GeV)" );
  hist1D_HitFit_[ "LepPt" ] = dir_HitFitValid.make< TH1D >( "HitFitValid_LepPt", "HitFit lepton transverse momentum", binsLepPt_, 0., maxLepPt_ );
  hist1D_HitFit_[ "LepPt" ]->SetXTitle( "p_{t, l}^{HitFit} (GeV)" );
  hist1D_HitFit_[ "LepEta" ] = dir_HitFitValid.make< TH1D >( "HitFitValid_LepEta", "HitFit lepton pseudo-rapidity", binsLepEta_, -maxLepEta_, maxLepEta_ );
  hist1D_HitFit_[ "LepEta" ]->SetXTitle( "#eta_{l}^{HitFit}" );
  hist1D_HitFit_[ "LepPhi" ] = dir_HitFitValid.make< TH1D >( "HitFitValid_LepPhi", "HitFit lepton azimutal angle", binsLepPhi_, -TMath::Pi(), TMath::Pi() );
  hist1D_HitFit_[ "LepPhi" ]->SetXTitle( "#phi_{l}^{HitFit}" );
  hist1D_HitFit_[ "LepPz" ] = dir_HitFitValid.make< TH1D >( "HitFitValid_LepPz", "HitFit lepton longitudinal momentum", binsLepPz_, -maxLepPz_, maxLepPz_ );
  hist1D_HitFit_[ "LepPz" ]->SetXTitle( "p_{z, l}^{HitFit} (GeV)" );
  hist1D_HitFit_[ "NuM" ] = dir_HitFitValid.make< TH1D >( "HitFitValid_NuM", "HitFit neutrino mass", binsNuM_, -maxNuM_, maxNuM_ );
  hist1D_HitFit_[ "NuM" ]->SetXTitle( "m_{#nu}^{HitFit} (GeV)" );
  hist1D_HitFit_[ "NuPt" ] = dir_HitFitValid.make< TH1D >( "HitFitValid_NuPt", "HitFit neutrino transverse momentum", binsNuPt_, 0., maxNuPt_ );
  hist1D_HitFit_[ "NuPt" ]->SetXTitle( "p_{t, #nu}^{HitFit} (GeV)" );
  hist1D_HitFit_[ "NuEta" ] = dir_HitFitValid.make< TH1D >( "HitFitValid_NuEta", "HitFit neutrino pseudo-rapidity", binsNuEta_, -maxNuEta_, maxNuEta_ );
  hist1D_HitFit_[ "NuEta" ]->SetXTitle( "#eta_{#nu}^{HitFit}" );
  hist1D_HitFit_[ "NuPhi" ] = dir_HitFitValid.make< TH1D >( "HitFitValid_NuPhi", "HitFit neutrino azimutal angle", binsNuPhi_, -TMath::Pi(), TMath::Pi() );
  hist1D_HitFit_[ "NuPhi" ]->SetXTitle( "#phi_{#nu}^{HitFit}" );
  hist1D_HitFit_[ "NuPz" ] = dir_HitFitValid.make< TH1D >( "HitFitValid_NuPz", "HitFit neutrino longitudinal momentum", binsNuPz_, -maxNuPz_, maxNuPz_ );
  hist1D_HitFit_[ "NuPz" ]->SetXTitle( "p_{z, #nu}^{HitFit} (GeV)" );
  hist1D_HitFit_[ "TopHadM" ] = dir_HitFitValid.make< TH1D >( "HitFitValid_TopHadM", "HitFit hadronic top mass", binsHitFitMT_, minHitFitMT_, maxHitFitMT_ );
  hist1D_HitFit_[ "TopHadM" ]->SetXTitle( "m_{t, h}^{HitFit} (GeV)" );
  for ( MapTH1D::const_iterator iHist = hist1D_HitFit_.begin(); iHist != hist1D_HitFit_.end(); ++iHist ) {
    iHist->second->SetYTitle( "events" );
    std::string name__Signal( iHist->second->GetName() );
    if ( std::string( iHist->second->GetName() ).substr( 0, strHitFit.size() ) == strHitFit )
      hist1D_HitFit__Signal_[ iHist->first ] = dir_HitFit__Signal.make< TH1D >( *( ( TH1D* )( iHist->second->Clone( name__Signal.append( "__Signal" ).c_str() ) ) ) );
    else if ( std::string( iHist->second->GetName() ).substr( 0, strHitFitValid.size() ) == strHitFitValid )
      hist1D_HitFit__Signal_[ iHist->first ] = dir_HitFitValid__Signal.make< TH1D >( *( ( TH1D* )( iHist->second->Clone( name__Signal.append( "__Signal" ).c_str() ) ) ) );
    std::string name__Tau( iHist->second->GetName() );
    if ( std::string( iHist->second->GetName() ).substr( 0, strHitFit.size() ) == strHitFit )
      hist1D_HitFit__Tau_[ iHist->first ] = dir_HitFit__Tau.make< TH1D >( *( ( TH1D* )( iHist->second->Clone( name__Tau.append( "__Tau" ).c_str() ) ) ) );
    else if ( std::string( iHist->second->GetName() ).substr( 0, strHitFitValid.size() ) == strHitFitValid )
      hist1D_HitFit__Tau_[ iHist->first ] = dir_HitFitValid__Tau.make< TH1D >( *( ( TH1D* )( iHist->second->Clone( name__Tau.append( "__Tau" ).c_str() ) ) ) );
    std::string name__SignalTau( iHist->second->GetName() );
    if ( std::string( iHist->second->GetName() ).substr( 0, strHitFit.size() ) == strHitFit )
      hist1D_HitFit__SignalTau_[ iHist->first ] = dir_HitFit__SignalTau.make< TH1D >( *( ( TH1D* )( iHist->second->Clone( name__SignalTau.append( "__SignalTau" ).c_str() ) ) ) );
    else if ( std::string( iHist->second->GetName() ).substr( 0, strHitFitValid.size() ) == strHitFitValid )
      hist1D_HitFit__SignalTau_[ iHist->first ] = dir_HitFitValid__SignalTau.make< TH1D >( *( ( TH1D* )( iHist->second->Clone( name__SignalTau.append( "__SignalTau" ).c_str() ) ) ) );
    std::string name__Bkg( iHist->second->GetName() );
    if ( std::string( iHist->second->GetName() ).substr( 0, strHitFit.size() ) == strHitFit )
      hist1D_HitFit__Bkg_[ iHist->first ] = dir_HitFit__Bkg.make< TH1D >( *( ( TH1D* )( iHist->second->Clone( name__Bkg.append( "__Bkg" ).c_str() ) ) ) );
    else if ( std::string( iHist->second->GetName() ).substr( 0, strHitFitValid.size() ) == strHitFitValid )
      hist1D_HitFit__Bkg_[ iHist->first ] = dir_HitFitValid__Bkg.make< TH1D >( *( ( TH1D* )( iHist->second->Clone( name__Bkg.append( "__Bkg" ).c_str() ) ) ) );
    std::string name__BkgNoTau( iHist->second->GetName() );
    if ( std::string( iHist->second->GetName() ).substr( 0, strHitFit.size() ) == strHitFit )
      hist1D_HitFit__BkgNoTau_[ iHist->first ] = dir_HitFit__BkgNoTau.make< TH1D >( *( ( TH1D* )( iHist->second->Clone( name__BkgNoTau.append( "__BkgNoTau" ).c_str() ) ) ) );
    else if ( std::string( iHist->second->GetName() ).substr( 0, strHitFitValid.size() ) == strHitFitValid )
      hist1D_HitFit__BkgNoTau_[ iHist->first ] = dir_HitFitValid__BkgNoTau.make< TH1D >( *( ( TH1D* )( iHist->second->Clone( name__BkgNoTau.append( "__BkgNoTau" ).c_str() ) ) ) );
    std::string nameFound__Signal( iHist->second->GetName() );
    if ( std::string( iHist->second->GetName() ).substr( 0, strHitFit.size() ) == strHitFit )
      hist1D_HitFitFound__Signal_[ iHist->first ] = dir_HitFitFound__Signal.make< TH1D >( *( ( TH1D* )( iHist->second->Clone( nameFound__Signal.replace( 6, 0, "Found" ).append( "__Signal" ).c_str() ) ) ) );
    else if ( std::string( iHist->second->GetName() ).substr( 0, strHitFitValid.size() ) == strHitFitValid )
      hist1D_HitFitFound__Signal_[ iHist->first ] = dir_HitFitFoundValid__Signal.make< TH1D >( *( ( TH1D* )( iHist->second->Clone( nameFound__Signal.replace( 6, 0, "Found" ).append( "__Signal" ).c_str() ) ) ) );
    std::string nameGood__Signal( iHist->second->GetName() );
    if ( std::string( iHist->second->GetName() ).substr( 0, strHitFit.size() ) == strHitFit )
      hist1D_HitFitGood__Signal_[ iHist->first ] = dir_HitFitGood__Signal.make< TH1D >( *( ( TH1D* )( iHist->second->Clone( nameGood__Signal.replace( 6, 0, "Good" ).append( "__Signal" ).c_str() ) ) ) );
    else if ( std::string( iHist->second->GetName() ).substr( 0, strHitFitValid.size() ) == strHitFitValid )
      hist1D_HitFitGood__Signal_[ iHist->first ] = dir_HitFitGoodValid__Signal.make< TH1D >( *( ( TH1D* )( iHist->second->Clone( nameGood__Signal.replace( 6, 0, "Good" ).append( "__Signal" ).c_str() ) ) ) );
  }
  // 2-dim
  hist2D_HitFit_[ "Prob_MT" ] = dir_HitFitValid.make< TH2D >( "HitFitValid_Prob_MT", "HitFit top mass vs. HitFit probabilty", binsHitFitProb_, 0., 1., binsHitFitMT_, minHitFitMT_, maxHitFitMT_ );
  hist2D_HitFit_[ "Prob_MT" ]->SetXTitle( "P^{HitFit}" );
  hist2D_HitFit_[ "Prob_MT" ]->SetYTitle( "m_{top}^{HitFit} (GeV)" );
  hist2D_HitFit_[ "Prob_SigMT" ] = dir_HitFitValid.make< TH2D >( "HitFitValid_Prob_SigMT", "HitFit top mass uncertainty vs. HitFit probabilty", binsHitFitProb_, 0., 1., binsHitFitSigMT_, 0., maxHitFitSigMT_ );
  hist2D_HitFit_[ "Prob_SigMT" ]->SetXTitle( "P^{HitFit}" );
  hist2D_HitFit_[ "Prob_SigMT" ]->SetYTitle( "#sigma_{m_{t}}^{HitFit} (GeV)" );
  hist2D_HitFit_[ "Prob_SigMTRel" ] = dir_HitFitValid.make< TH2D >( "HitFitValid_Prob_SigMTRel", "Relative HitFit top mass uncertainty vs. HitFit probabilty", binsHitFitProb_, 0., 1., binsHitFitSigMTRel_, 0., maxHitFitSigMTRel_ );
  hist2D_HitFit_[ "Prob_SigMTRel" ]->SetXTitle( "P^{HitFit}" );
  hist2D_HitFit_[ "Prob_SigMTRel" ]->SetYTitle( "#frac{#sigma_{m_{t}}^{HitFit}}{m_{t}^{HitFit}}" );
  hist2D_HitFit_[ "MT_SigMT" ] = dir_HitFitValid.make< TH2D >( "HitFitValid_MT_SigMT", "HitFit top mass uncertainty vs. HitFit top mass", binsHitFitMT_, minHitFitMT_, maxHitFitMT_, binsHitFitSigMT_, 0., maxHitFitSigMT_ );
  hist2D_HitFit_[ "MT_SigMT" ]->SetXTitle( "m_{t}^{HitFit} (GeV)" );
  hist2D_HitFit_[ "MT_SigMT" ]->SetYTitle( "#sigma_{m_{t}}^{HitFit} (GeV)" );
  hist2D_HitFit_[ "MT_SigMTRel" ] = dir_HitFitValid.make< TH2D >( "HitFitValid_MT_SigMTRel", "Relative HitFit top mass uncertainty vs. HitFit top mass", binsHitFitMT_, minHitFitMT_, maxHitFitMT_, binsHitFitSigMTRel_, 0., maxHitFitSigMTRel_ );
  hist2D_HitFit_[ "MT_SigMTRel" ]->SetXTitle( "m_{t}^{HitFit} (GeV)" );
  hist2D_HitFit_[ "MT_SigMTRel" ]->SetYTitle( "#frac{#sigma_{m_{t}}^{HitFit}}{m_{t}^{HitFit}}" );
  for ( MapTH2D::const_iterator iHist = hist2D_HitFit_.begin(); iHist != hist2D_HitFit_.end(); ++iHist ) {
    iHist->second->SetZTitle( "events" );
    std::string name__Signal( iHist->second->GetName() );
    if ( std::string( iHist->second->GetName() ).substr( 0,  7 ) == "HitFit_")
      hist2D_HitFit__Signal_[ iHist->first ] = dir_HitFit__Signal.make< TH2D >( *( ( TH2D* )( iHist->second->Clone( name__Signal.append( "__Signal" ).c_str() ) ) ) );
    else if ( std::string( iHist->second->GetName() ).substr( 0, strHitFitValid.size() ) == strHitFitValid )
      hist2D_HitFit__Signal_[ iHist->first ] = dir_HitFitValid__Signal.make< TH2D >( *( ( TH2D* )( iHist->second->Clone( name__Signal.append( "__Signal" ).c_str() ) ) ) );
    std::string name__Tau( iHist->second->GetName() );
    if ( std::string( iHist->second->GetName() ).substr( 0, strHitFit.size() ) == strHitFit )
      hist2D_HitFit__Tau_[ iHist->first ] = dir_HitFit__Tau.make< TH2D >( *( ( TH2D* )( iHist->second->Clone( name__Tau.append( "__Tau" ).c_str() ) ) ) );
    else if ( std::string( iHist->second->GetName() ).substr( 0, strHitFitValid.size() ) == strHitFitValid )
      hist2D_HitFit__Tau_[ iHist->first ] = dir_HitFitValid__Tau.make< TH2D >( *( ( TH2D* )( iHist->second->Clone( name__Tau.append( "__Tau" ).c_str() ) ) ) );
    std::string name__SignalTau( iHist->second->GetName() );
    if ( std::string( iHist->second->GetName() ).substr( 0, strHitFit.size() ) == strHitFit )
      hist2D_HitFit__SignalTau_[ iHist->first ] = dir_HitFit__SignalTau.make< TH2D >( *( ( TH2D* )( iHist->second->Clone( name__SignalTau.append( "__SignalTau" ).c_str() ) ) ) );
    else if ( std::string( iHist->second->GetName() ).substr( 0, strHitFitValid.size() ) == strHitFitValid )
      hist2D_HitFit__SignalTau_[ iHist->first ] = dir_HitFitValid__SignalTau.make< TH2D >( *( ( TH2D* )( iHist->second->Clone( name__SignalTau.append( "__SignalTau" ).c_str() ) ) ) );
    std::string name__Bkg( iHist->second->GetName() );
    if ( std::string( iHist->second->GetName() ).substr( 0, strHitFit.size() ) == strHitFit )
      hist2D_HitFit__Bkg_[ iHist->first ] = dir_HitFit__Bkg.make< TH2D >( *( ( TH2D* )( iHist->second->Clone( name__Bkg.append( "__Bkg" ).c_str() ) ) ) );
    else if ( std::string( iHist->second->GetName() ).substr( 0, strHitFitValid.size() ) == strHitFitValid )
      hist2D_HitFit__Bkg_[ iHist->first ] = dir_HitFitValid__Bkg.make< TH2D >( *( ( TH2D* )( iHist->second->Clone( name__Bkg.append( "__Bkg" ).c_str() ) ) ) );
    std::string name__BkgNoTau( iHist->second->GetName() );
    if ( std::string( iHist->second->GetName() ).substr( 0, strHitFit.size() ) == strHitFit )
      hist2D_HitFit__BkgNoTau_[ iHist->first ] = dir_HitFit__BkgNoTau.make< TH2D >( *( ( TH2D* )( iHist->second->Clone( name__BkgNoTau.append( "__BkgNoTau" ).c_str() ) ) ) );
    else if ( std::string( iHist->second->GetName() ).substr( 0, strHitFitValid.size() ) == strHitFitValid )
      hist2D_HitFit__BkgNoTau_[ iHist->first ] = dir_HitFitValid__BkgNoTau.make< TH2D >( *( ( TH2D* )( iHist->second->Clone( name__BkgNoTau.append( "__BkgNoTau" ).c_str() ) ) ) );
    std::string nameFound__Signal( iHist->second->GetName() );
    if ( std::string( iHist->second->GetName() ).substr( 0, strHitFit.size() ) == strHitFit )
      hist2D_HitFitFound__Signal_[ iHist->first ] = dir_HitFitFound__Signal.make< TH2D >( *( ( TH2D* )( iHist->second->Clone( nameFound__Signal.replace( 6, 0, "Found" ).append( "__Signal" ).c_str() ) ) ) );
    else if ( std::string( iHist->second->GetName() ).substr( 0, strHitFitValid.size() ) == strHitFitValid )
      hist2D_HitFitFound__Signal_[ iHist->first ] = dir_HitFitFoundValid__Signal.make< TH2D >( *( ( TH2D* )( iHist->second->Clone( nameFound__Signal.replace( 6, 0, "Found" ).append( "__Signal" ).c_str() ) ) ) );
    std::string nameGood__Signal( iHist->second->GetName() );
    if ( std::string( iHist->second->GetName() ).substr( 0, strHitFit.size() ) == strHitFit )
      hist2D_HitFitGood__Signal_[ iHist->first ] = dir_HitFitGood__Signal.make< TH2D >( *( ( TH2D* )( iHist->second->Clone( nameGood__Signal.replace( 6, 0, "Good" ).append( "__Signal" ).c_str() ) ) ) );
    else if ( std::string( iHist->second->GetName() ).substr( 0, strHitFitValid.size() ) == strHitFitValid )
      hist2D_HitFitGood__Signal_[ iHist->first ] = dir_HitFitGoodValid__Signal.make< TH2D >( *( ( TH2D* )( iHist->second->Clone( nameGood__Signal.replace( 6, 0, "Good" ).append( "__Signal" ).c_str() ) ) ) );
  }

  // GenMatch histograms

  TFileDirectory dir_GenMatch__Signal( fileService->mkdir( "GenMatch__Signal", "GenMatch histograms, signal events" ) );
  TFileDirectory dir_GenMatchValid__Signal( dir_GenMatch__Signal.mkdir( "GenMatchValid__Signal", "GenMatch histograms, signal events for valid hypotheses" ) );

  TFileDirectory dir_GenMatch__Bkg( fileService->mkdir( "GenMatch__Bkg", "GenMatch histograms, background events" ) );

  // 1-dim
  hist1D_GenMatch__Signal_[ "nValid" ] = dir_GenMatch__Signal.make< TH1D >( "GenMatch_nValid__Signal", "GenMatch valid hypotheses", binsGenMatchNValid_ + 1, -0.5, binsGenMatchNValid_ + 0.5 );
  hist1D_GenMatch__Signal_[ "nValid" ]->SetXTitle( "valid GenMatch hypotheses" );
  // 1-dim
  hist1D_GenMatch__Signal_[ "SumPt" ] = dir_GenMatchValid__Signal.make< TH1D >( "GenMatchValid_SumPt__Signal", "GenMatch sum of transverse momenta", binsGenMatchSumPt_, 0., maxGenMatchSumPt_ );
  hist1D_GenMatch__Signal_[ "SumPt" ]->SetXTitle( "#sum p_{t}^{GenMatch}" );
  hist1D_GenMatch__Signal_[ "SumDR" ] = dir_GenMatchValid__Signal.make< TH1D >( "GenMatchValid_SumDR__Signal", "GenMatch sum of spacial deviations", binsGenMatchSumDR_, 0., maxGenMatchSumDR_ );
  hist1D_GenMatch__Signal_[ "SumDR" ]->SetXTitle( "#sum #Delta R^{GenMatch}" );
  hist1D_GenMatch__Signal_[ "TopLepM" ] = dir_GenMatchValid__Signal.make< TH1D >( "GenMatchValid_TopLepM__Signal", "GenMatch muonic top mass", binsTopM_, minTopM_, maxTopM_ );
  hist1D_GenMatch__Signal_[ "TopLepM" ]->SetXTitle( "m_{t_{l}}^{gen.} (GeV)" );
  hist1D_GenMatch__Signal_[ "LepM" ] = dir_GenMatchValid__Signal.make< TH1D >( "GenMatchValid_LepM__Signal", "GenMatch lepton mass", binsGenLepM_, 0., maxGenLepM_ );
  hist1D_GenMatch__Signal_[ "LepM" ]->SetXTitle( "m_{l}^{gen.} (GeV)" );
  hist1D_GenMatch__Signal_[ "LepPt" ] = dir_GenMatchValid__Signal.make< TH1D >( "GenMatchValid_LepPt__Signal", "GenMatch lepton transverse momentum", binsLepPt_, 0., maxLepPt_ );
  hist1D_GenMatch__Signal_[ "LepPt" ]->SetXTitle( "p_{t, l}^{gen.} (GeV)" );
  hist1D_GenMatch__Signal_[ "LepEta" ] = dir_GenMatchValid__Signal.make< TH1D >( "GenMatchValid_LepEta__Signal", "GenMatch lepton pseudo-rapidity", binsLepEta_, -maxLepEta_, maxLepEta_ );
  hist1D_GenMatch__Signal_[ "LepEta" ]->SetXTitle( "#eta_{l}^{gen.}" );
  hist1D_GenMatch__Signal_[ "LepPhi" ] = dir_GenMatchValid__Signal.make< TH1D >( "GenMatchValid_LepPhi__Signal", "GenMatch lepton azimutal angle", binsLepPhi_, -TMath::Pi(), TMath::Pi() );
  hist1D_GenMatch__Signal_[ "LepPhi" ]->SetXTitle( "#phi_{l}^{gen.}" );
  hist1D_GenMatch__Signal_[ "LepPz" ] = dir_GenMatchValid__Signal.make< TH1D >( "GenMatchValid_LepPz__Signal", "GenMatch lepton longitudinal momentum", binsLepPz_, -maxLepPz_, maxLepPz_ );
  hist1D_GenMatch__Signal_[ "LepPz" ]->SetXTitle( "p_{z, l}^{gen.} (GeV)" );
  hist1D_GenMatch__Signal_[ "NuM" ] = dir_GenMatchValid__Signal.make< TH1D >( "GenMatchValid_NuM__Signal", "GenMatch neutrino mass", binsNuM_, -maxNuM_, maxNuM_ );
  hist1D_GenMatch__Signal_[ "NuM" ]->SetXTitle( "m_{#nu}^{gen.} (GeV)" );
  hist1D_GenMatch__Signal_[ "NuPt" ] = dir_GenMatchValid__Signal.make< TH1D >( "GenMatchValid_NuPt__Signal", "GenMatch neutrino transverse momentum", binsNuPt_, 0., maxNuPt_ );
  hist1D_GenMatch__Signal_[ "NuPt" ]->SetXTitle( "p_{t, #nu}^{gen.} (GeV)" );
  hist1D_GenMatch__Signal_[ "NuEta" ] = dir_GenMatchValid__Signal.make< TH1D >( "GenMatchValid_NuEta__Signal", "GenMatch neutrino pseudo-rapidity", binsNuEta_, -maxNuEta_, maxNuEta_ );
  hist1D_GenMatch__Signal_[ "NuEta" ]->SetXTitle( "#eta_{#nu}^{gen.}" );
  hist1D_GenMatch__Signal_[ "NuPhi" ] = dir_GenMatchValid__Signal.make< TH1D >( "GenMatchValid_NuPhi__Signal", "GenMatch neutrino azimutal angle", binsNuPhi_, -TMath::Pi(), TMath::Pi() );
  hist1D_GenMatch__Signal_[ "NuPhi" ]->SetXTitle( "#phi_{#nu}^{gen.}" );
  hist1D_GenMatch__Signal_[ "NuPz" ] = dir_GenMatchValid__Signal.make< TH1D >( "GenMatchValid_NuPz__Signal", "GenMatch neutrino longitudinal momentum", binsNuPz_, -maxNuPz_, maxNuPz_ );
  hist1D_GenMatch__Signal_[ "NuPz" ]->SetXTitle( "p_{z, #nu}^{gen.} (GeV)" );
  hist1D_GenMatch__Signal_[ "TopHadM" ] = dir_GenMatchValid__Signal.make< TH1D >( "GenMatchValid_TopHadM__Signal", "GenMatch hadronic top mass", binsTopM_, minTopM_, maxTopM_ );
  hist1D_GenMatch__Signal_[ "TopHadM" ]->SetXTitle( "m_{t_{h}}^{gen.} (GeV)" );
  hist1D_GenMatch__Signal_[ "GoodHitFit" ] = dir_GenMatchValid__Signal.make< TH1D >( "GenMatchValid_GoodHitFit__Signal", "HitFit ranking of GenMatch solution", binsHitFitNValid_ + 1, -0.5, binsHitFitNValid_ + 0.5  );
  hist1D_GenMatch__Signal_[ "GoodHitFit" ]->SetXTitle( "rank^{HitFit}" );
  for ( MapTH1D::const_iterator iHist = hist1D_GenMatch__Signal_.begin(); iHist != hist1D_GenMatch__Signal_.end(); ++iHist ) {
    iHist->second->SetYTitle( "events" );
  }
  // 2-dim
  hist2D_GenMatch__Signal_[ "PatPdgId" ] = dir_GenMatchValid__Signal.make< TH2D >( "GenMatchValid_PatPdgId__Signal", "PAT vs. TQAF match results", tqafMatchPartonLabels_.size()-1, 0., tqafMatchPartonLabels_.size()-1., patMatchPartonLabels_.size()-1, 0., patMatchPartonLabels_.size()-1. );
  hist2D_GenMatch__Signal_[ "PatPdgId" ]->SetXTitle( "PAT" );
  assignTqafMatchPartonNames( hist2D_GenMatch__Signal_[ "PatPdgId" ]->GetXaxis() );
  hist2D_GenMatch__Signal_[ "PatPdgId" ]->SetYTitle( "TQAF" );
  assignPatMatchPartonNames( hist2D_GenMatch__Signal_[ "PatPdgId" ]->GetYaxis() );
  hist2D_GenMatch__Signal_[ "TqafParton" ] = dir_GenMatchValid__Signal.make< TH2D >( "GenMatchValid_TqafParton__Signal", "TQAF vs. PAT match results", patMatchPartonLabels_.size()-1, 0., patMatchPartonLabels_.size()-1., tqafMatchPartonLabels_.size()-1, 0., tqafMatchPartonLabels_.size()-1. );
  hist2D_GenMatch__Signal_[ "TqafParton" ]->SetXTitle( "TQAF" );
  assignPatMatchPartonNames( hist2D_GenMatch__Signal_[ "TqafParton" ]->GetXaxis() );
  hist2D_GenMatch__Signal_[ "TqafParton" ]->SetYTitle( "PAT" );
  assignTqafMatchPartonNames( hist2D_GenMatch__Signal_[ "TqafParton" ]->GetYaxis() );
  for ( MapTH2D::const_iterator iHist = hist2D_GenMatch__Signal_.begin(); iHist != hist2D_GenMatch__Signal_.end(); ++iHist ) {
    iHist->second->SetZTitle( "events" );
  }

  TFileDirectory dir_Generator_HitFit( dir_Generator.mkdir( "Generator_HitFit", "HitFit histograms using generator information" ) );
  TFileDirectory dir_Generator_HitFitValid( dir_Generator_HitFit.mkdir( "Generator_HitFitValid", "HitFit histograms using generator information for valid hypotheses" ) );

  // 2-dim
  hist2D_Generator_HitFit_[ "DecayChn_nValid" ] = dir_Generator_HitFit.make< TH2D >( "Generator_HitFit_DecayChn_nValid", "No. of valid HitFit hypothesis vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., binsHitFitNValid_ + 1, -0.5, binsHitFitNValid_ + 0.5 );
  hist2D_Generator_HitFit_[ "DecayChn_nValid" ]->SetYTitle( "valid HitFit hypos" );
  // 2-dim
  hist2D_Generator_HitFit_[ "DecayChn_nRealNuSol" ] = dir_Generator_HitFitValid.make< TH2D >( "Generator_HitFitValid_DecayChn_nRealNuSol", "No. of real HitFit solutions vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., 4, -1.5, 2.5 );
  hist2D_Generator_HitFit_[ "DecayChn_nRealNuSol" ]->SetYTitle( "solutions_{real #nu}^{HitFit}" );
  hist2D_Generator_HitFit_[ "DecayChn_Prob" ] = dir_Generator_HitFitValid.make< TH2D >( "Generator_HitFitValid_DecayChn_Prob", "HitFit probabilty vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., binsHitFitProb_, 0., 1. );
  hist2D_Generator_HitFit_[ "DecayChn_Prob" ]->SetYTitle( "P^{HitFit}" );
  hist2D_Generator_HitFit_[ "DecayChn_ProbLow" ] = dir_Generator_HitFitValid.make< TH2D >( "Generator_HitFitValid_DecayChn_ProbLow", "HitFit probabilty vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., binsHitFitProbLow_, 0., maxHitFitProbLow_ );
  hist2D_Generator_HitFit_[ "DecayChn_ProbLow" ]->SetYTitle( "P^{HitFit}" );
  hist2D_Generator_HitFit_[ "DecayChn_ProbLog" ] = dir_Generator_HitFitValid.make< TH2D >( "Generator_HitFitValid_DecayChn_ProbLog", "HitFit probabilty vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., binsHitFitProbLog_, minHitFitProbLog_, 0. );
  hist2D_Generator_HitFit_[ "DecayChn_ProbLog" ]->SetYTitle( "log_{10} P" );
  hist2D_Generator_HitFit_[ "DecayChn_Chi2" ] = dir_Generator_HitFitValid.make< TH2D >( "Generator_HitFitValid_DecayChn_Chi2", "HitFit #chi^{2} vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., binsHitFitChi2_, 0., maxHitFitChi2_ );
  hist2D_Generator_HitFit_[ "DecayChn_Chi2" ]->SetYTitle( "(#chi^{2})^{HitFit}" );
  hist2D_Generator_HitFit_[ "DecayChn_MT" ] = dir_Generator_HitFitValid.make< TH2D >( "Generator_HitFitValid_DecayChn_MT", "HitFit top mass vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., binsHitFitMT_, minHitFitMT_, maxHitFitMT_ );
  hist2D_Generator_HitFit_[ "DecayChn_MT" ]->SetYTitle( "m_{top}^{HitFit} (GeV)" );
  hist2D_Generator_HitFit_[ "DecayChn_SigMT" ] = dir_Generator_HitFitValid.make< TH2D >( "Generator_HitFitValid_DecayChn_SigMT", "HitFit top mass uncertainty vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., binsHitFitSigMT_, 0., maxHitFitSigMT_ );
  hist2D_Generator_HitFit_[ "DecayChn_SigMT" ]->SetYTitle( "#sigma_{m_{top}}^{HitFit} (GeV)" );
  hist2D_Generator_HitFit_[ "DecayChn_SigMTRel" ] = dir_Generator_HitFitValid.make< TH2D >( "Generator_HitFitValid_DecayChn_SigMTRel", "Relative HitFit top mass uncertainty vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., binsHitFitSigMTRel_, 0., maxHitFitSigMTRel_ );
  hist2D_Generator_HitFit_[ "DecayChn_SigMTRel" ]->SetYTitle( "#frac{#sigma_{m_{t}}^{HitFit}}{m_{t}^{HitFit}}" );
  hist2D_Generator_HitFit_[ "DecayChn_TopLepM" ] = dir_Generator_HitFitValid.make< TH2D >( "Generator_HitFitValid_DecayChn_TopLepM", "HitFit leptonic top mass vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., binsHitFitMT_, minHitFitMT_, maxHitFitMT_ );
  hist2D_Generator_HitFit_[ "DecayChn_TopLepM" ]->SetYTitle( "m_{t, l}^{HitFit} (GeV)" );
  hist2D_Generator_HitFit_[ "DecayChn_LepM" ] = dir_Generator_HitFitValid.make< TH2D >( "Generator_HitFitValid_DecayChn_LepM", "HitFit lepton mass vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., binsLepM_, -maxLepM_, maxLepM_ );
  hist2D_Generator_HitFit_[ "DecayChn_LepM" ]->SetYTitle( "m_{l}^{HitFit} (GeV)" );
  hist2D_Generator_HitFit_[ "DecayChn_LepPt" ] = dir_Generator_HitFitValid.make< TH2D >( "Generator_HitFitValid_DecayChn_LepPt", "HitFit lepton transverse momentum vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., binsLepPt_, 0., maxLepPt_ );
  hist2D_Generator_HitFit_[ "DecayChn_LepPt" ]->SetYTitle( "p_{t, l}^{HitFit} (GeV)" );
  hist2D_Generator_HitFit_[ "DecayChn_LepEta" ] = dir_Generator_HitFitValid.make< TH2D >( "Generator_HitFitValid_DecayChn_LepEta", "HitFit lepton pseudo-rapidity vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., binsLepEta_, -maxLepEta_, maxLepEta_ );
  hist2D_Generator_HitFit_[ "DecayChn_LepEta" ]->SetYTitle( "#eta_{l}^{HitFit}" );
  hist2D_Generator_HitFit_[ "DecayChn_LepPhi" ] = dir_Generator_HitFitValid.make< TH2D >( "Generator_HitFitValid_DecayChn_LepPhi", "HitFit lepton azimutal angle vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., binsLepPhi_, -TMath::Pi(), TMath::Pi() );
  hist2D_Generator_HitFit_[ "DecayChn_LepPhi" ]->SetYTitle( "#phi_{l}^{HitFit}" );
  hist2D_Generator_HitFit_[ "DecayChn_LepPz" ] = dir_Generator_HitFitValid.make< TH2D >( "Generator_HitFitValid_DecayChn_LepPz", "HitFit lepton longitudinal momentum vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., binsLepPz_, -maxLepPz_, maxLepPz_ );
  hist2D_Generator_HitFit_[ "DecayChn_LepPz" ]->SetYTitle( "p_{z, l}^{HitFit} (GeV)" );
  hist2D_Generator_HitFit_[ "DecayChn_NuM" ] = dir_Generator_HitFitValid.make< TH2D >( "Generator_HitFitValid_DecayChn_NuM", "HitFit neutrino mass vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., binsNuM_, -maxNuM_, maxNuM_ );
  hist2D_Generator_HitFit_[ "DecayChn_NuM" ]->SetYTitle( "m_{#nu}^{HitFit} (GeV)" );
  hist2D_Generator_HitFit_[ "DecayChn_NuPt" ] = dir_Generator_HitFitValid.make< TH2D >( "Generator_HitFitValid_DecayChn_NuPt", "HitFit neutrino transverse momentum vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., binsNuPt_, 0., maxNuPt_ );
  hist2D_Generator_HitFit_[ "DecayChn_NuPt" ]->SetYTitle( "p_{t, #nu}^{HitFit} (GeV)" );
  hist2D_Generator_HitFit_[ "DecayChn_NuEta" ] = dir_Generator_HitFitValid.make< TH2D >( "Generator_HitFitValid_DecayChn_NuEta", "HitFit neutrino pseudo-rapidity vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., binsNuEta_, -maxNuEta_, maxNuEta_ );
  hist2D_Generator_HitFit_[ "DecayChn_NuEta" ]->SetYTitle( "#eta_{#nu}^{HitFit}" );
  hist2D_Generator_HitFit_[ "DecayChn_NuPhi" ] = dir_Generator_HitFitValid.make< TH2D >( "Generator_HitFitValid_DecayChn_NuPhi", "HitFit neutrino azimutal angle vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., binsNuPhi_, -TMath::Pi(), TMath::Pi() );
  hist2D_Generator_HitFit_[ "DecayChn_NuPhi" ]->SetYTitle( "#phi_{#nu}^{HitFit}" );
  hist2D_Generator_HitFit_[ "DecayChn_NuPz" ] = dir_Generator_HitFitValid.make< TH2D >( "Generator_HitFitValid_DecayChn_NuPz", "HitFit neutrino longitudinal momentum vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., binsNuPz_, -maxNuPz_, maxNuPz_ );
  hist2D_Generator_HitFit_[ "DecayChn_NuPz" ]->SetYTitle( "p_{z, #nu}^{HitFit} (GeV)" );
  hist2D_Generator_HitFit_[ "DecayChn_TopHadM" ] = dir_Generator_HitFitValid.make< TH2D >( "Generator_HitFitValid_DecayChn_TopHadM", "HitFit hadronic top mass vs. decay channel", decayChnLabels_.size()-1, 0., decayChnLabels_.size()-1., binsHitFitMT_, minHitFitMT_, maxHitFitMT_ );
  hist2D_Generator_HitFit_[ "DecayChn_TopHadM" ]->SetYTitle( "m_{t, h}^{HitFit} (GeV)" );
  for ( MapTH2D::const_iterator iHist = hist2D_Generator_HitFit_.begin(); iHist != hist2D_Generator_HitFit_.end(); ++iHist ) {
    iHist->second->SetYTitle( "" );
    assignDecayChnNames( iHist->second->GetXaxis() );
    iHist->second->SetZTitle( "events" );
  }

  TFileDirectory dir_Generator_HitFit__Signal( dir_Generator.mkdir( "Generator_HitFit__Signal", "HitFit histograms using generator information for valid hypotheses, signal events" ) );
  TFileDirectory dir_Generator_HitFitValid__Signal( dir_Generator_HitFit__Signal.mkdir( "Generator_HitFitValid__Signal", "HitFit histograms using generator information for valid hypotheses, signal events" ) );

  TFileDirectory dir_Generator_HitFitFound__Signal( dir_Generator.mkdir( "Generator_HitFitFound__Signal", "HitFit histograms using generator information for valid hypotheses, found HitFit solution in GenMatch" ) );
  TFileDirectory dir_Generator_HitFitFoundValid__Signal( dir_Generator_HitFitFound__Signal.mkdir( "Generator_HitFitFoundValid__Signal", "HitFit histograms using generator information for valid hypotheses, found HitFit solution in GenMatch" ) );

  TFileDirectory dir_Generator_HitFitGood__Signal( dir_Generator.mkdir( "Generator_HitFitGood__Signal", "HitFit histograms using generator information for valid hypotheses, good HitFit solution in GenMatch" ) );
  TFileDirectory dir_Generator_HitFitGoodValid__Signal( dir_Generator_HitFitGood__Signal.mkdir( "Generator_HitFitGoodValid__Signal", "HitFit histograms using generator information for valid hypotheses, good HitFit solution in GenMatch" ) );

  // 1-dim
  hist1D_Generator_HitFit__Signal_[ "DiffTopLepM" ] = dir_Generator_HitFitValid__Signal.make< TH1D >( "Generator_HitFitValid_DiffTopLepM", "HitFit leptonic top mass deviation", binsDiffTopM_, -maxDiffTopM_, maxDiffTopM_ );
  hist1D_Generator_HitFit__Signal_[ "DiffTopLepM" ]->SetXTitle( "#Delta m_{t_{l}} (GeV)" );
  hist1D_Generator_HitFit__Signal_[ "DiffLepM" ] = dir_Generator_HitFitValid__Signal.make< TH1D >( "Generator_HitFitValid_DiffLepM", "HitFit lepton mass deviation", binsDiffLepM_, -maxDiffLepM_, maxDiffLepM_ );
  hist1D_Generator_HitFit__Signal_[ "DiffLepM" ]->SetXTitle( "#Delta m_{l} (GeV)" );
  hist1D_Generator_HitFit__Signal_[ "DiffLepPt" ] = dir_Generator_HitFitValid__Signal.make< TH1D >( "Generator_HitFitValid_DiffLepPt", "HitFit lepton transverse momentum deviation", binsDiffLepPt_, -maxDiffLepPt_, maxDiffLepPt_ );
  hist1D_Generator_HitFit__Signal_[ "DiffLepPt" ]->SetXTitle( "#Delta p_{t, l} (GeV)" );
  hist1D_Generator_HitFit__Signal_[ "DiffLepEta" ] = dir_Generator_HitFitValid__Signal.make< TH1D >( "Generator_HitFitValid_DiffLepEta", "HitFit lepton pseudo-rapidity deviation", binsDiffLepEta_, -maxDiffLepEta_, maxDiffLepEta_ );
  hist1D_Generator_HitFit__Signal_[ "DiffLepEta" ]->SetXTitle( "#Delta #eta_{l}" );
  hist1D_Generator_HitFit__Signal_[ "DiffLepPhi" ] = dir_Generator_HitFitValid__Signal.make< TH1D >( "Generator_HitFitValid_DiffLepPhi", "HitFit lepton azimutal angle deviation", binsDiffLepPhi_, -maxDiffLepPhi_, maxDiffLepPhi_ );
  hist1D_Generator_HitFit__Signal_[ "DiffLepPhi" ]->SetXTitle( "#Delta #phi_{l}" );
  hist1D_Generator_HitFit__Signal_[ "DiffLepPz" ] = dir_Generator_HitFitValid__Signal.make< TH1D >( "Generator_HitFitValid_DiffLepPz", "HitFit lepton longitudinal momentum deviation", binsDiffLepPz_, -binsDiffLepPz_, binsDiffLepPz_ );
  hist1D_Generator_HitFit__Signal_[ "DiffLepPz" ]->SetXTitle( "#Delta p_{z, l} (GeV)" );
  hist1D_Generator_HitFit__Signal_[ "DiffNuM" ] = dir_Generator_HitFitValid__Signal.make< TH1D >( "Generator_HitFitValid_DiffNuM", "HitFit neutrino mass deviation", binsDiffNuM_, -maxDiffNuM_, maxDiffNuM_ );
  hist1D_Generator_HitFit__Signal_[ "DiffNuM" ]->SetXTitle( "#Delta m_{#nu} (GeV)" );
  hist1D_Generator_HitFit__Signal_[ "DiffNuPt" ] = dir_Generator_HitFitValid__Signal.make< TH1D >( "Generator_HitFitValid_DiffNuPt", "HitFit neutrino transverse momentum deviation", binsDiffNuPt_, -maxDiffNuPt_, maxDiffNuPt_ );
  hist1D_Generator_HitFit__Signal_[ "DiffNuPt" ]->SetXTitle( "#Delta p_{t, #nu} (GeV)" );
  hist1D_Generator_HitFit__Signal_[ "DiffNuEta" ] = dir_Generator_HitFitValid__Signal.make< TH1D >( "Generator_HitFitValid_DiffNuEta", "HitFit neutrino pseudo-rapidity deviation", binsDiffNuEta_, -maxDiffNuEta_, maxDiffNuEta_ );
  hist1D_Generator_HitFit__Signal_[ "DiffNuEta" ]->SetXTitle( "#Delta #eta_{#nu}" );
  hist1D_Generator_HitFit__Signal_[ "DiffNuPhi" ] = dir_Generator_HitFitValid__Signal.make< TH1D >( "Generator_HitFitValid_DiffNuPhi", "HitFit neutrino azimutal angle deviation", binsDiffNuPhi_, -maxDiffNuPhi_, maxDiffNuPhi_ );
  hist1D_Generator_HitFit__Signal_[ "DiffNuPhi" ]->SetXTitle( "#Delta #phi_{#nu}" );
  hist1D_Generator_HitFit__Signal_[ "DiffNuPz" ] = dir_Generator_HitFitValid__Signal.make< TH1D >( "Generator_HitFitValid_DiffNuPz", "HitFit neutrino longitudinal momentum deviation", binsDiffNuPz_, -maxDiffNuPz_, maxDiffNuPz_ );
  hist1D_Generator_HitFit__Signal_[ "DiffNuPz" ]->SetXTitle( "#Delta p_{z, #nu} (GeV)" );
  hist1D_Generator_HitFit__Signal_[ "DiffTopHadM" ] = dir_Generator_HitFitValid__Signal.make< TH1D >( "Generator_HitFitValid_DiffTopHadM", "HitFit hadronic top mass deviation", binsDiffTopM_, -maxDiffTopM_, maxDiffTopM_ );
  hist1D_Generator_HitFit__Signal_[ "DiffTopHadM" ]->SetXTitle( "#Delta m_{t_{h}} (GeV)" );
  for ( MapTH1D::const_iterator iHist = hist1D_Generator_HitFit__Signal_.begin(); iHist != hist1D_Generator_HitFit__Signal_.end(); ++iHist ) {
    iHist->second->SetYTitle( "events" );
    std::string nameFound( iHist->second->GetName() );
    if ( std::string( iHist->second->GetName() ).substr( 10, strHitFit.size() ) == strHitFit )
      hist1D_Generator_HitFitFound__Signal_[ iHist->first ] = dir_Generator_HitFitFound__Signal.make< TH1D >( *( ( TH1D* )( iHist->second->Clone( nameFound.replace( 16, 0, "Found" ).c_str() ) ) ) );
    else if ( std::string( iHist->second->GetName() ).substr( 10, strHitFitValid.size() ) == strHitFitValid )
      hist1D_Generator_HitFitFound__Signal_[ iHist->first ] = dir_Generator_HitFitFoundValid__Signal.make< TH1D >( *( ( TH1D* )( iHist->second->Clone( nameFound.replace( 16, 0, "Found" ).c_str() ) ) ) );
    std::string nameGood( iHist->second->GetName() );
    if ( std::string( iHist->second->GetName() ).substr( 10, strHitFit.size() ) == strHitFit )
      hist1D_Generator_HitFitGood__Signal_[ iHist->first ] = dir_Generator_HitFitGood__Signal.make< TH1D >( *( ( TH1D* )( iHist->second->Clone( nameGood.replace( 16, 0, "Good" ).c_str() ) ) ) );
    else if ( std::string( iHist->second->GetName() ).substr( 10, strHitFitValid.size() ) == strHitFitValid )
      hist1D_Generator_HitFitGood__Signal_[ iHist->first ] = dir_Generator_HitFitGoodValid__Signal.make< TH1D >( *( ( TH1D* )( iHist->second->Clone( nameGood.replace( 16, 0, "Good" ).c_str() ) ) ) );
  }

  TFileDirectory dir_GenMatch_HitFitGood__Signal( fileService->mkdir( "GenMatch_HitFitGood__Signal", "HitFit vs. GenMatch for valid hypotheses" ) );

  // 2-dim
  hist2D_GenMatch_HitFitGood__Signal_[ "Prob_ProbFound" ] = dir_GenMatch_HitFitGood__Signal.make< TH2D >( "GenMatch_HitFit_Prob_ProbFound__Signal", "HitFit probabilty GenMatch hypothesis vs. best HitFit probabilty", binsHitFitProb_, 0., 1., binsHitFitProb_, 0., 1. );
  hist2D_GenMatch_HitFitGood__Signal_[ "Prob_ProbFound" ]->SetXTitle( "P^{HitFit}" );
  hist2D_GenMatch_HitFitGood__Signal_[ "Prob_ProbFound" ]->SetYTitle( "P_{GenMatch}^{HitFit}" );
  hist2D_GenMatch_HitFitGood__Signal_[ "Chi2_Chi2Found" ] = dir_GenMatch_HitFitGood__Signal.make< TH2D >( "GenMatch_HitFit_Chi2_Chi2Found__Signal", "HitFit #chi^{2} GenMatch hypothesis vs. best HitFit #chi^{2}", binsHitFitChi2_, 0., maxHitFitChi2_, binsHitFitChi2_, 0., maxHitFitChi2_ );
  hist2D_GenMatch_HitFitGood__Signal_[ "Chi2_Chi2Found" ]->SetXTitle( "(#chi^{2})^{HitFit}" );
  hist2D_GenMatch_HitFitGood__Signal_[ "Chi2_Chi2Found" ]->SetYTitle( "(#chi^{2})_{GenMatch}^{HitFit}" );
  hist2D_GenMatch_HitFitGood__Signal_[ "MT_MTFound" ] = dir_GenMatch_HitFitGood__Signal.make< TH2D >( "GenMatch_HitFit_MT_MTFound__Signal", "HitFit top mass GenMatch hypothesis vs. best HitFit top mass", binsHitFitMT_/4, minHitFitMT_, maxHitFitMT_, binsHitFitMT_/4, minHitFitMT_, maxHitFitMT_ );
  hist2D_GenMatch_HitFitGood__Signal_[ "MT_MTFound" ]->SetXTitle( "m_{top}^{HitFit}" );
  hist2D_GenMatch_HitFitGood__Signal_[ "MT_MTFound" ]->SetYTitle( "m_{top}_{GenMatch}^{HitFit}" );
  hist2D_GenMatch_HitFitGood__Signal_[ "ProbFound_MTFound" ] = dir_GenMatch_HitFitGood__Signal.make< TH2D >( "GenMatch_HitFit_ProbFound_MTFound", "HitFit top mass GenMatch hypothesis vs. probabilty", binsHitFitProb_, 0., 1. , binsHitFitMT_, minHitFitMT_, maxHitFitMT_ );
  hist2D_GenMatch_HitFitGood__Signal_[ "ProbFound_MTFound" ]->SetXTitle( "P_{GenMatch}^{HitFit}" );
  hist2D_GenMatch_HitFitGood__Signal_[ "ProbFound_MTFound" ]->SetYTitle( "m_{top}_{GenMatch}^{HitFit}" );
  hist2D_GenMatch_HitFitGood__Signal_[ "ProbFound_LepPtFound" ] = dir_GenMatch_HitFitGood__Signal.make< TH2D >( "GenMatch_HitFit_ProbFound_LepPtFound", "HitFit top mass GenMatch hypothesis vs. lepton transverse momentum", binsHitFitProb_, 0., 1. , binsLepPt_, 0., maxLepPt_ );
  hist2D_GenMatch_HitFitGood__Signal_[ "ProbFound_LepPtFound" ]->SetXTitle( "P_{GenMatch}^{HitFit}" );
  hist2D_GenMatch_HitFitGood__Signal_[ "ProbFound_LepPtFound" ]->SetYTitle( "p_{t, l}_{GenMatch}^{HitFit} (GeV)" );
  hist2D_GenMatch_HitFitGood__Signal_[ "ProbFound_NuPtFound" ] = dir_GenMatch_HitFitGood__Signal.make< TH2D >( "GenMatch_HitFit_ProbFound_NuPtFound", "HitFit top mass GenMatch hypothesis vs. neutrino transverse momentum", binsHitFitProb_, 0., 1. , binsNuPt_, 0., maxNuPt_ );
  hist2D_GenMatch_HitFitGood__Signal_[ "ProbFound_NuPtFound" ]->SetXTitle( "P_{GenMatch}^{HitFit}" );
  hist2D_GenMatch_HitFitGood__Signal_[ "ProbFound_NuPtFound" ]->SetYTitle( "p_{t, #nu}_{GenMatch}^{HitFit} (GeV)" );
  hist2D_GenMatch_HitFitGood__Signal_[ "ProbFound_LepPtRecoFound" ] = dir_GenMatch_HitFitGood__Signal.make< TH2D >( "GenMatch_HitFit_ProbFound_LepPtRecoFound", "HitFit top mass GenMatch hypothesis vs. reconstructed lepton transverse momentum", binsHitFitProb_, 0., 1. , binsLepPt_, 0., maxLepPt_ );
  hist2D_GenMatch_HitFitGood__Signal_[ "ProbFound_LepPtRecoFound" ]->SetXTitle( "P_{GenMatch}^{HitFit}" );
  hist2D_GenMatch_HitFitGood__Signal_[ "ProbFound_LepPtRecoFound" ]->SetYTitle( "p_{t, l}^{Reco} (GeV)" );
  hist2D_GenMatch_HitFitGood__Signal_[ "ProbFound_METRecoFound" ] = dir_GenMatch_HitFitGood__Signal.make< TH2D >( "GenMatch_HitFit_ProbFound_METRecoFound", "HitFit top mass GenMatch hypothesis vs. reconstructed missing transverse momentum", binsHitFitProb_, 0., 1. , binsNuPt_, 0., maxNuPt_ );
  hist2D_GenMatch_HitFitGood__Signal_[ "ProbFound_METRecoFound" ]->SetXTitle( "P_{GenMatch}^{HitFit}" );
  hist2D_GenMatch_HitFitGood__Signal_[ "ProbFound_METRecoFound" ]->SetYTitle( "E_{t, miss.}^{Reco} (GeV)" );
  for ( MapTH2D::const_iterator iHist = hist2D_GenMatch_HitFitGood__Signal_.begin(); iHist != hist2D_GenMatch_HitFitGood__Signal_.end(); ++iHist ) {
    iHist->second->SetZTitle( "events" );
  }

}


// Event loop
void TopHitFitAnalyzer::analyze( const edm::Event & iEvent, const edm::EventSetup & iSetup )
{

  // Reset
  ttGenEvent_          =  0;
  isSignal_            = false;
  isSignalTau_         = false;
  nHitFitHyposValid_   =  0;
  nGenMatchHyposValid_ =  0;
  hitFitHypoFound_1_   = -1;
  hitFitHypoFound_2_   = -1;
  hitFitHypoFound_     = -1;

  // TQAF semi-leptonic event
  iEvent.getByLabel( ttSemiLeptonicEventTag_, ttSemiLeptonicEvent_ );
  ttSemiLeptonicEvent_->print( verbosity_ );

  // PAT collections
  iEvent.getByLabel( patMuonsTag_, patMuons_ );
  iEvent.getByLabel( patJetsTag_ , patJets_ );
  iEvent.getByLabel( patMETsTag_ , patMETs_ );

  std::map< unsigned, std::vector< int > > hitFitJetLeptonCombis;
  for ( unsigned iHypo = 0; iHypo < ttSemiLeptonicEvent_->numberOfAvailableHypos( TtEvent::kHitFit ); ++iHypo ) {
    if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit, iHypo ) ) {
      ++nHitFitHyposValid_;
      hitFitJetLeptonCombis[ iHypo ] = ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kHitFit, iHypo );
    }
  }
  fill1D_HitFit( hist1D_HitFit_ );
  fill2D_HitFit( hist2D_HitFit_ );
  if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit ) ) {
    fill1D_HitFitValid( hist1D_HitFit_ );
    fill2D_HitFitValid( hist2D_HitFit_ );
  }

  // MC specific

  if ( ! iEvent.isRealData() ) {

    // TQAF MC event
    ttGenEvent_ = const_cast< TtGenEvent* >( ttSemiLeptonicEvent_->genEvent().get() );

    fill1D_Generator( hist1D_Generator_ );
    fill2D_Generator( hist2D_Generator_ );

    // TTbar specific

    if ( ttGenEvent_->isTtBar() ) {

      // Decay channels
      // FIXME: This is still muon-specific
      unsigned nTrueOtherMuon( 0 );
      if ( ttGenEvent_->isSemiLeptonic( WDecay::kElec ) ) {
        decayChn_ = ttGenEvent_->semiLeptonicChannel();
      } // if ( ttGenEvent_->isSemiLeptonic( WDecay::kElec ) )
      else if ( ttGenEvent_->isSemiLeptonic( WDecay::kMuon ) ) {
        decayChn_ = ttGenEvent_->semiLeptonicChannel();
        isSignal_ = true;
      } // if ( ttGenEvent_->isSemiLeptonic( WDecay::kMuon ) )
      else if ( ttGenEvent_->isSemiLeptonic( WDecay::kTau ) ) {
        decayChn_ = ttGenEvent_->semiLeptonicChannel();
        const reco::GenParticle * genSemiTau( ttGenEvent_->singleLepton() );
        for ( size_t iD = 0; iD < genSemiTau->numberOfDaughters(); ++iD ) {
          const reco::Candidate * genTauDaughter( genSemiTau->daughter( iD ) );
          // First level daughter is a copy: need to go one level deeper
          for ( size_t iDD = 0; iDD < genTauDaughter->numberOfDaughters(); ++iDD ) {
            const reco::Candidate * genTauGrandDaughter( genTauDaughter->daughter( iDD ) );
            if ( reco::isMuon( *genTauGrandDaughter ) ) ++nTrueOtherMuon;
          }
        }
        decayChn_ += nTrueOtherMuon;
      } // if ( ttGenEvent_->isSemiLeptonic( WDecay::kTau ) )
      else if ( ttGenEvent_->isFullLeptonic() ) {
        decayChn_ = WDecay::kTau + 2;
        const reco::GenParticle * genFullLep( ttGenEvent_->lepton() );
        if ( reco::isTau( *genFullLep ) ) {
          for ( size_t iD = 0; iD < genFullLep->numberOfDaughters(); ++iD ) {
            const reco::Candidate * genTauDaughter( genFullLep->daughter( iD ) );
            // First level daughter is a copy: need to go one level deeper
            for ( size_t iDD = 0; iDD < genTauDaughter->numberOfDaughters(); ++iDD ) {
              const reco::Candidate * genTauGrandDaughter( genTauDaughter->daughter( iDD ) );
              if ( reco::isMuon( *genTauGrandDaughter ) ) ++nTrueOtherMuon;
            }
          }
        }
        else if ( reco::isMuon( *genFullLep ) ) {
           ++nTrueOtherMuon;
        }
        const reco::GenParticle * genFullLepBar( ttGenEvent_->leptonBar() );
        if ( reco::isTau( *genFullLep ) ) {
          for ( size_t iD = 0; iD < genFullLepBar->numberOfDaughters(); ++iD ) {
            const reco::Candidate * genTauDaughter( genFullLepBar->daughter( iD ) );
            // First level daughter is a copy: need to go one level deeper
            for ( size_t iDD = 0; iDD < genTauDaughter->numberOfDaughters(); ++iDD ) {
              const reco::Candidate * genTauGrandDaughter( genTauDaughter->daughter( iDD ) );
              if ( reco::isMuon( *genTauGrandDaughter ) ) ++nTrueOtherMuon;
            }
          }
        }
        else if ( fabs( genFullLepBar->pdgId() ) == 13 ) {
           ++nTrueOtherMuon;
        }
        decayChn_ += nTrueOtherMuon;
      } // if ( ttGenEvent_->isFullLeptonic() )
      if ( decayChn_ == WDecay::kTau + 1 ) isSignalTau_ = true;

      fill2D_Generator_HitFit( hist2D_Generator_HitFit_ );
      if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit ) ) {
        fill2D_Generator_HitFitValid( hist2D_Generator_HitFit_ );
      }

    } // if ( ttGenEvent_->isTtBar() )

    if ( isSignal_ ) {

      fill1D_GeneratorValid( hist1D_Generator__Signal_ );
      fill2D_GeneratorValid( hist2D_Generator__Signal_ );

      for ( unsigned iHypo = 0; iHypo < ttSemiLeptonicEvent_->numberOfAvailableHypos( TtEvent::kGenMatch ); ++iHypo ) {
        if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kGenMatch, iHypo ) ) {
          ++nGenMatchHyposValid_;
        }
      }
      fill1D_GenMatch( hist1D_GenMatch__Signal_ );
      fill2D_GenMatch( hist2D_GenMatch__Signal_ );
      if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kGenMatch ) ) {
        fill1D_GenMatchValid( hist1D_GenMatch__Signal_ );
        fill2D_GenMatchValid( hist2D_GenMatch__Signal_ );
      }

      // Determine corresponding HitFit hypotheses
      for ( unsigned iHypo = 0; iHypo < ttSemiLeptonicEvent_->numberOfAvailableHypos( TtEvent::kGenMatch ); ++iHypo ) {
        if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kGenMatch, iHypo ) ) {
          if ( hitFitHypoFound_1_ == -1 ) {
            const int hitFitHypoFound( ttSemiLeptonicEvent_->correspondingHypo( TtEvent::kGenMatch, iHypo, TtEvent::kHitFit ) );
            if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit, hitFitHypoFound ) ) hitFitHypoFound_1_ = hitFitHypoFound;
          }
          if ( hitFitHypoFound_2_ == -1 ) {
            const int hitFitHypoFound( correspondingAltHypo( TtEvent::kGenMatch, iHypo, TtEvent::kHitFit ) );
            if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit, hitFitHypoFound ) ) hitFitHypoFound_2_ = hitFitHypoFound;
          }
       } // if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kGenMatch, iHypo ) )
      } // iHypo
      if ( hitFitHypoFound_1_ == hitFitHypoFound_2_ ) {
        hitFitHypoFound_ = hitFitHypoFound_1_;
      }
      else if ( hitFitHypoFound_1_ == -1 || hitFitHypoFound_2_ == -1 ) {
        hitFitHypoFound_ = std::max( hitFitHypoFound_1_, hitFitHypoFound_2_ );
      }
      else {
        hitFitHypoFound_ = std::min( hitFitHypoFound_1_, hitFitHypoFound_2_ );
      }

      fill1D_HitFit( hist1D_HitFit__Signal_ );
      fill2D_HitFit( hist2D_HitFit__Signal_ );
      if ( hitFitHypoFound_ >= 0 ) {
        fill1D_HitFit( hist1D_HitFitFound__Signal_, hitFitHypoFound_ );
        fill2D_HitFit( hist2D_HitFitFound__Signal_, hitFitHypoFound_ );
      }
      if ( hitFitHypoFound_ == 0 ) {
        fill1D_HitFit( hist1D_HitFitGood__Signal_ );
        fill2D_HitFit( hist2D_HitFitGood__Signal_ );
      }
      fill1D_HitFit( hist1D_HitFit__SignalTau_ );
      fill2D_HitFit( hist2D_HitFit__SignalTau_ );
      if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit ) ) {
        fill1D_HitFitValid( hist1D_HitFit_ );
        fill2D_HitFitValid( hist2D_HitFit__Signal_ );
        if ( hitFitHypoFound_ >= 0 ) {
          fill1D_HitFitValid( hist1D_HitFitFound__Signal_, hitFitHypoFound_ );
          fill2D_HitFitValid( hist2D_HitFitFound__Signal_, hitFitHypoFound_ );
          fill1D_GenMatch_HitFitValid( hist1D_GenMatch_HitFitGood__Signal_, hitFitHypoFound_ );
          fill2D_GenMatch_HitFitValid( hist2D_GenMatch_HitFitGood__Signal_, hitFitHypoFound_ );
        }
        if ( hitFitHypoFound_ == 0 ) {
          fill1D_HitFitValid( hist1D_HitFitGood__Signal_ );
          fill2D_HitFitValid( hist2D_HitFitGood__Signal_ );
        }
        fill1D_HitFitValid( hist1D_HitFit__SignalTau_ );
        fill2D_HitFitValid( hist2D_HitFit__SignalTau_ );
        fill1D_Generator_HitFitValid( hist1D_Generator_HitFit__Signal_ );
        if ( hitFitHypoFound_ >= 0 ) {
          fill1D_Generator_HitFitValid( hist1D_Generator_HitFitFound__Signal_, hitFitHypoFound_ );
        }
        if ( hitFitHypoFound_ == 0 ) {
          fill1D_Generator_HitFitValid( hist1D_Generator_HitFitGood__Signal_ );
        }
      }

    } // if ( isSignal_ )
    else {

      fill1D_HitFit( hist1D_HitFit__Bkg_ );
      fill2D_HitFit( hist2D_HitFit__Bkg_ );
      if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit ) ) {
        fill1D_HitFitValid( hist1D_HitFit__Bkg_ );
        fill2D_HitFitValid( hist2D_HitFit__Bkg_ );
      }

      if ( isSignalTau_ ) {

        fill1D_HitFit( hist1D_HitFit__Tau_ );
        fill1D_HitFit( hist1D_HitFit__SignalTau_ );
        fill2D_HitFit( hist2D_HitFit__Tau_ );
        fill2D_HitFit( hist2D_HitFit__SignalTau_ );
        if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit ) ) {
          fill1D_HitFitValid( hist1D_HitFit__Tau_ );
          fill2D_HitFitValid( hist2D_HitFit__Tau_ );
          fill1D_HitFitValid( hist1D_HitFit__SignalTau_ );
          fill2D_HitFitValid( hist2D_HitFit__SignalTau_ );
        }

      } // if ( isSignalTau_ )
      else {

        fill1D_HitFit( hist1D_HitFit__BkgNoTau_ );
        fill2D_HitFit( hist2D_HitFit__BkgNoTau_ );
        if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit ) ) {
          fill1D_HitFitValid( hist1D_HitFit__BkgNoTau_ );
          fill2D_HitFitValid( hist2D_HitFit__BkgNoTau_ );
        }

      } // else ( isSignalTau_ )

      if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit ) ) {

        fill1D_HitFitValid( hist1D_HitFit__Bkg_ );
        fill2D_HitFitValid( hist2D_HitFit__Bkg_ );

        if ( decayChn_ == WDecay::kTau + 1 ) {
          fill1D_HitFitValid( hist1D_HitFit__Tau_ );
          fill2D_HitFitValid( hist2D_HitFit__Tau_ );
          fill1D_HitFitValid( hist1D_HitFit__SignalTau_ );
          fill2D_HitFitValid( hist2D_HitFit__SignalTau_ );
        } // if ( decayChn_ == WDecay::kTau + 1 )
        else {
          fill1D_HitFitValid( hist1D_HitFit__BkgNoTau_ );
          fill2D_HitFitValid( hist2D_HitFit__BkgNoTau_ );
        } // else ( decayChn_ == WDecay::kTau + 1 )

      } // if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kHitFit ) )

    } // else ( isSignal_ )

  }

}


// End job
void TopHitFitAnalyzer::endJob()
{
}


void TopHitFitAnalyzer::assignDecayChnNames( TAxis * iAxis )
{

  for ( unsigned i = 1; i < decayChnLabels_.size(); ++i ) {
    iAxis->SetBinLabel( i, decayChnLabels_.at( i ).c_str() );
  }

  return;

}

void TopHitFitAnalyzer::assignPatMatchPartonNames( TAxis * iAxis )
{

  for ( unsigned i = 1; i < patMatchPartonLabels_.size(); ++i ) {
    iAxis->SetBinLabel( i, patMatchPartonLabels_.at( i ).c_str() );
  }

  return;

}

void TopHitFitAnalyzer::assignTqafMatchPartonNames( TAxis * iAxis )
{

  for ( unsigned i = 1; i < tqafMatchPartonLabels_.size(); ++i ) {
    iAxis->SetBinLabel( i, tqafMatchPartonLabels_.at( i ).c_str() );
  }

  return;

}

float TopHitFitAnalyzer::getPatMatchPartonBin( const int pdgId )
{

  float partonBin;
  switch( pdgId ) {
    case -3:
    case -2:
    case -1:
    case  1:
    case  2:
    case  3:
      partonBin = 0.;
      break;
    case -4:
    case  4:
      partonBin = 1.;
      break;
    case -5:
    case  5:
      partonBin = 2.;
      break;
    default:
      partonBin = -1.;
  }

  return partonBin;

}


int TopHitFitAnalyzer::correspondingAltHypo( const TtEvent::HypoClassKey key1, const unsigned hyp1, const TtEvent::HypoClassKey key2 )
{

  // Swap quark and anti-quark
  std::vector< int > jetLeptonCombi( ttSemiLeptonicEvent_->jetLeptonCombination( key1, hyp1 ) );
  const int cache( jetLeptonCombi.at( TtSemiLepEvtPartons::LightQ ) );
  jetLeptonCombi[ TtSemiLepEvtPartons::LightQ ] = jetLeptonCombi.at( TtSemiLepEvtPartons::LightQBar );
  jetLeptonCombi[ TtSemiLepEvtPartons::LightQBar ] = cache;
  // Search
  for( unsigned hyp2 = 0; hyp2 < ttSemiLeptonicEvent_->numberOfAvailableHypos( key2 ); ++hyp2 ) {
    if( jetLeptonCombi == ttSemiLeptonicEvent_->jetLeptonCombination( key2, hyp2 ) ) return hyp2;
  }

  return -1; // if no corresponding hypothesis was found

}


void TopHitFitAnalyzer::fill1D_Generator( MapTH1D & hist1D )
{

  hist1D[ "DecayChn" ]->Fill( decayChn_ );

  return;

}

void TopHitFitAnalyzer::fill2D_Generator( MapTH2D & hist2D )
{

  return;

}


void TopHitFitAnalyzer::fill1D_GeneratorValid( MapTH1D & hist1D )
{

  fill1D_Generator( hist1D );

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

  return;

}

void TopHitFitAnalyzer::fill2D_GeneratorValid( MapTH2D & hist2D )
{

  fill2D_Generator( hist2D );

  return;

}

void TopHitFitAnalyzer::fill1D_HitFit( MapTH1D & hist1D, unsigned iCombi )
{

  hist1D[ "nValid" ]->Fill( nHitFitHyposValid_ );

  return;

}

void TopHitFitAnalyzer::fill2D_HitFit( MapTH2D & hist2D, unsigned iCombi )
{

  return;

}

void TopHitFitAnalyzer::fill1D_HitFitValid( MapTH1D & hist1D, unsigned iCombi )
{

  hist1D[ "nRealNuSol" ]->Fill( ttSemiLeptonicEvent_->numberOfRealNeutrinoSolutions( TtEvent::kHitFit ) );
  hist1D[ "Prob" ]->Fill( ttSemiLeptonicEvent_->hitFitProb( iCombi ) );
  hist1D[ "ProbLow" ]->Fill( ttSemiLeptonicEvent_->hitFitProb( iCombi ) );
  hist1D[ "ProbLog" ]->Fill( log10( ttSemiLeptonicEvent_->hitFitProb( iCombi ) ) );
  hist1D[ "Chi2" ]->Fill( ttSemiLeptonicEvent_->hitFitChi2( iCombi ) );
  hist1D[ "MT" ]->Fill( ttSemiLeptonicEvent_->hitFitMT( iCombi ) );
  hist1D[ "SigMT" ]->Fill( ttSemiLeptonicEvent_->hitFitSigMT( iCombi ) );
  hist1D[ "SigMTRel" ]->Fill( ttSemiLeptonicEvent_->hitFitSigMT( iCombi )/ttSemiLeptonicEvent_->hitFitMT( iCombi ) );

  hist1D[ "TopLepM" ]->Fill( ttSemiLeptonicEvent_->leptonicDecayTop( TtEvent::kHitFit, iCombi )->mass() );
  hist1D[ "LepM" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit, iCombi )->mass() );
  hist1D[ "LepPt" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit, iCombi )->pt() );
  hist1D[ "LepEta" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit, iCombi )->eta() );
  hist1D[ "LepPhi" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit, iCombi )->phi() );
  hist1D[ "LepPz" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit, iCombi )->pz() );
  hist1D[ "NuM" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit, iCombi )->mass() );
  hist1D[ "NuPt" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit, iCombi )->pt() );
  hist1D[ "NuEta" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit, iCombi )->eta() );
  hist1D[ "NuPhi" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit, iCombi )->phi() );
  hist1D[ "NuPz" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit, iCombi )->pz() );
  hist1D[ "TopHadM" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayTop( TtEvent::kHitFit, iCombi )->mass() );

  return;

}

void TopHitFitAnalyzer::fill2D_HitFitValid( MapTH2D & hist2D, unsigned iCombi )
{

  hist2D[ "Prob_MT" ]->Fill( ttSemiLeptonicEvent_->hitFitProb( iCombi ), ttSemiLeptonicEvent_->hitFitMT( iCombi ) );
  hist2D[ "Prob_SigMT" ]->Fill( ttSemiLeptonicEvent_->hitFitProb( iCombi ), ttSemiLeptonicEvent_->hitFitSigMT( iCombi ) );
  hist2D[ "Prob_SigMTRel" ]->Fill( ttSemiLeptonicEvent_->hitFitProb( iCombi ), ttSemiLeptonicEvent_->hitFitSigMT()/ttSemiLeptonicEvent_->hitFitMT( iCombi ) );
  hist2D[ "MT_SigMT" ]->Fill( ttSemiLeptonicEvent_->hitFitMT( iCombi ), ttSemiLeptonicEvent_->hitFitSigMT( iCombi ) );
  hist2D[ "MT_SigMTRel" ]->Fill( ttSemiLeptonicEvent_->hitFitMT( iCombi ), ttSemiLeptonicEvent_->hitFitSigMT()/ttSemiLeptonicEvent_->hitFitMT( iCombi ) );

  return;

}

void TopHitFitAnalyzer::fill1D_GenMatch( MapTH1D & hist1D, unsigned iCombi )
{

  hist1D[ "nValid" ]->Fill( nGenMatchHyposValid_ );

  return;

}

void TopHitFitAnalyzer::fill2D_GenMatch( MapTH2D & hist2D, unsigned iCombi )
{

  return;

}

void TopHitFitAnalyzer::fill1D_GenMatchValid( MapTH1D & hist1D, unsigned iCombi )
{

  hist1D[ "SumPt" ]->Fill( ttSemiLeptonicEvent_->genMatchSumPt( iCombi ) );
  hist1D[ "SumDR" ]->Fill( ttSemiLeptonicEvent_->genMatchSumDR( iCombi ) );

  hist1D[ "TopLepM" ]->Fill( ttSemiLeptonicEvent_->leptonicDecayTop( TtEvent::kGenMatch, iCombi )->mass() );
  hist1D[ "LepM" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch, iCombi )->mass() );
  hist1D[ "LepPt" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch, iCombi )->pt() );
  hist1D[ "LepEta" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch, iCombi )->eta() );
  hist1D[ "LepPhi" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch, iCombi )->phi() );
  hist1D[ "LepPz" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch, iCombi )->pz() );
  hist1D[ "NuM" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch, iCombi )->mass() );
  hist1D[ "NuPt" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch, iCombi )->pt() );
  hist1D[ "NuEta" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch, iCombi )->eta() );
  hist1D[ "NuPhi" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch, iCombi )->phi() );
  hist1D[ "NuPz" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch, iCombi )->pz() );
  hist1D[ "TopHadM" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayTop( TtEvent::kGenMatch, iCombi )->mass() );

  hist1D[ "GoodHitFit" ]->Fill( hitFitHypoFound_ );

  return;

}

void TopHitFitAnalyzer::fill2D_GenMatchValid( MapTH2D & hist2D, unsigned iCombi )
{

  std::vector< int > genMatchJetLeptonCombi( ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch, iCombi ) );

  const pat::Jet genMatchQJet( patJets_->at( ( unsigned )genMatchJetLeptonCombi.at( TtSemiLepEvtPartons::LightQ ) ) );
  const reco::GenParticleRef genMatchLightQPatMatchRef( genMatchQJet.genParticleRef() );
  const int genMatchLightQPatMatchPdgId( genMatchLightQPatMatchRef.isNonnull() ? genMatchLightQPatMatchRef->pdgId() : -6 );
  hist2D[ "PatPdgId" ]->Fill( TtSemiLepEvtPartons::LightQ, getPatMatchPartonBin( genMatchLightQPatMatchPdgId ) );
  const pat::Jet genMatchQbarJet( patJets_->at( ( unsigned )genMatchJetLeptonCombi.at( TtSemiLepEvtPartons::LightQBar ) ) );
  const reco::GenParticleRef genMatchLightQbarPatMatchRef( genMatchQbarJet.genParticleRef() );
  const int genMatchLightQbarPatMatchPdgId( genMatchLightQbarPatMatchRef.isNonnull() ? genMatchLightQbarPatMatchRef->pdgId() : -6 );
  hist2D[ "PatPdgId" ]->Fill( TtSemiLepEvtPartons::LightQBar, getPatMatchPartonBin( genMatchLightQbarPatMatchPdgId ) );
  const pat::Jet genMatchHadBJet( patJets_->at( ( unsigned )genMatchJetLeptonCombi.at( TtSemiLepEvtPartons::HadB ) ) );
  const reco::GenParticleRef genMatchHadBPatMatchRef( genMatchHadBJet.genParticleRef() );
  const int genMatchHadBPatMatchPdgId( genMatchHadBPatMatchRef.isNonnull() ? genMatchHadBPatMatchRef->pdgId() : -6 );
  hist2D[ "PatPdgId" ]->Fill( TtSemiLepEvtPartons::HadB, getPatMatchPartonBin( genMatchHadBPatMatchPdgId ) );
  const pat::Jet genMatchLepBJet( patJets_->at( ( unsigned )genMatchJetLeptonCombi.at( TtSemiLepEvtPartons::LepB ) ) );
  const reco::GenParticleRef genMatchLepBPatMatchRef( genMatchLepBJet.genParticleRef() );
  const int genMatchLepBPatMatchPdgId( genMatchLepBPatMatchRef.isNonnull() ? genMatchLepBPatMatchRef->pdgId() : -6 );
  hist2D[ "PatPdgId" ]->Fill( TtSemiLepEvtPartons::LepB, getPatMatchPartonBin( genMatchLepBPatMatchPdgId ) );

  for ( unsigned iJet = 0; iJet < patJets_->size(); ++iJet ) {
    const reco::GenParticleRef genMatchRef( patJets_->at( iJet ).genParticleRef() );
    const int genMatchPdgId( genMatchRef.isNonnull() ? genMatchRef->pdgId() : -6 );
    unsigned iParton = TtSemiLepEvtPartons::LightQ;
    for ( ; iParton <= TtSemiLepEvtPartons::LepB; ++iParton ) {
      if ( ( unsigned )genMatchJetLeptonCombi.at( iParton ) == iJet ) {
        hist2D[ "TqafParton" ]->Fill( getPatMatchPartonBin( genMatchPdgId ), iParton );
        break;
      }
    }
    if ( iParton > TtSemiLepEvtPartons::LepB ) {
      hist2D[ "TqafParton" ]->Fill( getPatMatchPartonBin( genMatchPdgId ), -1. );
    }
  }

  return;

}

void TopHitFitAnalyzer::fill2D_Generator_HitFit( MapTH2D & hist2D, unsigned iCombi )
{

  hist2D[ "DecayChn_nValid" ]->Fill( decayChn_, nHitFitHyposValid_ );

  return;

}

void TopHitFitAnalyzer::fill1D_Generator_HitFitValid( MapTH1D & hist1D, unsigned iCombi )
{

  hist1D[ "DiffTopLepM" ]->Fill( ttSemiLeptonicEvent_->hitFitMT( iCombi ) - ttGenEvent_->leptonicDecayTop()->mass() );
  hist1D[ "DiffLepM" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit )->mass() - ttGenEvent_->singleLepton()->mass() );
  hist1D[ "DiffLepPt" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit )->pt() - ttGenEvent_->singleLepton()->pt() );
  hist1D[ "DiffLepEta" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit )->eta() - ttGenEvent_->singleLepton()->eta() );
  hist1D[ "DiffLepPhi" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit )->phi() - ttGenEvent_->singleLepton()->phi() );
  hist1D[ "DiffLepPz" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit )->pz() - ttGenEvent_->singleLepton()->pz() );
  hist1D[ "DiffNuM" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit )->mass() - ttGenEvent_->singleNeutrino()->mass() );
  hist1D[ "DiffNuPt" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit )->pt() - ttGenEvent_->singleNeutrino()->pt() );
  hist1D[ "DiffNuEta" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit )->eta() - ttGenEvent_->singleNeutrino()->eta() );
  hist1D[ "DiffNuPhi" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit )->phi() - ttGenEvent_->singleNeutrino()->phi() );
  hist1D[ "DiffNuPz" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit )->pz() - ttGenEvent_->singleNeutrino()->pz() );
  hist1D[ "DiffTopHadM" ]->Fill( ttSemiLeptonicEvent_->hitFitMT( iCombi ) - ttGenEvent_->hadronicDecayTop()->mass() );

  return;

}

void TopHitFitAnalyzer::fill2D_Generator_HitFitValid( MapTH2D & hist2D, unsigned iCombi )
{

  hist2D[ "DecayChn_nRealNuSol" ]->Fill( decayChn_, ttSemiLeptonicEvent_->numberOfRealNeutrinoSolutions( TtEvent::kHitFit ) );
  hist2D[ "DecayChn_Prob" ]->Fill( decayChn_, ttSemiLeptonicEvent_->hitFitProb( iCombi ) );
  hist2D[ "DecayChn_ProbLow" ]->Fill( decayChn_, ttSemiLeptonicEvent_->hitFitProb( iCombi ) );
  hist2D[ "DecayChn_ProbLog" ]->Fill( decayChn_, log10( ttSemiLeptonicEvent_->hitFitProb( iCombi ) ) );
  hist2D[ "DecayChn_Chi2" ]->Fill( decayChn_, ttSemiLeptonicEvent_->hitFitChi2( iCombi ) );
  hist2D[ "DecayChn_MT" ]->Fill( decayChn_, ttSemiLeptonicEvent_->hitFitMT( iCombi ) );
  hist2D[ "DecayChn_SigMT" ]->Fill( decayChn_, ttSemiLeptonicEvent_->hitFitSigMT( iCombi ) );
  hist2D[ "DecayChn_SigMTRel" ]->Fill( decayChn_, ttSemiLeptonicEvent_->hitFitMT( iCombi ), ttSemiLeptonicEvent_->hitFitSigMT()/ttSemiLeptonicEvent_->hitFitMT( iCombi ) );

  hist2D[ "DecayChn_TopLepM" ]->Fill( decayChn_, ttSemiLeptonicEvent_->leptonicDecayTop( TtEvent::kHitFit, iCombi )->mass() );
  hist2D[ "DecayChn_LepM" ]->Fill( decayChn_, ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit, iCombi )->mass() );
  hist2D[ "DecayChn_LepPt" ]->Fill( decayChn_, ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit, iCombi )->pt() );
  hist2D[ "DecayChn_LepEta" ]->Fill( decayChn_, ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit, iCombi )->eta() );
  hist2D[ "DecayChn_LepPhi" ]->Fill( decayChn_, ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit, iCombi )->phi() );
  hist2D[ "DecayChn_LepPz" ]->Fill( decayChn_, ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit, iCombi )->pz() );
  hist2D[ "DecayChn_NuM" ]->Fill( decayChn_, ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit, iCombi )->mass() );
  hist2D[ "DecayChn_NuPt" ]->Fill( decayChn_, ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit, iCombi )->pt() );
  hist2D[ "DecayChn_NuEta" ]->Fill( decayChn_, ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit, iCombi )->eta() );
  hist2D[ "DecayChn_NuPhi" ]->Fill( decayChn_, ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit, iCombi )->phi() );
  hist2D[ "DecayChn_NuPz" ]->Fill( decayChn_, ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit, iCombi )->pz() );
  hist2D[ "DecayChn_TopHadM" ]->Fill( decayChn_, ttSemiLeptonicEvent_->hadronicDecayTop( TtEvent::kHitFit )->mass() );

  return;

}

void TopHitFitAnalyzer::fill1D_GenMatch_HitFitValid( MapTH1D & hist1D, unsigned iCombi )
{

  return;

}

void TopHitFitAnalyzer::fill2D_GenMatch_HitFitValid( MapTH2D & hist2D, unsigned iCombi )
{

  hist2D[ "Prob_ProbFound" ]->Fill( ttSemiLeptonicEvent_->hitFitProb(), ttSemiLeptonicEvent_->hitFitProb( hitFitHypoFound_ ) );
  hist2D[ "Chi2_Chi2Found" ]->Fill( ttSemiLeptonicEvent_->hitFitChi2(), ttSemiLeptonicEvent_->hitFitChi2( hitFitHypoFound_ ) );
  hist2D[ "MT_MTFound" ]->Fill( ttSemiLeptonicEvent_->hitFitMT(), ttSemiLeptonicEvent_->hitFitMT( hitFitHypoFound_ ) );
  hist2D[ "ProbFound_MTFound" ]->Fill( ttSemiLeptonicEvent_->hitFitProb( hitFitHypoFound_ ), ttSemiLeptonicEvent_->hitFitMT( hitFitHypoFound_ ) );

  hist2D[ "ProbFound_LepPtFound" ]->Fill( ttSemiLeptonicEvent_->hitFitProb( hitFitHypoFound_ ), ttSemiLeptonicEvent_->singleLepton( TtEvent::kHitFit, hitFitHypoFound_ )->pt() );
  hist2D[ "ProbFound_NuPtFound" ]->Fill( ttSemiLeptonicEvent_->hitFitProb( hitFitHypoFound_ ), ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kHitFit, hitFitHypoFound_ )->pt() );
  hist2D[ "ProbFound_LepPtRecoFound" ]->Fill( ttSemiLeptonicEvent_->hitFitProb( hitFitHypoFound_ ), patMuons_->at( ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kHitFit, hitFitHypoFound_ ).at( TtSemiLepEvtPartons::Lepton ) ).pt() );
  hist2D[ "ProbFound_METRecoFound" ]->Fill( ttSemiLeptonicEvent_->hitFitProb( hitFitHypoFound_ ), patMETs_->at( 0 ).pt() );

  return;

}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( TopHitFitAnalyzer );
