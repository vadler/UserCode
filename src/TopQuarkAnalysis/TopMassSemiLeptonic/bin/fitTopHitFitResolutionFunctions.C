#include <cstdlib>
#include <cmath>
#include <cassert>
#include <string>
#include <vector>
#include <iostream>

#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>

#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include "TopQuarkAnalysis/TopHitFit/interface/EtaDepResolution.h"

#include "TopQuarkAnalysis/TopMassSemiLeptonic/interface/Helpers.h"


int main( int argc, char * argv[] )
{

  int returnStatus_( 0 );

  // Load libraries
  gSystem->Load( "libFWCoreFWLite" );
  AutoLibraryLoader::enable();

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
  const std::vector< std::string > objCats_( process_.getParameter< std::vector< std::string > >( "objectCategories" ) );   // object categories
  const std::string resFunc_( process_.getParameter< std::string >( "resolutionFunction" ) );
  const std::string resFuncInv_( process_.getParameter< std::string >( "resolutionFunctionInverse" ) );
  // Configuration for fitting new resolution functions
  const edm::ParameterSet & fitter_( process_.getParameter< edm::ParameterSet >( "fitter" ) );
  const std::string inFile_( fitter_.getParameter< std::string >( "inputFile" ) );                                          // input file with RECO-GEN distributions
  const std::vector< std::string > evtSels_( fitter_.getParameter< std::vector< std::string > >( "selections" ) );
  // Configuration for existing resolution functions
  const bool useExisting_( process_.getParameter< bool >( "useExisting" ) );

  // Set constants
  // kinematic quantities
  std::vector< std::string > kinProps_;
  kinProps_.push_back( "Pt" );
  kinProps_.push_back( "Eta" );
  kinProps_.push_back( "Phi" );
  //
  std::vector< std::string > subFits_;
  subFits_.push_back( "Reco" );
  subFits_.push_back( "Gen" );
  subFits_.push_back( "RecoInv" );
  subFits_.push_back( "GenInv" );
//   subFits_.push_back( "RecoSymm" );
//   subFits_.push_back( "GenSymm" );
//   subFits_.push_back( "RecoSymmInv" );
//   subFits_.push_back( "GenSymmInv" );

  // Open input file
  TFile * inFile( TFile::Open( inFile_.c_str(), "UPDATE" ) );
  if ( ! inFile ) {
    std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
              << "   input file '" << inFile_ << "' missing" << std::endl;
    returnStatus_ += 0x10;
    return returnStatus_;
  }

  // Get binning (has to be identical for selections)

  if ( verbose_ )
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "   extract binning from input file '" << inFile_ << "'" << std::endl;

  std::vector< std::vector< double > > etaBins_;
  std::vector< std::vector< double > > ptBins_;

  inFile->cd();
  inFile->Cd( evtSels_.at( 0 ).c_str() );
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

  // Get existing resolution functions

  if ( useExisting_ ) {

    // Configuration for existing resolution functions
    const edm::ParameterSet & exist_( process_.getParameter< edm::ParameterSet >( "existing" ) );
    const std::string resolutionFile_( exist_.getParameter< std::string >( "resolutionFile" ) );

    if ( verbose_ )
      std::cout << std::endl
                << argv[ 0 ] << " --> INFO:" << std::endl
                << "   accessing existing resolution functions from resolution file '" << resolutionFile_ << "'" << std::endl;

    // Open output file
    TFile * resolutionFile( TFile::Open( resolutionFile_.c_str() ) );
    if ( resolutionFile ) {

      // Loop over objects and quantities

      for ( unsigned iCat = 0; iCat < objCats_.size(); ++iCat ) {
        const std::string objCat( objCats_.at( iCat ) );

        resolutionFile->cd();
        gDirectory->Cd( objCat.c_str() );
        TDirectory * curCat( gDirectory );
        if ( verbose_ ) gDirectory->pwd();

        for ( unsigned iProp = 0; iProp < kinProps_.size(); ++iProp ) {
          std::string kinProp( kinProps_.at( iProp ) );

          curCat->cd();
          TDirectory * curProp( ( TDirectory* )( gDirectory->Get( kinProp.c_str() ) ) );
          if ( ! curProp ) {
            kinProp.append( std::string( "Inv" ) );
            curProp = ( TDirectory* )( gDirectory->Get( kinProp.c_str() ) );
            if ( ! curProp ) {
              std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                        << "   resolution file '" << resolutionFile_ << "' does not have directory for" << std::endl
                        << "   object category " << objCat << std::endl
                        << "   quantity        " << kinProps_.at( iProp ) << std::endl;
              returnStatus_ += 0x200;
            }
          }

          for ( unsigned iEta = 0; iEta < etaBins_.at( iCat ).size() - 1; ++iEta ) {
            const std::string binEta( "Eta" + my::helpers::to_string( iEta ) );

            curProp->cd();
            gDirectory->Cd( std::string( binEta ).c_str() );
            if ( verbose_ ) gDirectory->pwd();

            const std::string name( "fitExist_" + objCat + "_" + kinProp + "_" + binEta );
            TF1 * func( ( TF1* )( gDirectory->Get( name.c_str() ) ) );
            if ( ! func ) {
              std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                        << "   resolution function '" << name << "' not found" << std::endl;
              returnStatus_ += 0x300;
            }

          }
        }

      }

    }
    else {
      std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                << "   resolution file '" << resolutionFile_ << "' missing" << std::endl;
      returnStatus_ += 0x100;
    }

    // Close output file
    resolutionFile->Close();

  }

  // Loop over objects, quantities and bins

  if ( verbose_ )
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "   fitting resolution functions from input file '" << inFile_ << "'" << std::endl;

  for ( unsigned iSel = 0; iSel < evtSels_.size(); ++iSel ) {
    const std::string evtSel( evtSels_.at( iSel ) );

    inFile->cd();
    inFile->Cd( evtSel.c_str() );
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
          const bool inverse( subFit.substr( subFit.size() - 3 ) == "Inv" );

          curProp->cd();
          curProp->Cd( subFit.c_str() );
          if ( verbose_ ) gDirectory->pwd();
          TDirectory * curFit( gDirectory );

          for ( unsigned iEta = 0; iEta < etaBins_.at( iCat ).size() - 1; ++iEta ) {
            const std::string binEta( "Eta" + my::helpers::to_string( iEta ) );

            curFit->cd();
            curFit->Cd( binEta.c_str() );
            if ( verbose_ ) gDirectory->pwd();

            const std::string name( objCat + "_" + kinProp + "_" + subFit + "_" + binEta );
            TH2D * hist2D( ( TH2D* )( gDirectory->Get( name.c_str() ) ) );
            hist2D->FitSlicesY( 0, 1, hist2D->GetNbinsX(), 1 );

            TH1D * histSigma( ( TH1D* )( gDirectory->Get( std::string( name + "_2" ).c_str() ) ) ); // sigmas of the slice fits
            const std::string nameFunc( "fit_" + name );
            const std::string formula( inverse ? resFuncInv_ : resFunc_ );
            TF1 * func( new TF1( nameFunc.c_str(), formula.c_str() ) );
            func->SetRange( histSigma->GetXaxis()->GetXmin(), histSigma->GetXaxis()->GetXmax() );
            histSigma->Fit( func, "QR" );

            const std::string nameSigma( name + "_Sigma" );
            TH1D * histSigmaPt( new TH1D( *( ( TH1D* )( histSigma->Clone( nameSigma.c_str() ) ) ) ) );
            for ( unsigned iPt = 1; iPt < ptBins_.at( iCat ).size() - 1; ++iPt ) {
              const std::string binPt( my::helpers::to_string( iPt ) );

              const std::string namePt( name + "_Pt_" + binPt );
              const std::string titlePt( std::string( hist2D->GetTitle() ) + ", " + my::helpers::to_string( hist2D->GetXaxis()->GetBinLowEdge( iPt ) ) + " GeV #leq p_{t} < " + my::helpers::to_string( hist2D->GetXaxis()->GetBinUpEdge( iPt ) ) + " GeV" );
              const Int_t nBins( hist2D->GetNbinsY() );
              TH1D * hist1D( new TH1D( namePt.c_str(), titlePt.c_str(), nBins, hist2D->GetYaxis()->GetXmin(), hist2D->GetYaxis()->GetXmax() ) );
              hist1D->SetXTitle( hist2D->GetYaxis()->GetTitle() );
              hist1D->SetYTitle( hist2D->GetZaxis()->GetTitle() );
              for ( Int_t iBin = 1; iBin <= nBins; ++iBin ) {
                hist1D->SetBinContent( iBin, hist2D->GetBinContent( iPt, iBin ) );
              }

              const std::string nameFuncGauss( "gauss_" + namePt );
              TF1 * funcGauss( new TF1( nameFuncGauss.c_str(), "gaus", hist1D->GetXaxis()->GetXmin(), hist1D->GetXaxis()->GetXmax() ) );
              Int_t fitStatus( hist1D->Fit( nameFuncGauss.c_str(), "QRS" ) );
              TF1 * funcFit( hist1D->GetFunction( nameFuncGauss.c_str() ) );
              if ( fitStatus == 0 ) {
                histSigmaPt->SetBinContent( iPt, funcFit->GetParameter( 2 ) );
                histSigmaPt->SetBinError( iPt, funcFit->GetParError( 2 ) );
              }
            }
            const std::string nameFuncSigma( "fit_" + nameSigma );
            const std::string formulaSigma( inverse ? resFuncInv_ : resFunc_ );
            TF1 * funcSigma( new TF1( nameFuncSigma.c_str(), formula.c_str() ) );
            funcSigma->SetRange( histSigmaPt->GetXaxis()->GetXmin(), histSigmaPt->GetXaxis()->GetXmax() );
            histSigmaPt->Fit( funcSigma, "QR" );

          }

        }

      }

    }

  }

  // Close input file
  inFile->Write();
  inFile->Close();

  if ( verbose_ )
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "   return status " << returnStatus_ << std::endl;
  return returnStatus_;

}
