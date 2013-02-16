// -*- C++ -*-
//
// Package: DQM/TrackerCommon
//
// $Id$
//
/**
  \brief    Performs DQM offline data certification for SiStrip, Pixel and Tracking

   Purpose:

   The procedure of certifying data of a given run range is automated in order to speed up the procedure and to reduce the Tracker Offline Shift Leader's workload.

   Input:
   
   Text files in order to make the results of hDQM and TkMap based flags available to the script have to be provided:
   - ./certSiStrip.txt
   - ./hDQMSiStrip.txt
   - ./TkMapSiStrip.txt
   - ./certPixel.txt
   - ./hDQMPixel.txt
   - ./TkMapPixel.txt
   - ./certTracking.txt
   - ./hDQMTracking.txt
   The format of the entries in these files is the following:
   One line per run of the structure
   RUNNUMBER FLAG [COMMENT]
   where:
   - RUNNUMBER is obvious
   - FLAG is either "GOOD" or "BAD" (Anything different from "BAD" will be treated as "GOOD".)
   - COMMENT is an "obligatory" explanation in case of flag "BAD", which can have more than one word.
     However, brief'n'clear statements are preferred (to be standardized in the future).
   The files can be empty, but must be present!

   Further necessary sources of input are:
   - RunRegistry
   - DQM output files available in AFS

   Output:

   Text file
   - ./trackerRunCertification.txt
   to be sent directly to the CMS DQM team as reply to the weekly certification request.
   It contains a list of all flags changed with respect to the RunRegistry, including the reason(s) in case the flag is changed to BAD.

   The (lengthy) stdout provides a complete list of all in-/output flags of all analyzed runs and at its end a summary only with the output flags.
   This summary can be used to populate the Tracker Good/Bad Run List (http://cmstac05.cern.ch/ajax/pierro/offShift/#good_bad_run).
   It makes sense to pipe the stdout to another text file.

   Usage:
   
   $ cmsrel CMSSW_RELEASE
   $ cd CMSSW_RELEASE/src
   $ cmsenv
   $ cvs co -r Vxx-yy-zz DQM/TrackerCommon
   $ scram b -j 5
   $ rehash
   $ cd WORKING_DIRECTORY
   $ [create input files]
   $ TrackerRunCertification [FIRTS [LAST [PATH] ] ]

   All arguments are optional, but must be given in the correct order:
   - FIRST: run number to start certification from
   - LAST:  run number to end the certification with
   - PATH:  basic path to DQM output files in AFS, from where these files are visible (default: /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/data/Express)
   If no argument is given, the certification will be run for all runs with DQM files found in the default PATH.

  \author   Volker Adler
  \version  $Id$
*/


#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "TROOT.h"
#include "TSystem.h"
#include "TString.h"
#include "TFile.h"
#include "TKey.h"
#include "TXMLEngine.h" // needs '<flags LDFLAGS="-lXMLIO">' in the BuildFile


using namespace std;


// Functions
Bool_t  readFiles();
Bool_t  createInputFileList();
Bool_t  createRRFile();
Bool_t  readData( const TString & pathFile );
Bool_t  readRR( const TString & pathFile );
Bool_t  readDQM( const TString & pathFile );
void    readCertificates( TDirectory * dir );
void    certifyRunRange();
void    certifyRun();
void    writeOutput();
TString RunNumber( const TString & pathFile );
Int_t   FlagSToI( const TString & flag );
TString FlagIToS( const Int_t flag );

// Global variables, incl. default initialisation
TString pathDqmData_( "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/data/Express" ); // configurable
TString nameFileList_( "fileList.txt" );
TString nameFileRR_( "runRegistry.xml" );
TString nameFileCertSiStrip_( "certSiStrip.txt" );
TString nameFileHDQMSiStrip_( "hDQMSiStrip.txt" );
TString nameFileTkMapSiStrip_( "tkMapSiStrip.txt" );
TString nameFileCertPixel_( "certPixel.txt" );
TString nameFileHDQMPixel_( "hDQMPixel.txt" );
TString nameFileTkMapPixel_( "tkMapPixel.txt" );
TString nameFileCertTracking_( "certTracking.txt" );
TString nameFileHDQMTracking_( "hDQMTracking.txt" );
TString nameLog_( "trackerRunCertification.txt" );
Int_t  minRun_( 1073741824 ); // 2^30
Int_t  maxRun_(          0 );
Int_t  minRange_( maxRun_ ); // configurable
Int_t  maxRange_( minRun_ ); // configurable

// Global constants
const UInt_t nSubSys_( 3 );
const TString sSubSys_[ nSubSys_ ] = { // sub-system directory names in DQM files
  "SiStrip",
  "Pixel",
  "Tracking",
};
enum SubSystems { // according enumeration
  SiStrip,
  Pixel,
  Tracking
};
enum Flags { // flags' enumeration
  NOTSET = -99,
  EXCL   =  -1,
  BAD    =   0,
  GOOD   =   1
};
const Double_t minReportSummarySiStripDet_( 0.98 );
const Double_t minReportSummarySiStripSubDet_( 0.95 );
const Double_t minReportSummaryTracking_( 0.85 );
const Int_t    iRunNewStruct_( 111948 );
const Int_t    iRunStartDeconProbl_( 110213 );
const Int_t    iRunStopDeconProbl_( iRunNewStruct_ );

// Certificates and flags
vector< TString > sRunNumbers_;
UInt_t nRunsNotRR_( 0 );
UInt_t nRunsExclSiStrip_( 0 );
UInt_t nRunsBadSiStrip_( 0 );
UInt_t nRunsChangedSiStrip_( 0 );
map< TString, TString > sSiStrip_;
map< TString, TString > sRRSiStrip_;
map< TString, TString > sDQMSiStrip_;
map< TString, TString > sCertSiStrip_;
map< TString, TString > sHDQMSiStrip_;
map< TString, TString > sTkMapSiStrip_;
map< TString, vector< TString > > sRunCommentsSiStrip_;
UInt_t nRunsExclPixel_( 0 );
UInt_t nRunsBadPixel_( 0 );
UInt_t nRunsChangedPixel_( 0 );
map< TString, TString > sPixel_;
map< TString, TString > sRRPixel_;
map< TString, TString > sDQMPixel_;
map< TString, TString > sCertPixel_;
map< TString, TString > sHDQMPixel_;
map< TString, TString > sTkMapPixel_;
map< TString, vector< TString > > sRunCommentsPixel_;
UInt_t nRunsExclTracking_( 0 );
UInt_t nRunsBadTracking_( 0 );
UInt_t nRunsChangedTracking_( 0 );
map< TString, TString > sTracking_;
map< TString, TString > sRRTracking_;
map< TString, TString > sDQMTracking_;
map< TString, TString > sCertTracking_;
map< TString, TString > sHDQMTracking_;
map< TString, vector< TString > > sRunCommentsTracking_;
// Certificates and flags (run-by-run)
TString sRunNumber_( "0" );
map< TString, Double_t > fCertificates_;
map< TString, Int_t >    iFlagsRR_;
map< TString, Bool_t >   bAvailable_;



