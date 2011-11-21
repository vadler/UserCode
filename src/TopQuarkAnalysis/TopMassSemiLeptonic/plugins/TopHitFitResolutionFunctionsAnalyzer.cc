// -*- C++ -*-
//
// Package:    TopMassSemiLeptonic
// Class:      TopHitFitResolutionFunctionsAnalyzer
//
// $Id: TopHitFitResolutionFunctionsAnalyzer.cc,v 1.1 2011/08/31 14:45:53 vadler Exp $
//
/**
  \class    TopHitFitResolutionFunctionsAnalyzer TopHitFitResolutionFunctionsAnalyzer.cc "TopQuarkAnalysis/TopMassSemiLeptonic/plugins/TopHitFitResolutionFunctionsAnalyzer.cc"
  \brief    Extract HitFit resolution functions from parameter files



  \author   Volker Adler
  \version  $Id: TopHitFitResolutionFunctionsAnalyzer.cc,v 1.1 2011/08/31 14:45:53 vadler Exp $
*/


#include <map>
#include <vector>
#include <string>
#include <iostream>

#include "TF1.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"


typedef std::map< std::string, TF1* > MapTF1;


class TopHitFitResolutionFunctionsAnalyzer : public edm::EDAnalyzer {

  public:

    /// Constructors and Desctructor

    /// Default constructor
    explicit TopHitFitResolutionFunctionsAnalyzer( const edm::ParameterSet & iConfig );

    /// Destructor
    virtual ~TopHitFitResolutionFunctionsAnalyzer();

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
    // Muon resolution file
    std::string muonResolutions_;
    // Electron resolution file
    std::string electronResolutions_;
    // Light jet resolution file
    std::string udscJetResolutions_;
    // b-jet resolution file
    std::string bJetResolutions_;
    // MET resolution file
    std::string metResolutions_;
    // Lepton
    unsigned binsLepPt_;
    double   maxLepPt_;
    double   maxDiffLepPt_;
    double   maxDiffLepPtInv_;
    double   maxDiffLepPtInvRel_;
    double   maxDiffLepEta_;
    double   maxDiffLepPhi_;
    // Jet
    unsigned binsJetPt_;
    double   maxJetPt_;
    double   maxDiffJetPt_;
    double   maxDiffJetPtInv_;
    double   maxDiffJetPtInvRel_;
    double   maxDiffJetEta_;
    double   maxDiffJetPhi_;
    // Neutrino
    unsigned binsNuPt_;
    double   maxNuPt_;
    double   maxDiffNuPt_;
    double   maxDiffNuPtInv_;
    double   maxDiffNuPtInvRel_;
    double   maxDiffNuEta_;
    double   maxDiffNuPhi_;

    /// Resolution functions
    MapTF1 func_MuonResolution_;
    MapTF1 func_ElectronResolution_;
    MapTF1 func_UdscJetResolution_;
    MapTF1 func_BJetResolution_;
    MapTF1 func_METResolution_;

};


#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TopQuarkAnalysis/TopHitFit/interface/LeptonTranslatorBase.h"
#include "TopQuarkAnalysis/TopHitFit/interface/JetTranslatorBase.h"
#include "TopQuarkAnalysis/TopHitFit/interface/METTranslatorBase.h"
#include "TopQuarkAnalysis/TopHitFit/interface/EtaDepResolution.h"

#include "TopQuarkAnalysis/TopMassSemiLeptonic/interface/Helpers.h"


