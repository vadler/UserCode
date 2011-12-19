// -*- C++ -*-
//
// Package:    TopMassSemiLeptonic
// Class:      AnalyzeHitFitResolutionFunctions
//
// $Id: AnalyzeHitFitResolutionFunctions.cc,v 1.1 2011/08/31 14:45:53 vadler Exp $
//
/**
  \class    AnalyzeHitFitResolutionFunctions AnalyzeHitFitResolutionFunctions.cc "TopQuarkAnalysis/TopMassSemiLeptonic/plugins/AnalyzeHitFitResolutionFunctions.cc"
  \brief    Fill histograms for HitFit resolution function determination



  \author   Volker Adler
  \version  $Id: AnalyzeHitFitResolutionFunctions.cc,v 1.1 2011/08/31 14:45:53 vadler Exp $
*/


#include <vector>
#include <map>
#include <string>

#include "TH1D.h"
#include "TH2D.h"

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
    virtual void endJob() {};

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

    /// Histograms
    // Binning
    std::vector< TH1D* > histos_EtaBins_;
    std::vector< TH1D* > histos_PtBins_;
    // Kinematic quantities RECO
    TH2DVecMap histos_Reco_;
    // Kinematic quantities GEN
    TH2DVecMap histos_Gen_;
    // Kinematic quantities RECO inverted
    TH2DVecMap histos_RecoInv_;
    // Kinematic quantities GEN inverted
    TH2DVecMap histos_GenInv_;
    // Kinematic quantities RECO, symmetric in eta
    TH2DVecMap histos_RecoSymm_;
    // Kinematic quantities GEN, symmetric in eta
    TH2DVecMap histos_GenSymm_;
    // Kinematic quantities RECO inverted, symmetric in eta
    TH2DVecMap histos_RecoInvSymm_;
    // Kinematic quantities GEN inverted, symmetric in eta
    TH2DVecMap histos_GenInvSymm_;

    /// Private functions

    unsigned getEtaBin( unsigned iCat, double eta, bool symm = false );

    // Fill histograms
    // Main function
    void fill( unsigned iCat, unsigned iProp );
    // Object categories
    void fillLepton( unsigned iCat, unsigned iProp );
    void fillUdscJet( unsigned iCat, unsigned iProp );
    void fillBJet( unsigned iCat, unsigned iProp );
    void fillMET( unsigned iCat, unsigned iProp );

};


#include <cassert>
#include <iostream>
#include <sstream>

#include "Math/GenVector/VectorUtil.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "AnalysisDataFormats/TopObjects/interface/TtSemiLepEvtPartons.h"
#include "TopQuarkAnalysis/TopHitFit/interface/EtaDepResolution.h"

#include "TopQuarkAnalysis/TopMassSemiLeptonic/interface/Helpers.h"


