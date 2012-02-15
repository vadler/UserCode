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
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TFitResult.h>

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
  const double widthFactor_( fitter_.getParameter< double >( "widthFactor" ) );
  // Configuration for existing resolution functions
  const bool useExisting_( process_.getParameter< bool >( "useExisting" ) );

  if ( verbose_ > 0 )
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "   called for object categories ";
    for ( unsigned uCat = 0; uCat < objCats_.size() - 1; ++uCat ) std::cout << "'" << objCats_.at( uCat ) << "', ";
    std::cout << "'" << objCats_.back() << "'" << std::endl;

  // Set constants
  const std::string nameDirClass( "TDirectoryFile" );
  const std::string nameFuncClass( "TF1" );
  std::string optionsFit_( "QRS" );
  std::string optionsFitSigma_( "MRS" );
  if      ( verbose_ < 2 ) optionsFitSigma_.append( "Q" );
  else if ( verbose_ > 3 ) optionsFitSigma_.append( "V" );


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

      for ( unsigned uCat = 0; uCat < objCats_.size(); ++uCat ) {
        const std::string objCat( objCats_.at( uCat ) );
        TDirectory * dirCat_( ( TDirectory* )( resolutionFile->Get( objCat.c_str() ) ) );
        if ( ! dirCat_ ) {
          std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                    << "   object category '" << objCat << "' does not exist in resolution file" << std::endl;
          continue;
        }

        TList * listCat( dirCat_->GetListOfKeys() );
        if ( verbose_ > 3 ) listCat->Print();
        TIter nextInListCat( listCat );
        while ( TKey * keyProp = ( TKey* )nextInListCat() ) {
          if ( std::string( keyProp->GetClassName() ) != nameDirClass ) continue;
          const std::string kinProp( keyProp->GetName() );
          TDirectory * dirProp_( ( TDirectory* )( dirCat_->Get( kinProp.c_str() ) ) );

          TList * listProp( dirProp_->GetListOfKeys() );
          if ( verbose_ > 3 ) listProp->Print();
          TIter nextInListProp( listProp );
          while ( TKey * keyEta = ( TKey* )nextInListProp() ) {
            if ( std::string( keyEta->GetClassName() ) != nameFuncClass ) continue;
            const std::string name( keyEta->GetName() );

//             TF1 * fitSigma2D2( ( TF1* )( dirProp_->Get( name.c_str() ) ) );

          } // loop: nextInListProp()

        } // loop: nextInListCat()

      } // loop: uCat < objCats_.size()

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
  TFile * fileIn_( TFile::Open( inFile_.c_str(), "UPDATE" ) );
  if ( ! fileIn_ ) {
    std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
              << "   input file '" << inFile_ << "' missing" << std::endl;
    returnStatus_ += 0x10;
    return returnStatus_;
  }
  TDirectory * dirSel_ = ( TDirectory* )( fileIn_->Get( evtSel_.c_str() ) );

  // Loops through directory structure

  if ( verbose_ > 0 )
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "   fitting resolution functions from input file '" << inFile_ << "'" << std::endl;

  typedef std::vector< std::vector< Double_t > > dataCont;

  // Loop over configured object categories
  for ( unsigned uCat = 0; uCat < objCats_.size(); ++uCat ) {
    const std::string objCat( objCats_.at( uCat ) );
    TDirectory * dirCat_( ( TDirectory* )( dirSel_->Get( objCat.c_str() ) ) );
    if ( ! dirCat_ ) {
      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                << "   object category '" << objCat << "' does not exist in input file" << std::endl;
      continue;
    }

    // Get binning per object category

    // Eta binning
    std::vector< double > etaBins_;
    TH1D * histBinsEta( ( TH1D* )( dirCat_->Get( std::string( objCat + "_binsEta" ).c_str() ) ) );
    for ( int iEta = 0; iEta < histBinsEta->GetNbinsX(); ++iEta ) {
      etaBins_.push_back( histBinsEta->GetBinLowEdge( iEta + 1 ) );
    }
    etaBins_.push_back( histBinsEta->GetBinLowEdge( histBinsEta->GetNbinsX() ) + histBinsEta->GetBinWidth( histBinsEta->GetNbinsX() ) );
    const unsigned nEtaBins_( etaBins_.size() - 1 );

    // Pt binning
    std::vector< double > ptBins_;
    TH1D * histBinsPt( ( TH1D* )( dirCat_->Get( std::string( objCat + "_binsPt" ).c_str() ) ) );
    for ( int uPt = 0; uPt < histBinsPt->GetNbinsX(); ++uPt ) {
      ptBins_.push_back( histBinsPt->GetBinLowEdge( uPt + 1 ) );
    }
    ptBins_.push_back( histBinsPt->GetBinLowEdge( histBinsPt->GetNbinsX() ) + histBinsPt->GetBinWidth( histBinsPt->GetNbinsX() ) );
    const unsigned nPtBins_( ptBins_.size() - 1 );

    // Read general n-tuple data
    dataCont momEData_( nEtaBins_ );
    dataCont momEGenData_( nEtaBins_ );
    Double_t momEData;
    Double_t momEGenData;
    Int_t    iEta;
    TTree * data_( ( TTree* )( dirCat_->Get( std::string( objCat + "_data" ).c_str() ) ) );
    data_->SetBranchAddress( "MomE", &momEData );
    data_->SetBranchAddress( "MomEGen", &momEGenData );
    if ( useSymm_ )
      if ( refGen_ ) data_->SetBranchAddress( "BinEtaSymmGen", &iEta );
      else           data_->SetBranchAddress( "BinEtaSymm", &iEta );
    else
      if ( refGen_ ) data_->SetBranchAddress( "BinEtaGen", &iEta );
      else           data_->SetBranchAddress( "BinEta", &iEta );
    Int_t nEntries( ( Int_t )data_->GetEntries() );
    std::vector< unsigned > sizeEta_( nEtaBins_ );
    for ( Int_t iEntry = 0; iEntry < nEntries; ++iEntry ) {
      data_->GetEntry( iEntry );
      assert( iEta < ( Int_t )( nEtaBins_ ) ); // has to fit (and be consistent)
      if ( iEta == -1 ) continue; // FIXME: eta out of range in analyzer; should be solved more consistently
      sizeEta_.at( iEta ) += 1;
      momEData_.at( iEta ).push_back( momEData );
      momEGenData_.at( iEta ).push_back( momEGenData );
    }

    // Loop over kinematic properties
    TList * listCat( dirCat_->GetListOfKeys() );
    if ( verbose_ > 3 ) listCat->Print();
    TIter nextInListCat( listCat );
    while ( TKey * keyProp = ( TKey* )nextInListCat() ) {
      if ( std::string( keyProp->GetClassName() ) != nameDirClass ) continue;
      const std::string kinProp( keyProp->GetName() );
      TDirectory * dirProp_( ( TDirectory* )( dirCat_->Get( kinProp.c_str() ) ) );

      // Read kinematic property n-tuple data
      dataCont propData_( nEtaBins_ );
      dataCont propGenData_( nEtaBins_ );
      Double_t propData;
      Double_t propGenData;
      std::string kinBranch;
      if ( kinProp == "Pt" ) kinBranch = "MomE"; //FIXME: in analyzer
      else                   kinBranch = kinProp;
      data_->SetBranchAddress( kinBranch.c_str(), &propData );
      data_->SetBranchAddress( std::string( kinBranch + "Gen" ).c_str(), &propGenData );
      for ( Int_t iEntry = 0; iEntry < nEntries; ++iEntry ) {
        data_->GetEntry( iEntry );
        if ( iEta == -1 ) continue; // FIXME: eta out of range in analyzer; should be solved more consistently
        propData_.at( iEta ).push_back( propData );
        propGenData_.at( iEta ).push_back( propGenData );
      }

      // Loop over fit versions
      TList * listProp( dirProp_->GetListOfKeys() );
      if ( verbose_ > 3 ) listProp->Print();
      TIter nextInListProp( listProp );
      while ( TKey * keyFit = ( TKey* )nextInListProp() ) {
        if ( std::string( keyFit->GetClassName() ) != nameDirClass ) continue;
        const std::string subFit( keyFit->GetName() );
        TDirectory * dirFit_( ( TDirectory* )( dirProp_->Get( subFit.c_str() ) ) );

        // Inversion flag from directory name
        const bool inverse( subFit.substr( subFit.size() - 3 ) == "Inv" );

        // Fit performance histograms

        dirFit_->cd();
        const std::string name( objCat + "_" + kinProp + "_" + subFit );
        const std::string titleChi2( "#chi^{2} / ndf" );
        const std::string titleProb( "Probability" );
        const std::string xTitle( "#eta" );
        const std::string yTitle( "p_{t} (GeV)" );

        const std::string name2D2Chi2Map( name + "_2D2_fitChi2Map" );
        TH1D * hist2D2Chi2Map( new TH1D( name2D2Chi2Map.c_str(), titleChi2.c_str(), nEtaBins_, etaBins_.data() ) );
        hist2D2Chi2Map->SetXTitle( xTitle.c_str() );
        hist2D2Chi2Map->SetYTitle( titleChi2.c_str() );
        const std::string nameSigmaChi2Map( name + "_Sigma_fitChi2Map" );
        TH1D * histSigmaChi2Map( new TH1D( nameSigmaChi2Map.c_str(), titleChi2.c_str(), nEtaBins_, etaBins_.data() ) );
        histSigmaChi2Map->SetXTitle( xTitle.c_str() );
        histSigmaChi2Map->SetYTitle( titleChi2.c_str() );
        const std::string nameSigmaNtupChi2Map( name + "_SigmaNtup_fitChi2Map" );
        TH1D * histSigmaNtupChi2Map( new TH1D( nameSigmaNtupChi2Map.c_str(), titleChi2.c_str(), nEtaBins_, etaBins_.data() ) );
        histSigmaNtupChi2Map->SetXTitle( xTitle.c_str() );
        histSigmaNtupChi2Map->SetYTitle( titleChi2.c_str() );
        const std::string name2D2ProbMap( name + "_2D2_fitProbMap" );
        TH1D * hist2D2ProbMap( new TH1D( name2D2ProbMap.c_str(), titleProb.c_str(), nEtaBins_, etaBins_.data() ) );
        hist2D2ProbMap->SetXTitle( xTitle.c_str() );
        hist2D2ProbMap->SetYTitle( titleProb.c_str() );
        const std::string nameSigmaProbMap( name + "_Sigma_fitProbMap" );
        TH1D * histSigmaProbMap( new TH1D( nameSigmaProbMap.c_str(), titleProb.c_str(), nEtaBins_, etaBins_.data() ) );
        histSigmaProbMap->SetXTitle( xTitle.c_str() );
        histSigmaProbMap->SetYTitle( titleProb.c_str() );
        const std::string nameSigmaNtupProbMap( name + "_SigmaNtup_fitProbMap" );
        TH1D * histSigmaNtupProbMap( new TH1D( nameSigmaNtupProbMap.c_str(), titleProb.c_str(), nEtaBins_, etaBins_.data() ) );
        histSigmaNtupProbMap->SetXTitle( xTitle.c_str() );
        histSigmaNtupProbMap->SetYTitle( titleProb.c_str() );
        const std::string name2D2Prob( name + "_2D2_fitProb" );
        TH1D * hist2D2Prob( new TH1D( name2D2Prob.c_str(), titleProb.c_str(), 20, 0., 1. ) );
        hist2D2Prob->SetXTitle( titleProb.c_str() );
        const std::string nameSigmaProb( name + "_Sigma_fitProb" );
        TH1D * histSigmaProb( new TH1D( nameSigmaProb.c_str(), titleProb.c_str(), 20, 0., 1. ) );
        histSigmaProb->SetXTitle( titleProb.c_str() );
        const std::string nameSigmaNtupProb( name + "_SigmaNtup_fitProb" );
        TH1D * histSigmaNtupProb( new TH1D( nameSigmaNtupProb.c_str(), titleProb.c_str(), 20, 0., 1. ) );
        histSigmaNtupProb->SetXTitle( titleProb.c_str() );

        const std::string nameDeltasChi2Map( name + "_Deltas_fitChi2Map" );
        TH2D * histDeltasChi2Map( new TH2D( nameDeltasChi2Map.c_str(), titleChi2.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
        histDeltasChi2Map->SetXTitle( xTitle.c_str() );
        histDeltasChi2Map->SetYTitle( yTitle.c_str() );
        histDeltasChi2Map->SetZTitle( titleChi2.c_str() );
        const std::string nameDeltasNtupChi2Map( name + "_DeltasNtup_fitChi2Map" );
        TH2D * histDeltasNtupChi2Map( new TH2D( nameDeltasNtupChi2Map.c_str(), titleChi2.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
        histDeltasNtupChi2Map->SetXTitle( xTitle.c_str() );
        histDeltasNtupChi2Map->SetYTitle( yTitle.c_str() );
        histDeltasNtupChi2Map->SetZTitle( titleChi2.c_str() );
        const std::string nameDeltasPropMap( name + "_Deltas_fitPropMap" );
        TH2D * histDeltasPropMap( new TH2D( nameDeltasPropMap.c_str(), titleProb.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
        histDeltasPropMap->SetXTitle( xTitle.c_str() );
        histDeltasPropMap->SetYTitle( yTitle.c_str() );
        histDeltasPropMap->SetZTitle( titleProb.c_str() );
        const std::string nameDeltasNtupPropMap( name + "_DeltasNtup_fitPropMap" );
        TH2D * histDeltasNtupPropMap( new TH2D( nameDeltasNtupPropMap.c_str(), titleProb.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
        histDeltasNtupPropMap->SetXTitle( xTitle.c_str() );
        histDeltasNtupPropMap->SetYTitle( yTitle.c_str() );
        histDeltasNtupPropMap->SetZTitle( titleProb.c_str() );
        ////////
        const std::string nameDeltasProp( name + "_Deltas_fitProp" );
        TH1D * histDeltasProp( new TH1D( nameDeltasProp.c_str(), titleProb.c_str(), 20, 0., 1. ) );
        histDeltasProp->SetXTitle( titleProb.c_str() );
        const std::string nameDeltasNtupProp( name + "_DeltasNtup_fitProp" );
        TH1D * histDeltasNtupProp( new TH1D( nameDeltasNtupProp.c_str(), titleProb.c_str(), 20, 0., 1. ) );
        histDeltasNtupProp->SetXTitle( titleProb.c_str() );

        // Loop over eta bins
        TList * listFit( dirFit_->GetListOfKeys() );
        if ( verbose_ > 3 ) listFit->Print();
        if ( listFit->GetSize() != ( Int_t )( nEtaBins_ ) ) {
          std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                    << "   mismatch of eta binning for object category '" << objCat << "':" << std::endl
                    << "       bins in directory structure: " << listFit->GetSize()         << std::endl
                    << "       bins in binning histogram  : " << nEtaBins_         << std::endl;
        }
        TIter nextInListFit( listFit );
        while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
          if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
          const std::string binEta( keyEta->GetName() );
          const unsigned uEta( std::atoi( binEta.substr( 3 ).data() ) );
          dirFit_->cd( binEta.c_str() );
          if ( verbose_ > 1 ) gDirectory->pwd();

          // Do the fits

          const std::string name( objCat + "_" + kinProp + "_" + subFit + "_" + binEta );

          // Direct fit in slices of the 2-dim histogram
          TH2D * hist2D( ( TH2D* )( gDirectory->Get( name.c_str() ) ) );
          hist2D->FitSlicesY( 0, 1, hist2D->GetNbinsX(), 1 );
          TH1D * histSigma2D2( ( TH1D* )( gDirectory->Get( std::string( name + "_2" ).c_str() ) ) ); // sigmas of the slice fits
          const std::string nameFit2D2( name + "_2D2_fit" );
          const std::string formula( inverse ? resFuncInv_ : resFunc_ );
          TF1 * fitSigma2D2( new TF1( nameFit2D2.c_str(), formula.c_str() ) );
          fitSigma2D2->SetRange( histSigma2D2->GetXaxis()->GetXmin(), histSigma2D2->GetXaxis()->GetXmax() );
          TFitResultPtr fitSigma2D2ResultPtr( histSigma2D2->Fit( fitSigma2D2, optionsFitSigma_.c_str() ) );
          if ( fitSigma2D2ResultPtr >= 0 && fitSigma2D2ResultPtr->Status() == 0 ) {
            hist2D2Chi2Map->SetBinContent( uEta + 1, fitSigma2D2ResultPtr->Chi2() / fitSigma2D2ResultPtr->Ndf() );
            hist2D2ProbMap->SetBinContent( uEta + 1, fitSigma2D2ResultPtr->Prob() );
            hist2D2Prob->Fill( fitSigma2D2ResultPtr->Prob() );
          }

          // FIXME: can be removed soon -- really?
          // Extract individual 1-dim histograms per p_t-bin and fit
          const std::string nameDeltaChi2( name + "Delta_fitChi2" );
          TH1D * histDeltaChi2( new TH1D( nameDeltaChi2.c_str(), titleChi2.c_str(), nPtBins_, ptBins_.data() ) );
          histDeltasChi2Map->SetXTitle( yTitle.c_str() );
          histDeltasChi2Map->SetYTitle( titleChi2.c_str() );
          const std::string nameDeltaProp( name + "Delta_fitProp" );
          TH1D * histDeltaProp( new TH1D( nameDeltaProp.c_str(), titleProb.c_str(), nPtBins_, ptBins_.data() ) );
          histDeltaProp->SetXTitle( yTitle.c_str() );
          histDeltaProp->SetYTitle( titleProb.c_str() );
          const std::string nameSigma( name + "_Sigma" );
          TH1D * histSigma( new TH1D( nameSigma.c_str(), hist2D->GetTitle(), nPtBins_, ptBins_.data() ) );
          for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
            const std::string binPt( boost::lexical_cast< std::string >( uPt ) );

            const std::string nameDelta( name + "_Pt" + binPt + "_Delta" );
            const std::string titleDelta( std::string( hist2D->GetTitle() ) + ", " + boost::lexical_cast< std::string >( hist2D->GetXaxis()->GetBinLowEdge( uPt + 1 ) ) + " GeV #leq p_{t} < " + boost::lexical_cast< std::string >( hist2D->GetXaxis()->GetBinUpEdge( uPt + 1 ) ) + " GeV" );
            const Int_t nBinsDelta( hist2D->GetNbinsY() );
            TH1D * histDelta( new TH1D( nameDelta.c_str(), titleDelta.c_str(), nBinsDelta, hist2D->GetYaxis()->GetXmin(), hist2D->GetYaxis()->GetXmax() ) );
            histDelta->SetXTitle( hist2D->GetYaxis()->GetTitle() );
            histDelta->SetYTitle( hist2D->GetZaxis()->GetTitle() );
            for ( Int_t iBin = 0; iBin < nBinsDelta; ++iBin ) {
              histDelta->SetBinContent( iBin, hist2D->GetBinContent( uPt + 1, iBin + 1 ) ); // skip underflow bin
            }

            const std::string nameFitDelta( nameDelta + "_fit" );
            TF1 * fitDelta( new TF1( nameFitDelta.c_str(), "gaus", histDelta->GetXaxis()->GetXmin(), histDelta->GetXaxis()->GetXmax() ) );
            TFitResultPtr fitDeltaResultPtr( histDelta->Fit( fitDelta, optionsFit_.c_str() ) );
            if ( fitDeltaResultPtr >= 0 && fitDeltaResultPtr->Status() == 0 && fitDeltaResultPtr->Ndf() != 0. ) {
              histDeltasChi2Map->SetBinContent( uEta + 1, uPt + 1, fitDeltaResultPtr->Chi2() / fitDeltaResultPtr->Ndf() );
              histDeltaChi2->SetBinContent( uPt + 1, fitDeltaResultPtr->Chi2() / fitDeltaResultPtr->Ndf() );
              histDeltasPropMap->SetBinContent( uEta + 1, uPt + 1, fitDeltaResultPtr->Prob() );
              histDeltasProp->Fill( fitDeltaResultPtr->Prob() );
              histDeltaProp->SetBinContent( uPt + 1, fitDeltaResultPtr->Prob() );
              histSigma->SetBinContent( uPt + 1, fitDeltaResultPtr->Parameter( 2 ) );
              histSigma->SetBinError( uPt + 1, fitDeltaResultPtr->ParError( 2 ) );
            }
          } // loop: uPt < nPtBins_
          const std::string nameFitSigma( nameSigma + "_fit" );
          TF1 * fitSigma( new TF1( nameFitSigma.c_str(), formula.c_str() ) );
          fitSigma->SetRange( histSigma->GetXaxis()->GetXmin(), histSigma->GetXaxis()->GetXmax() );
          TFitResultPtr fitSigmaResultPtr( histSigma->Fit( fitSigma, std::string( optionsFitSigma_ ).c_str() ) );
          if ( fitSigmaResultPtr >= 0 && fitSigmaResultPtr->Status() == 0 ) {
            histSigmaChi2Map->SetBinContent( uEta + 1, fitSigmaResultPtr->Chi2() / fitSigmaResultPtr->Ndf() );
            histSigmaProbMap->SetBinContent( uEta + 1, fitSigmaResultPtr->Prob() );
            histSigmaProb->Fill( fitSigmaResultPtr->Prob() );
          }

          // Create new 1-dim histograms from n-tuple
          const std::string nameDeltaNtupChi2( name + "DeltaNtup_fitChi2" );
          TH1D * histDeltaNtupChi2( new TH1D( nameDeltaNtupChi2.c_str(), titleChi2.c_str(), nPtBins_, ptBins_.data() ) );
          histDeltaNtupChi2->SetXTitle( yTitle.c_str() );
          histDeltaNtupChi2->SetYTitle( titleChi2.c_str() );
          const std::string nameDeltaNtupProp( name + "DeltaNtup_fitProp" );
          TH1D * histDeltaNtupProp( new TH1D( nameDeltaNtupProp.c_str(), titleProb.c_str(), nPtBins_, ptBins_.data() ) );
          histDeltaNtupProp->SetXTitle( yTitle.c_str() );
          histDeltaNtupProp->SetYTitle( titleProb.c_str() );
          const std::string nameSigmaNtup( nameSigma + "Ntup" );
          TH1D * histSigmaNtup( new TH1D( nameSigmaNtup.c_str(), hist2D->GetTitle(), nPtBins_, ptBins_.data() ) );
          // Split data into p_t bins
          dataCont momEEtaBin( nPtBins_ );
          dataCont propEtaBin( nPtBins_ );
          dataCont propGenEtaBin( nPtBins_ );
          std::vector< unsigned > sizePt( nPtBins_ );
          for ( unsigned uEntry = 0; uEntry < sizeEta_.at( uEta ); ++uEntry ) {
            for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
              if ( ptBins_.at( uPt ) <= momEData_.at( uEta ).at( uEntry ) && momEData_.at( uEta ).at( uEntry ) < ptBins_.at( uPt + 1 ) ) {
                sizePt.at( uPt ) += 1;
                momEEtaBin.at( uPt ).push_back( momEData_.at( uEta ).at( uEntry ) );
                propEtaBin.at( uPt ).push_back( propData_.at( uEta ).at( uEntry ) );
                propGenEtaBin.at( uPt ).push_back( propGenData_.at( uEta ).at( uEntry ) );
                break;
              }
            } // loop: uPt < nPtBins_
          } // loop: uEntry < nEntries
          for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
            if ( sizePt.at( uPt ) == 0 ) {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                          << "   empty bin in '" << name << "' for p_t bin" << uPt  << std::endl;
              }
              continue;
            }
            const std::string binPt( boost::lexical_cast< std::string >( uPt ) );

            const std::string nameDelta( name + "_Pt" + binPt + "_Delta" );
            TH1D * histDelta( ( TH1D* )( gDirectory->Get( nameDelta.c_str() ) ) );
            const std::string nameDeltaNtup( nameDelta + "Ntup" );
            const std::string titleDeltaNtup( histDelta->GetTitle() );
            const Int_t nBinsDeltaNtup( hist2D->GetNbinsY() ); // FIXME: tune number of bins,
//             const Double_t sigma( std::fabs( histSigma2D2->GetBinContent( uPt + 1 ) ) );
//             const Double_t width( sigma == 0. ? std::fabs( histDelta->GetRMS() ) : sigma ); // FIXME: Hmm...
            const Double_t mean( histDelta->GetMean() );
            const Double_t width( std::fabs( histDelta->GetRMS() ) );
            if ( width == 0. && verbose_ > 2 ) {
              std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                        << "   no histogram \"width\" in '" << nameDelta << "'" << std::endl;
            }
            const Double_t range( width == 0. ? widthFactor_ * std::fabs( histDelta->GetXaxis()->GetXmax() ) : widthFactor_ * width ); // FIXME: tune, incl. under- and overflow, remove hard-coding
            TH1D * histDeltaNtup( new TH1D( nameDeltaNtup.c_str(), titleDeltaNtup.c_str(), nBinsDeltaNtup, -range + mean, range + mean ) );
            histDeltaNtup->SetXTitle( histDelta->GetXaxis()->GetTitle() );
            histDeltaNtup->SetYTitle( histDelta->GetYaxis()->GetTitle() );
            for ( unsigned uEntry = 0; uEntry < sizePt.at( uPt ); ++uEntry ) {
              if ( refGen_ ) {
                if ( inverse && kinProp == "Pt" ) histDeltaNtup->Fill( 1. / propGenEtaBin.at( uPt ).at( uEntry ) - 1. / propEtaBin.at( uPt ).at( uEntry ) );
                else                              histDeltaNtup->Fill( propGenEtaBin.at( uPt ).at( uEntry ) - propEtaBin.at( uPt ).at( uEntry ) );
              }
              else {
                if ( inverse && kinProp == "Pt" ) histDeltaNtup->Fill( 1. / propEtaBin.at( uPt ).at( uEntry ) - 1. / propGenEtaBin.at( uPt ).at( uEntry ) );
                else                              histDeltaNtup->Fill( propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ) );
              }
            }

            const std::string nameFitDeltaNtup( nameDeltaNtup + "_fit" );
            TF1 * fitDeltaNtup( new TF1( nameFitDeltaNtup.c_str(), "gaus", histDeltaNtup->GetXaxis()->GetXmin(), histDeltaNtup->GetXaxis()->GetXmax() ) );
            TFitResultPtr fitDeltaNtupResultPtr( histDeltaNtup->Fit( fitDeltaNtup, optionsFit_.c_str() ) );
            if ( fitDeltaNtupResultPtr >= 0 && fitDeltaNtupResultPtr->Status() == 0 && fitDeltaNtupResultPtr->Ndf() != 0. ) {
              histDeltasNtupChi2Map->SetBinContent( uEta + 1, uPt + 1, fitDeltaNtupResultPtr->Chi2() / fitDeltaNtupResultPtr->Ndf() );
              histDeltaNtupChi2->SetBinContent( uPt + 1, fitDeltaNtupResultPtr->Chi2() / fitDeltaNtupResultPtr->Ndf() );
              histDeltasNtupPropMap->SetBinContent( uEta + 1, uPt + 1, fitDeltaNtupResultPtr->Prob() );
              histDeltasNtupProp->Fill( fitDeltaNtupResultPtr->Prob() );
              histDeltaNtupProp->SetBinContent( uPt + 1, fitDeltaNtupResultPtr->Prob() );
              histSigmaNtup->SetBinContent( uPt + 1, fitDeltaNtupResultPtr->Parameter( 2 ) );
              histSigmaNtup->SetBinError( uPt + 1, fitDeltaNtupResultPtr->ParError( 2 ) );
            }  // loop: uPt < nPtBins
          }  // loop: uEntry < sizePt.at( uPt )
          const std::string nameFitSigmaNtup( nameSigmaNtup + "_fit" );
          TF1 * fitSigmaNtup( new TF1( nameFitSigmaNtup.c_str(), formula.c_str() ) );
          fitSigmaNtup->SetRange( histSigmaNtup->GetXaxis()->GetXmin(), histSigmaNtup->GetXaxis()->GetXmax() );
          TFitResultPtr fitSigmaNtupResultPtr( histSigmaNtup->Fit( fitSigmaNtup, std::string( optionsFitSigma_ ).c_str() ) );
          if ( fitSigmaNtupResultPtr >= 0 && fitSigmaNtupResultPtr->Status() == 0 ) {
            histSigmaNtupChi2Map->SetBinContent( uEta + 1, fitSigmaNtupResultPtr->Chi2() / fitSigmaNtupResultPtr->Ndf() );
            histSigmaNtupProbMap->SetBinContent( uEta + 1, fitSigmaNtupResultPtr->Prob() );
            histSigmaNtupProb->Fill( fitSigmaNtupResultPtr->Prob() );
          }

        } // loop: keyEta

      } // loop: keyFit

    } // loop: keyProp

  } // loop: uCat < objCats_.size()

  // Write and close input file
  fileIn_->Write();
  fileIn_->Close();

  if ( verbose_ > 0 )
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "   return status " << returnStatus_ << std::endl;
  return returnStatus_;

}