// Default constructor
TopHitFitResolutionFunctionsAnalyzer::TopHitFitResolutionFunctionsAnalyzer( const edm::ParameterSet & iConfig )
: muonResolutions_( iConfig.getParameter< std::string >( "muonResolutions" ) )
, electronResolutions_( iConfig.getParameter< std::string >( "electronResolutions" ) )
, udscJetResolutions_( iConfig.getParameter< std::string >( "udscJetResolutions" ) )
, bJetResolutions_( iConfig.getParameter< std::string >( "bJetResolutions" ) )
, metResolutions_( iConfig.getParameter< std::string >( "metResolutions" ) )
, binsLepPt_( iConfig.getParameter< unsigned >( "binsLepPt" ) )
, maxLepPt_( iConfig.getParameter< double >( "maxLepPt" ) )
, maxDiffLepPt_( iConfig.getParameter< double >( "maxDiffLepPt" ) )
, maxDiffLepPtInv_( iConfig.getParameter< double >( "maxDiffLepPtInv" ) )
, maxDiffLepPtInvRel_( iConfig.getParameter< double >( "maxDiffLepPtInvRel" ) )
, maxDiffLepEta_( iConfig.getParameter< double >( "maxDiffLepEta" ) )
, maxDiffLepPhi_( iConfig.getParameter< double >( "maxDiffLepPhi" ) )
, binsJetPt_( iConfig.getParameter< unsigned >( "binsJetPt" ) )
, maxJetPt_( iConfig.getParameter< double >( "maxJetPt" ) )
, maxDiffJetPt_( iConfig.getParameter< double >( "maxDiffJetPt" ) )
, maxDiffJetPtInv_( iConfig.getParameter< double >( "maxDiffJetPtInv" ) )
, maxDiffJetPtInvRel_( iConfig.getParameter< double >( "maxDiffJetPtInvRel" ) )
, maxDiffJetEta_( iConfig.getParameter< double >( "maxDiffJetEta" ) )
, maxDiffJetPhi_( iConfig.getParameter< double >( "maxDiffJetPhi" ) )
, binsNuPt_( iConfig.getParameter< unsigned >( "binsNuPt" ) )
, maxNuPt_ ( iConfig.getParameter< double >( "maxNuPt" ) )
, maxDiffNuPt_ ( iConfig.getParameter< double >( "maxDiffNuPt" ) )
, maxDiffNuPtInv_ ( iConfig.getParameter< double >( "maxDiffNuPtInv" ) )
, maxDiffNuPtInvRel_ ( iConfig.getParameter< double >( "maxDiffNuPtInvRel" ) )
, maxDiffNuEta_ ( iConfig.getParameter< double >( "maxDiffNuEta" ) )
, maxDiffNuPhi_ ( iConfig.getParameter< double >( "maxDiffNuPhi" ) )
  // initialise function maps
, func_MuonResolution_()
, func_ElectronResolution_()
, func_UdscJetResolution_()
, func_BJetResolution_()
, func_METResolution_()
{
}


// Destructor
TopHitFitResolutionFunctionsAnalyzer::~TopHitFitResolutionFunctionsAnalyzer()
{
}