/// Checks arguments and runs input check/creation and run certification incl. output
int main( int argc, char * argv[] )
{

  cout << endl;
  
  // Input arguments
  if ( argc > 1 ) {
    const TString sRun( argv[ 1 ] );
    if ( ! sRun.IsDigit() ) {
      cout << "  ERROR: wrong usage" << endl;
      cout << "  TrackerRunCertification [basic path of DQM data] [first run number] [last run number]" << endl;
      cout << "  (\"first run number\") was not a number";
      return 1;
    }
    minRange_ = sRun.Atoi();
  }
  if ( argc > 2 ) {
    const TString sRun( argv[ 2 ] );
    if ( ! sRun.IsDigit() ) {
      cout << "  ERROR: wrong usage" << endl;
      cout << "  TrackerRunCertification [basic path of DQM data] [first run number] [last run number]" << endl;
      cout << "  (\"last run number\") was not a number";
      return 1;
    }
    maxRange_ = sRun.Atoi();
  }
  if ( argc > 3 ) {
    pathDqmData_ = argv[ 3 ];
  }
  if ( argc > 4 ) {
    cout << "  WARNING: too many arguments" << endl;
    cout << "  TrackerRunCertification [first run number] [last run number] [basic path of DQM data]" << endl;
    cout << "  redundant arguments discarded" << endl;
  };

  // Run
  if ( ! readFiles() )           return 11;
  if ( ! createInputFileList() ) return 12;
  if ( ! createRRFile() )        return 13;
  certifyRunRange();

  return 0;

}


/// Check existance of input files for hDQM and TkMap certificates and read them.
/// Only existing entries for bad runs are taken into account. Not appearing runs are assumed to be good without further warning.
/// Returns 'kTRUE', if all needed files are found, 'kFALSE' otherwise.
Bool_t readFiles()
{

  // Initialize
  Bool_t check( kTRUE );

  // open and check SiStrip certification file
  ifstream fileCertSiStripRead;
  fileCertSiStripRead.open( nameFileCertSiStrip_.Data() );
  if ( ! fileCertSiStripRead ) {
    cout << "    ERROR: no SiStrip general certificates' file" << endl;
    check = kFALSE;
  }

  // open and check SiStrip hDQM file
  ifstream fileHDQMSiStripRead;
  fileHDQMSiStripRead.open( nameFileHDQMSiStrip_.Data() );
  if ( ! fileHDQMSiStripRead ) {
    cout << "    ERROR: no SiStrip hDQM certificates' file" << endl;
    check = kFALSE;
  }

  // open and check SiStrip TkMap file
  ifstream fileTkMapSiStripRead;
  fileTkMapSiStripRead.open( nameFileTkMapSiStrip_.Data() );
  if ( ! fileTkMapSiStripRead ) {
    cout << "    ERROR: no SiStrip TkMap certificates' file" << endl;
    check = kFALSE;
  }

  // open and check Pixel certification file
  ifstream fileCertPixelRead;
  fileCertPixelRead.open( nameFileCertPixel_.Data() );
  if ( ! fileCertPixelRead ) {
    cout << "    ERROR: no Pixel general certificates' file" << endl;
    check = kFALSE;
  }

  // open and check Pixel hDQM file
  ifstream fileHDQMPixelRead;
  fileHDQMPixelRead.open( nameFileHDQMPixel_.Data() );
  if ( ! fileHDQMPixelRead ) {
    cout << "    ERROR: no Pixel hDQM certificates' file" << endl;
    check = kFALSE;
  }

  // open and check Pixel TkMap file
  ifstream fileTkMapPixelRead;
  fileTkMapPixelRead.open( nameFileTkMapPixel_.Data() );
  if ( ! fileTkMapPixelRead ) {
    cout << "    ERROR: no Pixel TkMap certificates' file" << endl;
    check = kFALSE;
  }

  // open and check Tracking certification file
  ifstream fileCertTrackingRead;
  fileCertTrackingRead.open( nameFileCertTracking_.Data() );
  if ( ! fileCertTrackingRead ) {
    cout << "    ERROR: no Tracking general certificates' file" << endl;
    check = kFALSE;
  }

  // open and check Tracking hDQM file
  ifstream fileHDQMTrackingRead;
  fileHDQMTrackingRead.open( nameFileHDQMTracking_.Data() );
  if ( ! fileHDQMTrackingRead ) {
    cout << "    ERROR: no Tracking hDQM certificates' file" << endl;
    check = kFALSE;
  }

  // Read data
  if ( check ) {
    while ( fileCertSiStripRead.good() ) {
      TString runNumber, runFlag;
      fileCertSiStripRead >> runNumber >> runFlag;
      if ( runNumber.Length() == 0 || runFlag != "BAD" ) continue;
      string comment;
      getline( fileCertSiStripRead, comment );
      TString runComment( comment.c_str() );
      sCertSiStrip_[ runNumber ] = runComment;
    }
    while ( fileHDQMSiStripRead.good() ) {
      TString runNumber, runFlag;
      fileHDQMSiStripRead >> runNumber >> runFlag;
      if ( runNumber.Length() == 0 || runFlag != "BAD" ) continue;
      string comment;
      getline( fileHDQMSiStripRead, comment );
      TString runComment( comment.c_str() );
      sHDQMSiStrip_[ runNumber ] = runComment;
    }
    while ( fileTkMapSiStripRead.good() ) {
      TString runNumber, runFlag;
      fileTkMapSiStripRead >> runNumber >> runFlag;
      if ( runNumber.Length() == 0 || runFlag != "BAD" ) continue;
      string comment;
      getline( fileTkMapSiStripRead, comment );
      TString runComment( comment.c_str() );
      sTkMapSiStrip_[ runNumber ] = runComment;
    }
    while ( fileCertPixelRead.good() ) {
      TString runNumber, runFlag;
      fileCertPixelRead >> runNumber >> runFlag;
      if ( runNumber.Length() == 0 || runFlag != "BAD" ) continue;
      string comment;
      getline( fileCertPixelRead, comment );
      TString runComment( comment.c_str() );
      sCertPixel_[ runNumber ] = runComment;
    }
    while ( fileHDQMPixelRead.good() ) {
      TString runNumber, runFlag;
      fileHDQMPixelRead >> runNumber >> runFlag;
      if ( runNumber.Length() == 0 || runFlag != "BAD" ) continue;
      string comment;
      getline( fileHDQMPixelRead, comment );
      TString runComment( comment.c_str() );
      sHDQMPixel_[ runNumber ] = runComment;
    }
    while ( fileTkMapPixelRead.good() ) {
      TString runNumber, runFlag;
      fileTkMapPixelRead >> runNumber >> runFlag;
      if ( runNumber.Length() == 0 || runFlag != "BAD" ) continue;
      string comment;
      getline( fileTkMapPixelRead, comment );
      TString runComment( comment.c_str() );
      sTkMapPixel_[ runNumber ] = runComment;
    }
    while ( fileCertTrackingRead.good() ) {
      TString runNumber, runFlag;
      fileCertTrackingRead >> runNumber >> runFlag;
      if ( runNumber.Length() == 0 || runFlag != "BAD" ) continue;
      string comment;
      getline( fileCertTrackingRead, comment );
      TString runComment( comment.c_str() );
      sCertTracking_[ runNumber ] = runComment;
    }
    while ( fileHDQMTrackingRead.good() ) {
      TString runNumber, runFlag;
      fileHDQMTrackingRead >> runNumber >> runFlag;
      if ( runNumber.Length() == 0 || runFlag != "BAD" ) continue;
      string comment;
      getline( fileHDQMTrackingRead, comment );
      TString runComment( comment.c_str() );
      sHDQMTracking_[ runNumber ] = runComment;
    }
  }
  
  fileCertSiStripRead.close();
  fileHDQMSiStripRead.close();
  fileTkMapSiStripRead.close();
  fileCertPixelRead.close();
  fileHDQMPixelRead.close();
  fileTkMapPixelRead.close();
  fileCertTrackingRead.close();
  fileHDQMTrackingRead.close();

  return check;

}


