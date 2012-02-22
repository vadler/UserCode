// -*- C++ -*-
//
// Package:    TopMassSemiLeptonic
// Class:      AnalyzeHitFitResolutionFunctions
//
// $Id: AnalyzeHitFitResolutionFunctions.cc,v 1.1 2011/08/31 14:45:53 vadler Exp $
//
/**
  \class    AnalyzeHitFitResolutionFunctions AnalyzeHitFitResolutionFunctions.cc "TopQuarkAnalysis/TopMassSemiLeptonic/plugins/AnalyzeHitFitResolutionFunctions.cc"
  \brief    Fill histograms and n-tuples for HitFit resolution function determination



  \author   Volker Adler
  \version  $Id: AnalyzeHitFitResolutionFunctions.cc,v 1.1 2011/08/31 14:45:53 vadler Exp $
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
#include "DataFormats/PatCandidates/interface/Jet.h"


typedef std::vector< TH2D* >             TH2DVec;
typedef std::map< std::string, TH2DVec > TH2DVecMap;


class AnalyzeHitFitResolutionFunctions : public edm::EDAnalyzer {

  public:

    /// Constructors and Desctructor

    /// Default constructor
    explicit AnalyzeHitFitResolutionFunctions( const edm::ParameterSet & iConfig );

    /// Destructor
    virtual ~AnalyzeHitFitResolutionFunctions() {};

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
    // PAT jets
    edm::Handle< pat::JetCollection > patJets_; // direct access needed to deal with JECs

    /// Configuration parameters
    edm::InputTag ttSemiLeptonicEventTag_;
    bool refGen_;
    bool useSymm_;
    bool useJetEt_;
    bool useMuons_;
    bool useElecs_;
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
    // Kinematic quantities RECO
    TH2DVecMap histos_Reco_;
    // Kinematic quantities RECO inverted
    TH2DVecMap histos_RecoInv_;
    // Kinematic quantities GEN
    TH2DVecMap histos_Gen_;
    // Kinematic quantities GEN inverted
    TH2DVecMap histos_GenInv_;
    // Kinematic quantities RECO, symmetric in eta
    TH2DVecMap histos_RecoSymm_;
    // Kinematic quantities RECO inverted, symmetric in eta
    TH2DVecMap histos_RecoInvSymm_;
    // Kinematic quantities GEN, symmetric in eta
    TH2DVecMap histos_GenSymm_;
    // Kinematic quantities GEN inverted, symmetric in eta
    TH2DVecMap histos_GenInvSymm_;

    /// Private functions

    unsigned getEtaBin( unsigned iCat, double eta, bool symm = false );

    // Fill histograms and n-tuples
    // Main function
    void fill( unsigned iCat, unsigned iProp );
    // Object categories
    void fillLepton( unsigned iCat, unsigned iProp );
    void fillUdscJet( unsigned iCat, unsigned iProp );
    void fillBJet( unsigned iCat, unsigned iProp );
    void fillMET( unsigned iCat, unsigned iProp );
    // One function fills all
    void fillObject( const std::string & index, unsigned iCat, unsigned iProp, double pt, double ptGen, double eta, double etaGen, double phi, double phiGen );

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
AnalyzeHitFitResolutionFunctions::AnalyzeHitFitResolutionFunctions( const edm::ParameterSet & iConfig )
: ttSemiLeptonicEvent_()
, ttGenEvent_()
, patJets_()
, ttSemiLeptonicEventTag_( iConfig.getParameter< edm::InputTag >( "ttSemiLeptonicEvent" ) )
, refGen_( iConfig.getParameter< bool >( "refGen" ) )
, useSymm_( iConfig.getParameter< bool >( "useSymm" ) )
, useJetEt_( iConfig.getParameter< bool >( "useJetEt" ) )
, useMuons_( iConfig.getParameter< bool >( "useMuons" ) )
, useElecs_( iConfig.getParameter< bool >( "useElectrons" ) )
, patJetsTag_( iConfig.getParameter< edm::InputTag >( "patJets" ) )
, jecLevel_( iConfig.getParameter< std::string >( "jecLevel" ) )
{

  // Constants

  objCats_.clear();
  if ( useMuons_ ) objCats_.push_back( "Mu" );
  if ( useElecs_ ) objCats_.push_back( "Elec" );
  objCats_.push_back( "UdscJet" );
  objCats_.push_back( "BJet" );
  objCats_.push_back( "MET" );

  std::vector< std::string > objNames;
  if ( useMuons_ ) objNames.push_back( "muon" );
  if ( useElecs_ ) objNames.push_back( "electron" );
  objNames.push_back( "udscJet" );
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
      edm::LogError( "AnalyzeHitFitResolutionFunctions" ) << objCats_.at( iCat ) << ": less than 2 eta bin bounderies found";
    sstrEta << "  " << objCats_.at( iCat ) << ": ";
    for ( unsigned iEta = 0; iEta < etaBins_.at( iCat ).size() - 1; ++iEta ) {
      sstrEta << etaBins_.at( iCat ).at( iEta ) << ", ";
    }
    sstrEta << etaBins_.at( iCat ).back() << std::endl;
    if ( ptBins_.at( iCat ).size() < 2 )
      edm::LogError( "AnalyzeHitFitResolutionFunctions" ) << objCats_.at( iCat ) << ": less than 2 p_t bin bounderies found";
    std::string cat( "" );
    if ( ( ( cat == "UdscJet" || cat == "BJet" ) && useJetEt_ ) ) cat.append( "E_t" );
    else                                                          cat.append( objCats_.at( iCat ) );
    sstrPt << "  " << cat << ": ";
    for ( unsigned iPt = 0; iPt < ptBins_.at( iCat ).size() - 1; ++iPt ) {
      sstrPt << ptBins_.at( iCat ).at( iPt ) << ", ";
    }
    sstrPt << ptBins_.at( iCat ).back() << std::endl;
  }
  edm::LogInfo( "AnalyzeHitFitResolutionFunctions" ) << "Eta binning\n" << sstrEta.str();
  edm::LogInfo( "AnalyzeHitFitResolutionFunctions" ) << "Pt binning\n" << sstrPt.str();

}


// Begin job
void AnalyzeHitFitResolutionFunctions::beginJob()
{

  edm::Service< TFileService > fileService;

  const std::string xTitleReco( "p_{t}^{RECO} (GeV)" );
  const std::string xTitleGen( "p_{t}^{GEN} (GeV)" );
  std::vector< std::string > yTitles;
  yTitles.push_back( "#Deltap_{t} (GeV)" );
  yTitles.push_back( "#Delta#eta" );
  yTitles.push_back( "#Delta#phi" );
  std::vector< std::string > yTitlesInv;
  yTitlesInv.push_back( "#Delta#frac{1}{p_{t}} (#frac{1}{GeV})" );
  yTitlesInv.push_back( "#Delta#frac{1}{#eta}" );
  yTitlesInv.push_back( "#Delta#frac{1}{#phi}" );
  const std::string zTitle( "events" );

  for ( unsigned iCat = 0; iCat < objCats_.size(); ++iCat ) {
    const std::string cat( objCats_.at( iCat ) );
    const bool useEt( ( cat == "UdscJet" || cat == "BJet" ) && useJetEt_ );
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
    if ( useEt ) histos_PtBins_.back()->SetXTitle( "E_{t} (GeV)" );
    else         histos_PtBins_.back()->SetXTitle( "p_{t} (GeV)" );
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
    data_.back()->Branch( "PtGen"        , &ptGen_        , "ptGen/D" );
    data_.back()->Branch( "EtaGen"       , &etaGen_       , "etaGen/D" );
    data_.back()->Branch( "PhiGen"       , &phiGen_       , "phiGen/D" );
    data_.back()->Branch( "BinEtaGen"    , &binEtaGen_    , "binEtaGen/I" );
    data_.back()->Branch( "BinEtaSymmGen", &binEtaSymmGen_, "binEtaSymmGen/I" );

    for ( unsigned iProp = 0; iProp < kinProps_.size(); ++iProp ) {
      const std::string prop( kinProps_.at( iProp ) );
      std::string xTitleRecoProp( xTitleReco );
      std::string xTitleGenProp( xTitleGen );
      std::string yTitle( yTitles.at( iProp ) );
      std::string yTitleInv( yTitlesInv.at( iProp ) );
      if ( useEt ) {
        boost::replace_all( xTitleRecoProp, "p_{t}", "E_{t}" );
        boost::replace_all( xTitleGenProp, "p_{t}", "E_{t}" );
        if ( prop == "Pt" ) {
          boost::replace_all( yTitle, "p_{t}", "E_{t}" );
          boost::replace_all( yTitleInv, "p_{t}", "E_{t}" );
        }
      }
      TFileDirectory subDir( dir.mkdir( prop.c_str(), "" ) );

      const std::string nameDirReco( "Reco" );
      TFileDirectory subDirReco( subDir.mkdir( nameDirReco.c_str(), "" ) );
      TH2DVec histos_Reco;
      const std::string nameDirRecoInv( "RecoInv" );
      TFileDirectory subDirRecoInv( subDir.mkdir( nameDirRecoInv.c_str(), "" ) );
      TH2DVec histos_RecoInv;
      for ( unsigned iEta = 0; iEta < etaBins_.at( iCat ).size() - 1; ++iEta ) {
        const std::string eta( "Eta" + boost::lexical_cast< std::string >( iEta ) );
        const std::string title( cat + ", " + boost::lexical_cast< std::string >( etaBins_.at( iCat ).at( iEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( iCat ).at( iEta + 1 ) ) );
        TFileDirectory subDirRecoEta( subDirReco.mkdir( eta.c_str(), title.c_str() ) );
        const std::string nameRecoEta( cat + "_" + prop + "_Reco_" + eta );
        histos_Reco.push_back( subDirRecoEta.make< TH2D >( nameRecoEta.c_str(), title.c_str(), ptBins_.at( iCat ).size() - 1, ptBins_.at( iCat ).data(), propBins_.at( iCat).at( iProp ), -propMaxs_.at( iCat).at( iProp ), propMaxs_.at( iCat).at( iProp ) ) );
        histos_Reco.back()->SetXTitle( xTitleRecoProp.c_str() );
        histos_Reco.back()->SetYTitle( yTitle.c_str() );
        histos_Reco.back()->SetZTitle( zTitle.c_str() );
        TFileDirectory subDirRecoInvEta( subDirRecoInv.mkdir( eta.c_str(), title.c_str() ) );
        const std::string nameRecoInvEta( cat + "_" + prop + "_RecoInv_" + eta );
        histos_RecoInv.push_back( subDirRecoInvEta.make< TH2D >( nameRecoInvEta.c_str(), title.c_str(), ptBins_.at( iCat ).size() - 1, ptBins_.at( iCat ).data(), propInvBins_.at( iCat).at( iProp ), -propInvMaxs_.at( iCat).at( iProp ), propInvMaxs_.at( iCat).at( iProp ) ) );
        histos_RecoInv.back()->SetXTitle( xTitleRecoProp.c_str() );
        histos_RecoInv.back()->SetYTitle( yTitleInv.c_str() );
        histos_RecoInv.back()->SetZTitle( zTitle.c_str() );
      }
      histos_Reco_[ cat + "_" + prop ] = histos_Reco;
      histos_RecoInv_[ cat + "_" + prop ] = histos_RecoInv;

      if ( refGen_ ) {
        const std::string nameDirGen( "Gen" );
        TFileDirectory subDirGen( subDir.mkdir( nameDirGen.c_str(), "" ) );
        TH2DVec histos_Gen;
        const std::string nameDirGenInv( "GenInv" );
        TFileDirectory subDirGenInv( subDir.mkdir( nameDirGenInv.c_str(), "" ) );
        TH2DVec histos_GenInv;
        for ( unsigned iEta = 0; iEta < etaBins_.at( iCat ).size() - 1; ++iEta ) {
          const std::string eta( "Eta" + boost::lexical_cast< std::string >( iEta ) );
          const std::string title( cat + ", " + boost::lexical_cast< std::string >( etaBins_.at( iCat ).at( iEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( iCat ).at( iEta + 1 ) ) );
          TFileDirectory subDirGenEta( subDirGen.mkdir( eta.c_str(), title.c_str() ) );
          const std::string nameGenEta( cat + "_" + prop + "_Gen_" + eta );
          histos_Gen.push_back( subDirGenEta.make< TH2D >( nameGenEta.c_str(), title.c_str(), ptBins_.at( iCat ).size() - 1, ptBins_.at( iCat ).data(), propBins_.at( iCat).at( iProp ), -propMaxs_.at( iCat).at( iProp ), propMaxs_.at( iCat).at( iProp ) ) );
          histos_Gen.back()->SetXTitle( xTitleGenProp.c_str() );
          histos_Gen.back()->SetYTitle( yTitle.c_str() );
          histos_Gen.back()->SetZTitle( zTitle.c_str() );
          TFileDirectory subDirGenInvEta( subDirGenInv.mkdir( eta.c_str(), title.c_str() ) );
          const std::string nameGenInvEta( cat + "_" + prop + "_GenInv_" + eta );
          histos_GenInv.push_back( subDirGenInvEta.make< TH2D >( nameGenInvEta.c_str(), title.c_str(), ptBins_.at( iCat ).size() - 1, ptBins_.at( iCat ).data(), propInvBins_.at( iCat).at( iProp ), -propInvMaxs_.at( iCat).at( iProp ), propInvMaxs_.at( iCat).at( iProp ) ) );
          histos_GenInv.back()->SetXTitle( xTitleGenProp.c_str() );
          histos_GenInv.back()->SetYTitle( yTitleInv.c_str() );
          histos_GenInv.back()->SetZTitle( zTitle.c_str() );
        }
        histos_Gen_[ cat + "_" + prop ] = histos_Gen;
        histos_GenInv_[ cat + "_" + prop ] = histos_GenInv;
      }

      if ( useSymm_ ) {
        const std::string nameDirRecoSymm( "RecoSymm" );
        TFileDirectory subDirRecoSymm( subDir.mkdir( nameDirRecoSymm.c_str(), "" ) );
        TH2DVec histos_RecoSymm;
        const std::string nameDirRecoInvSymm( "RecoInvSymm" );
        TFileDirectory subDirRecoInvSymm( subDir.mkdir( nameDirRecoInvSymm.c_str(), "" ) );
        TH2DVec histos_RecoInvSymm;
        for ( unsigned iEtaSymm = 0; iEtaSymm < etaSymmBins_.at( iCat ).size() - 1; ++iEtaSymm ) {
          const std::string etaSymm( "Eta" + boost::lexical_cast< std::string >( iEtaSymm ) );
          const std::string title( cat + ", " + boost::lexical_cast< std::string >( etaSymmBins_.at( iCat ).at( iEtaSymm ) ) + " #leq |#eta| #leq " + boost::lexical_cast< std::string >( etaSymmBins_.at( iCat ).at( iEtaSymm + 1 ) ) );
          TFileDirectory subDirRecoSymmEta( subDirRecoSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
          const std::string nameRecoSymmEta( cat + "_" + prop + "_RecoSymm_" + etaSymm );
          histos_RecoSymm.push_back( subDirRecoSymmEta.make< TH2D >( nameRecoSymmEta.c_str(), title.c_str(), ptBins_.at( iCat ).size() - 1, ptBins_.at( iCat ).data(), propBins_.at( iCat).at( iProp ), -propMaxs_.at( iCat).at( iProp ), propMaxs_.at( iCat).at( iProp ) ) );
          histos_RecoSymm.back()->SetXTitle( xTitleRecoProp.c_str() );
          histos_RecoSymm.back()->SetYTitle( yTitle.c_str() );
          histos_RecoSymm.back()->SetZTitle( zTitle.c_str() );
          TFileDirectory subDirRecoInvSymmEta( subDirRecoInvSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
          const std::string nameRecoInvSymmEta( cat + "_" + prop + "_RecoInvSymm_" + etaSymm );
          histos_RecoInvSymm.push_back( subDirRecoInvSymmEta.make< TH2D >( nameRecoInvSymmEta.c_str(), title.c_str(), ptBins_.at( iCat ).size() - 1, ptBins_.at( iCat ).data(), propInvBins_.at( iCat).at( iProp ), -propInvMaxs_.at( iCat).at( iProp ), propInvMaxs_.at( iCat).at( iProp ) ) );
          histos_RecoInvSymm.back()->SetXTitle( xTitleRecoProp.c_str() );
          histos_RecoInvSymm.back()->SetYTitle( yTitleInv.c_str() );
          histos_RecoInvSymm.back()->SetZTitle( zTitle.c_str() );
        }
        histos_RecoSymm_[ cat + "_" + prop ] = histos_RecoSymm;
        histos_RecoInvSymm_[ cat + "_" + prop ] = histos_RecoInvSymm;

        if ( refGen_ ) {
          const std::string nameDirGenSymm( "GenSymm" );
          TFileDirectory subDirGenSymm( subDir.mkdir( nameDirGenSymm.c_str(), "" ) );
          TH2DVec histos_GenSymm;
          const std::string nameDirGenInvSymm( "GenInvSymm" );
          TFileDirectory subDirGenInvSymm( subDir.mkdir( nameDirGenInvSymm.c_str(), "" ) );
          TH2DVec histos_GenInvSymm;
          for ( unsigned iEtaSymm = 0; iEtaSymm < etaSymmBins_.at( iCat ).size() - 1; ++iEtaSymm ) {
            const std::string etaSymm( "Eta" + boost::lexical_cast< std::string >( iEtaSymm ) );
            const std::string title( cat + ", " + boost::lexical_cast< std::string >( etaSymmBins_.at( iCat ).at( iEtaSymm ) ) + " #leq |#eta| #leq " + boost::lexical_cast< std::string >( etaSymmBins_.at( iCat ).at( iEtaSymm + 1 ) ) );
            TFileDirectory subDirGenSymmEta( subDirGenSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
            const std::string nameGenSymmEta( cat + "_" + prop + "_GenSymm_" + etaSymm );
            histos_GenSymm.push_back( subDirGenSymmEta.make< TH2D >( nameGenSymmEta.c_str(), title.c_str(), ptBins_.at( iCat ).size() - 1, ptBins_.at( iCat ).data(), propBins_.at( iCat).at( iProp ), -propMaxs_.at( iCat).at( iProp ), propMaxs_.at( iCat).at( iProp ) ) );
            histos_GenSymm.back()->SetXTitle( xTitleGenProp.c_str() );
            histos_GenSymm.back()->SetYTitle( yTitle.c_str() );
            histos_GenSymm.back()->SetZTitle( zTitle.c_str() );
            TFileDirectory subDirGenInvSymmEta( subDirGenInvSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
            const std::string nameGenInvSymmEta( cat + "_" + prop + "_GenInvSymm_" + etaSymm );
            histos_GenInvSymm.push_back( subDirGenInvSymmEta.make< TH2D >( nameGenInvSymmEta.c_str(), title.c_str(), ptBins_.at( iCat ).size() - 1, ptBins_.at( iCat ).data(), propInvBins_.at( iCat).at( iProp ), -propInvMaxs_.at( iCat).at( iProp ), propInvMaxs_.at( iCat).at( iProp ) ) );
            histos_GenInvSymm.back()->SetXTitle( xTitleGenProp.c_str() );
            histos_GenInvSymm.back()->SetYTitle( yTitleInv.c_str() );
            histos_GenInvSymm.back()->SetZTitle( zTitle.c_str() );
          }
          histos_GenSymm_[ cat + "_" + prop ] = histos_GenSymm;
          histos_GenInvSymm_[ cat + "_" + prop ] = histos_GenInvSymm;
        }

      }

    }

  }

}


// Event loop
void AnalyzeHitFitResolutionFunctions::analyze( const edm::Event & iEvent, const edm::EventSetup & iSetup )
{

  // Reset
  ttGenEvent_ = 0;

  // TQAF semi-leptonic event
  iEvent.getByLabel( ttSemiLeptonicEventTag_, ttSemiLeptonicEvent_ );

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

      // Valid TTbar MC matching
      if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kGenMatch ) ) {

        for ( unsigned iCat = 0; iCat < objCats_.size(); ++iCat ) {
          for ( unsigned iProp = 0; iProp < kinProps_.size(); ++iProp ) {
            fill( iCat, iProp );
          }
          // Fill tree
          data_.at( iCat )->Fill(); // works only after calling 'fill' for all kinematic properties
        }

        ++filledEvents_;

      } // Valid TTbar MC matching
      else edm::LogInfo( "AnalyzeHitFitResolutionFunctions" ) << "...no valid MC match";

    } // Semi-leptonic signal event
    else edm::LogInfo( "AnalyzeHitFitResolutionFunctions" ) << "...no signal event";

  } // MC

}


// End job
void AnalyzeHitFitResolutionFunctions::endJob()
{

  edm::LogInfo( "AnalyzeHitFitResolutionFunctions" ) << "Filled events: " << filledEvents_;

}


unsigned AnalyzeHitFitResolutionFunctions::getEtaBin( unsigned iCat, double eta, bool symm )
{

  std::vector< double > etaBins( symm ? etaSymmBins_.at( iCat ) : etaBins_.at( iCat ) );
  if ( symm ) eta = fabs( eta );
  for ( unsigned iEta = 0; iEta < etaBins.size() - 1; ++iEta ) {
    if ( etaBins.at( iEta ) <= eta && eta < etaBins.at( iEta + 1 ) ) return iEta;
  }

  return etaBins.size();

}


void AnalyzeHitFitResolutionFunctions::fill( unsigned iCat, unsigned iProp )
{

  const std::string cat( objCats_.at( iCat ) );
  if ( ttGenEvent_->isSemiLeptonic( WDecay::kMuon ) && cat == "Mu" ) {
    fillLepton( iCat, iProp );
  }
  else if ( ttGenEvent_->isSemiLeptonic( WDecay::kElec ) && cat == "Elec" ) {
    fillLepton( iCat, iProp );
  }
  else if ( cat == "UdscJet" ) {
    fillUdscJet( iCat, iProp );
  }
  else if ( cat == "BJet" ) {
    fillBJet( iCat, iProp );
  }
  else if ( cat == "MET" ) {
    fillMET( iCat, iProp );
  }
  else {
    edm::LogWarning( "AnalyzeHitFitResolutionFunctions" ) << "Category missmatch: " << cat;
  }

}


void AnalyzeHitFitResolutionFunctions::fillLepton( unsigned iCat, unsigned iProp )
{

  const std::string cat( objCats_.at( iCat ) );
  const std::string index( cat + "_" + kinProps_.at( iProp ) );

  double pt( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt() );
  double ptGen( ttGenEvent_->singleLepton()->pt() );
  double eta( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->eta() );
  double etaGen( ttGenEvent_->singleLepton()->eta() );
  double phi( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->phi() );
  double phiGen( ttGenEvent_->singleLepton()->phi() );

  fillObject( index, iCat, iProp, pt, ptGen, eta, etaGen, phi, phiGen );

}


void AnalyzeHitFitResolutionFunctions::fillUdscJet( unsigned iCat, unsigned iProp )
{

  const std::string cat( objCats_.at( iCat ) );
  const std::string index( cat + "_" + kinProps_.at( iProp ) );
  const std::vector< int > jetLepCombi( ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ) );

  pat::Jet qJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LightQ ) ).correctedJet( jecLevel_, "uds" ) );

  double pt( qJet.pt() );
  double ptGen( ttGenEvent_->hadronicDecayQuark()->pt() );
  double eta( qJet.eta() );
  double etaGen( ttGenEvent_->hadronicDecayQuark()->eta() );
  double phi( qJet.phi() );
  double phiGen( ttGenEvent_->hadronicDecayQuark()->phi() );
  if ( useJetEt_ ) {
    pt    = qJet.et();
    ptGen = ttGenEvent_->hadronicDecayQuark()->et();
  }

  fillObject( index, iCat, iProp, pt, ptGen, eta, etaGen, phi, phiGen );

  pat::Jet qBarJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LightQBar ) ).correctedJet( jecLevel_, "uds" ) );

  pt     = qBarJet.pt();
  ptGen  = ttGenEvent_->hadronicDecayQuarkBar()->pt();
  eta    = qBarJet.eta();
  etaGen = ttGenEvent_->hadronicDecayQuarkBar()->eta();
  phi    = qBarJet.phi();
  phiGen = ttGenEvent_->hadronicDecayQuarkBar()->phi();
  if ( useJetEt_ ) {
    pt    = qBarJet.et();
    ptGen = ttGenEvent_->hadronicDecayQuarkBar()->et();
  }

  fillObject( index, iCat, iProp, pt, ptGen, eta, etaGen, phi, phiGen );

}


void AnalyzeHitFitResolutionFunctions::fillBJet( unsigned iCat, unsigned iProp )
{

  const std::string cat( objCats_.at( iCat ) );
  const std::string index( cat + "_" + kinProps_.at( iProp ) );
  const std::vector< int > jetLepCombi( ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ) );

  pat::Jet hadBJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::HadB ) ).correctedJet( jecLevel_, "bottom" ) );

  double pt( hadBJet.pt() );
  double ptGen( ttGenEvent_->hadronicDecayB()->pt() );
  double eta( hadBJet.eta() );
  double etaGen( ttGenEvent_->hadronicDecayB()->eta() );
  double phi( hadBJet.phi() );
  double phiGen( ttGenEvent_->hadronicDecayB()->phi() );
  if ( useJetEt_ ) {
    pt    = hadBJet.et();
    ptGen = ttGenEvent_->hadronicDecayB()->et();
  }

  fillObject( index, iCat, iProp, pt, ptGen, eta, etaGen, phi, phiGen );

  pat::Jet lepBJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LepB ) ).correctedJet( jecLevel_, "bottom" ) );

  pt     = lepBJet.pt();
  ptGen  = ttGenEvent_->leptonicDecayB()->pt();
  eta    = lepBJet.eta();
  etaGen = ttGenEvent_->leptonicDecayB()->eta();
  phi    = lepBJet.phi();
  phiGen = ttGenEvent_->leptonicDecayB()->phi();
  if ( useJetEt_ ) {
    pt    = lepBJet.et();
    ptGen = ttGenEvent_->leptonicDecayB()->et();
  }

  fillObject( index, iCat, iProp, pt, ptGen, eta, etaGen, phi, phiGen );

}


void AnalyzeHitFitResolutionFunctions::fillMET( unsigned iCat, unsigned iProp )
{

  const std::string cat( objCats_.at( iCat ) );
  const std::string index( cat + "_" + kinProps_.at( iProp ) );

  double pt( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt() );
  double ptGen( ttGenEvent_->singleNeutrino()->pt() );
  double eta( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->eta() );
  double etaGen( ttGenEvent_->singleNeutrino()->eta() );
  double phi( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->phi() );
  double phiGen( ttGenEvent_->singleNeutrino()->phi() );

  fillObject( index, iCat, iProp, pt, ptGen, eta, etaGen, phi, phiGen );

}


void AnalyzeHitFitResolutionFunctions::fillObject( const std::string & index, unsigned iCat, unsigned iProp, double pt, double ptGen, double eta, double etaGen, double phi, double phiGen )
{

  // Fill n-tuple
   pt_          = pt;
   eta_           = eta;
   phi_           = phi;
   binEta_        = -1; // initialise
   binEtaSymm_    = -1; // initialise
   ptGen_       = ptGen;
   etaGen_        = etaGen;
   phiGen_        = phiGen;
   binEtaGen_     = -1; // initialise
   binEtaSymmGen_ = -1; // initialise

  const std::string prop( kinProps_.at( iProp ) );
  unsigned iEta( getEtaBin( iCat, eta ) );

  if ( iEta < etaBins_.at( iCat ).size() ) {
    binEta_ = iEta;
    if ( prop == "Pt" ) {
      histos_Reco_[ index ].at( iEta )->Fill( pt, pt - ptGen );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, 1. / pt - 1. / ptGen );
    }
    else if ( prop == "Eta" ) {
      histos_Reco_[ index ].at( iEta )->Fill( pt, eta - etaGen );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, eta - etaGen );
    }
    else if ( prop == "Phi" ) {
      histos_Reco_[ index ].at( iEta )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( phi - phiGen ) );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( phi - phiGen ) );
    }
  }

  if ( refGen_ ) {

    iEta = getEtaBin( iCat, etaGen );

    if ( iEta < etaBins_.at( iCat ).size() ) {
      binEtaGen_ = iEta;
      if ( prop == "Pt" ) {
        histos_Gen_[ index ].at( iEta )->Fill( ptGen, ptGen - pt );
        histos_GenInv_[ index ].at( iEta )->Fill( ptGen, 1. / ptGen - 1. / pt );
      }
      else if ( prop == "Eta" ) {
        histos_Gen_[ index ].at( iEta )->Fill( ptGen, etaGen - eta );
        histos_GenInv_[ index ].at( iEta )->Fill( ptGen, etaGen - eta );
      }
      else if ( prop == "Phi" ) {
        histos_Gen_[ index ].at( iEta )->Fill( ptGen, ROOT::Math::VectorUtil::Phi_mpi_pi( phiGen - phi ) );
        histos_GenInv_[ index ].at( iEta )->Fill( ptGen, ROOT::Math::VectorUtil::Phi_mpi_pi( phiGen - phi ) );
      }
    }
  }

  if ( useSymm_ ) {

    unsigned iEtaSymm( getEtaBin( iCat, eta, true ) );

    if ( iEtaSymm < etaSymmBins_.at( iCat ).size() ) {
      binEtaSymm_ = iEtaSymm;
      if ( prop == "Pt" ) {
        histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, pt - ptGen );
        histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / pt - 1. / ptGen );
      }
      else if ( prop == "Eta" ) {
        histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, eta - etaGen );
        histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, eta - etaGen );
      }
      else if ( prop == "Phi" ) {
        histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( phi - phiGen ) );
        histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( phi - phiGen ) );
      }
    }

    if ( refGen_ ) {

      iEtaSymm = getEtaBin( iCat, etaGen, true );

      if ( iEtaSymm < etaSymmBins_.at( iCat ).size() ) {
        binEtaSymmGen_ = iEtaSymm;
        if ( prop == "Pt" ) {
          histos_GenSymm_[ index ].at( iEtaSymm )->Fill( ptGen, ptGen - pt );
          histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( ptGen, 1. / ptGen - 1. / pt );
        }
        else if ( prop == "Eta" ) {
          histos_GenSymm_[ index ].at( iEtaSymm )->Fill( ptGen, etaGen - eta );
          histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( ptGen, etaGen - eta );
        }
        else if ( prop == "Phi" ) {
          histos_GenSymm_[ index ].at( iEtaSymm )->Fill( ptGen, ROOT::Math::VectorUtil::Phi_mpi_pi( phiGen - phi ) );
          histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( ptGen, ROOT::Math::VectorUtil::Phi_mpi_pi( phiGen - phi ) );
        }
      }
    }

  }

}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( AnalyzeHitFitResolutionFunctions );
