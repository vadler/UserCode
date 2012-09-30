// -*- C++ -*-
//
// Package:    TopMassSemiLeptonic
// Class:      AnalyzeHitFit
//
// $Id: AnalyzeHitFit.cc,v 1.1 2011/08/31 14:45:53 vadler Exp $
//
/**
  \class    AnalyzeHitFit AnalyzeHitFit.cc "TopQuarkAnalysis/TopMassSemiLeptonic/plugins/AnalyzeHitFit.cc"
  \brief    Fill histograms and n-tuples for HitFit resolution function determination



  \author   Volker Adler
  \version  $Id: AnalyzeHitFit.cc,v 1.1 2011/08/31 14:45:53 vadler Exp $
*/


#include <vector>
#include <map>
#include <string>

#include "TH1D.h"
#include "TTree.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"

#include "AnalysisDataFormats/TopObjects/interface/TtSemiLeptonicEvent.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"

#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"


class AnalyzeHitFit : public edm::EDAnalyzer {

  public:

    /// Constructors and Desctructor

    /// Default constructor
    explicit AnalyzeHitFit( const edm::ParameterSet & iConfig );

    /// Destructor
    virtual ~AnalyzeHitFit() {};

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
    edm::Handle< TtSemiLeptonicEvent > ttSemiLeptonicEventMuons_;
    edm::Handle< TtSemiLeptonicEvent > ttSemiLeptonicEventElecs_;
    // TQAF MC event
    edm::Handle< TtGenEvent > ttGenEvent_;
    // PAT leptons
    edm::Handle< pat::MuonCollection > patMuons_;     // direct access needed to deal with alternative kinematics
    edm::Handle< pat::ElectronCollection > patElecs_; // direct access needed to deal with alternative kinematics
    // PAT jets
    edm::Handle< pat::JetCollection > patJets_;       // direct access needed to deal with JECs and alternative kinematics
    // PAT METs
    edm::Handle< pat::METCollection > patMETs_;       // direct access needed to deal with JECs and alternative kinematics
    // Pile-up
    edm::Handle< std::vector< PileupSummaryInfo > > pileUp_;
    // Pile-up input files
    edm::FileInPath pileUpFileMCTrue_;
    edm::FileInPath pileUpFileDataTrue_;
    edm::FileInPath pileUpFileMCObserved_;
    edm::FileInPath pileUpFileDataObserved_;
    // Lumi weights
    edm::LumiReWeighting lumiWeightTrue_;
    edm::LumiReWeighting lumiWeightObserved_;

    /// Configuration parameters
    edm::InputTag ttSemiLeptonicEventMuonsTag_;
    edm::InputTag ttSemiLeptonicEventElecsTag_;
    edm::InputTag ttGenEventTag_;
    edm::InputTag patMuonsTag_;
    edm::InputTag patElecsTag_;
    edm::InputTag patJetsTag_;
    edm::InputTag patMETsTag_;
    std::string jecLevel_;
    std::string pathPlots_;
    bool        plot_;
    // Eta binning
    std::vector< std::vector< double > > etaBins_;
    std::vector< std::vector< double > > etaSymmBins_;
    // P_t binning
    std::vector< std::vector< double > > ptBins_;

    /// Constants
    // Object categories
    std::vector< std::string > objCats_;
    // Kinematic properties
    std::vector< std::string > kinProps_;

    /// Data
    unsigned filledEvents_;
    TTree * data_;                   // event data
    // pile-up
    Float_t  nPVTrue_;               // mean number of primery vertices' distribution
    Int_t    nPVObserved_;           // observed number of primery vertices
    Double_t pileUpWeightTrue_;      // MC weight for true pile-up
    Double_t pileUpWeightObserved_;  // MC weight for observed pile-up
    std::vector< TTree * > catData_; // data per object category
    // reconstructed
    Double_t pt_;                    // used momentum term, can be: E, p, E_t, p_t
    Double_t eta_;
    Double_t phi_;
    Int_t    binEta_;                // eta bin number as determined by 'getEtaBin'
    Int_t    binEtaSymm_;            // symmetrised eta bin number as determined by 'getEtaBin'
    // reconstructed alternative
    Double_t ptAlt_;                 // used momentum term, can be: E, p, E_t, p_t
    Double_t etaAlt_;
    Double_t phiAlt_;
    Int_t    binEtaAlt_;             // eta bin number as determined by 'getEtaBin'
    Int_t    binEtaSymmAlt_;         // symmetrised eta bin number as determined by 'getEtaBin'
    // recostructed from generated
    Double_t ptGenJet_;              // used momentum term, can be: E, p, E_t, p_t
    Double_t etaGenJet_;
    Double_t phiGenJet_;
    Int_t    binEtaGenJet_;          // eta bin number as determined by 'getEtaBin'
    Int_t    binEtaSymmGenJet_;      // symmetrised eta bin number as determined by 'getEtaBin'
    // recostructed alternative from generated
    Double_t ptGenJetAlt_;           // used momentum term, can be: E, p, E_t, p_t
    Double_t etaGenJetAlt_;
    Double_t phiGenJetAlt_;
    Int_t    binEtaGenJetAlt_;       // eta bin number as determined by 'getEtaBin'
    Int_t    binEtaSymmGenJetAlt_;   // symmetrised eta bin number as determined by 'getEtaBin'
    // generated
    Double_t ptGen_;                 // used momentum term, can be: E, p, E_t, p_t
    Double_t etaGen_;
    Double_t phiGen_;
    Int_t    binEtaGen_;             // eta bin number as determined by 'getEtaBin'
    Int_t    binEtaSymmGen_;         // symmetrised eta bin number as determined by 'getEtaBin'

    /// Histograms
    // Pile-up weights
    TH1D * histo_pileUpWeightTrue_;
    TH1D * histo_pileUpWeightObserved_;
    // Binning
    std::vector< TH1D * > histos_EtaBins_;
    std::vector< TH1D * > histos_PtBins_;

    /// Private functions

    unsigned getEtaBin( unsigned iCat, double eta, bool symm = false );

    // Fill n-tuples
    void fill();
    void fill( unsigned iCat, const edm::Handle< TtSemiLeptonicEvent > & ttSemiLeptonicEvent, bool repeat = false );

};


#include <cassert>
#include <iostream>
#include <sstream>

#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string/replace.hpp"
#include <boost/shared_ptr.hpp>

#include <TROOT.h>
#include <TSystem.h>
#include "TStyle.h"
#include "TCanvas.h"

