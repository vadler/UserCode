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
  const std::string fitFunc( plotCycle_ == "2" ? "Gaussian + log-normal" : "Gaussian" );
  const std::string bkgFunc( plotCycle_ == "2" ? "Log-normal" : "" );


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

        const std::string nameFracL5( name + "_FracL5" );
        const std::string nameFracL5Print( plotPath_ + "/" + evtSel_ + "_" + nameFracL5 + ".png" );
        TH1D * histFracL5( ( TH1D* )( dirFit_->Get( std::string( nameFracL5 + ";" + plotCycle_ ).c_str() ) ) );
        if ( plotNonRestr_ && histFracL5 != 0 ) {
          TLegend * legFracL5Fit( new TLegend( 0.1, 0.67, 0.9, 0.9, titleLegend.c_str() ) );
          legFracL5Fit->SetTextSize( 0.03 );
          legFracL5Fit->SetFillColor( kWhite );
          legFracL5Fit->SetFillStyle( 0 );
          legFracL5Fit->SetBorderSize( 0 );
          canv->cd();
          histFracL5->Draw();
          const std::string nameFracL5Fit( nameFracL5 + "_fit" );
          TF1 * fitFracL5( histFracL5->GetFunction( nameFracL5Fit.c_str() ) );
          if ( fitFracL5 != 0 ) legFracL5Fit->AddEntry( fitFracL5, fitFunc.c_str() );
          else                    legFracL5Fit->AddEntry( histFracL5, fitFunc.c_str() );
          const std::string nameFracL5Bkg( nameFracL5 + "_bkg" );
          TF1 * bkgFracL5(  ( TF1* )( dirFit_->Get( std::string( nameFracL5Bkg + ";" + plotCycle_ ).c_str() ) ));
          if ( bkgFracL5 != 0 ) {
            bkgFracL5->SetLineColor( kRed );
            bkgFracL5->Draw( "Same" );
            legFracL5Fit->AddEntry( bkgFracL5, bkgFunc.c_str() );
          }
          legFracL5Fit->Draw();
          canv->Print( nameFracL5Print.c_str() );
          delete legFracL5Fit;

        }

        const std::string nameFracL5Restr( nameFracL5 + "Restr" );
        const std::string nameFracL5RestrPrint( plotPath_ + "/" + evtSel_ + "_" + nameFracL5Restr + ".png" );
        TH1D * histFracL5Restr( ( TH1D* )( dirFit_->Get( std::string( nameFracL5Restr + ";" + plotCycle_ ).c_str() ) ) );
        std::cout << nameFracL5Restr << " " << histFracL5Restr << std::endl;
        if ( histFracL5Restr != 0 ) {
          TLegend * legFracL5RestrFit( new TLegend( 0.1, 0.67, 0.9, 0.9, titleLegend.c_str() ) );
          legFracL5RestrFit->SetTextSize( 0.03 );
          legFracL5RestrFit->SetFillColor( kWhite );
          legFracL5RestrFit->SetFillStyle( 0 );
          legFracL5RestrFit->SetBorderSize( 0 );
          canv->cd();
          histFracL5Restr->Draw();
          const std::string nameFracL5RestrFit( nameFracL5Restr + "_fit" );
          TF1 * fitFracL5Restr( histFracL5Restr->GetFunction( nameFracL5RestrFit.c_str() ) );
          if ( fitFracL5Restr != 0 ) legFracL5RestrFit->AddEntry( fitFracL5Restr, fitFunc.c_str() );
          else                         legFracL5RestrFit->AddEntry( histFracL5Restr, fitFunc.c_str() );
          const std::string nameFracL5RestrBkg( nameFracL5Restr + "_bkg" );
          TF1 * bkgFracL5Restr( ( TF1* )( dirFit_->Get( std::string( nameFracL5RestrBkg + ";" + plotCycle_ ).c_str() ) ) );
          if ( bkgFracL5Restr != 0 ) {
            bkgFracL5Restr->SetLineColor( kRed );
            bkgFracL5Restr->Draw( "Same" );
            legFracL5RestrFit->AddEntry( bkgFracL5Restr, bkgFunc.c_str() );
          }
          legFracL5RestrFit->Draw();
          canv->Print( nameFracL5RestrPrint.c_str() );
          delete legFracL5RestrFit;
        }

        // Loop over pt bins

        // Fit performance histograms
        if ( plotNonRestr_ ) {
          const std::string nameFracL5PtFitMeanMap( name + "_FracL5Pt_FitMeanMap" );
          const std::string nameFracL5PtFitMeanMapPrint( plotPath_ + "/" + evtSel_ + "_" + nameFracL5PtFitMeanMap + ".png" );
          TH1D * histFracL5PtFitMeanMap( ( TH1D* )( dirFit_->Get( std::string( nameFracL5PtFitMeanMap + ";" + plotCycle_ ).c_str() ) ) );
          if ( histFracL5PtFitMeanMap != 0 ) {
            canv->cd();
            histFracL5PtFitMeanMap->Draw();
            canv->Print( nameFracL5PtFitMeanMapPrint.c_str() );
          }
        }
        const std::string nameFracL5RestrPtFitMeanMap( name + "_FracL5RestrPt_FitMeanMap" );
        const std::string nameFracL5RestrPtFitMeanMapPrint( plotPath_ + "/" + evtSel_ + "_" + nameFracL5RestrPtFitMeanMap + ".png" );
        TH1D * histFracL5RestrPtFitMeanMap( ( TH1D* )( dirFit_->Get( std::string( nameFracL5RestrPtFitMeanMap + ";" + plotCycle_ ).c_str() ) ) );
        if ( histFracL5RestrPtFitMeanMap != 0 ) {
          canv->cd();
          histFracL5RestrPtFitMeanMap->Draw();
          canv->Print( nameFracL5RestrPtFitMeanMapPrint.c_str() );
        }

        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
          const std::string namePt( name + "_Pt" + binPt );
          const std::string titleLegendPt( titleLegend + ", " + boost::lexical_cast< std::string >( ptBins.at( uPt ) ) + " GeV #leq p_{t} < " + boost::lexical_cast< std::string >( ptBins.at( uPt + 1 ) ) + " GeV" );

          const std::string namePtFracL5( namePt + "_FracL5" );
          const std::string namePtFracL5Print( plotPath_ + "/" + evtSel_ + "_" + namePtFracL5 + ".png" );
          TH1D * histPtFracL5( ( TH1D* )( dirFit_->Get( std::string( namePtFracL5 + ";" + plotCycle_ ).c_str() ) ) );
          if ( plotNonRestr_ && histPtFracL5 != 0 ) {
            TLegend * legPtFracL5Fit( new TLegend( 0.1, 0.67, 0.9, 0.9, titleLegendPt.c_str() ) );
            legPtFracL5Fit->SetTextSize( 0.03 );
            legPtFracL5Fit->SetFillColor( kWhite );
            legPtFracL5Fit->SetFillStyle( 0 );
            legPtFracL5Fit->SetBorderSize( 0 );
            canv->cd();
            histPtFracL5->Draw();
            const std::string namePtFracL5Fit( namePtFracL5 + "_fit" );
            TF1 * fitPtFracL5( histPtFracL5->GetFunction( namePtFracL5Fit.c_str() ) );
            if ( fitPtFracL5 != 0 ) legPtFracL5Fit->AddEntry( fitPtFracL5, fitFunc.c_str() );
            else                      legPtFracL5Fit->AddEntry( histPtFracL5, fitFunc.c_str() );
            const std::string namePtFracL5Bkg( namePtFracL5 + "_bkg" );
            TF1 * bkgPtFracL5( ( TF1* )( dirFit_->Get( std::string( namePtFracL5Bkg + ";" + plotCycle_ ).c_str() ) ) );
            if ( bkgPtFracL5 != 0 ) {
              bkgPtFracL5->SetLineColor( kRed );
              bkgPtFracL5->Draw( "Same" );
              legPtFracL5Fit->AddEntry( bkgPtFracL5, bkgFunc.c_str() );
            }
            legPtFracL5Fit->Draw();
            canv->Print( namePtFracL5Print.c_str() );
            delete legPtFracL5Fit;
          }

          const std::string namePtFracL5Restr( namePtFracL5 + "Restr" );
          const std::string namePtFracL5RestrPrint( plotPath_ + "/" + evtSel_ + "_" + namePtFracL5Restr + ".png" );
          TH1D * histPtFracL5Restr( ( TH1D* )( dirFit_->Get( std::string( namePtFracL5Restr + ";" + plotCycle_ ).c_str() ) ) );
          if ( histPtFracL5Restr != 0 ) {
            TLegend * legPtFracL5RestrFit( new TLegend( 0.1, 0.67, 0.9, 0.9, titleLegendPt.c_str() ) );
            legPtFracL5RestrFit->SetTextSize( 0.03 );
            legPtFracL5RestrFit->SetFillColor( kWhite );
            legPtFracL5RestrFit->SetFillStyle( 0 );
            legPtFracL5RestrFit->SetBorderSize( 0 );
            canv->cd();
            histPtFracL5Restr->Draw();
            const std::string namePtFracL5RestrFit( namePtFracL5Restr + "_fit" );
            TF1 * fitPtFracL5Restr( histPtFracL5Restr->GetFunction( namePtFracL5RestrFit.c_str() ) );
            if ( fitPtFracL5Restr != 0 ) legPtFracL5RestrFit->AddEntry( fitPtFracL5Restr, fitFunc.c_str() );
            else                           legPtFracL5RestrFit->AddEntry( histPtFracL5Restr, fitFunc.c_str() );
            const std::string namePtFracL5RestrBkg( namePtFracL5Restr + "_bkg" );
            TF1 * bkgPtFracL5Restr( ( TF1* )( dirFit_->Get( std::string( namePtFracL5RestrBkg + ";" + plotCycle_ ).c_str() ) ) );
            if ( bkgPtFracL5Restr != 0 ) {
              bkgPtFracL5Restr->SetLineColor( kRed );
              bkgPtFracL5Restr->Draw( "Same" );
              legPtFracL5RestrFit->AddEntry( bkgPtFracL5Restr, bkgFunc.c_str() );
            }
            legPtFracL5RestrFit->Draw();
            canv->Print( namePtFracL5RestrPrint.c_str() );
            delete legPtFracL5RestrFit;
          }

        } // loop: uPt < nPtBins_

        // Loop over eta bins

        // Fit performance histograms
        if ( plotNonRestr_ ) {
          const std::string nameFracL5EtaFitMeanMap( name + "_FracL5Eta_FitMeanMap" );
          const std::string nameFracL5EtaFitMeanMapPrint( plotPath_ + "/" + evtSel_ + "_" + nameFracL5EtaFitMeanMap + ".png" );
          TH1D * histFracL5EtaFitMeanMap( ( TH1D* )( dirFit_->Get( std::string( nameFracL5EtaFitMeanMap + ";" + plotCycle_ ).c_str() ) ) );
          if ( histFracL5EtaFitMeanMap != 0 ) {
            canv->cd();
            histFracL5EtaFitMeanMap->Draw();
            canv->Print( nameFracL5EtaFitMeanMapPrint.c_str() );
          }
          const std::string nameFracL5EtaPtFitMeanMap( name + "_FracL5EtaPt_FitMeanMap" );
          TH2D * histFracL5EtaPtFitMeanMap( ( TH2D* )( dirFit_->Get( nameFracL5EtaPtFitMeanMap.c_str() ) ) );
          const std::string nameFracL5EtaPtFitMean( name + "_FracL5EtaPt_FitMean" );
          TH1D * histFracL5EtaPtFitMean( ( TH1D* )( dirFit_->Get( nameFracL5EtaPtFitMean.c_str() ) ) );
          const std::string nameFracL5EtaPtFitSigma( name + "_FracL5EtaPt_FitSigma" );
          TH1D * histFracL5EtaPtFitSigma( ( TH1D* )( dirFit_->Get( nameFracL5EtaPtFitSigma.c_str() ) ) );
        }
        const std::string nameFracL5RestrEtaFitMeanMap( name + "_FracL5RestrEta_FitMeanMap" );
        const std::string nameFracL5RestrEtaFitMeanMapPrint( plotPath_ + "/" + evtSel_ + "_" + nameFracL5RestrEtaFitMeanMap + ".png" );
        TH1D * histFracL5RestrEtaFitMeanMap( ( TH1D* )( dirFit_->Get( std::string( nameFracL5RestrEtaFitMeanMap + ";" + plotCycle_ ).c_str() ) ) );
        if ( histFracL5RestrEtaFitMeanMap != 0 ) {
          canv->cd();
          histFracL5RestrEtaFitMeanMap->Draw();
          canv->Print( nameFracL5RestrEtaFitMeanMapPrint.c_str() );
        }
        const std::string nameFracL5RestrEtaPtFitMeanMap( name + "_FracL5RestrEtaPt_FitMeanMap" );
        TH2D * histFracL5RestrEtaPtFitMeanMap( ( TH2D* )( dirFit_->Get( nameFracL5RestrEtaPtFitMeanMap.c_str() ) ) );
        const std::string nameFracL5RestrEtaPtFitMean( name + "_FracL5RestrEtaPt_FitMean" );
        TH1D * histFracL5RestrEtaPtFitMean( ( TH1D* )( dirFit_->Get( nameFracL5RestrEtaPtFitMean.c_str() ) ) );
        const std::string nameFracL5RestrEtaPtFitSigma( name + "_FracL5RestrEtaPt_FitSigma" );
        TH1D * histFracL5RestrEtaPtFitSigma( ( TH1D* )( dirFit_->Get( nameFracL5RestrEtaPtFitSigma.c_str() ) ) );

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

          const std::string nameEtaFracL5( nameEta + "_FracL5" );
          const std::string nameEtaFracL5Print( plotPath_ + "/" + evtSel_ + "_" + nameEtaFracL5 + ".png" );
          TH1D * histEtaFracL5( ( TH1D* )( dirEta_->Get( std::string( nameEtaFracL5 + ";" + plotCycle_ ).c_str() ) ) );
          if ( plotNonRestr_ && histEtaFracL5 != 0 ) {
            TLegend * legEtaFracL5Fit( new TLegend( 0.1, 0.67, 0.9, 0.9, titleLegendEta.c_str() ) );
            legEtaFracL5Fit->SetTextSize( 0.03 );
            legEtaFracL5Fit->SetFillColor( kWhite );
            legEtaFracL5Fit->SetFillStyle( 0 );
            legEtaFracL5Fit->SetBorderSize( 0 );
            canv->cd();
            histEtaFracL5->Draw();
            const std::string nameEtaFracL5Fit( nameEtaFracL5 + "_fit" );
            TF1 * fitEtaFracL5( histEtaFracL5->GetFunction( nameEtaFracL5Fit.c_str() ) );
            if ( fitEtaFracL5 != 0 ) legEtaFracL5Fit->AddEntry( fitEtaFracL5, fitFunc.c_str() );
            else                      legEtaFracL5Fit->AddEntry( histEtaFracL5, fitFunc.c_str() );
            const std::string nameEtaFracL5Bkg( nameEtaFracL5 + "_bkg" );
            TF1 * bkgEtaFracL5( ( TF1* )( dirEta_->Get( std::string( nameEtaFracL5Bkg + ";" + plotCycle_ ).c_str() ) ) );
            if ( bkgEtaFracL5 != 0 ) {
              bkgEtaFracL5->SetLineColor( kRed );
              bkgEtaFracL5->Draw( "Same" );
              legEtaFracL5Fit->AddEntry( bkgEtaFracL5, bkgFunc.c_str() );
            }
            legEtaFracL5Fit->Draw();
            canv->Print( nameEtaFracL5Print.c_str() );
            delete legEtaFracL5Fit;
          }

          const std::string nameEtaFracL5Restr( nameEtaFracL5 + "Restr" );
          const std::string nameEtaFracL5RestrPrint( plotPath_ + "/" + evtSel_ + "_" + nameEtaFracL5Restr + ".png" );
          TH1D * histEtaFracL5Restr( ( TH1D* )( dirEta_->Get( std::string( nameEtaFracL5Restr + ";" + plotCycle_ ).c_str() ) ) );
          if ( histEtaFracL5Restr != 0 ) {
            TLegend * legEtaFracL5RestrFit( new TLegend( 0.1, 0.67, 0.9, 0.9, titleLegendEta.c_str() ) );
            legEtaFracL5RestrFit->SetTextSize( 0.03 );
            legEtaFracL5RestrFit->SetFillColor( kWhite );
            legEtaFracL5RestrFit->SetFillStyle( 0 );
            legEtaFracL5RestrFit->SetBorderSize( 0 );
            canv->cd();
            histEtaFracL5Restr->Draw();
            const std::string nameEtaFracL5RestrFit( nameEtaFracL5Restr + "_fit" );
            TF1 * fitEtaFracL5Restr( histEtaFracL5Restr->GetFunction( nameEtaFracL5RestrFit.c_str() ) );
            if ( fitEtaFracL5Restr != 0 ) legEtaFracL5RestrFit->AddEntry( fitEtaFracL5Restr, fitFunc.c_str() );
            else                           legEtaFracL5RestrFit->AddEntry( histEtaFracL5Restr, fitFunc.c_str() );
            const std::string nameEtaFracL5RestrBkg( nameEtaFracL5Restr + "_bkg" );
            TF1 * bkgEtaFracL5Restr( ( TF1* )( dirEta_->Get( std::string( nameEtaFracL5RestrBkg + ";" + plotCycle_ ).c_str() ) ) );
            if ( bkgEtaFracL5Restr != 0 ) {
              bkgEtaFracL5Restr->SetLineColor( kRed );
              bkgEtaFracL5Restr->Draw( "Same" );
              legEtaFracL5RestrFit->AddEntry( bkgEtaFracL5Restr, bkgFunc.c_str() );
            }
            legEtaFracL5RestrFit->Draw();
            canv->Print( nameEtaFracL5RestrPrint.c_str() );
            delete legEtaFracL5RestrFit;
          }

          // Loop over pt bins

          if ( plotEtaPt_ ) {

            // Fit performance histograms
            if ( plotNonRestr_ ) {
              const std::string nameEtaFracL5PtFitMeanMap( nameEta + "_FracL5Pt_FitMeanMap" );
              TH1D * histEtaFracL5PtFitMeanMap( ( TH1D* )( dirEta_->Get( nameEtaFracL5PtFitMeanMap.c_str() ) ) );
            }
            const std::string nameEtaFracL5RestrPtFitMeanMap( nameEta + "_FracL5RestrPt_FitMeanMap" );
            TH1D * histEtaFracL5RestrPtFitMeanMap( ( TH1D* )( dirEta_->Get( nameEtaFracL5RestrPtFitMeanMap.c_str() ) ) );

            for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
              const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
              const std::string nameEtaPt( nameEta + "_Pt" + binPt );
              const std::string titleLegendEtaPt( titleLegendEta + ", " + boost::lexical_cast< std::string >( ptBins.at( uPt ) ) + " GeV #leq p_{t} < " + boost::lexical_cast< std::string >( ptBins.at( uPt + 1 ) ) + " GeV" );

              const std::string nameEtaPtFracL5( nameEtaPt + "_FracL5" );
              const std::string nameEtaPtFracL5Print( plotPath_ + "/" + evtSel_ + "_" + nameEtaPtFracL5 + ".png" );
              TH1D * histEtaPtFracL5( ( TH1D* )( dirEta_->Get( std::string( nameEtaPtFracL5 + ";" + plotCycle_ ).c_str() ) ) );
              if ( plotNonRestr_ && histEtaPtFracL5 != 0 ) {
                TLegend * legEtaPtFracL5Fit( new TLegend( 0.1, 0.67, 0.9, 0.9, titleLegendEtaPt.c_str() ) );
                legEtaPtFracL5Fit->SetTextSize( 0.03 );
                legEtaPtFracL5Fit->SetFillColor( kWhite );
                legEtaPtFracL5Fit->SetFillStyle( 0 );
                legEtaPtFracL5Fit->SetBorderSize( 0 );
                canv->cd();
                histEtaPtFracL5->Draw();
                const std::string nameEtaPtFracL5Fit( nameEtaPtFracL5 + "_fit" );
                TF1 * fitEtaPtFracL5( histEtaPtFracL5->GetFunction( nameEtaPtFracL5Fit.c_str() ) );
                if ( fitEtaPtFracL5 != 0 ) legEtaPtFracL5Fit->AddEntry( fitEtaPtFracL5, fitFunc.c_str() );
                else                      legEtaPtFracL5Fit->AddEntry( histEtaPtFracL5, fitFunc.c_str() );
                const std::string nameEtaPtFracL5Bkg( nameEtaPtFracL5 + "_bkg" );
                TF1 * bkgEtaPtFracL5( ( TF1* )( dirEta_->Get( std::string( nameEtaPtFracL5Bkg + ";" + plotCycle_ ).c_str() ) ) );
                if ( bkgEtaPtFracL5 != 0 ) {
                  bkgEtaPtFracL5->SetLineColor( kRed );
                  bkgEtaPtFracL5->Draw( "Same" );
                  legEtaPtFracL5Fit->AddEntry( bkgEtaPtFracL5, bkgFunc.c_str() );
                }
                legEtaPtFracL5Fit->Draw();
                canv->Print( nameEtaPtFracL5Print.c_str() );
                delete legEtaPtFracL5Fit;
              }

              const std::string nameEtaPtFracL5Restr( nameEtaPtFracL5 + "Restr" );
              const std::string nameEtaPtFracL5RestrPrint( plotPath_ + "/" + evtSel_ + "_" + nameEtaPtFracL5Restr + ".png" );
              TH1D * histEtaPtFracL5Restr( ( TH1D* )( dirEta_->Get( std::string( nameEtaPtFracL5Restr + ";" + plotCycle_ ).c_str() ) ) );
              if ( histEtaPtFracL5Restr != 0 ) {
                TLegend * legEtaPtFracL5RestrFit( new TLegend( 0.1, 0.67, 0.9, 0.9, titleLegendEtaPt.c_str() ) );
                legEtaPtFracL5RestrFit->SetTextSize( 0.03 );
                legEtaPtFracL5RestrFit->SetFillColor( kWhite );
                legEtaPtFracL5RestrFit->SetFillStyle( 0 );
                legEtaPtFracL5RestrFit->SetBorderSize( 0 );
                canv->cd();
                histEtaPtFracL5Restr->Draw();
                const std::string nameEtaPtFracL5RestrFit( nameEtaPtFracL5Restr + "_fit" );
                TF1 * fitEtaPtFracL5Restr( histEtaPtFracL5Restr->GetFunction( nameEtaPtFracL5RestrFit.c_str() ) );
                if ( fitEtaPtFracL5Restr != 0 ) legEtaPtFracL5RestrFit->AddEntry( fitEtaPtFracL5Restr, fitFunc.c_str() );
                else                           legEtaPtFracL5RestrFit->AddEntry( histEtaPtFracL5Restr, fitFunc.c_str() );
                const std::string nameEtaPtFracL5RestrBkg( nameEtaPtFracL5Restr + "_bkg" );
                TF1 * bkgEtaPtFracL5Restr( ( TF1* )( dirEta_->Get( std::string( nameEtaPtFracL5RestrBkg + ";" + plotCycle_ ).c_str() ) ) );
                if ( bkgEtaPtFracL5Restr != 0 ) {
                  bkgEtaPtFracL5Restr->SetLineColor( kRed );
                  bkgEtaPtFracL5Restr->Draw( "Same" );
                  legEtaPtFracL5RestrFit->AddEntry( bkgEtaPtFracL5Restr, bkgFunc.c_str() );
                }
                legEtaPtFracL5RestrFit->Draw();
                canv->Print( nameEtaPtFracL5RestrPrint.c_str() );
                delete legEtaPtFracL5RestrFit;
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