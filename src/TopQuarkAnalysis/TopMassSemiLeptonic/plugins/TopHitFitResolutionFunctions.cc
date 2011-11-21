// -*- C++ -*-
//
// Package:    TopMassSemiLeptonic
// Class:      TopHitFitResolutionFunctions
//
// $Id: TopHitFitResolutionFunctions.cc,v 1.1 2011/08/31 14:45:53 vadler Exp $
//
/**
  \class    TopHitFitResolutionFunctions TopHitFitResolutionFunctions.cc "TopQuarkAnalysis/TopMassSemiLeptonic/plugins/TopHitFitResolutionFunctions.cc"
  \brief    Analyses HitFit performance



  \author   Volker Adler
  \version  $Id: TopHitFitResolutionFunctions.cc,v 1.1 2011/08/31 14:45:53 vadler Exp $
*/


#include <map>
#include <vector>
#include <string>

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
#include "TopQuarkAnalysis/TopHitFit/interface/EtaDepResolution.h"


typedef std::map< std::string, TH1D* > MapTH1D;
typedef std::map< std::string, TH2D* > MapTH2D;
typedef std::map< std::string, std::vector< TH1D* > > VecMapTH1D;
typedef std::map< std::string, std::vector< TH2D* > > VecMapTH2D;


class TopHitFitResolutionFunctions : public edm::EDAnalyzer {

  public:

    /// Constructors and Desctructor

    /// Default constructor
    explicit TopHitFitResolutionFunctions( const edm::ParameterSet & iConfig );

    /// Destructor
    virtual ~TopHitFitResolutionFunctions();

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
    unsigned verbosity_;
    edm::InputTag ttSemiLeptonicEventTag_;
    std::vector< std::vector< unsigned > > bins_;
    std::vector< std::vector< double   > > min_;
    std::vector< std::vector< double   > > max_;
    std::vector< std::vector< unsigned > > binsDiff_;
    std::vector< std::vector< double   > > maxDiff_;

    /// Histograms
    MapTH1D histMap1D_;
    MapTH2D histMap2D_;
    VecMapTH1D histVecMap1D_;
    VecMapTH2D histVecMap2D_;

    /// Resolutions
    std::map< std::string, hitfit::EtaDepResolution > res_;

    /// Constants
    std::vector< std::string > objCat_;
    std::vector< std::string > kinProp_;
    std::vector< std::string > kinPropAxisTitle_;
    std::vector< std::string > fileNames_;

    /// Private functions

    void fill1D( unsigned iCat, MapTH1D & histMap1D );
    void fill2D( unsigned iCat, MapTH2D & histMap2D );
    void fill1D( unsigned iCat, VecMapTH1D & histVecMap1D, unsigned iBin = 0 );
    void fill2D( unsigned iCat, VecMapTH2D & histVecMap2D, unsigned iBin = 0 );

};


#include <iostream>
#include <cassert>

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "AnalysisDataFormats/TopObjects/interface/TtSemiLepEvtPartons.h"

#include "TopQuarkAnalysis/TopHitFit/interface/LeptonTranslatorBase.h"
#include "TopQuarkAnalysis/TopHitFit/interface/JetTranslatorBase.h"
#include "TopQuarkAnalysis/TopHitFit/interface/METTranslatorBase.h"

#include "TopQuarkAnalysis/TopMassSemiLeptonic/interface/Helpers.h"


