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
  const std::vector< std::string > objCats_( process_.getParameter< std::vector< std::string > >( "objectCategories" ) );   // object categories
  const bool overwrite_(  process_.getParameter< bool >( "overwrite" ));
  const bool usePileUp_( process_.getParameter< bool >( "usePileUp" ) );
  const bool useAlt_( process_.getParameter< bool >( "useAlt" ) );
  const bool useSymm_( process_.getParameter< bool >( "useSymm" ) );
  const bool refGen_( process_.getParameter< bool >( "refGen" ) );
  const bool refSel_( process_.getParameter< bool >( "refSel" ) );
  // Configuration for in- & output
  const edm::ParameterSet & io_( process_.getParameter< edm::ParameterSet >( "io" ) );
  const std::vector< std::string > inFiles_( io_.getParameter< std::vector< std::string > >( "inputFiles" ) );
  const std::string sample_( io_.getParameter< std::string >( "sample" ) );
  const std::string outFile_( io_.getParameter< std::string >( "outputFile" ) );
  const std::string resolutionFile_( io_.getParameter< std::string >( "resolutionFile" ) );
  // Configuration for plotting resolution functions
  const edm::ParameterSet & plot_( process_.getParameter< edm::ParameterSet >( "plot" ) );
  const bool onlyExisting_( plot_.getParameter< bool >( "onlyExisting" ) );
  const bool writeFiles_( plot_.getParameter< bool >( "writeFiles" ) && onlyExisting_ );

  std::vector< std::vector< bool > > nominalInv_( objCats_.size() );

  // Set constants
  std::string evtSel_( "analyzeHitFit" );
  if ( refSel_ ) evtSel_.append( "Reference" );
  const std::string nameDirClass( "TDirectoryFile" );
  const std::string nameFuncClass( "TF1" );


  // Use existing resolution functions

  std::cout << std::endl
            << argv[ 0 ] << " --> INFO:" << std::endl
            << "    accessing existing resolution functions from resolution file '" << resolutionFile_ << "'" << std::endl;

  // Open resolution file
  TFile * resolutionFile( TFile::Open( resolutionFile_.c_str(), "READ" ) );
  if ( ! resolutionFile ) {
    std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
              << "   resolution file '" << resolutionFile_ << "' not found" << std::endl;
    returnStatus_ += 0x100;
    return returnStatus_;
  }

  // Loop over objects and quantities

  for ( unsigned uCat = 0; uCat < objCats_.size(); ++uCat ) {
    const std::string objCat( objCats_.at( uCat ) );
    TDirectory * dirCatRes_( dynamic_cast< TDirectory* >( resolutionFile->Get( objCat.c_str() ) ) );
    if ( ! dirCatRes_ ) {
      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                << "    object category '" << objCat << "' does not exist in resolution file" << std::endl;
      continue;
    }

    TList * listCatRes( dirCatRes_->GetListOfKeys() );
    TIter nextInListCatRes( listCatRes );
    while ( TKey * keyPropRes = ( TKey* )nextInListCatRes() ) {
      if ( std::string( keyPropRes->GetClassName() ) != nameDirClass ) continue;
      const std::string kinProp( keyPropRes->GetName() );
      TDirectory * dirPropRes_( dynamic_cast< TDirectory* >( dirCatRes_->Get( kinProp.c_str() ) ) );

      TList * listPropRes( dirPropRes_->GetListOfKeys() );
      TIter nextInListPropRes( listPropRes );
      bool bEta( false );
      while ( TKey * keyEtaRes = ( TKey* )nextInListPropRes() ) {
        if ( std::string( keyEtaRes->GetClassName() ) != nameDirClass ) continue;
        const std::string binEta( keyEtaRes->GetName() );
        TDirectory * dirEtaRes_( dynamic_cast< TDirectory* >( dirPropRes_->Get( binEta.c_str() ) ) );

        const std::string nameEtaRes( "fitExist_" + objCat + "_" + kinProp + "_" + binEta );
        const std::string nameEtaInvRes( "fitExist_" + objCat + "_Inv_" + kinProp + "_" + binEta );

        TList * listEtaRes( dirEtaRes_->GetListOfKeys() );
        TIter nextInListEtaRes( listEtaRes );
        TF1 * resSigma( 0 );
        TF1 * resSigmaInv( 0 );
        while ( TKey * keyFunc = ( TKey* )nextInListEtaRes() ) {
          if ( std::string( keyFunc->GetClassName() ) != nameFuncClass ) continue;
          resSigma    = dynamic_cast< TF1* >( dirEtaRes_->Get( nameEtaRes.c_str() ) );
          resSigmaInv = dynamic_cast< TF1* >( dirEtaRes_->Get( nameEtaInvRes.c_str() ) );
        }
        if ( ( resSigma && resSigmaInv ) || ( ! resSigma && ! resSigmaInv ) ) {
          std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                    << "    inconsistent resolution functions in '" << objCat << "', '" << kinProp << "', '" << binEta << std::endl;
          continue;
        }
        if ( ! bEta ) {
          nominalInv_.at( uCat ).push_back( resSigmaInv ? true : false );
          bEta = true;
        }

      } // loop: nextInListPropRes()

      if (! bEta  ) {
        std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                  << "    no resolution function in "; dirPropRes_->pwd();
        returnStatus_ += 0x200;
      }

    } // loop: nextInListCatRes()

  } // loop: uCat < objCats_.size()


  std::vector< TFile* > files_;

  std::vector< std::vector< double > > etaBins_;
  std::vector< std::vector< double > > ptBins_;

  // Open input files

