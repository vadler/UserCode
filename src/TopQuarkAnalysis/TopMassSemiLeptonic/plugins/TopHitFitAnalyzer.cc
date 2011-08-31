// -*- C++ -*-
//
// Package:    TopMassSemiLeptonic
// Class:      TopHitFitAnalyzer
//
// $Id: TopHitFitAnalyzer.cc,v 1.1 2011/08/31 14:45:53 vadler Exp $
//
/**
  \class    TopHitFitAnalyzer TopHitFitAnalyzer.cc "TopQuarkAnalysis/TopMassSemiLeptonic/plugins/TopHitFitAnalyzer.cc"
  \brief    Analyzes



  \author   Volker Adler
  \version  $Id: TopHitFitAnalyzer.cc,v 1.1 2011/08/31 14:45:53 vadler Exp $
*/


#include <map>
#include <string>

#include "TH1D.h"
#include "TH2D.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"


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

    /// Configuration parameters
    edm::InputTag ttSemiLeptonicEvent_;

    /// Histograms
    std::map< std::string, TH1D* > histos1D_;
    std::map< std::string, TH2D* > histos2D_;

};


#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "AnalysisDataFormats/TopObjects/interface/TtSemiLeptonicEvent.h"


// Default constructor
TopHitFitAnalyzer::TopHitFitAnalyzer( const edm::ParameterSet & iConfig )
: ttSemiLeptonicEvent_( iConfig.getParameter< edm::InputTag >( "ttSemiLeptonicEvent" ) )
, histos1D_()
, histos2D_()
{
}


// Destructor
TopHitFitAnalyzer::~TopHitFitAnalyzer()
{
}


// Begin job
void TopHitFitAnalyzer::beginJob()
{

  edm::Service< TFileService > fileService;

  histos1D_[ "semiLepChan" ] = fileService->make< TH1D >( "semiLepChan", "Semi-leptonic decay channel", 5, -0.5, 4.5 );
  histos1D_[ "semiLepChan" ]->SetXTitle( "decay channel" );
  histos1D_[ "semiLepChan" ]->SetYTitle( "events" );

}


// Event loop
void TopHitFitAnalyzer::analyze( const edm::Event & iEvent, const edm::EventSetup & iSetup )
{

  // TQAF semi-leptonic event
  edm::Handle< TtSemiLeptonicEvent > ttSemiLeptonicEvent;
  iEvent.getByLabel( ttSemiLeptonicEvent_, ttSemiLeptonicEvent );

  const TtGenEvent * ttGenEvent( ttSemiLeptonicEvent->genEvent().get() );
  histos1D_[ "semiLepChan" ]->Fill( ttGenEvent->semiLeptonicChannel() );

}


// End job
void TopHitFitAnalyzer::endJob()
{
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( TopHitFitAnalyzer );
