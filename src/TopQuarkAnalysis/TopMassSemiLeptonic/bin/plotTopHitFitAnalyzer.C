#include <string>
#include <vector>
#include <iostream>

#include <TROOT.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TFile.h>
#include <TKey.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TCanvas.h>

#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"


int main(  int argc, char * argv[] )
{

  int returnStatus_( 0 );

  // Load libraries
  gSystem->Load( "libFWCoreFWLite" );
  AutoLibraryLoader::enable();

  // Adjust style
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
  gStyle->SetOptStat( 0 );
  gStyle->SetOptFit( 0 );
  gStyle->SetMarkerStyle( 8 );

  // Check configuration file
  if ( argc < 2 ) {
    std::cout << argv[ 0 ] << " --> Usage:" << std::endl
              << "    " << argv[ 0 ] << " [CONFIG_FILE.py]" << std::endl;
    returnStatus_ += 0x1;
    return returnStatus_;
  }
  if ( ! edm::readPSetsFrom( argv[ 1 ] )->existsAs< edm::ParameterSet >( "process" ) ) {
    std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
              << "   cms.PSet 'process' missing in " << argv[ 1 ] << std::endl;
    returnStatus_ += 0x2;
    return returnStatus_;
  }

  // Get the configurations
  const edm::ParameterSet & process_( edm::readPSetsFrom( argv[ 1 ] )->getParameter< edm::ParameterSet >( "process" ) );
  const bool verbose_( process_.getParameter< bool >( "verbose" ) );
  // Configuration for fitting new resolution functions
  const edm::ParameterSet & plotter_( process_.getParameter< edm::ParameterSet >( "plotter" ) );
  const std::string outFile_( plotter_.getParameter< std::string >( "outputFile" ) );
  const std::vector< std::string > inFiles_( plotter_.getParameter< std::vector< std::string > >( "inputFiles" ) );
  const std::vector< std::string > objCats_( plotter_.getParameter< std::vector< std::string > >( "objectCategories" ) );                                          // input file with RECO-GEN distributions
  const std::string evtSel_( plotter_.getParameter< std::string >( "selection" ) );

  // Set constants
  const std::string dirClassName_( "TDirectoryFile" );
  const std::string funcClassName_( "TF1" );

  std::vector< TFile* > files_;

  std::vector< std::vector< double > > etaBins_;
  std::vector< std::vector< double > > ptBins_;

  // Open input files

  bool first( true );
  for ( unsigned uFile = 0; uFile < inFiles_.size(); ++uFile ) {
    const std::string inFile( inFiles_.at( uFile ) );
    TFile * file( TFile::Open( inFile.c_str() ) );
    if ( ! file ) {
      std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                << "   input file '" << inFiles_.at( uFile ) << "' missing; trying next file" << std::endl;
      returnStatus_ += 0x10;
      continue;
    }
    files_.push_back( file );

    // Get binning (onyl once; has to be identical for all input files)
    if ( first ) {
      if ( verbose_ )
        std::cout << std::endl
                  << argv[ 0 ] << " --> INFO:" << std::endl
                  << "   extract binning from input file '" << inFiles_.at( uFile ) << "'" << std::endl;
      TDirectory * dirSel = ( TDirectory* )( file->Get( evtSel_.c_str() ) );

      for ( unsigned uCat = 0; uCat < objCats_.size(); ++uCat ) {
        const std::string objCat( objCats_.at( uCat ) );
        TDirectory * dirCat( ( TDirectory* )( dirSel->Get( objCat.c_str() ) ) );
        if ( ! dirCat ) {
          std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                    << "   object category '" << objCat << "' does not exist in input file '" << inFile << "'" << std::endl;
          continue;
        }

        // Eta binning
        std::vector< double > etaBins;
        TH1D * histEtaBins( ( TH1D* )( dirCat->Get( std::string( objCat + "_binsEta" ).c_str() ) ) );
        for ( int iEta = 1; iEta <= histEtaBins->GetNbinsX(); ++iEta ) {
          etaBins.push_back( histEtaBins->GetBinLowEdge( iEta ) );
        }
        etaBins.push_back( histEtaBins->GetBinLowEdge( histEtaBins->GetNbinsX() ) + histEtaBins->GetBinWidth( histEtaBins->GetNbinsX() ) );
        etaBins_.push_back( etaBins );

        // Pt binning
        std::vector< double > ptBins;
        TH1D * histPtBins( ( TH1D* )( dirCat->Get( std::string( objCat + "_binsPt" ).c_str() ) ) );
        for ( int iPt = 1; iPt <= histPtBins->GetNbinsX(); ++iPt ) {
          ptBins.push_back( histPtBins->GetBinLowEdge( iPt ) );
        }
        ptBins.push_back( histPtBins->GetBinLowEdge( histPtBins->GetNbinsX() ) + histPtBins->GetBinWidth( histPtBins->GetNbinsX() ) );
        ptBins_.push_back( ptBins );
      }

      first = false;
    }
  }
  if ( files_.empty() ) {
      std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                << "   no input files found" << std::endl;
      returnStatus_ += 0x20;
      return returnStatus_;
  }

  TFile * refFile( files_.at( 0 ) );
  TDirectory * dirSel = ( TDirectory* )( refFile->Get( evtSel_.c_str() ) );
  if ( ! dirSel ) {
    std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
              << "   selection '" << evtSel_ << "' does not exist in reference file '" << refFile->GetName() << "'" << std::endl;
    returnStatus_ += 0x20;
    return returnStatus_;
  }

  // Loop over configured object categories
  for ( unsigned uCat = 0; uCat < objCats_.size(); ++uCat ) {
    const std::string objCat( objCats_.at( uCat ) );
    TDirectory * dirCat( ( TDirectory* )( dirSel->Get( objCat.c_str() ) ) );
    if ( ! dirCat ) {
      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                << "   object category '" << objCat << "' does not exist in reference file '" << refFile->GetName() << "'" << std::endl;
      continue;
    }

    // Loop over kinematic properties
    TList * listCat( dirCat->GetListOfKeys() );
    TIter nextInListCat( listCat );
    while ( TKey * keyProp = ( TKey* )nextInListCat() ) {
      if ( std::string( keyProp->GetClassName() ) != dirClassName_ ) continue;
      const std::string kinProp( keyProp->GetName() );
      TDirectory * dirProp( ( TDirectory* )( dirCat->Get( kinProp.c_str() ) ) );

      // Loop over fit versions
      TList * listProp( dirProp->GetListOfKeys() );
      TIter nextInListProp( listProp );
      while ( TKey * keyFit = ( TKey* )nextInListProp() ) {
        if ( std::string( keyFit->GetClassName() ) != dirClassName_ ) continue;
        const std::string subFit( keyFit->GetName() );
        TDirectory * dirFit = ( TDirectory* )( dirProp->Get( subFit.c_str() ) );

        // Inversion flag from directory name
        const bool inverse( subFit.substr( subFit.size() - 3 ) == "Inv" );

        // Loop over eta bins
        TList * listFit( dirFit->GetListOfKeys() );
        TIter nextInListFit( listFit );
        while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
          if ( std::string( keyEta->GetClassName() ) != dirClassName_ ) continue;
          const std::string binEta( keyEta->GetName() );
          const unsigned uEta( std::atoi( binEta.substr( 3 ).data() ) );
          const std::string nameDir( evtSel_ + "/" + objCat + "/" + kinProp + "/" + subFit + "/" + binEta );
          const std::string name( objCat + "_" + kinProp + "_" + subFit + "_" + binEta );

          dirFit->cd( binEta.c_str() );
          if ( verbose_ ) gDirectory->pwd();
          const std::string nameSigma( name + "_Sigma" );
          TH1D * histSigma( ( TH1D* )( gDirectory->Get( nameSigma.c_str() ) ) );
          if ( histSigma != 0 ) {
            const std::string nameFitSigma( "fit_" + nameSigma );
            TF1 * fitSigma( histSigma->GetFunction( nameFitSigma.c_str() ) );

            const std::string nameCanvSigma( "canv_" + nameSigma );
            TCanvas * canvSigma( new TCanvas( nameCanvSigma.c_str(), "", 768, 512 ) );
            if ( fitSigma != 0 ) {
              fitSigma->DrawCopy();
            }

            for ( unsigned uFile = 1; uFile < files_.size(); ++uFile ) { // skip 1st file (we have it already)
              files_.at( uFile )->cd( nameDir.c_str() );
              if ( verbose_ ) gDirectory->pwd();
              canvSigma->cd();
              TH1D * histSigmaFile( ( TH1D* )( gDirectory->Get( nameSigma.c_str() ) ) );
              if ( histSigmaFile != 0 ) {
                TF1 * fitSigmaFile( histSigmaFile->GetFunction( nameFitSigma.c_str() ) );
                fitSigma->DrawCopy( "Same" );
              }
            }

            canvSigma->SaveAs( ".png" );

            delete canvSigma;

          }

        }

      }

    }

  }

  // Close input files
  for ( unsigned uFile = 0; uFile < files_.size(); ++uFile ) files_.at( uFile )->Close();

  return returnStatus_;

}