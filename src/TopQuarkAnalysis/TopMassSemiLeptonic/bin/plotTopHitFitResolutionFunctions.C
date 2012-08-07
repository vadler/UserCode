#include <string>
#include <vector>
#include <iostream>
#include <cmath>

#include "boost/lexical_cast.hpp"

#include <TROOT.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TFile.h>
#include <TKey.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TLegend.h>

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
  gStyle->SetPadTopMargin( 0.05 );
  gStyle->SetPadRightMargin( 0.05 );
  gStyle->SetPadBottomMargin( 0.1 );
  gStyle->SetPadLeftMargin( 0.1 );
  gStyle->SetTitleSize( 0.06, "XYZ" );
  gStyle->SetTitleFillColor( kWhite );
  gStyle->SetTitleBorderSize( 1 );
  gStyle->SetStatColor( kWhite );
  gStyle->SetStatBorderSize( 1 );
  gStyle->SetOptStat( 0 );
  gStyle->SetOptFit( 0 );
  gStyle->SetOptTitle( 0 );
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
  const std::string plotPath_( io_.getParameter< std::string >( "plotPath" ) );
  const std::string resolutionFile_( io_.getParameter< std::string >( "resolutionFile" ) );
  // Configuration for plotting resolution functions
  const edm::ParameterSet & plot_( process_.getParameter< edm::ParameterSet >( "plot" ) );
  const bool onlyExisting_( plot_.getParameter< bool >( "onlyExisting" ) );
  const bool writeFiles_( plot_.getParameter< bool >( "writeFiles" ) && onlyExisting_ );
  const unsigned accuEvery_( plot_.getParameter< unsigned >( "accuEvery" ) );

  std::vector< std::vector< bool > > nominalInv_( objCats_.size() );

  // Set constants
  std::string evtSel_( "analyzeHitFit" );
  if ( refSel_ ) evtSel_.append( "Reference" );
  const std::string nameDirClass( "TDirectoryFile" );
  const std::string nameFuncClass( "TF1" );
  std::map< std::string, std::string > titleCat;
  titleCat[ "Pt" ]  = "p_{t}";
  titleCat[ "Eta" ] = "#eta";
  titleCat[ "Phi" ] = "#phi";
  std::map< std::string, std::string > titleYPropSigma;
  titleYPropSigma[ "Pt" ]  = "#sigma of p_{t} (GeV)";
  titleYPropSigma[ "Eta" ] = "#sigma of #eta";
  titleYPropSigma[ "Phi" ] = "#sigma of #phi";
  std::map< std::string, std::string > titleYPropInvSigma;
  titleYPropInvSigma[ "Pt" ]  = "#sigma of #frac{1}{p_{t}} (#frac{1}{GeV})";
  titleYPropInvSigma[ "Eta" ] = "";
  titleYPropInvSigma[ "Phi" ] = "";


  // Use existing resolution functions

  std::cout << std::endl
            << argv[ 0 ] << " --> INFO:" << std::endl
            << "    accessing existing resolution functions from resolution file '" << resolutionFile_ << "'" << std::endl
            << std::endl;

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
    TDirectory * dirCatRes_( ( TDirectory* )( resolutionFile->Get( objCat.c_str() ) ) );
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
      TDirectory * dirPropRes_( ( TDirectory* )( dirCatRes_->Get( kinProp.c_str() ) ) );

      TList * listPropRes( dirPropRes_->GetListOfKeys() );
      TIter nextInListPropRes( listPropRes );
      bool bEta( false );
      while ( TKey * keyEtaRes = ( TKey* )nextInListPropRes() ) {
        if ( std::string( keyEtaRes->GetClassName() ) != nameDirClass ) continue;
        const std::string binEta( keyEtaRes->GetName() );
        TDirectory * dirEtaRes_( ( TDirectory* )( dirPropRes_->Get( binEta.c_str() ) ) );

        const std::string nameEtaRes( "fitExist_" + objCat + "_" + kinProp + "_" + binEta );
        const std::string nameEtaInvRes( "fitExist_" + objCat + "_Inv_" + kinProp + "_" + binEta );

        TList * listEtaRes( dirEtaRes_->GetListOfKeys() );
        TIter nextInListEtaRes( listEtaRes );
        TF1 * resEtaSigma( 0 );
        TF1 * resEtaSigmaInv( 0 );
        while ( TKey * keyFunc = ( TKey* )nextInListEtaRes() ) {
          if ( std::string( keyFunc->GetClassName() ) != nameFuncClass ) continue;
          resEtaSigma    = ( TF1* )( dirEtaRes_->Get( nameEtaRes.c_str() ) );
          resEtaSigmaInv = ( TF1* )( dirEtaRes_->Get( nameEtaInvRes.c_str() ) );
        }
        if ( ( resEtaSigma && resEtaSigmaInv ) || ( ! resEtaSigma && ! resEtaSigmaInv ) ) {
          std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                    << "    inconsistent resolution functions in '" << objCat << "', '" << kinProp << "', '" << binEta << std::endl;
          continue;
        }
        if ( ! bEta ) {
          nominalInv_.at( uCat ).push_back( resEtaSigmaInv ? true : false );
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
  TCanvas * canv( new TCanvas( "canv", "", 768, 512 ) );

  std::vector< std::vector< double > > etaBins;
  std::vector< std::vector< double > > ptBins;

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
    returnStatus_ += 0x30;
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

    // Eta binning
    std::vector< double > etaBins;
    TH1D * histBinsEta( ( TH1D* )( dirCat_->Get( std::string( objCat + "_binsEta" ).c_str() ) ) );
    const bool objMetLike( histBinsEta->GetNbinsX() == 1 );
    if ( objMetLike ) {
      etaBins.push_back( histBinsEta->GetBinLowEdge( 1 ) );
    }
    else {
      for ( int iEta = 0; iEta < histBinsEta->GetNbinsX(); ++iEta ) {
        double edge( histBinsEta->GetBinLowEdge( iEta + 1 ) );
        if ( useSymm_  && edge < 0. ) continue;
        etaBins.push_back( edge );
      }
    }
    etaBins.push_back( histBinsEta->GetBinLowEdge( histBinsEta->GetNbinsX() ) + histBinsEta->GetBinWidth( histBinsEta->GetNbinsX() ) );
    const unsigned nEtaBins_( etaBins.size() - 1 );

    // Pt binning
    std::vector< double > ptBins;
    TH1D * histBinsPt( ( TH1D* )( dirCat_->Get( std::string( objCat + "_binsPt" ).c_str() ) ) );
    for ( int uPt = 0; uPt < histBinsPt->GetNbinsX(); ++uPt ) {
      ptBins.push_back( histBinsPt->GetBinLowEdge( uPt + 1 ) );
    }
    ptBins.push_back( histBinsPt->GetBinLowEdge( histBinsPt->GetNbinsX() ) + histBinsPt->GetBinWidth( histBinsPt->GetNbinsX() ) );
    const unsigned nPtBins_( ptBins.size() - 1 );

    TDirectory * dirCatRes_( ( TDirectory* )( resolutionFile->Get( objCat.c_str() ) ) );
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

        // Inversion flag from directory name
        const bool inverse( subFit.find( "Inv" ) != std::string::npos );
        if ( onlyExisting_ && nominalInv_.at( uCat ).at( uProp ) != inverse ) continue;

        const std::string titleLegend( objCat + ", " + titleCat[ kinProp ] );
        const std::string titleYSigma( inverse && kinProp == "Pt" ? titleYPropInvSigma[ kinProp ] : titleYPropSigma[ kinProp ] );

        // Loop over eta bins
        Double_t minY;
        Double_t maxY;
        bool startMinMax( true );
        TList * listFit( dirFit_->GetListOfKeys() );
        TIter nextInListFit( listFit );
        while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
          if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
          const std::string binEta( keyEta->GetName() );
          const unsigned uEta( std::atoi( binEta.substr( 3 ).data() ) );
          TDirectory * dirEta_( ( TDirectory* )( dirFit_->Get( binEta.c_str() ) ) );
          if ( verbose_ ) dirEta_->pwd();

          const std::string nameEta( name + "_" + binEta );
          const std::string nameEtaSigma( nameEta + "_Sigma" );
          const std::string nameEtaSigmaFit( nameEtaSigma + "_fit" );
          const std::string nameEtaExist( inverse ? "fitExist_" + objCat + "_Inv_" + kinProp + "_" + binEta  : "fitExist_" + objCat + "_" + kinProp + "_" + binEta );
          const std::string nameEtaSigmaPrint( plotPath_ + "/" + evtSel_ + "_" + nameEtaSigma + ".png" );
          const std::string strEta( useSymm_ ? " #leq |#eta| < " :  " #leq #eta < ");
          const std::string titleLegendEta( titleLegend + ", " + boost::lexical_cast< std::string >( etaBins.at( uEta ) ) + strEta + boost::lexical_cast< std::string >( etaBins.at( uEta + 1 ) ) );

          canv->cd();
          TLegend * legEtaSigma( new TLegend( 0.33, 0.67, 0.67, 0.9, titleLegendEta.c_str() ) );
          legEtaSigma->SetTextSize( 0.03 );
          legEtaSigma->SetFillColor( kWhite );
          legEtaSigma->SetFillStyle( 0 );
          legEtaSigma->SetBorderSize( 0 );
          bool useSame( false );

          TDirectory * dirEtaRes_( ( TDirectory* )( dirPropRes_->Get( binEta.c_str() ) ) );
          TF1 * resEtaSigma( ( TF1* )( dirEtaRes_->Get( nameEtaExist.c_str() ) ) );
          if ( resEtaSigma == 0 ) {
            std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                      << "    no resolution function in "; dirEtaRes_->pwd();
            returnStatus_ += 0x1000;
            return returnStatus_;
          }
          Double_t resEtaLow( resEtaSigma->Eval( ptBins.front() ) );
          Double_t resEtaHigh( resEtaSigma->Eval( ptBins.back() ) );
          Double_t minYEta( std::min( resEtaLow, resEtaHigh ) );
          Double_t maxYEta( std::max( resEtaLow, resEtaHigh ) );

          TH1D * histSigmaEta( ( TH1D* )( dirEta_->Get( nameEtaSigma.c_str() ) ) );
          if ( histSigmaEta != 0 ) {
            histSigmaEta->SetYTitle( titleYSigma.c_str() );
            TF1 * fitEtaSigmaFit( histSigmaEta->GetFunction( nameEtaSigmaFit.c_str() ) );
            if ( fitEtaSigmaFit != 0 ) {
              Double_t fitEtaLow( fitEtaSigmaFit->Eval( ptBins.front() ) );
              Double_t fitEtaHigh( fitEtaSigmaFit->Eval( ptBins.back() ) );
              Double_t minYFitEta( std::min( fitEtaLow, fitEtaHigh ) );
              Double_t maxYFitEta( std::max( fitEtaLow, fitEtaHigh ) );
              minYEta = std::min( minYFitEta, minYEta );
              maxYEta = std::max( maxYFitEta, maxYEta );
            }
            else {
              minYEta = std::min( histSigmaEta->GetBinContent( histSigmaEta->GetMinimumBin() ), minYEta );
              maxYEta = std::max( histSigmaEta->GetBinContent( histSigmaEta->GetMaximumBin() ), maxYEta );
            }
            histSigmaEta->SetMinimum( minYEta * 0.95 );
            histSigmaEta->SetMaximum( maxYEta * 1.05 );
            histSigmaEta->Draw();
            legEtaSigma->AddEntry( fitEtaSigmaFit, "fit");
            useSame = true;
          }
          else {
            std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                      << "   no resolution histogram in "; dirEta_->pwd();
            continue;
          }
          if ( startMinMax ) {
            minY = minYEta;
            maxY = maxYEta;
            startMinMax = false;
          }
          else {
            minY = std::min( minYEta, minY );
            maxY = std::min( maxYEta, maxY );
          }

          resEtaSigma->SetLineColor( kRed );
          if ( useSame ) {
            resEtaSigma->Draw( "Same" );
          }
          else {
            resEtaSigma->Draw();
            useSame = true;
          }
          legEtaSigma->AddEntry( resEtaSigma, "HitFit existing" );

          legEtaSigma->Draw();
          canv->Update();
          canv->Print( nameEtaSigmaPrint.c_str() );
          delete legEtaSigma;

          for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
            const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
            const std::string nameEtaPt( nameEta + "_Pt" + binPt );

            const std::string nameEtaPtDelta( nameEtaPt + "_Delta" );
            const std::string nameEtaPtDeltaRebin( nameEtaPtDelta + "Rebin" );
            const std::string nameEtaPtDeltaFit( nameEtaPtDelta + "_fit" );
            const std::string nameEtaPtDeltaRebinFit( nameEtaPtDeltaRebin + "_fit" );
            const std::string nameEtaPtDeltaPrint( plotPath_ + "/" + evtSel_ + "_" + nameEtaPtDelta + ".png" );
            const std::string titleLegendEtaPt( titleLegendEta + ", " + boost::lexical_cast< std::string >( ptBins.at( uPt ) ) + " GeV #leq p_{t} < " + boost::lexical_cast< std::string >( ptBins.at( uPt + 1 ) ) + " GeV" );

            canv->cd();
            TLegend * legEtaPtDelta( new TLegend( 0.1, 0.67, 0.9, 0.9, titleLegendEtaPt.c_str() ) );
            legEtaPtDelta->SetTextSize( 0.03 );
            legEtaPtDelta->SetFillColor( kWhite );
            legEtaPtDelta->SetFillStyle( 0 );
            legEtaPtDelta->SetBorderSize( 0 );

            TH1D * histEtaPtDelta( ( TH1D* )( dirEta_->Get( nameEtaPtDelta.c_str() ) ) );

            TH1D * histEtaPtDeltaRebin( ( TH1D* )( dirEta_->Get( nameEtaPtDeltaRebin.c_str() ) ) );
            if ( histEtaPtDeltaRebin != 0 ) {
              if ( histEtaPtDelta != 0 ) histEtaPtDeltaRebin->SetMaximum( std::max( histEtaPtDeltaRebin->GetBinContent( histEtaPtDeltaRebin->GetMaximumBin() ), histEtaPtDelta->GetBinContent( histEtaPtDelta->GetMaximumBin() ) ) * 1.05 );
              histEtaPtDeltaRebin->Draw();
              TF1 * fitEtaPtDeltaRebin( histEtaPtDeltaRebin->GetFunction( nameEtaPtDeltaRebinFit.c_str() ) );
              if ( fitEtaPtDeltaRebin ) legEtaPtDelta->AddEntry( fitEtaPtDeltaRebin, "re-binned");
              else                      legEtaPtDelta->AddEntry( histEtaPtDeltaRebin, "re-binned");
            }

            if ( histEtaPtDelta != 0 ) {
              histEtaPtDelta->SetLineColor( kRed );
              if ( histEtaPtDeltaRebin != 0 ) histEtaPtDelta->Draw( "Same" );
              else                            histEtaPtDelta->Draw();
              legEtaPtDelta->AddEntry( histEtaPtDelta, "not re-binned");
            }

            legEtaPtDelta->Draw();
            canv->Update();
            canv->Print( nameEtaPtDeltaPrint.c_str() );
            delete legEtaPtDelta;
          } // loop: uPt

        } // loop: keyEta

        canv->cd();
        const std::string nameSigmasPrint( plotPath_ + "/" + evtSel_ + "_" + name + "_Sigmas.png" );
        TLegend * legSigma( new TLegend( 0.33, 0.67, 0.67, 0.9, titleLegend.c_str() ) );
        legSigma->SetTextSize( 0.03 );
        legSigma->SetFillColor( kWhite );
        legSigma->SetFillStyle( 0 );
        legSigma->SetBorderSize( 0 );
        bool useSame( false );
        nextInListFit.Reset();
        unsigned cEta( 0 );
        while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
          if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
          const std::string binEta( keyEta->GetName() );
          const unsigned uEta( std::atoi( binEta.substr( 3 ).data() ) );
          if ( uEta % accuEvery_ != 0 ) continue;
          ++cEta;
          TDirectory * dirEta_( ( TDirectory* )( dirFit_->Get( binEta.c_str() ) ) );
          if ( verbose_ ) dirEta_->pwd();

          const std::string nameEta( name + "_" + binEta );
          const std::string nameEtaSigma( nameEta + "_Sigma" );
          const std::string nameEtaSigmaFit( nameEtaSigma + "_fit" );
          const std::string strEta( useSymm_ ? " #leq |#eta| < " :  " #leq #eta < ");
          const std::string titleLegendEta( boost::lexical_cast< std::string >( etaBins.at( uEta ) ) + strEta + boost::lexical_cast< std::string >( etaBins.at( uEta + 1 ) ) );

          TH1D * histSigmaEta( ( TH1D* )( dirEta_->Get( nameEtaSigma.c_str() ) ) );
          if ( histSigmaEta != 0 ) {
            histSigmaEta->SetLineColor( cEta );
            histSigmaEta->SetTitle( titleLegendEta.c_str() );
            histSigmaEta->SetYTitle( titleYSigma.c_str() );
            TF1 * fitEtaSigmaFit( histSigmaEta->GetFunction( nameEtaSigmaFit.c_str() ) );
            if ( fitEtaSigmaFit ) fitEtaSigmaFit->SetLineColor( cEta );
            if ( ! useSame ) {
              histSigmaEta->SetMinimum( minY * 0.95 );
              histSigmaEta->SetMaximum( maxY * 1.05 );
              histSigmaEta->Draw();
              useSame = true;
            }
            else {
              histSigmaEta->Draw( "Same" );
            }
            if ( fitEtaSigmaFit ) legSigma->AddEntry( fitEtaSigmaFit, titleLegendEta.c_str() );
            else                  legSigma->AddEntry( histSigmaEta, titleLegendEta.c_str() );
          }
          else {
            std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                      << "   no resolution histogram in "; dirEta_->pwd();
            continue;
          }

        } // loop: keyEta

        legSigma->Draw();
        canv->Update();
        canv->Print( nameSigmasPrint.c_str() );
        delete legSigma;

        canv->cd();
        canv->SetLogz();
        const std::string nameDeltaEtaPtFitSigmaMap( name + "_DeltaEtaPt_FitSigmaMap" );
        const std::string nameDeltaEtaPtFitSigmaMapPrint( plotPath_ + "/" + evtSel_ + "_" + nameDeltaEtaPtFitSigmaMap + ".png" );
        TH2D * histDeltaEtaPtFitSigmaMap( ( TH2D* )( dirFit_->Get( nameDeltaEtaPtFitSigmaMap.c_str() ) ) );
        histDeltaEtaPtFitSigmaMap->GetXaxis()->SetTitleOffset( 1.5 );
        histDeltaEtaPtFitSigmaMap->GetYaxis()->SetTitleOffset( 1.5 );
        histDeltaEtaPtFitSigmaMap->GetZaxis()->SetTitleOffset( 1.5 );
        histDeltaEtaPtFitSigmaMap->GetZaxis()->SetRangeUser( 0.0001, 0.01 );
        histDeltaEtaPtFitSigmaMap->Draw( "Lego2" );
        canv->Update();
        canv->Print( nameDeltaEtaPtFitSigmaMapPrint.c_str() );
        canv->SetLogz( 0 );

      } // loop: keyFit

      ++uProp;
    } // loop: keyProp

  } // loop: uCat

  delete canv;

  // Close input files
  for ( unsigned uFile = 0; uFile < files_.size(); ++uFile ) files_.at( uFile )->Close();

  // Close resolution file
  resolutionFile->Close();

  return returnStatus_;

}