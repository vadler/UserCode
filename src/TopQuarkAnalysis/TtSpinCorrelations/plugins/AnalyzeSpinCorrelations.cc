// -*- C++ -*-
//
// Package:    TtSpinCorrelations
// Class:      AnalyzeSpinCorrelations
//
// $Id:$
//
/**
  \class    AnalyzeSpinCorrelations AnalyzeSpinCorrelations.cc "TopQuarkAnalysis/TtSpinCorrelations/plugins/AnalyzeSpinCorrelations.cc"
  \brief



  \author   Volker Adler
  \version  $Id:$
*/


#include <map>
#include <string>

#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TF2.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "AnalysisDataFormats/TopObjects/interface/TtSemiLeptonicEvent.h"


class AnalyzeSpinCorrelations : public edm::EDAnalyzer {

  public:

    /// Constructors and Desctructor

    /// Default constructor
    explicit AnalyzeSpinCorrelations( const edm::ParameterSet & iConfig );

    /// Destructor
    virtual ~AnalyzeSpinCorrelations() {};

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

    /// Configuration parameters
    // TQAF semi-leptonic event
    edm::InputTag ttSemiLeptonicEventTag_;
    // Leptons to use
    bool useMuons_;
    bool useElecs_;
    // Spin basis to use
    bool useHelBasis_;
    bool useBeamBasis_;
    bool useOffDiagBasis_;
    // Histogram settings
    unsigned binsMassTt_;
    double   minMassTt_;
    double   maxMassTt_;
    unsigned binsCos1D_;
    unsigned binsCos2D_;
    // Spin analyzer quantities
    double kappaLplus_;
    double kappaB_;
    double kappaQ_;

    /// Histograms
    // 1-dim
    std::map< std::string, TH1D* > histos1D_;
    std::map< std::string, TH1D* > histos1D_Hel_;
    std::map< std::string, TH1D* > histos1D_Beam_;
    std::map< std::string, TH1D* > histos1D_OffDiag_;
    // 2-dim
    std::map< std::string, TH2D* > histos2D_Hel_;
    std::map< std::string, TH2D* > histos2D_Beam_;
    std::map< std::string, TH2D* > histos2D_OffDiag_;

};


#include "Math/GenVector/VectorUtil.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "AnalysisDataFormats/TopObjects/interface/TtSemiLepEvtPartons.h"


// Default constructor
AnalyzeSpinCorrelations::AnalyzeSpinCorrelations( const edm::ParameterSet & iConfig )
: ttSemiLeptonicEvent_()
, ttGenEvent_()
, ttSemiLeptonicEventTag_( iConfig.getParameter< edm::InputTag >( "ttSemiLeptonicEvent" ) )
, useMuons_( iConfig.getParameter< bool >( "useMuons" ) )
, useElecs_( iConfig.getParameter< bool >( "useElectrons" ) )
, useHelBasis_(iConfig.getParameter< bool >( "useHelicityBasis" ) )
, useBeamBasis_(iConfig.getParameter< bool >( "useBeamBasis" ) )
, useOffDiagBasis_(iConfig.getParameter< bool > ( "useOffDiagonalBasis" ) )
, binsMassTt_(iConfig.getParameter< unsigned >( "binsMassTt" ) )
, minMassTt_(iConfig.getParameter< double >( "minMassTt" ) )
, maxMassTt_(iConfig.getParameter< double >( "maxMassTt" ) )
, binsCos1D_(iConfig.getParameter< unsigned >( "binsCos1D" ) )
, binsCos2D_(iConfig.getParameter< unsigned >( "binsCos2D" ) )
, kappaLplus_(iConfig.getParameter< double >( "kappaLplus" ) )
, kappaB_(iConfig.getParameter< double >( "kappaB" ) )
, kappaQ_(iConfig.getParameter< double >( "kappaQ" ) )
{
}