/// Checks for DQM RooT files in pre-defined directory, compares to optinally given run range and writes the resulting file list to a file
/// Returns 'kTRUE', if DQM files for the given run range and path have been found, 'kFALSE' otherwise.
Bool_t createInputFileList()
{

  // Create input file list on the fly
  gSystem->Exec( TString( "rm " + nameFileList_ ).Data() );
  gSystem->Exec( TString( "ls -1 " + pathDqmData_ + "/*/*/*.root > " + nameFileList_ ).Data() );
  ofstream fileListWrite;
  fileListWrite.open( nameFileList_.Data(), ios_base::app );
  fileListWrite << "EOF";
  fileListWrite.close();
  
  // Loop over input file list and recreate it according to run range
  ifstream fileListRead;
  fileListRead.open( nameFileList_.Data() );
  ofstream fileListNewWrite;
  const TString nameFileListNew( nameFileList_ + ".new" );
  fileListNewWrite.open( nameFileListNew, ios_base::app );
  UInt_t nFiles( 0 );
  while ( fileListRead.good() ) {
    TString pathFile;
    fileListRead >> pathFile;
    if ( pathFile.Length() == 0 ) continue;
    sRunNumber_ = RunNumber( pathFile );
    if ( ! sRunNumber_.IsDigit() ) continue;
    ++nFiles;
    const Int_t iRun( sRunNumber_.Atoi() );
    if ( minRange_ > iRun || iRun > maxRange_ ) continue;
    fileListNewWrite << pathFile << endl;
    if ( iRun < minRun_ ) minRun_ = iRun;
    if ( iRun > maxRun_ ) maxRun_ = iRun;
  }
  
  fileListRead.close();
  fileListNewWrite.close();
  gSystem->Exec( TString( "mv " ).Append( nameFileListNew ).Append( " " ).Append( nameFileList_ ) );

  if ( nFiles == 0 ) {
    cout << "  WARNING: no files to certify" << endl;
    cout << "  no files found in " << pathDqmData_ << " between the run numbers " << minRange_ << " and " << maxRange_ << endl;
    return kFALSE;
  }
  return kTRUE;

}


/// Gets XML file with complete RunRegistry information from the web server
/// Returns 'kTRUE', if XML file is present and not empty, 'kFALSE' otherwise.
Bool_t createRRFile()
{

  const TString xmlRRAddress( "http://pccmsdqm04.cern.ch/runregistry/runregisterdata?format=xml&intpl=xml&mime=text/xml&qtype=RUN_NUMBER&sortname=RUN_NUMBER" );
  const TString nameFileRRTmp( nameFileRR_ + ".tmp" );
  gSystem->Exec( TString( "wget -q -O " ).Append( nameFileRRTmp ).Append( " " ).Append( xmlRRAddress ).Data() );
  clock_t sleep( 2 * CLOCKS_PER_SEC + clock() ); // minimum 2 seconds delay to have the file completely downloaded
  while ( sleep > clock() );

  // Convert RR file to be readable by TXMLEngine
  TXMLEngine * xmlRR( new TXMLEngine );
  XMLDocPointer_t xmlRRDoc( xmlRR->NewDoc() );
  xmlRR->SaveDoc( xmlRRDoc, nameFileRR_.Data() );
  xmlRR->FreeDoc( xmlRRDoc );
  ifstream fileRRTmp;
  fileRRTmp.open( nameFileRRTmp.Data() );
  if ( ! fileRRTmp ) {
    cout << "  ERROR: temporary RR file does not exist" << endl;
    cout << "  Please, check access to RR" << endl;
    return kFALSE;
  }
  ofstream fileRR;
  fileRR.open( nameFileRR_.Data(), ios_base::app );
  const UInt_t maxLength( 131071 ); // hard-coding for what?
  char xmlLine[ maxLength ];
  UInt_t lines( 0 );
  while ( fileRRTmp.getline( xmlLine, maxLength ) ) {
    fileRR << xmlLine << endl;
    ++lines;
  }
  gSystem->Exec( TString( "rm " + nameFileRRTmp ).Data() );
  if ( lines <= 1 ) {
    cout << "  ERROR: empty RR file" << endl;
    cout << "  Please, check access to RR" << endl;
    return kFALSE;
  }
  fileRR.close();
  fileRRTmp.close();
  
  return kTRUE;

}


