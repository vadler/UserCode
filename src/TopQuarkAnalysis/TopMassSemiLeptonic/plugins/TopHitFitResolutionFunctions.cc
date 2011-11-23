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
#include "TF1.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
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
    // PAT jets
    edm::Handle< pat::JetCollection > patJets_; // direct access needed to deal with JECs

    /// Configuration parameters
    edm::InputTag ttSemiLeptonicEventTag_;
    edm::InputTag patJetsTag_;
    std::string jecLevel_;
    std::vector< std::vector< double > > binsPt_;
    std::vector< std::vector< unsigned > > binsDiff_;
    std::vector< std::vector< double   > > maxDiff_;

    /// Histograms
    MapTH1D histMap1D_;
    MapTH2D histMap2D_;
    VecMapTH1D histVecMap1D_;
    VecMapTH2D histVecMap2D_;

    MapTH1D histMap1DRel_;
    MapTH2D histMap2DRel_;
    VecMapTH1D histVecMap1DRel_;
    VecMapTH2D histVecMap2DRel_;

    /// Resolutions
    std::map< std::string, hitfit::EtaDepResolution > res_;

    /// Constants
    std::vector< std::string > objCat_;
    std::vector< std::string > kinProp_;
    std::vector< std::string > kinPropAxisTitle_;
    std::vector< std::string > fileNames_;

    /// Private functions

    bool checkCatBin( unsigned iCat, unsigned iBin );
    std::pair< double, double > getEtaLimits( unsigned iCat, unsigned iBin );

    MapTH1D buildHistMap1D( unsigned iCat, VecMapTH1D & histVecMap1D, unsigned iBin );
    MapTH2D buildHistMap2D( unsigned iCat, VecMapTH2D & histVecMap2D, unsigned iBin );

    void fillRel( unsigned iCat, MapTH1D & histMap1DRel, unsigned iBin = 0 );
    void fillRel( unsigned iCat, MapTH2D & histMap2DRel, unsigned iBin = 0 );
    void fillRel( unsigned iCat, VecMapTH1D & histVecMap1DRel, unsigned iBin = 0 );
    void fillRel( unsigned iCat, VecMapTH2D & histVecMap2DRel, unsigned iBin = 0 );

    void fill( unsigned iCat, MapTH1D & histMap1D, unsigned iBin = 0 );
    void fill( unsigned iCat, MapTH2D & histMap2D, unsigned iBin = 0 );
    void fill( unsigned iCat, VecMapTH1D & histVecMap1D, unsigned iBin = 0 );
    void fill( unsigned iCat, VecMapTH2D & histVecMap2D, unsigned iBin = 0 );

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
, patJets_()
, ttSemiLeptonicEventTag_( iConfig.getParameter< edm::InputTag >( "ttSemiLeptonicEvent" ) )
, patJetsTag_( iConfig.getParameter< edm::InputTag >( "patJets" ) )
, jecLevel_( iConfig.getParameter< std::string >( "jecLevel" ) )
, histMap1D_()
, histMap2D_()
, histVecMap1D_()
, histVecMap2D_()
, histMap1DRel_()
, histMap2DRel_()
, histVecMap1DRel_()
, histVecMap2DRel_()
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
  binsPt_.clear();
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

    binsPt_.push_back( iConfig.getParameter< std::vector< double > >( "bins" + objCat + "Pt" ) );

    for ( unsigned iProp = 0; iProp < kinProp_.size(); ++iProp ) {
      const std::string kinProp( kinProp_.at( iProp ) );

      std::string confPar( "binsDiff" + objCat + kinProp );
      binsDiff.push_back( iConfig.getParameter< unsigned >( confPar ) );
      maxDiff.push_back( iConfig.getParameter< double >( confPar.replace( 0, 4, "max" ) ) );

      // Resolution related
      std::string key( objCat + kinProp_.at( 0 ) + "_" + objCat + kinProp );
      if ( iCat < fileNames_.size() ) {
        histVecMap2D_[ key ] = std::vector< TH2D* >();
        histVecMap2D_[ key ].reserve( res_[ objCat ].GetEtaDepResElement().size() );
        histVecMap2DRel_[ key ] = std::vector< TH2D* >();
        histVecMap2DRel_[ key ].reserve( res_[ objCat ].GetEtaDepResElement().size() );
      }
      else {
        histVecMap2D_[ key ] = std::vector< TH2D* >();
        histVecMap2D_[ key ].reserve( 1 );
        histVecMap2DRel_[ key ] = std::vector< TH2D* >();
        histVecMap2DRel_[ key ].reserve( 1 );
      }
    }
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
    const std::string xTitle( "#Delta" ); // FIXME
    std::string name(  objCat );
    for ( unsigned iProp = 0; iProp < kinProp_.size(); ++iProp ) {
      const std::string kinProp( kinProp_.at( iProp ) );
      const std::string kinPropAxisTitle( kinPropAxisTitle_.at( iProp ) );
      // 1-dim
      const std::string key( objCat + kinProp );
      const std::string name( objCat + kinProp );
      const std::string title( "" ); // FIXME
      const std::string axisTitle( xTitle + kinPropAxisTitle );
      histMap1D_[ key ] = fileService->make< TH1D >( name.c_str(), title.c_str()
                                                   , binsDiff_.at( iCat ).at( iProp ), -maxDiff_.at( iCat ).at( iProp ), maxDiff_.at( iCat ).at( iProp ) );
      histMap1D_[ key ]->SetXTitle( axisTitle.c_str() );
      histMap1D_[ key ]->SetYTitle( "events" );
      edm::LogInfo( "TopHitFitResolutionFunctions" ) << "Created histogram " << name;
      const std::string nameRel( name + "_Rel" );
      const std::string titleRel( "relative " + title );
      histMap1DRel_[ key ] = fileService->make< TH1D >( nameRel.c_str(), titleRel.c_str()
                                                      , binsDiff_.at( iCat ).at( iProp ), -0.1, 0.1 );
      histMap1DRel_[ key ]->SetXTitle( "" ); // FIXME
      histMap1DRel_[ key ]->SetYTitle( "events" );
      edm::LogInfo( "TopHitFitResolutionFunctions" ) << "Created histogram " << nameRel;
      // 2-dim
      const std::string key2d( objCat + kinProp_.at( 0 ) + "_" + key );
      const std::string name2d( objCat + kinProp_.at( 0 ) + "_" + name );
      const std::string title2d( title + " vs. reconstructed transverse momentum" );
      histMap2D_[ key2d ] = fileService->make< TH2D >( name2d.c_str(), title2d.c_str()
                                                     , binsPt_.at( iCat ).size() - 1, binsPt_.at( iCat ).data()
                                                     , binsDiff_.at( iCat ).at( iProp ), -maxDiff_.at( iCat ).at( iProp ), maxDiff_.at( iCat ).at( iProp ) );
      histMap2D_[ key2d ]->SetXTitle( "p_{t} (GeV)" );
      histMap2D_[ key2d ]->SetYTitle( axisTitle.c_str() );
      histMap2D_[ key2d ]->SetZTitle( "events" );
      edm::LogInfo( "TopHitFitResolutionFunctions" ) << "Created histogram " << name2d;
      const std::string name2dRel( name2d + "_Rel" );
      const std::string title2dRel( "relative " + title2d );
      histMap2DRel_[ key2d ] = fileService->make< TH2D >( name2dRel.c_str(), title2dRel.c_str()
                                                      , binsPt_.at( iCat ).size() - 1, binsPt_.at( iCat ).data()
                                                      , binsDiff_.at( iCat ).at( iProp ), -0.1, 0.1 );
      histMap2DRel_[ key2d ]->SetXTitle( "p_{t} (GeV)" );
      histMap2DRel_[ key2d ]->SetYTitle( "" ); // FIXME
      histMap2DRel_[ key2d ]->SetZTitle( "events" );
      edm::LogInfo( "TopHitFitResolutionFunctions" ) << "Created histogram " << name2dRel;
      // eta bins
      const std::string nameDir( "Dir_" + name );
      TFileDirectory dir( fileService->mkdir( nameDir.c_str(), "" ) );
      unsigned iBin( 0 );
      if ( iCat < fileNames_.size() ) {
        for ( ; iBin < res_[ objCat ].GetEtaDepResElement().size(); ++iBin ) {
          std::pair< double, double > etaLimits( getEtaLimits( iCat, iBin ) );
          // 1-dim
          std::string nameBin( name + "_" + my::helpers::to_string( iBin ) );
          std::string titleBin( title + ", " + my::helpers::to_string( etaLimits.first ) + " #leq #eta #leq " + my::helpers::to_string( etaLimits.second ) );
          TH1D * hist1D = dir.make< TH1D >( *( ( TH1D* )( histMap1D_[ key ]->Clone( nameBin.c_str() ) ) ) );
          hist1D->SetTitle( titleBin.c_str() );
          histVecMap1D_[ key ].push_back( hist1D );
          edm::LogInfo( "TopHitFitResolutionFunctions" ) << "Created histogram " << nameBin << " for bin " << iBin;
          TH1D * hist1DRel = dir.make< TH1D >( *( ( TH1D* )( histMap1DRel_[ key ]->Clone( nameBin.append( "_Rel" ).c_str() ) ) ) );
          hist1DRel->SetTitle( titleBin.insert( 0, "relative " ).c_str() );
          histVecMap1DRel_[ key ].push_back( hist1DRel );
          edm::LogInfo( "TopHitFitResolutionFunctions" ) << "Created histogram " << nameBin << " for bin " << iBin;
          // 2-dim
          std::string nameBin2d( name2d + "_" + my::helpers::to_string( iBin ) );
          std::string titleBin2d( title2d + ", " + my::helpers::to_string( etaLimits.first ) + " #leq #eta #leq " + my::helpers::to_string( etaLimits.second ) );
          TH2D * hist2D = dir.make< TH2D >( *( ( TH2D* )( histMap2D_[ key2d ]->Clone( nameBin2d.c_str() ) ) ) );
          hist2D->SetTitle( titleBin2d.c_str() );
          histVecMap2D_[ key2d ].push_back( hist2D );
          edm::LogInfo( "TopHitFitResolutionFunctions" ) << "Created histogram " << nameBin2d << " for bin " << iBin;
          TH2D * hist2DRel = dir.make< TH2D >( *( ( TH2D* )( histMap2DRel_[ key2d ]->Clone( nameBin2d.append( "_Rel" ).c_str() ) ) ) );
          hist2DRel->SetTitle( titleBin2d.insert( 0, "relative " ).c_str() );
          histVecMap2DRel_[ key2d ].push_back( hist2DRel );
          edm::LogInfo( "TopHitFitResolutionFunctions" ) << "Created histogram " << nameBin2d << " for bin " << iBin;
        }
      }
      else {
        // 1-dim
        std::string nameBin( name + "_" + my::helpers::to_string( iBin ) );
        TH1D * hist1D = dir.make< TH1D >( *( ( TH1D* )( histMap1D_[ key ]->Clone( nameBin.c_str() ) ) ) );
        histVecMap1D_[ key ].push_back( hist1D );
        edm::LogInfo( "TopHitFitResolutionFunctions" ) << "Created histogram " << nameBin << " for bin " << iBin;
        TH1D * hist1DRel = dir.make< TH1D >( *( ( TH1D* )( histMap1DRel_[ key ]->Clone( nameBin.append( "_Rel" ).c_str() ) ) ) );
        histVecMap1DRel_[ key ].push_back( hist1DRel );
        edm::LogInfo( "TopHitFitResolutionFunctions" ) << "Created histogram " << nameBin << " for bin " << iBin;
        // 2-dim
        std::string nameBin2d( name2d + "_" + my::helpers::to_string( iBin ) );
        TH2D * hist2D = dir.make< TH2D >( *( ( TH2D* )( histMap2D_[ key2d ]->Clone( nameBin2d.c_str() ) ) ) );
        histVecMap2D_[ key2d ].push_back( hist2D );
        edm::LogInfo( "TopHitFitResolutionFunctions" ) << "Created histogram " << nameBin2d << " for bin " << iBin;
        TH2D * hist2DRel = dir.make< TH2D >( *( ( TH2D* )( histMap2DRel_[ key2d ]->Clone( nameBin2d.append( "_Rel" ).c_str() ) ) ) );
        histVecMap2DRel_[ key2d ].push_back( hist2DRel );
        edm::LogInfo( "TopHitFitResolutionFunctions" ) << "Created histogram " << nameBin2d << " for bin " << iBin;
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

  // PAT jets
  iEvent.getByLabel( patJetsTag_, patJets_ );

  if ( ! iEvent.isRealData() ) {

    // TQAF MC event
    ttGenEvent_ = const_cast< TtGenEvent* >( ttSemiLeptonicEvent_->genEvent().get() );

    if ( ttGenEvent_->isTtBar() && ttGenEvent_->isSemiLeptonic( WDecay::kMuon ) ) { // semi-muonic signal event

      if ( ttSemiLeptonicEvent_->isHypoValid( TtEvent::kGenMatch ) ) {
        for ( unsigned iCat = 0; iCat < objCat_.size(); ++iCat ) {
          const std::string objCat( objCat_.at( iCat ) );
          const unsigned nBins = iCat < fileNames_.size() ? res_[ objCat ].GetEtaDepResElement().size() : 1;
          fill( iCat, histMap1D_, nBins );
          fill( iCat, histMap2D_, nBins );
          fillRel( iCat, histMap1DRel_, nBins );
          fillRel( iCat, histMap2DRel_, nBins );
          for ( unsigned iBin = 0; iBin < nBins; ++iBin ) {
            fill( iCat, histVecMap1D_, iBin );
            fill( iCat, histVecMap2D_, iBin );
            fillRel( iCat, histVecMap1DRel_, iBin );
            fillRel( iCat, histVecMap2DRel_, iBin );
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


bool TopHitFitResolutionFunctions::checkCatBin( unsigned iCat, unsigned iBin )
{

  if ( iCat >= objCat_.size() ) {
    edm::LogError( "TopHitFitResolutionFunctions" ) << "Trying to access object category " << iCat << " (" << objCat_.size() << " categories)";
    return false;
  }
  const std::string objCat( objCat_.at( iCat ) );
  const unsigned nBins( iCat < fileNames_.size() ? res_[ objCat ].GetEtaDepResElement().size() : 1 );
  if ( iBin > nBins ) {
    edm::LogError( "TopHitFitResolutionFunctions" ) << "Trying to access bin " << iBin << " for object category " << objCat << " (" << nBins << " bins)";
    return false;
  }

  return true;

}

std::pair< double, double > TopHitFitResolutionFunctions::getEtaLimits( unsigned iCat, unsigned iBin )
{

  const std::string objCat( objCat_.at( iCat ) );
  const unsigned nBins( iCat < fileNames_.size() ? res_[ objCat ].GetEtaDepResElement().size() : 1 );
  double etaMin( -99999. );
  double etaMax(  99999. );
  if ( iCat < fileNames_.size() && iBin < nBins ) {
    etaMin = res_[ objCat ].GetEtaDepResElement().at( iBin ).EtaMin();
    etaMax = res_[ objCat ].GetEtaDepResElement().at( iBin ).EtaMax();
  }

  return std::make_pair< double, double >( etaMin, etaMax );

}


MapTH1D TopHitFitResolutionFunctions::buildHistMap1D( unsigned iCat, VecMapTH1D & histVecMap1D, unsigned iBin )
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

  return histMap1D;

}

MapTH2D TopHitFitResolutionFunctions::buildHistMap2D( unsigned iCat, VecMapTH2D & histVecMap2D, unsigned iBin )
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

  return histMap2D;

}


void TopHitFitResolutionFunctions::fill( unsigned iCat, VecMapTH1D & histVecMap1D, unsigned iBin )
{

  const std::string objCat( objCat_.at( iCat ) );
  MapTH1D histMap1D( buildHistMap1D( iCat, histVecMap1D, iBin ) );
  if ( histMap1D.size() == kinProp_.size() ) {
    fill( iCat, histMap1D, iBin );
  }
  else {
    edm::LogError( "TopHitFitResolutionFunctions" ) << "Found " << histMap1D.size() << " histos in object category " << objCat << " instead of " << kinProp_.size();
  }

  return;

}

void TopHitFitResolutionFunctions::fillRel( unsigned iCat, VecMapTH1D & histVecMap1DRel, unsigned iBin )
{

  const std::string objCat( objCat_.at( iCat ) );
  MapTH1D histMap1D( buildHistMap1D( iCat, histVecMap1DRel, iBin ) );
  if ( histMap1D.size() == kinProp_.size() ) {
    fillRel( iCat, histMap1D, iBin );
  }
  else {
    edm::LogError( "TopHitFitResolutionFunctions" ) << "Found " << histMap1D.size() << " histos in object category " << objCat << " instead of " << kinProp_.size();
  }

  return;

}

void TopHitFitResolutionFunctions::fill( unsigned iCat, VecMapTH2D & histVecMap2D, unsigned iBin )
{

  const std::string objCat( objCat_.at( iCat ) );
  MapTH2D histMap2D( buildHistMap2D( iCat, histVecMap2D, iBin ) );
  if ( histMap2D.size() == kinProp_.size() ) {
    fill( iCat, histMap2D, iBin );
  }
  else {
    edm::LogError( "TopHitFitResolutionFunctions" ) << "Found " << histMap2D.size() << " histos in object category " << objCat << " instead of " << kinProp_.size();
  }

  return;

}

void TopHitFitResolutionFunctions::fillRel( unsigned iCat, VecMapTH2D & histVecMap2DRel, unsigned iBin )
{

  const std::string objCat( objCat_.at( iCat ) );
  MapTH2D histMap2D( buildHistMap2D( iCat, histVecMap2DRel, iBin ) );
  if ( histMap2D.size() == kinProp_.size() ) {
    fillRel( iCat, histMap2D, iBin );
  }
  else {
    edm::LogError( "TopHitFitResolutionFunctions" ) << "Found " << histMap2D.size() << " histos in object category " << objCat << " instead of " << kinProp_.size();
  }

  return;

}


// Filling deviations of RECO from GEN for kinematic properties of object types
void TopHitFitResolutionFunctions::fill( unsigned iCat, MapTH1D & histMap1D, unsigned iBin )
{

  if ( ! checkCatBin( iCat, iBin ) ) return;
  std::pair< double, double > etaLimits( getEtaLimits( iCat, iBin ) );
  double eta;
  const std::vector< int > jetLepCombi( ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ) );
  pat::Jet qJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LightQ ) ) );
  pat::Jet qBarJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LightQBar ) ) );
  pat::Jet hadBJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::HadB ) ) );
  pat::Jet lepBJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LepB ) ) );
  const std::string objCat( objCat_.at( iCat ) );
  switch ( iCat ) {
    case 0:
      eta = ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->eta();
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        histMap1D[ objCat + "Pt" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt() - ttGenEvent_->singleLepton()->pt() );
        histMap1D[ objCat + "PtInv" ]->Fill( 1. / ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt() - 1. / ttGenEvent_->singleLepton()->pt() );
        histMap1D[ objCat + "Eta" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->eta() - ttGenEvent_->singleLepton()->eta() );
        histMap1D[ objCat + "Phi" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->phi() - ttGenEvent_->singleLepton()->phi() );
        histMap1D[ objCat + "Pz" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pz() - ttGenEvent_->singleLepton()->pz() );
        histMap1D[ objCat + "M" ]->Fill( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->mass() - ttGenEvent_->singleLepton()->mass() );
      }
      break;
    case 1:
      eta = qJet.correctedJet( jecLevel_ ).eta();
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        histMap1D[ objCat + "Pt" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pt() - ttGenEvent_->hadronicDecayQuark()->pt() );
        histMap1D[ objCat + "PtInv" ]->Fill( 1. / ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pt() - 1. / ttGenEvent_->hadronicDecayQuark()->pt() );
        histMap1D[ objCat + "Eta" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->eta() - ttGenEvent_->hadronicDecayQuark()->eta() );
        histMap1D[ objCat + "Phi" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->phi() - ttGenEvent_->hadronicDecayQuark()->phi() );
        histMap1D[ objCat + "Pz" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pz() - ttGenEvent_->hadronicDecayQuark()->pz() );
        histMap1D[ objCat + "M" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->mass() - ttGenEvent_->hadronicDecayQuark()->mass() );
      }
      eta = qBarJet.correctedJet( jecLevel_ ).eta();
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        histMap1D[ objCat + "Pt" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pt() - ttGenEvent_->hadronicDecayQuarkBar()->pt() );
        histMap1D[ objCat + "PtInv" ]->Fill( 1. / ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pt() - 1. / ttGenEvent_->hadronicDecayQuarkBar()->pt() );
        histMap1D[ objCat + "Eta" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->eta() - ttGenEvent_->hadronicDecayQuarkBar()->eta() );
        histMap1D[ objCat + "Phi" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->phi() - ttGenEvent_->hadronicDecayQuarkBar()->phi() );
        histMap1D[ objCat + "Pz" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pz() - ttGenEvent_->hadronicDecayQuarkBar()->pz() );
        histMap1D[ objCat + "M" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->mass() - ttGenEvent_->hadronicDecayQuarkBar()->mass() );
      }
      break;
    case 2:
      eta = hadBJet.correctedJet( jecLevel_, "bottom" ).eta(); // pat::JetCorrFactors::BOTTOM
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        histMap1D[ objCat + "Pt" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pt() - ttGenEvent_->hadronicDecayB()->pt() );
        histMap1D[ objCat + "PtInv" ]->Fill( 1. / ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pt() - 1. / ttGenEvent_->hadronicDecayB()->pt() );
        histMap1D[ objCat + "Eta" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->eta() - ttGenEvent_->hadronicDecayB()->eta() );
        histMap1D[ objCat + "Phi" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->phi() - ttGenEvent_->hadronicDecayB()->phi() );
        histMap1D[ objCat + "Pz" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pz() - ttGenEvent_->hadronicDecayB()->pz() );
        histMap1D[ objCat + "M" ]->Fill( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->mass() - ttGenEvent_->hadronicDecayB()->mass() );
      }
      eta = lepBJet.correctedJet( jecLevel_, "bottom" ).eta(); // pat::JetCorrFactors::BOTTOM
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        histMap1D[ objCat + "Pt" ]->Fill( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pt() - ttGenEvent_->leptonicDecayB()->pt() );
        histMap1D[ objCat + "PtInv" ]->Fill( 1. / ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pt() - 1. / ttGenEvent_->leptonicDecayB()->pt() );
        histMap1D[ objCat + "Eta" ]->Fill( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->eta() - ttGenEvent_->leptonicDecayB()->eta() );
        histMap1D[ objCat + "Phi" ]->Fill( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->phi() - ttGenEvent_->leptonicDecayB()->phi() );
        histMap1D[ objCat + "Pz" ]->Fill( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pz() - ttGenEvent_->leptonicDecayB()->pz() );
        histMap1D[ objCat + "M" ]->Fill( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->mass() - ttGenEvent_->leptonicDecayB()->mass() );
      }
      break;
    case 3:
      eta = ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->eta();
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        histMap1D[ objCat + "Pt" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt() - ttGenEvent_->singleNeutrino()->pt() );
        histMap1D[ objCat + "PtInv" ]->Fill( 1. / ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt() - 1. / ttGenEvent_->singleNeutrino()->pt() );
        histMap1D[ objCat + "Eta" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->eta() - ttGenEvent_->singleNeutrino()->eta() );
        histMap1D[ objCat + "Phi" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->phi() - ttGenEvent_->singleNeutrino()->phi() );
        histMap1D[ objCat + "Pz" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pz() - ttGenEvent_->singleNeutrino()->pz() );
        histMap1D[ objCat + "M" ]->Fill( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->mass() - ttGenEvent_->singleNeutrino()->mass() );
      }
      break;
    default:
      edm::LogError( "TopHitFitResolutionFunctions" ) << "Trying to access object category " << iCat << " (" << objCat_.size() << " categories)";
      break;
  }

  return;

}

