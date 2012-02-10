#include <cstdlib>
#include <cmath>
#include <cassert>
#include <string>
#include <vector>
#include <iostream>

#include "boost/lexical_cast.hpp"

#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TKey.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>

#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include "TopQuarkAnalysis/TopHitFit/interface/EtaDepResolution.h"


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
  const unsigned verbose_( process_.getParameter< unsigned >( "verbose" ) );
  const std::vector< std::string > objCats_( process_.getParameter< std::vector< std::string > >( "objectCategories" ) );   // object categories
  const bool refGen_(  process_.getParameter< bool >( "refGen" ));
  const bool useSymm_(  process_.getParameter< bool >( "useSymm" ));
  // Configuration for fitting new resolution functions
  const edm::ParameterSet & fitter_( process_.getParameter< edm::ParameterSet >( "fitter" ) );
  const std::string inFile_( fitter_.getParameter< std::string >( "inputFile" ) );                                          // input file with RECO-GEN distributions
  const std::string evtSel_( fitter_.getParameter< std::string >( "selection" ) );
  const std::string resFunc_( fitter_.getParameter< std::string >( "resolutionFunction" ) );
  const std::string resFuncInv_( fitter_.getParameter< std::string >( "resolutionFunctionInverse" ) );
  // Configuration for existing resolution functions
  const bool useExisting_( process_.getParameter< bool >( "useExisting" ) );

    if ( verbose_ > 0 )
      std::cout << std::endl
                << argv[ 0 ] << " --> INFO:" << std::endl
                << "   called for object categories ";
      for ( unsigned iCat = 0; iCat < objCats_.size() - 1; ++iCat )
        std::cout << "'" << objCats_.at( iCat ) << "', ";
      std::cout << "'" << objCats_.back() << "'" << std::endl;

  // Set constants
  const std::string dirClassName( "TDirectoryFile" );


  // Get existing resolution functions

  if ( useExisting_ ) {

    // Configuration for existing resolution functions
    const edm::ParameterSet & exist_( process_.getParameter< edm::ParameterSet >( "existing" ) );
    const std::string resolutionFile_( exist_.getParameter< std::string >( "resolutionFile" ) );

    if ( verbose_ > 0 )
      std::cout << std::endl
                << argv[ 0 ] << " --> INFO:" << std::endl
                << "   accessing existing resolution functions from resolution file '" << resolutionFile_ << "'" << std::endl;

    // Open output file
    TFile * resolutionFile( TFile::Open( resolutionFile_.c_str() ) );
    if ( resolutionFile ) {
      if ( verbose_ > 2 ) gDirectory->pwd();

      // Loop over objects and quantities

      for ( unsigned iCat = 0; iCat < objCats_.size(); ++iCat ) {
        const std::string objCat( objCats_.at( iCat ) );
        TDirectory * dirCat = ( TDirectory* )( resolutionFile->Get( objCat.c_str() ) );
        if ( ! dirCat ) {
          std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                    << "   object category '" << objCat << "' does not exist in resolution file" << std::endl;
          continue;
        }

        if ( objCat == "MET" ) {

          TList * listCat( dirCat->GetListOfKeys() );
          if ( verbose_ > 3 ) listCat->Print();
          TIter nextInListCat( listCat );
          TKey * keyProp( ( TKey* )nextInListCat() );
          const std::string kinProp( keyProp->GetName() );
          TDirectory * dirProp( ( TDirectory* )( dirCat->Get( kinProp.c_str() ) ) );
          if ( ! dirProp ) {
            std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                      << "   resolution file '" << resolutionFile_ << "' does not have directory for" << std::endl
                      << "   object category " << objCat << std::endl
                      << "   quantity        " << kinProp << std::endl;
            returnStatus_ += 0x200;
            continue;
          }

          const std::string name( "fitExist_" + objCat + "_" + kinProp );
          TF1 * func( ( TF1* )( dirProp->Get( name.c_str() ) ) );
          if ( ! func ) {
            std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                      << "   resolution function '" << name << "' not found" << std::endl;
            returnStatus_ += 0x300;
            continue;
          }

        } // objCat == "MET"

        else {

          TList * listCat( dirCat->GetListOfKeys() );
          TIter nextInListCat( listCat );
          if ( verbose_ > 3 ) listCat->Print();
          while ( TKey * keyProp = ( TKey* )nextInListCat() ) {
            if ( std::string( keyProp->GetClassName() ) != dirClassName ) continue;
            const std::string kinProp( keyProp->GetName() );
            TDirectory * dirProp( ( TDirectory* )( dirCat->Get( kinProp.c_str() ) ) );
            if ( ! dirProp ) {
              std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                        << "   resolution file '" << resolutionFile_ << "' does not have directory for" << std::endl
                        << "   object category " << objCat << std::endl
                        << "   quantity        " << kinProp << std::endl;
              returnStatus_ += 0x200;
              continue;
            }

            TList * listProp( dirProp->GetListOfKeys() );
            TIter nextInListProp( listProp );
            if ( verbose_ > 3 ) listProp->Print();
            while ( TKey * keyEta = ( TKey* )nextInListProp() ) {
              if ( std::string( keyEta->GetClassName() ) != dirClassName ) continue;
              const std::string binEta( keyEta->GetName() );
              TDirectory * dirEta = ( TDirectory* )( dirProp->Get( binEta.c_str() ) );

              const std::string name( "fitExist_" + objCat + "_" + kinProp + "_" + binEta );
              TF1 * func( ( TF1* )( dirEta->Get( name.c_str() ) ) );
              if ( ! func ) {
                std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                          << "   resolution function '" << name << "' not found" << std::endl;
                returnStatus_ += 0x300;
                continue;
              }

            } // loop: nextInListProp()

          } // loop: nextInListCat()

        } // !( objCat == "MET" )

      } // loop: iCat < objCats_.size()

      // Close output file
      resolutionFile->Close();

    } // ( resolutionFile )

    else {
      std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                << "   resolution file '" << resolutionFile_ << "' missing" << std::endl;
      returnStatus_ += 0x100;
    } // !( resolutionFile )

  } // ( useExisting_ )


  // Fit the new resolution functions

  // Open input file
  TFile * inFile( TFile::Open( inFile_.c_str(), "UPDATE" ) );
  if ( ! inFile ) {
    std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
              << "   input file '" << inFile_ << "' missing" << std::endl;
    returnStatus_ += 0x10;
    return returnStatus_;
  }
  TDirectory * dirSel = ( TDirectory* )( inFile->Get( evtSel_.c_str() ) );

  // Loops through directory structure

  if ( verbose_ > 0 )
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "   fitting resolution functions from input file '" << inFile_ << "'" << std::endl;

  // Loop over configured object categories
  for ( unsigned iCat = 0; iCat < objCats_.size(); ++iCat ) {
    const std::string objCat( objCats_.at( iCat ) );
    TDirectory * dirCat( ( TDirectory* )( dirSel->Get( objCat.c_str() ) ) );
    if ( ! dirCat ) {
      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                << "   object category '" << objCat << "' does not exist in input file" << std::endl;
      continue;
    }

    // Get binning per object category

    // Eta binning
    std::vector< double > etaBins_;
    TH1D * hist_EtaBins( ( TH1D* )( dirCat->Get( std::string( objCat + "_binsEta" ).c_str() ) ) );
    for ( int iEta = 1; iEta <= hist_EtaBins->GetNbinsX(); ++iEta ) {
      etaBins_.push_back( hist_EtaBins->GetBinLowEdge( iEta ) );
    }
    etaBins_.push_back( hist_EtaBins->GetBinLowEdge( hist_EtaBins->GetNbinsX() ) + hist_EtaBins->GetBinWidth( hist_EtaBins->GetNbinsX() ) );

    // Pt binning
    std::vector< double > ptBins_;
    TH1D * hist_PtBins( ( TH1D* )( dirCat->Get( std::string( objCat + "_binsPt" ).c_str() ) ) );
    for ( int iPt = 1; iPt <= hist_PtBins->GetNbinsX(); ++iPt ) {
      ptBins_.push_back( hist_PtBins->GetBinLowEdge( iPt ) );
    }
    ptBins_.push_back( hist_PtBins->GetBinLowEdge( hist_PtBins->GetNbinsX() ) + hist_PtBins->GetBinWidth( hist_PtBins->GetNbinsX() ) );

    // Read n-tuple data
    std::vector< std::vector< Double_t > > momE_( etaBins_.size() - 1 );
    std::vector< std::vector< Double_t > > eta_( etaBins_.size() - 1 );
    std::vector< std::vector< Double_t > > phi_( etaBins_.size() - 1 );
    std::vector< std::vector< Double_t > > momEGen_( etaBins_.size() - 1 );
    std::vector< std::vector< Double_t > > etaGen_( etaBins_.size() - 1 );
    std::vector< std::vector< Double_t > > phiGen_( etaBins_.size() - 1 );
    std::cout << "DEBUG size: " << momE_.size() << std::endl;
    Double_t momE, eta, phi;
    Double_t momEGen, etaGen, phiGen;
    Int_t    binEta;
    TTree * data_( ( TTree* )( dirCat->Get( std::string( objCat + "_data" ).c_str() ) ) );
    data_->SetBranchAddress( "MomE"  , &momE );
    data_->SetBranchAddress( "Eta"   , &eta );
    data_->SetBranchAddress( "Phi"   , &phi );
    data_->SetBranchAddress( "MomEGen", &momEGen );
    data_->SetBranchAddress( "EtaGen" , &etaGen );
    data_->SetBranchAddress( "PhiGen" , &phiGen );
    if ( useSymm_ )
      if ( refGen_ ) data_->SetBranchAddress( "BinEtaSymmGen", &binEta );
      else           data_->SetBranchAddress( "BinEtaSymm", &binEta );
    else
      if ( refGen_ ) data_->SetBranchAddress( "BinEtaGen", &binEta );
      else           data_->SetBranchAddress( "BinEta", &binEta );
    Int_t n( ( Int_t )data_->GetEntries() );
    std::cout << "DEBUG objCat: " << objCat << std::endl; // DEBUG
    std::cout << "DEBUG      n: " << n      << std::endl; // DEBUG
    for ( Int_t i = 0; i < n; ++i ) {
      data_->GetEntry( i );
      assert( binEta <= ( Int_t )( etaBins_.size() - 1 ) ); // has to fit (and be consistent)
      momE_.at( binEta ).push_back( momE );
      eta_.at( binEta ).push_back( eta );
      phi_.at( binEta ).push_back( phi );
      momEGen_.at( binEta ).push_back( momEGen );
      etaGen_.at( binEta ).push_back( etaGen );
      phiGen_.at( binEta ).push_back( phiGen );
    }

    // Loop over kinematic properties
    TList * listCat( dirCat->GetListOfKeys() );
    TIter nextInListCat( listCat );
    if ( verbose_ > 3 ) listCat->Print();
    while ( TKey * keyProp = ( TKey* )nextInListCat() ) {
      if ( std::string( keyProp->GetClassName() ) != dirClassName ) continue;
      const std::string kinProp( keyProp->GetName() );
      TDirectory * dirProp( ( TDirectory* )( dirCat->Get( kinProp.c_str() ) ) );

      // Loop over fit versions
      TList * listProp( dirProp->GetListOfKeys() );
      TIter nextInListProp( listProp );
      if ( verbose_ > 3 ) listProp->Print();
      while ( TKey * keyFit = ( TKey* )nextInListProp() ) {
        if ( std::string( keyFit->GetClassName() ) != dirClassName ) continue;
        const std::string subFit( keyFit->GetName() );
        TDirectory * dirFit = ( TDirectory* )( dirProp->Get( subFit.c_str() ) );

        // Inversion flag from directory name
        const bool inverse( subFit.substr( subFit.size() - 3 ) == "Inv" );

        // Loop over eta bins
        TList * listFit( dirFit->GetListOfKeys() );
        if ( listFit->GetSize() != ( Int_t )( etaBins_.size() - 1 ) ) {
          std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                    << "   mismatch of eta binning for object category '" << objCat << "':" << std::endl
                    << "       bins in directory structure: " << listFit->GetSize()         << std::endl
                    << "       bins in binning histogram  : " << etaBins_.size() - 1         << std::endl;
        }
        TIter nextInListFit( listFit );
        if ( verbose_ > 3 ) listFit->Print();
        while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
          if ( std::string( keyEta->GetClassName() ) != dirClassName ) continue;
          const std::string binEta( keyEta->GetName() );
          const unsigned iEta( std::atoi( binEta.substr( 3 ).data() ) );
          dirFit->cd( binEta.c_str() );
          if ( verbose_ > 1 ) gDirectory->pwd();

          // Do the fits

          const std::string name( objCat + "_" + kinProp + "_" + subFit + "_" + binEta );

          // Direct fit in slices of the 2-dim histogram
          TH2D * hist2D( ( TH2D* )( gDirectory->Get( name.c_str() ) ) );
          hist2D->FitSlicesY( 0, 1, hist2D->GetNbinsX(), 1 );
          TH1D * histSigma( ( TH1D* )( gDirectory->Get( std::string( name + "_2" ).c_str() ) ) ); // sigmas of the slice fits
          const std::string nameFunc( "fit_" + name );
          const std::string formula( inverse ? resFuncInv_ : resFunc_ );
          TF1 * func( new TF1( nameFunc.c_str(), formula.c_str() ) );
          func->SetRange( histSigma->GetXaxis()->GetXmin(), histSigma->GetXaxis()->GetXmax() );
          histSigma->Fit( func, "QR" );

          // Extract individual 1-dim histograms per p_t-bin and fit
          const std::string nameSigma( name + "_Sigma" );
          TH1D * histSigmaPt( new TH1D( *( ( TH1D* )( histSigma->Clone( nameSigma.c_str() ) ) ) ) );
          histSigmaPt->Reset( "ICE" ); // emoty the contents to use it as "template" only
          for ( unsigned iPt = 0; iPt < ptBins_.size() - 1; ++iPt ) {
            const std::string binPt( boost::lexical_cast< std::string >( iPt ) );

            const std::string namePt( name + "_Pt_" + binPt );
            const std::string titlePt( std::string( hist2D->GetTitle() ) + ", " + boost::lexical_cast< std::string >( hist2D->GetXaxis()->GetBinLowEdge( iPt ) ) + " GeV #leq p_{t} < " + boost::lexical_cast< std::string >( hist2D->GetXaxis()->GetBinUpEdge( iPt ) ) + " GeV" );
            const Int_t nBins( hist2D->GetNbinsY() );
            TH1D * hist1D( new TH1D( namePt.c_str(), titlePt.c_str(), nBins, hist2D->GetYaxis()->GetXmin(), hist2D->GetYaxis()->GetXmax() ) );
            hist1D->SetXTitle( hist2D->GetYaxis()->GetTitle() );
            hist1D->SetYTitle( hist2D->GetZaxis()->GetTitle() );
            for ( Int_t iBin = 1; iBin <= nBins; ++iBin ) {
              hist1D->SetBinContent( iBin, hist2D->GetBinContent( iPt, iBin ) );
            }

            const std::string nameFuncGauss( "gauss_" + namePt );
            TF1 * funcGauss( new TF1( nameFuncGauss.c_str(), "gaus", hist1D->GetXaxis()->GetXmin(), hist1D->GetXaxis()->GetXmax() ) );
            Int_t fitStatus( hist1D->Fit( funcGauss, "QRS" ) );
            TF1 * funcFit( hist1D->GetFunction( nameFuncGauss.c_str() ) );
            if ( fitStatus == 0 ) {
              histSigmaPt->SetBinContent( iPt, funcFit->GetParameter( 2 ) );
              histSigmaPt->SetBinError( iPt, funcFit->GetParError( 2 ) );
            }
          } // loop: iPt < ptBins_.size() - 1
          const std::string nameFuncSigma( "fit_" + nameSigma );
          TF1 * funcSigma( new TF1( nameFuncSigma.c_str(), formula.c_str() ) );
          funcSigma->SetRange( histSigmaPt->GetXaxis()->GetXmin(), histSigmaPt->GetXaxis()->GetXmax() );
          histSigmaPt->Fit( funcSigma, "QR" );

          // Create new 1-dim histograms from n-tuple
          TH1D * histSigmaPtNTup( new TH1D( *( ( TH1D* )( histSigma->Clone( nameSigma.c_str() ) ) ) ) );
          histSigmaPtNTup->Reset( "ICE" ); // emoty the contents to use it as "template" only     std::vector< std::vector< Double_t > > momE_( etaBins_.size() - 1 );
          std::vector< std::vector< Double_t > > momEPt( ptBins_.size() - 1 );
          std::vector< std::vector< Double_t > > etaPt( ptBins_.size() - 1 );
          std::vector< std::vector< Double_t > > phiPt( ptBins_.size() - 1 );
          std::vector< std::vector< Double_t > > momEGenPt( ptBins_.size() - 1 );
          std::vector< std::vector< Double_t > > etaGenPt( ptBins_.size() - 1 );
          std::vector< std::vector< Double_t > > phiGenPt( ptBins_.size() - 1 );
          unsigned size( momE_.at( iEta ).size() );
          assert( size == eta_.at( iEta ).size() );
          assert( size == phi_.at( iEta ).size() );
          assert( size == momEGen_.at( iEta ).size() );
          assert( size == etaGen_.at( iEta ).size() );
          assert( size == phiGen_.at( iEta ).size() );
          for ( unsigned i = 0; i < size; ++i ) {
            for ( unsigned iPt = 0; iPt < ptBins_.size() - 1; ++iPt ) {
              if ( ptBins_.at( iPt ) <= momE_.at( iEta ).at( i ) && momE_.at( iEta ).at( i ) < ptBins_.at( iPt + 1 ) ) {
                momEPt.at( iPt ).push_back( momE_.at( iEta ).at( i ) );
                etaPt.at( iPt ).push_back( eta_.at( iEta ).at( i ) );
                phiPt.at( iPt ).push_back( phi_.at( iEta ).at( i ) );
                momEGenPt.at( iPt ).push_back( momEGen_.at( iEta ).at( i ) );
                etaGenPt.at( iPt ).push_back( etaGen_.at( iEta ).at( i ) );
                phiGenPt.at( iPt ).push_back( phiGen_.at( iEta ).at( i ) );
                break;
              }
            }
          }
          for ( unsigned iPt = 0; iPt < ptBins_.size() - 1; ++iPt ) {
            std::cout << "DEBUG iPt: " << iPt << std::endl;                     // DEBUG
            std::cout << "DEBUG size momEPt: " << momEPt.at( iPt ).size() << std::endl; // DEBUG
          }

        } // loop: nextInListFit()

      } // loop: nextInListProp()

    } // loop: nextInListCat()

  } // loop: iCat < objCats_.size(

  // Write and close input file
  inFile->Write();
  inFile->Close();

  if ( verbose_ > 0 )
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "   return status " << returnStatus_ << std::endl;
  return returnStatus_;

}
