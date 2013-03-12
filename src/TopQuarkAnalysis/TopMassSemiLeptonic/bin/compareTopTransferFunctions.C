// #include <cstdlib>
#include <cmath>
#include <cassert>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>

#include "boost/lexical_cast.hpp"

#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TFile.h>
#include <TKey.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TF2.h>
#include <TMath.h>
#include <TCanvas.h>

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include "CommonTools/MyTools/interface/RootTools.h"
#include "CommonTools/MyTools/interface/RootFunctions.h"


// Compute R2
void computeR2( TH1D * histo, TF1 * function, double & R2, double & adjustedR2 );
// Weight R2
void weightR2( const bool useNorm, int nPar, int nParMax, double & R2, double & adjustedR2 );


int main( int argc, char * argv[] )
{

  int returnStatus_( 0 );

  // Set up ROOT
  my::setPlotEnvironment( gStyle );
  gStyle->SetOptStat( 111111 );
  gStyle->SetOptFit( 1111 );

  // Check configuration file
  if ( argc < 2 ) {
    std::cout << argv[ 0 ] << " --> Usage:" << std::endl
              << "    " << argv[ 0 ] << " [CONFIG_FILE.py]" << std::endl;
    returnStatus_ += 0x1;
    return returnStatus_;
  }
  if ( ! edm::readPSetsFrom( argv[ 1 ] )->existsAs< edm::ParameterSet >( "process" ) ) {
    std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
              << "    cms.PSet 'process' missing in " << argv[ 1 ] << std::endl;
    returnStatus_ += 0x2;
    return returnStatus_;
  }

  // Get the configurations
  const edm::ParameterSet & process_( edm::readPSetsFrom( argv[ 1 ] )->getParameter< edm::ParameterSet >( "process" ) );
  const unsigned verbose_( process_.getParameter< unsigned >( "verbose" ) );
  const std::vector< std::string > objCats_( process_.getParameter< std::vector< std::string > >( "objectCategories" ) );   // object categories
  const bool useAlt_( process_.getParameter< bool >( "useAlt" ) );
  const bool useNonT_( process_.getParameter< bool >( "useNonT" ) );
  const bool useSymm_( process_.getParameter< bool >( "useSymm" ) );
  const bool refGen_( process_.getParameter< bool >( "refGen" ) );
  const bool refSel_( process_.getParameter< bool >( "refSel" ) );
  // Configuration for in- & output
  const edm::ParameterSet & io_( process_.getParameter< edm::ParameterSet >( "io" ) );
  const std::string inFile_( io_.getParameter< std::string >( "inputFile" ) );
  const bool overwrite_( io_.getParameter< bool >( "overwrite" ));
  const std::string pathPlots_( io_.getParameter< std::string >( "pathPlots" ) );
  const bool plot_( ! pathPlots_.empty() );
  // Configuration for fitting transfer functions
  const edm::ParameterSet & fit_( process_.getParameter< edm::ParameterSet >( "fit" ) );
//   const bool fitNonRestr_( fit_.getParameter< bool >( "fitNonRestr" ) );
//   const bool fitEtaBins_( fit_.getParameter< bool >( "fitEtaBins" ) );
  const bool useNorm_( fit_.getParameter< bool >( "useNorm" ) );
  const int  norm_( fit_.getParameter< int >( "norm" ) );
  const bool weightR2_( fit_.getParameter< bool >( "weightR2" ) );

  if ( verbose_ > 0 ) {
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "    called for object categories ";
    for ( unsigned uCat = 0; uCat < objCats_.size() - 1; ++uCat ) std::cout << "'" << objCats_.at( uCat ) << "', ";
    std::cout << "'" << objCats_.back() << "'" << std::endl;
  }

  // Set constants
  std::string evtSel_( "analyzeHitFit" );
  if ( refSel_ ) evtSel_.append( "Reference" );
  const std::string nameDirClass( "TDirectoryFile" );
  const std::string nameFuncClass( "TF1" );
  const std::string baseTitlePtL( useAlt_ ? "E" : "p" );
  const std::string baseTitlePtU( useAlt_ ? "E" : "P" );
  const std::string baseTitlePt( useNonT_ ? baseTitlePtU : baseTitlePtU + "t" );
  const std::string titleCycles( "Cycle" );
  const std::string titlePars( "Parameter" );
  const std::string titleChi2( "#chi^{2}" );
  const std::string titleReducedChi2( "#chi^{2} / ndf" );
  const std::string titleProb( "P" );
  const std::string titleLogProb( "log_{10}(P)" );
  const std::string titleR2( "R^{2}" );
  const std::string titleAdjustedR2( "R^{2}_{adj.}" );


  // Open input file

  TFile * fileIn_( TFile::Open( inFile_.c_str(), "UPDATE" ) );
  if ( ! fileIn_ ) {
    std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
              << "    input file '" << inFile_ << "' missing" << std::endl;
    returnStatus_ += 0x10;
    return returnStatus_;
  }
  TDirectory * dirSel_ = ( TDirectory* )( fileIn_->Get( evtSel_.c_str() ) );
  if ( ! dirSel_ ) {
    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
              << "    selection '" << evtSel_ << "' does not exist in input file" << std::endl;
    returnStatus_ += 0x20;
    return returnStatus_;
  }
  if ( verbose_ > 1 ) gDirectory->pwd();

  TCanvas c1( "c1" );
  c1.cd();
  TH1D::SetDefaultSumw2();
  TH2D::SetDefaultSumw2();

  // Loops through directory structure

  typedef std::vector< std::vector< Double_t > > DataCont;

  // Loop over configured object categories
  for ( unsigned uCat = 0; uCat < objCats_.size(); ++uCat ) {
    const std::string objCat( objCats_.at( uCat ) );
    TDirectory * dirCat_( ( TDirectory* )( dirSel_->Get( objCat.c_str() ) ) );
    if ( ! dirCat_ ) {
      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                << "    object category '" << objCat << "' does not exist in input file" << std::endl;
      returnStatus_ += 0x100;
      continue;
    }
    if ( verbose_ > 1 ) gDirectory->pwd();

    TDirectory * dirPt_( ( TDirectory* )( dirCat_->Get( "Pt" ) ) );
    if ( ! dirPt_ ) {
      std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                << "    kinematic property 'Pt' does not exist in input file" << std::endl;
      returnStatus_ += 0x1000;
      continue;
    }
    if ( verbose_ > 1 ) gDirectory->pwd();

    // Compare transfer functions

    if ( verbose_ > 1 ) {
      std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                << "    transfer function comparison for " << objCat << " started" << std::endl;
    }

    // Loop over fit versions
    TList * listProp( dirPt_->GetListOfKeys() );
    if ( verbose_ > 3 ) listProp->Print();
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
      if ( ! dirFit_ ) {
        std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                  << "    fit '" << subFit << "' does not exist in output file" << std::endl;
        returnStatus_ += 0x100000;
        continue;
      }
      dirFit_->cd();

      const std::string name( objCat + "_" + baseTitlePt + "_" + subFit );

      const std::string nameTrans( name + "_Trans" );
      const std::string nameTransRebin( nameTrans + "Rebin" );
      const std::string nameTransRestr( nameTrans + "Restr" );
      const std::string nameTransRestrRebin( nameTransRestr + "Rebin" ); // Base histogram to identify cycles

      // Determine cycles

      int nCycles( 0 );
      std::vector< int > iVecNPar;
      int nParMax( 0 );
      std::string sCycle( "1" );
      std::string nameTransRestrRebinCycle( nameTransRestrRebin + ";" + sCycle );
      TH1D * histTransRestrRebinCycle( ( TH1D* )( dirFit_->Get( nameTransRestrRebinCycle.c_str() ) ) );

      while ( histTransRestrRebinCycle != 0 ) {
        const std::string nameTransRestrRebinFit( nameTransRestrRebin + "_fit" );
        TF1 * fitTransRestrRebin( histTransRestrRebinCycle->GetFunction( nameTransRestrRebinFit.c_str() ) );
        const int nPar( fitTransRestrRebin->GetNpar() );
        iVecNPar.push_back( nPar );
        if ( nPar > nParMax ) nParMax = nPar;
        if ( verbose_ > 0 ) std::cout << argv[ 0 ] << " --> CYCLE: " << sCycle << std::endl;
        if ( verbose_ > 2 ) {
          std::cout << "    fit parameters: " << nPar << std::endl;
        }

        ++nCycles;
        sCycle = boost::lexical_cast< std::string >( nCycles + 1 );
        nameTransRestrRebinCycle = nameTransRestrRebin + ";" + sCycle;
        histTransRestrRebinCycle = ( TH1D* )( dirFit_->Get( nameTransRestrRebinCycle.c_str() ) );
      } // histTransRestrRebinCycle

      assert( nCycles == ( int )iVecNPar.size() );
      if ( nCycles == 0 ) {
        std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                  << "    fit '" << subFit << "' does not exist in output file" << std::endl;
        returnStatus_ += 0x1000000;
        return returnStatus_;
      }

      // Determine parameter cycles

      std::vector<std::vector< int > > iVecVecParCycle;
      for ( int iPar = 0; iPar < nParMax; ++iPar ) {
        std::vector< int > iVecPar;
        int parCycle( 0 );
        for ( int iCycle = 1; iCycle <= nCycles; ++iCycle ) {
          if ( iPar < iVecNPar.at( iCycle - 1 ) ) {
            ++parCycle;
            iVecPar.push_back( parCycle );
          }
          else {
            iVecPar.push_back( 0 );
          }
        }
        iVecVecParCycle.push_back( iVecPar );
      }

      // Loop over cycles

      // Create histograms
      const std::string nameTransRestrRebinChi2( nameTransRestrRebin + "_Chi2" );
      TH1D * histTransRestrRebinChi2( new TH1D( nameTransRestrRebinChi2.c_str(), objCat.c_str(), nCycles, 0.5, nCycles + 0.5 ) );
      histTransRestrRebinChi2->SetXTitle( titleCycles.c_str() );
      histTransRestrRebinChi2->SetYTitle( titleChi2.c_str() );
      const std::string nameTransRestrRebinReducedChi2( nameTransRestrRebin + "_ReducedChi2" );
      TH1D * histTransRestrRebinReducedChi2( new TH1D( nameTransRestrRebinReducedChi2.c_str(), objCat.c_str(), nCycles, 0.5, nCycles + 0.5 ) );
      histTransRestrRebinReducedChi2->SetXTitle( titleCycles.c_str() );
      histTransRestrRebinReducedChi2->SetYTitle( titleReducedChi2.c_str() );
      const std::string nameTransRestrRebinProb( nameTransRestrRebin + "_Prob" );
      TH1D * histTransRestrRebinProb( new TH1D( nameTransRestrRebinProb.c_str(), objCat.c_str(), nCycles, 0.5, nCycles + 0.5 ) );
      histTransRestrRebinProb->SetXTitle( titleCycles.c_str() );
      histTransRestrRebinProb->SetYTitle( titleProb.c_str() );
      const std::string nameTransRestrRebinLogProb( nameTransRestrRebin + "_LogProb" );
      TH1D * histTransRestrRebinLogProb( new TH1D( nameTransRestrRebinLogProb.c_str(), objCat.c_str(), nCycles, 0.5, nCycles + 0.5 ) );
      histTransRestrRebinLogProb->SetXTitle( titleCycles.c_str() );
      histTransRestrRebinLogProb->SetYTitle( titleLogProb.c_str() );
      const std::string nameTransRestrRebinR2( nameTransRestrRebin + "_R2" );
      TH1D * histTransRestrRebinR2( new TH1D( nameTransRestrRebinR2.c_str(), objCat.c_str(), nCycles, 0.5, nCycles + 0.5 ) );
      histTransRestrRebinR2->SetXTitle( titleCycles.c_str() );
      histTransRestrRebinR2->SetYTitle( titleR2.c_str() );
      histTransRestrRebinR2->GetYaxis()->SetRangeUser( 0., 1. );
      const std::string nameTransRestrRebinAdjustedR2( nameTransRestrRebin + "_AdjustedR2" );
      TH1D * histTransRestrRebinAdjustedR2( new TH1D( nameTransRestrRebinAdjustedR2.c_str(), objCat.c_str(), nCycles, 0.5, nCycles + 0.5 ) );
      histTransRestrRebinAdjustedR2->SetXTitle( titleCycles.c_str() );
      histTransRestrRebinAdjustedR2->SetYTitle( titleAdjustedR2.c_str() );
      histTransRestrRebinAdjustedR2->GetYaxis()->SetRangeUser( 0., 1. );

      for ( int iCycle = 1; iCycle <= nCycles; ++iCycle ) {
        const std::string sCycle = boost::lexical_cast< std::string >( iCycle );
        const std::string nameTransRestrRebinCycle = nameTransRestrRebin + ";" + sCycle;
        histTransRestrRebinCycle = ( TH1D* )( dirFit_->Get( nameTransRestrRebinCycle.c_str() ) );
        const std::string nameTransRestrRebinFit( nameTransRestrRebin + "_fit" );
        TF1 * fitTransRestrRebin( histTransRestrRebinCycle->GetFunction( nameTransRestrRebinFit.c_str() ) );

        // Chi2
        histTransRestrRebinChi2->SetBinContent( iCycle, fitTransRestrRebin->GetChisquare() );
        histTransRestrRebinReducedChi2->SetBinContent( iCycle, fitTransRestrRebin->GetChisquare() / fitTransRestrRebin->GetNDF() );
        histTransRestrRebinProb->SetBinContent( iCycle, fitTransRestrRebin->GetProb() );
        if ( fitTransRestrRebin->GetProb() > 0. ) {
          histTransRestrRebinLogProb->SetBinContent( iCycle, std::log10( fitTransRestrRebin->GetProb() ) );
        }
        else {
          histTransRestrRebinLogProb->SetBinContent( iCycle, 0. );
        }

        // R2
        double fitTransRestrRebinR2;
        double fitTransRestrRebinAdjustedR2;
        computeR2( histTransRestrRebinCycle, fitTransRestrRebin, fitTransRestrRebinR2, fitTransRestrRebinAdjustedR2 );
        histTransRestrRebinR2->SetBinContent( iCycle, fitTransRestrRebinR2 );
        if ( fitTransRestrRebinR2 == 1. ) histTransRestrRebinR2->SetBinError( iCycle, fitTransRestrRebinR2 );
        histTransRestrRebinAdjustedR2->SetBinContent( iCycle, fitTransRestrRebinAdjustedR2 );
        if ( fitTransRestrRebinAdjustedR2 == 1. ) histTransRestrRebinAdjustedR2->SetBinError( iCycle, fitTransRestrRebinAdjustedR2 );
      } // iCycle

      if ( plot_ ) {
        histTransRestrRebinChi2->Draw();
        c1.Print( std::string( pathPlots_ + histTransRestrRebinChi2->GetName() + ".png" ).c_str() );
        histTransRestrRebinReducedChi2->Draw();
        c1.Print( std::string( pathPlots_ + histTransRestrRebinReducedChi2->GetName() + ".png" ).c_str() );
        histTransRestrRebinProb->Draw();
        c1.Print( std::string( pathPlots_ + histTransRestrRebinProb->GetName() + ".png" ).c_str() );
        histTransRestrRebinLogProb->Draw();
        c1.Print( std::string( pathPlots_ + histTransRestrRebinLogProb->GetName() + ".png" ).c_str() );
        histTransRestrRebinR2->Draw();
        c1.Print( std::string( pathPlots_ + histTransRestrRebinR2->GetName() + ".png" ).c_str() );
        histTransRestrRebinAdjustedR2->Draw();
        c1.Print( std::string( pathPlots_ + histTransRestrRebinAdjustedR2->GetName() + ".png" ).c_str() );
      }

      // Loop over parameters

      // Create histograms and cache vectors
      const std::string nameTransRestrRebinPtFitMapProd( nameTransRestrRebin + baseTitlePt + "_FitMap_Prod" );
      const std::string nameTransRestrRebinPtFitMapProdChi2( nameTransRestrRebinPtFitMapProd + "_Chi2" );
      TH1D * histTransRestrRebinPtFitMapProdChi2( new TH1D( nameTransRestrRebinPtFitMapProdChi2.c_str(), objCat.c_str(), nCycles, 0.5, nCycles + 0.5 ) );
      histTransRestrRebinPtFitMapProdChi2->SetXTitle( titlePars.c_str() );
      histTransRestrRebinPtFitMapProdChi2->SetYTitle( titleChi2.c_str() );
      const std::string nameTransRestrRebinPtFitMapProdReducedChi2( nameTransRestrRebinPtFitMapProd + "_ReducedChi2" );
      TH1D * histTransRestrRebinPtFitMapProdReducedChi2( new TH1D( nameTransRestrRebinPtFitMapProdReducedChi2.c_str(), objCat.c_str(), nCycles, 0.5, nCycles + 0.5 ) );
      histTransRestrRebinPtFitMapProdReducedChi2->SetXTitle( titlePars.c_str() );
      histTransRestrRebinPtFitMapProdReducedChi2->SetYTitle( titleReducedChi2.c_str() );
      const std::string nameTransRestrRebinPtFitMapProdProb( nameTransRestrRebinPtFitMapProd + "_Prob" );
      TH1D * histTransRestrRebinPtFitMapProdProb( new TH1D( nameTransRestrRebinPtFitMapProdProb.c_str(), objCat.c_str(), nCycles, 0.5, nCycles + 0.5 ) );
      histTransRestrRebinPtFitMapProdProb->SetXTitle( titlePars.c_str() );
      histTransRestrRebinPtFitMapProdProb->SetYTitle( titleProb.c_str() );
      const std::string nameTransRestrRebinPtFitMapProdLogProb( nameTransRestrRebinPtFitMapProd + "_LogProb" );
      TH1D * histTransRestrRebinPtFitMapProdLogProb( new TH1D( nameTransRestrRebinPtFitMapProdLogProb.c_str(), objCat.c_str(), nCycles, 0.5, nCycles + 0.5 ) );
      histTransRestrRebinPtFitMapProdLogProb->SetXTitle( titlePars.c_str() );
      histTransRestrRebinPtFitMapProdLogProb->SetYTitle( titleLogProb.c_str() );
      const std::string nameTransRestrRebinPtFitMapProdR2( nameTransRestrRebinPtFitMapProd + "_R2" );
      TH1D * histTransRestrRebinPtFitMapProdR2( new TH1D( nameTransRestrRebinPtFitMapProdR2.c_str(), objCat.c_str(), nCycles, 0.5, nCycles + 0.5 ) );
      histTransRestrRebinPtFitMapProdR2->SetXTitle( titlePars.c_str() );
      histTransRestrRebinPtFitMapProdR2->SetYTitle( titleR2.c_str() );
      histTransRestrRebinPtFitMapProdR2->GetYaxis()->SetRangeUser( 0., 1. );
      const std::string nameTransRestrRebinPtFitMapProdAdjustedR2( nameTransRestrRebinPtFitMapProd + "_AdjustedR2" );
      TH1D * histTransRestrRebinPtFitMapProdAdjustedR2( new TH1D( nameTransRestrRebinPtFitMapProdAdjustedR2.c_str(), objCat.c_str(), nCycles, 0.5, nCycles + 0.5 ) );
      histTransRestrRebinPtFitMapProdAdjustedR2->SetXTitle( titlePars.c_str() );
      histTransRestrRebinPtFitMapProdAdjustedR2->SetYTitle( titleAdjustedR2.c_str() );
      histTransRestrRebinPtFitMapProdAdjustedR2->GetYaxis()->SetRangeUser( 0., 1. );
      std::vector< double > dVecTransRestrRebinPtFitMapProdChi2( nCycles );
      std::vector< double > dVecTransRestrRebinPtFitMapProdReducedChi2( nCycles );
      std::vector< double > dVecTransRestrRebinPtFitMapProdProb( nCycles );
      std::vector< double > dVecTransRestrRebinPtFitMapProdLogProb( nCycles );
      std::vector< double > dVecTransRestrRebinPtFitMapProdR2( nCycles );
      std::vector< double > dVecTransRestrRebinPtFitMapProdAdjustedR2( nCycles );

      for ( int iPar = 0; iPar < nParMax; ++iPar ) {
        const std::string parFit( boost::lexical_cast< std::string >( iPar ) );

        // Create histograms
        const std::string nameTransRestrRebinPtFitMap( nameTransRestrRebin + baseTitlePt + "_FitMap_Par" + parFit );
        const std::string titleTransRestrRebinPtFitMap( objCat + ", par. " + parFit );
        const std::string nameTransRestrRebinPtFitMapChi2( nameTransRestrRebinPtFitMap + "_Chi2" );
        TH1D * histTransRestrRebinPtFitMapChi2( new TH1D( nameTransRestrRebinPtFitMapChi2.c_str(), titleTransRestrRebinPtFitMap.c_str(), nCycles, 0.5, nCycles + 0.5 ) );
        histTransRestrRebinPtFitMapChi2->SetXTitle( titleCycles.c_str() );
        histTransRestrRebinPtFitMapChi2->SetYTitle( titleChi2.c_str() );
        const std::string nameTransRestrRebinPtFitMapReducedChi2( nameTransRestrRebinPtFitMap + "_ReducedChi2" );
        TH1D * histTransRestrRebinPtFitMapReducedChi2( new TH1D( nameTransRestrRebinPtFitMapReducedChi2.c_str(), titleTransRestrRebinPtFitMap.c_str(), nCycles, 0.5, nCycles + 0.5 ) );
        histTransRestrRebinPtFitMapReducedChi2->SetXTitle( titleCycles.c_str() );
        histTransRestrRebinPtFitMapReducedChi2->SetYTitle( titleReducedChi2.c_str() );
        const std::string nameTransRestrRebinPtFitMapProb( nameTransRestrRebinPtFitMap + "_Prob" );
        TH1D * histTransRestrRebinPtFitMapProb( new TH1D( nameTransRestrRebinPtFitMapProb.c_str(), titleTransRestrRebinPtFitMap.c_str(), nCycles, 0.5, nCycles + 0.5 ) );
        histTransRestrRebinPtFitMapProb->SetXTitle( titleCycles.c_str() );
        histTransRestrRebinPtFitMapProb->SetYTitle( titleProb.c_str() );
        const std::string nameTransRestrRebinPtFitMapLogProb( nameTransRestrRebinPtFitMap + "_LogProb" );
        TH1D * histTransRestrRebinPtFitMapLogProb( new TH1D( nameTransRestrRebinPtFitMapLogProb.c_str(), titleTransRestrRebinPtFitMap.c_str(), nCycles, 0.5, nCycles + 0.5 ) );
        histTransRestrRebinPtFitMapLogProb->SetXTitle( titleCycles.c_str() );
        histTransRestrRebinPtFitMapLogProb->SetYTitle( titleLogProb.c_str() );
        const std::string nameTransRestrRebinPtFitMapR2( nameTransRestrRebinPtFitMap + "_R2" );
        TH1D * histTransRestrRebinPtFitMapR2( new TH1D( nameTransRestrRebinPtFitMapR2.c_str(), titleTransRestrRebinPtFitMap.c_str(), nCycles, 0.5, nCycles + 0.5 ) );
        histTransRestrRebinPtFitMapR2->SetXTitle( titleCycles.c_str() );
        histTransRestrRebinPtFitMapR2->SetYTitle( titleR2.c_str() );
        histTransRestrRebinPtFitMapR2->GetYaxis()->SetRangeUser( 0., 1. );
        const std::string nameTransRestrRebinPtFitMapAdjustedR2( nameTransRestrRebinPtFitMap + "_AdjustedR2" );
        TH1D * histTransRestrRebinPtFitMapAdjustedR2( new TH1D( nameTransRestrRebinPtFitMapAdjustedR2.c_str(), titleTransRestrRebinPtFitMap.c_str(), nCycles, 0.5, nCycles + 0.5 ) );
        histTransRestrRebinPtFitMapAdjustedR2->SetXTitle( titleCycles.c_str() );
        histTransRestrRebinPtFitMapAdjustedR2->SetYTitle( titleAdjustedR2.c_str() );
        histTransRestrRebinPtFitMapAdjustedR2->GetYaxis()->SetRangeUser( 0., 1. );

        for ( int iCycle = 1; iCycle <= nCycles; ++iCycle ) {
          if ( iPar == 0 ) {
            dVecTransRestrRebinPtFitMapProdChi2.at( iCycle - 1 )        = 1.;
            dVecTransRestrRebinPtFitMapProdReducedChi2.at( iCycle - 1 ) = 1.;
            dVecTransRestrRebinPtFitMapProdProb.at( iCycle - 1 )        = 1.;
            dVecTransRestrRebinPtFitMapProdLogProb.at( iCycle - 1 )     = 1.;
            dVecTransRestrRebinPtFitMapProdR2.at( iCycle - 1 )          = 1.;
            dVecTransRestrRebinPtFitMapProdAdjustedR2.at( iCycle - 1 )  = 1.;
          }
          if ( iVecVecParCycle.at( iPar ).at( iCycle - 1 ) == 0 ) {
            histTransRestrRebinPtFitMapChi2->SetBinContent( iCycle, 0. );
            histTransRestrRebinPtFitMapReducedChi2->SetBinContent( iCycle, 0. );
            histTransRestrRebinPtFitMapProb->SetBinContent( iCycle, 0. );
            histTransRestrRebinPtFitMapLogProb->SetBinContent( iCycle, 0. );
            histTransRestrRebinPtFitMapR2->SetBinContent( iCycle, 0. );
            histTransRestrRebinPtFitMapAdjustedR2->SetBinContent( iCycle, 0. );
            continue;
          }

          const std::string sCycle = boost::lexical_cast< std::string >( iVecVecParCycle.at( iPar ).at( iCycle - 1 ) );
          const std::string nameTransRestrRebinPtFitMapCycle = nameTransRestrRebinPtFitMap + ";" + sCycle;
          TH1D * histTransRestrRebinPtFitMapCycle = ( TH1D* )( dirFit_->Get( nameTransRestrRebinPtFitMapCycle.c_str() ) );
          const std::string nameTransRestrRebinPtFitMapFit( nameTransRestrRebinPtFitMap + "_fit" );
          TF1 * fitTransRestrRebinPtFitMap( histTransRestrRebinPtFitMapCycle->GetFunction( nameTransRestrRebinPtFitMapFit.c_str() ) );

          // Chi2
          histTransRestrRebinPtFitMapChi2->SetBinContent( iCycle, fitTransRestrRebinPtFitMap->GetChisquare() );
          if ( ! useNorm_ || iPar != norm_ ) dVecTransRestrRebinPtFitMapProdChi2.at( iCycle - 1 ) *= fitTransRestrRebinPtFitMap->GetChisquare();
          if ( fitTransRestrRebinPtFitMap->GetNDF() != 0 ) {
            histTransRestrRebinPtFitMapReducedChi2->SetBinContent( iCycle, fitTransRestrRebinPtFitMap->GetChisquare() / fitTransRestrRebinPtFitMap->GetNDF() );
            if ( ! useNorm_ || iPar != norm_ ) dVecTransRestrRebinPtFitMapProdReducedChi2.at( iCycle - 1 ) *= fitTransRestrRebinPtFitMap->GetChisquare() / fitTransRestrRebinPtFitMap->GetNDF();
          }
          else {
            histTransRestrRebinPtFitMapReducedChi2->SetBinContent( iCycle, 0. );
            if ( ! useNorm_ || iPar != norm_ ) dVecTransRestrRebinPtFitMapProdReducedChi2.at( iCycle - 1 ) = 0.;
          }
          histTransRestrRebinPtFitMapProb->SetBinContent( iCycle, fitTransRestrRebinPtFitMap->GetProb() );
          if ( ! useNorm_ || iPar != norm_ ) dVecTransRestrRebinPtFitMapProdProb.at( iCycle - 1 ) *= fitTransRestrRebinPtFitMap->GetProb();
          if ( fitTransRestrRebinPtFitMap->GetProb() > 0. ) {
            histTransRestrRebinPtFitMapLogProb->SetBinContent( iCycle, std::log10( fitTransRestrRebinPtFitMap->GetProb() ) );
            if ( ! useNorm_ || iPar != norm_ ) dVecTransRestrRebinPtFitMapProdLogProb.at( iCycle - 1 ) *= std::log10( fitTransRestrRebinPtFitMap->GetProb() );
          }
          else {
            histTransRestrRebinPtFitMapLogProb->SetBinContent( iCycle, 0. );
            if ( ! useNorm_ || iPar != norm_ ) dVecTransRestrRebinPtFitMapProdLogProb.at( iCycle - 1 ) = 0.;
          }

          // R2
          double fitTransRestrRebinPtFitMapR2;
          double fitTransRestrRebinPtFitMapAdjustedR2;
          computeR2( histTransRestrRebinPtFitMapCycle, fitTransRestrRebinPtFitMap, fitTransRestrRebinPtFitMapR2, fitTransRestrRebinPtFitMapAdjustedR2 );
          histTransRestrRebinPtFitMapR2->SetBinContent( iCycle, fitTransRestrRebinPtFitMapR2 );
          histTransRestrRebinPtFitMapAdjustedR2->SetBinContent( iCycle, fitTransRestrRebinPtFitMapAdjustedR2 );
          if ( weightR2_ ) weightR2( useNorm_, iVecNPar.at( iCycle - 1 ), nParMax, fitTransRestrRebinPtFitMapR2, fitTransRestrRebinPtFitMapAdjustedR2 );
          if ( useNorm_ || iPar != norm_ ) {
            dVecTransRestrRebinPtFitMapProdR2.at( iCycle - 1 ) *= std::fabs( fitTransRestrRebinPtFitMapR2 );
            if ( fitTransRestrRebinPtFitMapR2 < 0. ) {
              dVecTransRestrRebinPtFitMapProdR2.at( iCycle - 1 ) = -std::fabs( dVecTransRestrRebinPtFitMapProdR2.at( iCycle - 1 ) );
            }
          }
          if ( useNorm_ || iPar != norm_ ) {
            dVecTransRestrRebinPtFitMapProdAdjustedR2.at( iCycle - 1 ) *= std::fabs( fitTransRestrRebinPtFitMapAdjustedR2 );
            if ( fitTransRestrRebinPtFitMapAdjustedR2 < 0. ) {
              dVecTransRestrRebinPtFitMapProdAdjustedR2.at( iCycle - 1 ) = -std::fabs( dVecTransRestrRebinPtFitMapProdAdjustedR2.at( iCycle - 1 ) );
            }
          }

          delete histTransRestrRebinPtFitMapCycle;
        } // iCycle

        if ( plot_ ) {
          histTransRestrRebinPtFitMapChi2->Draw();
          c1.Print( std::string( pathPlots_ + histTransRestrRebinPtFitMapChi2->GetName() + ".png" ).c_str() );
          histTransRestrRebinPtFitMapReducedChi2->Draw();
          c1.Print( std::string( pathPlots_ + histTransRestrRebinPtFitMapReducedChi2->GetName() + ".png" ).c_str() );
          histTransRestrRebinPtFitMapProb->Draw();
          c1.Print( std::string( pathPlots_ + histTransRestrRebinPtFitMapProb->GetName() + ".png" ).c_str() );
          histTransRestrRebinPtFitMapLogProb->Draw();
          c1.Print( std::string( pathPlots_ + histTransRestrRebinPtFitMapLogProb->GetName() + ".png" ).c_str() );
          histTransRestrRebinPtFitMapR2->Draw();
          c1.Print( std::string( pathPlots_ + histTransRestrRebinPtFitMapR2->GetName() + ".png" ).c_str() );
          histTransRestrRebinPtFitMapAdjustedR2->Draw();
          c1.Print( std::string( pathPlots_ + histTransRestrRebinPtFitMapAdjustedR2->GetName() + ".png" ).c_str() );
        }
      } // iPar

      for ( int iCycle = 1; iCycle <= nCycles; ++iCycle ) {
        histTransRestrRebinPtFitMapProdChi2->SetBinContent( iCycle, dVecTransRestrRebinPtFitMapProdChi2.at( iCycle - 1 ) );
        histTransRestrRebinPtFitMapProdReducedChi2->SetBinContent( iCycle, dVecTransRestrRebinPtFitMapProdReducedChi2.at( iCycle - 1 ) );
        histTransRestrRebinPtFitMapProdProb->SetBinContent( iCycle, dVecTransRestrRebinPtFitMapProdProb.at( iCycle - 1 ) );
        histTransRestrRebinPtFitMapProdLogProb->SetBinContent( iCycle, dVecTransRestrRebinPtFitMapProdLogProb.at( iCycle - 1 ) );
        histTransRestrRebinPtFitMapProdR2->SetBinContent( iCycle, dVecTransRestrRebinPtFitMapProdR2.at( iCycle - 1 ) );
        if ( dVecTransRestrRebinPtFitMapProdR2.at( iCycle - 1 ) == 1. ) histTransRestrRebinPtFitMapProdR2->SetBinError( iCycle, dVecTransRestrRebinPtFitMapProdR2.at( iCycle - 1 ) );
        histTransRestrRebinPtFitMapProdAdjustedR2->SetBinContent( iCycle, dVecTransRestrRebinPtFitMapProdAdjustedR2.at( iCycle - 1 ) );
        if ( dVecTransRestrRebinPtFitMapProdAdjustedR2.at( iCycle - 1 ) == 1. ) histTransRestrRebinPtFitMapProdAdjustedR2->SetBinError( iCycle, dVecTransRestrRebinPtFitMapProdAdjustedR2.at( iCycle - 1 ) );
      } // iCycle

      if ( plot_ ) {
        histTransRestrRebinPtFitMapProdChi2->Draw();
        c1.Print( std::string( pathPlots_ + histTransRestrRebinPtFitMapProdChi2->GetName() + ".png" ).c_str() );
        histTransRestrRebinPtFitMapProdReducedChi2->Draw();
        c1.Print( std::string( pathPlots_ + histTransRestrRebinPtFitMapProdReducedChi2->GetName() + ".png" ).c_str() );
        histTransRestrRebinPtFitMapProdProb->Draw();
        c1.Print( std::string( pathPlots_ + histTransRestrRebinPtFitMapProdProb->GetName() + ".png" ).c_str() );
        histTransRestrRebinPtFitMapProdLogProb->Draw();
        c1.Print( std::string( pathPlots_ + histTransRestrRebinPtFitMapProdLogProb->GetName() + ".png" ).c_str() );
        histTransRestrRebinPtFitMapProdR2->Draw();
        c1.Print( std::string( pathPlots_ + histTransRestrRebinPtFitMapProdR2->GetName() + ".png" ).c_str() );
        histTransRestrRebinPtFitMapProdAdjustedR2->Draw();
        c1.Print( std::string( pathPlots_ + histTransRestrRebinPtFitMapProdAdjustedR2->GetName() + ".png" ).c_str() );
      }

      delete histTransRestrRebinCycle;

    } // loop: keyFit

  } // loop: uCat < objCats_.size()


  // Write and close ROOT files
  Int_t writeOut_( 0 );
  if ( overwrite_ ) {
    writeOut_ = fileIn_->Write( 0, TObject::kOverwrite );
  }
  else {
    writeOut_ = fileIn_->Write();
  }
  fileIn_->Close();

  if ( verbose_ > 0 )
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "    return status   " << returnStatus_ << std::endl
              << "    " << writeOut_ << " bytes written to output file" << std::endl;
  return returnStatus_;

}