// Default constructor
AnalyzeHitFitResolutionFunctions::AnalyzeHitFitResolutionFunctions( const edm::ParameterSet & iConfig )
: ttSemiLeptonicEvent_()
, ttGenEvent_()
, patJets_()
, ttSemiLeptonicEventTag_( iConfig.getParameter< edm::InputTag >( "ttSemiLeptonicEvent" ) )
, useMuons_( iConfig.getParameter< bool >( "useMuons" ) )
, useElecs_( iConfig.getParameter< bool >( "useElectrons" ) )
, patJetsTag_( iConfig.getParameter< edm::InputTag >( "patJets" ) )
, jecLevel_( iConfig.getParameter< std::string >( "jecLevel" ) )
{

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
    sstrPt << "  " << objCats_.at( iCat ) << ": ";
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

    for ( unsigned iProp = 0; iProp < kinProps_.size(); ++iProp ) {
      const std::string prop( kinProps_.at( iProp ) );
      const std::string yTitle( yTitles.at( iProp ) );
      const std::string yTitleInv( yTitlesInv.at( iProp ) );
      TFileDirectory subDir( dir.mkdir( prop.c_str(), "" ) );

      const std::string nameDirReco( "Reco" );
      TFileDirectory subDirReco( subDir.mkdir( nameDirReco.c_str(), "" ) );
      TH2DVec histos_Reco;
      const std::string nameDirGen( "Gen" );
      TFileDirectory subDirGen( subDir.mkdir( nameDirGen.c_str(), "" ) );
      TH2DVec histos_Gen;
      const std::string nameDirRecoInv( "RecoInv" );
      TFileDirectory subDirRecoInv( subDir.mkdir( nameDirRecoInv.c_str(), "" ) );
      TH2DVec histos_RecoInv;
      const std::string nameDirGenInv( "GenInv" );
      TFileDirectory subDirGenInv( subDir.mkdir( nameDirGenInv.c_str(), "" ) );
      TH2DVec histos_GenInv;
      for ( unsigned iEta = 0; iEta < etaBins_.at( iCat ).size() - 1; ++iEta ) {
        const std::string eta( "Eta" + my::helpers::to_string( iEta ) );
        const std::string title( cat + ", " + my::helpers::to_string( etaBins_.at( iCat ).at( iEta ) ) + " #leq #eta #leq " + my::helpers::to_string( etaBins_.at( iCat ).at( iEta + 1 ) ) );
        TFileDirectory subDirRecoEta( subDirReco.mkdir( eta.c_str(), title.c_str() ) );
        const std::string nameRecoEta( cat + "_" + prop + "_Reco_" + eta );
        histos_Reco.push_back( subDirRecoEta.make< TH2D >( nameRecoEta.c_str(), title.c_str(), ptBins_.at( iCat ).size() - 1, ptBins_.at( iCat ).data(), propBins_.at( iCat).at( iProp ), -propMaxs_.at( iCat).at( iProp ), propMaxs_.at( iCat).at( iProp ) ) );
        histos_Reco.back()->SetXTitle( xTitleReco.c_str() );
        histos_Reco.back()->SetYTitle( yTitle.c_str() );
        histos_Reco.back()->SetZTitle( zTitle.c_str() );
        TFileDirectory subDirGenEta( subDirGen.mkdir( eta.c_str(), title.c_str() ) );
        const std::string nameGenEta( cat + "_" + prop + "_Gen_" + eta );
        histos_Gen.push_back( subDirGenEta.make< TH2D >( nameGenEta.c_str(), title.c_str(), ptBins_.at( iCat ).size() - 1, ptBins_.at( iCat ).data(), propBins_.at( iCat).at( iProp ), -propMaxs_.at( iCat).at( iProp ), propMaxs_.at( iCat).at( iProp ) ) );
        histos_Gen.back()->SetXTitle( xTitleGen.c_str() );
        histos_Gen.back()->SetYTitle( yTitle.c_str() );
        histos_Gen.back()->SetZTitle( zTitle.c_str() );
        TFileDirectory subDirRecoInvEta( subDirRecoInv.mkdir( eta.c_str(), title.c_str() ) );
        const std::string nameRecoInvEta( cat + "_" + prop + "_RecoInv_" + eta );
        histos_RecoInv.push_back( subDirRecoInvEta.make< TH2D >( nameRecoInvEta.c_str(), title.c_str(), ptBins_.at( iCat ).size() - 1, ptBins_.at( iCat ).data(), propInvBins_.at( iCat).at( iProp ), -propInvMaxs_.at( iCat).at( iProp ), propInvMaxs_.at( iCat).at( iProp ) ) );
        histos_RecoInv.back()->SetXTitle( xTitleReco.c_str() );
        histos_RecoInv.back()->SetYTitle( yTitleInv.c_str() );
        histos_RecoInv.back()->SetZTitle( zTitle.c_str() );
        TFileDirectory subDirGenInvEta( subDirGenInv.mkdir( eta.c_str(), title.c_str() ) );
        const std::string nameGenInvEta( cat + "_" + prop + "_GenInv_" + eta );
        histos_GenInv.push_back( subDirGenInvEta.make< TH2D >( nameGenInvEta.c_str(), title.c_str(), ptBins_.at( iCat ).size() - 1, ptBins_.at( iCat ).data(), propInvBins_.at( iCat).at( iProp ), -propInvMaxs_.at( iCat).at( iProp ), propInvMaxs_.at( iCat).at( iProp ) ) );
        histos_GenInv.back()->SetXTitle( xTitleGen.c_str() );
        histos_GenInv.back()->SetYTitle( yTitleInv.c_str() );
        histos_GenInv.back()->SetZTitle( zTitle.c_str() );
      }
      histos_Reco_[ cat + "_" + prop ] = histos_Reco;
      histos_Gen_[ cat + "_" + prop ] = histos_Gen;
      histos_RecoInv_[ cat + "_" + prop ] = histos_RecoInv;
      histos_GenInv_[ cat + "_" + prop ] = histos_GenInv;

      const std::string nameDirRecoSymm( "RecoSymm" );
      TFileDirectory subDirRecoSymm( subDir.mkdir( nameDirRecoSymm.c_str(), "" ) );
      TH2DVec histos_RecoSymm;
      const std::string nameDirGenSymm( "GenSymm" );
      TFileDirectory subDirGenSymm( subDir.mkdir( nameDirGenSymm.c_str(), "" ) );
      TH2DVec histos_GenSymm;
      const std::string nameDirRecoInvSymm( "RecoInvSymm" );
      TFileDirectory subDirRecoInvSymm( subDir.mkdir( nameDirRecoInvSymm.c_str(), "" ) );
      TH2DVec histos_RecoInvSymm;
      const std::string nameDirGenInvSymm( "GenInvSymm" );
      TFileDirectory subDirGenInvSymm( subDir.mkdir( nameDirGenInvSymm.c_str(), "" ) );
      TH2DVec histos_GenInvSymm;
      for ( unsigned iEtaSymm = 0; iEtaSymm < etaSymmBins_.at( iCat ).size() - 1; ++iEtaSymm ) {
        const std::string etaSymm( "Eta" + my::helpers::to_string( iEtaSymm ) );
        const std::string title( cat + ", " + my::helpers::to_string( etaSymmBins_.at( iCat ).at( iEtaSymm ) ) + " #leq |#eta| #leq " + my::helpers::to_string( etaSymmBins_.at( iCat ).at( iEtaSymm + 1 ) ) );
        TFileDirectory subDirRecoSymmEta( subDirRecoSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
        const std::string nameRecoSymmEta( cat + "_" + prop + "_RecoSymm_" + etaSymm );
        histos_RecoSymm.push_back( subDirRecoSymmEta.make< TH2D >( nameRecoSymmEta.c_str(), title.c_str(), ptBins_.at( iCat ).size() - 1, ptBins_.at( iCat ).data(), propBins_.at( iCat).at( iProp ), -propMaxs_.at( iCat).at( iProp ), propMaxs_.at( iCat).at( iProp ) ) );
        histos_RecoSymm.back()->SetXTitle( xTitleReco.c_str() );
        histos_RecoSymm.back()->SetYTitle( yTitle.c_str() );
        histos_RecoSymm.back()->SetZTitle( zTitle.c_str() );
        TFileDirectory subDirGenSymmEta( subDirGenSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
        const std::string nameGenSymmEta( cat + "_" + prop + "_GenSymm_" + etaSymm );
        histos_GenSymm.push_back( subDirGenSymmEta.make< TH2D >( nameGenSymmEta.c_str(), title.c_str(), ptBins_.at( iCat ).size() - 1, ptBins_.at( iCat ).data(), propBins_.at( iCat).at( iProp ), -propMaxs_.at( iCat).at( iProp ), propMaxs_.at( iCat).at( iProp ) ) );
        histos_GenSymm.back()->SetXTitle( xTitleGen.c_str() );
        histos_GenSymm.back()->SetYTitle( yTitle.c_str() );
        histos_GenSymm.back()->SetZTitle( zTitle.c_str() );
        TFileDirectory subDirRecoInvSymmEta( subDirRecoInvSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
        const std::string nameRecoInvSymmEta( cat + "_" + prop + "_RecoInvSymm_" + etaSymm );
        histos_RecoInvSymm.push_back( subDirRecoInvSymmEta.make< TH2D >( nameRecoInvSymmEta.c_str(), title.c_str(), ptBins_.at( iCat ).size() - 1, ptBins_.at( iCat ).data(), propInvBins_.at( iCat).at( iProp ), -propInvMaxs_.at( iCat).at( iProp ), propInvMaxs_.at( iCat).at( iProp ) ) );
        histos_RecoInvSymm.back()->SetXTitle( xTitleReco.c_str() );
        histos_RecoInvSymm.back()->SetYTitle( yTitleInv.c_str() );
        histos_RecoInvSymm.back()->SetZTitle( zTitle.c_str() );
        TFileDirectory subDirGenInvSymmEta( subDirGenInvSymm.mkdir( etaSymm.c_str(), title.c_str() ) );
        const std::string nameGenInvSymmEta( cat + "_" + prop + "_GenInvSymm_" + etaSymm );
        histos_GenInvSymm.push_back( subDirGenInvSymmEta.make< TH2D >( nameGenInvSymmEta.c_str(), title.c_str(), ptBins_.at( iCat ).size() - 1, ptBins_.at( iCat ).data(), propInvBins_.at( iCat).at( iProp ), -propInvMaxs_.at( iCat).at( iProp ), propInvMaxs_.at( iCat).at( iProp ) ) );
        histos_GenInvSymm.back()->SetXTitle( xTitleGen.c_str() );
        histos_GenInvSymm.back()->SetYTitle( yTitleInv.c_str() );
        histos_GenInvSymm.back()->SetZTitle( zTitle.c_str() );
      }
      histos_RecoSymm_[ cat + "_" + prop ] = histos_RecoSymm;
      histos_GenSymm_[ cat + "_" + prop ] = histos_GenSymm;
      histos_RecoInvSymm_[ cat + "_" + prop ] = histos_RecoInvSymm;
      histos_GenInvSymm_[ cat + "_" + prop ] = histos_GenInvSymm;

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
        }

      } // Valid TTbar MC matching

    } // Semi-leptonic signal event

  } // MC

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
  if ( ( ttGenEvent_->isSemiLeptonic( WDecay::kMuon ) && cat == "Mu" ) || ( ttGenEvent_->isSemiLeptonic( WDecay::kElec ) && cat == "Elec" ) ) {
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
  const std::string prop( kinProps_.at( iProp ) );
  const std::string index( cat + "_" + prop );

  double eta( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->eta() );
  double pt( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt() );
  unsigned iEta( getEtaBin( iCat, eta ) );
  unsigned iEtaSymm( getEtaBin( iCat, eta, true ) );
  if ( iEta < etaBins_.at( iCat ).size() && iEtaSymm < etaSymmBins_.at( iCat ).size() ) {
    if ( prop == "Pt" ) {
      histos_Reco_[ index ].at( iEta )->Fill( pt, pt - ttGenEvent_->singleLepton()->pt() );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, 1. / pt - 1. / ttGenEvent_->singleLepton()->pt() );
      histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, pt - ttGenEvent_->singleLepton()->pt() );
      histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / pt - 1. / ttGenEvent_->singleLepton()->pt() );
    }
    else if ( prop == "Eta" ) {
      histos_Reco_[ index ].at( iEta )->Fill( pt, eta - ttGenEvent_->singleLepton()->eta() );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, 1. / eta - 1. / ttGenEvent_->singleLepton()->eta() );
      histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, eta - ttGenEvent_->singleLepton()->eta() );
      histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / eta - 1. / ttGenEvent_->singleLepton()->eta() );
    }
    else if ( prop == "Phi" ) {
      const double phi( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->phi() );
      histos_Reco_[ index ].at( iEta )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( phi - ttGenEvent_->singleLepton()->phi() ) );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, 1. / phi - 1. / ttGenEvent_->singleLepton()->phi() );
      histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( phi - ttGenEvent_->singleLepton()->phi() ) );
      histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / phi - 1. / ttGenEvent_->singleLepton()->phi() );
    }
  }

  eta = ttGenEvent_->singleLepton()->eta();
  pt  = ttGenEvent_->singleLepton()->pt();
  iEta     = getEtaBin( iCat, eta );
  iEtaSymm = getEtaBin( iCat, eta, true );
  if ( iEta < etaBins_.at( iCat ).size() && iEtaSymm < etaSymmBins_.at( iCat ).size() ) {
    if ( prop == "Pt" ) {
      histos_Gen_[ index ].at( iEta )->Fill( pt, ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt() - pt );
      histos_GenInv_[ index ].at( iEta )->Fill( pt, 1. / ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt() - 1. / pt );
      histos_GenSymm_[ index ].at( iEtaSymm )->Fill( pt, ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt() - pt );
      histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt() - 1. / pt );
    }
    else if ( prop == "Eta" ) {
      histos_Gen_[ index ].at( iEta )->Fill( pt, ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->eta() - eta );
      histos_GenInv_[ index ].at( iEta )->Fill( pt, 1. / ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->eta() - 1. / eta );
      histos_GenSymm_[ index ].at( iEtaSymm )->Fill( pt, ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->eta() - eta );
      histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->eta() - 1. / eta );
    }
    else if ( prop == "Phi" ) {
      const double phi( ttGenEvent_->singleLepton()->phi() );
      histos_Gen_[ index ].at( iEta )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->phi() - phi ) );
      histos_GenInv_[ index ].at( iEta )->Fill( pt, 1. / ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->phi() - 1. / phi );
      histos_GenSymm_[ index ].at( iEtaSymm )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->phi() - phi ) );
      histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->phi() - 1. / phi );
    }
  }

}