// Begin job
void AnalyzeSpinCorrelations::beginJob()
{

  edm::Service< TFileService > fileService;

  // tt mass
  histos1D_[ "massTt" ] = fileService->make< TH1D >( "massTt", "t#bar{t} invariant mass", binsMassTt_, minMassTt_, maxMassTt_ );
  histos1D_[ "massTt" ]->SetXTitle( "m_{t#bar{t}} (GeV)" );
  histos1D_[ "massTt" ]->SetYTitle( "events" );
  histos1D_[ "cosLB" ] = fileService->make< TH1D >( "cosLB", "Angle between lepton and b-quark", binsCos1D_, -1., 1. );
  histos1D_[ "cosLB" ]->SetXTitle( "cos #phi_{l,b}" );
  histos1D_[ "cosLB" ]->SetYTitle( "events" );
  histos1D_[ "cosLQ" ] = fileService->make< TH1D >( "cosLQ", "Angle between lepton and low-energy light quark", binsCos1D_, -1., 1. );
  histos1D_[ "cosLQ" ]->SetXTitle( "cos #phi_{l,q}" );
  histos1D_[ "cosLQ" ]->SetYTitle( "events" );

  if ( useHelBasis_ ) {
    TFileDirectory dirHel( fileService->mkdir( "helicityBasis", "Helicity basis" ) );
    histos1D_Hel_[ "cosTL" ] = dirHel.make< TH1D >( "cosLT_Hel", "Helicity basis: pseudo-angle between t-quark and lepton", binsCos1D_, -1., 1. );
    histos1D_Hel_[ "cosTL" ]->SetXTitle( "cos #theta_{t,l}" );
    histos1D_Hel_[ "cosTL" ]->SetYTitle( "events" );
    histos1D_Hel_[ "cosTB" ] = dirHel.make< TH1D >( "cosLB_Hel", "Helicity basis: pseudo-angle between t-quark and b-quark", binsCos1D_, -1., 1. );
    histos1D_Hel_[ "cosTB" ]->SetXTitle( "cos #theta_{t,b}" );
    histos1D_Hel_[ "cosTB" ]->SetYTitle( "events" );
    histos1D_Hel_[ "cosTQ" ] = dirHel.make< TH1D >( "cosLQ_Hel", "Helicity basis: pseudo-angle between t-quark and low-energy light quark", binsCos1D_, -1., 1. );
    histos1D_Hel_[ "cosTQ" ]->SetXTitle( "cos #theta_{t,q}" );
    histos1D_Hel_[ "cosTQ" ]->SetYTitle( "events" );
    histos2D_Hel_[ "cosTBCosTL" ] = dirHel.make< TH2D >( "cosTBCosTL_Hel", "Helicity basis: pseudo-angles between t-quark and lepton/b-quark", binsCos2D_, -1., 1., binsCos2D_, -1., 1. );
    histos2D_Hel_[ "cosTBCosTL" ]->SetXTitle( "cos #theta_{t,l}" );
    histos2D_Hel_[ "cosTBCosTL" ]->SetYTitle( "cos #theta_{t,b}" );
    histos2D_Hel_[ "cosTBCosTL" ]->SetZTitle( "events" );
    histos2D_Hel_[ "cosTQCosTL" ] = dirHel.make< TH2D >( "cosTQCosTL_Hel", "Helicity basis: pseudo-angles between t-quark and lepton/low-energy quark", binsCos2D_, -1., 1., binsCos2D_, -1., 1. );
    histos2D_Hel_[ "cosTQCosTL" ]->SetXTitle( "cos #theta_{t,l}" );
    histos2D_Hel_[ "cosTQCosTL" ]->SetYTitle( "cos #theta_{t,q}" );
    histos2D_Hel_[ "cosTQCosTL" ]->SetZTitle( "events" );
  }

  if ( useBeamBasis_ ) {
    TFileDirectory dirBeam( fileService->mkdir( "beamBasis", "Beam basis" ) );
    histos1D_Beam_[ "cosTL" ] = dirBeam.make< TH1D >( "cosLT_Beam", "Beam basis: pseudo-angle between t-quark and lepton", binsCos1D_, -1., 1. );
    histos1D_Beam_[ "cosTL" ]->SetXTitle( "cos #theta_{t,l}" );
    histos1D_Beam_[ "cosTL" ]->SetYTitle( "events" );
    histos1D_Beam_[ "cosTB" ] = dirBeam.make< TH1D >( "cosLB_Beam", "Beam basis: pseudo-angle between t-quark and b-quark", binsCos1D_, -1., 1. );
    histos1D_Beam_[ "cosTB" ]->SetXTitle( "cos #theta_{t,b}" );
    histos1D_Beam_[ "cosTB" ]->SetYTitle( "events" );
    histos1D_Beam_[ "cosTQ" ] = dirBeam.make< TH1D >( "cosLQ_Beam", "Beam basis: pseudo-angle between t-quark and low-energy light quark", binsCos1D_, -1., 1. );
    histos1D_Beam_[ "cosTQ" ]->SetXTitle( "cos #theta_{t,q}" );
    histos1D_Beam_[ "cosTQ" ]->SetYTitle( "events" );
    histos2D_Beam_[ "cosTBCosTL" ] = dirBeam.make< TH2D >( "cosTBCosTL_Beam", "Beam basis: pseudo-angles between t-quark and lepton/b-quark", binsCos2D_, -1., 1., binsCos2D_, -1., 1. );
    histos2D_Beam_[ "cosTBCosTL" ]->SetXTitle( "cos #theta_{t,l}" );
    histos2D_Beam_[ "cosTBCosTL" ]->SetYTitle( "cos #theta_{t,b}" );
    histos2D_Beam_[ "cosTBCosTL" ]->SetZTitle( "events" );
    histos2D_Beam_[ "cosTQCosTL" ] = dirBeam.make< TH2D >( "cosTQCosTL_Beam", "Beam basis: pseudo-angles between t-quark and lepton/low-energy quark", binsCos2D_, -1., 1., binsCos2D_, -1., 1. );
    histos2D_Beam_[ "cosTQCosTL" ]->SetXTitle( "cos #theta_{t,l}" );
    histos2D_Beam_[ "cosTQCosTL" ]->SetYTitle( "cos #theta_{t,q}" );
    histos2D_Beam_[ "cosTQCosTL" ]->SetZTitle( "events" );
  }

  if ( useOffDiagBasis_ ) {
    TFileDirectory dirOffDiag( fileService->mkdir( "offDiagBasis", "Off-diagonal basis" ) );
    histos1D_OffDiag_[ "cosTL" ] = dirOffDiag.make< TH1D >( "cosLT_OffDiag", "Off-diagonal basis: pseudo-angle between t-quark and lepton", binsCos1D_, -1., 1. );
    histos1D_OffDiag_[ "cosTL" ]->SetXTitle( "cos #theta_{t,l}" );
    histos1D_OffDiag_[ "cosTL" ]->SetYTitle( "events" );
    histos1D_OffDiag_[ "cosTB" ] = dirOffDiag.make< TH1D >( "cosLB_OffDiag", "Off-diagonal basis: pseudo-angle between t-quark and b-quark", binsCos1D_, -1., 1. );
    histos1D_OffDiag_[ "cosTB" ]->SetXTitle( "cos #theta_{t,b}" );
    histos1D_OffDiag_[ "cosTB" ]->SetYTitle( "events" );
    histos1D_OffDiag_[ "cosTQ" ] = dirOffDiag.make< TH1D >( "cosLQ_OffDiag", "Off-diagonal basis: pseudo-angle between t-quark and low-energy light quark", binsCos1D_, -1., 1. );
    histos1D_OffDiag_[ "cosTQ" ]->SetXTitle( "cos #theta_{t,q}" );
    histos1D_OffDiag_[ "cosTQ" ]->SetYTitle( "events" );
    histos2D_OffDiag_[ "cosTBCosTL" ] = dirOffDiag.make< TH2D >( "cosTBCosTL_OffDiag", "Off-diagonal basis: pseudo-angles between t-quark and lepton/b-quark", binsCos2D_, -1., 1., binsCos2D_, -1., 1. );
    histos2D_OffDiag_[ "cosTBCosTL" ]->SetXTitle( "cos #theta_{t,l}" );
    histos2D_OffDiag_[ "cosTBCosTL" ]->SetYTitle( "cos #theta_{t,b}" );
    histos2D_OffDiag_[ "cosTBCosTL" ]->SetZTitle( "events" );
    histos2D_OffDiag_[ "cosTQCosTL" ] = dirOffDiag.make< TH2D >( "cosTQCosTL_OffDiag", "Off-diagonal basis: pseudo-angles between t-quark and lepton/low-energy quark", binsCos2D_, -1., 1., binsCos2D_, -1., 1. );
    histos2D_OffDiag_[ "cosTQCosTL" ]->SetXTitle( "cos #theta_{t,l}" );
    histos2D_OffDiag_[ "cosTQCosTL" ]->SetYTitle( "cos #theta_{t,q}" );
    histos2D_OffDiag_[ "cosTQCosTL" ]->SetZTitle( "events" );
  }

}