// Begin job
void TopHitFitResolutionFunctionsAnalyzer::beginJob()
{

  edm::Service< TFileService > fileService;

  // Determine HitFit resolution functions

  const std::string funcRes(      "sqrt(([0]*[0]*x+[1]*[1])*x+[2]*[2])" );
  const std::string funcResInv(   "sqrt(([0]*[0]/x+[1]*[1])/x+[2]*[2])" );
  // for E_T dependence on E:
  const std::string funcResEt(    "sqrt(([0]*[0]*x+[1]*[1])*x+[2]*[2])*cosh([3])" );
  const std::string funcResEtInv( "sqrt(([0]*[0]/x+[1]*[1])/x+[2]*[2])/cosh([3])" );

  TFileDirectory dir_Resolution( fileService->mkdir( "Resolution", "HitFit resolution functions" ) );

  // Muon resolutions

  TFileDirectory dir_MuonResolution( dir_Resolution.mkdir( "MuonResolution", "HitFit resolution functions for muons" ) );
  TFileDirectory dir_MuonResolutionP( dir_MuonResolution.mkdir( "MuonResolutionP", "HitFit momentum resolution functions for muons" ) );
  TFileDirectory dir_MuonResolutionEta( dir_MuonResolution.mkdir( "MuonResolutionEta", "HitFit eta resolution functions for muons" ) );
  TFileDirectory dir_MuonResolutionPhi( dir_MuonResolution.mkdir( "MuonResolutionPhi", "HitFit phi resolution functions for muons" ) );

  const edm::FileInPath muonResFile( muonResolutions_ );
  const hitfit::EtaDepResolution muonRes( muonResFile.fullPath() );
  const std::vector< hitfit::EtaDepResElement > muonResElems( muonRes.GetEtaDepResElement() );

  unsigned count( 0 );
  for ( unsigned iResEl = 0; iResEl < muonResElems.size(); ++iResEl ) {

    const hitfit::Vector_Resolution vecRes( muonResElems.at( iResEl ).GetResolution() );

    const double etaMin( muonResElems.at( iResEl ).EtaMin() );
    const double etaMax( muonResElems.at( iResEl ).EtaMax() );
    // positive eta only, since everything is symmetric
    if ( etaMin < 0. && etaMax <= 0. ) continue;

    std::string title( my::helpers::to_string( etaMin ) + " #leq #eta #leq " + my::helpers::to_string( etaMax ) );
    std::string xAxisTitle( "p" );
    if ( vecRes.use_et() ) xAxisTitle += "_{t}";

    // Momentum
    const hitfit::Resolution pRes( vecRes.p_res() );
    std::string key( "P_" + my::helpers::to_string( count ) );
    std::string name( "Muon_" + key );
    std::string yAxisTitle( "#sigma_{" + xAxisTitle + "}" );
//     std::string function;
//     if ( pRes.inverse() ) function = vecRes.use_et() ? funcResEtInv : funcResInv;
//     else                  function = vecRes.use_et() ? funcResEt    : funcRes;
    std::string function( pRes.inverse() ? funcResInv : funcRes );
    func_MuonResolution_[ key ] = dir_MuonResolutionP.make< TF1 >( name.c_str(), function.c_str(), 0., maxLepPt_ );
    func_MuonResolution_[ key ]->SetParameters( pRes.C(), pRes.R(), pRes.N(), std::min( std::fabs( etaMin ), std::fabs( etaMax ) ) );
    func_MuonResolution_[ key ]->SetMinimum( 0. );
    func_MuonResolution_[ key ]->SetMaximum( pRes.inverse() ? maxDiffLepPtInv_ : maxDiffLepPt_ );
    func_MuonResolution_[ key ]->SetTitle( title.c_str() );
    func_MuonResolution_[ key ]->GetXaxis()->SetTitle( xAxisTitle.c_str() );
    func_MuonResolution_[ key ]->GetYaxis()->SetTitle( yAxisTitle.c_str() );
    key  += "_Rel";
    name += "_Rel";
    yAxisTitle = "#frac{" + yAxisTitle + "}{" + xAxisTitle + "}";
    function = "(" + function + ( pRes.inverse() ? ")*x" : ")/x" );
    func_MuonResolution_[ key ] = dir_MuonResolutionP.make< TF1 >( name.c_str(), function.c_str(), 0., maxLepPt_ );
    func_MuonResolution_[ key ]->SetParameters( pRes.C(), pRes.R(), pRes.N(), std::min( std::fabs( etaMin ), std::fabs( etaMax ) ) );
    func_MuonResolution_[ key ]->SetMinimum( 0. );
    func_MuonResolution_[ key ]->SetMaximum( pRes.inverse() ? maxDiffLepPtInvRel_ : 1. );
    func_MuonResolution_[ key ]->SetTitle( title.c_str() );
    func_MuonResolution_[ key ]->GetXaxis()->SetTitle( xAxisTitle.c_str() );
    func_MuonResolution_[ key ]->GetYaxis()->SetTitle( yAxisTitle.c_str() );

    // Eta
    const hitfit::Resolution etaRes( vecRes.eta_res() );
    key  = "Eta_" + my::helpers::to_string( count );
    name = "Muon_" + key;
    func_MuonResolution_[ key ] = dir_MuonResolutionEta.make< TF1 >( name.c_str(), etaRes.inverse() ? funcResInv.c_str() : funcRes.c_str(), 0., maxLepPt_ );
    func_MuonResolution_[ key ]->SetParameters( etaRes.C(), etaRes.R(), etaRes.N() );
    func_MuonResolution_[ key ]->SetMinimum( 0. );
    func_MuonResolution_[ key ]->SetMaximum( maxDiffLepEta_ );
    func_MuonResolution_[ key ]->SetTitle( title.c_str() );
    func_MuonResolution_[ key ]->GetXaxis()->SetTitle( xAxisTitle.c_str() );
    func_MuonResolution_[ key ]->GetYaxis()->SetTitle( "#sigma_{#eta}" );

    // Phi
    const hitfit::Resolution phiRes( vecRes.phi_res() );
    key  = "Phi_" + my::helpers::to_string( count );
    name = "Muon_" + key;
    func_MuonResolution_[ key ] = dir_MuonResolutionPhi.make< TF1 >( name.c_str(), phiRes.inverse() ? funcResInv.c_str() : funcRes.c_str(), 0., maxLepPt_ );
    func_MuonResolution_[ key ]->SetParameters( phiRes.C(), phiRes.R(), phiRes.N() );
    func_MuonResolution_[ key ]->SetMinimum( 0. );
    func_MuonResolution_[ key ]->SetMaximum( maxDiffLepPhi_ );
    func_MuonResolution_[ key ]->SetTitle( title.c_str() );
    func_MuonResolution_[ key ]->GetXaxis()->SetTitle( xAxisTitle.c_str() );
    func_MuonResolution_[ key ]->GetYaxis()->SetTitle( "#sigma_{#phi}" );

    ++count;
  }

  // Electron resolutions

  TFileDirectory dir_ElectronResolution( dir_Resolution.mkdir( "ElectronResolution", "HitFit resolution functions for electrons" ) );
  TFileDirectory dir_ElectronResolutionP( dir_ElectronResolution.mkdir( "ElectronResolutionP", "HitFit momentum resolution functions for electrons" ) );
  TFileDirectory dir_ElectronResolutionEta( dir_ElectronResolution.mkdir( "ElectronResolutionEta", "HitFit eta resolution functions for electrons" ) );
  TFileDirectory dir_ElectronResolutionPhi( dir_ElectronResolution.mkdir( "ElectronResolutionPhi", "HitFit phi resolution functions for electrons" ) );

  const edm::FileInPath electronResFile( electronResolutions_ );
  const hitfit::EtaDepResolution electronRes( electronResFile.fullPath() );
  const std::vector< hitfit::EtaDepResElement > electronResElems( electronRes.GetEtaDepResElement() );

  count = 0;
  for ( unsigned iResEl = 0; iResEl < electronResElems.size(); ++iResEl ) {

    const hitfit::Vector_Resolution vecRes( electronResElems.at( iResEl ).GetResolution() );

    const double etaMin( electronResElems.at( iResEl ).EtaMin() );
    const double etaMax( electronResElems.at( iResEl ).EtaMax() );
    // positive eta only, since everything is symmetric
    if ( etaMin < 0. && etaMax <= 0. ) continue;

    std::string title( my::helpers::to_string( etaMin ) + " #leq #eta #leq " + my::helpers::to_string( etaMax ) );

    // Momentum
    const hitfit::Resolution pRes( vecRes.p_res() );
    std::string key( "P_" + my::helpers::to_string( count ) );
    std::string name( "Elec_" + key );
    std::string xAxisTitle( "p" );
    if ( vecRes.use_et() ) xAxisTitle += "_{t}";
    std::string yAxisTitle( "#sigma_{" + xAxisTitle + "}" );
    xAxisTitle += " (GeV)";
//     std::string function;
//     if ( pRes.inverse() ) function = vecRes.use_et() ? funcResEtInv : funcResInv;
//     else                  function = vecRes.use_et() ? funcResEt    : funcRes;
    std::string function( pRes.inverse() ? funcResInv : funcRes );
    func_ElectronResolution_[ key ] = dir_ElectronResolutionP.make< TF1 >( name.c_str(), function.c_str(), 0., maxLepPt_ );
    func_ElectronResolution_[ key ]->SetParameters( pRes.C(), pRes.R(), pRes.N(), std::min( std::fabs( etaMin ), std::fabs( etaMax ) ) );
    func_ElectronResolution_[ key ]->SetMinimum(  0. );
    func_ElectronResolution_[ key ]->SetMaximum( pRes.inverse() ? maxDiffLepPtInv_ : maxDiffLepPt_ );
    func_ElectronResolution_[ key ]->SetTitle( title.c_str() );
    func_ElectronResolution_[ key ]->GetXaxis()->SetTitle( xAxisTitle.c_str() );
    func_ElectronResolution_[ key ]->GetYaxis()->SetTitle( yAxisTitle.c_str() );
    key  += "_Rel";
    name += "_Rel";
    yAxisTitle = "#frac{" + yAxisTitle + "}{" + xAxisTitle + "}";
    function = "(" + function + ( pRes.inverse() ? ")*x" : ")/x" );
    func_ElectronResolution_[ key ] = dir_ElectronResolutionP.make< TF1 >( name.c_str(), function.c_str(), 0., maxLepPt_ );
    func_ElectronResolution_[ key ]->SetParameters( pRes.C(), pRes.R(), pRes.N(), std::min( std::fabs( etaMin ), std::fabs( etaMax ) ) );
    func_ElectronResolution_[ key ]->SetMinimum( 0. );
    func_ElectronResolution_[ key ]->SetMaximum( pRes.inverse() ? maxDiffLepPtInvRel_ : 1. );
    func_ElectronResolution_[ key ]->SetTitle( title.c_str() );
    func_ElectronResolution_[ key ]->GetXaxis()->SetTitle( xAxisTitle.c_str() );
    func_ElectronResolution_[ key ]->GetYaxis()->SetTitle( yAxisTitle.c_str() );

    // Eta
    const hitfit::Resolution etaRes( vecRes.eta_res() );
    key  = "Eta_" + my::helpers::to_string( count );
    name = "Elec_" + key;
    func_ElectronResolution_[ key ] = dir_ElectronResolutionEta.make< TF1 >( name.c_str(), etaRes.inverse() ? funcResInv.c_str() : funcRes.c_str(), 0., maxLepPt_ );
    func_ElectronResolution_[ key ]->SetParameters( etaRes.C(), etaRes.R(), etaRes.N() );
    func_ElectronResolution_[ key ]->SetMinimum( 0. );
    func_ElectronResolution_[ key ]->SetMaximum( maxDiffLepEta_ );
    func_ElectronResolution_[ key ]->SetTitle( title.c_str() );
    func_ElectronResolution_[ key ]->GetXaxis()->SetTitle( xAxisTitle.c_str() );
    func_ElectronResolution_[ key ]->GetYaxis()->SetTitle( "#sigma_{#eta}" );

    // Phi
    const hitfit::Resolution phiRes( vecRes.phi_res() );
    key  = "Phi_" + my::helpers::to_string( count );
    name = "Elec_" + key;
    func_ElectronResolution_[ key ] = dir_ElectronResolutionPhi.make< TF1 >( name.c_str(), phiRes.inverse() ? funcResInv.c_str() : funcRes.c_str(), 0., maxLepPt_ );
    func_ElectronResolution_[ key ]->SetParameters( phiRes.C(), phiRes.R(), phiRes.N() );
    func_ElectronResolution_[ key ]->SetMinimum( 0. );
    func_ElectronResolution_[ key ]->SetMaximum( maxDiffLepPhi_ );
    func_ElectronResolution_[ key ]->SetTitle( title.c_str() );
    func_ElectronResolution_[ key ]->GetXaxis()->SetTitle( xAxisTitle.c_str() );
    func_ElectronResolution_[ key ]->GetYaxis()->SetTitle( "#sigma_{#phi}" );

    ++count;
  }

  // Light jet resolutions

  TFileDirectory dir_UdscJetResolution( dir_Resolution.mkdir( "UdscJetResolution", "HitFit resolution functions for light jets" ) );
  TFileDirectory dir_UdscJetResolutionP( dir_UdscJetResolution.mkdir( "UdscJetResolutionP", "HitFit momentum resolution functions for light jets" ) );
  TFileDirectory dir_UdscJetResolutionEta( dir_UdscJetResolution.mkdir( "UdscJetResolutionEta", "HitFit eta resolution functions for light jets" ) );
  TFileDirectory dir_UdscJetResolutionPhi( dir_UdscJetResolution.mkdir( "UdscJetResolutionPhi", "HitFit phi resolution functions for light jets" ) );

  const edm::FileInPath udscJetResFile( udscJetResolutions_ );
  const hitfit::EtaDepResolution udscJetRes( udscJetResFile.fullPath() );
  const std::vector< hitfit::EtaDepResElement > udscJetResElems( udscJetRes.GetEtaDepResElement() );

  count = 0;
  for ( unsigned iResEl = 0; iResEl < udscJetResElems.size(); ++iResEl ) {

    const hitfit::Vector_Resolution vecRes( udscJetResElems.at( iResEl ).GetResolution() );

    const double etaMin( udscJetResElems.at( iResEl ).EtaMin() );
    const double etaMax( udscJetResElems.at( iResEl ).EtaMax() );
    // positive eta only, since everything is symmetric
    if ( etaMin < 0. && etaMax <= 0. ) continue;

    std::string title( my::helpers::to_string( etaMin ) + " #leq #eta #leq " + my::helpers::to_string( etaMax ) );

    // Momentum
    const hitfit::Resolution pRes( vecRes.p_res() );
    std::string key( "P_" + my::helpers::to_string( count ) );
    std::string name( "UdscJet_" + key );
    std::string xAxisTitle( "p" );
    if ( vecRes.use_et() ) xAxisTitle += "_{t}";
    std::string yAxisTitle( "#sigma_{" + xAxisTitle + "}" );
    xAxisTitle += " (GeV)";
//     std::string function;
//     if ( pRes.inverse() ) function = vecRes.use_et() ? funcResEtInv : funcResInv;
//     else                  function = vecRes.use_et() ? funcResEt    : funcRes;
    std::string function( pRes.inverse() ? funcResInv : funcRes );
    func_UdscJetResolution_[ key ] = dir_UdscJetResolutionP.make< TF1 >( name.c_str(), function.c_str(), 0., maxJetPt_ );
    func_UdscJetResolution_[ key ]->SetParameters( pRes.C(), pRes.R(), pRes.N(), std::min( std::fabs( etaMin ), std::fabs( etaMax ) ) );
//     func_UdscJetResolution_[ key ]->SetRange( 0., maxJetPt_ );
    func_UdscJetResolution_[ key ]->SetMinimum(   0. );
    func_UdscJetResolution_[ key ]->SetMaximum( pRes.inverse() ? maxDiffJetPtInv_ : maxDiffJetPt_ );
    func_UdscJetResolution_[ key ]->SetTitle( title.c_str() );
    func_UdscJetResolution_[ key ]->GetXaxis()->SetTitle( xAxisTitle.c_str() );
    func_UdscJetResolution_[ key ]->GetYaxis()->SetTitle( yAxisTitle.c_str() );
    key  += "_Rel";
    name += "_Rel";
    yAxisTitle = "#frac{" + yAxisTitle + "}{" + xAxisTitle + "}";
    function = "(" + function + ( pRes.inverse() ? ")*x" : ")/x" );
    func_UdscJetResolution_[ key ] = dir_UdscJetResolutionP.make< TF1 >( name.c_str(), function.c_str(), 0., maxJetPt_ );
    func_UdscJetResolution_[ key ]->SetParameters( pRes.C(), pRes.R(), pRes.N(), std::min( std::fabs( etaMin ), std::fabs( etaMax ) ) );
//     func_UdscJetResolution_[ key ]->SetRange( 0., maxJetPt_ );
    func_UdscJetResolution_[ key ]->SetMinimum( 0. );
    func_UdscJetResolution_[ key ]->SetMaximum( pRes.inverse() ? maxDiffJetPtInvRel_ : 1. );
    func_UdscJetResolution_[ key ]->SetTitle( title.c_str() );
    func_UdscJetResolution_[ key ]->GetXaxis()->SetTitle( xAxisTitle.c_str() );
    func_UdscJetResolution_[ key ]->GetYaxis()->SetTitle( yAxisTitle.c_str() );

    // Eta
    const hitfit::Resolution etaRes( vecRes.eta_res() );
    key  = "Eta_" + my::helpers::to_string( count );
    name = "UdscJet_" + key;
    func_UdscJetResolution_[ key ] = dir_UdscJetResolutionEta.make< TF1 >( name.c_str(), etaRes.inverse() ? funcResInv.c_str() : funcRes.c_str(), 0., maxJetPt_ );
    func_UdscJetResolution_[ key ]->SetParameters( etaRes.C(), etaRes.R(), etaRes.N() );
//     func_UdscJetResolution_[ key ]->SetRange( 0., maxJetPt_ );
    func_UdscJetResolution_[ key ]->SetMinimum( 0. );
    func_UdscJetResolution_[ key ]->SetMaximum( maxDiffJetEta_ );
    func_UdscJetResolution_[ key ]->SetTitle( title.c_str() );
    func_UdscJetResolution_[ key ]->GetXaxis()->SetTitle( xAxisTitle.c_str() );
    func_UdscJetResolution_[ key ]->GetYaxis()->SetTitle( "#sigma_{#eta}" );

    // Phi
    const hitfit::Resolution phiRes( vecRes.phi_res() );
    key  = "Phi_" + my::helpers::to_string( count );
    name = "UdscJet_" + key;
    func_UdscJetResolution_[ key ] = dir_UdscJetResolutionPhi.make< TF1 >( name.c_str(), phiRes.inverse() ? funcResInv.c_str() : funcRes.c_str(), 0., maxJetPt_ );
    func_UdscJetResolution_[ key ]->SetParameters( phiRes.C(), phiRes.R(), phiRes.N() );
//     func_UdscJetResolution_[ key ]->SetRange( 0., maxJetPt_ );
    func_UdscJetResolution_[ key ]->SetMinimum( 0. );
    func_UdscJetResolution_[ key ]->SetMaximum( maxDiffJetPhi_ );
    func_UdscJetResolution_[ key ]->SetTitle( title.c_str() );
    func_UdscJetResolution_[ key ]->GetXaxis()->SetTitle( xAxisTitle.c_str() );
    func_UdscJetResolution_[ key ]->GetYaxis()->SetTitle( "#sigma_{#phi}" );

    ++count;
  }

  // B-jet resolutions

  TFileDirectory dir_BJetResolution( dir_Resolution.mkdir( "BJetResolution", "HitFit resolution functions for b-jets" ) );
  TFileDirectory dir_BJetResolutionP( dir_BJetResolution.mkdir( "BJetResolutionP", "HitFit momentum resolution functions for b-jets" ) );
  TFileDirectory dir_BJetResolutionEta( dir_BJetResolution.mkdir( "BJetResolutionEta", "HitFit eta resolution functions for b-jets" ) );
  TFileDirectory dir_BJetResolutionPhi( dir_BJetResolution.mkdir( "BJetResolutionPhi", "HitFit phi resolution functions for b-jets" ) );

  const edm::FileInPath bJetResFile( bJetResolutions_ );
  const hitfit::EtaDepResolution bJetRes( bJetResFile.fullPath() );
  const std::vector< hitfit::EtaDepResElement > bJetResElems( bJetRes.GetEtaDepResElement() );

  count = 0;
  for ( unsigned iResEl = 0; iResEl < bJetResElems.size(); ++iResEl ) {

    const hitfit::Vector_Resolution vecRes( bJetResElems.at( iResEl ).GetResolution() );

    const double etaMin( bJetResElems.at( iResEl ).EtaMin() );
    const double etaMax( bJetResElems.at( iResEl ).EtaMax() );
    // positive eta only, since everything is symmetric
    if ( etaMin < 0. && etaMax <= 0. ) continue;

    std::string title( my::helpers::to_string( etaMin ) + " #leq #eta #leq " + my::helpers::to_string( etaMax ) );

    // Momentum
    const hitfit::Resolution pRes( vecRes.p_res() );
    std::string key( "P_" + my::helpers::to_string( count ) );
    std::string name( "BJet_" + key );
    std::string xAxisTitle( "p" );
    if ( vecRes.use_et() ) xAxisTitle += "_{t}";
    std::string yAxisTitle( "#sigma_{" + xAxisTitle + "}" );
    xAxisTitle += " (GeV)";
//     std::string function;
//     if ( pRes.inverse() ) function = vecRes.use_et() ? funcResEtInv : funcResInv;
//     else                  function = vecRes.use_et() ? funcResEt    : funcRes;
    std::string function( pRes.inverse() ? funcResInv : funcRes );
    func_BJetResolution_[ key ] = dir_BJetResolutionP.make< TF1 >( name.c_str(), function.c_str(), 0., maxJetPt_ );
    func_BJetResolution_[ key ]->SetParameters( pRes.C(), pRes.R(), pRes.N(), std::min( std::fabs( etaMin ), std::fabs( etaMax ) ) );
//     func_BJetResolution_[ key ]->SetRange( 0., maxJetPt_ );
    func_BJetResolution_[ key ]->SetMinimum(   0. );
    func_BJetResolution_[ key ]->SetMaximum( pRes.inverse() ? maxDiffJetPtInv_ : maxDiffJetPt_ );
    func_BJetResolution_[ key ]->SetTitle( title.c_str() );
    func_BJetResolution_[ key ]->GetXaxis()->SetTitle( xAxisTitle.c_str() );
    func_BJetResolution_[ key ]->GetYaxis()->SetTitle( yAxisTitle.c_str() );
    key  += "_Rel";
    name += "_Rel";
    yAxisTitle = "#frac{" + yAxisTitle + "}{" + xAxisTitle + "}";
    function = "(" + function + ( pRes.inverse() ? ")*x" : ")/x" );
    func_BJetResolution_[ key ] = dir_BJetResolutionP.make< TF1 >( name.c_str(), function.c_str(), 0., maxJetPt_ );
    func_BJetResolution_[ key ]->SetParameters( pRes.C(), pRes.R(), pRes.N(), std::min( std::fabs( etaMin ), std::fabs( etaMax ) ) );
//     func_BJetResolution_[ key ]->SetRange( 0., maxJetPt_ );
    func_BJetResolution_[ key ]->SetMinimum( 0. );
    func_BJetResolution_[ key ]->SetMaximum( pRes.inverse() ? maxDiffJetPtInvRel_ : 1. );
    func_BJetResolution_[ key ]->SetTitle( title.c_str() );
    func_BJetResolution_[ key ]->GetXaxis()->SetTitle( xAxisTitle.c_str() );
    func_BJetResolution_[ key ]->GetYaxis()->SetTitle( yAxisTitle.c_str() );

    // Eta
    const hitfit::Resolution etaRes( vecRes.eta_res() );
    key  = "Eta_" + my::helpers::to_string( count );
    name = "BJet_" + key;
    func_BJetResolution_[ key ] = dir_BJetResolutionEta.make< TF1 >( name.c_str(), etaRes.inverse() ? funcResInv.c_str() : funcRes.c_str(), 0., maxJetPt_ );
    func_BJetResolution_[ key ]->SetParameters( etaRes.C(), etaRes.R(), etaRes.N() );
//     func_BJetResolution_[ key ]->SetRange( 0., maxJetPt_ );
    func_BJetResolution_[ key ]->SetMinimum( 0. );
    func_BJetResolution_[ key ]->SetMaximum( maxDiffJetEta_ );
    func_BJetResolution_[ key ]->SetTitle( title.c_str() );
    func_BJetResolution_[ key ]->GetXaxis()->SetTitle( xAxisTitle.c_str() );
    func_BJetResolution_[ key ]->GetYaxis()->SetTitle( "#sigma_{#eta}" );

    // Phi
    const hitfit::Resolution phiRes( vecRes.phi_res() );
    key  = "Phi_" + my::helpers::to_string( count );
    name = "BJet_" + key;
    func_BJetResolution_[ key ] = dir_BJetResolutionPhi.make< TF1 >( name.c_str(), phiRes.inverse() ? funcResInv.c_str() : funcRes.c_str(), 0., maxJetPt_ );
    func_BJetResolution_[ key ]->SetParameters( phiRes.C(), phiRes.R(), phiRes.N() );
//     func_BJetResolution_[ key ]->SetRange( 0., maxJetPt_ );
    func_BJetResolution_[ key ]->SetMinimum( 0. );
    func_BJetResolution_[ key ]->SetMaximum( maxDiffJetPhi_ );
    func_BJetResolution_[ key ]->SetTitle( title.c_str() );
    func_BJetResolution_[ key ]->GetXaxis()->SetTitle( xAxisTitle.c_str() );
    func_BJetResolution_[ key ]->GetYaxis()->SetTitle( "#sigma_{#phi}" );

    ++count;
  }

  // MET resolutions FIXME

  TFileDirectory dir_METResolution( dir_Resolution.mkdir( "METResolution", "HitFit resolution functions for MET" ) );
  TFileDirectory dir_METResolutionP( dir_METResolution.mkdir( "METResolutionP", "HitFit momentum resolution functions for MET" ) );

  const edm::FileInPath metResFile( metResolutions_ );
  const hitfit::Defaults_Text metDefs( metResFile.fullPath() );
  const hitfit::Resolution metRes( metDefs.get_string( "met_resolution" ) );

  std::string metTitle( "no #eta-dependence" );
  std::string metKey( "P" );
  std::string metName( "MET_" + metKey );
  std::string metXAxisTitle( "p_{t}" );
  std::string metYAxisTitle( "#sigma_{" + metXAxisTitle + "}" );
  metXAxisTitle += " (GeV)";
//   std::string metFunction;
//   if ( metRes.inverse() ) metFunction = vecRes.use_et() ? funcResEtInv : funcResInv;
//   else                  metFunction = vecRes.use_et() ? funcResEt    : funcRes;
  std::string metFunction( metRes.inverse() ? funcResInv : funcRes );
  func_METResolution_[ metKey ] = dir_METResolutionP.make< TF1 >( metName.c_str(), metFunction.c_str(), 0., maxNuPt_ );
  func_METResolution_[ metKey ]->SetParameters( metRes.C(), metRes.R(), metRes.N() );
  func_METResolution_[ metKey ]->SetMinimum( 0. );
  func_METResolution_[ metKey ]->SetMaximum( metRes.inverse() ? maxDiffNuPtInv_ : maxDiffNuPt_ );
  func_METResolution_[ metKey ]->SetTitle( metTitle.c_str() );
  func_METResolution_[ metKey ]->GetXaxis()->SetTitle( metXAxisTitle.c_str() );
  func_METResolution_[ metKey ]->GetYaxis()->SetTitle( metYAxisTitle.c_str() );
  metKey  += "_Rel";
  metName += "_Rel";
  metYAxisTitle = "#frac{" + metYAxisTitle + "}{" + metXAxisTitle + "}";
  metFunction = "(" + metFunction + ( metRes.inverse() ? ")*x" : ")/x" );
  func_METResolution_[ metKey ] = dir_METResolutionP.make< TF1 >( metName.c_str(), metFunction.c_str(), 0., maxNuPt_ );
  func_METResolution_[ metKey ]->SetParameters( metRes.C(), metRes.R(), metRes.N() );
  func_METResolution_[ metKey ]->SetMinimum( 0. );
  func_METResolution_[ metKey ]->SetMaximum( metRes.inverse() ? maxDiffNuPtInvRel_ : 1. );
  func_METResolution_[ metKey ]->SetTitle( metTitle.c_str() );
  func_METResolution_[ metKey ]->GetXaxis()->SetTitle( metXAxisTitle.c_str() );
  func_METResolution_[ metKey ]->GetYaxis()->SetTitle( metYAxisTitle.c_str() );

}


// Event loop
void TopHitFitResolutionFunctionsAnalyzer::analyze( const edm::Event & iEvent, const edm::EventSetup & iSetup )
{
}


// End job
void TopHitFitResolutionFunctionsAnalyzer::endJob()
{
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( TopHitFitResolutionFunctionsAnalyzer );
