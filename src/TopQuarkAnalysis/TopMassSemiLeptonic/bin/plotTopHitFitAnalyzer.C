#include <string>
#include <vector>
#include <iostream>

#include "boost/lexical_cast.hpp"

#include <TROOT.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TFile.h>
#include <TDirectory.h>
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
  gStyle->SetPalette(1,0);
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetPadColor(kWhite);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetPadTopMargin(.075);
  gStyle->SetPadRightMargin(.075);
  gStyle->SetPadBottomMargin(.15);
  gStyle->SetPadLeftMargin(.15);
  gStyle->SetTitleSize(.06,"XYZ");
  gStyle->SetTitleFillColor(kWhite);
  gStyle->SetTitleBorderSize(1);
  gStyle->SetStatColor(kWhite);
  gStyle->SetStatBorderSize(1);
  gStyle->SetOptStat( 0 );
  gStyle->SetOptFit( 0 );
  gStyle->SetMarkerStyle(8);

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
  const std::vector< std::string > evtSels_( plotter_.getParameter< std::vector< std::string > >( "selections" ) );

  // Set constants
  // kinematic quantities
  std::vector< std::string > kinProps_;
  kinProps_.push_back( "Pt" );
  kinProps_.push_back( "Eta" );
  kinProps_.push_back( "Phi" );
  //
  std::vector< std::string > subFits_;
  subFits_.push_back( "Reco" );
//   subFits_.push_back( "Gen" );
  subFits_.push_back( "RecoInv" );
//   subFits_.push_back( "GenInv" );
//   subFits_.push_back( "RecoSymm" );
//   subFits_.push_back( "GenSymm" );
//   subFits_.push_back( "RecoSymmInv" );
//   subFits_.push_back( "GenSymmInv" );

  // Loop over input files

  std::vector< std::vector< double > > etaBins_;
  std::vector< std::vector< double > > ptBins_;

  bool first_( true );

  for ( unsigned iFile = 0; iFile < inFiles_.size(); ++iFile ) {
    const std::string inFile( inFiles_.at( iFile ) );

    // Open input file
    TFile * file( TFile::Open( inFile.c_str() ) );
    if ( ! file ) {
      std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                << "   input file '" << inFiles_.at( iFile ) << "' missing; trying next file" << std::endl;
      returnStatus_ += 0x10;
      continue;
    }

    // Get binning (onyl once; has to be identical for all input files)

    if ( first_ ) {

      if ( verbose_ )
        std::cout << std::endl
                  << argv[ 0 ] << " --> INFO:" << std::endl
                  << "   extract binning from input file '" << inFiles_.at( iFile ) << "'" << std::endl;

      file->cd();
      file->Cd( evtSels_.at( 0 ).c_str() );
      if ( verbose_ ) gDirectory->pwd();
      TDirectory * curSel( gDirectory );

      for ( unsigned iCat = 0; iCat < objCats_.size(); ++iCat ) {
        const std::string objCat( objCats_.at( iCat ) );

        curSel->cd();
        curSel->Cd( objCat.c_str() );
        if ( verbose_ ) gDirectory->pwd();

        // Eta binning
        std::vector< double > etaBins;
        TH1D * hist_EtaBins( ( TH1D* )( gDirectory->Get( std::string( objCat + "_binsEta" ).c_str() ) ) );
        for ( int iEta = 1; iEta <= hist_EtaBins->GetNbinsX(); ++iEta ) {
          etaBins.push_back( hist_EtaBins->GetBinLowEdge( iEta ) );
        }
        etaBins.push_back( hist_EtaBins->GetBinLowEdge( hist_EtaBins->GetNbinsX() ) + hist_EtaBins->GetBinWidth( hist_EtaBins->GetNbinsX() ) );
        etaBins_.push_back( etaBins );
        // Pt binning
        std::vector< double > ptBins;
        TH1D * hist_PtBins( ( TH1D* )( gDirectory->Get( std::string( objCat + "_binsPt" ).c_str() ) ) );
        for ( int iPt = 1; iPt <= hist_PtBins->GetNbinsX(); ++iPt ) {
          ptBins.push_back( hist_PtBins->GetBinLowEdge( iPt ) );
        }
        ptBins.push_back( hist_PtBins->GetBinLowEdge( hist_PtBins->GetNbinsX() ) + hist_PtBins->GetBinWidth( hist_PtBins->GetNbinsX() ) );
        ptBins_.push_back( ptBins );

      }

    }

    // Loop over objects, quantities and bins

    if ( verbose_ )
      std::cout << std::endl
                << argv[ 0 ] << " --> INFO:" << std::endl
                << "   reading resolution functions from input file '" << inFiles_.at( iFile ) << "'" << std::endl;

    for ( unsigned iSel = 0; iSel < evtSels_.size(); ++iSel ) {
      const std::string evtSel( evtSels_.at( iSel ) );

      file->cd();
      file->Cd( evtSel.c_str() );
      if ( verbose_ ) gDirectory->pwd();
      TDirectory * curSel( gDirectory );

      for ( unsigned iCat = 0; iCat < objCats_.size(); ++iCat ) {
        const std::string objCat( objCats_.at( iCat ) );

        curSel->cd();
        curSel->Cd( objCat.c_str() );
        if ( verbose_ ) gDirectory->pwd();
        TDirectory * curCat( gDirectory );

        for ( unsigned iProp = 0; iProp < kinProps_.size(); ++iProp ) {
          const std::string kinProp( kinProps_.at( iProp ) );

          curCat->cd();
          curCat->Cd( kinProp.c_str() );
          if ( verbose_ ) gDirectory->pwd();
          TDirectory * curProp( gDirectory );

          for ( unsigned iFit = 0; iFit < subFits_.size(); ++iFit ) {
            const std::string subFit( subFits_.at( iFit ) );

            curProp->cd();
            curProp->Cd( subFit.c_str() );
            if ( verbose_ ) gDirectory->pwd();
            TDirectory * curFit( gDirectory );

            for ( unsigned iEta = 0; iEta < etaBins_.at( iCat ).size() - 1; ++iEta ) {
              const std::string binEta( "Eta" + boost::lexical_cast< std::string >( iEta ) );

              curFit->cd();
              curFit->Cd( binEta.c_str() );
              if ( verbose_ ) gDirectory->pwd();
              TDirectory * curEta( gDirectory );

              const std::string name( objCat + "_" + kinProp + "_" + subFit + "_" + binEta );
              const std::string nameSigma( name + "_Sigma" );
              TH1D * histSigma( ( TH1D* )( curEta->Get( nameSigma.c_str() ) ) );
              const std::string nameFuncSigma( "fit_" + nameSigma );
              TF1 * funcSigma( histSigma->GetFunction( nameFuncSigma.c_str() ) );

              TCanvas * canvasSigma;
              const std::string nameSigmaCanvas( "canvas_" + nameSigma );

              if ( first_ ) {
                canvasSigma = new TCanvas( nameSigmaCanvas.c_str(), "", 768, 512 );
                funcSigma->DrawCopy();
              }
              else {
                canvasSigma = ( TCanvas* )( gROOT->FindObject( nameSigmaCanvas.c_str() ) );
                canvasSigma->cd();
                funcSigma->DrawCopy( "SAME" );
              }

//               if ( iFile == inFiles_.size() - 1 ) {
//                 canvasSigma->Update();
                canvasSigma->SaveAs( ".png" );
//               }

            }

          }

        }

      }

    }

    // Close input file
    file->Close();

    first_ = false;

  }

  if ( verbose_ )
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "   return status " << returnStatus_ << std::endl;
  return returnStatus_;

  return returnStatus_;

}