// Default constructor
TopHitFitResolutionFunctions::TopHitFitResolutionFunctions( const edm::ParameterSet & iConfig )
: ttSemiLeptonicEvent_()
, ttGenEvent_()
, verbosity_( iConfig.getParameter< unsigned >( "verbosity" ) )
, ttSemiLeptonicEventTag_( iConfig.getParameter< edm::InputTag >( "ttSemiLeptonicEvent" ) )
, histMap1D_()
, histMap2D_()
, histVecMap1D_()
, histVecMap2D_()
{

  // Naming

  // Object categories
  objCat_.clear();
  objCat_.push_back( "Lep" );
  objCat_.push_back( "UdscJet" );
  objCat_.push_back( "BJet" );
  objCat_.push_back( "Nu" );

  // Kinematic properties
  kinProp_.clear();
  kinProp_.push_back( "Pt" );
  kinProp_.push_back( "PtInv" );
  kinProp_.push_back( "Eta" );
  kinProp_.push_back( "Phi" );
  kinProp_.push_back( "Pz" );
  kinProp_.push_back( "M" );
  // axis titles
  kinPropAxisTitle_.clear();
  kinPropAxisTitle_.push_back( "p_{t} (GeV)" );
  kinPropAxisTitle_.push_back( "#frac{1}{p_{t}} (GeV^{-1})" );
  kinPropAxisTitle_.push_back( "#eta" );
  kinPropAxisTitle_.push_back( "#phi" );
  kinPropAxisTitle_.push_back( "p_{z} (GeV)" );
  kinPropAxisTitle_.push_back( "m (GeV)" );
  assert( kinProp_.size() == kinPropAxisTitle_.size() );

  // Resolution file name parameter names
  // No file name for nu given, since there is no binning
  fileNames_.clear();
  fileNames_.push_back( "lepResolutions" );
  fileNames_.push_back( "udscJetResolutions" );
  fileNames_.push_back( "bJetResolutions" );
  assert( objCat_.size() - fileNames_.size() == 1 );

  // Loop over object categories
  bins_.clear();
  min_.clear();
  max_.clear();
  for ( unsigned iCat = 0; iCat < objCat_.size(); ++iCat ) {
    const std::string objCat( objCat_.at( iCat ) );

    std::vector< unsigned > bins;
    std::vector< double   > min;
    std::vector< double   > max;
    std::vector< unsigned > binsDiff;
    std::vector< double   > maxDiff;
    // Eta-dependent resolutions
    if ( iCat < fileNames_.size() ) {
      res_[ objCat ] = hitfit::EtaDepResolution( edm::FileInPath( iConfig.getParameter< std::string >( fileNames_.at( iCat ) ) ).fullPath() );
    }

    for ( unsigned iProp = 0; iProp < kinProp_.size(); ++iProp ) {
      const std::string kinProp( kinProp_.at( iProp ) );

      std::string confPar( "bins" + objCat + kinProp );
      bins.push_back( iConfig.getParameter< unsigned >( confPar ) );
      confPar.replace( 0, 4, "max" );
      // phi range is [-PI, PI]
      if ( kinProp_.at( iProp ) == "Phi" ) max.push_back( TMath::Pi() );
      else                                 max.push_back( iConfig.getParameter< double >( confPar ) );
      confPar.replace( 0, 3, "min" );
      // eta and phi ranges are symmetric
      if (  kinProp_.at( iProp ) == "Phi"
         || kinProp_.at( iProp ) == "Eta" ) min.push_back( -max.back() );
      else                                  min.push_back( iConfig.getParameter< double >( confPar ) );
      binsDiff.push_back( iConfig.getParameter< unsigned >( confPar.replace( 0, 3, "binsDiff" ) ) );
      maxDiff.push_back( iConfig.getParameter< double >( confPar.replace( 0, 4, "max" ) ) );

      // Resolution related
      std::string key( objCat + kinProp_.at( 0 ) + "_" + objCat + kinProp );
      if ( iCat < fileNames_.size() ) {
        histVecMap2D_[ key ] = std::vector< TH2D* >();
        histVecMap2D_[ key ].reserve( res_[ objCat ].GetEtaDepResElement().size() );
      }
      else {
        histVecMap2D_[ key ] = std::vector< TH2D* >();
        histVecMap2D_[ key ].reserve( 1 );
      }
    }
    bins_.push_back( bins );
    min_.push_back( min );
    max_.push_back( max );
    binsDiff_.push_back( binsDiff );
    maxDiff_.push_back( maxDiff );
  }

}


// Destructor
TopHitFitResolutionFunctions::~TopHitFitResolutionFunctions()
{
}