#include "Math/GenVector/VectorUtil.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "AnalysisDataFormats/TopObjects/interface/TtSemiLepEvtPartons.h"
#include "TopQuarkAnalysis/TopHitFit/interface/EtaDepResolution.h"

#include "CommonTools/MyTools/interface/RootTools.h"


// Default constructor
AnalyzeHitFit::AnalyzeHitFit( const edm::ParameterSet & iConfig )
: ttSemiLeptonicEventMuons_()
, ttSemiLeptonicEventElecs_()
, ttGenEvent_()
, patMuons_()
, patElecs_()
, patJets_()
, patMETs_()
, pileUp_()
, pileUpFileMCTrue_( iConfig.getParameter< edm::FileInPath >( "pileUpFileMCTrue" ) )
, pileUpFileDataTrue_( iConfig.getParameter< edm::FileInPath >( "pileUpFileDataTrue" ) )
, pileUpFileMCObserved_( iConfig.getParameter< edm::FileInPath >( "pileUpFileMCObserved" ) )
, pileUpFileDataObserved_( iConfig.getParameter< edm::FileInPath >( "pileUpFileDataObserved" ) )
, ttSemiLeptonicEventMuonsTag_( iConfig.getParameter< edm::InputTag >( "ttSemiLeptonicEventMuons" ) )
, ttSemiLeptonicEventElecsTag_( iConfig.getParameter< edm::InputTag >( "ttSemiLeptonicEventElectrons" ) )
, ttGenEventTag_( iConfig.getParameter< edm::InputTag >( "ttGenEvent" ) )
, patMuonsTag_( iConfig.getParameter< edm::InputTag >( "patMuons" ) )
, patElecsTag_( iConfig.getParameter< edm::InputTag >( "patElectrons" ) )
, patJetsTag_( iConfig.getParameter< edm::InputTag >( "patJets" ) )
, patMETsTag_( iConfig.getParameter< edm::InputTag >( "patMETs" ) )
, jecLevel_( iConfig.getParameter< std::string >( "jecLevel" ) )
, pathPlots_( iConfig.getParameter< std::string >( "pathPlots" ) )
, plot_( ! pathPlots_.empty() )
, filledEvents_( 0 )
{

  lumiWeightTrue_     = edm::LumiReWeighting( pileUpFileMCTrue_.fullPath()    , pileUpFileDataTrue_.fullPath()    , "pileup", "pileup" );
  lumiWeightObserved_ = edm::LumiReWeighting( pileUpFileMCObserved_.fullPath(), pileUpFileDataObserved_.fullPath(), "pileup", "pileup" );

  // Constants

  objCats_.clear();
  objCats_.push_back( "Mu" );
  objCats_.push_back( "Elec" );
  objCats_.push_back( "UdscJet" );
  objCats_.push_back( "BJet" );
  objCats_.push_back( "MET" );

  std::vector< std::string > objNames;
  objNames.push_back( "muon" );
  objNames.push_back( "electron" );
  objNames.push_back( "udscJet" );
  objNames.push_back( "bJet" );
  objNames.push_back( "met" );

  kinProps_.clear();
  kinProps_.push_back( "Pt" );
  kinProps_.push_back( "Eta" );
  kinProps_.push_back( "Phi" );

  std::vector< std::string > fileNameParams;
  std::vector< std::string > etaBinParams;
  std::vector< std::string > ptBinParams;
  for ( unsigned iObj = 0; iObj < objNames.size(); ++iObj ) {
    fileNameParams.push_back( objNames.at( iObj ) + "Resolutions" );
    etaBinParams.push_back( objNames.at( iObj ) + "EtaBins" );
    ptBinParams.push_back( objNames.at( iObj ) + "PtBins" );
  }

  for ( unsigned iCat = 0; iCat < objCats_.size(); ++iCat ) {

    if ( iConfig.getParameter< std::vector< double > >( etaBinParams.at( iCat ) ).size() == 0 ) {
      std::vector< double > etaBins;
      if ( objCats_.at( iCat ) == "MET" ) {
        etaBins.push_back( -999. );
        etaBins.push_back(  999. );
      }
      else {
        hitfit::EtaDepResolution hitFitRes( edm::FileInPath( iConfig.getParameter< std::string >( fileNameParams.at( iCat ) ) ).fullPath() );
        for ( unsigned iEta = 0; iEta < hitFitRes.GetEtaDepResElement().size(); ++iEta ) {
          etaBins.push_back( hitFitRes.GetEtaDepResElement().at( iEta ).EtaMin() );
        }
        etaBins.push_back( hitFitRes.GetEtaDepResElement().back().EtaMax() );
      }
      etaBins_.push_back( etaBins );
    }
    else {
      etaBins_.push_back( iConfig.getParameter< std::vector< double > >( etaBinParams.at( iCat ) ) );
    }

    std::vector< double > etaSymmBins;
    for ( unsigned iEta = 0; iEta < etaBins_.back().size() - 1; ++iEta ) {
      if ( etaBins_.back().at( iEta + 1 ) <= 0. ) continue;
      if ( etaBins_.back().at( iEta ) < 0. )
        etaSymmBins.push_back( 0. );
      else
        etaSymmBins.push_back( etaBins_.back().at( iEta ) );
    }
    if ( etaBins_.back().size() > 0 && etaBins_.back().back() > 0. ) {
      etaSymmBins.push_back( etaBins_.back().back() );
    }
    etaSymmBins_.push_back( etaSymmBins );

    ptBins_.push_back( iConfig.getParameter< std::vector< double > >( ptBinParams.at( iCat ) ) );

  }

  // Logging output
  std::stringstream sstrEta;
  std::stringstream sstrPt;
  for ( unsigned iCat = 0; iCat < objCats_.size(); ++iCat ) {
    if ( etaBins_.at( iCat ).size() < 2 )
      edm::LogError( "AnalyzeHitFit" ) << objCats_.at( iCat ) << ": less than 2 eta bin bounderies found";
    sstrEta << "  " << objCats_.at( iCat ) << ": ";
    for ( unsigned iEta = 0; iEta < etaBins_.at( iCat ).size() - 1; ++iEta ) {
      sstrEta << etaBins_.at( iCat ).at( iEta ) << ", ";
    }
    sstrEta << etaBins_.at( iCat ).back() << std::endl;
    if ( ptBins_.at( iCat ).size() < 2 )
      edm::LogError( "AnalyzeHitFit" ) << objCats_.at( iCat ) << ": less than 2 p_t bin bounderies found";
    std::string cat( objCats_.at( iCat ) );
    sstrPt << "  " << cat << ": ";
    for ( unsigned iPt = 0; iPt < ptBins_.at( iCat ).size() - 1; ++iPt ) {
      sstrPt << ptBins_.at( iCat ).at( iPt ) << ", ";
    }
    sstrPt << ptBins_.at( iCat ).back() << std::endl;
  }
  edm::LogInfo( "AnalyzeHitFit" ) << "Eta binning\n" << sstrEta.str();
  edm::LogInfo( "AnalyzeHitFit" ) << "Pt binning\n" << sstrPt.str();

}