// Filling realtive deviations of RECO from GEN for kinematic properties of object types
void TopHitFitResolutionFunctions::fillRel( unsigned iCat, MapTH1D & histMap1DRel, unsigned iBin )
{

  if ( ! checkCatBin( iCat, iBin ) ) return;
  std::pair< double, double > etaLimits( getEtaLimits( iCat, iBin ) );
  double eta;
  const std::vector< int > jetLepCombi( ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ) );
  pat::Jet qJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LightQ ) ) );
  pat::Jet qBarJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LightQBar ) ) );
  pat::Jet hadBJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::HadB ) ) );
  pat::Jet lepBJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LepB ) ) );
  const std::string objCat( objCat_.at( iCat ) );
  switch ( iCat ) {
    case 0:
      eta = ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->eta();
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        histMap1DRel[ objCat + "Pt" ]->Fill( ( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt() - ttGenEvent_->singleLepton()->pt() ) / ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt() );
        histMap1DRel[ objCat + "PtInv" ]->Fill( ( 1. / ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt() - 1. / ttGenEvent_->singleLepton()->pt() ) * ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt() );
        histMap1DRel[ objCat + "Eta" ]->Fill( ( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->eta() - ttGenEvent_->singleLepton()->eta() ) / ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->eta() );
        histMap1DRel[ objCat + "Phi" ]->Fill( ( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->phi() - ttGenEvent_->singleLepton()->phi() ) / ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->phi() );
        histMap1DRel[ objCat + "Pz" ]->Fill( ( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pz() - ttGenEvent_->singleLepton()->pz() ) / ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pz() );
        histMap1DRel[ objCat + "M" ]->Fill( ( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->mass() - ttGenEvent_->singleLepton()->mass() ) / ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->mass() );
      }
      break;
    case 1:
      eta = qJet.correctedJet( jecLevel_ ).eta();
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        histMap1DRel[ objCat + "Pt" ]->Fill( ( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pt() - ttGenEvent_->hadronicDecayQuark()->pt() ) / ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pt() );
        histMap1DRel[ objCat + "PtInv" ]->Fill( ( 1. / ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pt() - 1. / ttGenEvent_->hadronicDecayQuark()->pt() ) * ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pt() );
        histMap1DRel[ objCat + "Eta" ]->Fill( ( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->eta() - ttGenEvent_->hadronicDecayQuark()->eta() ) / ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->eta() );
        histMap1DRel[ objCat + "Phi" ]->Fill( ( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->phi() - ttGenEvent_->hadronicDecayQuark()->phi() ) / ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->phi() );
        histMap1DRel[ objCat + "Pz" ]->Fill( ( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pz() - ttGenEvent_->hadronicDecayQuark()->pz() ) / ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pz() );
        histMap1DRel[ objCat + "M" ]->Fill( ( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->mass() - ttGenEvent_->hadronicDecayQuark()->mass() ) / ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->mass() );
      }
      eta = qBarJet.correctedJet( jecLevel_ ).eta();
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        histMap1DRel[ objCat + "Pt" ]->Fill( ( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pt() - ttGenEvent_->hadronicDecayQuarkBar()->pt() ) / ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pt() );
        histMap1DRel[ objCat + "PtInv" ]->Fill( ( 1. / ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pt() - 1. / ttGenEvent_->hadronicDecayQuarkBar()->pt() ) * ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pt() );
        histMap1DRel[ objCat + "Eta" ]->Fill( ( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->eta() - ttGenEvent_->hadronicDecayQuarkBar()->eta() ) / ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->eta() );
        histMap1DRel[ objCat + "Phi" ]->Fill( ( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->phi() - ttGenEvent_->hadronicDecayQuarkBar()->phi() ) / ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->phi() );
        histMap1DRel[ objCat + "Pz" ]->Fill( ( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pz() - ttGenEvent_->hadronicDecayQuarkBar()->pz() ) / ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pz() );
        histMap1DRel[ objCat + "M" ]->Fill( ( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->mass() - ttGenEvent_->hadronicDecayQuarkBar()->mass() ) / ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->mass() );
      }
      break;
    case 2:
      eta = hadBJet.correctedJet( jecLevel_, "bottom" ).eta(); // pat::JetCorrFactors::BOTTOM
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        histMap1DRel[ objCat + "Pt" ]->Fill( ( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pt() - ttGenEvent_->hadronicDecayB()->pt() ) / ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pt() );
        histMap1DRel[ objCat + "PtInv" ]->Fill( ( 1. / ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pt() - 1. / ttGenEvent_->hadronicDecayB()->pt() ) * ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pt() );
        histMap1DRel[ objCat + "Eta" ]->Fill( ( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->eta() - ttGenEvent_->hadronicDecayB()->eta() ) / ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->eta() );
        histMap1DRel[ objCat + "Phi" ]->Fill( ( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->phi() - ttGenEvent_->hadronicDecayB()->phi() ) / ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->phi() );
        histMap1DRel[ objCat + "Pz" ]->Fill( ( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pz() - ttGenEvent_->hadronicDecayB()->pz() ) / ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pz() );
        histMap1DRel[ objCat + "M" ]->Fill( ( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->mass() - ttGenEvent_->hadronicDecayB()->mass() ) / ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->mass() );
      }
      eta = lepBJet.correctedJet( jecLevel_, "bottom" ).eta(); // pat::JetCorrFactors::BOTTOM
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        histMap1DRel[ objCat + "Pt" ]->Fill( ( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pt() - ttGenEvent_->leptonicDecayB()->pt() ) / ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pt() );
        histMap1DRel[ objCat + "PtInv" ]->Fill( ( 1. / ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pt() - 1. / ttGenEvent_->leptonicDecayB()->pt() ) * ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pt() );
        histMap1DRel[ objCat + "Eta" ]->Fill( ( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->eta() - ttGenEvent_->leptonicDecayB()->eta() ) / ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->eta() );
        histMap1DRel[ objCat + "Phi" ]->Fill( ( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->phi() - ttGenEvent_->leptonicDecayB()->phi() ) / ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->phi() );
        histMap1DRel[ objCat + "Pz" ]->Fill( ( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pz() - ttGenEvent_->leptonicDecayB()->pz() ) / ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pz() );
        histMap1DRel[ objCat + "M" ]->Fill( ( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->mass() - ttGenEvent_->leptonicDecayB()->mass() ) / ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->mass() );
      }
      break;
    case 3:
      eta = ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->eta();
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        histMap1DRel[ objCat + "Pt" ]->Fill( ( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt() - ttGenEvent_->singleNeutrino()->pt() ) / ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt() );
        histMap1DRel[ objCat + "PtInv" ]->Fill( ( 1. / ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt() - 1. / ttGenEvent_->singleNeutrino()->pt() ) * ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt() );
        histMap1DRel[ objCat + "Eta" ]->Fill( ( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->eta() - ttGenEvent_->singleNeutrino()->eta() ) / ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->eta() );
        histMap1DRel[ objCat + "Phi" ]->Fill( ( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->phi() - ttGenEvent_->singleNeutrino()->phi() ) / ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->phi() );
        histMap1DRel[ objCat + "Pz" ]->Fill( ( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pz() - ttGenEvent_->singleNeutrino()->pz() ) / ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pz() );
        histMap1DRel[ objCat + "M" ]->Fill( ( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->mass() - ttGenEvent_->singleNeutrino()->mass() ) / ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->mass() );
      }
      break;
    default:
      edm::LogError( "TopHitFitResolutionFunctions" ) << "Trying to access object category " << iCat << " (" << objCat_.size() << " categories)";
      break;
  }

  return;

}