// Begin job
void TopHitFitResolutionFunctions::beginJob()
{

  edm::Service< TFileService > fileService;

  for ( unsigned iCat = 0; iCat < objCat_.size(); ++iCat ) {
    const std::string objCat( objCat_.at( iCat ) );
    const std::string xTitle( "#Delta" );
    std::string name(  objCat );
    for ( unsigned iProp = 0; iProp < kinProp_.size(); ++iProp ) {
      const std::string kinProp( kinProp_.at( iProp ) );
      const std::string kinPropAxisTitle( kinPropAxisTitle_.at( iProp ) );
      const std::string key( objCat + kinProp );
      const std::string name( objCat + kinProp );
      std::string title( "" );
      std::string axisTitle( xTitle + kinPropAxisTitle );
      std::string key2d( objCat + kinProp_.at( 0 ) + "_" + key );
      std::string name2d( name );
      name2d.insert( 0, objCat + kinProp_.at( 0 ) + "_" );
      std::string title2d( title + " vs. reconstructed transverse momentum" );
      std::string nameDir( name2d );
      TFileDirectory dir( fileService->mkdir( nameDir.c_str(), "" ) );
      // 1-dim
      histMap1D_[ key ] = dir.make< TH1D >( name.c_str(), title.c_str()
                                       , binsDiff_.at( iCat ).at( iProp ), -maxDiff_.at( iCat ).at( iProp ), maxDiff_.at( iCat ).at( iProp ) );
      histMap1D_[ key ]->SetXTitle( axisTitle.c_str() );
      histMap1D_[ key ]->SetYTitle( "events" );
      edm::LogInfo( "TopHitFitResolutionFunctions" ) << "Created histogram " << name << " with key " << key;
      // 2-dim
      histMap2D_[ key2d ] = dir.make< TH2D >( name2d.c_str(), title2d.c_str()
                                         , bins_.at( iCat ).at( 0 ), 0., max_.at( iCat ).at( 0 )
                                         , binsDiff_.at( iCat ).at( iProp ), -maxDiff_.at( iCat ).at( iProp ), maxDiff_.at( iCat ).at( iProp ) );
      histMap2D_[ key2d ]->SetXTitle( "p_{t} (GeV)" );
      histMap2D_[ key2d ]->SetYTitle( axisTitle.c_str() );
      histMap2D_[ key2d ]->SetZTitle( "events" );
      edm::LogInfo( "TopHitFitResolutionFunctions" ) << "Created histogram " << name2d << " with key " << key2d;
      // eta bins
      if ( iCat < fileNames_.size() ) {
        for ( unsigned iBin = 0; iBin < res_[ objCat ].GetEtaDepResElement().size(); ++iBin ) {
          // 1-dim
          std::string nameBin( name + "_" + my::helpers::to_string( iBin ) );
          std::string titleBin( title + ", " + my::helpers::to_string( res_[ objCat ].GetEtaDepResElement().at( iBin ).EtaMin() ) + " #leq #eta #leq " + my::helpers::to_string( res_[ objCat ].GetEtaDepResElement().at( iBin ).EtaMax() ) );
          TH1D * hist1D = dir.make< TH1D >( *( ( TH1D* )( histMap1D_[ key ]->Clone( nameBin.c_str() ) ) ) );
          hist1D->SetTitle( titleBin.c_str() );
          histVecMap1D_[ key ].push_back( hist1D );
          edm::LogInfo( "TopHitFitResolutionFunctions" ) << "Created histogram " << nameBin << " with key " << key << " and index " << histVecMap1D_[ key ].size() - 1;
          // 2-dim
          std::string nameBin2d( name2d + "_" + my::helpers::to_string( iBin ) );
          std::string titleBin2d( title2d + ", " + my::helpers::to_string( res_[ objCat ].GetEtaDepResElement().at( iBin ).EtaMin() ) + " #leq #eta #leq " + my::helpers::to_string( res_[ objCat ].GetEtaDepResElement().at( iBin ).EtaMax() ) );
          TH2D * hist2D = dir.make< TH2D >( *( ( TH2D* )( histMap2D_[ key2d ]->Clone( nameBin2d.c_str() ) ) ) );
          hist2D->SetTitle( titleBin2d.c_str() );
          histVecMap2D_[ key2d ].push_back( hist2D );
          edm::LogInfo( "TopHitFitResolutionFunctions" ) << "Created histogram " << nameBin2d << " with key " << key2d << " and index " << histVecMap2D_[ key2d ].size() - 1;
        }
      }
      else {
        // 1-dim
        std::string nameBin( name + "_0" );
        TH1D * hist1D = dir.make< TH1D >( *( ( TH1D* )( histMap1D_[ key ]->Clone( nameBin.c_str() ) ) ) );
        histVecMap1D_[ key ].push_back( hist1D );
        edm::LogInfo( "TopHitFitResolutionFunctions" ) << "Created histogram " << nameBin << " with key " << key << " and index " << histVecMap1D_[ key ].size() - 1;
        // 2-dim
        std::string nameBin2d( name2d + "_0" );
        TH2D * hist2D = dir.make< TH2D >( *( ( TH2D* )( histMap2D_[ key2d ]->Clone( nameBin2d.c_str() ) ) ) );
        histVecMap2D_[ key2d ].push_back( hist2D );
        edm::LogInfo( "TopHitFitResolutionFunctions" ) << "Created histogram " << nameBin2d << " with key " << key2d << " and index " << histVecMap2D_[ key2d ].size() - 1;
      }
    }
  }

}


