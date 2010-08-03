#include <map>
#include <iostream>

// RooT, needs '<use name="root">' in the BuildFile
#include "TROOT.h"
#include "TStyle.h"
#include "TString.h"
#include "TFile.h"
#include "TKey.h"
#include "TH1D.h"

// CMSSW, needs according package usement statements in the BuildFile
#include "DQM/SiStripCommon/interface/TkHistoMap.h"


using namespace std;


// Functions
Bool_t  analyzeFile();
void    readCertificates( TDirectory * dir );
TString runNumber( const TString & pathFile );

// Configurables
map< TString, TString > sArguments_;
map< TString, Bool_t >  sOptions_;

// Global constants
const TString nameDirHead_( "DQMData" );
const TString nameDirBase_( "EventInfo" );
const TString nameDirCert_( "CertificationContents" );
const TString nameDirReport_( "reportSummaryContents" );
const TString nameDirDAQ_( "DAQContents" );
const TString nameDirDCS_( "DCSContents" );
const TString pathRunFragment_( "/Run /" );
const UInt_t  nSubSys_( 3 );
const TString sSubSys_[ nSubSys_ ] = { // sub-system directory names in DQM files
  "SiStrip",
  "Pixel",
  "Tracking"
};
enum SubSystems { // according enumeration
  SiStrip,
  Pixel,
  Tracking
};
const UInt_t  nSiStripSub_( 4 );
const TString sSiStripSub_[ nSiStripSub_ ] = { // sub-system directory granularity
  "TEC",
  "TIB",
  "TID",
  "TOB"
};
const UInt_t  nSiStripSubBF_( 6 );
const TString sSiStripSubBF_[ nSiStripSubBF_ ] = { // sub-system certification granularity
  "TECB",
  "TECF",
  "TIB",
  "TIDB",
  "TIDF",
  "TOB"
};

// Certificates and flags (run-by-run)
TString sRunNumber_;
TString sVersion_;
map< TString, Double_t > fCertificates_;


int main( int argc, char * argv[] )
{

  cout << endl;
  gStyle->SetOptStat( kFALSE );
  gStyle->SetOptFit( kFALSE );

  // Initialize defaults
  sArguments_[ "-f" ] = "";                             // path to DQM input file
  sArguments_[ "-o" ] = "analyzeRunCertification.root"; // path to RooT output file
  sArguments_[ "-c" ] =  "1";                           // histogram line & marker color
  sArguments_[ "-m" ] = "20";                           // histogram marker style
  sOptions_[ "-v" ] = kFALSE; // verbose output?

  // Input arguments (very simple)
  for ( int iArgument = 1; iArgument < argc; ++iArgument ) {
    if ( sArguments_.find( argv[ iArgument ] ) != sArguments_.end() ) {
      if ( sArguments_.find( argv[ iArgument + 1 ] ) == sArguments_.end() && sOptions_.find( argv[ iArgument + 1 ] ) == sOptions_.end() ) {
        sArguments_[ argv[ iArgument ] ] = argv[ iArgument + 1 ];
      }
    } else if ( sOptions_.find( argv[ iArgument ] ) != sOptions_.end() ) {
      sOptions_[ argv[ iArgument ] ] = kTRUE;
    }
  }
  if ( sArguments_[ "-f" ] == "" ) {
    cerr << "    ERROR: no input DQM file with '-f' option!" << endl;
    return 1;
  }

  // Run
  if ( ! analyzeFile() ) return 1;

  return 0;

}

