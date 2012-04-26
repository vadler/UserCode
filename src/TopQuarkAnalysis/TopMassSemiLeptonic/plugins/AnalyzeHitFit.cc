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

#include "AnalysisDataFormats/TopObjects/interface/TtSemiLeptonicEvent.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"


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
    bool useAlt_;
    bool useSymm_;
    bool refGen_;
    std::string leptonType_;
    bool useMuons_; // indirect
    bool useElecs_; // indirect
    edm::InputTag patLeptonsTag_;
    edm::InputTag patJetsTag_;
    std::string jecLevel_;
    // Eta binning
    std::vector< std::vector< double > > etaBins_;
    std::vector< std::vector< double > > etaSymmBins_;
    // P_t binning
    std::vector< std::vector< double > > ptBins_;
    // Quantities Delta binning
    std::vector< std::vector< unsigned > > propBins_;
    std::vector< std::vector< double > > propMaxs_;
    std::vector< std::vector< unsigned > > propInvBins_;
    std::vector< std::vector< double > > propInvMaxs_;

    /// Constants
    // Object categories
    std::vector< std::string > objCats_;
    // Kinematic properties
    std::vector< std::string > kinProps_;

    /// Data
    unsigned filledEvents_;
    std::vector< TTree* > data_;
    // reconstructed
    Double_t pt_;            // used momentum term, can be: E, p, E_t, p_t
    Double_t eta_;
    Double_t phi_;
    Int_t    binEta_;        // eta bin number as determined by 'getEtaBin'
    Int_t    binEtaSymm_;    // symmetrised eta bin number as determined by 'getEtaBin'
    // reconstructed alternative
    Double_t ptAlt_;         // used momentum term, can be: E, p, E_t, p_t
    Double_t etaAlt_;
    Double_t phiAlt_;
    Int_t    binEtaAlt_;     // eta bin number as determined by 'getEtaBin'
    Int_t    binEtaSymmAlt_; // symmetrised eta bin number as determined by 'getEtaBin'
    // generated
    Double_t ptGen_;         // used momentum term, can be: E, p, E_t, p_t
    Double_t etaGen_;
    Double_t phiGen_;
    Int_t    binEtaGen_;     // eta bin number as determined by 'getEtaBin'
    Int_t    binEtaSymmGen_; // symmetrised eta bin number as determined by 'getEtaBin'

    /// Histograms
    // Binning
    std::vector< TH1D* > histos_EtaBins_;
    std::vector< TH1D* > histos_PtBins_;

    /// Private functions

    unsigned getEtaBin( unsigned iCat, double eta, bool symm = false );

    // Fill n-tuples
    void fill( unsigned iCat );

};


#include <cassert>
#include <iostream>
#include <sstream>

#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string/replace.hpp"

#include "Math/GenVector/VectorUtil.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "AnalysisDataFormats/TopObjects/interface/TtSemiLepEvtPartons.h"
#include "TopQuarkAnalysis/TopHitFit/interface/EtaDepResolution.h"