// Event loop
void TopHitFitResolutionFunctions::analyze( const edm::Event & iEvent, const edm::EventSetup & iSetup )
{

  // Reset
  ttGenEvent_ =  0;

  // TQAF semi-leptonic event
  iEvent.getByLabel( ttSemiLeptonicEventTag_, ttSemiLeptonicEvent_ );
  ttSemiLeptonicEvent_->print( verbosity_ );

  if ( ! iEvent.isRealData() ) {

    // TQAF MC event
    ttGenEvent_ = const_cast< TtGenEvent* >( ttSemiLeptonicEvent_->genEvent().get() );

    if ( ttGenEvent_->isTtBar() && ttGenEvent_->isSemiLeptonic( WDecay::kMuon ) ) { // semi-muonic signal event

      if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kGenMatch ) ) {
        for ( unsigned iCat = 0; iCat < objCat_.size(); ++iCat ) {
          const std::string objCat( objCat_.at( iCat ) );
          fill1D( iCat, histMap1D_ );
          fill2D( iCat, histMap2D_ );
          if ( iCat < fileNames_.size() ) {
            const unsigned nBins( res_[ objCat ].GetEtaDepResElement().size() );
            for ( unsigned iBin = 0; iBin < nBins; ++iBin ) {
              fill1D( iCat, histVecMap1D_, iBin );
              fill2D( iCat, histVecMap2D_, iBin );
            }
          }
          else {
            fill1D( iCat, histVecMap1D_ );
            fill2D( iCat, histVecMap2D_ );
          }
        }
      }

    }

  }

}


// End job
void TopHitFitResolutionFunctions::endJob()
{
}