Bool_t analyzeFile()
{

  TFile * fileDQM( TFile::Open( sArguments_[ "-f" ].Data() ) );
  if ( ! fileDQM ) {
    cerr << "    ERROR: DQM file not found!" << endl;
    return kFALSE;
  }

  sRunNumber_ = runNumber( sArguments_[ "-f" ] );
  if ( ! sRunNumber_.IsDigit() ) {
    cerr << "    ERROR: run number in DQM file name corrupted!" << endl;
    return kFALSE;
  }

  // Browse certification folders
  vector< TString > nameCertDir;
  nameCertDir.push_back( nameDirHead_ );
  for ( UInt_t iSys = 0; iSys < nSubSys_; ++iSys ) {
    const TString baseDir( nameDirHead_ + pathRunFragment_ + sSubSys_[ iSys ] + "/Run summary/" + nameDirBase_ );
    nameCertDir.push_back( baseDir );
    nameCertDir.push_back( baseDir + "/" + nameDirCert_ );
    nameCertDir.push_back( baseDir + "/" + nameDirReport_ );
    if ( iSys != Tracking ) {
      nameCertDir.push_back( baseDir + "/" + nameDirDAQ_ );
      nameCertDir.push_back( baseDir + "/" + nameDirDCS_ );
    }
  }
  for ( UInt_t iDir = 0; iDir < nameCertDir.size(); ++iDir ) {
    const TString nameCurDir( nameCertDir.at( iDir ).Contains( pathRunFragment_ ) ? nameCertDir.at( iDir ).Insert( nameCertDir.at( iDir ).Index( "Run " ) + 4, sRunNumber_ ) : nameCertDir.at( iDir ) );
    TDirectory * dirSub( ( TDirectory * )fileDQM->Get( nameCurDir.Data() ) );
    if ( ! dirSub ) {
      cout << "    WARNING: " << nameCurDir.Data() << " does not exist" << endl;
      continue;
    }
    readCertificates( dirSub );
  }

  fileDQM->Close();

  if ( sOptions_[ "-v" ] ) {
    cout << "    " << sVersion_ << endl;
    for ( map< TString, Double_t >::const_iterator iCert = fCertificates_.begin(); iCert != fCertificates_.end(); ++iCert ) cout << "    " << iCert->first << ": " << iCert->second << endl;
  }

  TFile * fileHisto( TFile::Open( sArguments_[ "-o" ].Data(), "RECREATE" ) );
  TH1D * ReportSiStrip_DetFraction( new TH1D( "ReportSiStrip_DetFraction", "SiStrip fraction of good modules from digis etc.", nSiStripSubBF_, 0., nSiStripSubBF_ ) );
  ReportSiStrip_DetFraction->SetMarkerStyle( atoi( sArguments_[ "-m" ] ) );
  ReportSiStrip_DetFraction->SetMarkerColor( atoi( sArguments_[ "-c" ] ) );
  ReportSiStrip_DetFraction->SetLineColor( atoi( sArguments_[ "-c" ] ) );
  TH1D * ReportSiStrip_SToNFlag( new TH1D( "ReportSiStrip_SToNFlag", "SiStrip fraction of good modules from S/N", nSiStripSubBF_, 0., nSiStripSubBF_ ) );
  ReportSiStrip_SToNFlag->SetMarkerStyle( atoi( sArguments_[ "-m" ] ) );
  ReportSiStrip_SToNFlag->SetMarkerColor( atoi( sArguments_[ "-c" ] ) );
  ReportSiStrip_SToNFlag->SetLineColor( atoi( sArguments_[ "-c" ] ) );
  TH1D * ReportSiStrip( new TH1D( "ReportSiStrip", "SiStrip total fraction of good modules", nSiStripSubBF_, 0., nSiStripSubBF_ ) );
  ReportSiStrip->SetMarkerStyle( atoi( sArguments_[ "-m" ] ) );
  ReportSiStrip->SetMarkerColor( atoi( sArguments_[ "-c" ] ) );
  ReportSiStrip->SetLineColor( atoi( sArguments_[ "-c" ] ) );
  TH1D * CertSiStrip( new TH1D( "CertSiStrip", "SiStrip certification value", nSiStripSubBF_, 0., nSiStripSubBF_ ) );
  CertSiStrip->SetMarkerStyle( atoi( sArguments_[ "-m" ] ) );
  CertSiStrip->SetMarkerColor( atoi( sArguments_[ "-c" ] ) );
  CertSiStrip->SetLineColor( atoi( sArguments_[ "-c" ] ) );
  TH1D * DCSSiStrip( new TH1D( "DCSSiStrip", "SiStrip DCS fraction", nSiStripSubBF_, 0., nSiStripSubBF_ ) );
  DCSSiStrip->SetMarkerStyle( atoi( sArguments_[ "-m" ] ) );
  DCSSiStrip->SetMarkerColor( atoi( sArguments_[ "-c" ] ) );
  DCSSiStrip->SetLineColor( atoi( sArguments_[ "-c" ] ) );
  TH1D * DAQSiStrip( new TH1D( "DAQSiStrip", "SiStrip DAQ fraction", nSiStripSubBF_, 0., nSiStripSubBF_ ) );
  DAQSiStrip->SetMarkerStyle( atoi( sArguments_[ "-m" ] ) );
  DAQSiStrip->SetMarkerColor( atoi( sArguments_[ "-c" ] ) );
  DAQSiStrip->SetLineColor( atoi( sArguments_[ "-c" ] ) );
  for ( UInt_t iBin = 1; iBin <= nSiStripSubBF_; ++iBin ) {
    CertSiStrip              ->GetXaxis()->SetBinLabel( iBin, sSiStripSubBF_[ iBin - 1 ] );
    ReportSiStrip_DetFraction->GetXaxis()->SetBinLabel( iBin, sSiStripSubBF_[ iBin - 1 ] );
    ReportSiStrip_SToNFlag   ->GetXaxis()->SetBinLabel( iBin, sSiStripSubBF_[ iBin - 1 ] );
    ReportSiStrip            ->GetXaxis()->SetBinLabel( iBin, sSiStripSubBF_[ iBin - 1 ] );
    DAQSiStrip               ->GetXaxis()->SetBinLabel( iBin, sSiStripSubBF_[ iBin - 1 ] );
    DCSSiStrip               ->GetXaxis()->SetBinLabel( iBin, sSiStripSubBF_[ iBin - 1 ] );
  }
  TH1D * SiStrip( new TH1D( "SiStrip", "SiStrip certification summaries", 4, 0., 4. ) );
  SiStrip->SetMarkerStyle( atoi( sArguments_[ "-m" ] ) );
  SiStrip->SetMarkerColor( atoi( sArguments_[ "-c" ] ) );
  SiStrip->SetLineColor( atoi( sArguments_[ "-c" ] ) );
  SiStrip->GetXaxis()->SetBinLabel( 1, "DCS" );
  SiStrip->GetXaxis()->SetBinLabel( 2, "DAQ" );
  SiStrip->GetXaxis()->SetBinLabel( 3, "Report" );
  SiStrip->GetXaxis()->SetBinLabel( 4, "Certification" );
  TH1D * Pixel( new TH1D( "Pixel", "Pixel certification summaries", 3, 0., 3. ) );
  Pixel->SetMarkerStyle( atoi( sArguments_[ "-m" ] ) );
  Pixel->SetMarkerColor( atoi( sArguments_[ "-c" ] ) );
  Pixel->SetLineColor( atoi( sArguments_[ "-c" ] ) );
  Pixel->GetXaxis()->SetBinLabel( 1, "DCS" );
  Pixel->GetXaxis()->SetBinLabel( 2, "DAQ" );
  Pixel->GetXaxis()->SetBinLabel( 3, "Report" );
  TH1D * Tracking( new TH1D( "Tracking", "Tracking certification summaries", 2, 0., 2. ) );
  Tracking->SetMarkerStyle( atoi( sArguments_[ "-m" ] ) );
  Tracking->SetMarkerColor( atoi( sArguments_[ "-c" ] ) );
  Tracking->SetLineColor( atoi( sArguments_[ "-c" ] ) );
  Tracking->GetXaxis()->SetBinLabel( 1, "Report" );
  Tracking->GetXaxis()->SetBinLabel( 2, "Certification" );

  for ( map< TString, Double_t >::const_iterator iCert = fCertificates_.begin(); iCert != fCertificates_.end(); ++iCert ) {
    const TString certName( iCert->first );
    const Ssiz_t  index( certName.Index( "_T" ) );
    if ( index < 0 ) continue;
    const TString histName( certName( 0, index ) );
    TH1D * hist( ( TH1D * )fileHisto->Get( histName.Data() ) );
    if ( ! hist ) continue;
    const TString subName( certName( index + 1, certName.Length() - index - 1 ) );
    UInt_t iSub( 0 );
    while ( iSub < nSiStripSubBF_ ) {
      if ( sSiStripSubBF_[ iSub ] == subName ) break;
      ++iSub;
    }
    if ( iSub == nSiStripSubBF_ ) continue;
    hist->Fill( sSiStripSubBF_[ iSub ].Data(), iCert->second );
  }
  SiStrip->Fill( "DCS"          , fCertificates_[ "SiStripDCSSummary" ] );
  SiStrip->Fill( "DAQ"          , fCertificates_[ "SiStripDAQSummary" ] );
  SiStrip->Fill( "Report"       , fCertificates_[ "SiStripReportSummary" ] );
  SiStrip->Fill( "Certification", fCertificates_[ "SiStripCertificationSummary" ] );
  Pixel->Fill( "DCS"   , fCertificates_[ "PixelDCSSummary" ] );
  Pixel->Fill( "DAQ"   , fCertificates_[ "PixelDAQSummary" ] );
  Pixel->Fill( "Report", fCertificates_[ "PixelReportSummary" ] );
  Tracking->Fill( "Report"       , fCertificates_[ "TrackingReportSummary" ] );
  Tracking->Fill( "Certification", fCertificates_[ "TrackingCertificationSummary" ] );

  fileHisto->Write();
  fileHisto->Close();

  return kTRUE;

}