void TopHitFitResolutionFunctions::fill( unsigned iCat, MapTH2D & histMap2D, unsigned iBin )
{

  if ( ! checkCatBin( iCat, iBin ) ) return;
  std::pair< double, double > etaLimits( getEtaLimits( iCat, iBin ) );
  double eta;
  const std::vector< int > jetLepCombi( ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ) );
  pat::Jet qJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LightQ ) ) );
  pat::Jet qBarJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LightQBar ) ) );
  pat::Jet hadBJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::HadB ) ) );
  pat::Jet lepBJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LepB ) ) );
  const std::string objCat( objCat_.at( iCat ) );
  switch ( iCat ) {
    case 0:
      eta = ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->eta();
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        const double pt( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt() );
        histMap2D[ objCat + "Pt_" +objCat  + "Pt" ]->Fill( pt, pt - ttGenEvent_->singleLepton()->pt() );
        histMap2D[ objCat + "Pt_" +objCat  + "PtInv" ]->Fill( pt, 1. / pt - 1. / ttGenEvent_->singleLepton()->pt() );
        histMap2D[ objCat + "Pt_" +objCat  + "Eta" ]->Fill( pt, ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->eta() - ttGenEvent_->singleLepton()->eta() );
        histMap2D[ objCat + "Pt_" +objCat  + "Phi" ]->Fill( pt, ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->phi() - ttGenEvent_->singleLepton()->phi() );
        histMap2D[ objCat + "Pt_" +objCat  + "Pz" ]->Fill( pt, ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pz() - ttGenEvent_->singleLepton()->pz() );
        histMap2D[ objCat + "Pt_" +objCat  + "M" ]->Fill( pt, ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->mass() - ttGenEvent_->singleLepton()->mass() );
      }
      break;
    case 1:
      eta = qJet.correctedJet( jecLevel_ ).eta();
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        const double pt( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pt() );
        histMap2D[ objCat + "Pt_" +objCat  + "Pt" ]->Fill( pt, pt - ttGenEvent_->hadronicDecayQuark()->pt() );
        histMap2D[ objCat + "Pt_" +objCat  + "PtInv" ]->Fill( pt, 1. / pt - 1. / ttGenEvent_->hadronicDecayQuark()->pt() );
        histMap2D[ objCat + "Pt_" +objCat  + "Eta" ]->Fill( pt, ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->eta() - ttGenEvent_->hadronicDecayQuark()->eta() );
        histMap2D[ objCat + "Pt_" +objCat  + "Phi" ]->Fill( pt, ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->phi() - ttGenEvent_->hadronicDecayQuark()->phi() );
        histMap2D[ objCat + "Pt_" +objCat  + "Pz" ]->Fill( pt, ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pz() - ttGenEvent_->hadronicDecayQuark()->pz() );
        histMap2D[ objCat + "Pt_" +objCat  + "M" ]->Fill( pt, ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->mass() - ttGenEvent_->hadronicDecayQuark()->mass() );
      }
      eta = qBarJet.correctedJet( jecLevel_ ).eta();
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        const double pt( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pt() );
        histMap2D[ objCat + "Pt_" +objCat  + "Pt" ]->Fill( pt, pt - ttGenEvent_->hadronicDecayQuarkBar()->pt() );
        histMap2D[ objCat + "Pt_" +objCat  + "PtInv" ]->Fill( pt, 1. / pt - 1. / ttGenEvent_->hadronicDecayQuarkBar()->pt() );
        histMap2D[ objCat + "Pt_" +objCat  + "Eta" ]->Fill( pt, ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->eta() - ttGenEvent_->hadronicDecayQuarkBar()->eta() );
        histMap2D[ objCat + "Pt_" +objCat  + "Phi" ]->Fill( pt, ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->phi() - ttGenEvent_->hadronicDecayQuarkBar()->phi() );
        histMap2D[ objCat + "Pt_" +objCat  + "Pz" ]->Fill( pt, ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pz() - ttGenEvent_->hadronicDecayQuarkBar()->pz() );
        histMap2D[ objCat + "Pt_" +objCat  + "M" ]->Fill( pt, ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->mass() - ttGenEvent_->hadronicDecayQuarkBar()->mass() );
      }
      break;
    case 2:
      eta = hadBJet.correctedJet( jecLevel_, "bottom" ).eta(); // pat::JetCorrFactors::BOTTOM
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        const double pt( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pt() );
        histMap2D[ objCat + "Pt_" +objCat  + "Pt" ]->Fill( pt, pt - ttGenEvent_->hadronicDecayB()->pt() );
        histMap2D[ objCat + "Pt_" +objCat  + "PtInv" ]->Fill( pt, 1. / pt - 1. / ttGenEvent_->hadronicDecayB()->pt() );
        histMap2D[ objCat + "Pt_" +objCat  + "Eta" ]->Fill( pt, ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->eta() - ttGenEvent_->hadronicDecayB()->eta() );
        histMap2D[ objCat + "Pt_" +objCat  + "Phi" ]->Fill( pt, ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->phi() - ttGenEvent_->hadronicDecayB()->phi() );
        histMap2D[ objCat + "Pt_" +objCat  + "Pz" ]->Fill( pt, ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pz() - ttGenEvent_->hadronicDecayB()->pz() );
        histMap2D[ objCat + "Pt_" +objCat  + "M" ]->Fill( pt, ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->mass() - ttGenEvent_->hadronicDecayB()->mass() );
      }
      eta = lepBJet.correctedJet( jecLevel_, "bottom" ).eta(); // pat::JetCorrFactors::BOTTOM
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        const double pt( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pt() );
        histMap2D[ objCat + "Pt_" +objCat  + "Pt" ]->Fill( pt, pt - ttGenEvent_->leptonicDecayB()->pt() );
        histMap2D[ objCat + "Pt_" +objCat  + "PtInv" ]->Fill( pt, 1. / pt - 1. / ttGenEvent_->leptonicDecayB()->pt() );
        histMap2D[ objCat + "Pt_" +objCat  + "Eta" ]->Fill( pt, ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->eta() - ttGenEvent_->leptonicDecayB()->eta() );
        histMap2D[ objCat + "Pt_" +objCat  + "Phi" ]->Fill( pt, ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->phi() - ttGenEvent_->leptonicDecayB()->phi() );
        histMap2D[ objCat + "Pt_" +objCat  + "Pz" ]->Fill( pt, ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pz() - ttGenEvent_->leptonicDecayB()->pz() );
        histMap2D[ objCat + "Pt_" +objCat  + "M" ]->Fill( pt, ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->mass() - ttGenEvent_->leptonicDecayB()->mass() );
      }
      break;
    case 3:
      eta = ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->eta();
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        const double pt( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt() );
        histMap2D[ objCat + "Pt_" +objCat  + "Pt" ]->Fill( pt, pt - ttGenEvent_->singleNeutrino()->pt() );
        histMap2D[ objCat + "Pt_" +objCat  + "PtInv" ]->Fill( pt, 1. / pt - 1. / ttGenEvent_->singleNeutrino()->pt() );
        histMap2D[ objCat + "Pt_" +objCat  + "Eta" ]->Fill( pt, ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->eta() - ttGenEvent_->singleNeutrino()->eta() );
        histMap2D[ objCat + "Pt_" +objCat  + "Phi" ]->Fill( pt, ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->phi() - ttGenEvent_->singleNeutrino()->phi() );
        histMap2D[ objCat + "Pt_" +objCat  + "Pz" ]->Fill( pt, ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pz() - ttGenEvent_->singleNeutrino()->pz() );
        histMap2D[ objCat + "Pt_" +objCat  + "M" ]->Fill( pt, ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->mass() - ttGenEvent_->singleNeutrino()->mass() );
      }
      break;
    default:
      edm::LogError( "TopHitFitResolutionFunctions" ) << "Trying to access object category " << iCat << " (" << objCat_.size() << " categories)";
      break;
  }

  return;

}

