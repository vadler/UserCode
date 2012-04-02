// -*- C++ -*-
//
// Package:    TopMassSemiLeptonic
// Class:      AnalyzeHitFitMCMatching
//
// $Id:$
//
/**
  \class    AnalyzeHitFitMCMatching AnalyzeHitFitMCMatching.cc "TopQuarkAnalysis/TopMassSemiLeptonic/plugins/AnalyzeHitFitMCMatching.cc"
  \brief    Fill histograms and n-tuples for TQAF MC matching studies



  \author   Volker Adler
  \version  $Id:$
*/


#include <vector>
#include <map>
#include <string>

#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "AnalysisDataFormats/TopObjects/interface/TtSemiLeptonicEvent.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"


typedef std::map< std::string, TH2D* > TH2DMap;


class AnalyzeHitFitMCMatching : public edm::EDAnalyzer {

  public:

    /// Constructors and Desctructor

    /// Default constructor
    explicit AnalyzeHitFitMCMatching( const edm::ParameterSet & iConfig );

    /// Destructor
    virtual ~AnalyzeHitFitMCMatching() {};

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
    // PAT leptons
    edm::Handle< pat::MuonCollection > patMuons_;     // direct access needed to deal with alternative kinematics
    edm::Handle< pat::ElectronCollection > patElecs_; // direct access needed to deal with alternative kinematics
    // PAT jets
    edm::Handle< pat::JetCollection > patJets_; // direct access needed to deal with JECs and alternative kinematics

    /// Configuration parameters
    edm::InputTag ttSemiLeptonicEventTag_;
    std::string leptonType_;
    bool useMuons_; // indirect
    bool useElecs_; // indirect
    edm::InputTag patLeptonsTag_;
    edm::InputTag patJetsTag_;

    /// Constants

    /// Data
    unsigned filledEvents_;
    TTree * data_;
    // reconstructed
    Int_t nLeps_;
    Double_t recoLepPt_[ 1 ];
    Double_t recoLepEta_[ 1 ];
    Double_t recoLepPhi_[ 1 ];
    Int_t nJets_;
    Double_t recoJetPt_[ 10 ];
    Double_t recoJetEta_[ 10 ];
    Double_t recoJetPhi_[ 10 ];
    // generated
    Int_t decayChn_;
    Int_t mcLepMatch_;
    Double_t mcLepPt_;
    Double_t mcLepEta_;
    Double_t mcLepPhi_;
    Int_t mcQuarkMatch_;
    Double_t mcQuarkPt_;
    Double_t mcQuarkEta_;
    Double_t mcQuarkPhi_;
    Int_t mcQuarkBarMatch_;
    Double_t mcQuarkBarPt_;
    Double_t mcQuarkBarEta_;
    Double_t mcQuarkBarPhi_;
    Int_t mcBQuarkLepMatch_;
    Double_t mcBQuarkLepPt_;
    Double_t mcBQuarkLepEta_;
    Double_t mcBQuarkLepPhi_;
    Int_t mcBQuarkHadMatch_;
    Double_t mcBQuarkHadPt_;
    Double_t mcBQuarkHadEta_;
    Double_t mcBQuarkHadPhi_;
    Bool_t mcMatchValid_;

    bool isSignal_;

    /// Histograms
    TH2DMap histos_;

    /// Private functions

};


#include <cassert>

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "AnalysisDataFormats/TopObjects/interface/TtSemiLepEvtPartons.h"


// Default constructor
AnalyzeHitFitMCMatching::AnalyzeHitFitMCMatching( const edm::ParameterSet & iConfig )
: ttSemiLeptonicEvent_()
, ttGenEvent_()
, patMuons_()
, patElecs_()
, patJets_()
, ttSemiLeptonicEventTag_( iConfig.getParameter< edm::InputTag >( "ttSemiLeptonicEvent" ) )
, leptonType_( iConfig.getParameter< std::string >( "leptonType" ) )
, useMuons_( leptonType_ == "Muons" )
, useElecs_( leptonType_ == "Electrons" )
, patLeptonsTag_( iConfig.getParameter< edm::InputTag >( "patLeptons" ) )
, patJetsTag_( iConfig.getParameter< edm::InputTag >( "patJets" ) )
{

  // Check configuration

  assert( useMuons_ != useElecs_ );

}