//   bool first( true );
  for ( unsigned uFile = 0; uFile < inFiles_.size(); ++uFile ) {
    const std::string inFile( inFiles_.at( uFile ) );
    TFile * file( TFile::Open( inFile.c_str(), "READ" ) );
    if ( ! file ) {
      std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                << "   input file '" << inFiles_.at( uFile ) << "' missing; trying next file" << std::endl;
      returnStatus_ += 0x10;
      continue;
    }
    files_.push_back( file );

//     // Get binning (onyl once; has to be identical for all input files)
//     if ( first ) {
//       if ( verbose_ )
//         std::cout << std::endl
//                   << argv[ 0 ] << " --> INFO:" << std::endl
//                   << "   extract binning from input file '" << inFiles_.at( uFile ) << "'" << std::endl;
//       TDirectory * dirSel_ = ( TDirectory* )( file->Get( evtSel_.c_str() ) );
//
//       for ( unsigned uCat = 0; uCat < objCats_.size(); ++uCat ) {
//         const std::string objCat( objCats_.at( uCat ) );
//         TDirectory * dirCat_( ( TDirectory* )( dirSel_->Get( objCat.c_str() ) ) );
//         if ( ! dirCat_ ) {
//           std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
//                     << "   object category '" << objCat << "' does not exist in input file '" << inFile << "'" << std::endl;
//           continue;
//         }
//
//         // Eta binning
//         std::vector< double > etaBins;
//         TH1D * histEtaBins( ( TH1D* )( dirCat_->Get( std::string( objCat + "_binsEta" ).c_str() ) ) );
//         for ( int iEta = 1; iEta <= histEtaBins->GetNbinsX(); ++iEta ) {
//           etaBins.push_back( histEtaBins->GetBinLowEdge( iEta ) );
//         }
//         etaBins.push_back( histEtaBins->GetBinLowEdge( histEtaBins->GetNbinsX() ) + histEtaBins->GetBinWidth( histEtaBins->GetNbinsX() ) );
//         etaBins_.push_back( etaBins );
//
//         // Pt binning
//         std::vector< double > ptBins;
//         TH1D * histPtBins( ( TH1D* )( dirCat_->Get( std::string( objCat + "_binsPt" ).c_str() ) ) );
//         for ( int iPt = 1; iPt <= histPtBins->GetNbinsX(); ++iPt ) {
//           ptBins.push_back( histPtBins->GetBinLowEdge( iPt ) );
//         }
//         ptBins.push_back( histPtBins->GetBinLowEdge( histPtBins->GetNbinsX() ) + histPtBins->GetBinWidth( histPtBins->GetNbinsX() ) );
//         ptBins_.push_back( ptBins );
//       }
//
//       first = false;
//     }
  }  // loop: uFile
  if ( files_.empty() ) {
      std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                << "   no input files found" << std::endl;
      returnStatus_ += 0x20;
      return returnStatus_;
  }

  TFile * refFile( files_.at( 0 ) );
  TDirectory * dirSel_ = ( TDirectory* )( refFile->Get( evtSel_.c_str() ) );
  if ( ! dirSel_ ) {
    std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
              << "   selection '" << evtSel_ << "' does not exist in reference file '" << refFile->GetName() << "'" << std::endl;
    returnStatus_ += 0x20;
    return returnStatus_;
  }

  // Loop over configured object categories
  for ( unsigned uCat = 0; uCat < objCats_.size(); ++uCat ) {
    const std::string objCat( objCats_.at( uCat ) );
    TDirectory * dirCat_( ( TDirectory* )( dirSel_->Get( objCat.c_str() ) ) );
    if ( ! dirCat_ ) {
      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                << "   object category '" << objCat << "' does not exist in reference file '" << refFile->GetName() << "'" << std::endl;
      continue;
    }

    TDirectory * dirCatRes_( dynamic_cast< TDirectory* >( resolutionFile->Get( objCat.c_str() ) ) );
    if ( ! dirCatRes_ ) {
      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                << "    object category '" << objCat << "' does not exist in resolution file" << std::endl;
      continue;
    }

    // Loop over kinematic properties
    TList * listCat( dirCat_->GetListOfKeys() );
    TIter nextInListCat( listCat );
    unsigned uProp( 0 );
    while ( TKey * keyProp = ( TKey* )nextInListCat() ) {
      if ( std::string( keyProp->GetClassName() ) != nameDirClass ) continue;
      if ( onlyExisting_ && ! ( uProp < nominalInv_.at( uCat ).size() ) ) break;
      const std::string kinProp( keyProp->GetName() );
      TDirectory * dirProp_( ( TDirectory* )( dirCat_->Get( kinProp.c_str() ) ) );

      TDirectory * dirPropRes_( ( TDirectory* )( dirCatRes_->Get( kinProp.c_str() ) ) );

      // Loop over fit versions
      TList * listProp( dirProp_->GetListOfKeys() );
      TIter nextInListProp( listProp );
      while ( TKey * keyFit = ( TKey* )nextInListProp() ) {
        if ( std::string( keyFit->GetClassName() ) != nameDirClass ) continue;
        const std::string subFit( keyFit->GetName() );
        // These are real switches: depending on configuration, only one setting combination can be run at a time
        if ( useAlt_  == ( subFit.find( "Alt" )  == std::string::npos ) ) continue;
        if ( useSymm_ == ( subFit.find( "Symm" ) == std::string::npos ) ) continue;
        if ( refGen_  == ( subFit.find( "Gen" )  == std::string::npos ) ) continue;
        TDirectory * dirFit_ = ( TDirectory* )( dirProp_->Get( subFit.c_str() ) );

        const std::string name( objCat + "_" + kinProp + "_" + subFit );

        // Inversion flag from directory nameEta
        const bool inverse( subFit.find( "Inv" ) != std::string::npos );
        if ( onlyExisting_ && nominalInv_.at( uCat ).at( uProp ) != inverse ) continue;

        // Loop over eta bins
        TList * listFit( dirFit_->GetListOfKeys() );
        TIter nextInListFit( listFit );
        while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
          if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
          const std::string binEta( keyEta->GetName() );
          TDirectory * dirEta_( dynamic_cast< TDirectory* >( dirFit_->Get( binEta.c_str() ) ) );
          if ( verbose_ ) dirEta_->pwd();

          const std::string nameEta( name + "_" + binEta );
          const std::string nameSigma( nameEta + "_Sigma" );
          TH1D * histSigma( ( TH1D* )( dirEta_->Get( nameSigma.c_str() ) ) );

          TDirectory * dirEtaRes_( dynamic_cast< TDirectory* >( dirPropRes_->Get( binEta.c_str() ) ) );
          const std::string nameEtaExist( inverse ? "fitExist_" + objCat + "_Inv_" + kinProp + "_" + binEta  : "fitExist_" + objCat + "_" + kinProp + "_" + binEta );
          TF1 * resSigma( 0 );
          TList * listEtaRes( dirEtaRes_->GetListOfKeys() );
          TIter nextInListEtaRes( listEtaRes );
          while ( TKey * keyFuncRes = ( TKey* )nextInListEtaRes() ) {
            if ( std::string( keyFuncRes->GetClassName() ) != nameFuncClass ) continue;
            resSigma = dynamic_cast< TF1* >( dirEtaRes_->Get( nameEtaExist.c_str() ) );
          }
          if ( resSigma == 0 ) {
            std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                      << "    no resolution function in "; dirEtaRes_->pwd();
          }

          if ( histSigma != 0 ) {
            const std::string nameCanvSigma( "canv_" + nameSigma );
            TCanvas * canvSigma( new TCanvas( nameCanvSigma.c_str(), "", 768, 512 ) );
            histSigma->Draw();
            if ( resSigma != 0 ) {
              resSigma->SetLineColor( kRed );
              resSigma->Draw( "Same" );
            }

//             const std::string nameDirEta( evtSel_ + "/" + objCat + "/" + kinProp + "/" + subFit + "/" + binEta );
//             for ( unsigned uFile = 1; uFile < files_.size(); ++uFile ) { // skip 1st file (we have it already)
//               files_.at( uFile )->cd( nameDirEta.c_str() );
//               if ( verbose_ ) gDirectory->pwd();
//               canvSigma->cd();
//               TH1D * histSigmaFile( ( TH1D* )( gDirectory->Get( nameSigma.c_str() ) ) );
//               if ( histSigmaFile != 0 ) {
//                 TF1 * fitSigmaFile( histSigmaFile->GetFunction( nameFitSigma.c_str() ) );
//                 fitSigma->DrawCopy( "Same" );
//               }
//             }

            canvSigma->Update();
            canvSigma->SaveAs( ".png" );

            delete canvSigma;

          }
          else {
            std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                      << "   no resolution histogram in "; dirEta_->pwd();
            returnStatus_ += 0x1000;
            continue;
          }

        } // loop: keyEta

      } // loop: keyFit

      ++uProp;
    } // loop: keyProp

  } // loop: uCat

  // Close input files
  for ( unsigned uFile = 0; uFile < files_.size(); ++uFile ) files_.at( uFile )->Close();

  // Close resolution file
  resolutionFile->Close();

  return returnStatus_;

}