void TopHitFitResolutionFunctions::fillRel( unsigned iCat, MapTH2D & histMap2DRel, unsigned iBin )
{

  if ( ! checkCatBin( iCat, iBin ) ) return;
  std::pair< double, double > etaLimits( getEtaLimits( iCat, iBin ) );
  double eta;
  const std::vector< int > jetLepCombi( ttSemiLeptonicEvent_->jetLeptonCombination( TtEvent::kGenMatch ) );
  pat::Jet qJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LightQ ) ) );
  pat::Jet qBarJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LightQBar ) ) );
  pat::Jet hadBJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::HadB ) ) );
  pat::Jet lepBJet( patJets_->at( ( unsigned )jetLepCombi.at( TtSemiLepEvtPartons::LepB ) ) );
  const std::string objCat( objCat_.at( iCat ) );
  switch ( iCat ) {
    case 0:
      eta = ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->eta();
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        const double pt( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pt() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Pt" ]->Fill( pt, ( pt - ttGenEvent_->singleLepton()->pt() ) / pt );
        histMap2DRel[ objCat + "Pt_" +objCat  + "PtInv" ]->Fill( pt, ( 1. / pt - 1. / ttGenEvent_->singleLepton()->pt() ) * pt );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Eta" ]->Fill( pt, ( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->eta() - ttGenEvent_->singleLepton()->eta() ) / ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->eta() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Phi" ]->Fill( pt, ( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->phi() - ttGenEvent_->singleLepton()->phi() ) / ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->phi() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Pz" ]->Fill( pt, ( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pz() - ttGenEvent_->singleLepton()->pz() ) / ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->pz() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "M" ]->Fill( pt, ( ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->mass() - ttGenEvent_->singleLepton()->mass() ) / ttSemiLeptonicEvent_->singleLepton( TtEvent::kGenMatch )->mass() );
      }
      break;
    case 1:
      eta = qJet.correctedJet( jecLevel_ ).eta();
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        const double pt( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pt() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Pt" ]->Fill( pt, ( pt - ttGenEvent_->hadronicDecayQuark()->pt() ) / pt );
        histMap2DRel[ objCat + "Pt_" +objCat  + "PtInv" ]->Fill( pt, ( 1. / pt - 1. / ttGenEvent_->hadronicDecayQuark()->pt() ) * pt );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Eta" ]->Fill( pt, ( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->eta() - ttGenEvent_->hadronicDecayQuark()->eta() ) / ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->eta() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Phi" ]->Fill( pt, ( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->phi() - ttGenEvent_->hadronicDecayQuark()->phi() ) / ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->phi() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Pz" ]->Fill( pt, ( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pz() - ttGenEvent_->hadronicDecayQuark()->pz() ) / ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->pz() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "M" ]->Fill( pt, ( ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->mass() - ttGenEvent_->hadronicDecayQuark()->mass() ) / ttSemiLeptonicEvent_->hadronicDecayQuark( TtEvent::kGenMatch )->mass() );
      }
      eta = qBarJet.correctedJet( jecLevel_ ).eta();
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        const double pt( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pt() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Pt" ]->Fill( pt, ( pt - ttGenEvent_->hadronicDecayQuarkBar()->pt() ) / pt );
        histMap2DRel[ objCat + "Pt_" +objCat  + "PtInv" ]->Fill( pt, ( 1. / pt - 1. / ttGenEvent_->hadronicDecayQuarkBar()->pt() ) * pt );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Eta" ]->Fill( pt, ( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->eta() - ttGenEvent_->hadronicDecayQuarkBar()->eta() ) / ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->eta() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Phi" ]->Fill( pt, ( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->phi() - ttGenEvent_->hadronicDecayQuarkBar()->phi() ) / ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->phi() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Pz" ]->Fill( pt, ( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pz() - ttGenEvent_->hadronicDecayQuarkBar()->pz() ) / ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->pz() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "M" ]->Fill( pt, ( ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->mass() - ttGenEvent_->hadronicDecayQuarkBar()->mass() ) / ttSemiLeptonicEvent_->hadronicDecayQuarkBar( TtEvent::kGenMatch )->mass() );
      }
      break;
    case 2:
      eta = hadBJet.correctedJet( jecLevel_, "bottom" ).eta(); // pat::JetCorrFactors::BOTTOM
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        const double pt( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pt() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Pt" ]->Fill( pt, ( pt - ttGenEvent_->hadronicDecayB()->pt() ) / pt );
        histMap2DRel[ objCat + "Pt_" +objCat  + "PtInv" ]->Fill( pt, ( 1. / pt - 1. / ttGenEvent_->hadronicDecayB()->pt() ) * pt );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Eta" ]->Fill( pt, ( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->eta() - ttGenEvent_->hadronicDecayB()->eta() ) / ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->eta() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Phi" ]->Fill( pt, ( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->phi() - ttGenEvent_->hadronicDecayB()->phi() ) / ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->phi() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Pz" ]->Fill( pt, ( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pz() - ttGenEvent_->hadronicDecayB()->pz() ) / ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->pz() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "M" ]->Fill( pt, ( ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->mass() - ttGenEvent_->hadronicDecayB()->mass() ) / ttSemiLeptonicEvent_->hadronicDecayB( TtEvent::kGenMatch )->mass() );
      }
      eta = lepBJet.correctedJet( jecLevel_, "bottom" ).eta(); // pat::JetCorrFactors::BOTTOM
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        const double pt( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pt() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Pt" ]->Fill( pt, ( pt - ttGenEvent_->leptonicDecayB()->pt() ) / pt );
        histMap2DRel[ objCat + "Pt_" +objCat  + "PtInv" ]->Fill( pt, ( 1. / pt - 1. / ttGenEvent_->leptonicDecayB()->pt() ) * pt );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Eta" ]->Fill( pt, ( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->eta() - ttGenEvent_->leptonicDecayB()->eta() ) / ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->eta() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Phi" ]->Fill( pt, ( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->phi() - ttGenEvent_->leptonicDecayB()->phi() ) / ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->phi() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Pz" ]->Fill( pt, ( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pz() - ttGenEvent_->leptonicDecayB()->pz() ) / ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->pz() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "M" ]->Fill( pt, ( ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->mass() - ttGenEvent_->leptonicDecayB()->mass() ) / ttSemiLeptonicEvent_->leptonicDecayB( TtEvent::kGenMatch )->mass() );
      }
      break;
    case 3:
      eta = ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->eta();
      if ( etaLimits.first <= eta && eta < etaLimits.second ) {
        const double pt( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pt() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Pt" ]->Fill( pt, ( pt - ttGenEvent_->singleNeutrino()->pt() ) / pt );
        histMap2DRel[ objCat + "Pt_" +objCat  + "PtInv" ]->Fill( pt, ( 1. / pt - 1. / ttGenEvent_->singleNeutrino()->pt() ) * pt );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Eta" ]->Fill( pt, ( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->eta() - ttGenEvent_->singleNeutrino()->eta() ) / ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->eta() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Phi" ]->Fill( pt, ( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->phi() - ttGenEvent_->singleNeutrino()->phi() ) / ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->phi() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "Pz" ]->Fill( pt, ( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pz() - ttGenEvent_->singleNeutrino()->pz() ) / ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->pz() );
        histMap2DRel[ objCat + "Pt_" +objCat  + "M" ]->Fill( pt, ( ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->mass() - ttGenEvent_->singleNeutrino()->mass() ) / ttSemiLeptonicEvent_->singleNeutrino( TtEvent::kGenMatch )->mass() );
      }
      break;
    default:
      edm::LogError( "TopHitFitResolutionFunctions" ) << "Trying to access object category " << iCat << " (" << objCat_.size() << " categories)";
      break;
  }

  return;

}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( TopHitFitResolutionFunctions );