/// Loops over runs
void certifyRunRange()
{

  // Loop over runs
  ifstream fileListRead;
  fileListRead.open( nameFileList_.Data() );
  while ( fileListRead.good() ) {
    TString pathFile;
    fileListRead >> pathFile;
    if ( pathFile.Length() == 0 ) continue;
    sRunNumber_ = RunNumber( pathFile );
    cout << "  Processing RUN " << sRunNumber_.Data();
    if ( readData( pathFile ) ) {
      sRunNumbers_.push_back( sRunNumber_ );
      certifyRun();
    }
  }
  fileListRead.close();
  writeOutput();
  
  return;

}


/// Reads input data for a given run
/// Returns 'kTRUE', if RR and DQM information have been read successfully, 'kFALSE' otherwise.
Bool_t readData( const TString & pathFile )
{

  if ( ! readRR( pathFile ) )  return kFALSE;
  if ( ! readDQM( pathFile ) ) return kFALSE;
  return kTRUE;

}


/// Reads manually set RR certification flags for a given run
/// Returns 'kTRUE', if a given run is present in RR, 'kFALSE' otherwise.
Bool_t readRR( const TString & pathFile )
{

  // Initialize
  map< TString, TString > sFlagsRR;
  iFlagsRR_.clear();

  // Read RR file
  TXMLEngine * xmlRR( new TXMLEngine );
  XMLDocPointer_t  xmlRRDoc( xmlRR->ParseFile( nameFileRR_.Data() ) );
  XMLNodePointer_t nodeMain( xmlRR->DocGetRootElement( xmlRRDoc ) );
  vector< TString > nameFlagNode;
  nameFlagNode.push_back( "on_STRIP" );
  nameFlagNode.push_back( "on_PIX" );
  nameFlagNode.push_back( "off_STRIP" );
  nameFlagNode.push_back( "off_PIX" );
  nameFlagNode.push_back( "off_TRACK" );
  Bool_t foundRun( kFALSE );
  XMLNodePointer_t nodeRun( xmlRR->GetChild( nodeMain ) );
  while ( nodeRun ) {
    XMLNodePointer_t nodeRunChild( xmlRR->GetChild( nodeRun ) );
    while ( nodeRunChild && TString( xmlRR->GetNodeName( nodeRunChild ) ) != "RUN_NUMBER" ) {
      nodeRunChild = xmlRR->GetNext( nodeRunChild );
    }
    if ( nodeRunChild ) {
      if ( xmlRR->GetNodeContent( nodeRunChild ) == sRunNumber_ ) {
        nodeRunChild = xmlRR->GetChild( nodeRun );
        while ( nodeRunChild ) {
          const TString nameNode( xmlRR->GetNodeName( nodeRunChild ) );
          for ( UInt_t iNameNode = 0; iNameNode < nameFlagNode.size(); ++iNameNode ) {
            if ( nameNode == nameFlagNode.at( iNameNode ) ) {
              sFlagsRR[ nameNode ] = TString( xmlRR->GetNodeContent( nodeRunChild ) );
            }
          }
          nodeRunChild = xmlRR->GetNext( nodeRunChild );
        }
        foundRun = kTRUE;
        break;
      }
    }
    nodeRun = xmlRR->GetNext( nodeRun );
  }
  xmlRR->FreeDoc( xmlRRDoc );
  
  if ( ! foundRun ) {
    ++nRunsNotRR_;
    cout << " --> not found in RR" << endl;
    return kFALSE;
  }
  cout << endl;
  
  for ( map< TString, TString >::const_iterator flag = sFlagsRR.begin(); flag != sFlagsRR.end(); ++flag ) {
    cout << "    RR: " << flag->first << ": " << flag->second << endl;
  }
  // FIXME Check for missing RR flags possibly needed here(?)

  iFlagsRR_[ sSubSys_[ SiStrip ] ]  = FlagSToI( sFlagsRR[ "off_STRIP" ] );
  iFlagsRR_[ sSubSys_[ Pixel ] ]    = FlagSToI( sFlagsRR[ "off_PIX" ] );
  iFlagsRR_[ sSubSys_[ Tracking ] ] = FlagSToI( sFlagsRR[ "off_TRACK" ] );
  if ( iFlagsRR_[ sSubSys_[ SiStrip ] ] == EXCL ) ++nRunsExclSiStrip_;
  if ( iFlagsRR_[ sSubSys_[ Pixel ] ] == EXCL ) ++nRunsExclPixel_;
  if ( iFlagsRR_[ sSubSys_[ SiStrip ] ] == EXCL && iFlagsRR_[ sSubSys_[ Pixel ] ] == EXCL ) ++nRunsExclTracking_;
  
  return kTRUE;

}