// Begin job
void AnalyzeHitFit::beginJob()
{

  boost::shared_ptr< TFile > fileMCTrue_( new TFile( pileUpFileMCTrue_.fullPath().c_str() ) );
  boost::shared_ptr< TH1 >   histoMCTrue_( ( static_cast< TH1 * >( fileMCTrue_->Get( "pileup" )->Clone() ) ) );
  const int nBinsMCTrue( histoMCTrue_->GetNbinsX() );
  boost::shared_ptr< TFile > fileMCObserved_( new TFile( pileUpFileMCObserved_.fullPath().c_str() ) );
  boost::shared_ptr< TH1 >   histoMCObserved_( ( static_cast< TH1 * >( fileMCObserved_->Get( "pileup" )->Clone() ) ) );
  const int nBinsMCObserved( histoMCObserved_->GetNbinsX() );

  edm::Service< TFileService > fileService;

  data_ = fileService->make< TTree >( "Data", "Event data" );
  data_->Branch( "NPVTrue"             , &nPVTrue_             , "nPVTrue/F" );
  data_->Branch( "NPVObserved"         , &nPVObserved_         , "nPVObserved/I" );
  data_->Branch( "PileUpWeightTrue"    , &pileUpWeightTrue_    , "pileUpWeightTrue/D" );
  data_->Branch( "PileUpWeightObserved", &pileUpWeightObserved_, "pileUpWeightObserved/D" );

  histo_pileUpWeightTrue_ = fileService->make< TH1D >( "pileUpWeightTrue", "True pile-up weights", nBinsMCTrue, 0., double( nBinsMCTrue ) );
  histo_pileUpWeightTrue_->SetXTitle( "number of true interactions" );
  histo_pileUpWeightTrue_->SetYTitle( "weight (#frac{data}{MC})" );
  for ( int iBin = 1; iBin < nBinsMCTrue + 1; ++iBin ) {
    histo_pileUpWeightTrue_->Fill( iBin, lumiWeightTrue_.weight( iBin ) );
  }
  histo_pileUpWeightObserved_ = fileService->make< TH1D >( "pileUpWeightObserved", "Observed pile-up weights", nBinsMCObserved, 0., double( nBinsMCObserved ) );
  histo_pileUpWeightObserved_->SetXTitle( "number of observed interactions" );
  histo_pileUpWeightObserved_->SetYTitle( "weight (#frac{data}{MC})" );
  for ( int iBin = 1; iBin < nBinsMCObserved + 1; ++iBin ) {
    histo_pileUpWeightObserved_->Fill( iBin, lumiWeightObserved_.weight( iBin ) );
  }

  for ( unsigned iCat = 0; iCat < objCats_.size(); ++iCat ) {
    const std::string cat( objCats_.at( iCat ) );
    TFileDirectory dir( fileService->mkdir( cat.c_str(), "" ) );

    // Eta binning
    histos_EtaBins_.push_back( dir.make< TH1D >( std::string( cat + "_binsEta" ).c_str(), cat.c_str(), etaBins_.at( iCat ).size() - 1, etaBins_.at( iCat ).data() ) );
    histos_EtaBins_.back()->SetXTitle( "#eta" );
    histos_EtaBins_.back()->SetYTitle( "bin" );
    histos_EtaBins_.back()->GetYaxis()->SetRangeUser( -1., etaBins_.at( iCat ).size() );
    for ( unsigned iEta = 0; iEta < etaBins_.at( iCat ).size() - 1; ++iEta ) {
      histos_EtaBins_.back()->Fill( ( etaBins_.at( iCat ).at( iEta ) + etaBins_.at( iCat ).at( iEta + 1 ) ) / 2., iEta ); // fill bin with mean
    }

    // Pt binning
    histos_PtBins_.push_back( dir.make< TH1D >( std::string( cat + "_binsPt" ).c_str(), cat.c_str(), ptBins_.at( iCat ).size() - 1, ptBins_.at( iCat ).data() ) );
    histos_PtBins_.back()->SetXTitle( "p_{t} (GeV)" );
    histos_PtBins_.back()->SetYTitle( "bin" );
    histos_PtBins_.back()->GetYaxis()->SetRangeUser( -1., ptBins_.at( iCat ).size() );
    for ( unsigned iPt = 0; iPt < ptBins_.at( iCat ).size() - 1; ++iPt ) {
      histos_PtBins_.back()->Fill( ( ptBins_.at( iCat ).at( iPt ) + ptBins_.at( iCat ).at( iPt + 1 ) ) / 2., iPt ); // fill bin with mean
    }

    // N-tuple
    catData_.push_back( dir.make< TTree >( std::string( cat + "_data" ).c_str(), std::string( cat + " data" ).c_str() ) );
    catData_.back()->Branch( "Pt"        , &pt_        , "pt/D" );
    catData_.back()->Branch( "Eta"       , &eta_       , "eta/D" );
    catData_.back()->Branch( "Phi"       , &phi_       , "phi/D" );
    catData_.back()->Branch( "BinEta"    , &binEta_    , "binEta/I" );
    catData_.back()->Branch( "BinEtaSymm", &binEtaSymm_, "binEtaSymm/I" );
    catData_.back()->Branch( "PtAlt"        , &ptAlt_        , "ptAlt/D" );
    catData_.back()->Branch( "EtaAlt"       , &etaAlt_       , "etaAlt/D" );
    catData_.back()->Branch( "PhiAlt"       , &phiAlt_       , "phiAlt/D" );
    catData_.back()->Branch( "BinEtaAlt"    , &binEtaAlt_    , "binEtaAlt/I" );
    catData_.back()->Branch( "BinEtaSymmAlt", &binEtaSymmAlt_, "binEtaSymmAlt/I" );
    if ( cat == "UdscJet" || cat == "BJet" ) {
      catData_.back()->Branch( "PtGenJet"        , &ptGenJet_        , "ptGenJet/D" );
      catData_.back()->Branch( "EtaGenJet"       , &etaGenJet_       , "etaGenJet/D" );
      catData_.back()->Branch( "PhiGenJet"       , &phiGenJet_       , "phiGenJet/D" );
      catData_.back()->Branch( "BinEtaGenJet"    , &binEtaGenJet_    , "binEtaGenJet/I" );
      catData_.back()->Branch( "BinEtaSymmGenJet", &binEtaSymmGenJet_, "binEtaSymmGenJet/I" );
      catData_.back()->Branch( "PtGenJetAlt"        , &ptGenJetAlt_        , "ptGenJetAlt/D" );
      catData_.back()->Branch( "EtaGenJetAlt"       , &etaGenJetAlt_       , "etaGenJetAlt/D" );
      catData_.back()->Branch( "PhiGenJetAlt"       , &phiGenJetAlt_       , "phiGenJetAlt/D" );
      catData_.back()->Branch( "BinEtaGenJetAlt"    , &binEtaGenJetAlt_    , "binEtaGenJetAlt/I" );
      catData_.back()->Branch( "BinEtaSymmGenJetAlt", &binEtaSymmGenJetAlt_, "binEtaSymmGenJetAlt/I" );
    }
    catData_.back()->Branch( "PtGen"        , &ptGen_        , "ptGen/D" );
    catData_.back()->Branch( "EtaGen"       , &etaGen_       , "etaGen/D" );
    catData_.back()->Branch( "PhiGen"       , &phiGen_       , "phiGen/D" );
    catData_.back()->Branch( "BinEtaGen"    , &binEtaGen_    , "binEtaGen/I" );
    catData_.back()->Branch( "BinEtaSymmGen", &binEtaSymmGen_, "binEtaSymmGen/I" );

    for ( unsigned iProp = 0; iProp < kinProps_.size(); ++iProp ) {
      const std::string prop( kinProps_.at( iProp ) );
      TFileDirectory subDir( dir.mkdir( prop.c_str(), "" ) );

      TFileDirectory subDirReco( subDir.mkdir( "Reco", "" ) );
      TFileDirectory subDirRecoInv( subDir.mkdir( "RecoInv", "" ) );
      TFileDirectory subDirAlt( subDir.mkdir( "Alt", "" ) );
      TFileDirectory subDirAltInv( subDir.mkdir( "AltInv", "" ) );
      TFileDirectory subDirGen( subDir.mkdir( "Gen", "" ) );
      TFileDirectory subDirGenInv( subDir.mkdir( "GenInv", "" ) );
      TFileDirectory subDirAltGen( subDir.mkdir( "AltGen", "" ) );
      TFileDirectory subDirAltGenInv( subDir.mkdir( "AltGenInv", "" ) );
      for ( unsigned iEta = 0; iEta < etaBins_.at( iCat ).size() - 1; ++iEta ) {
        const std::string eta( "Eta" + boost::lexical_cast< std::string >( iEta ) );
        const std::string title( cat + ", " + boost::lexical_cast< std::string >( etaBins_.at( iCat ).at( iEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( iCat ).at( iEta + 1 ) ) );
        TFileDirectory subDirRecoEta( subDirReco.mkdir( eta.c_str(), title.c_str() ) );
        subDirRecoEta.mkdir( std::string( "Reco_" + eta ).c_str(), "" );
        TFileDirectory subDirRecoInvEta( subDirRecoInv.mkdir( eta.c_str(), title.c_str() ) );
        subDirRecoInvEta.mkdir( std::string( "RecoInv_" + eta ).c_str(), "" );
        TFileDirectory subDirAltEta( subDirAlt.mkdir( eta.c_str(), title.c_str() ) );
        subDirAltEta.mkdir( std::string( "Alt_" + eta ).c_str(), "" );
        TFileDirectory subDirAltInvEta( subDirAltInv.mkdir( eta.c_str(), title.c_str() ) );
        subDirAltInvEta.mkdir( std::string( "AltInv_" + eta ).c_str(), "" );
        TFileDirectory subDirGenEta( subDirGen.mkdir( eta.c_str(), title.c_str() ) );
        subDirGenEta.mkdir( std::string( "Gen_" + eta ).c_str(), "" );
        TFileDirectory subDirGenInvEta( subDirGenInv.mkdir( eta.c_str(), title.c_str() ) );
        subDirGenInvEta.mkdir( std::string( "GenInv_" + eta ).c_str(), "" );
        TFileDirectory subDirAltGenEta( subDirAltGen.mkdir( eta.c_str(), title.c_str() ) );
        subDirAltGenEta.mkdir( std::string( "AltGen_" + eta ).c_str(), "" );
        TFileDirectory subDirAltGenInvEta( subDirAltGenInv.mkdir( eta.c_str(), title.c_str() ) );
        subDirAltGenInvEta.mkdir( std::string( "AltGenInv_" + eta ).c_str(), "" );
      }

      TFileDirectory subDirRecoSymm( subDir.mkdir( "RecoSymm", "" ) );
      TFileDirectory subDirRecoInvSymm( subDir.mkdir( "RecoInvSymm", "" ) );
      TFileDirectory subDirAltSymm( subDir.mkdir( "AltSymm", "" ) );
      TFileDirectory subDirAltInvSymm( subDir.mkdir( "AltInvSymm", "" ) );
      TFileDirectory subDirGenSymm( subDir.mkdir( "GenSymm", "" ) );
      TFileDirectory subDirGenInvSymm( subDir.mkdir( "GenInvSymm", "" ) );
      TFileDirectory subDirAltGenSymm( subDir.mkdir( "AltGenSymm", "" ) );
      TFileDirectory subDirAltGenInvSymm( subDir.mkdir( "AltGenInvSymm", "" ) );
      for ( unsigned iEtaSymm = 0; iEtaSymm < etaSymmBins_.at( iCat ).size() - 1; ++iEtaSymm ) {
        const std::string etaSymm( "Eta" + boost::lexical_cast< std::string >( iEtaSymm ) );
        const std::string title( cat + ", " + boost::lexical_cast< std::string >( etaSymmBins_.at( iCat ).at( iEtaSymm ) ) + " #leq |#eta| #leq " + boost::lexical_cast< std::string >( etaSymmBins_.at( iCat ).at( iEtaSymm + 1 ) ) );
        TFileDirectory subDirRecoSymmEta( subDirRecoSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
        subDirRecoSymmEta.mkdir( std::string( "RecoSymm_" + etaSymm ).c_str(), "" );
        TFileDirectory subDirRecoInvSymmEta( subDirRecoInvSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
        subDirRecoInvSymmEta.mkdir( std::string( "RecoInvSymm_" + etaSymm ).c_str(), "" );
        TFileDirectory subDirAltSymmEta( subDirAltSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
        subDirAltSymmEta.mkdir( std::string( "AltSymm_" + etaSymm ).c_str(), "" );
        TFileDirectory subDirAltInvSymmEta( subDirAltInvSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
        subDirAltInvSymmEta.mkdir( std::string( "AltInvSymm_" + etaSymm ).c_str(), "" );
        TFileDirectory subDirGenSymmEta( subDirGenSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
        subDirGenSymmEta.mkdir( std::string( "GenSymm_" + etaSymm ).c_str(), "" );
        TFileDirectory subDirGenInvSymmEta( subDirGenInvSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
        subDirGenInvSymmEta.mkdir( std::string( "GenInvSymm_" + etaSymm ).c_str(), "" );
        TFileDirectory subDirAltGenSymmEta( subDirAltGenSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
        subDirAltGenSymmEta.mkdir( std::string( "AltGenSymm_" + etaSymm ).c_str(), "" );
        TFileDirectory subDirAltGenInvSymmEta( subDirAltGenInvSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
        subDirAltGenInvSymmEta.mkdir( std::string( "AltGenInvSymm_" + etaSymm ).c_str(), "" );
      }

    }

  }

}


// Event loop
void AnalyzeHitFit::analyze( const edm::Event & iEvent, const edm::EventSetup & iSetup )
{

  // TQAF semi-leptonic events
  iEvent.getByLabel( ttSemiLeptonicEventMuonsTag_, ttSemiLeptonicEventMuons_ );
  iEvent.getByLabel( ttSemiLeptonicEventElecsTag_, ttSemiLeptonicEventElecs_ );

  // PAT objects
  iEvent.getByLabel( patMuonsTag_, patMuons_ );
  iEvent.getByLabel( patElecsTag_, patElecs_ );
  iEvent.getByLabel( patJetsTag_, patJets_ );
  iEvent.getByLabel( patMETsTag_, patMETs_ );

  // Pile-up
  iEvent.getByLabel( "addPileupInfo", pileUp_ );

  // MC
  if ( ! iEvent.isRealData() ) {

    // TQAF MC event
    iEvent.getByLabel( ttGenEventTag_, ttGenEvent_ );

    // Semi-leptonic signal event
    if (    ttGenEvent_->isTtBar()
         && ( ttGenEvent_->isSemiLeptonic( WDecay::kMuon ) || ttGenEvent_->isSemiLeptonic( WDecay::kElec ) )
       ) {

      // Valid full TTbar MC matching
      if (  ( ttSemiLeptonicEventMuons_.isValid() && ttSemiLeptonicEventMuons_->isHypoValid( TtEvent::kGenMatch ) )
         || ( ttSemiLeptonicEventElecs_.isValid() && ttSemiLeptonicEventElecs_->isHypoValid( TtEvent::kGenMatch ) )
         ) {
        // Initialise variables
        nPVTrue_              = -1.;
        nPVObserved_          = -1;
        pileUpWeightTrue_     =  0.;
        pileUpWeightObserved_ =  0.;

        for ( unsigned iCat = 0; iCat < objCats_.size(); ++iCat ) {
          const std::string cat( objCats_.at( iCat ) );
          // Initialise variables
          binEta_              = -1;
          binEtaAlt_           = -1;
          binEtaGenJet_        = -1;
          binEtaGenJetAlt_     = -1;
          binEtaGen_           = -1;
          binEtaSymm_          = -1;
          binEtaSymmAlt_       = -1;
          binEtaSymmGen_       = -1;
          binEtaSymmGenJet_    = -1;
          binEtaSymmGenJetAlt_ = -1;
          pt_           = -9.;
          ptAlt_        = -9.;
          ptGenJet_     = -9.;
          ptGenJetAlt_  = -9.;
          ptGen_        = -9.;
          eta_          = -9.;
          etaAlt_       = -9.;
          etaGenJet_    = -9.;
          etaGenJetAlt_ = -9.;
          etaGen_       = -9.;
          phi_          = -9.;
          phiAlt_       = -9.;
          phiGenJet_    = -9.;
          phiGenJetAlt_ = -9.;
          phiGen_       = -9.;
          // Fill variables
          if ( objCats_.at( iCat ) == "Mu" ) {
            if ( ttGenEvent_->isSemiLeptonic( WDecay::kMuon ) ) {
              if ( ttSemiLeptonicEventMuons_.isValid() && ttSemiLeptonicEventMuons_->isHypoValid( TtEvent::kGenMatch )  ) {
                fill( iCat, ttSemiLeptonicEventMuons_ );
              }
              else edm::LogInfo( "AnalyzeHitFit" ) << "...no valid MC match in muon channel";
            }
          }
          else if ( objCats_.at( iCat ) == "Elec" ) {
            if ( ttGenEvent_->isSemiLeptonic( WDecay::kElec ) ) {
              if ( ttSemiLeptonicEventElecs_.isValid() && ttSemiLeptonicEventElecs_->isHypoValid( TtEvent::kGenMatch ) ) {
                fill( iCat, ttSemiLeptonicEventElecs_ );
              }
              else edm::LogInfo( "AnalyzeHitFit" ) << "...no valid MC match in electron channel";
            }
          }
          else {
            if ( ttGenEvent_->isSemiLeptonic( WDecay::kMuon ) && ttSemiLeptonicEventMuons_.isValid() && ttSemiLeptonicEventMuons_->isHypoValid( TtEvent::kGenMatch )  ) {
              fill( iCat, ttSemiLeptonicEventMuons_ );
            }
            else if ( ttGenEvent_->isSemiLeptonic( WDecay::kElec ) && ttSemiLeptonicEventElecs_.isValid() && ttSemiLeptonicEventElecs_->isHypoValid( TtEvent::kGenMatch ) ) {
              fill( iCat, ttSemiLeptonicEventElecs_ );
            }
            else edm::LogInfo( "AnalyzeHitFit" ) << "...no valid MC match in both channel";
          }
          // Fill tree
          catData_.at( iCat )->Fill();

          // Do it again for jets
          if ( cat == "UdscJet" || cat == "BJet" ) {
            // Do it again
            binEta_              = -1;
            binEtaAlt_           = -1;
            binEtaGenJet_        = -1;
            binEtaGenJetAlt_     = -1;
            binEtaGen_           = -1;
            binEtaSymm_          = -1;
            binEtaSymmAlt_       = -1;
            binEtaSymmGen_       = -1;
            binEtaSymmGenJet_    = -1;
            binEtaSymmGenJetAlt_ = -1;
            pt_           = -9.;
            ptAlt_        = -9.;
            ptGenJet_     = -9.;
            ptGenJetAlt_  = -9.;
            ptGen_        = -9.;
            eta_          = -9.;
            etaAlt_       = -9.;
            etaGenJet_    = -9.;
            etaGenJetAlt_ = -9.;
            etaGen_       = -9.;
            phi_          = -9.;
            phiAlt_       = -9.;
            phiGenJet_    = -9.;
            phiGenJetAlt_ = -9.;
            phiGen_       = -9.;
            if ( ttSemiLeptonicEventMuons_.isValid() && ttSemiLeptonicEventMuons_->isHypoValid( TtEvent::kGenMatch )  ) {
              fill( iCat, ttSemiLeptonicEventMuons_, true );
            }
            else if ( ttSemiLeptonicEventElecs_.isValid() && ttSemiLeptonicEventElecs_->isHypoValid( TtEvent::kGenMatch ) ) {
              fill( iCat, ttSemiLeptonicEventElecs_, true );
            }
            else edm::LogInfo( "AnalyzeHitFit" ) << "...no valid MC match in both channel";
            catData_.at( iCat )->Fill();
          }
        }
        fill();
        data_->Fill();
        ++filledEvents_;

      } // Valid full TTbar MC matching
      else edm::LogInfo( "AnalyzeHitFit" ) << "...no valid MC match";

    } // Semi-leptonic signal event
    else edm::LogInfo( "AnalyzeHitFit" ) << "...no signal event";

  } // MC

}


// End job
void AnalyzeHitFit::endJob()
{

  edm::LogPrint( "AnalyzeHitFit" ) << "\n\n**************\nFilled events: " << filledEvents_ << "\n**************\n";

  if ( plot_ ) {

    my::setPlotEnvironment( gStyle );

    TCanvas c1( "c1" );
    c1.cd();

    histo_pileUpWeightTrue_->Draw();
    c1.Print( std::string( pathPlots_ + histo_pileUpWeightTrue_->GetName() + ".png" ).c_str() );
    histo_pileUpWeightObserved_->Draw();
    c1.Print( std::string( pathPlots_ + histo_pileUpWeightObserved_->GetName() + ".png" ).c_str() );

    data_->Draw( "NPVTrue" );
    c1.Print( std::string( pathPlots_ + "NPVTrue.png" ).c_str() );
    data_->Draw( "NPVObserved" );
    c1.Print( std::string( pathPlots_ + "NPVObserved.png" ).c_str() );
    data_->Draw( "PileUpWeightTrue" );
    c1.Print( std::string( pathPlots_ + "PileUpWeightTrue.png" ).c_str() );
    data_->Draw( "PileUpWeightObserved" );
    c1.Print( std::string( pathPlots_ + "PileUpWeightObserved.png" ).c_str() );

    for ( unsigned iCat = 0; iCat < objCats_.size(); ++iCat ) {
      const std::string cat( objCats_.at( iCat ) );

      histos_EtaBins_.at( iCat )->Draw();
      c1.Print( std::string( pathPlots_ + histos_EtaBins_.at( iCat )->GetName() + ".png" ).c_str() );
      histos_PtBins_.at( iCat )->Draw();
      c1.Print( std::string( pathPlots_ + histos_PtBins_.at( iCat )->GetName() + ".png" ).c_str() );
      catData_.at( iCat )->Draw( "Pt" );
      c1.Print( std::string( pathPlots_ + cat + "_Pt.png" ).c_str() );
      catData_.at( iCat )->Draw( "Eta" );
      c1.Print( std::string( pathPlots_ + cat + "_Eta.png" ).c_str() );
      catData_.at( iCat )->Draw( "Phi" );
      c1.Print( std::string( pathPlots_ + cat + "_Phi.png" ).c_str() );
      catData_.at( iCat )->Draw( "BinEta" );
      c1.Print( std::string( pathPlots_ + cat + "_BinEta.png" ).c_str() );
      catData_.at( iCat )->Draw( "BinEtaSymm" );
      c1.Print( std::string( pathPlots_ + cat + "_BinEtaSymm.png" ).c_str() );
      catData_.at( iCat )->Draw( "PtAlt" );
      c1.Print( std::string( pathPlots_ + cat + "_PtAlt.png" ).c_str() );
      catData_.at( iCat )->Draw( "EtaAlt" );
      c1.Print( std::string( pathPlots_ + cat + "_EtaAlt.png" ).c_str() );
      catData_.at( iCat )->Draw( "PhiAlt" );
      c1.Print( std::string( pathPlots_ + cat + "_PhiAlt.png" ).c_str() );
      catData_.at( iCat )->Draw( "BinEtaAlt" );
      c1.Print( std::string( pathPlots_ + cat + "_BinEtaAlt.png" ).c_str() );
      catData_.at( iCat )->Draw( "BinEtaSymmAlt" );
      c1.Print( std::string( pathPlots_ + cat + "_BinEtaSymmAlt.png" ).c_str() );
      if ( cat == "UdscJet" || cat == "BJet" ) {
        catData_.at( iCat )->Draw( "PtGenJet" );
        c1.Print( std::string( pathPlots_ + cat + "_PtGenJet.png" ).c_str() );
        catData_.at( iCat )->Draw( "EtaGenJet" );
        c1.Print( std::string( pathPlots_ + cat + "_EtaGenJet.png" ).c_str() );
        catData_.at( iCat )->Draw( "PhiGenJet" );
        c1.Print( std::string( pathPlots_ + cat + "_PhiGenJet.png" ).c_str() );
        catData_.at( iCat )->Draw( "BinEtaGenJet" );
        c1.Print( std::string( pathPlots_ + cat + "_BinEtaGenJet.png" ).c_str() );
        catData_.at( iCat )->Draw( "BinEtaSymmGenJet" );
        c1.Print( std::string( pathPlots_ + cat + "_BinEtaSymmGenJet.png" ).c_str() );
        catData_.at( iCat )->Draw( "PtGenJetAlt" );
        c1.Print( std::string( pathPlots_ + cat + "_PtGenJetAlt.png" ).c_str() );
        catData_.at( iCat )->Draw( "EtaGenJetAlt" );
        c1.Print( std::string( pathPlots_ + cat + "_EtaGenJetAlt.png" ).c_str() );
        catData_.at( iCat )->Draw( "PhiGenJetAlt" );
        c1.Print( std::string( pathPlots_ + cat + "_PhiGenJetAlt.png" ).c_str() );
        catData_.at( iCat )->Draw( "BinEtaGenJetAlt" );
        c1.Print( std::string( pathPlots_ + cat + "_BinEtaGenJetAlt.png" ).c_str() );
        catData_.at( iCat )->Draw( "BinEtaSymmGenJetAlt" );
        c1.Print( std::string( pathPlots_ + cat + "_BinEtaSymmGenJetAlt.png" ).c_str() );
      }
      catData_.at( iCat )->Draw( "PtGen" );
      c1.Print( std::string( pathPlots_ + cat + "_PtGen.png" ).c_str() );
      catData_.at( iCat )->Draw( "EtaGen" );
      c1.Print( std::string( pathPlots_ + cat + "_EtaGen.png" ).c_str() );
      catData_.at( iCat )->Draw( "PhiGen" );
      c1.Print( std::string( pathPlots_ + cat + "_PhiGen.png" ).c_str() );
      catData_.at( iCat )->Draw( "BinEtaGen" );
      c1.Print( std::string( pathPlots_ + cat + "_BinEtaGen.png" ).c_str() );
      catData_.at( iCat )->Draw( "BinEtaSymmGen" );
      c1.Print( std::string( pathPlots_ + cat + "_BinEtaSymmGen.png" ).c_str() );

    }

  }

}


unsigned AnalyzeHitFit::getEtaBin( unsigned iCat, double eta, bool symm )
{

  std::vector< double > etaBins( symm ? etaSymmBins_.at( iCat ) : etaBins_.at( iCat ) );
  if ( symm ) eta = fabs( eta );
  for ( unsigned iEta = 0; iEta < etaBins.size() - 1; ++iEta ) {
    if ( etaBins.at( iEta ) <= eta && eta < etaBins.at( iEta + 1 ) ) return iEta;
  }

  return etaBins.size();

}


void AnalyzeHitFit::fill()
{

  // Vertices
  for ( std::vector< PileupSummaryInfo >::const_iterator iPileUp = pileUp_->begin(); iPileUp != pileUp_->end(); ++iPileUp ) {
    if ( iPileUp->getBunchCrossing() == 0 ) {
      nPVTrue_     = iPileUp->getTrueNumInteractions();
      nPVObserved_ = iPileUp->getPU_NumInteractions();
      // Pile-up weight
      pileUpWeightTrue_     = lumiWeightTrue_.weight( nPVTrue_ );
      pileUpWeightObserved_ = lumiWeightObserved_.weight( nPVObserved_ );
    }
  }

}


void AnalyzeHitFit::fill( unsigned iCat, const edm::Handle< TtSemiLeptonicEvent > & ttSemiLeptonicEvent, bool repeat )
{

  const std::string cat( objCats_.at( iCat ) );
  const std::vector< int > jetLepCombi( ttSemiLeptonicEvent->jetLeptonCombination( TtEvent::kGenMatch ) );
  if ( cat == "Mu" ) {
    if ( repeat ) {
      edm::LogWarning( "AnalyzeHitFit" ) << "Category repeated: " << cat;
      return;
    }
    const int muonIndex( ttSemiLeptonicEvent->jetLeptonCombination( TtEvent::kGenMatch ).at( TtSemiLepEvtPartons::Lepton ) );
    pt_      = ttSemiLeptonicEvent->singleLepton( TtEvent::kGenMatch )->pt();
    ptAlt_   = patMuons_->at( muonIndex ).globalTrack()->pt();
    ptGen_   = ttGenEvent_->singleLepton()->pt();
    eta_     = ttSemiLeptonicEvent->singleLepton( TtEvent::kGenMatch )->eta();
    etaAlt_  = patMuons_->at( muonIndex ).globalTrack()->eta();
    etaGen_  = ttGenEvent_->singleLepton()->eta();
    phi_     = ttSemiLeptonicEvent->singleLepton( TtEvent::kGenMatch )->phi();
    phiAlt_  = patMuons_->at( muonIndex ).globalTrack()->phi();
    phiGen_  = ttGenEvent_->singleLepton()->phi();
  }
  else if ( cat == "Elec" ) {
    if ( repeat ) {
      edm::LogWarning( "AnalyzeHitFit" ) << "Category repeated: " << cat;
      return;
    }
//     const int elecIndex( ttSemiLeptonicEvent->jetLeptonCombination( TtEvent::kGenMatch ).at( TtSemiLepEvtPartons::Lepton ) );
    pt_      = ttSemiLeptonicEvent->singleLepton( TtEvent::kGenMatch )->pt();
//     ptAlt_   = patElecs_->at( elecIndex ).closestCtfTrackRef()->pt(); // FIXME: or which one?
    ptAlt_   = pt_;
    ptGen_   = ttGenEvent_->singleLepton()->pt();
    eta_     = ttSemiLeptonicEvent->singleLepton( TtEvent::kGenMatch )->eta();
//     etaAlt_  = patElecs_->at( elecIndex ).closestCtfTrackRef()->eta(); // FIXME: or which one?
    etaAlt_  = eta_;
    etaGen_  = ttGenEvent_->singleLepton()->eta();
    phi_     = ttSemiLeptonicEvent->singleLepton( TtEvent::kGenMatch )->phi();
//     phiAlt_  = patElecs_->at( elecIndex ).closestCtfTrackRef()->phi(); // FIXME: or which one?
    phiAlt_  = phi_;
    phiGen_  = ttGenEvent_->singleLepton()->phi();
  }
  else if ( cat == "UdscJet" ) {
    if ( repeat ) {
      const pat::Jet jet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LightQ ) ).correctedJet( jecLevel_, "uds" ) );
      pt_     = jet.pt();
      ptAlt_  = jet.et();
      ptGen_  = ttGenEvent_->hadronicDecayQuark()->pt();
      eta_    = jet.eta();
      etaAlt_ = eta_;
      etaGen_ = ttGenEvent_->hadronicDecayQuark()->eta();
      phi_    = jet.phi();
      phiAlt_ = phi_;
      phiGen_ = ttGenEvent_->hadronicDecayQuark()->phi();
      if ( jet.genJet() != 0 ) {
        ptGenJet_  = jet.genJet()->pt();
        etaGenJet_ = jet.genJet()->eta();
        phiGenJet_ = jet.genJet()->phi();
        ptGenJetAlt_  = jet.genJet()->et();
        etaGenJetAlt_ = etaGenJet_;
        phiGenJetAlt_ = phiGenJet_;
      }
    }
    else {
      const pat::Jet jetBar( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LightQBar ) ).correctedJet( jecLevel_, "uds" ) );
      pt_     = jetBar.pt();
      ptAlt_  = jetBar.et();
      ptGen_  = ttGenEvent_->hadronicDecayQuarkBar()->pt();
      eta_    = jetBar.eta();
      etaAlt_ = eta_;
      etaGen_ = ttGenEvent_->hadronicDecayQuarkBar()->eta();
      phi_    = jetBar.phi();
      phiAlt_ = phi_;
      phiGen_ = ttGenEvent_->hadronicDecayQuarkBar()->phi();
      if ( jetBar.genJet() != 0 ) {
        ptGenJet_  = jetBar.genJet()->pt();
        etaGenJet_ = jetBar.genJet()->eta();
        phiGenJet_ = jetBar.genJet()->phi();
        ptGenJetAlt_  = jetBar.genJet()->et();
        etaGenJetAlt_ = etaGenJet_;
        phiGenJetAlt_ = phiGenJet_;
      }
    }
  }
  else if ( cat == "BJet" ) {
    if ( repeat ) {
      const pat::Jet bJetLep( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LepB ) ).correctedJet( jecLevel_, "bottom" ) );
      pt_     = bJetLep.pt();
      ptAlt_  = bJetLep.et();
      ptGen_  = ttGenEvent_->leptonicDecayB()->pt();
      eta_    = bJetLep.eta();
      etaAlt_ = eta_;
      etaGen_ = ttGenEvent_->leptonicDecayB()->eta();
      phi_    = bJetLep.phi();
      phiAlt_ = phi_;
      phiGen_ = ttGenEvent_->leptonicDecayB()->phi();
      if ( bJetLep.genJet() != 0 ) {
        ptGenJet_  = bJetLep.genJet()->pt();
        etaGenJet_ = bJetLep.genJet()->eta();
        phiGenJet_ = bJetLep.genJet()->phi();
        ptGenJetAlt_  = bJetLep.genJet()->et();
        etaGenJetAlt_ = etaGenJet_;
        phiGenJetAlt_ = phiGenJet_;
      }
    }
    else {
      const pat::Jet bJetHad( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::HadB ) ).correctedJet( jecLevel_, "bottom" ) );
      pt_     = bJetHad.pt();
      ptAlt_  = bJetHad.et();
      ptGen_  = ttGenEvent_->hadronicDecayB()->pt();
      eta_    = bJetHad.eta();
      etaAlt_ = eta_;
      etaGen_ = ttGenEvent_->hadronicDecayB()->eta();
      phi_    = bJetHad.phi();
      phiAlt_ = phi_;
      phiGen_ = ttGenEvent_->hadronicDecayB()->phi();
      if ( bJetHad.genJet() != 0 ) {
        ptGenJet_  = bJetHad.genJet()->pt();
        etaGenJet_ = bJetHad.genJet()->eta();
        phiGenJet_ = bJetHad.genJet()->phi();
        ptGenJetAlt_  = bJetHad.genJet()->et();
        etaGenJetAlt_ = etaGenJet_;
        phiGenJetAlt_ = phiGenJet_;
      }
    }
  }
  else if ( cat == "MET" ) {
    if ( repeat ) {
      edm::LogWarning( "AnalyzeHitFit" ) << "Category repeated: " << cat;
      return;
    }
//     pt_     = ttSemiLeptonicEvent->singleNeutrino( TtEvent::kGenMatch )->pt();
    pt_     = patMETs_->at( 0 ).pt();
    ptAlt_  = pt_;
    ptGen_  = ttGenEvent_->singleNeutrino()->pt();
//     eta_    = ttSemiLeptonicEvent->singleNeutrino( TtEvent::kGenMatch )->eta();
    eta_    = patMETs_->at( 0 ).eta();
    etaAlt_ = eta_;
    etaGen_ = ttGenEvent_->singleNeutrino()->eta();
//     phi_    = ttSemiLeptonicEvent->singleNeutrino( TtEvent::kGenMatch )->phi();
    phi_    = patMETs_->at( 0 ).phi();
    phiAlt_ = phi_;
    phiGen_ = ttGenEvent_->singleNeutrino()->phi();
  }
  else {
    edm::LogWarning( "AnalyzeHitFit" ) << "Category missmatch: " << cat;
    return;
  }

  unsigned iEta;
  if ( eta_ != -9. ) {
    iEta = getEtaBin( iCat, eta_ );
    if ( iEta < etaBins_.at( iCat ).size() ) binEta_ = iEta;
  }
  if ( etaAlt_ != -9. ) {
    iEta = getEtaBin( iCat, etaAlt_ );
    if ( iEta < etaBins_.at( iCat ).size() ) binEtaAlt_ = iEta;
  }
  if ( etaGenJet_ != -9. ) {
    iEta = getEtaBin( iCat, etaGenJet_ );
    if ( iEta < etaBins_.at( iCat ).size() ) binEtaGenJet_ = iEta;
  }
  if ( etaGenJetAlt_ != -9. ) {
    iEta = getEtaBin( iCat, etaGenJetAlt_ );
    if ( iEta < etaBins_.at( iCat ).size() ) binEtaGenJetAlt_ = iEta;
  }
  if ( etaGen_ != -9. ) {
    iEta = getEtaBin( iCat, etaGen_ );
    if ( iEta < etaBins_.at( iCat ).size() ) binEtaGen_ = iEta;
  }
  if ( eta_ != -9. ) {
    iEta = getEtaBin( iCat, eta_, true );
    if ( iEta < etaSymmBins_.at( iCat ).size() ) binEtaSymm_ = iEta;
  }
  if ( etaAlt_ != -9. ) {
    iEta = getEtaBin( iCat, etaAlt_, true );
    if ( iEta < etaSymmBins_.at( iCat ).size() ) binEtaSymmAlt_ = iEta;
  }
  if ( etaGenJet_ != -9. ) {
    iEta = getEtaBin( iCat, etaGenJet_, true );
    if ( iEta < etaSymmBins_.at( iCat ).size() ) binEtaSymmGenJet_ = iEta;
  }
  if ( etaGenJetAlt_ != -9. ) {
    iEta = getEtaBin( iCat, etaGenJetAlt_, true );
    if ( iEta < etaSymmBins_.at( iCat ).size() ) binEtaSymmGenJetAlt_ = iEta;
  }
  if ( etaGen_ != -9. ) {
    iEta = getEtaBin( iCat, etaGen_, true );
    if ( iEta < etaSymmBins_.at( iCat ).size() ) binEtaSymmGen_ = iEta;
  }

}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( AnalyzeHitFit );