void AnalyzeHitFitResolutionFunctions::fillUdscJet( unsigned iCat, unsigned iProp )
{

  const std::string prop( kinProps_.at( iProp ) );
  const std::string index( "UdscJet_" + prop );
  const std::vector< int > jetLepCombi( ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ) );

  pat::Jet qJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LightQ ) ).correctedJet( jecLevel_, "uds" ) );

  double eta( qJet.eta() );
  double pt( qJet.pt() );
  unsigned iEta( getEtaBin( iCat, eta ) );
  unsigned iEtaSymm( getEtaBin( iCat, eta, true ) );
  if ( iEta < etaBins_.at( iCat ).size() && iEtaSymm < etaSymmBins_.at( iCat ).size() ) {
    if ( prop == "Pt" ) {
      histos_Reco_[ index ].at( iEta )->Fill( pt, pt - ttGenEvent_->hadronicDecayQuark()->pt() );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, 1. / pt - 1. / ttGenEvent_->hadronicDecayQuark()->pt() );
      histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, pt - ttGenEvent_->hadronicDecayQuark()->pt() );
      histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / pt - 1. / ttGenEvent_->hadronicDecayQuark()->pt() );
    }
    else if ( prop == "Eta" ) {
      histos_Reco_[ index ].at( iEta )->Fill( pt, eta - ttGenEvent_->hadronicDecayQuark()->eta() );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, 1. / eta - 1. / ttGenEvent_->hadronicDecayQuark()->eta() );
      histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, eta - ttGenEvent_->hadronicDecayQuark()->eta() );
      histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / eta - 1. / ttGenEvent_->hadronicDecayQuark()->eta() );
    }
    else if ( prop == "Phi" ) {
      const double phi( qJet.phi() );
      histos_Reco_[ index ].at( iEta )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( phi - ttGenEvent_->hadronicDecayQuark()->phi() ) );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, 1. / phi - 1. / ttGenEvent_->hadronicDecayQuark()->phi() );
      histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( phi - ttGenEvent_->hadronicDecayQuark()->phi() ) );
      histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / phi - 1. / ttGenEvent_->hadronicDecayQuark()->phi() );
    }
  }

  eta = ttGenEvent_->hadronicDecayQuark()->eta();
  pt  = ttGenEvent_->hadronicDecayQuark()->pt();
  iEta     = getEtaBin( iCat, eta );
  iEtaSymm = getEtaBin( iCat, eta, true );
  if ( iEta < etaBins_.at( iCat ).size() && iEtaSymm < etaSymmBins_.at( iCat ).size() ) {
    if ( prop == "Pt" ) {
      histos_Gen_[ index ].at( iEta )->Fill( pt, qJet.pt() - pt );
      histos_GenInv_[ index ].at( iEta )->Fill( pt, 1. / qJet.pt() - 1. / pt );
      histos_GenSymm_[ index ].at( iEtaSymm )->Fill( pt, qJet.pt() - pt );
      histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / qJet.pt() - 1. / pt );
    }
    else if ( prop == "Eta" ) {
      histos_Gen_[ index ].at( iEta )->Fill( pt, qJet.eta() - eta );
      histos_GenInv_[ index ].at( iEta )->Fill( pt, 1. / qJet.eta() - 1. / eta );
      histos_GenSymm_[ index ].at( iEtaSymm )->Fill( pt, qJet.eta() - eta );
      histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / qJet.eta() - 1. / eta );
    }
    else if ( prop == "Phi" ) {
      const double phi( ttGenEvent_->hadronicDecayQuark()->phi() );
      histos_Gen_[ index ].at( iEta )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( qJet.phi() - phi ) );
      histos_GenInv_[ index ].at( iEta )->Fill( pt, 1. / qJet.phi() - 1. / phi );
      histos_GenSymm_[ index ].at( iEtaSymm )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( qJet.phi() - phi ) );
      histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / qJet.phi() - 1. / phi );
    }
  }

  pat::Jet qBarJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LightQBar ) ).correctedJet( jecLevel_, "uds" ) );

  eta = qBarJet.eta();
  pt  = qBarJet.pt();
  iEta     = getEtaBin( iCat, eta );
  iEtaSymm = getEtaBin( iCat, eta, true );
  if ( iEta < etaBins_.at( iCat ).size() && iEtaSymm < etaSymmBins_.at( iCat ).size() ) {
    if ( prop == "Pt" ) {
      histos_Reco_[ index ].at( iEta )->Fill( pt, pt - ttGenEvent_->hadronicDecayQuarkBar()->pt() );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, 1. / pt - 1. / ttGenEvent_->hadronicDecayQuarkBar()->pt() );
      histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, pt - ttGenEvent_->hadronicDecayQuarkBar()->pt() );
      histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / pt - 1. / ttGenEvent_->hadronicDecayQuarkBar()->pt() );
    }
    else if ( prop == "Eta" ) {
      histos_Reco_[ index ].at( iEta )->Fill( pt, eta - ttGenEvent_->hadronicDecayQuarkBar()->eta() );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, 1. / eta - 1. / ttGenEvent_->hadronicDecayQuarkBar()->eta() );
      histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, eta - ttGenEvent_->hadronicDecayQuarkBar()->eta() );
      histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / eta - 1. / ttGenEvent_->hadronicDecayQuarkBar()->eta() );
    }
    else if ( prop == "Phi" ) {
      const double phi( qBarJet.phi() );
      histos_Reco_[ index ].at( iEta )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( phi - ttGenEvent_->hadronicDecayQuarkBar()->phi() ) );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, 1. / phi - 1. / ttGenEvent_->hadronicDecayQuarkBar()->phi() );
      histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( phi - ttGenEvent_->hadronicDecayQuarkBar()->phi() ) );
      histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / phi - 1. / ttGenEvent_->hadronicDecayQuarkBar()->phi() );
    }
  }

  eta = ttGenEvent_->hadronicDecayQuarkBar()->eta();
  pt  = ttGenEvent_->hadronicDecayQuarkBar()->pt();
  iEta     = getEtaBin( iCat, eta );
  iEtaSymm = getEtaBin( iCat, eta, true );
  if ( iEta < etaBins_.at( iCat ).size() && iEtaSymm < etaSymmBins_.at( iCat ).size() ) {
    if ( prop == "Pt" ) {
      histos_Gen_[ index ].at( iEta )->Fill( pt, qBarJet.pt() - pt );
      histos_GenInv_[ index ].at( iEta )->Fill( pt, 1. / qBarJet.pt() - 1. / pt );
      histos_GenSymm_[ index ].at( iEtaSymm )->Fill( pt, qBarJet.pt() - pt );
      histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / qBarJet.pt() - 1. / pt );
    }
    else if ( prop == "Eta" ) {
      histos_Gen_[ index ].at( iEta )->Fill( pt, qBarJet.eta() - eta );
      histos_GenInv_[ index ].at( iEta )->Fill( pt, 1. / qBarJet.eta() - 1. / eta );
      histos_GenSymm_[ index ].at( iEtaSymm )->Fill( pt, qBarJet.eta() - eta );
      histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / qBarJet.eta() - 1. / eta );
    }
    else if ( prop == "Phi" ) {
      const double phi( ttGenEvent_->hadronicDecayQuarkBar()->phi() );
      histos_Gen_[ index ].at( iEta )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( qBarJet.phi() - phi ) );
      histos_GenInv_[ index ].at( iEta )->Fill( pt, 1. / qBarJet.phi() - 1. / phi );
      histos_GenSymm_[ index ].at( iEtaSymm )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( qBarJet.phi() - phi ) );
      histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / qBarJet.phi() - 1. / phi );
    }
  }

}