// Begin job
void AnalyzeHitFitMCMatching::beginJob()
{

  edm::Service< TFileService > fileService;

  // N-tuple
  filledEvents_ = 0;
  data_ = fileService->make< TTree >( "data", "Data" );
  // reconstructed
  data_->Branch( "nLeps" ,  &nLeps_  , "nLeps/I" );
  data_->Branch( "recoLepPt" , recoLepPt_ , "recoLepPt[nLeps]/D" );
  data_->Branch( "recoLepEta", recoLepEta_, "recoLepEta[nLeps]/D" );
  data_->Branch( "recoLepPhi", recoLepPhi_, "recoLepPhi[nLeps]/D" );
  data_->Branch( "nJets" ,  &nJets_  , "nJets/I" );
  data_->Branch( "recoJetPt" , recoJetPt_ , "recoJetPt[nJets]/D" );
  data_->Branch( "recoJetEta", recoJetEta_, "recoJetEta[nJets]/D" );
  data_->Branch( "recoJetPhi", recoJetPhi_, "recoJetPhi[nJets]/D" );
  // generated
  data_->Branch( "decayChn", &decayChn_, "decayChn/I" );
  data_->Branch( "mcLepMatch"    , &mcLepMatch_    , "mcLepMatch/I" );
  data_->Branch( "mcLepPt"       , &mcLepPt_       , "mcLepPt/D" );
  data_->Branch( "mcLepEta"      , &mcLepEta_      , "mcLepEta/D" );
  data_->Branch( "mcLepPhi"      , &mcLepPhi_      , "mcLepPhi/D" );
  data_->Branch( "mcQuarkMatch"  , &mcQuarkMatch_  , "mcQuarkMatch/I" );
  data_->Branch( "mcQuarkPt"     , &mcQuarkPt_     , "mcQuarkPt/D" );
  data_->Branch( "mcQuarkEta"    , &mcQuarkEta_    , "mcQuarkEta/D" );
  data_->Branch( "mcQuarkPhi"    , &mcQuarkPhi_    , "mcQuarkPhi/D" );
  data_->Branch( "mcQuarkBarMatch", &mcQuarkBarMatch_  , "mcQuarkBarMatch/I" );
  data_->Branch( "mcQuarkBarPt"  , &mcQuarkBarPt_  , "mcQuarkBarPt/D" );
  data_->Branch( "mcQuarkBarEta" , &mcQuarkBarEta_ , "mcQuarkBarEta/D" );
  data_->Branch( "mcQuarkBarPhi" , &mcQuarkBarPhi_ , "mcQuarkBarPhi/D" );
  data_->Branch( "mcBQuarkLepMatch", &mcBQuarkLepMatch_  , "mcBQuarkLepMatch/I" );
  data_->Branch( "mcBQuarkLepPt" , &mcBQuarkLepPt_ , "mcBQuarkLepPt/D" );
  data_->Branch( "mcBQuarkLepEta", &mcBQuarkLepEta_, "mcBQuarkLepEta/D" );
  data_->Branch( "mcBQuarkLepPhi", &mcBQuarkLepPhi_, "mcBQuarkLepPhi/D" );
  data_->Branch( "mcBQuarkHadMatch", &mcBQuarkHadMatch_  , "mcBQuarkHadMatch/I" );
  data_->Branch( "mcBQuarkHadPt" , &mcBQuarkHadPt_ , "mcBQuarkHadPt/D" );
  data_->Branch( "mcBQuarkHadEta", &mcBQuarkHadEta_, "mcBQuarkHadEta/D" );
  data_->Branch( "mcBQuarkHadPhi", &mcBQuarkHadPhi_, "mcBQuarkHadPhi/D" );
  data_->Branch( "mcMatchValid", &mcMatchValid_, "mcMatchValid/b" );

}


