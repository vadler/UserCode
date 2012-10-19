#include <cstdlib>
#include <cmath>
#include <cassert>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>

#include "boost/lexical_cast.hpp"

#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TKey.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TF2.h>
#include <TFitResult.h>
#include <TMath.h>
#include <TCanvas.h>

#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include "DataFormats/Math/interface/deltaR.h"

#include "CommonTools/MyTools/interface/RootTools.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"


// Initialise parameters for fit function
void setParametersFit( TF1 * fit, TH1D * histo, bool useBkgFunction = false );


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
  const bool usePileUp_( process_.getParameter< bool >( "usePileUp" ) );
  const bool useAlt_( process_.getParameter< bool >( "useAlt" ) );
  const bool useNonT_( process_.getParameter< bool >( "useNonT" ) );
  const bool useSymm_( process_.getParameter< bool >( "useSymm" ) );
  const bool refGenJet_( process_.getParameter< bool >( "refGenJet" ) );
  const bool refSel_( process_.getParameter< bool >( "refSel" ) );
  const std::string pileUp_( process_.getParameter< std::string >( "pileUp" ) );
  // Configuration for in- & output
  const edm::ParameterSet & io_( process_.getParameter< edm::ParameterSet >( "io" ) );
  const std::string inFile_( io_.getParameter< std::string >( "inputFile" ) );
  const std::string outFile_( io_.getParameter< std::string >( "outputFile" ) );
  const bool overwrite_( io_.getParameter< bool >( "overwrite" ));
  const std::string sample_( io_.getParameter< std::string >( "sample" ) );
  const std::string pathPlots_( io_.getParameter< std::string >( "pathPlots" ) );
  const bool plot_( ! pathPlots_.empty() );
  // Configuration for histogram binning
  // Configuration for histogram binning
  const edm::ParameterSet & histos_( process_.getParameter< edm::ParameterSet >( "histos" ) );
  const unsigned histBins_( histos_.getParameter< unsigned >( "FracPtBins" ) );
  const double   histMax_( histos_.getParameter< double >( "FracPtMax" ) );
  const double widthFactor_( histos_.getParameter< double >( "widthFactor" ) );
  // Configuration for fitting L5 JECs
  const edm::ParameterSet & fit_( process_.getParameter< edm::ParameterSet >( "fit" ) );
  const bool fitNonRestr_( fit_.getParameter< bool >( "fitNonRestr" ) );
  const double minPt_( fit_.getParameter< double >( "minPt" ) );
  const double maxDR_( fit_.getParameter< double >( "maxDR" ) );
  // Configuration for fitting L5 JECs
  const edm::ParameterSet & jecL5_( process_.getParameter< edm::ParameterSet >( "jecL5" ) );
  const bool doFit_( jecL5_.getParameter< bool >( "doFit" ) );
  const std::string fitFunction_( jecL5_.getParameter< std::string >( "fitFunction" ) );
  const std::string bkgFunction_( jecL5_.getParameter< std::string >( "bkgFunction" ) );
  const bool useBkg_( bkgFunction_ != "" || bkgFunction_ != "0" );
  std::string fitOptions_( jecL5_.getParameter< std::string >( "fitOptions" ) );
  const double fitRange_( std::min( jecL5_.getParameter< double >( "fitRange" ), widthFactor_ ) );
  const std::vector< std::string > jecVars_( jecL5_.getParameter< std::vector< std::string > >( "jecVars" ) );
  const std::vector< std::string > jecDims_( jecL5_.getParameter< std::vector< std::string > >( "jecDims" ) );
  const std::string jecFunction_( jecL5_.getParameter< std::string >( "jecFunction" ) );
  const bool writeFiles_( jecL5_.getParameter< bool >( "writeFiles" ) );
  const std::string pathOut_( jecL5_.getParameter< std::string >( "pathOut" ) );

  if ( verbose_ > 0 ) {
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "    called for object categories ";
    for ( unsigned uCat = 0; uCat < objCats_.size() - 1; ++uCat ) std::cout << "'" << objCats_.at( uCat ) << "', ";
    std::cout << "'" << objCats_.back() << "'" << std::endl;
  }

  // Set constants

  const TFormula fitFormula( "fitFormula", fitFunction_.c_str() );
  const unsigned nParFit_( fitFormula.GetNpar() );
  const TFormula jecFormula( "jecFormula", jecFunction_.c_str() );
  const unsigned nVar_( jecVars_.size() );