// Filling deviations of RECO from GEN for kinematic properties of object types
void TopHitFitResolutionFunctions::fill1D( unsigned iCat, MapTH1D & histMap1D )
{

  const unsigned nCats( objCat_.size() );
  const std::string objCat( objCat_.at( iCat ) );
  switch ( iCat ) {
    case 0:
      histMap1D[ objCat + "Pt" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt() - ttGenEvent_->singleLepton()->pt() );
      histMap1D[ objCat + "PtInv" ]->Fill( 1./( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt() ) - 1./( ttGenEvent_->singleLepton()->pt() ) );
      histMap1D[ objCat + "Eta" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->eta() - ttGenEvent_->singleLepton()->eta() );
      histMap1D[ objCat + "Phi" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->phi() - ttGenEvent_->singleLepton()->phi() );
      histMap1D[ objCat + "Pz" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pz() - ttGenEvent_->singleLepton()->pz() );
      histMap1D[ objCat + "M" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->mass() - ttGenEvent_->singleLepton()->mass() );
      break;
    case 1:
      histMap1D[ objCat + "Pt" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pt() - ttGenEvent_->hadronicDecayQuark()->pt() );
      histMap1D[ objCat + "Pt" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pt() - ttGenEvent_->hadronicDecayQuarkBar()->pt() );
      histMap1D[ objCat + "PtInv" ]->Fill( 1./( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pt() ) - 1./( ttGenEvent_->hadronicDecayQuark()->pt() ) );
      histMap1D[ objCat + "PtInv" ]->Fill( 1./( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pt() ) - 1./( ttGenEvent_->hadronicDecayQuarkBar()->pt() ) );
      histMap1D[ objCat + "Eta" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->eta() - ttGenEvent_->hadronicDecayQuark()->eta() );
      histMap1D[ objCat + "Eta" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->eta() - ttGenEvent_->hadronicDecayQuarkBar()->eta() );
      histMap1D[ objCat + "Phi" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->phi() - ttGenEvent_->hadronicDecayQuark()->phi() );
      histMap1D[ objCat + "Phi" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->phi() - ttGenEvent_->hadronicDecayQuarkBar()->phi() );
      histMap1D[ objCat + "Pz" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pz() - ttGenEvent_->hadronicDecayQuark()->pz() );
      histMap1D[ objCat + "Pz" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pz() - ttGenEvent_->hadronicDecayQuarkBar()->pz() );
      histMap1D[ objCat + "M" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->mass() - ttGenEvent_->hadronicDecayQuark()->mass() );
      histMap1D[ objCat + "M" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->mass() - ttGenEvent_->hadronicDecayQuarkBar()->mass() );
      break;
    case 2:
      histMap1D[ objCat + "Pt" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pt() - ttGenEvent_->hadronicDecayB()->pt() );
      histMap1D[ objCat + "Pt" ]->Fill( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pt() - ttGenEvent_->leptonicDecayB()->pt() );
      histMap1D[ objCat + "PtInv" ]->Fill( 1./( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pt() ) - 1./( ttGenEvent_->hadronicDecayB()->pt() ) );
      histMap1D[ objCat + "PtInv" ]->Fill( 1./( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pt() ) - 1./( ttGenEvent_->leptonicDecayB()->pt() ) );
      histMap1D[ objCat + "Eta" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->eta() - ttGenEvent_->hadronicDecayB()->eta() );
      histMap1D[ objCat + "Eta" ]->Fill( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->eta() - ttGenEvent_->leptonicDecayB()->eta() );
      histMap1D[ objCat + "Phi" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->phi() - ttGenEvent_->hadronicDecayB()->phi() );
      histMap1D[ objCat + "Phi" ]->Fill( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->phi() - ttGenEvent_->leptonicDecayB()->phi() );
      histMap1D[ objCat + "Pz" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pz() - ttGenEvent_->hadronicDecayB()->pz() );
      histMap1D[ objCat + "Pz" ]->Fill( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pz() - ttGenEvent_->leptonicDecayB()->pz() );
      histMap1D[ objCat + "M" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->mass() - ttGenEvent_->hadronicDecayB()->mass() );
      histMap1D[ objCat + "M" ]->Fill( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->mass() - ttGenEvent_->leptonicDecayB()->mass() );
      break;
    case 3:
      histMap1D[ objCat + "Pt" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt() - ttGenEvent_->singleNeutrino()->pt() );
      histMap1D[ objCat + "PtInv" ]->Fill( 1./( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt() ) - 1./( ttGenEvent_->singleNeutrino()->pt() ) );
      histMap1D[ objCat + "Eta" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->eta() - ttGenEvent_->singleNeutrino()->eta() );
      histMap1D[ objCat + "Phi" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->phi() - ttGenEvent_->singleNeutrino()->phi() );
      histMap1D[ objCat + "Pz" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pz() - ttGenEvent_->singleNeutrino()->pz() );
      histMap1D[ objCat + "M" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->mass() - ttGenEvent_->singleNeutrino()->mass() );
      break;
    default:
      edm::LogError( "TopHitFitResolutionFunctions" ) << "Trying to access object category " << iCat << " (" << nCats << " categories)";
      break;
  }

  return;

}