/// Reads autom,atically created certification flags/values from the DQM file for a given run
/// Returns 'kTRUE', if the DQM file is readable, 'kFALSE' otherwise.
Bool_t readDQM( const TString & pathFile )
{                    

  // Initialize
  fCertificates_.clear();
  bAvailable_.clear();

  // Open DQM file
  TFile * fileDQM( TFile::Open( pathFile.Data() ) );
  if ( ! fileDQM ) {
    cout << "    ERROR: DQM file not found" << endl;
    cout << "    Please, check path to DQM files" << endl;
    return kFALSE;
  }

  // Browse certification folders
  vector< TString > nameCertDir;
  for ( UInt_t iSys = 0; iSys < nSubSys_; ++iSys ) {
    bAvailable_[ sSubSys_[ iSys ] ] = ( iFlagsRR_[ sSubSys_[ iSys ] ] != EXCL );
    if ( bAvailable_[ sSubSys_[ iSys ] ] ) {
      const TString baseDir( "/DQMData/Run /" + sSubSys_[ iSys ] + "/Run summary/EventInfo" );
      nameCertDir.push_back( baseDir );
      nameCertDir.push_back( baseDir + "/CertificationContents" );
      nameCertDir.push_back( baseDir + "/reportSummaryContents" );
      if ( iSys != Tracking ) {
        nameCertDir.push_back( baseDir + "/DAQContents" );
        nameCertDir.push_back( baseDir + "/DCSContents" );
      }
    }
  }
  for ( UInt_t iDir = 0; iDir < nameCertDir.size(); ++iDir ) {
    TString nameCurDir( nameCertDir.at( iDir ).Insert( nameCertDir.at( iDir ).Index( "Run " ) + 4, sRunNumber_ ) );
    TDirectory * dirSub( ( TDirectory * )fileDQM->Get( nameCurDir.Data() ) );
    if ( ! dirSub ) {
      cout << "    WARNING: " << nameCurDir.Data() << " does not exist" << endl;
      continue;
    }
    readCertificates( dirSub );
  }
  
  fileDQM->Close();
  
  for ( map< TString, Double_t >::const_iterator cert = fCertificates_.begin(); cert != fCertificates_.end(); ++cert ) {
    cout << "    " << cert->first << ": " << cert->second << endl;
  }
  
  return kTRUE;
  
}


/// Extract run certificates from DQM file
void readCertificates( TDirectory * dir )
{
  
  TIter nextKey( dir->GetListOfKeys() );
  TKey * key;
  while ( key = ( TKey * )nextKey() ) {
    const TString nameKey( key->GetName() );
    const Int_t index1( nameKey.Index( ">" ) );
    if ( index1 == kNPOS ) continue;
    TString nameCert( nameKey( 1, index1 - 1 ) );
    TString nameCertFirst( nameCert( 0, 1 ) );
    nameCertFirst.ToUpper();
    nameCert.Replace( 0, 1, nameCertFirst );
    if ( TString( dir->GetName() ) == "EventInfo" ) { // indicates summaries
      if ( ! nameCert.Contains( "Summary" ) ) continue;
      const TString nameDir( dir->GetPath() );
      const UInt_t index2( nameDir.Index( "/", nameDir.Index( ":" ) + 10 ) );
      const TString nameSub( nameDir( index2 + 1, nameDir.Index( "/", index2 + 1 ) - index2 - 1 ) );
      nameCert.Prepend( nameSub );
    } else if ( TString( dir->GetName() ) == "CertificationContents" ) {
      nameCert.Prepend( "Cert" );
    } else if ( TString( dir->GetName() ) == "DAQContents" ) {
      nameCert.Prepend( "DAQ" );
    } else if ( TString( dir->GetName() ) == "DCSContents" ) {
      nameCert.Prepend( "DCS" );
    } else {
      nameCert.Prepend( "Report" );
    }
    fCertificates_[ nameCert ] = atof( ( nameKey( nameKey.Index( "f=" ) + 2, nameKey.Length() - nameKey.Index( "f=" ) - 2 ) ).Data() );
  }
  
  return;

}