//   const unsigned nDim_( jecDims_.size() );
  const unsigned nPar_( jecFormula.GetNpar() );

  // Set string constants
  std::string evtSel_( "analyzeHitFit" );
  if ( refSel_ ) evtSel_.append( "Reference" );
  const std::string nameDirClass( "TDirectoryFile" );
  const std::string nameFuncClass( "TF1" );
  if ( verbose_ < 3 ) fitOptions_.append( "Q" );
  else if ( verbose_ > 4 ) fitOptions_.append( "V" );
  const std::string baseTitlePtL( useAlt_ ? "E" : "p" );
  const std::string baseTitlePtU( useAlt_ ? "E" : "P" );
  const std::string baseTitlePt( useNonT_ ? baseTitlePtU : baseTitlePtU + "t" );
  const std::string titlePtT( useNonT_ ? baseTitlePtL : baseTitlePtL + "_{t}" );
  const std::string titlePt( refGenJet_ ? titlePtT + "^{GenJet} (GeV)" : titlePtT + "^{L3} (GeV)" );
  const std::string baseTitleEta( useSymm_ ? "|#eta|" : "#eta" );
  const std::string titleEta( refGenJet_ ? baseTitleEta + "^{GenJet}" : baseTitleEta + "^{L3}" );
  std::vector< std::string > titlesVar( 1, titleEta );
  std::vector< std::string > titlesPar( 1, titlePt );
  const std::string titleFracL5( refGenJet_ ? "#frac{" + titlePtT + "^{L3}}{" + titlePtT + "^{GenJet}}" : "#frac{" + titlePtT + "^{GenJet}}{" + titlePtT + "^{L3}}" );
  const std::string titleFracL5Mean( "#mu of " + titleFracL5 );
  const std::string titleFracL5Norm( "c of " + titleFracL5 );
  const std::string titleFracL5Sigma( "#sigma of " + titleFracL5 );
  const std::string titleEvents( "events" );
  const std::string titleProb( "log_{10}(prob.)" );
  const std::string titleFits( "fits" );

  // Open input file

  if ( verbose_ > 0 )
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "    using      input  file '" << inFile_  << "'" << std::endl
              << "    writing to output file '" << outFile_ << "'" << std::endl;

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
  TH1D::SetDefaultSumw2();
  TH2D::SetDefaultSumw2();

  // Read pile-up data
  std::vector< Double_t > pileUpWeights_;
  Double_t pileUpWeight;
  TTree * pileUpData_( dynamic_cast< TTree* >( dirSel_->Get( "Data" ) ) );
  pileUpData_->SetBranchAddress( pileUp_.c_str(), &pileUpWeight );
  Int_t nEntries( ( Int_t )pileUpData_->GetEntries() );
  for ( Int_t iEntry = 0; iEntry < nEntries; ++iEntry ) {
    pileUpData_->GetEntry( iEntry );
    if ( usePileUp_ ) pileUpWeights_.push_back( pileUpWeight );
    else              pileUpWeights_.push_back( 1. );
  }

  // Open output file

  bool newOut( false );
  TFile * fileOut_( TFile::Open( outFile_.c_str(), "UPDATE" ) );
  if ( ! fileOut_ ) {
    fileOut_ = TFile::Open( outFile_.c_str(), "NEW" );
    newOut = true;
  }
  if ( ! fileOut_ ) {
    std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
              << "    output file '" << outFile_ << "' could not be opened" << std::endl;
    returnStatus_ += 0x30;
    return returnStatus_;
  }
  TDirectory * dirOutSel_( ( TDirectory* )( fileOut_->Get( evtSel_.c_str() ) ) );
  if ( ! dirOutSel_ ) {
    fileOut_->cd();
    dirOutSel_ = new TDirectory( evtSel_.c_str(), "" );
  }

  TCanvas c1( "c1" );
  c1.cd();

  // Loops through directory structure

  typedef std::vector< std::vector< Double_t > > DataCont;

  // Loop over configured object categories
  for ( unsigned uCat = 0; uCat < objCats_.size(); ++uCat ) {
    const std::string objCat( objCats_.at( uCat ) );
    if ( objCat != "UdscJet" && objCat != "BJet" ) {
      if ( verbose_ > 1 ) {
        std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                  << "    object category '" << objCat << "' not appropriate for L5 JECs" << std::endl;
      }
      continue;
    }
    TDirectory * dirCat_( ( TDirectory* )( dirSel_->Get( objCat.c_str() ) ) );
    if ( ! dirCat_ ) {
      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                << "    object category '" << objCat << "' does not exist in input file" << std::endl;
      returnStatus_ += 0x100;
      continue;
    }
    TDirectory * dirOutCat_( ( TDirectory* )( dirOutSel_->Get( objCat.c_str() ) ) );
    if ( ! dirOutCat_ ) {
      dirOutSel_->cd();
      dirOutCat_ = new TDirectory( objCat.c_str(), "" );
    }

    // Get binning per object category

    // Eta binning
    std::vector< double > etaBins_;
    TH1D * histBinsEta( ( TH1D* )( dirCat_->Get( std::string( objCat + "_binsEta" ).c_str() ) ) );
    const bool objMetLike( histBinsEta->GetNbinsX() == 1 );
    if ( objMetLike ) {
      etaBins_.push_back( histBinsEta->GetBinLowEdge( 1 ) );
    }
    else {
      for ( int iEta = 0; iEta < histBinsEta->GetNbinsX(); ++iEta ) {
        double edge( histBinsEta->GetBinLowEdge( iEta + 1 ) );
        if ( useSymm_  && edge < 0. ) continue;
        etaBins_.push_back( edge );
      }
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

    // Read kinematic property n-tuple data
    DataCont weightData_( nEtaBins_ );
    DataCont ptData_( nEtaBins_ );
    DataCont ptGenJetData_( nEtaBins_ );
    DataCont etaData_( nEtaBins_ );
    DataCont etaGenJetData_( nEtaBins_ );
    DataCont phiData_( nEtaBins_ );
    DataCont phiGenJetData_( nEtaBins_ );
    Double_t ptData;
    Double_t ptGenJetData;
    Double_t etaData;
    Double_t etaGenJetData;
    Double_t phiData;
    Double_t phiGenJetData;
    Int_t    iEta;
    TTree * data_( ( TTree* )( dirCat_->Get( std::string( objCat + "_data" ).c_str() ) ) );
    if ( useAlt_ ) {
      data_->SetBranchAddress( "PtAlt" , &ptData );
      data_->SetBranchAddress( "EtaAlt", &etaData );
      data_->SetBranchAddress( "PhiAlt", &phiData );
      data_->SetBranchAddress( "PtGenJetAlt" , &ptGenJetData );
      data_->SetBranchAddress( "EtaGenJetAlt", &etaGenJetData );
      data_->SetBranchAddress( "PhiGenJetAlt", &phiGenJetData );
    }
    else {
      data_->SetBranchAddress( "Pt" , &ptData );
      data_->SetBranchAddress( "Eta", &etaData );
      data_->SetBranchAddress( "Phi", &phiData );
      data_->SetBranchAddress( "PtGenJet" , &ptGenJetData );
      data_->SetBranchAddress( "EtaGenJet", &etaGenJetData );
      data_->SetBranchAddress( "PhiGenJet", &phiGenJetData );
    }
    if ( useSymm_ ) {
      if ( useAlt_ ) {
        if ( refGenJet_ ) data_->SetBranchAddress( "BinEtaSymmGenJetAlt", &iEta );
        else              data_->SetBranchAddress( "BinEtaSymmAlt", &iEta );
      }
      else {
        if ( refGenJet_ ) data_->SetBranchAddress( "BinEtaSymmGenJet", &iEta );
        else              data_->SetBranchAddress( "BinEtaSymm", &iEta );
      }
    }
    else {
      if ( useAlt_ ) {
        if ( refGenJet_ ) data_->SetBranchAddress( "BinEtaGenJetAlt", &iEta );
        else              data_->SetBranchAddress( "BinEtaAlt", &iEta );
      }
      else {
        if ( refGenJet_ ) data_->SetBranchAddress( "BinEtaGenJet", &iEta );
        else              data_->SetBranchAddress( "BinEta", &iEta );
      }
    }
    Int_t nEntries( ( Int_t )data_->GetEntries() );
    assert( nEntries % 2 == 0 ); // need two jet entries per event
    std::vector< unsigned > sizeEta_( nEtaBins_ );
    for ( Int_t iEntry = 0; iEntry < nEntries; ++iEntry ) {
      data_->GetEntry( iEntry );
      assert( iEta < ( Int_t )( nEtaBins_ ) ); // has to fit (and be consistent)
      if ( iEta == -1 ) continue; // FIXME: eta out of range in analyzer; should be solved more consistently
      sizeEta_.at( iEta ) += 1;
      Int_t pileUpEntry( iEntry / 2 );
      weightData_.at( iEta ).push_back( pileUpWeights_.at( pileUpEntry ) );
      ptData_.at( iEta ).push_back( ptData );
      ptGenJetData_.at( iEta ).push_back( ptGenJetData );
      etaData_.at( iEta ).push_back( etaData );
      etaGenJetData_.at( iEta ).push_back( etaGenJetData );
      phiData_.at( iEta ).push_back( phiData );
      phiGenJetData_.at( iEta ).push_back( phiGenJetData );
    }

    TDirectory * dirPt_( ( TDirectory* )( dirCat_->Get( "Pt" ) ) );
    if ( ! dirPt_ ) {
      std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                << "    kinematic property 'Pt' does not exist in input file" << std::endl;
      returnStatus_ += 0x1000;
      continue;
    }
    TDirectory * dirOutPt_( ( TDirectory* )( dirOutCat_->Get( "Pt" ) ) );
    if ( ! dirOutPt_ ) {
      dirOutCat_->cd();
      dirOutPt_ = new TDirectory( "Pt", "" );
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
      if ( useAlt_     == ( subFit.find( "Alt" )  == std::string::npos ) ) continue;
      if ( useSymm_    == ( subFit.find( "Symm" ) == std::string::npos ) ) continue;
      if ( refGenJet_  == ( subFit.find( "Gen" )  == std::string::npos ) ) continue;
      TDirectory * dirFit_( ( TDirectory* )( dirPt_->Get( subFit.c_str() ) ) );
      if ( ! dirFit_ ) {
        std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                  << "    fit '" << subFit << "' does not exist in input file" << std::endl;
        returnStatus_ += 0x10000;
        continue;
      }
      dirFit_->cd();

      std::string name( objCat + "_" + baseTitlePt + "_" + subFit );

      const std::string nameFracL5( name + "_FracL5" );
      TH1D * histFracL5( new TH1D( nameFracL5.c_str(), objCat.c_str(), histBins_, 0., histMax_ ) );
      histFracL5->SetXTitle( titleFracL5.c_str() );
      histFracL5->SetYTitle( titleEvents.c_str() );
      const std::string nameFracL5MapPt( nameFracL5 + "_Map_Pt" );
      TH2D * histFracL5MapPt( new TH2D( nameFracL5MapPt.c_str(), objCat.c_str(), nPtBins_, ptBins_.data(), histBins_, 0., histMax_ ) );
      histFracL5MapPt->SetXTitle( titlePt.c_str() );
      histFracL5MapPt->SetYTitle( titleFracL5.c_str() );
      histFracL5MapPt->SetZTitle( titleEvents.c_str() );
      const std::string nameFracL5MapEta( nameFracL5 + "_Map_Eta" );
      TH2D * histFracL5MapEta( new TH2D( nameFracL5MapEta.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data(), histBins_, 0., histMax_ ) );
      histFracL5MapEta->SetXTitle( titleEta.c_str() );
      histFracL5MapEta->SetYTitle( titleFracL5.c_str() );
      histFracL5MapEta->SetZTitle( titleEvents.c_str() );

      const std::string nameFracL5Restr( nameFracL5 + "Restr" );
      TH1D * histFracL5Restr( new TH1D( nameFracL5Restr.c_str(), objCat.c_str(), histBins_, 0., histMax_ ) );
      histFracL5Restr->SetXTitle( titleFracL5.c_str() );
      histFracL5Restr->SetYTitle( titleEvents.c_str() );
      const std::string nameFracL5RestrMapPt( nameFracL5Restr + "_Map_Pt" );
      TH2D * histFracL5RestrMapPt( new TH2D( nameFracL5RestrMapPt.c_str(), objCat.c_str(), nPtBins_, ptBins_.data(), histBins_, 0., histMax_ ) );
      histFracL5RestrMapPt->SetXTitle( titlePt.c_str() );
      histFracL5RestrMapPt->SetYTitle( titleFracL5.c_str() );
      histFracL5RestrMapPt->SetZTitle( titleEvents.c_str() );
      const std::string nameFracL5RestrMapEta( nameFracL5Restr + "_Map_Eta" );
      TH2D * histFracL5RestrMapEta( new TH2D( nameFracL5RestrMapEta.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data(), histBins_, 0., histMax_ ) );
      histFracL5RestrMapEta->SetXTitle( titleEta.c_str() );
      histFracL5RestrMapEta->SetYTitle( titleFracL5.c_str() );
      histFracL5RestrMapEta->SetZTitle( titleEvents.c_str() );

      std::vector< TH1D * > histVecPtFracL5;
      std::vector< TH2D * > histVecPtFracL5MapEta;
      std::vector< TH1D * > histVecPtFracL5Restr;
      std::vector< TH2D * > histVecPtFracL5RestrMapEta;
      for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
        const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
        const std::string namePt( name + "_" + baseTitlePt + binPt );
        const std::string namePtFracL5( namePt + "_FracL5" );
        const std::string titlePtFracL5( objCat + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq " + titlePtT + " < " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
        TH1D * histPtFracL5( new TH1D( namePtFracL5.c_str(), titlePtFracL5.c_str(), histBins_, 0., histMax_ ) );
        histPtFracL5->SetXTitle( titleFracL5.c_str() );
        histPtFracL5->SetYTitle( titleEvents.c_str() );
        histVecPtFracL5.push_back( histPtFracL5 );
        const std::string namePtFracL5MapEta( namePtFracL5 + "_Map_Eta" );
        TH2D * histPtFracL5MapEta( new TH2D( namePtFracL5MapEta.c_str(), titlePtFracL5.c_str(), nEtaBins_, etaBins_.data(), histBins_, 0., histMax_ ) );
        histPtFracL5MapEta->SetXTitle( titleEta.c_str() );
        histPtFracL5MapEta->SetYTitle( titleFracL5.c_str() );
        histPtFracL5MapEta->SetZTitle( titleEvents.c_str() );
        histVecPtFracL5MapEta.push_back( histPtFracL5MapEta );
        const std::string namePtFracL5Restr( namePtFracL5 + "Restr" );
        TH1D * histPtFracL5Restr( new TH1D( namePtFracL5Restr.c_str(), titlePtFracL5.c_str(), histBins_, 0., histMax_ ) );
        histPtFracL5Restr->SetXTitle( titleFracL5.c_str() );
        histPtFracL5Restr->SetYTitle( titleEvents.c_str() );
        histVecPtFracL5Restr.push_back( histPtFracL5Restr );
        const std::string namePtFracL5RestrMapEta( namePtFracL5Restr + "_Map_Eta" );
        TH2D * histPtFracL5RestrMapEta( new TH2D( namePtFracL5RestrMapEta.c_str(), titlePtFracL5.c_str(), nEtaBins_, etaBins_.data(), histBins_, 0., histMax_ ) );
        histPtFracL5RestrMapEta->SetXTitle( titleEta.c_str() );
        histPtFracL5RestrMapEta->SetYTitle( titleFracL5.c_str() );
        histPtFracL5RestrMapEta->SetZTitle( titleEvents.c_str() );
        histVecPtFracL5RestrMapEta.push_back( histPtFracL5RestrMapEta );
      }

      // Loop over eta bins
      TList * listFit( dirFit_->GetListOfKeys() );
      if ( verbose_ > 3 ) listFit->Print();
      unsigned sizeEtaBins( 0 );
      TIter nextInListFit( listFit );
      while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
        if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
        ++sizeEtaBins;
        const std::string binEta( keyEta->GetName() );
        const unsigned uEta( std::atoi( binEta.substr( 3 ).data() ) );
        dirFit_->cd( binEta.c_str() );
        if ( verbose_ > 1 ) gDirectory->pwd();

        const std::string nameEta( name + "_" + binEta );

        const std::string nameEtaFracL5( nameEta + "_FracL5" );
        const std::string titleEtaFracL5( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta < " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) );
        TH1D * histEtaFracL5( new TH1D( nameEtaFracL5.c_str(), titleEtaFracL5.c_str(), histBins_, 0., histMax_ ) );
        histEtaFracL5->SetXTitle( titleFracL5.c_str() );
        histEtaFracL5->SetYTitle( titleEvents.c_str() );
        const std::string nameEtaFracL5MapPt( nameEtaFracL5 + "_Map_Pt" );
        TH2D * histEtaFracL5MapPt( new TH2D( nameEtaFracL5MapPt.c_str(), titleEtaFracL5.c_str(), nPtBins_, ptBins_.data(), histBins_, 0., histMax_ ) );
        histEtaFracL5MapPt->SetXTitle( titlePt.c_str() );
        histEtaFracL5MapPt->SetYTitle( titleFracL5.c_str() );
        histEtaFracL5MapPt->SetZTitle( titleEvents.c_str() );

        const std::string nameEtaFracL5Restr( nameEtaFracL5 + "Restr" );
        TH1D * histEtaFracL5Restr( new TH1D( nameEtaFracL5Restr.c_str(), titleEtaFracL5.c_str(), histBins_, 0., histMax_ ) );
        histEtaFracL5Restr->SetXTitle( titleFracL5.c_str() );
        histEtaFracL5Restr->SetYTitle( titleEvents.c_str() );
        const std::string nameEtaFracL5RestrMapPt( nameEtaFracL5Restr + "_Map_Pt" );
        TH2D * histEtaFracL5RestrMapPt( new TH2D( nameEtaFracL5RestrMapPt.c_str(), titleEtaFracL5.c_str(), nPtBins_, ptBins_.data(), histBins_, 0., histMax_ ) );
        histEtaFracL5RestrMapPt->SetXTitle( titlePt.c_str() );
        histEtaFracL5RestrMapPt->SetYTitle( titleFracL5.c_str() );
        histEtaFracL5RestrMapPt->SetZTitle( titleEvents.c_str() );

        // Split data into p_t bins
        DataCont weightEtaBin( nPtBins_ );
        DataCont ptEtaBin( nPtBins_ );
        DataCont ptGenJetEtaBin( nPtBins_ );
        DataCont etaEtaBin( nPtBins_ );
        DataCont etaGenJetEtaBin( nPtBins_ );
        DataCont phiEtaBin( nPtBins_ );
        DataCont phiGenJetEtaBin( nPtBins_ );
        std::vector< unsigned > sizePt( nPtBins_ );
        for ( unsigned uEntry = 0; uEntry < sizeEta_.at( uEta ); ++uEntry ) {
          if ( ptData_.at( uEta ).at( uEntry ) == -9. || ptGenJetData_.at( uEta ).at( uEntry ) == -9. ) continue; // no match
          Double_t ptVal( useNonT_ ? ptData_.at( uEta ).at( uEntry ) * std::cosh( etaData_.at( uEta ).at( uEntry ) ) : ptData_.at( uEta ).at( uEntry ) );
          Double_t ptGenVal( useNonT_ ? ptGenJetData_.at( uEta ).at( uEntry ) * std::cosh( etaGenJetData_.at( uEta ).at( uEntry ) ) : ptGenJetData_.at( uEta ).at( uEntry ) );
          Double_t ptRef( refGenJet_ ? ptGenVal : ptVal );
          for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
            if ( ptBins_.at( uPt ) <= ptRef && ptRef < ptBins_.at( uPt + 1 ) ) {
              sizePt.at( uPt ) += 1;
              weightEtaBin.at( uPt ).push_back( weightData_.at( uEta ).at( uEntry ) );
              ptEtaBin.at( uPt ).push_back( ptVal );
              ptGenJetEtaBin.at( uPt ).push_back( ptGenVal );
              etaEtaBin.at( uPt ).push_back( etaData_.at( uEta ).at( uEntry ) );
              etaGenJetEtaBin.at( uPt ).push_back( etaGenJetData_.at( uEta ).at( uEntry ) );
              phiEtaBin.at( uPt ).push_back( phiData_.at( uEta ).at( uEntry ) );
              phiGenJetEtaBin.at( uPt ).push_back( phiGenJetData_.at( uEta ).at( uEntry ) );
              break;
            }
          } // loop: uPt < nPtBins_
        } // loop: uEntry < sizeEta_.at( uEta )

        // Loop over pt bins
        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
          if ( sizePt.at( uPt ) == 0 ) {
            if ( verbose_ > 2 ) {
              std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                        << "    empty bin " << uPt << " in '" << nameEta << "' for " + titlePtT + " bin " << uPt  << std::endl;
            }
          }

          const std::string nameEtaPt( nameEta + "_" + baseTitlePt + binPt );

          const std::string nameEtaPtFracL5( nameEtaPt + "_FracL5" );
          const std::string titleEtaPtFracL5( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta < " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq " + titlePtT + " < " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
          TH1D * histEtaPtFracL5( new TH1D( nameEtaPtFracL5.c_str(), titleEtaPtFracL5.c_str(), histBins_, 0., histMax_  ) );
          histEtaPtFracL5->SetXTitle( titleFracL5.c_str() );
          histEtaPtFracL5->SetYTitle( titleEvents.c_str() );

          const std::string nameEtaPtFracL5Restr( nameEtaPtFracL5 + "Restr" );
          TH1D * histEtaPtFracL5Restr( new TH1D( nameEtaPtFracL5Restr.c_str(), titleEtaPtFracL5.c_str(), histBins_, 0., histMax_  ) );
          histEtaPtFracL5Restr->SetXTitle( titleFracL5.c_str() );
          histEtaPtFracL5Restr->SetYTitle( titleEvents.c_str() );

          for ( unsigned uEntry = 0; uEntry < sizePt.at( uPt ); ++uEntry ) {
            const Double_t value( refGenJet_ ? ptEtaBin.at( uPt ).at( uEntry ) / ptGenJetEtaBin.at( uPt ).at( uEntry ) : ptGenJetEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ) );
            const Double_t pt( refGenJet_ ? ptGenJetEtaBin.at( uPt ).at( uEntry ) : ptEtaBin.at( uPt ).at( uEntry ) );
            const Double_t etaGenJetSymm( useSymm_ ? std::fabs( etaGenJetEtaBin.at( uPt ).at( uEntry ) ) : etaGenJetEtaBin.at( uPt ).at( uEntry ) );
            const Double_t etaSymm( useSymm_ ? std::fabs( etaEtaBin.at( uPt ).at( uEntry ) ) : etaEtaBin.at( uPt ).at( uEntry ) );
            const Double_t eta( refGenJet_ ? etaGenJetSymm : etaSymm );
            if ( fitNonRestr_ ) {
              histEtaPtFracL5->Fill( value, weightEtaBin.at( uPt ).at( uEntry ) );
              histVecPtFracL5.at( uPt )->Fill( value, weightEtaBin.at( uPt ).at( uEntry ) );
              histEtaFracL5->Fill( value, weightEtaBin.at( uPt ).at( uEntry ) );
              histFracL5->Fill( value, weightEtaBin.at( uPt ).at( uEntry ) );
              histEtaFracL5MapPt->Fill( pt, value, weightEtaBin.at( uPt ).at( uEntry ) );
              histVecPtFracL5MapEta.at( uPt )->Fill( eta, value, weightEtaBin.at( uPt ).at( uEntry ) );
              histFracL5MapPt->Fill( pt, value, weightEtaBin.at( uPt ).at( uEntry ) );
              histFracL5MapEta->Fill( eta, value, weightEtaBin.at( uPt ).at( uEntry ) );
            }
            if ( pt >= minPt_ && reco::deltaR( etaGenJetEtaBin.at( uPt ).at( uEntry ), phiGenJetEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDR_ ) {
              histEtaPtFracL5Restr->Fill( value, weightEtaBin.at( uPt ).at( uEntry ) );
              histVecPtFracL5Restr.at( uPt )->Fill( value, weightEtaBin.at( uPt ).at( uEntry ) );
              histEtaFracL5Restr->Fill( value, weightEtaBin.at( uPt ).at( uEntry ) );
              histFracL5Restr->Fill( value, weightEtaBin.at( uPt ).at( uEntry ) );
              histEtaFracL5RestrMapPt->Fill( pt, value, weightEtaBin.at( uPt ).at( uEntry ) );
              histVecPtFracL5RestrMapEta.at( uPt )->Fill( eta, value, weightEtaBin.at( uPt ).at( uEntry ) );
              histFracL5RestrMapPt->Fill( pt, value, weightEtaBin.at( uPt ).at( uEntry ) );
              histFracL5RestrMapEta->Fill( eta, value, weightEtaBin.at( uPt ).at( uEntry ) );
            }
          } // loop: uEntry < ptEtaBin.at( uPt ).size()

        } // loop: uPt < nPtBins_

        if ( plot_ ) {
          if ( fitNonRestr_ ) {
            histEtaFracL5MapPt->Draw();
            c1.Print( std::string( pathPlots_ + histEtaFracL5MapPt->GetName() + ".png" ).c_str() );
          }
          histEtaFracL5RestrMapPt->Draw();
          c1.Print( std::string( pathPlots_ + histEtaFracL5RestrMapPt->GetName() + ".png" ).c_str() );
        }

      } // loop: keyEta

      if ( plot_ ) {
        if ( fitNonRestr_ ) {
          histFracL5MapPt->Draw();
          c1.Print( std::string( pathPlots_ + histFracL5MapPt->GetName() + ".png" ).c_str() );
          histFracL5MapEta->Draw();
          c1.Print( std::string( pathPlots_ + histFracL5MapEta->GetName() + ".png" ).c_str() );
          for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
            histVecPtFracL5MapEta.at( uPt )->Draw();
            c1.Print( std::string( pathPlots_ + histVecPtFracL5MapEta.at( uPt )->GetName() + ".png" ).c_str() );
          }
        }
        histFracL5RestrMapPt->Draw();
        c1.Print( std::string( pathPlots_ + histFracL5RestrMapPt->GetName() + ".png" ).c_str() );
        histFracL5RestrMapEta->Draw();
        c1.Print( std::string( pathPlots_ + histFracL5RestrMapEta->GetName() + ".png" ).c_str() );
        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          histVecPtFracL5RestrMapEta.at( uPt )->Draw();
          c1.Print( std::string( pathPlots_ + histVecPtFracL5RestrMapEta.at( uPt )->GetName() + ".png" ).c_str() );
        }
      }

      dirFit_->cd();

      if ( sizeEtaBins != nEtaBins_ ) {
        std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                  << "    mismatch of eta binning for object category '" << objCat << "':" << std::endl
                  << "        bins in directory structure: " << sizeEtaBins                << std::endl
                  << "        bins in binning histogram  : " << nEtaBins_                  << std::endl;
        returnStatus_ += 0x100000;
      }

    } // loop: keyFit


    // Fit JECs

    if ( doFit_ ) {
      if ( verbose_ > 1 ) {
        std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                  << "    L5 JEC determination for " << objCat << " started" << std::endl;
      }

      const JetCorrectorParameters::Definitions jecDefinitions( jecVars_, jecDims_, jecFunction_, false );

      // Loop over fit versions
      nextInListProp.Reset();
      while ( TKey * keyFit = ( TKey* )nextInListProp() ) {
        if ( std::string( keyFit->GetClassName() ) != nameDirClass ) continue;
        const std::string subFit( keyFit->GetName() );
        if ( subFit.find( "Inv" ) != std::string::npos ) continue; // nothing to do for inverse
        // These are real switches: depending on configuration, only one setting combination can be run at a time
        if ( useAlt_     == ( subFit.find( "Alt" )  == std::string::npos ) ) continue;
        if ( useSymm_    == ( subFit.find( "Symm" ) == std::string::npos ) ) continue;
        if ( refGenJet_  == ( subFit.find( "Gen" )  == std::string::npos ) ) continue;
        TDirectory * dirFit_( ( TDirectory* )( dirPt_->Get( subFit.c_str() ) ) );
        TDirectory * dirOutFit_( ( TDirectory* )( dirOutPt_->Get( subFit.c_str() ) ) );
        if ( ! dirOutFit_ ) {
          dirOutPt_->cd();
          dirOutFit_ = new TDirectory( subFit.c_str(), "" );
        }
        dirFit_->cd();

        const std::string name( objCat + "_" + baseTitlePt + "_" + subFit );

        std::vector< JetCorrectorParameters::Record > jecRecords;

        std::vector< float > jecVarsMins( nVar_, useSymm_ ? ( float )( -etaBins_.back() ) : ( float )( etaBins_.front() ) );
        std::vector< float > jecVarsMaxs( nVar_, ( float )( etaBins_.back() ) );

        std::vector< float > jecPars( nPar_, 0. );

        const std::string nameFracL5( name + "_FracL5" );
        TH1D * histFracL5( ( TH1D* )( dirFit_->Get( nameFracL5.c_str() ) ) );
        if ( fitNonRestr_ && histFracL5 != 0 ) {
          const std::string nameFracL5Fit( nameFracL5 + "_fit" );
          TF1 * fitFracL5( new TF1( nameFracL5Fit.c_str(), fitFunction_.c_str(), std::max( histFracL5->GetXaxis()->GetXmin(), histFracL5->GetMean() - histFracL5->GetRMS() * fitRange_ ), std::min( histFracL5->GetXaxis()->GetXmax(), histFracL5->GetMean() + histFracL5->GetRMS() * fitRange_ ) ) );
          setParametersFit( fitFracL5, histFracL5, useBkg_ );
          TFitResultPtr fitFracL5ResultPtr( histFracL5->Fit( fitFracL5, fitOptions_.c_str() ) );
          if ( fitFracL5ResultPtr >= 0 ) {
            if ( fitFracL5ResultPtr->Status() == 0 && fitFracL5ResultPtr->Ndf() != 0. ) {
              jecPars.at( 0 ) = ( float)( fitFracL5->GetParameter( 1 ) );
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << nameFracL5 << "' status " << fitFracL5ResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; dirFit_->pwd();
              std::cout << "    '" << nameFracL5 << std::endl;
            }
          }
          if ( plot_ ) {
            histFracL5->Draw();
            c1.Print( std::string( pathPlots_ + histFracL5->GetName() + ".png" ).c_str() );
          }
        }

        JetCorrectorParameters::Record jecRecord( nVar_, jecVarsMins, jecVarsMaxs, jecPars );
        jecRecords.push_back( jecRecord );

        JetCorrectorParameters jec( jecDefinitions, jecRecords );
        if ( writeFiles_ && fitNonRestr_ ) {
          std::string nameOut( pathOut_ + "/gentJecL5_" + sample_ );
          if ( usePileUp_ ) nameOut.append( "_PileUp" );
          if ( refSel_)     nameOut.append( "_Ref" );
          nameOut.append( "_" + name + ".txt" );
          jec.printFile( nameOut );
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    written L5 JEC file:" << std::endl
                    << "        " << nameOut << std::endl;
        }

        std::vector< JetCorrectorParameters::Record > jecRecordsRestr;

        std::vector< float > jecParsRestr( nPar_, 0. );

        const std::string nameFracL5Restr( nameFracL5 + "Restr" );
        TH1D * histFracL5Restr( ( TH1D* )( dirFit_->Get( nameFracL5Restr.c_str() ) ) );
        if ( histFracL5Restr != 0 ) {
          const std::string nameFracL5RestrFit( nameFracL5Restr + "_fit" );
          TF1 * fitFracL5Restr( new TF1( nameFracL5RestrFit.c_str(), fitFunction_.c_str(), std::max( histFracL5Restr->GetXaxis()->GetXmin(), histFracL5Restr->GetMean() - histFracL5Restr->GetRMS() * fitRange_ ), std::min( histFracL5Restr->GetXaxis()->GetXmax(), histFracL5Restr->GetMean() + histFracL5Restr->GetRMS() * fitRange_ ) ) );
          setParametersFit( fitFracL5Restr, histFracL5Restr, useBkg_ );
          TFitResultPtr fitFracL5RestrResultPtr( histFracL5Restr->Fit( fitFracL5Restr, fitOptions_.c_str() ) );
          if ( fitFracL5RestrResultPtr >= 0 ) {
            if ( fitFracL5RestrResultPtr->Status() == 0 && fitFracL5RestrResultPtr->Ndf() != 0. ) {
              jecParsRestr.at( 0 ) = ( float)( fitFracL5Restr->GetParameter( 1 ) );
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << nameFracL5Restr << "' status " << fitFracL5RestrResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; dirFit_->pwd();
              std::cout << "    '" << nameFracL5Restr << std::endl;
            }
          }
          if ( plot_ ) {
            histFracL5Restr->Draw();
            c1.Print( std::string( pathPlots_ + histFracL5Restr->GetName() + ".png" ).c_str() );
          }
        }

        JetCorrectorParameters::Record jecRecordRestr( nVar_, jecVarsMins, jecVarsMaxs, jecParsRestr );
        jecRecordsRestr.push_back( jecRecordRestr );

        JetCorrectorParameters jecRestr( jecDefinitions, jecRecordsRestr );
        if ( writeFiles_ ) {
          std::string nameOutRestr( pathOut_ + "/gentJecL5_" + sample_ );
          if ( usePileUp_ ) nameOutRestr.append( "_PileUp" );
          if ( refSel_)     nameOutRestr.append( "_Ref" );
          nameOutRestr.append( "_" + name + "_Restr.txt" );
          jecRestr.printFile( nameOutRestr );
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    written L5 JEC file:" << std::endl
                    << "        " << nameOutRestr << std::endl;
        }

        // Loop over pt bins

        std::vector< TH1D * > histVecFracL5PtFitMap;
        std::vector< TH1D * > histVecFracL5RestrPtFitMap;
        for ( unsigned uParFit = 0; uParFit < nParFit_; ++uParFit ) {
          const std::string parFit( boost::lexical_cast< std::string >( uParFit ) );
          const std::string nameFracL5PtFitMap( nameFracL5 + baseTitlePt + "_FitMap_Par" + parFit );
          const std::string titleFracL5PtFitMap( objCat + ", par. " + parFit );
          TH1D * histFracL5PtFitMap( new TH1D( nameFracL5PtFitMap.c_str(), titleFracL5PtFitMap.c_str(), nPtBins_, ptBins_.data() ) );
          histFracL5PtFitMap->SetXTitle( titlePt.c_str() );
          histVecFracL5PtFitMap.push_back( histFracL5PtFitMap );
          const std::string nameFracL5RestrPtFitMap( nameFracL5Restr + baseTitlePt + "_FitMap_Par" + parFit );
          TH1D * histFracL5RestrPtFitMap( new TH1D( nameFracL5RestrPtFitMap.c_str(), titleFracL5PtFitMap.c_str(), nPtBins_, ptBins_.data() ) );
          histFracL5RestrPtFitMap->SetXTitle( titlePt.c_str() );
          histVecFracL5RestrPtFitMap.push_back( histFracL5RestrPtFitMap );
        }
        const std::string titleFracL5PtFitMapProb( objCat + ", fit prob." );
        const std::string nameFracL5PtFitMapProb( nameFracL5 + baseTitlePt + "_FitMap_Prob" );
        TH1D * histFracL5PtFitMapProb( new TH1D( nameFracL5PtFitMapProb.c_str(), titleFracL5PtFitMapProb.c_str(), nPtBins_, ptBins_.data() ) );
        histFracL5PtFitMapProb->SetXTitle( titlePt.c_str() );
        histFracL5PtFitMapProb->SetYTitle( titleProb.c_str() );
        const std::string nameFracL5RestrPtFitMapProb( nameFracL5Restr + baseTitlePt + "_FitMap_Prob" );
        TH1D * histFracL5RestrPtFitMapProb( new TH1D( nameFracL5RestrPtFitMapProb.c_str(), titleFracL5PtFitMapProb.c_str(), nPtBins_, ptBins_.data() ) );
        histFracL5RestrPtFitMapProb->SetXTitle( titlePt.c_str() );
        histFracL5RestrPtFitMapProb->SetYTitle( titleProb.c_str() );

        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
          const std::string namePt( name + "_" + baseTitlePt + binPt );

          const std::string namePtFracL5( namePt + "_FracL5" );
          TH1D * histPtFracL5( ( TH1D* )( dirFit_->Get( namePtFracL5.c_str() ) ) );
          if ( fitNonRestr_ && histPtFracL5 != 0 ) {
            const std::string namePtFracL5Fit( namePtFracL5 + "_fit" );
            TF1 * fitPtFracL5( new TF1( namePtFracL5Fit.c_str(), fitFunction_.c_str(), std::max( histPtFracL5->GetXaxis()->GetXmin(), histPtFracL5->GetMean() - histPtFracL5->GetRMS() * fitRange_ ), std::min( histPtFracL5->GetXaxis()->GetXmax(), histPtFracL5->GetMean() + histPtFracL5->GetRMS() * fitRange_ ) ) );
            setParametersFit( fitPtFracL5, histPtFracL5, useBkg_ );
            TFitResultPtr fitPtFracL5ResultPtr( histPtFracL5->Fit( fitPtFracL5, fitOptions_.c_str() ) );
            if ( fitPtFracL5ResultPtr >= 0 ) {
              if ( fitPtFracL5ResultPtr->Status() == 0 && fitPtFracL5ResultPtr->Ndf() != 0. ) {
                for ( unsigned uParFit = 0; uParFit < nParFit_; ++uParFit ) {
                  histVecFracL5PtFitMap.at( uParFit )->SetBinContent( uPt + 1, fitPtFracL5ResultPtr->Parameter( uParFit ) );
                  histVecFracL5PtFitMap.at( uParFit )->SetBinError( uPt + 1, fitPtFracL5ResultPtr->ParError( uParFit ) );
                }
                if ( fitPtFracL5ResultPtr->Prob() > 0 ) histFracL5PtFitMapProb->SetBinContent( uPt + 1, log10( fitPtFracL5ResultPtr->Prob() ) );
                else histFracL5PtFitMapProb->SetBinContent( uPt + 1, 1. );
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; dirFit_->pwd();
                  std::cout << "    '" << namePtFracL5 << "' status " << fitPtFracL5ResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << namePtFracL5 << std::endl;
              }
            }
          }

          const std::string namePtFracL5Restr( namePt + "_FracL5Restr" );
          TH1D * histPtFracL5Restr( ( TH1D* )( dirFit_->Get( namePtFracL5Restr.c_str() ) ) );
          if ( histPtFracL5Restr != 0 ) {
            const std::string namePtFracL5RestrFit( namePtFracL5Restr + "_fit" );
            TF1 * fitPtFracL5Restr( new TF1( namePtFracL5RestrFit.c_str(), fitFunction_.c_str(), std::max( histPtFracL5Restr->GetXaxis()->GetXmin(), histPtFracL5Restr->GetMean() - histPtFracL5Restr->GetRMS() * fitRange_ ), std::min( histPtFracL5Restr->GetXaxis()->GetXmax(), histPtFracL5Restr->GetMean() + histPtFracL5Restr->GetRMS() * fitRange_ ) ) );
            setParametersFit( fitPtFracL5Restr, histPtFracL5Restr, useBkg_ );
            TFitResultPtr fitPtFracL5RestrResultPtr( histPtFracL5Restr->Fit( fitPtFracL5Restr, fitOptions_.c_str() ) );
            if ( fitPtFracL5RestrResultPtr >= 0 ) {
              if ( fitPtFracL5RestrResultPtr->Status() == 0 && fitPtFracL5RestrResultPtr->Ndf() != 0. ) {
                for ( unsigned uParFit = 0; uParFit < nParFit_; ++uParFit ) {
                  histVecFracL5RestrPtFitMap.at( uParFit )->SetBinContent( uPt + 1, fitPtFracL5RestrResultPtr->Parameter( uParFit ) );
                  histVecFracL5RestrPtFitMap.at( uParFit )->SetBinError( uPt + 1, fitPtFracL5RestrResultPtr->ParError( uParFit ) );
                }
                if ( fitPtFracL5RestrResultPtr->Prob() > 0 ) histFracL5RestrPtFitMapProb->SetBinContent( uPt + 1, log10( fitPtFracL5RestrResultPtr->Prob() ) );
                else histFracL5RestrPtFitMapProb->SetBinContent( uPt + 1, 1. );
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; dirFit_->pwd();
                  std::cout << "    '" << namePtFracL5Restr << "' status " << fitPtFracL5RestrResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << namePtFracL5Restr << std::endl;
              }
            }
          }

          if ( plot_ ) {
            if ( fitNonRestr_ ) {
              histPtFracL5->Draw();
              c1.Print( std::string( pathPlots_ + histPtFracL5->GetName() + ".png" ).c_str() );
            }
            histPtFracL5Restr->Draw();
            c1.Print( std::string( pathPlots_ + histPtFracL5Restr->GetName() + ".png" ).c_str() );
          }

        } // loop: uPt < nPtBins_

        std::vector< JetCorrectorParameters::Record > jecRecordsPt;
        std::vector< JetCorrectorParameters::Record > jecRecordsPtRestr;

        std::vector< float > jecParsPt( nPar_, 0. );
        std::vector< float > jecParsPtRestr( nPar_, 0. );

        for ( unsigned uParFit = 0; uParFit < nParFit_; ++uParFit ) {
          const std::string parFit( boost::lexical_cast< std::string >( uParFit ) );

          if ( fitNonRestr_ ) {
            const std::string nameFracL5PtFitMap( name + "_FracL5" + baseTitlePt + "_FitMap_Par" + parFit );
            const std::string nameFracL5PtFitMapFit( nameFracL5PtFitMap + "_fit" );
            TF1 * fitFracL5PtFitMap( new TF1( nameFracL5PtFitMapFit.c_str(), jecFunction_.c_str(), ptBins_.front(), ptBins_.back() ) );
            fitFracL5PtFitMap->SetParameter( 0, 1. );
            TFitResultPtr fitFracL5PtFitMapResultPtr( histVecFracL5PtFitMap.at( uParFit )->Fit( fitFracL5PtFitMap, fitOptions_.c_str() ) );
            if ( fitFracL5PtFitMapResultPtr >= 0 ) {
              if ( fitFracL5PtFitMapResultPtr->Status() == 0 && fitFracL5PtFitMapResultPtr->Ndf() != 0. ) {
                if ( uParFit == 1 ) {
                  for ( size_t uPar = 0; uPar < nPar_; ++uPar ) {
                    jecParsPt.at( uPar ) = ( float)( fitFracL5PtFitMap->GetParameter( uPar ) );
                  }
                }
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; dirFit_->pwd();
                  std::cout << "    '" << nameFracL5PtFitMap << "' status " << fitFracL5PtFitMapResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << nameFracL5PtFitMap << std::endl;
              }
            }
          }

          const std::string nameFracL5RestrPtFitMap( name + "_FracL5Restr" + baseTitlePt + "_FitMap_Par" + parFit );
          const std::string nameFracL5RestrPtFitMapFit( nameFracL5RestrPtFitMap + "_fit" );
          TF1 * fitFracL5RestrPtFitMap( new TF1( nameFracL5RestrPtFitMapFit.c_str(), jecFunction_.c_str(), ptBins_.front(), ptBins_.back() ) );
          fitFracL5RestrPtFitMap->SetParameter( 0, 1. );
          TFitResultPtr fitFracL5RestrPtFitMapResultPtr( histVecFracL5RestrPtFitMap.at( uParFit )->Fit( fitFracL5RestrPtFitMap, fitOptions_.c_str() ) );
          if ( fitFracL5RestrPtFitMapResultPtr >= 0 ) {
            if ( fitFracL5RestrPtFitMapResultPtr->Status() == 0 && fitFracL5RestrPtFitMapResultPtr->Ndf() != 0. ) {
              if ( uParFit == 1 ) {
                for ( size_t uPar = 0; uPar < nPar_; ++uPar ) {
                  jecParsPtRestr.at( uPar ) = ( float)( fitFracL5RestrPtFitMap->GetParameter( uPar ) );
                }
              }
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << nameFracL5RestrPtFitMap << "' status " << fitFracL5RestrPtFitMapResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; dirFit_->pwd();
              std::cout << "    '" << nameFracL5RestrPtFitMap << std::endl;
            }
          }
        } // loop: uParFit < nParFit_

        if ( plot_ ) {
          for ( unsigned uParFit = 0; uParFit < nParFit_; ++uParFit ) {
            if ( fitNonRestr_ ) {
              histVecFracL5PtFitMap.at( uParFit )->Draw();
              c1.Print( std::string( pathPlots_ + histVecFracL5PtFitMap.at( uParFit )->GetName() + ".png" ).c_str() );
            }
            histVecFracL5RestrPtFitMap.at( uParFit )->Draw();
            c1.Print( std::string( pathPlots_ + histVecFracL5RestrPtFitMap.at( uParFit )->GetName() + ".png" ).c_str() );
          }
          if ( fitNonRestr_ ) {
            histFracL5PtFitMapProb->Draw();
            c1.Print( std::string( pathPlots_ + histFracL5PtFitMapProb->GetName() + ".png" ).c_str() );
          }
          histFracL5RestrPtFitMapProb->Draw();
          c1.Print( std::string( pathPlots_ + histFracL5RestrPtFitMapProb->GetName() + ".png" ).c_str() );
        }

        JetCorrectorParameters::Record jecRecordPt( nVar_, jecVarsMins, jecVarsMaxs, jecParsPt );
        jecRecordsPt.push_back( jecRecordPt );
        JetCorrectorParameters::Record jecRecordPtRestr( nVar_, jecVarsMins, jecVarsMaxs, jecParsPtRestr );
        jecRecordsPtRestr.push_back( jecRecordPtRestr );

        JetCorrectorParameters jecPt( jecDefinitions, jecRecordsPt );
        if ( writeFiles_ && fitNonRestr_ ) {
          std::string nameOutPt( pathOut_ + "/gentJecL5_" + sample_ );
          if ( usePileUp_ ) nameOutPt.append( "_PileUp" );
          if ( refSel_)     nameOutPt.append( "_Ref" );
          nameOutPt.append( "_" + name + "_Pt.txt" );
          jecPt.printFile( nameOutPt );
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    written L5 JEC file:" << std::endl
                    << "        " << nameOutPt << std::endl;
        }
        JetCorrectorParameters jecPtRestr( jecDefinitions, jecRecordsPtRestr );
        if ( writeFiles_ ) {
          std::string nameOutPtRestr( pathOut_ + "/gentJecL5_" + sample_ );
          if ( usePileUp_ ) nameOutPtRestr.append( "_PileUp" );
          if ( refSel_)     nameOutPtRestr.append( "_Ref" );
          nameOutPtRestr.append( "_" + name + "_PtRestr.txt" );
          jecPtRestr.printFile( nameOutPtRestr );
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    written L5 JEC file:" << std::endl
                    << "        " << nameOutPtRestr << std::endl;
        }

        // Loop over eta bins

        std::vector< JetCorrectorParameters::Record > jecRecordsEta( useSymm_ ? 2 * nEtaBins_ : nEtaBins_ );
        std::vector< JetCorrectorParameters::Record > jecRecordsEtaRestr( useSymm_ ? 2 * nEtaBins_ : nEtaBins_ );

        std::vector< JetCorrectorParameters::Record > jecRecordsEtaPt( useSymm_ ? 2 * nEtaBins_ : nEtaBins_ );
        std::vector< JetCorrectorParameters::Record > jecRecordsEtaPtRestr( useSymm_ ? 2 * nEtaBins_ : nEtaBins_ );

        std::vector< TH1D * > histVecFracL5EtaFitMap;
        std::vector< TH1D * > histVecFracL5RestrEtaFitMap;
        for ( unsigned uParFit = 0; uParFit < nParFit_; ++uParFit ) {
          const std::string parFit( boost::lexical_cast< std::string >( uParFit ) );
          const std::string nameFracL5EtaFitMap( nameFracL5 + "Eta_FitMap_Par" + parFit );
          const std::string titleFracL5EtaFitMap( objCat + ", par. " + parFit );
          TH1D * histFracL5EtaFitMap( new TH1D( nameFracL5EtaFitMap.c_str(), titleFracL5EtaFitMap.c_str(), nEtaBins_, etaBins_.data() ) );
          histFracL5EtaFitMap->SetXTitle( titleEta.c_str() );
          histVecFracL5EtaFitMap.push_back( histFracL5EtaFitMap );
          const std::string nameFracL5RestrEtaFitMap( name + "_FracL5RestrEta_FitMap_Par" + parFit );
          TH1D * histFracL5RestrEtaFitMap( new TH1D( nameFracL5RestrEtaFitMap.c_str(), titleFracL5EtaFitMap.c_str(), nEtaBins_, etaBins_.data() ) );
          histFracL5RestrEtaFitMap->SetXTitle( titleEta.c_str() );
          histVecFracL5RestrEtaFitMap.push_back( histFracL5RestrEtaFitMap );
        }
        const std::string titleFracL5EtaFitMapProb( objCat + ", fit prob." );
        const std::string nameFracL5EtaFitMapProb( nameFracL5 + "Eta_FitMap_Prob" );
        TH1D * histFracL5EtaFitMapProb( new TH1D( nameFracL5EtaFitMapProb.c_str(), titleFracL5EtaFitMapProb.c_str(), nEtaBins_, etaBins_.data() ) );
        histFracL5EtaFitMapProb->SetXTitle( titleEta.c_str() );
        histFracL5EtaFitMapProb->SetYTitle( titleProb.c_str() );
        const std::string nameFracL5RestrEtaFitMapProb( nameFracL5Restr + "Eta_FitMap_Prob" );
        TH1D * histFracL5RestrEtaFitMapProb( new TH1D( nameFracL5RestrEtaFitMapProb.c_str(), titleFracL5EtaFitMapProb.c_str(), nEtaBins_, etaBins_.data() ) );
        histFracL5RestrEtaFitMapProb->SetXTitle( titleEta.c_str() );
        histFracL5RestrEtaFitMapProb->SetYTitle( titleProb.c_str() );

        std::vector< TH1D * > histVecFracL5EtaParAMap;
        std::vector< TH1D * > histVecFracL5EtaParBMap;
        std::vector< TH1D * > histVecFracL5RestrEtaParAMap;
        std::vector< TH1D * > histVecFracL5RestrEtaParBMap;
        for ( unsigned uParFit = 0; uParFit < nParFit_; ++uParFit ) {
          const std::string parFit( boost::lexical_cast< std::string >( uParFit ) );
          const std::string nameFracL5EtaParMap( name + "_FracL5Eta_ParMap_Par" + parFit );
          const std::string titleFracL5EtaParMap( objCat + ", par. " + parFit + ", " );
          TH1D * histFracL5EtaParAMap( new TH1D( std::string( nameFracL5EtaParMap + "-A" ).c_str(), std::string( titleFracL5EtaParMap + "A" ).c_str(), nEtaBins_, etaBins_.data() ) );
          histFracL5EtaParAMap->SetXTitle( titleEta.c_str() );
          histVecFracL5EtaParAMap.push_back( histFracL5EtaParAMap );
          TH1D * histFracL5EtaParBMap( new TH1D( std::string( nameFracL5EtaParMap + "-B" ).c_str(), std::string( titleFracL5EtaParMap + "B" ).c_str(), nEtaBins_, etaBins_.data() ) );
          histFracL5EtaParBMap->SetXTitle( titleEta.c_str() );
          histVecFracL5EtaParBMap.push_back( histFracL5EtaParBMap );
          const std::string nameFracL5RestrEtaParMap( name + "_FracL5RestrEta_ParMap_Par" + parFit );
          TH1D * histFracL5RestrEtaParAMap( new TH1D( std::string( nameFracL5RestrEtaParMap + "-A" ).c_str(), std::string( titleFracL5EtaParMap + "A" ).c_str(), nEtaBins_, etaBins_.data() ) );
          histFracL5RestrEtaParAMap->SetXTitle( titleEta.c_str() );
          histVecFracL5RestrEtaParAMap.push_back( histFracL5RestrEtaParAMap );
          TH1D * histFracL5RestrEtaParBMap( new TH1D( std::string( nameFracL5RestrEtaParMap + "-B" ).c_str(), std::string( titleFracL5EtaParMap + "B" ).c_str(), nEtaBins_, etaBins_.data() ) );
          histFracL5RestrEtaParBMap->SetXTitle( titleEta.c_str() );
          histVecFracL5RestrEtaParBMap.push_back( histFracL5RestrEtaParBMap );
        }

        TList * listFit( dirFit_->GetListOfKeys() );
        TIter nextInListFit( listFit );
        while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
          if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
          const std::string binEta( keyEta->GetName() );
          const unsigned uEta( std::atoi( binEta.substr( 3 ).data() ) );
          TDirectory * dirEta_( ( TDirectory* )( dirFit_->Get( binEta.c_str() ) ) );
          TDirectory * dirOutEta_( ( TDirectory* )( dirOutFit_->Get( binEta.c_str() ) ) );
          if ( ! dirOutEta_ ) {
            dirOutFit_->cd();
            dirOutEta_ = new TDirectory( binEta.c_str(), "" );
          }
          dirEta_->cd();

          const std::string nameEta( name + "_" + binEta );

          std::vector< float > jecVarsMinsEtaMinus( nVar_, ( float )( -etaBins_.at( uEta + 1 ) ) );
          std::vector< float > jecVarsMaxsEtaMinus( nVar_, ( float )( -etaBins_.at( uEta ) ) );
          std::vector< float > jecVarsMinsEtaPlus( nVar_, ( float )( etaBins_.at( uEta ) ) );
          std::vector< float > jecVarsMaxsEtaPlus( nVar_, ( float )( etaBins_.at( uEta + 1 ) ) );

          std::vector< float > jecParsEta( nPar_, 0. );
          std::vector< float > jecParsEtaRestr( nPar_, 0. );

          const std::string nameEtaFracL5( nameEta + "_FracL5" );
          if ( fitNonRestr_ ) {
            TH1D * histEtaFracL5( ( TH1D* )( dirEta_->Get( nameEtaFracL5.c_str() ) ) );
            if ( fitNonRestr_ && histEtaFracL5 != 0 ) {
              const std::string nameEtaFracL5Fit( nameEtaFracL5 + "_fit" );
              TF1 * fitEtaFracL5( new TF1( nameEtaFracL5Fit.c_str(), fitFunction_.c_str(), std::max( histEtaFracL5->GetXaxis()->GetXmin(), histEtaFracL5->GetMean() - histEtaFracL5->GetRMS() * fitRange_ ), std::min( histEtaFracL5->GetXaxis()->GetXmax(), histEtaFracL5->GetMean() + histEtaFracL5->GetRMS() * fitRange_ ) ) );
              setParametersFit( fitEtaFracL5, histEtaFracL5, useBkg_ );
              TFitResultPtr fitEtaFracL5ResultPtr( histEtaFracL5->Fit( fitEtaFracL5, fitOptions_.c_str() ) );
              if ( fitEtaFracL5ResultPtr >= 0 ) {
                if ( fitEtaFracL5ResultPtr->Status() == 0 && fitEtaFracL5ResultPtr->Ndf() != 0. ) {
                  for ( unsigned uParFit = 0; uParFit < nParFit_; ++uParFit ) {
                    histVecFracL5EtaFitMap.at( uParFit )->SetBinContent( uEta + 1, fitEtaFracL5ResultPtr->Parameter( uParFit ) );
                    histVecFracL5EtaFitMap.at( uParFit )->SetBinError( uEta + 1, fitEtaFracL5ResultPtr->ParError( uParFit ) );
                  }
                  if ( fitEtaFracL5ResultPtr->Prob() > 0 ) histFracL5EtaFitMapProb->SetBinContent( uEta + 1, log10( fitEtaFracL5ResultPtr->Prob() ) );
                  else histFracL5EtaFitMapProb->SetBinContent( uEta + 1, 1 );
                  jecParsEta.at( 0 ) = ( float)( fitEtaFracL5->GetParameter( 1 ) );
                }
                else {
                  if ( verbose_ > 2 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    failing fit in directory '"; dirEta_->pwd();
                    std::cout << "    '" << nameEtaFracL5 << "' status " << fitEtaFracL5ResultPtr->Status() << std::endl;
                  }
                }
              }
              else {
                if ( verbose_ > 1 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    missing fit in directory '"; dirEta_->pwd();
                  std::cout << "    '" << nameEtaFracL5 << std::endl;
                }
              }
            }
            if ( plot_ ) {
              histEtaFracL5->Draw();
              c1.Print( std::string( pathPlots_ + histEtaFracL5->GetName() + ".png" ).c_str() );
            }
          }

          const std::string nameEtaFracL5Restr( nameEta + "_FracL5Restr" );
          TH1D * histEtaFracL5Restr( ( TH1D* )( dirEta_->Get( nameEtaFracL5Restr.c_str() ) ) );
          if ( histEtaFracL5Restr != 0 ) {
            const std::string nameEtaFracL5RestrFit( nameEtaFracL5Restr + "_fit" );
            TF1 * fitEtaFracL5Restr( new TF1( nameEtaFracL5RestrFit.c_str(), fitFunction_.c_str(), std::max( histEtaFracL5Restr->GetXaxis()->GetXmin(), histEtaFracL5Restr->GetMean() - histEtaFracL5Restr->GetRMS() * fitRange_ ), std::min( histEtaFracL5Restr->GetXaxis()->GetXmax(), histEtaFracL5Restr->GetMean() + histEtaFracL5Restr->GetRMS() * fitRange_ ) ) );
            setParametersFit( fitEtaFracL5Restr, histEtaFracL5Restr, useBkg_ );
            TFitResultPtr fitEtaFracL5RestrResultPtr( histEtaFracL5Restr->Fit( fitEtaFracL5Restr, fitOptions_.c_str() ) );
            if ( fitEtaFracL5RestrResultPtr >= 0 ) {
              if ( fitEtaFracL5RestrResultPtr->Status() == 0 && fitEtaFracL5RestrResultPtr->Ndf() != 0. ) {
                for ( unsigned uParFit = 0; uParFit < nParFit_; ++uParFit ) {
                  histVecFracL5RestrEtaFitMap.at( uParFit )->SetBinContent( uEta + 1, fitEtaFracL5RestrResultPtr->Parameter( uParFit ) );
                  histVecFracL5RestrEtaFitMap.at( uParFit )->SetBinError( uEta + 1, fitEtaFracL5RestrResultPtr->ParError( uParFit ) );
                }
                if ( fitEtaFracL5RestrResultPtr->Prob() > 0 ) histFracL5RestrEtaFitMapProb->SetBinContent( uEta + 1, log10( fitEtaFracL5RestrResultPtr->Prob() ) );
                else histFracL5RestrEtaFitMapProb->SetBinContent( uEta + 1, 1 );
                jecParsEtaRestr.at( 0 ) = ( float)( fitEtaFracL5Restr->GetParameter( 1 ) );
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; dirEta_->pwd();
                  std::cout << "    '" << nameEtaFracL5Restr << "' status " << fitEtaFracL5RestrResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; dirEta_->pwd();
                std::cout << "    '" << nameEtaFracL5Restr << std::endl;
              }
            }
          }

          if ( plot_ ) {
            histEtaFracL5Restr->Draw();
            c1.Print( std::string( pathPlots_ + histEtaFracL5Restr->GetName() + ".png" ).c_str() );
          }

          if ( useSymm_ ) {
            JetCorrectorParameters::Record jecRecordEtaMinus( nVar_, jecVarsMinsEtaMinus, jecVarsMaxsEtaMinus, jecParsEta );
            jecRecordsEta.at( nEtaBins_ - uEta - 1 ) = jecRecordEtaMinus;
            JetCorrectorParameters::Record jecRecordEtaRestrMinus( nVar_, jecVarsMinsEtaMinus, jecVarsMaxsEtaMinus, jecParsEtaRestr );
            jecRecordsEtaRestr.at( nEtaBins_ - uEta - 1 ) = jecRecordEtaRestrMinus;
            JetCorrectorParameters::Record jecRecordEtaPlus( nVar_, jecVarsMinsEtaPlus, jecVarsMaxsEtaPlus, jecParsEta );
            jecRecordsEta.at( nEtaBins_ + uEta ) = jecRecordEtaPlus;
            JetCorrectorParameters::Record jecRecordEtaRestrPlus( nVar_, jecVarsMinsEtaPlus, jecVarsMaxsEtaPlus, jecParsEtaRestr );
            jecRecordsEtaRestr.at( nEtaBins_ + uEta ) = jecRecordEtaRestrPlus;
          }
          else {
            JetCorrectorParameters::Record jecRecordEta( nVar_, jecVarsMinsEtaPlus, jecVarsMaxsEtaPlus, jecParsEta );
            jecRecordsEta.at( uEta ) = jecRecordEta;
            JetCorrectorParameters::Record jecRecordEtaRestr( nVar_, jecVarsMinsEtaPlus, jecVarsMaxsEtaPlus, jecParsEtaRestr );
            jecRecordsEtaRestr.at( uEta ) = jecRecordEtaRestr;
          }

          // Loop over pt bins

          std::vector< float > jecParsEtaPt( nPar_, 0. );
          std::vector< float > jecParsEtaPtRestr( nPar_, 0. );

          std::vector< TH1D * > histVecFracL5EtaPtFitMap;
          std::vector< TH1D * > histVecFracL5RestrEtaPtFitMap;
          for ( unsigned uParFit = 0; uParFit < nParFit_; ++uParFit ) {
            const std::string parFit( boost::lexical_cast< std::string >( uParFit ) );
            const std::string nameFracL5EtaPtFitMap( nameEta + "_FracL5" + baseTitlePt + "_FitMap_Par" + parFit );
            const std::string titleFracL5EtaPtFitMap( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta < " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) + ", par. " + parFit );
            TH1D * histFracL5EtaPtFitMap( new TH1D( nameFracL5EtaPtFitMap.c_str(), titleFracL5EtaPtFitMap.c_str(), nPtBins_, ptBins_.data() ) );
            histFracL5EtaPtFitMap->SetXTitle( titlePt.c_str() );
            histVecFracL5EtaPtFitMap.push_back( histFracL5EtaPtFitMap );
            const std::string nameFracL5RestrEtaPtFitMap( nameEta + "_FracL5Restr" + baseTitlePt + "_FitMap_Par" + parFit );
            TH1D * histFracL5RestrEtaPtFitMap( new TH1D( nameFracL5RestrEtaPtFitMap.c_str(), titleFracL5EtaPtFitMap.c_str(), nPtBins_, ptBins_.data() ) );
            histFracL5RestrEtaPtFitMap->SetXTitle( titlePt.c_str() );
            histVecFracL5RestrEtaPtFitMap.push_back( histFracL5RestrEtaPtFitMap );
          }
          const std::string titleFracL5EtaPtFitMapProb( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta < " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) + ", fit prob." );
          const std::string nameFracL5EtaPtFitMapProb( nameEtaFracL5 + baseTitlePt + "_FitMap_Prob" );
          TH1D * histFracL5EtaPtFitMapProb( new TH1D( nameFracL5EtaPtFitMapProb.c_str(), titleFracL5EtaPtFitMapProb.c_str(), nPtBins_, ptBins_.data() ) );
          histFracL5EtaPtFitMapProb->SetXTitle( titlePt.c_str() );
          histFracL5EtaPtFitMapProb->SetYTitle( titleProb.c_str() );
          const std::string nameFracL5RestrEtaPtFitMapProb( nameEtaFracL5Restr + baseTitlePt + "_FitMap_Prob" );
          TH1D * histFracL5RestrEtaPtFitMapProb( new TH1D( nameFracL5RestrEtaPtFitMapProb.c_str(), titleFracL5EtaPtFitMapProb.c_str(), nPtBins_, ptBins_.data() ) );
          histFracL5RestrEtaPtFitMapProb->SetXTitle( titlePt.c_str() );
          histFracL5RestrEtaPtFitMapProb->SetYTitle( titleProb.c_str() );

          for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
            const std::string binPt( boost::lexical_cast< std::string >( uPt ) );

            const std::string nameEtaPt( nameEta + "_" + baseTitlePt + binPt );
            const std::string nameEtaPtFracL5( nameEtaPt + "_FracL5" );

            if ( fitNonRestr_ ) {
              TH1D * histEtaPtFracL5( ( TH1D* )( dirEta_->Get( nameEtaPtFracL5.c_str() ) ) );
              if ( histEtaPtFracL5 != 0 ) {
                const std::string nameEtaPtFracL5Fit( nameEtaPtFracL5 + "_fit" );
                TF1 * fitEtaPtFracL5( new TF1( nameEtaPtFracL5Fit.c_str(), fitFunction_.c_str(), std::max( histEtaPtFracL5->GetXaxis()->GetXmin(), histEtaPtFracL5->GetMean() - histEtaPtFracL5->GetRMS() * fitRange_ ), std::min( histEtaPtFracL5->GetXaxis()->GetXmax(), histEtaPtFracL5->GetMean() + histEtaPtFracL5->GetRMS() * fitRange_ ) ) );
                setParametersFit( fitEtaPtFracL5, histEtaPtFracL5, useBkg_ );
                TFitResultPtr fitEtaPtFracL5ResultPtr( histEtaPtFracL5->Fit( fitEtaPtFracL5, fitOptions_.c_str() ) );
                if ( fitEtaPtFracL5ResultPtr >= 0 ) {
                  if ( fitEtaPtFracL5ResultPtr->Status() == 0 && fitEtaPtFracL5ResultPtr->Ndf() != 0. ) {
                    for ( unsigned uParFit = 0; uParFit < nParFit_; ++uParFit ) {
                      histVecFracL5EtaPtFitMap.at( uParFit )->SetBinContent( uPt + 1, fitEtaPtFracL5ResultPtr->Parameter( uParFit ) );
                      histVecFracL5EtaPtFitMap.at( uParFit )->SetBinError( uPt + 1, fitEtaPtFracL5ResultPtr->ParError( uParFit ) );
                    }
                    if ( fitEtaPtFracL5ResultPtr->Prob() > 0 ) histFracL5EtaPtFitMapProb->SetBinContent( uPt + 1, log10( fitEtaPtFracL5ResultPtr->Prob() ) );
                    else histFracL5EtaPtFitMapProb->SetBinContent( uPt + 1, 1. );
                  }
                  else {
                    if ( verbose_ > 2 ) {
                      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                                << "    failing fit in directory '"; dirEta_->pwd();
                      std::cout << "    '" << nameEtaPtFracL5 << "' status " << fitEtaPtFracL5ResultPtr->Status() << std::endl;
                    }
                  }
                }
                else {
                  if ( verbose_ > 1 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    missing fit in directory '"; dirEta_->pwd();
                    std::cout << "    '" << nameEtaPtFracL5 << std::endl;
                  }
                }
                if ( plot_ ) {
                  histEtaPtFracL5->Draw();
                  c1.Print( std::string( pathPlots_ + histEtaPtFracL5->GetName() + ".png" ).c_str() );
                }
              }
            }

            const std::string nameEtaPtFracL5Restr( nameEtaPtFracL5 + "Restr" );
            TH1D * histEtaPtFracL5Restr( ( TH1D* )( dirEta_->Get( nameEtaPtFracL5Restr.c_str() ) ) );
            if ( histEtaPtFracL5Restr != 0 ) {
              const std::string nameEtaPtFracL5RestrFit( nameEtaPtFracL5Restr + "_fit" );
              TF1 * fitEtaPtFracL5Restr( new TF1( nameEtaPtFracL5RestrFit.c_str(), fitFunction_.c_str(), std::max( histEtaPtFracL5Restr->GetXaxis()->GetXmin(), histEtaPtFracL5Restr->GetMean() - histEtaPtFracL5Restr->GetRMS() * fitRange_ ), std::min( histEtaPtFracL5Restr->GetXaxis()->GetXmax(), histEtaPtFracL5Restr->GetMean() + histEtaPtFracL5Restr->GetRMS() * fitRange_ ) ) );
              setParametersFit( fitEtaPtFracL5Restr, histEtaPtFracL5Restr, useBkg_ );
              TFitResultPtr fitEtaPtFracL5RestrResultPtr( histEtaPtFracL5Restr->Fit( fitEtaPtFracL5Restr, fitOptions_.c_str() ) );
              if ( fitEtaPtFracL5RestrResultPtr >= 0 ) {
                if ( fitEtaPtFracL5RestrResultPtr->Status() == 0 && fitEtaPtFracL5RestrResultPtr->Ndf() != 0. ) {
                  for ( unsigned uParFit = 0; uParFit < nParFit_; ++uParFit ) {
                    histVecFracL5RestrEtaPtFitMap.at( uParFit )->SetBinContent( uPt + 1, fitEtaPtFracL5RestrResultPtr->Parameter( uParFit ) );
                    histVecFracL5RestrEtaPtFitMap.at( uParFit )->SetBinError( uPt + 1, fitEtaPtFracL5RestrResultPtr->ParError( uParFit ) );
                  }
                  if ( fitEtaPtFracL5RestrResultPtr->Prob() > 0 ) histFracL5RestrEtaPtFitMapProb->SetBinContent( uPt + 1, log10( fitEtaPtFracL5RestrResultPtr->Prob() ) );
                  else histFracL5RestrEtaPtFitMapProb->SetBinContent( uPt + 1, 1. );
                }
                else {
                  if ( verbose_ > 2 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    failing fit in directory '"; dirEta_->pwd();
                    std::cout << "    '" << nameEtaPtFracL5Restr << "' status " << fitEtaPtFracL5RestrResultPtr->Status() << std::endl;
                  }
                }
              }
              else {
                if ( verbose_ > 1 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    missing fit in directory '"; dirEta_->pwd();
                  std::cout << "    '" << nameEtaPtFracL5Restr << std::endl;
                }
              }
              if ( plot_ ) {
                histEtaPtFracL5Restr->Draw();
                c1.Print( std::string( pathPlots_ + histEtaPtFracL5Restr->GetName() + ".png" ).c_str() );
              }
            }

          } // loop: uPt < nPtBins_

        for ( unsigned uParFit = 0; uParFit < nParFit_; ++uParFit ) {
          const std::string parFit( boost::lexical_cast< std::string >( uParFit ) );

          if ( fitNonRestr_ ) {
            const std::string nameFracL5EtaPtFitMap( name + "_FracL5Eta" + baseTitlePt + "_FitMap_Par" + parFit );
            const std::string nameFracL5EtaPtFitMapFit( nameFracL5EtaPtFitMap + "_fit" );
            TF1 * fitFracL5EtaPtFitMap( new TF1( nameFracL5EtaPtFitMapFit.c_str(), jecFunction_.c_str(), ptBins_.front(), ptBins_.back() ) );
            fitFracL5EtaPtFitMap->SetParameter( 0, 1. );
            TFitResultPtr fitFracL5EtaPtFitMapResultPtr( histVecFracL5EtaPtFitMap.at( uParFit )->Fit( fitFracL5EtaPtFitMap, fitOptions_.c_str() ) );
            if ( fitFracL5EtaPtFitMapResultPtr >= 0 ) {
              if ( fitFracL5EtaPtFitMapResultPtr->Status() == 0 && fitFracL5EtaPtFitMapResultPtr->Ndf() != 0. ) {
                if ( uParFit == 1 ) {
                  for ( size_t uPar = 0; uPar < nPar_; ++uPar ) {
                    jecParsEtaPt.at( uPar ) = ( float)( fitFracL5EtaPtFitMap->GetParameter( uPar ) );
                  }
                }
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; dirFit_->pwd();
                  std::cout << "    '" << nameFracL5EtaPtFitMap << "' status " << fitFracL5EtaPtFitMapResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << nameFracL5EtaPtFitMap << std::endl;
              }
            }
          }

          const std::string nameFracL5RestrEtaPtFitMap( name + "_FracL5RestrEta" + baseTitlePt + "_FitMap_Par" + parFit );
          const std::string nameFracL5RestrEtaPtFitMapFit( nameFracL5RestrEtaPtFitMap + "_fit" );
          TF1 * fitFracL5RestrEtaPtFitMap( new TF1( nameFracL5RestrEtaPtFitMapFit.c_str(), jecFunction_.c_str(), ptBins_.front(), ptBins_.back() ) );
          fitFracL5RestrEtaPtFitMap->SetParameter( 0, 1. );
          TFitResultPtr fitFracL5RestrEtaPtFitMapResultPtr( histVecFracL5RestrEtaPtFitMap.at( uParFit )->Fit( fitFracL5RestrEtaPtFitMap, fitOptions_.c_str() ) );
          if ( fitFracL5RestrEtaPtFitMapResultPtr >= 0 ) {
            if ( fitFracL5RestrEtaPtFitMapResultPtr->Status() == 0 && fitFracL5RestrEtaPtFitMapResultPtr->Ndf() != 0. ) {
              if ( uParFit == 1 ) {
                for ( size_t uPar = 0; uPar < nPar_; ++uPar ) {
                  jecParsEtaPtRestr.at( uPar ) = ( float)( fitFracL5RestrEtaPtFitMap->GetParameter( uPar ) );
                }
              }
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << nameFracL5RestrEtaPtFitMap << "' status " << fitFracL5RestrEtaPtFitMapResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; dirFit_->pwd();
              std::cout << "    '" << nameFracL5RestrEtaPtFitMap << std::endl;
            }
          }
        } // loop: uParFit < nParFit_

          if ( plot_ ) {
            for ( unsigned uParFit = 0; uParFit < nParFit_; ++uParFit ) {
              if ( fitNonRestr_ ) {
                histVecFracL5EtaPtFitMap.at( uParFit )->Draw();
                c1.Print( std::string( pathPlots_ + histVecFracL5EtaPtFitMap.at( uParFit )->GetName() + ".png" ).c_str() );
              }
              histVecFracL5RestrEtaPtFitMap.at( uParFit )->Draw();
              c1.Print( std::string( pathPlots_ + histVecFracL5RestrEtaPtFitMap.at( uParFit )->GetName() + ".png" ).c_str() );
            }
            if ( fitNonRestr_ ) {
              histFracL5EtaPtFitMapProb->Draw();
              c1.Print( std::string( pathPlots_ + histFracL5EtaPtFitMapProb->GetName() + ".png" ).c_str() );
            }
            histFracL5RestrEtaPtFitMapProb->Draw();
            c1.Print( std::string( pathPlots_ + histFracL5RestrEtaPtFitMapProb->GetName() + ".png" ).c_str() );
          }

          if ( useSymm_ ) {
            JetCorrectorParameters::Record jecRecordEtaMinus( nVar_, jecVarsMinsEtaMinus, jecVarsMaxsEtaMinus, jecParsEtaPt );
            jecRecordsEtaPt.at( nEtaBins_ - uEta - 1 ) = jecRecordEtaMinus;
            JetCorrectorParameters::Record jecRecordEtaPtRestrMinus( nVar_, jecVarsMinsEtaMinus, jecVarsMaxsEtaMinus, jecParsEtaPtRestr );
            jecRecordsEtaPtRestr.at( nEtaBins_ - uEta - 1 ) = jecRecordEtaPtRestrMinus;
            JetCorrectorParameters::Record jecRecordEtaPlus( nVar_, jecVarsMinsEtaPlus, jecVarsMaxsEtaPlus, jecParsEtaPt );
            jecRecordsEtaPt.at( nEtaBins_ + uEta ) = jecRecordEtaPlus;
            JetCorrectorParameters::Record jecRecordEtaPtRestrPlus( nVar_, jecVarsMinsEtaPlus, jecVarsMaxsEtaPlus, jecParsEtaPtRestr );
            jecRecordsEtaPtRestr.at( nEtaBins_ + uEta ) = jecRecordEtaPtRestrPlus;
          }
          else {
            JetCorrectorParameters::Record jecRecordEtaPt( nVar_, jecVarsMinsEtaPlus, jecVarsMaxsEtaPlus, jecParsEtaPt );
            jecRecordsEtaPt.at( uEta ) = jecRecordEtaPt;
            JetCorrectorParameters::Record jecRecordEtaPtRestr( nVar_, jecVarsMinsEtaPlus, jecVarsMaxsEtaPlus, jecParsEtaPtRestr );
            jecRecordsEtaPtRestr.at( uEta ) = jecRecordEtaPtRestr;
          }

        } // loop: keyEta

        JetCorrectorParameters jecEta( jecDefinitions, jecRecordsEta );
        if ( writeFiles_ && fitNonRestr_ ) {
          std::string nameOutEta( pathOut_ + "/gentJecL5_" + sample_ );
          if ( usePileUp_ ) nameOutEta.append( "_PileUp" );
          if ( refSel_)     nameOutEta.append( "_Ref" );
          nameOutEta.append( "_" + name + "_Eta.txt" );
          jecEta.printFile( nameOutEta );
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    written L5 JEC file:" << std::endl
                    << "        " << nameOutEta << std::endl;
        }
        JetCorrectorParameters jecEtaRestr( jecDefinitions, jecRecordsEtaRestr );
        if ( writeFiles_ ) {
          std::string nameOutEtaRestr( pathOut_ + "/gentJecL5_" + sample_ );
          if ( usePileUp_ ) nameOutEtaRestr.append( "_PileUp" );
          if ( refSel_)     nameOutEtaRestr.append( "_Ref" );
          nameOutEtaRestr.append( "_" + name + "_EtaRestr.txt" );
          jecEtaRestr.printFile( nameOutEtaRestr );
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    written L5 JEC file:" << std::endl
                    << "        " << nameOutEtaRestr << std::endl;
        }

        JetCorrectorParameters jecEtaPt( jecDefinitions, jecRecordsEtaPt );
        if ( writeFiles_ && fitNonRestr_ ) {
          std::string nameOutEtaPt( pathOut_ + "/gentJecL5_" + sample_ );
          if ( usePileUp_ ) nameOutEtaPt.append( "_PileUp" );
          if ( refSel_)     nameOutEtaPt.append( "_Ref" );
          nameOutEtaPt.append( "_" + name + "_EtaPt.txt" );
          jecEtaPt.printFile( nameOutEtaPt );
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    written L5 JEC file:" << std::endl
                    << "        " << nameOutEtaPt << std::endl;
        }
        JetCorrectorParameters jecEtaPtRestr( jecDefinitions, jecRecordsEtaPtRestr );
        if ( writeFiles_ ) {
          std::string nameOutEtaPtRestr( pathOut_ + "/gentJecL5_" + sample_ );
          if ( usePileUp_ ) nameOutEtaPtRestr.append( "_PileUp" );
          if ( refSel_)     nameOutEtaPtRestr.append( "_Ref" );
          nameOutEtaPtRestr.append( "_" + name + "_EtaPtRestr.txt" );
          jecEtaPtRestr.printFile( nameOutEtaPtRestr );
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    written L5 JEC file:" << std::endl
                    << "        " << nameOutEtaPtRestr << std::endl;
        }

        if ( plot_ ) {
          for ( unsigned uParFit = 0; uParFit < nParFit_; ++uParFit ) {
            if ( fitNonRestr_ ) {
              histVecFracL5EtaParAMap.at( uParFit )->Draw();
              c1.Print( std::string( pathPlots_ + histVecFracL5EtaParAMap.at( uParFit )->GetName() + ".png" ).c_str() );
              histVecFracL5EtaParBMap.at( uParFit )->Draw();
              c1.Print( std::string( pathPlots_ + histVecFracL5EtaParBMap.at( uParFit )->GetName() + ".png" ).c_str() );
            }
            histVecFracL5RestrEtaParAMap.at( uParFit )->Draw();
            c1.Print( std::string( pathPlots_ + histVecFracL5RestrEtaParAMap.at( uParFit )->GetName() + ".png" ).c_str() );
            histVecFracL5RestrEtaParBMap.at( uParFit )->Draw();
            c1.Print( std::string( pathPlots_ + histVecFracL5RestrEtaParBMap.at( uParFit )->GetName() + ".png" ).c_str() );
          }
          for ( unsigned uParFit = 0; uParFit < nParFit_; ++uParFit ) {
            if ( fitNonRestr_ ) {
              histVecFracL5EtaFitMap.at( uParFit )->Draw();
              c1.Print( std::string( pathPlots_ + histVecFracL5EtaFitMap.at( uParFit )->GetName() + ".png" ).c_str() );
            }
            histVecFracL5RestrEtaFitMap.at( uParFit )->Draw();
            c1.Print( std::string( pathPlots_ + histVecFracL5RestrEtaFitMap.at( uParFit )->GetName() + ".png" ).c_str() );
          }
          if ( fitNonRestr_ ) {
            histFracL5EtaFitMapProb->Draw();
            c1.Print( std::string( pathPlots_ + histFracL5EtaFitMapProb->GetName() + ".png" ).c_str() );
          }
          histFracL5RestrEtaFitMapProb->Draw();
          c1.Print( std::string( pathPlots_ + histFracL5RestrEtaFitMapProb->GetName() + ".png" ).c_str() );
        }

      } // loop: keyFit

    }

  } // loop: uCat < objCats_.size()


  // Write and close ROOT files
  if ( overwrite_ ) {
    fileIn_->Write( 0, TObject::kOverwrite );
    fileOut_->Write( 0, TObject::kOverwrite );
  }
  else {
    fileIn_->Write();
    fileOut_->Write();
  }
  fileIn_->Close();
  fileOut_->Close();

  if ( verbose_ > 0 )
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "    return status " << returnStatus_ << std::endl;
  return returnStatus_;

}


