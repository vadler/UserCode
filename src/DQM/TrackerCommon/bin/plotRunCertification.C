#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TH1D.h"
#include "TString.h"


void plotRunCertification()
{

  // All
  const TString output_( "R000123596_All" );
  const UInt_t  nInput_( 7 );
  TString input_[ nInput_ ] = {
    TString( "R000123596__StreamExpress__BeamCommissioning09-Express-v2__DQM" ),
    TString( "R000123596__ExpressPhysics__CMSSW_3_3_5_patch4__RECO" ),
    TString( "R000123596__ExpressPhysics__CMSSW_3_3_5_patch4__RECO-PB" ),
    TString( "R000123596__ExpressPhysics__CMSSW_3_3_5_patch4__RECO-noPB" ),
    TString( "R000123596__MinimumBias__CMSSW_3_3_5_patch4__RECO" ),
    TString( "R000123596__MinimumBias__CMSSW_3_3_5_patch4__RECO-L1" ),
    TString( "R000123596__MinimumBias__CMSSW_3_3_5_patch4__RECO-L1-PB" )
  };
//   // Express
//   const TString output_( "R000123596_Express" );
//   const UInt_t  nInput_( 4 );
//   TString input_[ nInput_ ] = {
//     TString( "R000123596__StreamExpress__BeamCommissioning09-Express-v2__DQM" ),
//     TString( "R000123596__ExpressPhysics__CMSSW_3_3_5_patch4__RECO" ),
//     TString( "R000123596__ExpressPhysics__CMSSW_3_3_5_patch4__RECO-PB" ),
//     TString( "R000123596__ExpressPhysics__CMSSW_3_3_5_patch4__RECO-noPB" )
//   };
//   // MinBias
//   const TString output_( "R000123596_MinBias" );
//   const UInt_t  nInput_( 4 );
//   TString input_[ nInput_ ] = {
//     TString( "R000123596__StreamExpress__BeamCommissioning09-Express-v2__DQM" ),
//     TString( "R000123596__MinimumBias__CMSSW_3_3_5_patch4__RECO" ),
//     TString( "R000123596__MinimumBias__CMSSW_3_3_5_patch4__RECO-L1" ),
//     TString( "R000123596__MinimumBias__CMSSW_3_3_5_patch4__RECO-L1-PB" )
//   };

  const Int_t ww( 1272 );
  const Int_t wh(  740 );

  gStyle->SetPalette( 1, 0 );
  gStyle->SetCanvasColor( kWhite );
  gStyle->SetPadColor( kWhite );
  gStyle->SetPadTickX( 1 );
  gStyle->SetPadTickY( 1 );
  gStyle->SetPadTopMargin( 0.075 );
  gStyle->SetPadRightMargin( 0.075 );
  gStyle->SetPadBottomMargin( 0.15 );
  gStyle->SetPadLeftMargin( 0.15 );
  gStyle->SetTitleSize( 0.06, "XYZ" );
  gStyle->SetTitleFillColor( kWhite );
  gStyle->SetTitleBorderSize( 1 );
  gStyle->SetStatColor( kWhite );
  gStyle->SetStatBorderSize( 1 );
  gStyle->SetOptStat( kFALSE );
  gStyle->SetOptFit( kFALSE );
  gStyle->SetMarkerStyle( 8 );

  const TString baseDir( "Rint:/" );
  if ( gDirectory->GetPath() != baseDir.Data() ) gDirectory->cd( baseDir.Data() );

  TFile * filesInput[ nInput_ ];
  TH1D * ReportSiStrip_DetFraction[ nInput_ ];
  TH1D * ReportSiStrip_SToNFlag[ nInput_ ];
  TH1D * ReportSiStrip[ nInput_ ];
  TH1D * CertSiStrip[ nInput_ ];
  TH1D * DCSSiStrip[ nInput_ ];
  TH1D * DAQSiStrip[ nInput_ ];
  TH1D * SiStrip[ nInput_ ];
  TH1D * Pixel[ nInput_ ];
  TH1D * Tracking[ nInput_ ];
  for ( size_t iInput = 0; iInput < nInput_; ++iInput ) {
    const TString nameInput( "/afs/cern.ch/user/v/vadler/scratch0/cms/SiStripDQM/CMSSW_3_3_5_patch4/output/" + input_[ iInput ] + ".root" );
    filesInput[ iInput ] = ( TFile * )gROOT->GetListOfFiles()->FindObject( nameInput );
    if ( ! filesInput[ iInput ] ) filesInput[ iInput ] = new TFile( nameInput );
    ReportSiStrip_DetFraction[ iInput ] = ( TH1D * )filesInput[ iInput ]->Get( "ReportSiStrip_DetFraction" );
    ReportSiStrip_SToNFlag[ iInput ]    = ( TH1D * )filesInput[ iInput ]->Get( "ReportSiStrip_SToNFlag" );
    ReportSiStrip[ iInput ]             = ( TH1D * )filesInput[ iInput ]->Get( "ReportSiStrip" );
    CertSiStrip[ iInput ]               = ( TH1D * )filesInput[ iInput ]->Get( "CertSiStrip" );
    DCSSiStrip[ iInput ]                = ( TH1D * )filesInput[ iInput ]->Get( "DCSSiStrip" );
    DAQSiStrip[ iInput ]                = ( TH1D * )filesInput[ iInput ]->Get( "DAQSiStrip" );
    SiStrip[ iInput ]                   = ( TH1D * )filesInput[ iInput ]->Get( "SiStrip" );
    Pixel[ iInput ]                     = ( TH1D * )filesInput[ iInput ]->Get( "Pixel" );
    Tracking[ iInput ]                  = ( TH1D * )filesInput[ iInput ]->Get( "Tracking" );
  }

  TLegend * legendSummaries( new TLegend( 0., 0., 1., 1. ) );
  legendSummaries->SetBorderSize( 0 );
  legendSummaries->SetFillColor( kWhite );
  TCanvas * canvasSummaries( new TCanvas( "canvasSummaries", "Summaries", ww, wh ) );
  canvasSummaries->Divide( 2, 2, 0.0001, 0.0001 );
  canvasSummaries->cd( 1 );
  SiStrip[ 0 ]->Draw( "PL" );
  for ( size_t iInput = 0; iInput < nInput_; ++iInput ) {
    SiStrip[ iInput ]->Draw( "PLSame" );
    legendSummaries->AddEntry( SiStrip[ iInput ], input_[ iInput ], "PL" );
  }
  canvasSummaries->cd( 2 );
  Pixel[ 0 ]->Draw( "PL" );
  for ( size_t iInput = 0; iInput < nInput_; ++iInput ) {
    Pixel[ iInput ]->Draw( "PL" );
  }
  canvasSummaries->cd( 3 );
  Tracking[ 0 ]->Draw( "PL" );
  for ( size_t iInput = 0; iInput < nInput_; ++iInput ) {
    Tracking[ iInput ]->Draw( "PLSame" );
  }
  canvasSummaries->cd( 4 );
  legendSummaries->Draw();

  TLegend * legendReportsSiStrip( new TLegend() );
  legendReportsSiStrip = ( TLegend * )legendSummaries->Clone("legendReportsSiStrip");
  TCanvas * canvasReportsSiStrip( new TCanvas( "canvasReportsSiStrip", "SiStrip reports", ww, wh ) );
  canvasReportsSiStrip->Divide( 2, 2, 0.0001, 0.0001 );
  canvasReportsSiStrip->cd( 1 );
  ReportSiStrip_DetFraction[ 0 ]->Draw( "PL" );
  for ( size_t iInput = 0; iInput < nInput_; ++iInput ) {
    ReportSiStrip_DetFraction[ iInput ]->Draw( "PLSame" );
  }
  canvasReportsSiStrip->cd( 2 );
  ReportSiStrip_SToNFlag[ 0 ]->Draw( "PL" );
  for ( size_t iInput = 0; iInput < nInput_; ++iInput ) {
    ReportSiStrip_SToNFlag[ iInput ]->Draw( "PLSame" );
  }
  canvasReportsSiStrip->cd( 3 );
  ReportSiStrip[ 0 ]->Draw( "PL" );
  for ( size_t iInput = 0; iInput < nInput_; ++iInput ) {
    ReportSiStrip[ iInput ]->Draw( "PLSame" );
  }
  canvasReportsSiStrip->cd( 4 );
  legendReportsSiStrip->Draw();

  TLegend * legendFlagsSiStrip( new TLegend() );
  legendFlagsSiStrip = ( TLegend * )legendSummaries->Clone("legendFlagsSiStrip");
  TCanvas * canvasFlagsSiStrip( new TCanvas( "canvasFlagsSiStrip", "SiStrip flags", ww, wh ) );
  canvasFlagsSiStrip->Divide( 2, 2, 0.0001, 0.0001 );
  canvasFlagsSiStrip->cd( 1 );
  DCSSiStrip[ 0 ]->Draw( "PL" );
  for ( size_t iInput = 0; iInput < nInput_; ++iInput ) {
    DCSSiStrip[ iInput ]->Draw( "PLSame" );
  }
  canvasFlagsSiStrip->cd( 2 );
  DAQSiStrip[ 0 ]->Draw( "PL" );
  for ( size_t iInput = 0; iInput < nInput_; ++iInput ) {
    DAQSiStrip[ iInput ]->Draw( "PLSame" );
  }
  canvasFlagsSiStrip->cd( 3 );
  CertSiStrip[ 0 ]->Draw( "PL" );
  for ( size_t iInput = 0; iInput < nInput_; ++iInput ) {
    CertSiStrip[ iInput ]->Draw( "PLSame" );
  }
  canvasFlagsSiStrip->cd( 4 );
  legendFlagsSiStrip->Draw();

  for ( size_t iInput = 0; iInput < nInput_; ++iInput ) {
    gROOT->GetListOfFiles()     ->Remove( filesInput[ iInput ] );
    gROOT->GetListOfBrowsables()->Remove( filesInput[ iInput ] );
  }

  TCanvas * c = (TCanvas*)gROOT->FindObject( "myCanvas" );
  if ( c ) c->cd();
  gDirectory->cd( baseDir );

}