/// Determine actual certification flags per run and sub-system
void certifyRun()
{
  
  // Initialize
  map< TString, Int_t > iFlags;
  
  // SiStrip
  if ( bAvailable_[ sSubSys_[ SiStrip ] ] ) {
    Bool_t flagDet;
    Bool_t flagSubDet;
    Bool_t flagSToN;
    if ( iRunStartDeconProbl_ <= sRunNumber_.Atoi() && sRunNumber_.Atoi() <= iRunStopDeconProbl_ ) { // special treatment for switch to deconvolution mode without changing CMSSW release
      flagDet = kTRUE;
      flagSubDet = ( // FIXME to be discussed (this is from DQM shifter instr.)
        ( fCertificates_[ "ReportSiStrip_DetFraction_TECB" ] == ( Double_t )EXCL || fCertificates_[ "ReportSiStrip_DetFraction_TECB" ] > minReportSummarySiStripSubDet_ ) &&
        ( fCertificates_[ "ReportSiStrip_DetFraction_TECF" ] == ( Double_t )EXCL || fCertificates_[ "ReportSiStrip_DetFraction_TECF" ] > minReportSummarySiStripSubDet_ ) &&
        ( fCertificates_[ "ReportSiStrip_DetFraction_TIB" ]  == ( Double_t )EXCL || fCertificates_[ "ReportSiStrip_DetFraction_TIB" ]  > minReportSummarySiStripSubDet_ ) &&
        ( fCertificates_[ "ReportSiStrip_DetFraction_TIDB" ] == ( Double_t )EXCL || fCertificates_[ "ReportSiStrip_DetFraction_TIDB" ] > minReportSummarySiStripSubDet_ ) &&
        ( fCertificates_[ "ReportSiStrip_DetFraction_TIDF" ] == ( Double_t )EXCL || fCertificates_[ "ReportSiStrip_DetFraction_TIDF" ] > minReportSummarySiStripSubDet_ ) &&
        ( fCertificates_[ "ReportSiStrip_DetFraction_TOB" ]  == ( Double_t )EXCL || fCertificates_[ "ReportSiStrip_DetFraction_TOB" ]  > minReportSummarySiStripSubDet_ )
      );
      flagSToN =kTRUE;
    } else {
      flagDet = ( fCertificates_[ "SiStripReportSummary" ] > minReportSummarySiStripDet_ );
      flagSubDet = ( // FIXME to be discussed (this is from DQM shifter instr.)
        ( fCertificates_[ "ReportSiStrip_TECB" ] == ( Double_t )EXCL || fCertificates_[ "ReportSiStrip_TECB" ] > minReportSummarySiStripSubDet_ ) &&
        ( fCertificates_[ "ReportSiStrip_TECF" ] == ( Double_t )EXCL || fCertificates_[ "ReportSiStrip_TECF" ] > minReportSummarySiStripSubDet_ ) &&
        ( fCertificates_[ "ReportSiStrip_TIB" ]  == ( Double_t )EXCL || fCertificates_[ "ReportSiStrip_TIB" ]  > minReportSummarySiStripSubDet_ ) &&
        ( fCertificates_[ "ReportSiStrip_TIDB" ] == ( Double_t )EXCL || fCertificates_[ "ReportSiStrip_TIDB" ] > minReportSummarySiStripSubDet_ ) &&
        ( fCertificates_[ "ReportSiStrip_TIDF" ] == ( Double_t )EXCL || fCertificates_[ "ReportSiStrip_TIDF" ] > minReportSummarySiStripSubDet_ ) &&
        ( fCertificates_[ "ReportSiStrip_TOB" ]  == ( Double_t )EXCL || fCertificates_[ "ReportSiStrip_TOB" ]  > minReportSummarySiStripSubDet_ )
      );
      flagSToN = (
        ( fCertificates_[ "ReportSiStrip_SToNFlag_TECB" ] == ( Double_t )EXCL || fCertificates_[ "ReportSiStrip_SToNFlag_TECB" ] == ( Double_t )GOOD ) &&
        ( fCertificates_[ "ReportSiStrip_SToNFlag_TECF" ] == ( Double_t )EXCL || fCertificates_[ "ReportSiStrip_SToNFlag_TECF" ] == ( Double_t )GOOD ) &&
        ( fCertificates_[ "ReportSiStrip_SToNFlag_TIB" ]  == ( Double_t )EXCL || fCertificates_[ "ReportSiStrip_SToNFlag_TIB" ]  == ( Double_t )GOOD ) &&
        ( fCertificates_[ "ReportSiStrip_SToNFlag_TIDB" ] == ( Double_t )EXCL || fCertificates_[ "ReportSiStrip_SToNFlag_TIDB" ] == ( Double_t )GOOD ) &&
        ( fCertificates_[ "ReportSiStrip_SToNFlag_TIDF" ] == ( Double_t )EXCL || fCertificates_[ "ReportSiStrip_SToNFlag_TIDF" ] == ( Double_t )GOOD ) &&
        ( fCertificates_[ "ReportSiStrip_SToNFlag_TOB" ]  == ( Double_t )EXCL || fCertificates_[ "ReportSiStrip_SToNFlag_TOB" ]  == ( Double_t )GOOD )
      );
    }
    Bool_t flagDAQ;
    Bool_t flagDCS;
    if ( sRunNumber_.Atoi() <= iRunNewStruct_ ) { // Change of DQM output structure with new CMSSW release
//       flagDAQ = ( fCertificates_[ "DAQSiStripDaqFraction" ] == ( Double_t )EXCL || fCertificates_[ "DAQSiStripDaqFraction" ] > 0.98 );
      flagDAQ = kTRUE; // FIXME clear up correct threshold
      flagDCS = ( fCertificates_[ "DCSSiStripDcsFraction" ] == ( Double_t )EXCL || fCertificates_[ "DCSSiStripDcsFraction" ] == ( Double_t )GOOD );
    } else {
      flagDAQ = ( fCertificates_[ "SiStripDAQSummary" ] == ( Double_t )EXCL || fCertificates_[ "SiStripDAQSummary" ] > minReportSummarySiStripSubDet_ ); // FIXME clear up correct threshold/treatment
      flagDCS = ( fCertificates_[ "SiStripDCSSummary" ] == ( Double_t )EXCL || fCertificates_[ "SiStripDCSSummary" ] == ( Double_t )GOOD );
    }
//     Bool_t flagDQM( flagDet * flagSubDet * flagSToN * flagDAQ * flagDSC );
    Bool_t flagDQM( flagDet * flagSubDet * flagSToN * flagDAQ ); // FIXME DCS info not yet determined correctly
    Bool_t flagCert( sCertSiStrip_.find( sRunNumber_ )   == sCertSiStrip_.end() );
    Bool_t flagHDQM( sHDQMSiStrip_.find( sRunNumber_ )   == sHDQMSiStrip_.end() );
    Bool_t flagTkMap( sTkMapSiStrip_.find( sRunNumber_ ) == sTkMapSiStrip_.end() );
    iFlags[ sSubSys_[ SiStrip ] ] = ( Int_t )( flagDQM * flagCert * flagHDQM * flagTkMap );
    
    sRRSiStrip_[ sRunNumber_ ] = FlagIToS( iFlagsRR_[ sSubSys_[ SiStrip ] ] );
    sDQMSiStrip_[ sRunNumber_ ] = FlagIToS( ( Int_t )( flagDQM ) );
    sSiStrip_[ sRunNumber_ ] = FlagIToS( iFlags[ sSubSys_[ SiStrip ] ] );
    vector< TString > comments;
    if ( ! flagDet )     comments.push_back( "too low overall fraction of good modules" );
    if ( ! flagSubDet )  comments.push_back( "too low fraction of good modules in a sub-system" );
    if ( ! flagSToN )    comments.push_back( "too low S/N in a sub-system" );
    if ( ! flagDAQ )     comments.push_back( "DAQSummary BAD" );
//     if ( ! flagDCS )     comments.push_back( "DCSSummary BAD" ); // FIXME DCS info not yet determined correctly
    if ( ! flagCert )    comments.push_back( "general: " + sCertSiStrip_[ sRunNumber_ ] );
    if ( ! flagHDQM )    comments.push_back( "hDQM   : " + sHDQMSiStrip_[ sRunNumber_ ] );
    if ( ! flagTkMap )   comments.push_back( "TkMap  : " + sTkMapSiStrip_[ sRunNumber_ ] );
    if ( iFlags[ sSubSys_[ SiStrip ] ] == BAD ) {
      ++nRunsBadSiStrip_;
      sRunCommentsSiStrip_[ sRunNumber_ ] = comments;
    }
  }

  // Pixel
  if ( bAvailable_[ sSubSys_[ Pixel ] ] ) {
    Bool_t flagReportSummary(
      fCertificates_[ "PixelReportSummary" ] == ( Double_t )GOOD
    );
    Bool_t flagDAQ;
    Bool_t flagDCS;
    if ( sRunNumber_.Atoi() < iRunNewStruct_ ) {
//       flagDAQ = ( fCertificates_[ "DAQPixelDaqFraction" ] == ( Double_t )EXCL || fCertificates_[ "DAQPixelDaqFraction" ] > 0.98 );
      flagDAQ = kTRUE; // FIXME clear up correct threshold
      flagDCS = ( fCertificates_[ "DCSPixelDcsFraction" ] == ( Double_t )EXCL || fCertificates_[ "DCSPixelDcsFraction" ] == ( Double_t )GOOD );
    } else {
      flagDAQ = ( fCertificates_[ "PixelDAQSummary" ] == ( Double_t )EXCL || fCertificates_[ "PixelDAQSummary" ] == ( Double_t )GOOD ); // FIXME Follow up flag definition
      flagDCS = ( fCertificates_[ "PixelDCSSummary" ] == ( Double_t )EXCL || fCertificates_[ "PixelDCSSummary" ] == ( Double_t )GOOD );
    } 
//     Bool_t flagDQM( flagReportSummary * flagDAQ * flagDCS );
    Bool_t flagDQM( flagReportSummary * flagDAQ ); // FIXME DCS info not yet determined correctly
    Bool_t flagCert( sCertPixel_.find( sRunNumber_ ) == sCertPixel_.end() );
    Bool_t flagHDQM( sHDQMPixel_.find( sRunNumber_ ) == sHDQMPixel_.end() );
    Bool_t flagTkMap( sTkMapPixel_.find( sRunNumber_ ) == sTkMapPixel_.end() );
    iFlags[ sSubSys_[ Pixel ] ] = ( Int_t )( flagDQM * flagCert * flagHDQM * flagTkMap );
    
    sRRPixel_[ sRunNumber_ ] = FlagIToS( iFlagsRR_[ sSubSys_[ Pixel ] ] );
    sDQMPixel_[ sRunNumber_ ] = FlagIToS( ( Int_t )( flagDQM ) );
    sPixel_[ sRunNumber_ ] = FlagIToS( iFlags[ sSubSys_[ Pixel ] ] );
    vector< TString > comments;
    if ( ! flagReportSummary ) comments.push_back( "ReportSummary BAD" );
    if ( ! flagDAQ )           comments.push_back( "DAQSummary BAD" );
//     if ( ! flagDCS )           comments.push_back( "DCSSummary BAD" ); // FIXME DCS info not yet determined correctly
    if ( ! flagCert )          comments.push_back( "general: " + sCertPixel_[ sRunNumber_ ] );
    if ( ! flagHDQM )          comments.push_back( "hDQM   : " + sHDQMPixel_[ sRunNumber_ ] );
    if ( ! flagTkMap )         comments.push_back( "TkMap  : " + sTkMapPixel_[ sRunNumber_ ] );
    if ( iFlags[ sSubSys_[ Pixel ] ] == BAD ) {
      ++nRunsBadPixel_;
      sRunCommentsPixel_[ sRunNumber_ ] = comments;
    }
  }

  // Tracking
  if ( bAvailable_[ sSubSys_[ Tracking ] ] ) {
    Bool_t flagChi2(
      fCertificates_[ "ReportTrackChi2overDoF" ] > minReportSummaryTracking_
    );
    Bool_t flagRate(
      fCertificates_[ "ReportTrackRate" ] > minReportSummaryTracking_
    );
    Bool_t flagRecHits(
      fCertificates_[ "ReportTrackRecHits" ] > minReportSummaryTracking_
    );
    Bool_t flagDQM( flagChi2 * flagRate * flagRecHits );
    Bool_t flagCert( sCertTracking_.find( sRunNumber_ ) == sCertTracking_.end() );
    Bool_t flagHDQM( sHDQMTracking_.find( sRunNumber_ ) == sHDQMTracking_.end() );
    iFlags[ sSubSys_[ Tracking ] ] = ( Int_t )( flagDQM * flagCert * flagHDQM );

    sRRTracking_[ sRunNumber_ ]  = FlagIToS( iFlagsRR_[ sSubSys_[ Tracking ] ] );
    sDQMTracking_[ sRunNumber_ ] = FlagIToS( -99 );
    sTracking_[ sRunNumber_ ]    = FlagIToS( iFlags[ sSubSys_[ Tracking ] ] );
    vector< TString > comments;
    if ( ! flagChi2 )    comments.push_back( "Chi2/DoF too low" );
    if ( ! flagRate )    comments.push_back( "Track rate too low" );
    if ( ! flagRecHits ) comments.push_back( "Too few RecHits" );
    if ( ! flagCert )    comments.push_back( "general: " + sCertTracking_[ sRunNumber_ ] );
    if ( ! flagHDQM )    comments.push_back( "hDQM   : " + sHDQMTracking_[ sRunNumber_ ] );
    if ( iFlags[ sSubSys_[ Tracking ] ] == BAD ) {
      ++nRunsBadTracking_;
      sRunCommentsTracking_[ sRunNumber_ ] = comments;
    }
  }

  for ( map< TString, Int_t >::const_iterator iSys = iFlags.begin(); iSys != iFlags.end(); ++iSys ) {
    if ( iSys->second != iFlagsRR_[ iSys->first ] ) {
      if ( iSys->first == sSubSys_[ SiStrip ] )  ++nRunsChangedSiStrip_;
      if ( iSys->first == sSubSys_[ Pixel ] )    ++nRunsChangedPixel_;
      if ( iSys->first == sSubSys_[ Tracking ] ) ++nRunsChangedTracking_;
      cout << "    " << iSys->first << ": " << FlagIToS( iFlagsRR_[ iSys->first ] ) << " --> " << FlagIToS( iSys->second ) << endl;
    }
  }
  
  return;

}