void TopHitFitResolutionFunctions::fill2D( unsigned iCat, MapTH2D & histMap2D )
{

  const unsigned nCats( objCat_.size() );
  const std::string objCat( objCat_.at( iCat ) );
  switch ( iCat ) {
    case 0:
      histMap2D[ objCat + "Pt_" +objCat  + "Pt" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt() - ttGenEvent_->singleLepton()->pt() );
      histMap2D[ objCat + "Pt_" +objCat  + "PtInv" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt(), 1./( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt() ) - 1./( ttGenEvent_->singleLepton()->pt() ) );
      histMap2D[ objCat + "Pt_" +objCat  + "Eta" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->eta() - ttGenEvent_->singleLepton()->eta() );
      histMap2D[ objCat + "Pt_" +objCat  + "Phi" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->phi() - ttGenEvent_->singleLepton()->phi() );
      histMap2D[ objCat + "Pt_" +objCat  + "Pz" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pz() - ttGenEvent_->singleLepton()->pz() );
      histMap2D[ objCat + "Pt_" +objCat  + "M" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->mass() - ttGenEvent_->singleLepton()->mass() );
      break;
    case 1:
      histMap2D[ objCat + "Pt_" +objCat  + "Pt" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pt() - ttGenEvent_->hadronicDecayQuark()->pt() );
      histMap2D[ objCat + "Pt_" +objCat  + "Pt" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pt() - ttGenEvent_->hadronicDecayQuarkBar()->pt() );
      histMap2D[ objCat + "Pt_" +objCat  + "PtInv" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pt(), 1./( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pt() ) - 1./( ttGenEvent_->hadronicDecayQuark()->pt() ) );
      histMap2D[ objCat + "Pt_" +objCat  + "PtInv" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pt(), 1./( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pt() ) - 1./( ttGenEvent_->hadronicDecayQuarkBar()->pt() ) );
      histMap2D[ objCat + "Pt_" +objCat  + "Eta" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->eta() - ttGenEvent_->hadronicDecayQuark()->eta() );
      histMap2D[ objCat + "Pt_" +objCat  + "Eta" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->eta() - ttGenEvent_->hadronicDecayQuarkBar()->eta() );
      histMap2D[ objCat + "Pt_" +objCat  + "Phi" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->phi() - ttGenEvent_->hadronicDecayQuark()->phi() );
      histMap2D[ objCat + "Pt_" +objCat  + "Phi" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->phi() - ttGenEvent_->hadronicDecayQuarkBar()->phi() );
      histMap2D[ objCat + "Pt_" +objCat  + "Pz" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pz() - ttGenEvent_->hadronicDecayQuark()->pz() );
      histMap2D[ objCat + "Pt_" +objCat  + "Pz" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pz() - ttGenEvent_->hadronicDecayQuarkBar()->pz() );
      histMap2D[ objCat + "Pt_" +objCat  + "M" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->mass() - ttGenEvent_->hadronicDecayQuark()->mass() );
      histMap2D[ objCat + "Pt_" +objCat  + "M" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->mass() - ttGenEvent_->hadronicDecayQuarkBar()->mass() );
      break;
    case 2:
      histMap2D[ objCat + "Pt_" +objCat  + "Pt" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pt() - ttGenEvent_->hadronicDecayB()->pt() );
      histMap2D[ objCat + "Pt_" +objCat  + "Pt" ]->Fill( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pt() - ttGenEvent_->leptonicDecayB()->pt() );
      histMap2D[ objCat + "Pt_" +objCat  + "PtInv" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pt(), 1./( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pt() ) - 1./( ttGenEvent_->hadronicDecayB()->pt() ) );
      histMap2D[ objCat + "Pt_" +objCat  + "PtInv" ]->Fill( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pt(), 1./( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pt() ) - 1./( ttGenEvent_->leptonicDecayB()->pt() ) );
      histMap2D[ objCat + "Pt_" +objCat  + "Eta" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->eta() - ttGenEvent_->hadronicDecayB()->eta() );
      histMap2D[ objCat + "Pt_" +objCat  + "Eta" ]->Fill( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->eta() - ttGenEvent_->leptonicDecayB()->eta() );
      histMap2D[ objCat + "Pt_" +objCat  + "Phi" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->phi() - ttGenEvent_->hadronicDecayB()->phi() );
      histMap2D[ objCat + "Pt_" +objCat  + "Phi" ]->Fill( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->phi() - ttGenEvent_->leptonicDecayB()->phi() );
      histMap2D[ objCat + "Pt_" +objCat  + "Pz" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pz() - ttGenEvent_->hadronicDecayB()->pz() );
      histMap2D[ objCat + "Pt_" +objCat  + "Pz" ]->Fill( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pz() - ttGenEvent_->leptonicDecayB()->pz() );
      histMap2D[ objCat + "Pt_" +objCat  + "M" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->mass() - ttGenEvent_->hadronicDecayB()->mass() );
      histMap2D[ objCat + "Pt_" +objCat  + "M" ]->Fill( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->mass() - ttGenEvent_->leptonicDecayB()->mass() );
      break;
    case 3:
      histMap2D[ objCat + "Pt_" +objCat  + "Pt" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt() - ttGenEvent_->singleNeutrino()->pt() );
      histMap2D[ objCat + "Pt_" +objCat  + "PtInv" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt(), 1./( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt() ) - 1./( ttGenEvent_->singleNeutrino()->pt() ) );
      histMap2D[ objCat + "Pt_" +objCat  + "Eta" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->eta() - ttGenEvent_->singleNeutrino()->eta() );
      histMap2D[ objCat + "Pt_" +objCat  + "Phi" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->phi() - ttGenEvent_->singleNeutrino()->phi() );
      histMap2D[ objCat + "Pt_" +objCat  + "Pz" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pz() - ttGenEvent_->singleNeutrino()->pz() );
      histMap2D[ objCat + "Pt_" +objCat  + "M" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt(), ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->mass() - ttGenEvent_->singleNeutrino()->mass() );
      break;
    default:
      edm::LogError( "TopHitFitResolutionFunctions" ) << "Trying to access object category " << iCat << " (" << nCats << " categories)";
      break;
  }

  return;

}