void AnalyzeHitFitResolutionFunctions::fillBJet( unsigned iCat, unsigned iProp )
{

  const std::string prop( kinProps_.at( iProp ) );
  const std::string index( "BJet_" + prop );
  const std::vector< int > jetLepCombi( ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ) );

  pat::Jet hadBJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::HadB ) ).correctedJet( jecLevel_, "bottom" ) );

  double eta( hadBJet.eta() );
  double pt( hadBJet.pt() );
  unsigned iEta( getEtaBin( iCat, eta ) );
  unsigned iEtaSymm( getEtaBin( iCat, eta, true ) );
  if ( iEta < etaBins_.at( iCat ).size() && iEtaSymm < etaSymmBins_.at( iCat ).size() ) {
    if ( prop == "Pt" ) {
      histos_Reco_[ index ].at( iEta )->Fill( pt, pt - ttGenEvent_->hadronicDecayB()->pt() );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, 1. / pt - 1. / ttGenEvent_->hadronicDecayB()->pt() );
      histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, pt - ttGenEvent_->hadronicDecayB()->pt() );
      histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / pt - 1. / ttGenEvent_->hadronicDecayB()->pt() );
    }
    else if ( prop == "Eta" ) {
      histos_Reco_[ index ].at( iEta )->Fill( pt, eta - ttGenEvent_->hadronicDecayB()->eta() );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, 1. / eta - 1. / ttGenEvent_->hadronicDecayB()->eta() );
      histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, eta - ttGenEvent_->hadronicDecayB()->eta() );
      histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / eta - 1. / ttGenEvent_->hadronicDecayB()->eta() );
    }
    else if ( prop == "Phi" ) {
      const double phi( hadBJet.phi() );
      histos_Reco_[ index ].at( iEta )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( phi - ttGenEvent_->hadronicDecayB()->phi() ) );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, 1. / phi - 1. / ttGenEvent_->hadronicDecayB()->phi() );
      histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( phi - ttGenEvent_->hadronicDecayB()->phi() ) );
      histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / phi - 1. / ttGenEvent_->hadronicDecayB()->phi() );
    }
  }

  eta = ttGenEvent_->leptonicDecayB()->eta();
  pt  = ttGenEvent_->hadronicDecayB()->pt();
  iEta     = getEtaBin( iCat, eta );
  iEtaSymm = getEtaBin( iCat, eta, true );
  if ( iEta < etaBins_.at( iCat ).size() && iEtaSymm < etaSymmBins_.at( iCat ).size() ) {
    if ( prop == "Pt" ) {
      histos_Gen_[ index ].at( iEta )->Fill( pt, hadBJet.pt() - pt );
      histos_GenInv_[ index ].at( iEta )->Fill( pt, 1. / hadBJet.pt() - 1. / pt );
      histos_GenSymm_[ index ].at( iEtaSymm )->Fill( pt, hadBJet.pt() - pt );
      histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / hadBJet.pt() - 1. / pt );
    }
    else if ( prop == "Eta" ) {
      histos_Gen_[ index ].at( iEta )->Fill( pt, hadBJet.eta() - eta );
      histos_GenInv_[ index ].at( iEta )->Fill( pt, 1. / hadBJet.eta() - 1. / eta );
      histos_GenSymm_[ index ].at( iEtaSymm )->Fill( pt, hadBJet.eta() - eta );
      histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / hadBJet.eta() - 1. / eta );
    }
    else if ( prop == "Phi" ) {
      const double phi( ttGenEvent_->hadronicDecayB()->phi() );
      histos_Gen_[ index ].at( iEta )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( hadBJet.phi() - phi ) );
      histos_GenInv_[ index ].at( iEta )->Fill( pt, 1. / hadBJet.phi() - 1. / phi );
      histos_GenSymm_[ index ].at( iEtaSymm )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( hadBJet.phi() - phi ) );
      histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / hadBJet.phi() - 1. / phi );
    }
  }

  pat::Jet lepBJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LepB ) ).correctedJet( jecLevel_, "bottom" ) );

  eta = lepBJet.eta();
  pt  = lepBJet.pt();
  iEta     = getEtaBin( iCat, eta );
  iEtaSymm = getEtaBin( iCat, eta, true );
  if ( iEta < etaBins_.at( iCat ).size() && iEtaSymm < etaSymmBins_.at( iCat ).size() ) {
    if ( prop == "Pt" ) {
      histos_Reco_[ index ].at( iEta )->Fill( pt, pt - ttGenEvent_->leptonicDecayB()->pt() );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, 1. / pt - 1. / ttGenEvent_->leptonicDecayB()->pt() );
      histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, pt - ttGenEvent_->leptonicDecayB()->pt() );
      histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / pt - 1. / ttGenEvent_->leptonicDecayB()->pt() );
    }
    else if ( prop == "Eta" ) {
      histos_Reco_[ index ].at( iEta )->Fill( pt, eta - ttGenEvent_->leptonicDecayB()->eta() );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, 1. / eta - 1. / ttGenEvent_->leptonicDecayB()->eta() );
      histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, eta - ttGenEvent_->leptonicDecayB()->eta() );
      histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / eta - 1. / ttGenEvent_->leptonicDecayB()->eta() );
    }
    else if ( prop == "Phi" ) {
      const double phi( lepBJet.phi() );
      histos_Reco_[ index ].at( iEta )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( phi - ttGenEvent_->leptonicDecayB()->phi() ) );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, 1. / phi - 1. / ttGenEvent_->leptonicDecayB()->phi() );
      histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( phi - ttGenEvent_->leptonicDecayB()->phi() ) );
      histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / phi - 1. / ttGenEvent_->leptonicDecayB()->phi() );
    }
  }

  eta = ttGenEvent_->leptonicDecayB()->eta();
  pt  = ttGenEvent_->leptonicDecayB()->pt();
  iEta     = getEtaBin( iCat, eta );
  iEtaSymm = getEtaBin( iCat, eta, true );
  if ( iEta < etaBins_.at( iCat ).size() && iEtaSymm < etaSymmBins_.at( iCat ).size() ) {
    if ( prop == "Pt" ) {
      histos_Gen_[ index ].at( iEta )->Fill( pt, lepBJet.pt() - pt );
      histos_GenInv_[ index ].at( iEta )->Fill( pt, 1. / lepBJet.pt() - 1. / pt );
      histos_GenSymm_[ index ].at( iEtaSymm )->Fill( pt, lepBJet.pt() - pt );
      histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / lepBJet.pt() - 1. / pt );
    }
    else if ( prop == "Eta" ) {
      histos_Gen_[ index ].at( iEta )->Fill( pt, lepBJet.eta() - eta );
      histos_GenInv_[ index ].at( iEta )->Fill( pt, 1. / lepBJet.eta() - 1. / eta );
      histos_GenSymm_[ index ].at( iEtaSymm )->Fill( pt, lepBJet.eta() - eta );
      histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / lepBJet.eta() - 1. / eta );
    }
    else if ( prop == "Phi" ) {
      const double phi( ttGenEvent_->leptonicDecayB()->phi() );
      histos_Gen_[ index ].at( iEta )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( lepBJet.phi() - phi ) );
      histos_GenInv_[ index ].at( iEta )->Fill( pt, 1. / lepBJet.phi() - 1. / phi );
      histos_GenSymm_[ index ].at( iEtaSymm )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( lepBJet.phi() - phi ) );
      histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / lepBJet.phi() - 1. / phi );
    }
  }

}


