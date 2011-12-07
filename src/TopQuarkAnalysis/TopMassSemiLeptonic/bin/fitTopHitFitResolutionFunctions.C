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


int main(  int argc, char * argv[] )
{

  // Load libraries
  gSystem->Load( "libFWCoreFWLite" );
  AutoLibraryLoader::enable();

  // Check configuration file
  if ( argc < 2 ) {
    std::cout << argv[ 0 ] << " --> Usage:" << std::endl
              << "    " << argv[ 0 ] << " [CONFIG_FILE.py]" << std::endl;
    return 0;
  }
  if ( ! edm::readPSetsFrom( argv[ 1 ] )->existsAs< edm::ParameterSet >( "process" ) ) {
    std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
              << "   cms.PSet 'process' missing in " << argv[ 1 ] << std::endl;
    return 1;
  }

  // Get the configurations
  const edm::ParameterSet & process_( edm::readPSetsFrom( argv[ 1 ] )->getParameter< edm::ParameterSet >( "process" ) );
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

  // Loop over objects, quantities and bins

  std::vector< std::vector< double > > etaBins_;
  std::vector< std::vector< double > > ptBins_;

  for ( unsigned iSel = 0; iSel < evtSels_.size(); ++iSel ) {
    const std::string evtSel( evtSels_.at( iSel ) );

    inFile->cd(); inFile->Cd( evtSel.c_str() );
    gDirectory->pwd();
    TDirectory * curSel( gDirectory );

    for ( unsigned iCat = 0; iCat < objCats_.size(); ++iCat ) {
      const std::string objCat( objCats_.at( iCat ) );

      curSel->cd(); curSel->Cd( objCat.c_str() );
      gDirectory->pwd();
      TDirectory * curCat( gDirectory );

      // Eta binning
      std::vector< double > etaBins;
      TH1D * hist_EtaBins( ( TH1D* )( gDirectory->Get( std::string( objCat + "_binsEta" ).c_str() ) ) );
      for ( int iBin = 1; iBin <= hist_EtaBins->GetNbinsX(); ++iBin ) {
        etaBins.push_back( hist_EtaBins->GetBinLowEdge( iBin ) );
      }
      etaBins.push_back( hist_EtaBins->GetBinLowEdge( hist_EtaBins->GetNbinsX() ) + hist_EtaBins->GetBinWidth( hist_EtaBins->GetNbinsX() ) );
      if ( iSel == 0 ) etaBins_.push_back( etaBins );
      // Pt binning
      std::vector< double > ptBins;
      TH1D * hist_PtBins( ( TH1D* )( gDirectory->Get( std::string( objCat + "_binsPt" ).c_str() ) ) );
      for ( int iBin = 1; iBin <= hist_PtBins->GetNbinsX(); ++iBin ) {
        ptBins.push_back( hist_PtBins->GetBinLowEdge( iBin ) );
      }
      ptBins.push_back( hist_PtBins->GetBinLowEdge( hist_PtBins->GetNbinsX() ) + hist_PtBins->GetBinWidth( hist_PtBins->GetNbinsX() ) );
      if ( iSel == 0 ) ptBins_.push_back( ptBins );

      for ( unsigned iProp = 0; iProp < kinProps_.size(); ++iProp ) {
        const std::string kinProp( kinProps_.at( iProp ) );

        curCat->cd(); curCat->Cd( kinProp.c_str() );
        gDirectory->pwd();
        TDirectory * curProp( gDirectory );

        for ( unsigned iFit = 0; iFit < subFits_.size(); ++iFit ) {
          const std::string subFit( subFits_.at( iFit ) );
          const bool inverse( subFit.substr( subFit.size() - 3 ) == "Inv" );

          curProp->cd(); curProp->Cd( subFit.c_str() );
          gDirectory->pwd();
          TDirectory * curFit( gDirectory );

          for ( unsigned iBin = 0; iBin < etaBins.size() - 1; ++iBin ) {
            const std::string bin( my::helpers::to_string( iBin ) );

            const std::string name( objCat + "_" + kinProp + "_" + subFit + "_" + bin );
            TH2D * hist2D( ( TH2D* )( gDirectory->Get( name.c_str() ) ) );
            hist2D->FitSlicesY( 0, 1, hist2D->GetNbinsX(), 1 );

            TH1D * hist1D( ( TH1D* )( gDirectory->Get( std::string( name + "_2" ).c_str() ) ) ); // sigmas of the slice fits
            const std::string nameFunc( "fit_" + name );
            const std::string formula( inverse ? resFuncInv_ : resFunc_ );
            TF1 * func( new TF1( nameFunc.c_str(), formula.c_str() ) );
            func->SetRange( hist1D->GetXaxis()->GetXmin(), hist1D->GetXaxis()->GetXmax() );
            hist1D->Fit( func, "QR" );

          }

        }

      }

    }

  }

  // Close input file
  inFile->Write();
  inFile->Close();

  // Get existing resolution functions

  if ( useExisting_ ) {

    // Configuration for existing resolution functions
    const edm::ParameterSet & exist_( process_.getParameter< edm::ParameterSet >( "existing" ) );
    const std::string outFile_( exist_.getParameter< std::string >( "outputFile" ) );
    const std::vector< std::string > resFiles_( exist_.getParameter< std::vector< std::string > >( "resolutionFiles" ) ); // "file-in-path" location of input resolution files
    assert( objCats_.size() == resFiles_.size() );

    // Open output file
    TFile * outFile( TFile::Open( outFile_.c_str(), "RECREATE" ) );

    // Loop over objects and quantities

    for ( unsigned iCat = 0; iCat < objCats_.size(); ++iCat ) {
      const std::string objCat( objCats_.at( iCat ) );

      const edm::FileInPath resFile( resFiles_.at( iCat ) );
      const hitfit::EtaDepResolution res( resFile.fullPath() );
      const std::vector< hitfit::EtaDepResElement > resElems( res.GetEtaDepResElement() );

      for ( unsigned iResElem = 0; iResElem < resElems.size(); ++iResElem ) {

        const hitfit::Vector_Resolution vecRes( resElems.at( iResElem ).GetResolution() );

        for ( unsigned iProp = 0; iProp < kinProps_.size(); ++iProp ) {
          const std::string kinProp( kinProps_.at( iProp ) );

          hitfit::Resolution res;
          if ( kinProp == "Pt" ) {
            res = hitfit::Resolution( vecRes.p_res() );
          }
          else if ( kinProp == "Eta" ) {
            res = hitfit::Resolution( vecRes.eta_res() );
          }
          else if ( kinProp == "Phi" ) {
            res = hitfit::Resolution( vecRes.phi_res() );
          }

          std::string name( "fitExist_" + objCat + "_" + kinProp + "_" );
          if ( res.inverse() ) name.append( "Inv_" );
          name.append( my::helpers::to_string( iResElem ) );
          TF1 * func = new TF1( name.c_str(), res.inverse() ? resFuncInv_.c_str() : resFunc_.c_str(), 0., ptBins_.at( iCat ).back() );
          func->SetParameters( res.C(), res.R(), res.N() );
          func->Write();

        }
      }

    }

    // Close output file
    outFile->Write();
    outFile->Close();

  }

  return 0;

}