void TopHitFitResolutionFunctions::fill1D( unsigned iCat, VecMapTH1D & histVecMap1D, unsigned iBin )
{

  const std::string objCat( objCat_.at( iCat ) );
  MapTH1D histMap1D;
  for ( VecMapTH1D::const_iterator iMap = histVecMap1D.begin(); iMap != histVecMap1D.end(); ++iMap ) {
    if ( iMap->first.substr( 0, objCat.size() ) == objCat ) {
      const unsigned nBins( iMap->second.size() );
      if ( iBin < nBins ) {
        histMap1D[ iMap->first ] = iMap->second.at( iBin );
      }
      else {
        edm::LogWarning( "TopHitFitResolutionFunctions" ) << "Trying to access bin " << iBin << " for object category " << objCat << " (" << nBins << " bins)";
        continue;
      }
    }
  }
  if ( histMap1D.size() == kinProp_.size() ) {
    fill1D( iCat, histMap1D );
  }
  else {
    edm::LogError( "TopHitFitResolutionFunctions" ) << "Found " << histMap1D.size() << " histos in object category " << objCat << " instead of " << kinProp_.size();
  }

  return;

}

void TopHitFitResolutionFunctions::fill2D( unsigned iCat, VecMapTH2D & histVecMap2D, unsigned iBin )
{

  const std::string objCat( objCat_.at( iCat ) );
  MapTH2D histMap2D;
  for ( VecMapTH2D::const_iterator iMap = histVecMap2D.begin(); iMap != histVecMap2D.end(); ++iMap ) {
    if ( iMap->first.substr( 0, objCat.size() ) == objCat ) {
      const unsigned nBins( iMap->second.size() );
      if ( iBin < nBins ) {
        histMap2D[ iMap->first ] = iMap->second.at( iBin );
      }
      else {
        edm::LogWarning( "TopHitFitResolutionFunctions" ) << "Trying to access bin " << iBin << " for object category " << objCat << " (" << nBins << " bins)";
        continue;
      }
    }
  }
  if ( histMap2D.size() == kinProp_.size() ) {
    fill2D( iCat, histMap2D );
  }
  else {
    edm::LogError( "TopHitFitResolutionFunctions" ) << "Found " << histMap2D.size() << " histos in object category " << objCat << " instead of " << kinProp_.size();
  }

  return;

}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( TopHitFitResolutionFunctions );