void AnalyzeHitFitResolutionFunctions::fillMET( unsigned iCat, unsigned iProp )
{

  const std::string prop( kinProps_.at( iProp ) );
  const std::string index( "MET_" + prop );

  double eta( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->eta() );
  double pt( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt() );
  unsigned iEta( getEtaBin( iCat, eta ) );
  unsigned iEtaSymm( getEtaBin( iCat, eta, true ) );
  if ( iEta < etaBins_.at( iCat ).size() && iEtaSymm < etaSymmBins_.at( iCat ).size() ) {
    if ( prop == "Pt" ) {
      histos_Reco_[ index ].at( iEta )->Fill( pt, pt - ttGenEvent_->singleNeutrino()->pt() );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, 1. / pt - 1. / ttGenEvent_->singleNeutrino()->pt() );
      histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, pt - ttGenEvent_->singleNeutrino()->pt() );
      histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / pt - 1. / ttGenEvent_->singleNeutrino()->pt() );
    }
    else if ( prop == "Eta" ) {
      histos_Reco_[ index ].at( iEta )->Fill( pt, eta - ttGenEvent_->singleNeutrino()->eta() );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, 1. / eta - 1. / ttGenEvent_->singleNeutrino()->eta() );
      histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, eta - ttGenEvent_->singleNeutrino()->eta() );
      histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / eta - 1. / ttGenEvent_->singleNeutrino()->eta() );
    }
    else if ( prop == "Phi" ) {
      const double phi( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->phi() );
      histos_Reco_[ index ].at( iEta )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( phi - ttGenEvent_->singleNeutrino()->phi() ) );
      histos_RecoInv_[ index ].at( iEta )->Fill( pt, 1. / phi - 1. / ttGenEvent_->singleNeutrino()->phi() );
      histos_RecoSymm_[ index ].at( iEtaSymm )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( phi - ttGenEvent_->singleNeutrino()->phi() ) );
      histos_RecoInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / phi - 1. / ttGenEvent_->singleNeutrino()->phi() );
    }
  }

  eta = ttGenEvent_->singleNeutrino()->eta();
  pt  = ttGenEvent_->singleNeutrino()->pt();
  iEta     = getEtaBin( iCat, eta );
  iEtaSymm = getEtaBin( iCat, eta, true );
  if ( iEta < etaBins_.at( iCat ).size() && iEtaSymm < etaSymmBins_.at( iCat ).size() ) {
    if ( prop == "Pt" ) {
      histos_Gen_[ index ].at( iEta )->Fill( pt, ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt() - pt );
      histos_GenInv_[ index ].at( iEta )->Fill( pt, 1. / ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt() - 1. / pt );
      histos_GenSymm_[ index ].at( iEtaSymm )->Fill( pt, ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt() - pt );
      histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt() - 1. / pt );
    }
    else if ( prop == "Eta" ) {
      histos_Gen_[ index ].at( iEta )->Fill( pt, ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->eta() - eta );
      histos_GenInv_[ index ].at( iEta )->Fill( pt, 1. / ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->eta() - 1. / eta );
      histos_GenSymm_[ index ].at( iEtaSymm )->Fill( pt, ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->eta() - eta );
      histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->eta() - 1. / eta );
    }
    else if ( prop == "Phi" ) {
      const double phi( ttGenEvent_->singleNeutrino()->phi() );
      histos_Gen_[ index ].at( iEta )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->phi() - phi ) );
      histos_GenInv_[ index ].at( iEta )->Fill( pt, 1. / ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->phi() - 1. / phi );
      histos_GenSymm_[ index ].at( iEtaSymm )->Fill( pt, ROOT::Math::VectorUtil::Phi_mpi_pi( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->phi() - phi ) );
      histos_GenInvSymm_[ index ].at( iEtaSymm )->Fill( pt, 1. / ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->phi() - 1. / phi );
    }
  }

}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( AnalyzeHitFitResolutionFunctions );