/// Print summary
void writeOutput()
{

  // Initialize
  ofstream fileLog;
  fileLog.open( nameLog_.Data() );
  fileLog << "Tracker Certification runs " << minRange_ << " - " << maxRange_ << endl << "==========================================" << endl << endl;
  fileLog << "Used DQM files found in " << pathDqmData_ << endl << endl;
  fileLog << "# of runs certified         : " << sRunNumbers_.size()   << endl;
  fileLog << "# of runs not found in RR   : " << nRunsNotRR_           << endl << endl;
  fileLog << "# of runs w/o SiStrip       : " << nRunsExclSiStrip_     << endl;
  fileLog << "# of bad runs SiStrip       : " << nRunsBadSiStrip_      << endl;
  fileLog << "# of changed runs SiStrip   : " << nRunsChangedSiStrip_  << endl;
  fileLog << "# of runs w/o Pixel         : " << nRunsExclPixel_       << endl;
  fileLog << "# of bad runs Pixel         : " << nRunsBadPixel_        << endl;
  fileLog << "# of changed runs Pixel     : " << nRunsChangedPixel_    << endl;
  fileLog << "# of runs w/o Tracking (BAD): " << nRunsExclTracking_    << endl;
  fileLog << "# of bad runs Tracking      : " << nRunsBadTracking_     << endl;
  fileLog << "# of changed runs Tracking  : " << nRunsChangedTracking_ << endl;

  // SiStrip
  fileLog << endl << sSubSys_[ 0 ] << ":" << endl << endl;
  for ( UInt_t iRun = 0; iRun < sRunNumbers_.size(); ++iRun ) {
    if ( sRRSiStrip_[ sRunNumbers_.at( iRun ) ] != sSiStrip_[ sRunNumbers_.at( iRun ) ] ) {
      fileLog << "  " << sRunNumbers_.at( iRun ) << ": " << sRRSiStrip_[ sRunNumbers_.at( iRun ) ] << " --> " << sSiStrip_[ sRunNumbers_.at( iRun ) ] << endl;
      for ( UInt_t iCom = 0; iCom < sRunCommentsSiStrip_[ sRunNumbers_.at( iRun ) ].size(); ++iCom ) {
        fileLog << "    " << sRunCommentsSiStrip_[ sRunNumbers_.at( iRun ) ].at( iCom ).Data() << endl;
      }
    }
  }

  // Pixel
  fileLog << endl << sSubSys_[ 1 ] << ":" << endl << endl;
  for ( UInt_t iRun = 0; iRun < sRunNumbers_.size(); ++iRun ) {
    if ( sRRPixel_[ sRunNumbers_.at( iRun ) ] != sPixel_[ sRunNumbers_.at( iRun ) ] ) {
      fileLog << "  " << sRunNumbers_.at( iRun ) << ": " << sRRPixel_[ sRunNumbers_.at( iRun ) ] << " --> " << sPixel_[ sRunNumbers_.at( iRun ) ] << endl;
      for ( UInt_t iCom = 0; iCom < sRunCommentsPixel_[ sRunNumbers_.at( iRun ) ].size(); ++iCom ) {
        fileLog << "    " << sRunCommentsPixel_[ sRunNumbers_.at( iRun ) ].at( iCom ).Data() << endl;
      }
    }
  }

  // Tracking
  fileLog << endl << sSubSys_[ 2 ] << ":" << endl << endl;
  for ( UInt_t iRun = 0; iRun < sRunNumbers_.size(); ++iRun ) {
    if ( sRRTracking_[ sRunNumbers_.at( iRun ) ] != sTracking_[ sRunNumbers_.at( iRun ) ] ) {
      fileLog << "  " << sRunNumbers_.at( iRun ) << ": " << sRRTracking_[ sRunNumbers_.at( iRun ) ] << " --> " << sTracking_[ sRunNumbers_.at( iRun ) ] << endl;
      for ( UInt_t iCom = 0; iCom < sRunCommentsTracking_[ sRunNumbers_.at( iRun ) ].size(); ++iCom ) {
        fileLog << "    " << sRunCommentsTracking_[ sRunNumbers_.at( iRun ) ].at( iCom ).Data() << endl;
      }
    }
  }
  
  fileLog.close();

  cout << endl << "SUMMARY:" << endl << endl;
  for ( UInt_t iRun = 0; iRun < sRunNumbers_.size(); ++iRun ) {
    cout << "  " << sRunNumbers_.at( iRun ) << ":" << endl;
    cout << "    " << sSubSys_[ 0 ] << ": " << sSiStrip_[ sRunNumbers_.at( iRun ) ] << endl;
    for ( UInt_t iCom = 0; iCom < sRunCommentsSiStrip_[ sRunNumbers_.at( iRun ) ].size(); ++iCom ) {
      cout << "      " << sRunCommentsSiStrip_[ sRunNumbers_.at( iRun ) ].at( iCom ).Data() << endl;
    }
    cout << "    " << sSubSys_[ 1 ] << ": " << sPixel_[ sRunNumbers_.at( iRun ) ] << endl;
    for ( UInt_t iCom = 0; iCom < sRunCommentsPixel_[ sRunNumbers_.at( iRun ) ].size(); ++iCom ) {
      cout << "      " << sRunCommentsPixel_[ sRunNumbers_.at( iRun ) ].at( iCom ).Data() << endl;
    }
    cout << "    " << sSubSys_[ 2 ] << ": " << sTracking_[ sRunNumbers_.at( iRun ) ] << endl;
    for ( UInt_t iCom = 0; iCom < sRunCommentsTracking_[ sRunNumbers_.at( iRun ) ].size(); ++iCom ) {
      cout << "      " << sRunCommentsTracking_[ sRunNumbers_.at( iRun ) ].at( iCom ).Data() << endl;
    }
  }
  
  cout << endl << "Certification SUMMARY to be sent to CMS DQM team available in ./" << nameLog_.Data() << endl << endl;
  
  return;

}