// Initialise parameters for fit function
void setParametersFit( TF1 * fit, TH1D * histo, bool useBkgFunction )
{
  //. This function assumes fit functions of the forms
  // - [0]*exp(-0.5*((x-[1])/[2])**2)/([2]*sqrt(2*pi)) (single Gaissian) or
  // - [0]*exp(-0.5*((x-[1])/[2])**2)/([2]*sqrt(2*pi)) + [3]*exp(-0.5*((x-[4])/[5])**2)/([5]*sqrt(2*pi)) (double Gaussian)

  // Starting points
  Double_t c( histo->Integral() );                             // Constant
  Double_t m( histo->GetMean() );                              // Mean
  Double_t p( histo->GetBinCenter( histo->GetMaximumBin() ) ); // Peak
  Double_t s( histo->GetRMS() );                               // RMS
  // Gaussian part
  // Constant
  fit->SetParameter( 0, c );
  fit->SetParLimits( 0, 0., 2. * c );
  fit->SetParName( 0, "Constant c" );
  // Mean
  fit->SetParameter( 1, p ); // No double peak structure in this case
  fit->SetParLimits( 1, 1. - 2. * s, 1. + 2. * s );
  fit->SetParName( 1, "Gaussian #mu" );
  // Sigma
  fit->SetParameter( 2, s );
  fit->SetParLimits( 2, 0., 2. * s );
  fit->SetParName( 2, "Gaussian #sigma" );
  // Additional part
  if ( useBkgFunction ) {
    Double_t fitMin, fitMax;
    fit->GetRange( fitMin, fitMax );
    fit->SetParameter( 3, 0.25 * c );
    fit->SetParLimits( 3, 0., 0.5 * c );
    fit->SetParName( 3, "bkg c" );
    fit->SetParameter( 4, m + ( m - p ) ); // shift into opposite direction compared to main Gaussian
    fit->SetParLimits( 4, fitMin, fitMax );
    fit->SetParName( 4, "bkg #mu" );
    fit->SetParameter( 5, ( fitMax - fitMin ) / 4. ); // starting from sigma covering half of the fit range
    fit->SetParLimits( 5, 0., fitMax - fitMin );
    fit->SetParName( 5, "bkg #sigma" );
  }
}