// Default constructor
AnalyzeHitFit::AnalyzeHitFit( const edm::ParameterSet & iConfig )
: ttSemiLeptonicEvent_()
, ttGenEvent_()
, patJets_()
, ttSemiLeptonicEventTag_( iConfig.getParameter< edm::InputTag >( "ttSemiLeptonicEvent" ) )
, leptonType_( iConfig.getParameter< std::string >( "leptonType" ) )
, useMuons_( leptonType_ == "Muons" )
, useElecs_( leptonType_ == "Electrons" )
, patLeptonsTag_( iConfig.getParameter< edm::InputTag >( "patLeptons" ) )
, patJetsTag_( iConfig.getParameter< edm::InputTag >( "patJets" ) )
, jecLevel_( iConfig.getParameter< std::string >( "jecLevel" ) )
, filledEvents_( 0 )
{

  // Check configuration

  assert( useMuons_ != useElecs_ );

  // Constants

  objCats_.clear();
  if ( useMuons_ ) objCats_.push_back( "Mu" );
  if ( useElecs_ ) objCats_.push_back( "Elec" );
  objCats_.push_back( "UdscJet" );
  objCats_.push_back( "UdscJetBar" );
  objCats_.push_back( "BJetHad" );
  objCats_.push_back( "BJetLep" );
  objCats_.push_back( "MET" );

  std::vector< std::string > objNames;
  if ( useMuons_ ) objNames.push_back( "muon" );
  if ( useElecs_ ) objNames.push_back( "electron" );
  objNames.push_back( "udscJet" );
  objNames.push_back( "udscJet" );
  objNames.push_back( "bJet" );
  objNames.push_back( "bJet" );
  objNames.push_back( "met" );
  assert( objNames.size() == objCats_.size() );

  kinProps_.clear();
  kinProps_.push_back( "Pt" );
  kinProps_.push_back( "Eta" );
  kinProps_.push_back( "Phi" );

  std::vector< std::string > fileNameParams;
  std::vector< std::string > etaBinParams;
  std::vector< std::string > ptBinParams;
  std::vector< std::vector< std::string > > objPropBinParams;
  std::vector< std::vector< std::string > > objPropMaxParams;
  std::vector< std::vector< std::string > > objPropInvBinParams;
  std::vector< std::vector< std::string > > objPropInvMaxParams;
  for ( unsigned iObj = 0; iObj < objNames.size(); ++iObj ) {
    fileNameParams.push_back( objNames.at( iObj ) + "Resolutions" );
    etaBinParams.push_back( objNames.at( iObj ) + "EtaBins" );
    ptBinParams.push_back( objNames.at( iObj ) + "PtBins" );
    std::vector< std::string > propBinParams;
    std::vector< std::string > propMaxParams;
    std::vector< std::string > propInvBinParams;
    std::vector< std::string > propInvMaxParams;
    for ( unsigned iProp = 0; iProp < kinProps_.size(); ++iProp ) {
      propBinParams.push_back( objNames.at( iObj ) + "Delta" + kinProps_.at( iProp ) + "Bins" );
      propMaxParams.push_back( objNames.at( iObj ) + "Delta" + kinProps_.at( iProp ) + "Max" );
      propInvBinParams.push_back( objNames.at( iObj ) + "Delta" + kinProps_.at( iProp ) + "InvBins" );
      propInvMaxParams.push_back( objNames.at( iObj ) + "Delta" + kinProps_.at( iProp ) + "InvMax" );
    }
    objPropBinParams.push_back( propBinParams );
    objPropMaxParams.push_back( propMaxParams );
    objPropInvBinParams.push_back( propInvBinParams );
    objPropInvMaxParams.push_back( propInvMaxParams );
  }

  for ( unsigned iCat = 0; iCat < objCats_.size(); ++iCat ) {

    if ( iConfig.getParameter< std::vector< double > >( etaBinParams.at( iCat ) ).size() == 0 ) {
      std::vector< double > etaBins;
      if ( objCats_.at( iCat ) != "MET" ) {
        hitfit::EtaDepResolution hitFitRes( edm::FileInPath( iConfig.getParameter< std::string >( fileNameParams.at( iCat ) ) ).fullPath() );
        for ( unsigned iEta = 0; iEta < hitFitRes.GetEtaDepResElement().size(); ++iEta ) {
          etaBins.push_back( hitFitRes.GetEtaDepResElement().at( iEta ).EtaMin() );
        }
        etaBins.push_back( hitFitRes.GetEtaDepResElement().back().EtaMax() );
      }
      else {
        etaBins.push_back( -999. );
        etaBins.push_back(  999. );
      }
      etaBins_.push_back( etaBins );
    }
    else {
      etaBins_.push_back( iConfig.getParameter< std::vector< double > >( etaBinParams.at( iCat ) ) );
    }

    if ( useSymm_ ) {
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
    }

    ptBins_.push_back( iConfig.getParameter< std::vector< double > >( ptBinParams.at( iCat ) ) );

    std::vector< unsigned > propBins;
    std::vector< double > propMaxs;
    std::vector< unsigned > propInvBins;
    std::vector< double > propInvMaxs;
    for ( unsigned iProp = 0; iProp < kinProps_.size(); ++iProp ) {
      propBins.push_back( iConfig.getParameter< unsigned >( objPropBinParams.at( iCat ).at( iProp ) ) );
      propMaxs.push_back( iConfig.getParameter< double >( objPropMaxParams.at( iCat ).at( iProp ) ) );
      propInvBins.push_back( iConfig.getParameter< unsigned >( objPropInvBinParams.at( iCat ).at( iProp ) ) );
      propInvMaxs.push_back( iConfig.getParameter< double >( objPropInvMaxParams.at( iCat ).at( iProp ) ) );
    }
    propBins_.push_back( propBins );
    propMaxs_.push_back( propMaxs );
    propInvBins_.push_back( propInvBins );
    propInvMaxs_.push_back( propInvMaxs );

  }

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
    std::string cat( "" );
    if ( ( ( cat == "UdscJet" || cat == "BJet" ) && useAlt_ ) ) cat.append( "E_t" );
    else                                                        cat.append( objCats_.at( iCat ) );
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

  edm::Service< TFileService > fileService;

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
    filledEvents_ = 0;
    data_.push_back( dir.make< TTree >( std::string( cat + "_data" ).c_str(), std::string( cat + " data" ).c_str() ) );
    data_.back()->Branch( "Pt"           , &pt_           , "pt/D" );
    data_.back()->Branch( "Eta"          , &eta_          , "eta/D" );
    data_.back()->Branch( "Phi"          , &phi_          , "phi/D" );
    data_.back()->Branch( "BinEta"       , &binEta_       , "binEta/I" );
    data_.back()->Branch( "BinEtaSymm"   , &binEtaSymm_   , "binEtaSymm/I" );
    data_.back()->Branch( "PtAlt"        , &ptAlt_        , "ptAlt/D" );
    data_.back()->Branch( "EtaAlt"       , &etaAlt_       , "etaAlt/D" );
    data_.back()->Branch( "PhiAlt"       , &phiAlt_       , "phiAlt/D" );
    data_.back()->Branch( "BinEtaAlt"    , &binEtaAlt_    , "binEtaAlt/I" );
    data_.back()->Branch( "BinEtaSymmAlt", &binEtaSymmAlt_, "binEtaSymmAlt/I" );
    data_.back()->Branch( "PtGen"        , &ptGen_        , "ptGen/D" );
    data_.back()->Branch( "EtaGen"       , &etaGen_       , "etaGen/D" );
    data_.back()->Branch( "PhiGen"       , &phiGen_       , "phiGen/D" );
    data_.back()->Branch( "BinEtaGen"    , &binEtaGen_    , "binEtaGen/I" );
    data_.back()->Branch( "BinEtaSymmGen", &binEtaSymmGen_, "binEtaSymmGen/I" );

    for ( unsigned iProp = 0; iProp < kinProps_.size(); ++iProp ) {
      const std::string prop( kinProps_.at( iProp ) );
      TFileDirectory subDir( dir.mkdir( prop.c_str(), "" ) );

      const std::string nameDirReco( "Reco" );
      TFileDirectory subDirReco( subDir.mkdir( nameDirReco.c_str(), "" ) );
      const std::string nameDirRecoInv( "RecoInv" );
      TFileDirectory subDirRecoInv( subDir.mkdir( nameDirRecoInv.c_str(), "" ) );
      for ( unsigned iEta = 0; iEta < etaBins_.at( iCat ).size() - 1; ++iEta ) {
        const std::string eta( "Eta" + boost::lexical_cast< std::string >( iEta ) );
        const std::string title( cat + ", " + boost::lexical_cast< std::string >( etaBins_.at( iCat ).at( iEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( iCat ).at( iEta + 1 ) ) );
        TFileDirectory subDirRecoEta( subDirReco.mkdir( eta.c_str(), title.c_str() ) );
        TFileDirectory subDirRecoInvEta( subDirRecoInv.mkdir( eta.c_str(), title.c_str() ) );
      }

      const std::string nameDirAlt( "Alt" );
      TFileDirectory subDirAlt( subDir.mkdir( nameDirAlt.c_str(), "" ) );
      const std::string nameDirAltInv( "AltInv" );
      TFileDirectory subDirAltInv( subDir.mkdir( nameDirAltInv.c_str(), "" ) );
      for ( unsigned iEta = 0; iEta < etaBins_.at( iCat ).size() - 1; ++iEta ) {
        const std::string eta( "Eta" + boost::lexical_cast< std::string >( iEta ) );
        const std::string title( cat + ", " + boost::lexical_cast< std::string >( etaBins_.at( iCat ).at( iEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( iCat ).at( iEta + 1 ) ) );
        TFileDirectory subDirAltEta( subDirAlt.mkdir( eta.c_str(), title.c_str() ) );
        TFileDirectory subDirAltInvEta( subDirAltInv.mkdir( eta.c_str(), title.c_str() ) );
      }

      const std::string nameDirGen( "Gen" );
      TFileDirectory subDirGen( subDir.mkdir( nameDirGen.c_str(), "" ) );
      const std::string nameDirGenInv( "GenInv" );
      TFileDirectory subDirGenInv( subDir.mkdir( nameDirGenInv.c_str(), "" ) );
      for ( unsigned iEta = 0; iEta < etaBins_.at( iCat ).size() - 1; ++iEta ) {
        const std::string eta( "Eta" + boost::lexical_cast< std::string >( iEta ) );
        const std::string title( cat + ", " + boost::lexical_cast< std::string >( etaBins_.at( iCat ).at( iEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( iCat ).at( iEta + 1 ) ) );
        TFileDirectory subDirGenEta( subDirGen.mkdir( eta.c_str(), title.c_str() ) );
        TFileDirectory subDirGenInvEta( subDirGenInv.mkdir( eta.c_str(), title.c_str() ) );
      }
      const std::string nameDirAltGen( "AltGen" );
      TFileDirectory subDirAltGen( subDir.mkdir( nameDirAltGen.c_str(), "" ) );
      const std::string nameDirAltGenInv( "AltGenInv" );
      TFileDirectory subDirAltGenInv( subDir.mkdir( nameDirAltGenInv.c_str(), "" ) );
      for ( unsigned iEta = 0; iEta < etaBins_.at( iCat ).size() - 1; ++iEta ) {
        const std::string eta( "Eta" + boost::lexical_cast< std::string >( iEta ) );
        const std::string title( cat + ", " + boost::lexical_cast< std::string >( etaBins_.at( iCat ).at( iEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( iCat ).at( iEta + 1 ) ) );
        TFileDirectory subDirAltGenEta( subDirAltGen.mkdir( eta.c_str(), title.c_str() ) );
        TFileDirectory subDirAltGenInvEta( subDirAltGenInv.mkdir( eta.c_str(), title.c_str() ) );
      }

      const std::string nameDirRecoSymm( "RecoSymm" );
      TFileDirectory subDirRecoSymm( subDir.mkdir( nameDirRecoSymm.c_str(), "" ) );
      const std::string nameDirRecoInvSymm( "RecoInvSymm" );
      TFileDirectory subDirRecoInvSymm( subDir.mkdir( nameDirRecoInvSymm.c_str(), "" ) );
      for ( unsigned iEtaSymm = 0; iEtaSymm < etaSymmBins_.at( iCat ).size() - 1; ++iEtaSymm ) {
        const std::string etaSymm( "Eta" + boost::lexical_cast< std::string >( iEtaSymm ) );
        const std::string title( cat + ", " + boost::lexical_cast< std::string >( etaSymmBins_.at( iCat ).at( iEtaSymm ) ) + " #leq |#eta| #leq " + boost::lexical_cast< std::string >( etaSymmBins_.at( iCat ).at( iEtaSymm + 1 ) ) );
        TFileDirectory subDirRecoSymmEta( subDirRecoSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
        TFileDirectory subDirRecoInvSymmEta( subDirRecoInvSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
      }

      const std::string nameDirAltSymm( "AltSymm" );
      TFileDirectory subDirAltSymm( subDir.mkdir( nameDirAltSymm.c_str(), "" ) );
      const std::string nameDirAltInvSymm( "AltInvSymm" );
      TFileDirectory subDirAltInvSymm( subDir.mkdir( nameDirAltInvSymm.c_str(), "" ) );
      for ( unsigned iEtaSymm = 0; iEtaSymm < etaSymmBins_.at( iCat ).size() - 1; ++iEtaSymm ) {
        const std::string etaSymm( "Eta" + boost::lexical_cast< std::string >( iEtaSymm ) );
        const std::string title( cat + ", " + boost::lexical_cast< std::string >( etaSymmBins_.at( iCat ).at( iEtaSymm ) ) + " #leq |#eta| #leq " + boost::lexical_cast< std::string >( etaSymmBins_.at( iCat ).at( iEtaSymm + 1 ) ) );
        TFileDirectory subDirAltSymmEta( subDirAltSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
        TFileDirectory subDirAltInvSymmEta( subDirAltInvSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
      }

      const std::string nameDirGenSymm( "GenSymm" );
      TFileDirectory subDirGenSymm( subDir.mkdir( nameDirGenSymm.c_str(), "" ) );
      const std::string nameDirGenInvSymm( "GenInvSymm" );
      TFileDirectory subDirGenInvSymm( subDir.mkdir( nameDirGenInvSymm.c_str(), "" ) );
      for ( unsigned iEtaSymm = 0; iEtaSymm < etaSymmBins_.at( iCat ).size() - 1; ++iEtaSymm ) {
        const std::string etaSymm( "Eta" + boost::lexical_cast< std::string >( iEtaSymm ) );
        const std::string title( cat + ", " + boost::lexical_cast< std::string >( etaSymmBins_.at( iCat ).at( iEtaSymm ) ) + " #leq |#eta| #leq " + boost::lexical_cast< std::string >( etaSymmBins_.at( iCat ).at( iEtaSymm + 1 ) ) );
        TFileDirectory subDirGenSymmEta( subDirGenSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
        TFileDirectory subDirGenInvSymmEta( subDirGenInvSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
      }
      const std::string nameDirAltGenSymm( "AltGenSymm" );
      TFileDirectory subDirAltGenSymm( subDir.mkdir( nameDirAltGenSymm.c_str(), "" ) );
      const std::string nameDirAltGenInvSymm( "AltGenInvSymm" );
      TFileDirectory subDirAltGenInvSymm( subDir.mkdir( nameDirAltGenInvSymm.c_str(), "" ) );
      for ( unsigned iEtaSymm = 0; iEtaSymm < etaSymmBins_.at( iCat ).size() - 1; ++iEtaSymm ) {
        const std::string etaSymm( "Eta" + boost::lexical_cast< std::string >( iEtaSymm ) );
        const std::string title( cat + ", " + boost::lexical_cast< std::string >( etaSymmBins_.at( iCat ).at( iEtaSymm ) ) + " #leq |#eta| #leq " + boost::lexical_cast< std::string >( etaSymmBins_.at( iCat ).at( iEtaSymm + 1 ) ) );
        TFileDirectory subDirAltGenSymmEta( subDirAltGenSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
        TFileDirectory subDirAltGenInvSymmEta( subDirAltGenInvSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
      }

    }

  }

}


// Event loop
void AnalyzeHitFit::analyze( const edm::Event & iEvent, const edm::EventSetup & iSetup )
{

  // Reset
  ttGenEvent_ = 0;

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

    // Semi-leptonic signal event
    if ( ttGenEvent_->isTtBar() &&
         ( ( ttGenEvent_->isSemiLeptonic( WDecay::kMuon ) && useMuons_ ) ||
           ( ttGenEvent_->isSemiLeptonic( WDecay::kElec ) && useElecs_ ) ) ) {

      // Valid full TTbar MC matching
      if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kGenMatch ) ) {

        for ( unsigned iCat = 0; iCat < objCats_.size(); ++iCat ) {
          // Initialise variables
          binEta_        = -1;
          binEtaAlt_     = -1;
          binEtaGen_     = -1;
          binEtaSymm_    = -1;
          binEtaSymmAlt_ = -1;
          binEtaSymmGen_ = -1;
          pt_      = -999.;
          ptAlt_   = -999.;
          ptGen_   = -999.;
          eta_     = -999.;
          etaAlt_  = -999.;
          etaGen_  = -999.;
          phi_     = -999.;
          phiAlt_  = -999.;
          phiGen_  = -999.;
          // Fill variables
          if ( objCats_.at( iCat ) == "Mu" ) {
            if ( useMuons_ ) fill( iCat );
          }
          else if ( objCats_.at( iCat ) == "Elec" ) {
            if ( useElecs_ ) fill( iCat );
          }
          else {
            fill( iCat );
          }
          // Fill tree
          data_.at( iCat )->Fill();
        }
        ++filledEvents_;

      } // Valid full TTbar MC matching
      else {
        if      ( patJets_->size() < 4 )               edm::LogInfo( "AnalyzeHitFit" ) << "...no valid MC match due to missing jets";
        else if ( useMuons_ && patMuons_->size() < 1 ) edm::LogInfo( "AnalyzeHitFit" ) << "...no valid MC match due to missing muon";
        else if ( useElecs_ && patElecs_->size() < 1 ) edm::LogInfo( "AnalyzeHitFit" ) << "...no valid MC match due to missing electron";
        else                                           edm::LogInfo( "AnalyzeHitFit" ) << "...no valid MC match";
      }

    } // Semi-leptonic signal event
    else edm::LogInfo( "AnalyzeHitFit" ) << "...no signal event";

  } // MC

}


// End job
void AnalyzeHitFit::endJob()
{

  edm::LogInfo( "AnalyzeHitFit" ) << "Filled events: " << filledEvents_;

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


void AnalyzeHitFit::fill( unsigned iCat )
{

  const std::string cat( objCats_.at( iCat ) );
  if ( cat == "Mu" ) {
    const int muonIndex( ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ).at( TtSemiLepEvtPartons::Lepton ) );
    pt_      = ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt();
    ptAlt_   = patMuons_->at( muonIndex ).globalTrack()->pt();
    ptGen_   = ttGenEvent_->singleLepton()->pt();
    eta_     = ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->eta();
    etaAlt_  = patMuons_->at( muonIndex ).globalTrack()->eta();
    etaGen_  = ttGenEvent_->singleLepton()->eta();
    phi_     = ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->phi();
    phiAlt_  = patMuons_->at( muonIndex ).globalTrack()->phi();
    phiGen_  = ttGenEvent_->singleLepton()->phi();
  }
  else if ( cat == "Elec" ) {
    const int elecIndex( ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ).at( TtSemiLepEvtPartons::Lepton ) );
    pt_      = ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt();
    ptAlt_   = patElecs_->at( elecIndex ).closestCtfTrackRef()->pt(); // FIXME: or which one?
    ptGen_   = ttGenEvent_->singleLepton()->pt();
    eta_     = ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->eta();
    etaAlt_  = patElecs_->at( elecIndex ).closestCtfTrackRef()->eta(); // FIXME: or which one?
    etaGen_  = ttGenEvent_->singleLepton()->eta();
    phi_     = ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->phi();
    phiAlt_  = patElecs_->at( elecIndex ).closestCtfTrackRef()->phi(); // FIXME: or which one?
    phiGen_  = ttGenEvent_->singleLepton()->phi();
  }
  else if ( cat == "UdscJet" ) {
    const std::vector< int > jetLepCombi( ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ) );
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
  }
  else if ( cat == "UdscJetBar" ) {
    const std::vector< int > jetLepCombi( ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ) );
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
  }
  else if ( cat == "BJetHad" ) {
    const std::vector< int > jetLepCombi( ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ) );
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
  }
  else if ( cat == "BJetLep" ) {
    const std::vector< int > jetLepCombi( ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ) );
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
  }
  else if ( cat == "MET" ) {
    pt_     = ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt();
    ptAlt_  = pt_;
    ptGen_  = ttGenEvent_->singleNeutrino()->pt();
    eta_    = ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->eta();
    etaAlt_ = eta_;
    etaGen_ = ttGenEvent_->singleNeutrino()->eta();
    phi_    = ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->phi();
    phiAlt_ = phi_;
    phiGen_ = ttGenEvent_->singleNeutrino()->phi();
  }
  else {
    edm::LogWarning( "AnalyzeHitFit" ) << "Category missmatch: " << cat;
    return;
  }

  unsigned iEta( getEtaBin( iCat, eta_ ) );
  if ( iEta < etaBins_.at( iCat ).size() ) binEta_ = iEta;
  iEta = getEtaBin( iCat, etaAlt_ );
  if ( iEta < etaBins_.at( iCat ).size() ) binEtaAlt_ = iEta;
  iEta = getEtaBin( iCat, etaGen_ );
  if ( iEta < etaBins_.at( iCat ).size() ) binEtaGen_ = iEta;
  iEta = getEtaBin( iCat, eta_, true );
  if ( iEta < etaSymmBins_.at( iCat ).size() ) binEtaSymm_ = iEta;
  iEta = getEtaBin( iCat, etaAlt_, true );
  if ( iEta < etaSymmBins_.at( iCat ).size() ) binEtaSymmAlt_ = iEta;
  iEta = getEtaBin( iCat, etaGen_, true );
  if ( iEta < etaSymmBins_.at( iCat ).size() ) binEtaSymmGen_ = iEta;

}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( AnalyzeHitFit );
