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
  // Configuration for plotting resolution functions
  const edm::ParameterSet & plot_( process_.getParameter< edm::ParameterSet >( "plot" ) );
  const bool onlyExisting_( plot_.getParameter< bool >( "onlyExisting" ) );
  const bool writeFiles_( plot_.getParameter< bool >( "writeFiles" ) && onlyExisting_ );
  const std::string plotCycle_( plot_.getParameter< std::string >( "plotCycle" ) );
  const bool plotNonRestr_( plot_.getParameter< bool >( "plotNonRestr" ) );
  const bool plotEtaPt_( plot_.getParameter< bool >( "plotEtaPt" ) );
  const unsigned accuEvery_( plot_.getParameter< unsigned >( "accuEvery" ) );

  // Set constants
  std::string evtSel_( "analyzeHitFit" );
  if ( refSel_ ) evtSel_.append( "Reference" );
  const std::string nameDirClass( "TDirectoryFile" );
  const std::string nameFuncClass( "TF1" );
  const std::string fitFunc( plotCycle_ == "1" ? "Gaussian + log-normal" : "Gaussian" );
  const std::string bkgFunc( plotCycle_ == "1" ? "Log-normal" : "" );


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
    if ( objCat == "UdscJet" || objCat == "BJet" ) {
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

      TDirectory * dirPt_( ( TDirectory* )( dirCat_->Get( "Pt" ) ) );

      // Loop over fit versions
      TList * listProp( dirPt_->GetListOfKeys() );
      TIter nextInListProp( listProp );
      while ( TKey * keyFit = ( TKey* )nextInListProp() ) {
        if ( std::string( keyFit->GetClassName() ) != nameDirClass ) continue;
        const std::string subFit( keyFit->GetName() );
        if ( subFit.find( "Inv" ) != std::string::npos ) continue; // nothing to do for inverse
        // These are real switches: depending on configuration, only one setting combination can be run at a time
        if ( useAlt_  == ( subFit.find( "Alt" )  == std::string::npos ) ) continue;
        if ( useSymm_ == ( subFit.find( "Symm" ) == std::string::npos ) ) continue;
        if ( refGen_  == ( subFit.find( "Gen" )  == std::string::npos ) ) continue;
        TDirectory * dirFit_( ( TDirectory* )( dirPt_->Get( subFit.c_str() ) ) );

        const std::string name( objCat + "_Pt_" + subFit );
        const std::string titleLegend( objCat + ", p_{t}" );

        const std::string nameFracL5L7( name + "_FracL5L7" );
        const std::string nameFracL5L7Print( plotPath_ + "/" + evtSel_ + "_" + nameFracL5L7 + ".png" );
        TH1D * histFracL5L7( ( TH1D* )( dirFit_->Get( std::string( nameFracL5L7 + ";" + plotCycle_ ).c_str() ) ) );
        if ( plotNonRestr_ && histFracL5L7 != 0 ) {
          TLegend * legFracL5L7Fit( new TLegend( 0.1, 0.67, 0.9, 0.9, titleLegend.c_str() ) );
          legFracL5L7Fit->SetTextSize( 0.03 );
          legFracL5L7Fit->SetFillColor( kWhite );
          legFracL5L7Fit->SetFillStyle( 0 );
          legFracL5L7Fit->SetBorderSize( 0 );
          canv->cd();
          histFracL5L7->Draw();
          const std::string nameFracL5L7Fit( nameFracL5L7 + "_fit" );
          TF1 * fitFracL5L7( histFracL5L7->GetFunction( nameFracL5L7Fit.c_str() ) );
          if ( fitFracL5L7 != 0 ) legFracL5L7Fit->AddEntry( fitFracL5L7, fitFunc.c_str() );
          else                    legFracL5L7Fit->AddEntry( histFracL5L7, fitFunc.c_str() );
          const std::string nameFracL5L7Bkg( nameFracL5L7 + "_bkg" );
          TF1 * bkgFracL5L7(  ( TF1* )( dirFit_->Get( std::string( nameFracL5L7Bkg + ";" + plotCycle_ ).c_str() ) ));
          if ( bkgFracL5L7 != 0 ) {
            bkgFracL5L7->SetLineColor( kRed );
            bkgFracL5L7->Draw( "Same" );
            legFracL5L7Fit->AddEntry( bkgFracL5L7, bkgFunc.c_str() );
          }
          legFracL5L7Fit->Draw();
          canv->Print( nameFracL5L7Print.c_str() );
          delete legFracL5L7Fit;

        }

        const std::string nameFracL5L7Restr( nameFracL5L7 + "Restr" );
        const std::string nameFracL5L7RestrPrint( plotPath_ + "/" + evtSel_ + "_" + nameFracL5L7Restr + ".png" );
        TH1D * histFracL5L7Restr( ( TH1D* )( dirFit_->Get( std::string( nameFracL5L7Restr + ";" + plotCycle_ ).c_str() ) ) );
        std::cout << nameFracL5L7Restr << " " << histFracL5L7Restr << std::endl;
        if ( histFracL5L7Restr != 0 ) {
          TLegend * legFracL5L7RestrFit( new TLegend( 0.1, 0.67, 0.9, 0.9, titleLegend.c_str() ) );
          legFracL5L7RestrFit->SetTextSize( 0.03 );
          legFracL5L7RestrFit->SetFillColor( kWhite );
          legFracL5L7RestrFit->SetFillStyle( 0 );
          legFracL5L7RestrFit->SetBorderSize( 0 );
          canv->cd();
          histFracL5L7Restr->Draw();
          const std::string nameFracL5L7RestrFit( nameFracL5L7Restr + "_fit" );
          TF1 * fitFracL5L7Restr( histFracL5L7Restr->GetFunction( nameFracL5L7RestrFit.c_str() ) );
          if ( fitFracL5L7Restr != 0 ) legFracL5L7RestrFit->AddEntry( fitFracL5L7Restr, fitFunc.c_str() );
          else                         legFracL5L7RestrFit->AddEntry( histFracL5L7Restr, fitFunc.c_str() );
          const std::string nameFracL5L7RestrBkg( nameFracL5L7Restr + "_bkg" );
          TF1 * bkgFracL5L7Restr( ( TF1* )( dirFit_->Get( std::string( nameFracL5L7RestrBkg + ";" + plotCycle_ ).c_str() ) ) );
          if ( bkgFracL5L7Restr != 0 ) {
            bkgFracL5L7Restr->SetLineColor( kRed );
            bkgFracL5L7Restr->Draw( "Same" );
            legFracL5L7RestrFit->AddEntry( bkgFracL5L7Restr, bkgFunc.c_str() );
          }
          legFracL5L7RestrFit->Draw();
          canv->Print( nameFracL5L7RestrPrint.c_str() );
          delete legFracL5L7RestrFit;
        }

        // Loop over pt bins

        // Fit performance histograms
        if ( plotNonRestr_ ) {
          const std::string nameFracL5L7PtFitMeanMap( name + "_FracL5L7Pt_FitMeanMap" );
          const std::string nameFracL5L7PtFitMeanMapPrint( plotPath_ + "/" + evtSel_ + "_" + nameFracL5L7PtFitMeanMap + ".png" );
          TH1D * histFracL5L7PtFitMeanMap( ( TH1D* )( dirFit_->Get( std::string( nameFracL5L7PtFitMeanMap + ";" + plotCycle_ ).c_str() ) ) );
          if ( histFracL5L7PtFitMeanMap != 0 ) {
            canv->cd();
            histFracL5L7PtFitMeanMap->Draw();
            canv->Print( nameFracL5L7PtFitMeanMapPrint.c_str() );
          }
        }
        const std::string nameFracL5L7RestrPtFitMeanMap( name + "_FracL5L7RestrPt_FitMeanMap" );
        const std::string nameFracL5L7RestrPtFitMeanMapPrint( plotPath_ + "/" + evtSel_ + "_" + nameFracL5L7RestrPtFitMeanMap + ".png" );
        TH1D * histFracL5L7RestrPtFitMeanMap( ( TH1D* )( dirFit_->Get( std::string( nameFracL5L7RestrPtFitMeanMap + ";" + plotCycle_ ).c_str() ) ) );
        if ( histFracL5L7RestrPtFitMeanMap != 0 ) {
          canv->cd();
          histFracL5L7RestrPtFitMeanMap->Draw();
          canv->Print( nameFracL5L7RestrPtFitMeanMapPrint.c_str() );
        }

        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
          const std::string namePt( name + "_Pt" + binPt );
          const std::string titleLegendPt( titleLegend + ", " + boost::lexical_cast< std::string >( ptBins.at( uPt ) ) + " GeV #leq p_{t} < " + boost::lexical_cast< std::string >( ptBins.at( uPt + 1 ) ) + " GeV" );

          const std::string namePtFracL5L7( namePt + "_FracL5L7" );
          const std::string namePtFracL5L7Print( plotPath_ + "/" + evtSel_ + "_" + namePtFracL5L7 + ".png" );
          TH1D * histPtFracL5L7( ( TH1D* )( dirFit_->Get( std::string( namePtFracL5L7 + ";" + plotCycle_ ).c_str() ) ) );
          if ( plotNonRestr_ && histPtFracL5L7 != 0 ) {
            TLegend * legPtFracL5L7Fit( new TLegend( 0.1, 0.67, 0.9, 0.9, titleLegendPt.c_str() ) );
            legPtFracL5L7Fit->SetTextSize( 0.03 );
            legPtFracL5L7Fit->SetFillColor( kWhite );
            legPtFracL5L7Fit->SetFillStyle( 0 );
            legPtFracL5L7Fit->SetBorderSize( 0 );
            canv->cd();
            histPtFracL5L7->Draw();
            const std::string namePtFracL5L7Fit( namePtFracL5L7 + "_fit" );
            TF1 * fitPtFracL5L7( histPtFracL5L7->GetFunction( namePtFracL5L7Fit.c_str() ) );
            if ( fitPtFracL5L7 != 0 ) legPtFracL5L7Fit->AddEntry( fitPtFracL5L7, fitFunc.c_str() );
            else                      legPtFracL5L7Fit->AddEntry( histPtFracL5L7, fitFunc.c_str() );
            const std::string namePtFracL5L7Bkg( namePtFracL5L7 + "_bkg" );
            TF1 * bkgPtFracL5L7( ( TF1* )( dirFit_->Get( std::string( namePtFracL5L7Bkg + ";" + plotCycle_ ).c_str() ) ) );
            if ( bkgPtFracL5L7 != 0 ) {
              bkgPtFracL5L7->SetLineColor( kRed );
              bkgPtFracL5L7->Draw( "Same" );
              legPtFracL5L7Fit->AddEntry( bkgPtFracL5L7, bkgFunc.c_str() );
            }
            legPtFracL5L7Fit->Draw();
            canv->Print( namePtFracL5L7Print.c_str() );
            delete legPtFracL5L7Fit;
          }

          const std::string namePtFracL5L7Restr( namePtFracL5L7 + "Restr" );
          const std::string namePtFracL5L7RestrPrint( plotPath_ + "/" + evtSel_ + "_" + namePtFracL5L7Restr + ".png" );
          TH1D * histPtFracL5L7Restr( ( TH1D* )( dirFit_->Get( std::string( namePtFracL5L7Restr + ";" + plotCycle_ ).c_str() ) ) );
          if ( histPtFracL5L7Restr != 0 ) {
            TLegend * legPtFracL5L7RestrFit( new TLegend( 0.1, 0.67, 0.9, 0.9, titleLegendPt.c_str() ) );
            legPtFracL5L7RestrFit->SetTextSize( 0.03 );
            legPtFracL5L7RestrFit->SetFillColor( kWhite );
            legPtFracL5L7RestrFit->SetFillStyle( 0 );
            legPtFracL5L7RestrFit->SetBorderSize( 0 );
            canv->cd();
            histPtFracL5L7Restr->Draw();
            const std::string namePtFracL5L7RestrFit( namePtFracL5L7Restr + "_fit" );
            TF1 * fitPtFracL5L7Restr( histPtFracL5L7Restr->GetFunction( namePtFracL5L7RestrFit.c_str() ) );
            if ( fitPtFracL5L7Restr != 0 ) legPtFracL5L7RestrFit->AddEntry( fitPtFracL5L7Restr, fitFunc.c_str() );
            else                           legPtFracL5L7RestrFit->AddEntry( histPtFracL5L7Restr, fitFunc.c_str() );
            const std::string namePtFracL5L7RestrBkg( namePtFracL5L7Restr + "_bkg" );
            TF1 * bkgPtFracL5L7Restr( ( TF1* )( dirFit_->Get( std::string( namePtFracL5L7RestrBkg + ";" + plotCycle_ ).c_str() ) ) );
            if ( bkgPtFracL5L7Restr != 0 ) {
              bkgPtFracL5L7Restr->SetLineColor( kRed );
              bkgPtFracL5L7Restr->Draw( "Same" );
              legPtFracL5L7RestrFit->AddEntry( bkgPtFracL5L7Restr, bkgFunc.c_str() );
            }
            legPtFracL5L7RestrFit->Draw();
            canv->Print( namePtFracL5L7RestrPrint.c_str() );
            delete legPtFracL5L7RestrFit;
          }

        } // loop: uPt < nPtBins_

        // Loop over eta bins

        // Fit performance histograms
        if ( plotNonRestr_ ) {
          const std::string nameFracL5L7EtaFitMeanMap( name + "_FracL5L7Eta_FitMeanMap" );
          const std::string nameFracL5L7EtaFitMeanMapPrint( plotPath_ + "/" + evtSel_ + "_" + nameFracL5L7EtaFitMeanMap + ".png" );
          TH1D * histFracL5L7EtaFitMeanMap( ( TH1D* )( dirFit_->Get( std::string( nameFracL5L7EtaFitMeanMap + ";" + plotCycle_ ).c_str() ) ) );
          if ( histFracL5L7EtaFitMeanMap != 0 ) {
            canv->cd();
            histFracL5L7EtaFitMeanMap->Draw();
            canv->Print( nameFracL5L7EtaFitMeanMapPrint.c_str() );
          }
          const std::string nameFracL5L7EtaPtFitMeanMap( name + "_FracL5L7EtaPt_FitMeanMap" );
          TH2D * histFracL5L7EtaPtFitMeanMap( ( TH2D* )( dirFit_->Get( nameFracL5L7EtaPtFitMeanMap.c_str() ) ) );
          const std::string nameFracL5L7EtaPtFitMean( name + "_FracL5L7EtaPt_FitMean" );
          TH1D * histFracL5L7EtaPtFitMean( ( TH1D* )( dirFit_->Get( nameFracL5L7EtaPtFitMean.c_str() ) ) );
          const std::string nameFracL5L7EtaPtFitSigma( name + "_FracL5L7EtaPt_FitSigma" );
          TH1D * histFracL5L7EtaPtFitSigma( ( TH1D* )( dirFit_->Get( nameFracL5L7EtaPtFitSigma.c_str() ) ) );
        }
        const std::string nameFracL5L7RestrEtaFitMeanMap( name + "_FracL5L7RestrEta_FitMeanMap" );
        const std::string nameFracL5L7RestrEtaFitMeanMapPrint( plotPath_ + "/" + evtSel_ + "_" + nameFracL5L7RestrEtaFitMeanMap + ".png" );
        TH1D * histFracL5L7RestrEtaFitMeanMap( ( TH1D* )( dirFit_->Get( std::string( nameFracL5L7RestrEtaFitMeanMap + ";" + plotCycle_ ).c_str() ) ) );
        if ( histFracL5L7RestrEtaFitMeanMap != 0 ) {
          canv->cd();
          histFracL5L7RestrEtaFitMeanMap->Draw();
          canv->Print( nameFracL5L7RestrEtaFitMeanMapPrint.c_str() );
        }
        const std::string nameFracL5L7RestrEtaPtFitMeanMap( name + "_FracL5L7RestrEtaPt_FitMeanMap" );
        TH2D * histFracL5L7RestrEtaPtFitMeanMap( ( TH2D* )( dirFit_->Get( nameFracL5L7RestrEtaPtFitMeanMap.c_str() ) ) );
        const std::string nameFracL5L7RestrEtaPtFitMean( name + "_FracL5L7RestrEtaPt_FitMean" );
        TH1D * histFracL5L7RestrEtaPtFitMean( ( TH1D* )( dirFit_->Get( nameFracL5L7RestrEtaPtFitMean.c_str() ) ) );
        const std::string nameFracL5L7RestrEtaPtFitSigma( name + "_FracL5L7RestrEtaPt_FitSigma" );
        TH1D * histFracL5L7RestrEtaPtFitSigma( ( TH1D* )( dirFit_->Get( nameFracL5L7RestrEtaPtFitSigma.c_str() ) ) );

        TList * listFit( dirFit_->GetListOfKeys() );
        TIter nextInListFit( listFit );
        while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
          if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
          const std::string binEta( keyEta->GetName() );
          const unsigned uEta( std::atoi( binEta.substr( 3 ).data() ) );
          TDirectory * dirEta_( ( TDirectory* )( dirFit_->Get( binEta.c_str() ) ) );

          const std::string nameEta( name + "_" + binEta );
          const std::string strEta( useSymm_ ? " #leq |#eta| < " :  " #leq #eta < ");
          const std::string titleLegendEta( titleLegend + ", " + boost::lexical_cast< std::string >( etaBins.at( uEta ) ) + strEta + boost::lexical_cast< std::string >( etaBins.at( uEta + 1 ) ) );

          const std::string nameEtaFracL5L7( nameEta + "_FracL5L7" );
          const std::string nameEtaFracL5L7Print( plotPath_ + "/" + evtSel_ + "_" + nameEtaFracL5L7 + ".png" );
          TH1D * histEtaFracL5L7( ( TH1D* )( dirEta_->Get( std::string( nameEtaFracL5L7 + ";" + plotCycle_ ).c_str() ) ) );
          if ( plotNonRestr_ && histEtaFracL5L7 != 0 ) {
            TLegend * legEtaFracL5L7Fit( new TLegend( 0.1, 0.67, 0.9, 0.9, titleLegendEta.c_str() ) );
            legEtaFracL5L7Fit->SetTextSize( 0.03 );
            legEtaFracL5L7Fit->SetFillColor( kWhite );
            legEtaFracL5L7Fit->SetFillStyle( 0 );
            legEtaFracL5L7Fit->SetBorderSize( 0 );
            canv->cd();
            histEtaFracL5L7->Draw();
            const std::string nameEtaFracL5L7Fit( nameEtaFracL5L7 + "_fit" );
            TF1 * fitEtaFracL5L7( histEtaFracL5L7->GetFunction( nameEtaFracL5L7Fit.c_str() ) );
            if ( fitEtaFracL5L7 != 0 ) legEtaFracL5L7Fit->AddEntry( fitEtaFracL5L7, fitFunc.c_str() );
            else                      legEtaFracL5L7Fit->AddEntry( histEtaFracL5L7, fitFunc.c_str() );
            const std::string nameEtaFracL5L7Bkg( nameEtaFracL5L7 + "_bkg" );
            TF1 * bkgEtaFracL5L7( ( TF1* )( dirEta_->Get( std::string( nameEtaFracL5L7Bkg + ";" + plotCycle_ ).c_str() ) ) );
            if ( bkgEtaFracL5L7 != 0 ) {
              bkgEtaFracL5L7->SetLineColor( kRed );
              bkgEtaFracL5L7->Draw( "Same" );
              legEtaFracL5L7Fit->AddEntry( bkgEtaFracL5L7, bkgFunc.c_str() );
            }
            legEtaFracL5L7Fit->Draw();
            canv->Print( nameEtaFracL5L7Print.c_str() );
            delete legEtaFracL5L7Fit;
          }

          const std::string nameEtaFracL5L7Restr( nameEtaFracL5L7 + "Restr" );
          const std::string nameEtaFracL5L7RestrPrint( plotPath_ + "/" + evtSel_ + "_" + nameEtaFracL5L7Restr + ".png" );
          TH1D * histEtaFracL5L7Restr( ( TH1D* )( dirEta_->Get( std::string( nameEtaFracL5L7Restr + ";" + plotCycle_ ).c_str() ) ) );
          if ( histEtaFracL5L7Restr != 0 ) {
            TLegend * legEtaFracL5L7RestrFit( new TLegend( 0.1, 0.67, 0.9, 0.9, titleLegendEta.c_str() ) );
            legEtaFracL5L7RestrFit->SetTextSize( 0.03 );
            legEtaFracL5L7RestrFit->SetFillColor( kWhite );
            legEtaFracL5L7RestrFit->SetFillStyle( 0 );
            legEtaFracL5L7RestrFit->SetBorderSize( 0 );
            canv->cd();
            histEtaFracL5L7Restr->Draw();
            const std::string nameEtaFracL5L7RestrFit( nameEtaFracL5L7Restr + "_fit" );
            TF1 * fitEtaFracL5L7Restr( histEtaFracL5L7Restr->GetFunction( nameEtaFracL5L7RestrFit.c_str() ) );
            if ( fitEtaFracL5L7Restr != 0 ) legEtaFracL5L7RestrFit->AddEntry( fitEtaFracL5L7Restr, fitFunc.c_str() );
            else                           legEtaFracL5L7RestrFit->AddEntry( histEtaFracL5L7Restr, fitFunc.c_str() );
            const std::string nameEtaFracL5L7RestrBkg( nameEtaFracL5L7Restr + "_bkg" );
            TF1 * bkgEtaFracL5L7Restr( ( TF1* )( dirEta_->Get( std::string( nameEtaFracL5L7RestrBkg + ";" + plotCycle_ ).c_str() ) ) );
            if ( bkgEtaFracL5L7Restr != 0 ) {
              bkgEtaFracL5L7Restr->SetLineColor( kRed );
              bkgEtaFracL5L7Restr->Draw( "Same" );
              legEtaFracL5L7RestrFit->AddEntry( bkgEtaFracL5L7Restr, bkgFunc.c_str() );
            }
            legEtaFracL5L7RestrFit->Draw();
            canv->Print( nameEtaFracL5L7RestrPrint.c_str() );
            delete legEtaFracL5L7RestrFit;
          }

          // Loop over pt bins

          if ( plotEtaPt_ ) {

            // Fit performance histograms
            if ( plotNonRestr_ ) {
              const std::string nameEtaFracL5L7PtFitMeanMap( nameEta + "_FracL5L7Pt_FitMeanMap" );
              TH1D * histEtaFracL5L7PtFitMeanMap( ( TH1D* )( dirEta_->Get( nameEtaFracL5L7PtFitMeanMap.c_str() ) ) );
            }
            const std::string nameEtaFracL5L7RestrPtFitMeanMap( nameEta + "_FracL5L7RestrPt_FitMeanMap" );
            TH1D * histEtaFracL5L7RestrPtFitMeanMap( ( TH1D* )( dirEta_->Get( nameEtaFracL5L7RestrPtFitMeanMap.c_str() ) ) );

            for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
              const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
              const std::string nameEtaPt( nameEta + "_Pt" + binPt );
              const std::string titleLegendEtaPt( titleLegendEta + ", " + boost::lexical_cast< std::string >( ptBins.at( uPt ) ) + " GeV #leq p_{t} < " + boost::lexical_cast< std::string >( ptBins.at( uPt + 1 ) ) + " GeV" );

              const std::string nameEtaPtFracL5L7( nameEtaPt + "_FracL5L7" );
              const std::string nameEtaPtFracL5L7Print( plotPath_ + "/" + evtSel_ + "_" + nameEtaPtFracL5L7 + ".png" );
              TH1D * histEtaPtFracL5L7( ( TH1D* )( dirEta_->Get( std::string( nameEtaPtFracL5L7 + ";" + plotCycle_ ).c_str() ) ) );
              if ( plotNonRestr_ && histEtaPtFracL5L7 != 0 ) {
                TLegend * legEtaPtFracL5L7Fit( new TLegend( 0.1, 0.67, 0.9, 0.9, titleLegendEtaPt.c_str() ) );
                legEtaPtFracL5L7Fit->SetTextSize( 0.03 );
                legEtaPtFracL5L7Fit->SetFillColor( kWhite );
                legEtaPtFracL5L7Fit->SetFillStyle( 0 );
                legEtaPtFracL5L7Fit->SetBorderSize( 0 );
                canv->cd();
                histEtaPtFracL5L7->Draw();
                const std::string nameEtaPtFracL5L7Fit( nameEtaPtFracL5L7 + "_fit" );
                TF1 * fitEtaPtFracL5L7( histEtaPtFracL5L7->GetFunction( nameEtaPtFracL5L7Fit.c_str() ) );
                if ( fitEtaPtFracL5L7 != 0 ) legEtaPtFracL5L7Fit->AddEntry( fitEtaPtFracL5L7, fitFunc.c_str() );
                else                      legEtaPtFracL5L7Fit->AddEntry( histEtaPtFracL5L7, fitFunc.c_str() );
                const std::string nameEtaPtFracL5L7Bkg( nameEtaPtFracL5L7 + "_bkg" );
                TF1 * bkgEtaPtFracL5L7( ( TF1* )( dirEta_->Get( std::string( nameEtaPtFracL5L7Bkg + ";" + plotCycle_ ).c_str() ) ) );
                if ( bkgEtaPtFracL5L7 != 0 ) {
                  bkgEtaPtFracL5L7->SetLineColor( kRed );
                  bkgEtaPtFracL5L7->Draw( "Same" );
                  legEtaPtFracL5L7Fit->AddEntry( bkgEtaPtFracL5L7, bkgFunc.c_str() );
                }
                legEtaPtFracL5L7Fit->Draw();
                canv->Print( nameEtaPtFracL5L7Print.c_str() );
                delete legEtaPtFracL5L7Fit;
              }

              const std::string nameEtaPtFracL5L7Restr( nameEtaPtFracL5L7 + "Restr" );
              const std::string nameEtaPtFracL5L7RestrPrint( plotPath_ + "/" + evtSel_ + "_" + nameEtaPtFracL5L7Restr + ".png" );
              TH1D * histEtaPtFracL5L7Restr( ( TH1D* )( dirEta_->Get( std::string( nameEtaPtFracL5L7Restr + ";" + plotCycle_ ).c_str() ) ) );
              if ( histEtaPtFracL5L7Restr != 0 ) {
                TLegend * legEtaPtFracL5L7RestrFit( new TLegend( 0.1, 0.67, 0.9, 0.9, titleLegendEtaPt.c_str() ) );
                legEtaPtFracL5L7RestrFit->SetTextSize( 0.03 );
                legEtaPtFracL5L7RestrFit->SetFillColor( kWhite );
                legEtaPtFracL5L7RestrFit->SetFillStyle( 0 );
                legEtaPtFracL5L7RestrFit->SetBorderSize( 0 );
                canv->cd();
                histEtaPtFracL5L7Restr->Draw();
                const std::string nameEtaPtFracL5L7RestrFit( nameEtaPtFracL5L7Restr + "_fit" );
                TF1 * fitEtaPtFracL5L7Restr( histEtaPtFracL5L7Restr->GetFunction( nameEtaPtFracL5L7RestrFit.c_str() ) );
                if ( fitEtaPtFracL5L7Restr != 0 ) legEtaPtFracL5L7RestrFit->AddEntry( fitEtaPtFracL5L7Restr, fitFunc.c_str() );
                else                           legEtaPtFracL5L7RestrFit->AddEntry( histEtaPtFracL5L7Restr, fitFunc.c_str() );
                const std::string nameEtaPtFracL5L7RestrBkg( nameEtaPtFracL5L7Restr + "_bkg" );
                TF1 * bkgEtaPtFracL5L7Restr( ( TF1* )( dirEta_->Get( std::string( nameEtaPtFracL5L7RestrBkg + ";" + plotCycle_ ).c_str() ) ) );
                if ( bkgEtaPtFracL5L7Restr != 0 ) {
                  bkgEtaPtFracL5L7Restr->SetLineColor( kRed );
                  bkgEtaPtFracL5L7Restr->Draw( "Same" );
                  legEtaPtFracL5L7RestrFit->AddEntry( bkgEtaPtFracL5L7Restr, bkgFunc.c_str() );
                }
                legEtaPtFracL5L7RestrFit->Draw();
                canv->Print( nameEtaPtFracL5L7RestrPrint.c_str() );
                delete legEtaPtFracL5L7RestrFit;
              }

            } // loop: uPt < nPtBins_

          }

        } // loop: keyEta

      } // loop: keyFit

    }

  } // loop: uCat

  delete canv;

  // Close input files
  for ( unsigned uFile = 0; uFile < files_.size(); ++uFile ) files_.at( uFile )->Close();

  return returnStatus_;

}