// Event loop
void AnalyzeHitFitMCMatching::analyze( const edm::Event & iEvent, const edm::EventSetup & iSetup )
{

  // Reset
  ttGenEvent_ = 0;
  isSignal_   = false;
  // reconstructed
  nLeps_ = 0;
  for ( unsigned iLep = 0; iLep < 1; ++iLep ) {
    recoLepPt_[ iLep ]  = 0.;
    recoLepEta_[ iLep ] = 0.;
    recoLepPhi_[ iLep ] = 0.;
  }
  nJets_ = 0;
  for ( unsigned iJet = 0; iJet < 10; ++iJet ) {
    recoJetPt_[ iJet ]  = 0.;
    recoJetEta_[ iJet ] = 0.;
    recoJetPhi_[ iJet ] = 0.;
  }
  // generated
  decayChn_ = WDecay::kNone;
  mcLepMatch_        = -1;
  mcLepPt_        = 0.;
  mcLepEta_       = 0.;
  mcLepPhi_       = 0.;
  mcQuarkPt_      = 0.;
  mcQuarkMatch_        = -1;
  mcQuarkEta_     = 0.;
  mcQuarkPhi_     = 0.;
  mcQuarkBarPt_   = 0.;
  mcQuarkBarMatch_        = -1;
  mcQuarkBarEta_  = 0.;
  mcQuarkBarPhi_  = 0.;
  mcBQuarkLepPt_  = 0.;
  mcBQuarkLepMatch_        = -1;
  mcBQuarkLepEta_ = 0.;
  mcBQuarkLepPhi_ = 0.;
  mcBQuarkHadPt_  = 0.;
  mcBQuarkHadMatch_        = -1;
  mcBQuarkHadEta_ = 0.;
  mcBQuarkHadPhi_ = 0.;
  mcMatchValid_ = false;

  // TQAF semi-leptonic event
  iEvent.getByLabel( ttSemiLeptonicEventTag_, ttSemiLeptonicEvent_ );

  // PAT leptons
  if ( useMuons_ ) iEvent.getByLabel( patLeptonsTag_, patMuons_ );
  if ( useElecs_ ) iEvent.getByLabel( patLeptonsTag_, patElecs_ );

  // PAT jets
  iEvent.getByLabel( patJetsTag_, patJets_ );

  // MC
  if ( ! iEvent.isRealData() ) {

    // TQAF MC event
    ttGenEvent_ = const_cast< TtGenEvent* >( ttSemiLeptonicEvent_->genEvent().get() );

    // FIXME: This is still muon-specific
    // Semi-leptonic signal event
    if ( ttGenEvent_->isTtBar() ) {

      // Decay channels
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

      if ( ( ttGenEvent_->isSemiLeptonic( WDecay::kMuon ) && useMuons_ ) ||
           ( ttGenEvent_->isSemiLeptonic( WDecay::kElec ) && useElecs_ ) ) {

        // Fill tree
        // reconstructed
        nLeps_ = patMuons_->size();
        for ( int iLep = 0; iLep < nLeps_; ++iLep ) {
          recoLepPt_[ iLep ]  = patMuons_->at( iLep ).pt();
          recoLepEta_[ iLep ] = patMuons_->at( iLep ).eta();
          recoLepPhi_[ iLep ] = patMuons_->at( iLep ).phi();
        }
        nJets_ = patJets_->size();
        for ( int iJet = 0; iJet < nJets_; ++iJet ) {
          recoJetPt_[ iJet ]  = patJets_->at( iJet ).pt();
          recoJetEta_[ iJet ] = patJets_->at( iJet ).eta();
          recoJetPhi_[ iJet ] = patJets_->at( iJet ).phi();
        }
        // generated
        if ( ttGenEvent_->singleLepton() == 0 ) {
          mcLepMatch_ = -2;
          edm::LogError( "AnalyzeHitFitMCMatching" ) << "Cannot find lepton";
        }
        else {
          if ( TtSemiLepEvtPartons::Lepton < ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ).size() )
            mcLepMatch_ = ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ).at( TtSemiLepEvtPartons::Lepton );
          else
            mcLepMatch_ = -3;
          mcLepPt_  = ttGenEvent_->singleLepton()->pt();
          mcLepEta_ = ttGenEvent_->singleLepton()->eta();
          mcLepPhi_ = ttGenEvent_->singleLepton()->phi();
        }
        if ( ttGenEvent_->hadronicDecayQuark() == 0 ) {
          mcQuarkMatch_ = -2;
          edm::LogError( "AnalyzeHitFitMCMatching" ) << "Cannot find quark";
        }
        else {
          if ( TtSemiLepEvtPartons::LightQ < ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ).size() )
            mcQuarkMatch_ = ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ).at( TtSemiLepEvtPartons::LightQ );
          else
            mcQuarkMatch_ = -3;
          mcQuarkPt_   = ttGenEvent_->hadronicDecayQuark()->pt();
          mcQuarkEta_  = ttGenEvent_->hadronicDecayQuark()->eta();
          mcQuarkPhi_  = ttGenEvent_->hadronicDecayQuark()->phi();
        }
        if ( ttGenEvent_->hadronicDecayQuarkBar() == 0 ) {
          mcQuarkBarMatch_ = -2;
          edm::LogError( "AnalyzeHitFitMCMatching" ) << "Cannot find quarkbar";
        }
        else {
          if ( TtSemiLepEvtPartons::LightQBar < ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ).size() )
            mcQuarkBarMatch_ = ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ).at( TtSemiLepEvtPartons::LightQBar );
          else
            mcQuarkBarMatch_ = -3;
          mcQuarkBarPt_  = ttGenEvent_->hadronicDecayQuarkBar()->pt();
          mcQuarkBarEta_ = ttGenEvent_->hadronicDecayQuarkBar()->eta();
          mcQuarkBarPhi_ = ttGenEvent_->hadronicDecayQuarkBar()->phi();
        }
        if ( ttGenEvent_->leptonicDecayB() == 0 ) {
          mcBQuarkLepMatch_ = -2;
          edm::LogError( "AnalyzeHitFitMCMatching" ) << "Cannot find leptonic b";
        }
        else {
          if ( TtSemiLepEvtPartons::LepB < ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ).size() )
            mcBQuarkLepMatch_ = ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ).at( TtSemiLepEvtPartons::LepB );
          else
            mcBQuarkLepMatch_ = -3;
          mcBQuarkLepPt_  = ttGenEvent_->leptonicDecayB()->pt();
          mcBQuarkLepEta_ = ttGenEvent_->leptonicDecayB()->eta();
          mcBQuarkLepPhi_ = ttGenEvent_->leptonicDecayB()->phi();
        }
        if ( ttGenEvent_->hadronicDecayB() == 0 ) {
          mcBQuarkHadMatch_ = -2;
          edm::LogError( "AnalyzeHitFitMCMatching" ) << "Cannot find hadronic b";
        }
        else {
          if ( TtSemiLepEvtPartons::HadB < ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ).size() )
            mcBQuarkHadMatch_ = ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ).at( TtSemiLepEvtPartons::HadB );
          else
            mcBQuarkHadMatch_ = -3;
          mcBQuarkHadPt_  = ttGenEvent_->hadronicDecayB()->pt();
          mcBQuarkHadEta_ = ttGenEvent_->hadronicDecayB()->eta();
          mcBQuarkHadPhi_ = ttGenEvent_->hadronicDecayB()->phi();
        }

        // Valid full TTbar MC matching
        if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kGenMatch ) ) {

          mcMatchValid_ = true;

        } // Valid full TTbar MC matching

        data_->Fill(); // works only after calling 'fill' for all kinematic properties
        ++filledEvents_;

      } // Semi-leptonic signal event

    } // ttbar event

  } // MC

}


// End job
void AnalyzeHitFitMCMatching::endJob()
{

  edm::LogInfo( "AnalyzeHitFitMCMatching" ) << "Filled events: " << filledEvents_;

}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( AnalyzeHitFitMCMatching );