// Event loop
void AnalyzeSpinCorrelations::analyze( const edm::Event & iEvent, const edm::EventSetup & iSetup )
{

  // TQAF semi-leptonic event
  iEvent.getByLabel( ttSemiLeptonicEventTag_, ttSemiLeptonicEvent_ );

  // MC
  if ( ! iEvent.isRealData() ) {

    // TQAF MC event
    ttGenEvent_ = const_cast< TtGenEvent* >( ttSemiLeptonicEvent_->genEvent().get() );

    // Semi-leptonic signal event
    if ( ttGenEvent_->isTtBar() &&
         ( ( ttGenEvent_->isSemiLeptonic( WDecay::kMuon ) && useMuons_ ) ||
           ( ttGenEvent_->isSemiLeptonic( WDecay::kElec ) && useElecs_ ) ) ) {

      // build CM sytem
      reco::Particle::LorentzVector topPairCmf( ttGenEvent_->top()->p4() + ttGenEvent_->topBar()->p4() );

      // boost particle 4-vectors to tt Cmf
      reco::Particle::LorentzVector tLeptonicCmf( ROOT::Math::VectorUtil::boost( ttGenEvent_->leptonicDecayTop()->p4(), topPairCmf.BoostToCM() ) );
      reco::Particle::LorentzVector tHadronicCmf( ROOT::Math::VectorUtil::boost( ttGenEvent_->hadronicDecayTop()->p4(), topPairCmf.BoostToCM() ) );
      reco::Particle::LorentzVector lLeptonicCmf( ROOT::Math::VectorUtil::boost( ttGenEvent_->singleLepton()->p4(), topPairCmf.BoostToCM() ) );
      reco::Particle::LorentzVector bHadronicCmf( ROOT::Math::VectorUtil::boost( ttGenEvent_->hadronicDecayB()->p4(), topPairCmf.BoostToCM() ) );
      reco::Particle::LorentzVector q1HadronicCmf( ROOT::Math::VectorUtil::boost( ttGenEvent_->hadronicDecayQuark()->p4(), topPairCmf.BoostToCM() ) );
      reco::Particle::LorentzVector q2HadronicCmf( ROOT::Math::VectorUtil::boost( ttGenEvent_->hadronicDecayQuarkBar()->p4(), topPairCmf.BoostToCM() ) );
      reco::Particle::LorentzVector beamCmf(ROOT::Math::VectorUtil::boost( ( ttGenEvent_->initialPartons() )[0].p4(), topPairCmf.BoostToCM() ) );

      // build spin basis unit vectors
      reco::Particle::Vector leptHelCmf( tLeptonicCmf.Vect().Unit() );
      reco::Particle::Vector hadrHelCmf( tHadronicCmf.Vect().Unit() ); // = -leptHelCmf
      reco::Particle::Vector beamBeamCmf( beamCmf.Vect().Unit() );
      reco::Particle::Vector offDiagCmf( ( ( -beamBeamCmf+( 1.-topPairCmf.Gamma() ) * ( beamBeamCmf.Dot( leptHelCmf ) ) * leptHelCmf ) / ( std::sqrt( 1. - std::pow( beamBeamCmf.Dot( leptHelCmf ), 2. ) * ( 1. - std::pow( topPairCmf.Gamma(), 2. ) ) ) ) ).Unit() );                                                                                                                            // FIXME: use initial hadron instead of initial parton

      // boost 4-vectors to t(bar) rest frames
      reco::Particle::LorentzVector lLeptonicTRest( ROOT::Math::VectorUtil::boost( lLeptonicCmf, tLeptonicCmf.BoostToCM() ) );
      reco::Particle::LorentzVector bHadronicTRest( ROOT::Math::VectorUtil::boost( bHadronicCmf, tHadronicCmf.BoostToCM() ) );
      reco::Particle::LorentzVector q1HadronicTRest( ROOT::Math::VectorUtil::boost( q1HadronicCmf, tHadronicCmf.BoostToCM() ) );
      reco::Particle::LorentzVector q2HadronicTRest( ROOT::Math::VectorUtil::boost( q2HadronicCmf, tHadronicCmf.BoostToCM() ) );
      reco::Particle::LorentzVector qHadronicTRest( q1HadronicTRest.energy() < q2HadronicTRest.energy() ? q1HadronicTRest : q2HadronicTRest );

      // extract particle directions in t(bar) rest frames
      reco::Particle::Vector lDirectionTRest( lLeptonicTRest.Vect().Unit() );
      reco::Particle::Vector bDirectionTRest( bHadronicTRest.Vect().Unit() );
      reco::Particle::Vector qDirectionTRest( qHadronicTRest.Vect().Unit() );

      // fill histograms

      double cosPhiLB( lDirectionTRest.Dot( bDirectionTRest ) );
      double cosPhiLQ( lDirectionTRest.Dot( qDirectionTRest ) );
      histos1D_[ "massTt" ]->Fill( topPairCmf.mass() );
      histos1D_[ "cosLB" ]->Fill( cosPhiLB );
      histos1D_[ "cosLQ" ]->Fill( cosPhiLQ );

      if ( useHelBasis_ ) {
        double cosThetaTL_Hel( leptHelCmf.Dot( lDirectionTRest ) );
        double cosThetaTB_Hel( hadrHelCmf.Dot( bDirectionTRest ) );
        double cosThetaTQ_Hel( hadrHelCmf.Dot( qDirectionTRest ) );
        histos1D_Hel_[ "cosTL" ]->Fill( cosThetaTL_Hel );
        histos1D_Hel_[ "cosTB" ]->Fill( cosThetaTB_Hel );
        histos1D_Hel_[ "cosTQ" ]->Fill( cosThetaTQ_Hel );
        histos2D_Hel_[ "cosTBCosTL" ]->Fill( cosThetaTB_Hel, cosThetaTL_Hel );
        histos2D_Hel_[ "cosTQCosTL" ]->Fill( cosThetaTQ_Hel, cosThetaTL_Hel );
      }

      if ( useBeamBasis_ ) {
        double cosThetaTL_Beam( beamBeamCmf.Dot( lDirectionTRest ) );
        double cosThetaTB_Beam( beamBeamCmf.Dot( bDirectionTRest ) );
        double cosThetaTQ_Beam( beamBeamCmf.Dot( qDirectionTRest ) );
        histos1D_Beam_[ "cosTL" ]->Fill( cosThetaTL_Beam );
        histos1D_Beam_[ "cosTB" ]->Fill( cosThetaTB_Beam );
        histos1D_Beam_[ "cosTQ" ]->Fill( cosThetaTQ_Beam );
        histos2D_Beam_[ "cosTBCosTL" ]->Fill( cosThetaTB_Beam, cosThetaTL_Beam );
        histos2D_Beam_[ "cosTQCosTL" ]->Fill( cosThetaTQ_Beam, cosThetaTL_Beam );
      }

      if ( useOffDiagBasis_ ) {
        double cosThetaTL_OffDiag( offDiagCmf.Dot( lDirectionTRest ) );
        double cosThetaTB_OffDiag( offDiagCmf.Dot( bDirectionTRest ) );
        double cosThetaTQ_OffDiag( offDiagCmf.Dot( qDirectionTRest ) );
        histos1D_OffDiag_[ "cosTL" ]->Fill( cosThetaTL_OffDiag );
        histos1D_OffDiag_[ "cosTB" ]->Fill( cosThetaTB_OffDiag );
        histos1D_OffDiag_[ "cosTQ" ]->Fill( cosThetaTQ_OffDiag );
        histos2D_OffDiag_[ "cosTBCosTL" ]->Fill( cosThetaTB_OffDiag, cosThetaTL_OffDiag );
        histos2D_OffDiag_[ "cosTQCosTL" ]->Fill( cosThetaTQ_OffDiag, cosThetaTL_OffDiag );
      }

    } // Semi-leptonic signal event

  } // MC

}