// Compute R2
void computeR2( TH1D * histo, TF1 * function, double & R2, double & adjustedR2 )
{
  double minTransRestrRebinFit;
  double maxTransRestrRebinFit;
  function->GetRange( minTransRestrRebinFit, maxTransRestrRebinFit );
  const int minBinTransRestrRebin( histo->FindBin( minTransRestrRebinFit ) );
  const int minOffsetTransRestrRebin( histo->GetBinCenter( minBinTransRestrRebin ) < minTransRestrRebinFit ? 1 : 0 );
  const int maxBinTransRestrRebin( histo->FindBin( maxTransRestrRebinFit ) );
  const int maxOffsetTransRestrRebin( histo->GetBinCenter( maxBinTransRestrRebin ) >= maxTransRestrRebinFit ? 1 : 0 );

  double yMeanTransRestrRebin( 0. );
  unsigned nBin( 0 );
  for ( int iBin = minBinTransRestrRebin + minOffsetTransRestrRebin; iBin <= maxBinTransRestrRebin - maxOffsetTransRestrRebin; ++iBin ) {
    yMeanTransRestrRebin += histo->GetBinContent( iBin );
    ++nBin;
  }
  if ( nBin == 0 ) {
    R2         = 1.;
    adjustedR2 = 1.;
    return;
  }
  yMeanTransRestrRebin /= nBin;

  double ssTotTransRestrRebin( 0. );
  double ssResTransRestrRebin( 0. );
  for ( int iBin = minBinTransRestrRebin + minOffsetTransRestrRebin; iBin <= maxBinTransRestrRebin - maxOffsetTransRestrRebin; ++iBin ) {
    const double yTransRestrRebin( histo->GetBinContent( iBin ) );
    ssTotTransRestrRebin += std::pow( yTransRestrRebin - yMeanTransRestrRebin, 2. );
    const double yTransRestrRebinFit( function->Eval( histo->GetBinCenter( iBin ) ) );
    ssResTransRestrRebin += std::pow( yTransRestrRebin - yTransRestrRebinFit, 2. );
  }

  R2         = 1. -   ssResTransRestrRebin                                    /   ssTotTransRestrRebin;
  adjustedR2 = 1. - ( ssResTransRestrRebin / ( nBin - function->GetNpar() ) ) / ( ssTotTransRestrRebin / ( nBin - 1. ) );

  return;
}


// Weight R2
void weightR2( const bool useNorm, int nPar, int nParMax, double & R2, double & adjustedR2 )
{
  if ( ! useNorm ) {
    --nParMax;
    --nPar;
  }
  if ( nPar <= 0 ) {
    R2         = 1.;
    adjustedR2 = 1.;
    return;
  }
  double exponent( ( double )nParMax / ( double )nPar );
  if ( R2 >= 0. ) {
    R2 = std::pow( R2, exponent );
  }
  else {
    R2 = -std::pow( std::fabs( R2 ), exponent );
  }
  if ( adjustedR2 >= 0. ) {
    adjustedR2 = std::pow( adjustedR2, exponent );
  }
  else {
    adjustedR2 = -std::pow( std::fabs( adjustedR2 ), exponent );
  }
  return;
}