/// Little helper to determine run number (TString) from file name/path
TString RunNumber( const TString & pathFile )
{

  const TString sPrefix( "DQM_V0001_R" );
  const TString sNumber( pathFile( pathFile.Index( sPrefix ) + sPrefix.Length(), 9 ) );
  UInt_t index( ( string( sNumber.Data() ) ).find_first_not_of( '0' ) );
  return sNumber( index, sNumber.Length() - index );

}


/// Little helper to convert RR flags from TString into Int_t
Int_t FlagSToI( const TString & flag )
{
  map< TString, Int_t > flagSToI;
  flagSToI[ "NOTSET" ] = NOTSET;
  flagSToI[ "EXCL" ]   = EXCL;
  flagSToI[ "BAD" ]    = BAD;
  flagSToI[ "GOOD" ]   = GOOD;
  return flagSToI[ flag ];

}


/// Little helper to convert RR flags from Int_t into TString
TString FlagIToS( const Int_t flag )
{
  map< Int_t, TString > flagIToS;
  flagIToS[ NOTSET ] = "NOTSET";
  flagIToS[ EXCL ]   = "EXCL";
  flagIToS[ BAD ]    = "BAD";
  flagIToS[ GOOD ]   = "GOOD";
  return flagIToS[ flag ];

}