// End job
void AnalyzeSpinCorrelations::endJob()
{

  // fit histograms

  TF1 fit1L( "fit1L_", "[0]*(1.+[1]*[2]*x)" );
  fit1L.SetParameter( 2, kappaLplus_ );
  fit1L.FixParameter( 2, kappaLplus_ );
  TF1 fit1B( "fit1B_", "[0]*(1.+[1]*[2]*x)" );
  fit1B.SetParameter( 2,kappaB_ );
  fit1B.FixParameter( 2,kappaB_ );
  TF1 fit1Q( "fit1Q_", "[0]*(1.+[1]*[2]*x)" );
  fit1Q.SetParameter( 2,kappaQ_ );
  fit1Q.FixParameter( 2,kappaQ_ );
  TF2 fit2LB( "fit2LB_", "[0]*(1.-[1]*[2]*[3]*x*y)" );
  fit2LB.SetParameter( 2,kappaLplus_ );
  fit2LB.FixParameter( 2,kappaLplus_ );
  fit2LB.SetParameter( 3,kappaB_ );
  fit2LB.FixParameter( 3,kappaB_ );
  TF2 fit2LQ( "fit2LQ_", "[0]*(1.-[1]*[2]*[3]*x*y)" );
  fit2LQ.SetParameter( 2,kappaLplus_ );
  fit2LQ.FixParameter( 2,kappaLplus_ );
  fit2LQ.SetParameter( 3,kappaQ_ );
  fit2LQ.FixParameter( 3,kappaQ_ );

  if ( useHelBasis_ ) {
    histos1D_Hel_[ "cosTL" ]->Fit( &fit1L );
    histos1D_Hel_[ "cosTB" ]->Fit( &fit1B );
    histos1D_Hel_[ "cosTQ" ]->Fit( &fit1Q );
    histos2D_Hel_[ "cosTBCosTL" ]->Fit( &fit2LB );
    histos2D_Hel_[ "cosTQCosTL" ]->Fit( &fit2LQ );
  }

  if ( useBeamBasis_ ) {
    histos1D_Beam_[ "cosTL" ]->Fit( &fit1L );
    histos1D_Beam_[ "cosTB" ]->Fit( &fit1B );
    histos1D_Beam_[ "cosTQ" ]->Fit( &fit1Q );
    histos2D_Beam_[ "cosTBCosTL" ]->Fit( &fit2LB );
    histos2D_Beam_[ "cosTQCosTL" ]->Fit( &fit2LQ );
  }

  if ( useOffDiagBasis_ ) {
    histos1D_OffDiag_[ "cosTL" ]->Fit( &fit1L );
    histos1D_OffDiag_[ "cosTB" ]->Fit( &fit1B );
    histos1D_OffDiag_[ "cosTQ" ]->Fit( &fit1Q );
    histos2D_OffDiag_[ "cosTBCosTL" ]->Fit( &fit2LB );
    histos2D_OffDiag_[ "cosTQCosTL" ]->Fit( &fit2LQ );
  }

}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( AnalyzeSpinCorrelations );
