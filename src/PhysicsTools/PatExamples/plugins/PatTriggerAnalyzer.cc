
#include "PhysicsTools/PatExamples/plugins/PatTriggerAnalyzer.h"

#include "PhysicsTools/PatUtils/interface/TriggerHelper.h"
#include "DataFormats/Common/interface/AssociativeIterator.h"

#include "TMath.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "PhysicsTools/UtilAlgos/interface/TFileService.h"

using namespace pat;
using namespace pat::helper;
using namespace TMath;

PatTriggerAnalyzer::PatTriggerAnalyzer( const edm::ParameterSet & iConfig ) :
  trigger_( iConfig.getParameter< edm::InputTag >( "trigger" ) ),
  triggerEvent_( iConfig.getParameter< edm::InputTag >( "triggerEvent" ) ),
  muons_( iConfig.getParameter< edm::InputTag >( "muons" ) ),
  muonMatch_( iConfig.getParameter< std::string >( "muonMatch" ) ),
  histos1D_(),
  histos2D_()
{
}

PatTriggerAnalyzer::~PatTriggerAnalyzer()
{
}

void PatTriggerAnalyzer::beginJob( const edm::EventSetup & iSetup )
{
  edm::Service< TFileService > fileService;
  // histogram definitions
//   YOUR HISTOGRAM DEFINITIONS GO HERE!
//   histos1D_[ "histoName" ] = fileService->make< TH1D >( "[normal TH1D constructor]" ); // EXAMPLE CODE
//   histos1D_[ "histoName" ]->SetXTitle( "x-axis label" );                               // EXAMPLE CODE
//   histos1D_[ "histoName" ]->SetYTitle( "y-axis label" );                               // EXAMPLE CODE
  histos2D_[ "ptTrigCand" ] = fileService->make< TH2D >( "ptTrigCand", "Object vs. candidate p_{T}", 60, 0., 300., 60, 0., 300. );
  histos2D_[ "ptTrigCand" ]->SetXTitle( "candidate" );
  histos2D_[ "ptTrigCand" ]->SetYTitle( "object" );
  histos2D_[ "etaTrigCand" ] = fileService->make< TH2D >( "etaTrigCand", "Object vs. candidate #eta", 50, -2.5, 2.5, 50, -2.5, 2.5 );
  histos2D_[ "etaTrigCand" ]->SetXTitle( "candidate" );
  histos2D_[ "etaTrigCand" ]->SetYTitle( "object" );
  histos2D_[ "phiTrigCand" ] = fileService->make< TH2D >( "phiTrigCand", "Object vs. candidate #phi", 60, -Pi(), Pi(), 60, -Pi(), Pi() );
  histos2D_[ "phiTrigCand" ]->SetXTitle( "candidate" );
  histos2D_[ "phiTrigCand" ]->SetYTitle( "object" );
}

void PatTriggerAnalyzer::analyze( const edm::Event & iEvent, const edm::EventSetup & iSetup )
{
  // PAT trigger information
  edm::Handle< TriggerEvent > triggerEvent;
  iEvent.getByLabel( triggerEvent_, triggerEvent );
  edm::Handle< TriggerPathCollection > triggerPaths;
  iEvent.getByLabel( trigger_, triggerPaths );
  edm::Handle< TriggerFilterCollection > triggerFilters;
  iEvent.getByLabel( trigger_, triggerFilters );
  edm::Handle< TriggerObjectCollection > triggerObjects;
  iEvent.getByLabel( trigger_, triggerObjects );

  // PAT object collection
  edm::Handle< MuonCollection > muons;
  iEvent.getByLabel( muons_, muons );

  // PAT trigger helper for trigger matching information
  const TriggerMatchHelper matchHelper;

//   YOUR ANALYSIS CODE GOES HERE!

  const TriggerObjectMatch * triggerMatch( triggerEvent->triggerObjectMatchResult( muonMatch ) );
  for ( size_t iMuon = 0; iMuon < muons->size(); ++iMuon ) { // loop over muon references (PAT muons have been used in the matcher in task 3)
    const reco::CandidateBaseRef candBaseRef( MuonRef( muons, iMuon ) );
    const TriggerObjectRef trigRef( matchHelper.triggerMatchObject( candBaseRef, triggerMatch, iEvent, *triggerEvent ) );
    // fill histograms
    if ( trigRef.isAvailable() ) { // check references (necessary!)
      histos2D_[ "ptTrigCand" ]->Fill( candBaseRef->pt(), trigRef->pt() );
      histos2D_[ "etaTrigCand" ]->Fill( candBaseRef->eta(), trigRef->eta() );
      histos2D_[ "phiTrigCand" ]->Fill( candBaseRef->phi(), trigRef->phi() );
    }
  } // iMuon
  
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( PatTriggerAnalyzer );