/// Extract run certificates from DQM file
void readCertificates( TDirectory * dir )
{

  TIter nextKey( dir->GetListOfKeys() );
  TKey * key;
  while ( ( key = ( TKey * )nextKey() ) ) {
    const TString nameKey( key->GetName() );
    const Int_t index1( nameKey.Index( ">" ) );
    if ( index1 == kNPOS ) continue;
    TString nameCert( nameKey( 1, index1 - 1 ) );
    if ( TString( dir->GetName() ) == nameDirHead_ ) {
      if ( nameCert.CompareTo( "ReleaseTag" ) == 0 ) {
        const Ssiz_t indexKey( nameKey.Index( "s=" ) + 2 );
        const TString nameKeyBrake( nameKey( indexKey, nameKey.Length() - indexKey ) );
        sVersion_ = nameKeyBrake( 0, nameKeyBrake.Index( "<" ) );
      }
      continue;
    }
    TString nameCertFirst( nameCert( 0, 1 ) );
    nameCertFirst.ToUpper();
    nameCert.Replace( 0, 1, nameCertFirst );
    if ( TString( dir->GetName() ) == nameDirBase_ ) { // indicates summaries
      if ( ! nameCert.Contains( "Summary" ) ) continue;
      const TString nameDir( dir->GetPath() );
      const UInt_t index2( nameDir.Index( "/", nameDir.Index( ":" ) + 10 ) );
      const TString nameSub( nameDir( index2 + 1, nameDir.Index( "/", index2 + 1 ) - index2 - 1 ) );
      nameCert.Prepend( nameSub );
    } else if ( TString( dir->GetName() ) == nameDirCert_ ) {
      nameCert.Prepend( "Cert" );
    } else if ( TString( dir->GetName() ) == nameDirDAQ_ ) {
      nameCert.Prepend( "DAQ" );
    } else if ( TString( dir->GetName() ) == nameDirDCS_ ) {
      nameCert.Prepend( "DCS" );
    } else {
      nameCert.Prepend( "Report" );
    }
    const Ssiz_t  indexKey( nameKey.Index( "f=" ) + 2 );
    const TString nameKeyBrake( nameKey( indexKey, nameKey.Length() - indexKey ) );
    const TString nameKeyBrakeAll( nameKeyBrake( 0, nameKeyBrake.Index( "<" ) ) );
    fCertificates_[ nameCert ] = atof( nameKeyBrakeAll.Data() );
  }

  return;

}

/// Little helper to determine run number (TString) from file name/path
TString runNumber( const TString & pathFile )
{

  const TString sPrefix( "DQM_V0001_R" );
  const TString sNumber( pathFile( pathFile.Index( sPrefix ) + sPrefix.Length(), 9 ) );
  UInt_t index( ( string( sNumber.Data() ) ).find_first_not_of( '0' ) );
  return sNumber( index, sNumber.Length() - index );

}
