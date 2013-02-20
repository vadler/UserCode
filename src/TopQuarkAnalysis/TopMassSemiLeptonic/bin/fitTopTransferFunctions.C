#include <cstdlib>
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
#include <TFitResult.h>
#include <TMath.h>
#include <TCanvas.h>

#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include "DataFormats/Math/interface/deltaR.h"

#include "CommonTools/MyTools/interface/RootTools.h"
#include "TopQuarkAnalysis/TopMassSemiLeptonic/interface/TransferFunction.h"


// Initialise parameters for fit function
void setParametersFit( std::string objCat, TF1 * fit, TH1D * histo, std::string fitFuncId, bool scale );
// Check correct assignment of signal and background to the fit function parameters
bool checkParametersFit( std::string objCat, TF1 * fit, std::string fitFuncId );
// Interchange signal and background parameters of fit function
void mixParametersFit( my::TransferFunction & transfer, TF1 * fit );
// Interchange signal and background parameters of fit function in histogram filling
void fillMixedParametersFit( int bin, std::vector< TH1D * > & histoVec, TFitResultPtr & fitResultPtr );
// Initialise parametrs for dependency function
void setParametersDependency( std::string objCat, TF1 * dep, TH1D * histo, std::string fitFuncId, std::string depFuncId, unsigned par );


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
  const bool refGen_( process_.getParameter< bool >( "refGen" ) );
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
  const edm::ParameterSet & histos_( process_.getParameter< edm::ParameterSet >( "histos" ) );
  const unsigned histBins_( histos_.getParameter< unsigned >( "DeltaPtBins" ) );
  const double   histMax_( histos_.getParameter< double >( "DeltaPtMax" ) );
  const double widthFactor_( histos_.getParameter< double >( "widthFactor" ) );
  // Configuration for fitting transfer functions
  const edm::ParameterSet & fit_( process_.getParameter< edm::ParameterSet >( "fit" ) );
  const bool fitNonRestr_( fit_.getParameter< bool >( "fitNonRestr" ) );
  const bool fitEtaBins_( fit_.getParameter< bool >( "fitEtaBins" ) );
  const double minPt_( fit_.getParameter< double >( "minPt" ) );
  const double maxDR_( fit_.getParameter< double >( "maxDR" ) );
  // Configuration for fitting transfer functions
  const edm::ParameterSet & transfer_( process_.getParameter< edm::ParameterSet >( "transfer" ) );
  const bool doFit_( transfer_.getParameter< bool >( "doFit" ) );
  const bool scale_( transfer_.getParameter< bool >( "scale" ) );
  double fitMaxPt_( transfer_.getParameter< double >( "fitMaxPt" ) );
  const std::string fitFuncId_( transfer_.getParameter< std::string >( "fitFunction" ) );
  const int norm_( transfer_.getParameter< int >( "norm" ) );
  std::string fitOptions_( transfer_.getParameter< std::string >( "fitOptions" ) );
  const double fitRange_( std::min( transfer_.getParameter< double >( "fitRange" ), widthFactor_ ) );
  const std::string depFuncId_( transfer_.getParameter< std::string >( "dependencyFunction" ) );
  const std::string pathOut_( transfer_.getParameter< std::string >( "pathOut" ) );
  const bool writeFiles_( ! pathOut_.empty() );

  if ( verbose_ > 0 ) {
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "    called for object categories ";
    for ( unsigned uCat = 0; uCat < objCats_.size() - 1; ++uCat ) std::cout << "'" << objCats_.at( uCat ) << "', ";
    std::cout << "'" << objCats_.back() << "'" << std::endl;
  }

  // Set fit functions
  std::map< std::string, std::string > fitFunctions_;
  fitFunctions_[ "sGauss" ] = "[0]*exp(-0.5*((x-[1])/[2])**2)/([2]*sqrt(2*pi))";                                            // single Gaussian
  fitFunctions_[ "dGauss" ] = "[0]*(exp(-0.5*((x-[1])/[2])**2)+[3]*exp(-0.5*((x-[4])/[5])**2))/(([2]+[3]*[5])*sqrt(2*pi))"; // double Gaussian
  if ( fitFunctions_.find( fitFuncId_ ) == fitFunctions_.end() ) {
    std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
              << "    fit function identifier '" << fitFuncId_ << "' unknown" << std::endl;
    returnStatus_ += 0x3;
    return returnStatus_;
  }
  const std::string fitFunction_( fitFunctions_[ fitFuncId_ ] );
  // Set dependency functions
  std::map< std::string, std::string > dependencyFunctions_;
  dependencyFunctions_[ "linear" ]     = "[0]+[1]*x";
  dependencyFunctions_[ "squared" ]    = "[0]+[1]*x+[2]*x**2";
  dependencyFunctions_[ "resolution" ] = "sqrt([0]**2+[1]**2*x+[2]**2*x**2)";
  if ( dependencyFunctions_.find( depFuncId_ ) == dependencyFunctions_.end() ) {
    std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
              << "    dependency function identifier '" << depFuncId_ << "' unknown" << std::endl;
    returnStatus_ += 0x4;
    return returnStatus_;
  }
  const std::string dependencyFunction_( dependencyFunctions_[ depFuncId_ ] );

  // Set constants
  std::string evtSel_( "analyzeHitFit" );
  if ( refSel_ ) evtSel_.append( "Reference" );
  const std::string nameDirClass( "TDirectoryFile" );
  const std::string nameFuncClass( "TF1" );
  if      ( verbose_ < 3 ) fitOptions_.append( "Q" );
  else if ( verbose_ > 4 ) fitOptions_.append( "V" );
  const std::string baseTitlePtL( useAlt_ ? "E" : "p" );
  const std::string baseTitlePtU( useAlt_ ? "E" : "P" );
  const std::string baseTitlePt( useNonT_ ? baseTitlePtU : baseTitlePtU + "t" );
  const std::string titlePtT( useNonT_ ? baseTitlePtL : baseTitlePtL + "_{t}" );
  const std::string titlePt( refGen_ ? titlePtT + "^{GEN} (GeV)" : titlePtT + " (GeV)" );
  const std::string baseTitleEta( useSymm_ ? "|#eta|" : "#eta" );
  const std::string titleEta( refGen_ ? baseTitleEta + "^{GEN}" : baseTitleEta + "^{RECO}" );
  const std::string titleTrans( "#Delta" + titlePtT + " (GeV)" );
  const std::string titleTransMean( "#mu of " + titleTrans );
  const std::string titleTransNorm( "c of " + titleTrans );
  const std::string titleTransSigma( "#sigma of " + titleTrans );
  const std::string titleEvents( "events" );
  const std::string titleChi2( "#chi^{2} / ndf" );
  const std::string titleProb( "log_{10}(prob.)" );
  const std::string titleNdf( "ndf" );
  const std::string titleFits( "fits" );


  // Open input file

  if ( verbose_ > 0 )
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "    using      input  file '" << inFile_  << "'" << std::endl
              << "    writing to output file '" << outFile_ << "'" << std::endl;

  TFile * fileIn_( TFile::Open( inFile_.c_str(), "READ" ) );
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

  TFile * fileOut_( TFile::Open( outFile_.c_str(), "UPDATE" ) );
  if ( ! fileOut_ ) {
    fileOut_ = TFile::Open( outFile_.c_str(), "NEW" );
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
    dirOutSel_ = new TDirectoryFile( evtSel_.c_str(), std::string( refSel_ ? "Reference selection" : "Basic selection" ).c_str() );
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
    TDirectory * dirOutCat_( ( TDirectory* )( dirOutSel_->Get( objCat.c_str() ) ) );
    if ( ! dirOutCat_ ) {
      dirOutSel_->cd();
      dirOutCat_ = new TDirectoryFile( objCat.c_str(), std::string( objCat + " objects" ).c_str() );
    }
    if ( verbose_ > 1 ) gDirectory->pwd();

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
    if ( fitMaxPt_ > ptBins_.back() ) fitMaxPt_ = ptBins_.back();

    // Read kinematic property n-tuple data
    DataCont weightData_( nEtaBins_ );
    DataCont ptData_( nEtaBins_ );
    DataCont ptGenData_( nEtaBins_ );
    DataCont etaData_( nEtaBins_ );
    DataCont etaGenData_( nEtaBins_ );
    DataCont phiData_( nEtaBins_ );
    DataCont phiGenData_( nEtaBins_ );
    Double_t ptData;
    Double_t ptGenData;
    Double_t etaData;
    Double_t etaGenData;
    Double_t phiData;
    Double_t phiGenData;
    Int_t    iEta;
    TTree * data_( ( TTree* )( dirCat_->Get( std::string( objCat + "_data" ).c_str() ) ) );
    if ( useAlt_ ) {
      data_->SetBranchAddress( "PtAlt" , &ptData );
      data_->SetBranchAddress( "EtaAlt", &etaData );
      data_->SetBranchAddress( "PhiAlt", &phiData );
    }
    else {
      data_->SetBranchAddress( "Pt" , &ptData );
      data_->SetBranchAddress( "Eta", &etaData );
      data_->SetBranchAddress( "Phi", &phiData );
    }
    data_->SetBranchAddress( "PtGen" , &ptGenData );
    data_->SetBranchAddress( "EtaGen", &etaGenData );
    data_->SetBranchAddress( "PhiGen", &phiGenData );
    if ( useSymm_ )
      if      ( refGen_ ) data_->SetBranchAddress( "BinEtaSymmGen", &iEta );
      else if ( useAlt_ ) data_->SetBranchAddress( "BinEtaSymmAlt", &iEta );
      else                data_->SetBranchAddress( "BinEtaSymm"   , &iEta );
    else
      if      ( refGen_ ) data_->SetBranchAddress( "BinEtaGen", &iEta );
      else if ( useAlt_ ) data_->SetBranchAddress( "BinEtaAlt", &iEta );
      else                data_->SetBranchAddress( "BinEta"   , &iEta );
    Int_t nEntries( ( Int_t )data_->GetEntries() );
    if ( objCat == "UdscJet" || objCat == "BJet" ) assert( nEntries % 2 == 0 ); // need two jet entries per event
    std::vector< unsigned > sizeEta_( nEtaBins_, 0 );
    for ( int iEntry = 0; iEntry < nEntries; ++iEntry ) {
      data_->GetEntry( iEntry );
      assert( iEta < ( Int_t )( nEtaBins_ ) ); // has to fit (and be consistent)
      if ( iEta == -1 ) continue; // FIXME: eta out of range in analyzer; should be solved more consistently
      sizeEta_.at( iEta ) += 1;
      Int_t pileUpEntry( ( objCat == "UdscJet" || objCat == "BJet" ) ? iEntry / 2 : iEntry );
      weightData_.at( iEta ).push_back( pileUpWeights_.at( pileUpEntry ) );
      ptData_.at( iEta ).push_back( ptData );
      ptGenData_.at( iEta ).push_back( ptGenData );
      etaData_.at( iEta ).push_back( etaData );
      etaGenData_.at( iEta ).push_back( etaGenData );
      phiData_.at( iEta ).push_back( phiData );
      phiGenData_.at( iEta ).push_back( phiGenData );
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
      dirOutPt_ = new TDirectoryFile( "Pt", "Momentum variable" );
    }
    if ( verbose_ > 1 ) gDirectory->pwd();

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
                  << "    fit '" << subFit << "' does not exist in input file" << std::endl;
        returnStatus_ += 0x10000;
        continue;
      }
      TDirectory * dirOutFit_( ( TDirectory* )( dirOutPt_->Get( subFit.c_str() ) ) );
      if ( ! dirOutFit_ ) {
        dirOutPt_->cd();
        dirOutFit_ = new TDirectoryFile( subFit.c_str(), "Particular fit" );
      }
      dirOutFit_->cd();
      if ( verbose_ > 1 ) gDirectory->pwd();

      std::string name( objCat + "_" + baseTitlePt + "_" + subFit );

      const std::string nameTrans( name + "_Trans" );
      TH1D * histTrans( new TH1D( nameTrans.c_str(), objCat.c_str(), histBins_, -histMax_, histMax_ ) );
      histTrans->SetXTitle( titleTrans.c_str() );
      histTrans->SetYTitle( titleEvents.c_str() );
      const std::string nameTransMapPt( nameTrans + "_Map_Pt" );
      TH2D * histTransMapPt( new TH2D( nameTransMapPt.c_str(), objCat.c_str(), nPtBins_, ptBins_.data(), histBins_, -histMax_, histMax_ ) );
      histTransMapPt->SetXTitle( titlePt.c_str() );
      histTransMapPt->SetYTitle( titleTrans.c_str() );
      histTransMapPt->SetZTitle( titleEvents.c_str() );
      const std::string nameTransMapEta( nameTrans + "_Map_Eta" );
      TH2D * histTransMapEta( new TH2D( nameTransMapEta.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data(), histBins_, -histMax_, histMax_ ) );
      histTransMapEta->SetXTitle( titleEta.c_str() );
      histTransMapEta->SetYTitle( titleTrans.c_str() );
      histTransMapEta->SetZTitle( titleEvents.c_str() );

      const std::string nameTransRestr( nameTrans + "Restr" );
      TH1D * histTransRestr( new TH1D( nameTransRestr.c_str(), objCat.c_str(), histBins_, -histMax_, histMax_ ) );
      histTransRestr->SetXTitle( titleTrans.c_str() );
      histTransRestr->SetYTitle( titleEvents.c_str() );
      const std::string nameTransRestrMapPt( nameTransRestr + "_Map_Pt" );
      TH2D * histTransRestrMapPt( new TH2D( nameTransRestrMapPt.c_str(), objCat.c_str(), nPtBins_, ptBins_.data(), histBins_, -histMax_, histMax_ ) );
      histTransRestrMapPt->SetXTitle( titlePt.c_str() );
      histTransRestrMapPt->SetYTitle( titleTrans.c_str() );
      histTransRestrMapPt->SetZTitle( titleEvents.c_str() );
      const std::string nameTransRestrMapEta( nameTransRestr + "_Map_Eta" );
      TH2D * histTransRestrMapEta( new TH2D( nameTransRestrMapEta.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data(), histBins_, -histMax_, histMax_ ) );
      histTransRestrMapEta->SetXTitle( titleEta.c_str() );
      histTransRestrMapEta->SetYTitle( titleTrans.c_str() );
      histTransRestrMapEta->SetZTitle( titleEvents.c_str() );

      std::vector< TH1D * > histVecPtTrans;
      std::vector< TH2D * > histVecPtTransMapEta;
      std::vector< TH1D * > histVecPtTransRestr;
      std::vector< TH2D * > histVecPtTransRestrMapEta;
      for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
        const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
        const std::string namePt( name + "_" + baseTitlePt + binPt );
        const std::string namePtTrans( namePt + "_Trans" );
        const std::string titlePtTrans( objCat + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq " + titlePtT + " < " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
        TH1D * histPtTrans( new TH1D( namePtTrans.c_str(), titlePtTrans.c_str(), histBins_, -histMax_, histMax_ ) );
        histPtTrans->SetXTitle( titleTrans.c_str() );
        histPtTrans->SetYTitle( titleEvents.c_str() );
        histVecPtTrans.push_back( histPtTrans );
        const std::string namePtTransMapEta( namePtTrans + "_Map_Eta" );
        TH2D * histPtTransMapEta( new TH2D( namePtTransMapEta.c_str(), titlePtTrans.c_str(), nEtaBins_, etaBins_.data(), histBins_, -histMax_, histMax_ ) );
        histPtTransMapEta->SetXTitle( titleEta.c_str() );
        histPtTransMapEta->SetYTitle( titleTrans.c_str() );
        histPtTransMapEta->SetZTitle( titleEvents.c_str() );
        histVecPtTransMapEta.push_back( histPtTransMapEta );
        const std::string namePtTransRestr( namePtTrans + "Restr" );
        TH1D * histPtTransRestr( new TH1D( namePtTransRestr.c_str(), titlePtTrans.c_str(), histBins_, -histMax_, histMax_ ) );
        histPtTransRestr->SetXTitle( titleTrans.c_str() );
        histPtTransRestr->SetYTitle( titleEvents.c_str() );
        histVecPtTransRestr.push_back( histPtTransRestr );
        const std::string namePtTransRestrMapEta( namePtTransRestr + "_Map_Eta" );
        TH2D * histPtTransRestrMapEta( new TH2D( namePtTransRestrMapEta.c_str(), titlePtTrans.c_str(), nEtaBins_, etaBins_.data(), histBins_, -histMax_, histMax_ ) );
        histPtTransRestrMapEta->SetXTitle( titleEta.c_str() );
        histPtTransRestrMapEta->SetYTitle( titleTrans.c_str() );
        histPtTransRestrMapEta->SetZTitle( titleEvents.c_str() );
        histVecPtTransRestrMapEta.push_back( histPtTransRestrMapEta );
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
//         TDirectory * dirEta_( ( TDirectory* )( dirFit_->Get( binEta.c_str() ) ) );
//         dirEta_->cd();
//         if ( verbose_ > 1 ) gDirectory->pwd();
        TDirectory * dirOutEta_( ( TDirectory* )( dirOutFit_->Get( binEta.c_str() ) ) );
        if ( ! dirOutEta_ ) {
          dirOutFit_->cd();
          dirOutEta_ = new TDirectoryFile( binEta.c_str(), "Eta bin" );
        }
        dirOutEta_->cd();
        if ( verbose_ > 1 ) gDirectory->pwd();

        const std::string nameEta( name + "_" + binEta );

        const std::string nameEtaTrans( nameEta + "_Trans" );
        const std::string titleEtaTrans( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta < " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) );
        TH1D * histEtaTrans( new TH1D( nameEtaTrans.c_str(), titleEtaTrans.c_str(), histBins_, -histMax_, histMax_ ) );
        histEtaTrans->SetXTitle( titleTrans.c_str() );
        histEtaTrans->SetYTitle( titleEvents.c_str() );
        const std::string nameEtaTransMapPt( nameEtaTrans + "_Map_Pt" );
        TH2D * histEtaTransMapPt( new TH2D( nameEtaTransMapPt.c_str(), titleEtaTrans.c_str(), nPtBins_, ptBins_.data(), histBins_, -histMax_, histMax_ ) );
        histEtaTransMapPt->SetXTitle( titlePt.c_str() );
        histEtaTransMapPt->SetYTitle( titleTrans.c_str() );
        histEtaTransMapPt->SetZTitle( titleEvents.c_str() );

        const std::string nameEtaTransRestr( nameEtaTrans + "Restr" );
        TH1D * histEtaTransRestr( new TH1D( nameEtaTransRestr.c_str(), titleEtaTrans.c_str(), histBins_, -histMax_, histMax_ ) );
        histEtaTransRestr->SetXTitle( titleTrans.c_str() );
        histEtaTransRestr->SetYTitle( titleEvents.c_str() );
        const std::string nameEtaTransRestrMapPt( nameEtaTransRestr + "_Map_Pt" );
        TH2D * histEtaTransRestrMapPt( new TH2D( nameEtaTransRestrMapPt.c_str(), titleEtaTrans.c_str(), nPtBins_, ptBins_.data(), histBins_, -histMax_, histMax_ ) );
        histEtaTransRestrMapPt->SetXTitle( titlePt.c_str() );
        histEtaTransRestrMapPt->SetYTitle( titleTrans.c_str() );
        histEtaTransRestrMapPt->SetZTitle( titleEvents.c_str() );

        // Split data into p_t bins
        DataCont weightEtaBin( nPtBins_ );
        DataCont ptEtaBin( nPtBins_ );
        DataCont ptGenEtaBin( nPtBins_ );
        DataCont etaEtaBin( nPtBins_ );
        DataCont etaGenEtaBin( nPtBins_ );
        DataCont phiEtaBin( nPtBins_ );
        DataCont phiGenEtaBin( nPtBins_ );
        std::vector< unsigned > sizePt( nPtBins_ );
        for ( unsigned uEntry = 0; uEntry < sizeEta_.at( uEta ); ++uEntry ) {
          if ( ptData_.at( uEta ).at( uEntry ) == -9. || ptGenData_.at( uEta ).at( uEntry ) == -9. ) continue; // no match
          Double_t ptVal( useNonT_ ? ptData_.at( uEta ).at( uEntry ) * std::cosh( etaData_.at( uEta ).at( uEntry ) ) : ptData_.at( uEta ).at( uEntry ) );
          Double_t ptGenVal( useNonT_ ? ptGenData_.at( uEta ).at( uEntry ) * std::cosh( etaGenData_.at( uEta ).at( uEntry ) ) : ptGenData_.at( uEta ).at( uEntry ) );
          Double_t ptRef( refGen_ ? ptGenVal : ptVal );
          for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
            if ( ptBins_.at( uPt ) <= ptRef && ptRef < ptBins_.at( uPt + 1 ) ) {
              sizePt.at( uPt ) += 1;
              weightEtaBin.at( uPt ).push_back( weightData_.at( uEta ).at( uEntry ) );
              ptEtaBin.at( uPt ).push_back( ptVal );
              ptGenEtaBin.at( uPt ).push_back( ptGenVal );
              etaEtaBin.at( uPt ).push_back( etaData_.at( uEta ).at( uEntry ) );
              etaGenEtaBin.at( uPt ).push_back( etaGenData_.at( uEta ).at( uEntry ) );
              phiEtaBin.at( uPt ).push_back( phiData_.at( uEta ).at( uEntry ) );
              phiGenEtaBin.at( uPt ).push_back( phiGenData_.at( uEta ).at( uEntry ) );
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

          const std::string nameEtaPtTrans( nameEtaPt + "_Trans" );
          const std::string titleEtaPtTrans( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta < " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq " + titlePtT + " < " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
          TH1D * histEtaPtTrans( new TH1D( nameEtaPtTrans.c_str(), titleEtaPtTrans.c_str(), histBins_, -histMax_, histMax_ ) );
          histEtaPtTrans->SetXTitle( titleTrans.c_str() );
          histEtaPtTrans->SetYTitle( titleEvents.c_str() );

          const std::string nameEtaPtTransRestr( nameEtaPtTrans + "Restr" );
          TH1D * histEtaPtTransRestr( new TH1D( nameEtaPtTransRestr.c_str(), titleEtaPtTrans.c_str(), histBins_, -histMax_, histMax_ ) );
          histEtaPtTransRestr->SetXTitle( titleTrans.c_str() );
          histEtaPtTransRestr->SetYTitle( titleEvents.c_str() );

          for ( unsigned uEntry = 0; uEntry < sizePt.at( uPt ); ++uEntry ) {
            const Double_t value( refGen_ ? ptGenEtaBin.at( uPt ).at( uEntry ) - ptEtaBin.at( uPt ).at( uEntry ) : ptEtaBin.at( uPt ).at( uEntry ) - ptGenEtaBin.at( uPt ).at( uEntry ) );
            const Double_t ptRef( refGen_ ? ptGenEtaBin.at( uPt ).at( uEntry ) : ptEtaBin.at( uPt ).at( uEntry ) );
            if ( fitNonRestr_ ) {
              histEtaPtTrans->Fill( value, weightEtaBin.at( uPt ).at( uEntry ) );
              histVecPtTrans.at( uPt )->Fill( value, weightEtaBin.at( uPt ).at( uEntry ) );
              histEtaTrans->Fill( value, weightEtaBin.at( uPt ).at( uEntry ) );
              histTrans->Fill( value, weightEtaBin.at( uPt ).at( uEntry ) );
            }
            if ( ptRef >= minPt_ && reco::deltaR( etaGenEtaBin.at( uPt ).at( uEntry ), phiGenEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDR_ ) {
              histEtaPtTransRestr->Fill( value, weightEtaBin.at( uPt ).at( uEntry ) );
              histVecPtTransRestr.at( uPt )->Fill( value, weightEtaBin.at( uPt ).at( uEntry ) );
              histEtaTransRestr->Fill( value, weightEtaBin.at( uPt ).at( uEntry ) );
              histTransRestr->Fill( value, weightEtaBin.at( uPt ).at( uEntry ) );
            }
          } // loop: uEntry < ptEtaBin.at( uPt ).size()
          if ( fitNonRestr_ && ! ( scale_ && histEtaPtTrans->GetSumOfWeights() == 0. ) ) {
            for ( unsigned uEntry = 0; uEntry < sizePt.at( uPt ); ++uEntry ) {
              const Double_t value( refGen_ ? ptGenEtaBin.at( uPt ).at( uEntry ) - ptEtaBin.at( uPt ).at( uEntry ) : ptEtaBin.at( uPt ).at( uEntry ) - ptGenEtaBin.at( uPt ).at( uEntry ) );
              const Double_t ptRef( refGen_ ? ptGenEtaBin.at( uPt ).at( uEntry ) : ptEtaBin.at( uPt ).at( uEntry ) );
              const Double_t etaGenSymm( useSymm_ ? std::fabs( etaGenEtaBin.at( uPt ).at( uEntry ) ) : etaGenEtaBin.at( uPt ).at( uEntry ) );
              const Double_t etaSymm( useSymm_ ? std::fabs( etaEtaBin.at( uPt ).at( uEntry ) ) : etaEtaBin.at( uPt ).at( uEntry ) );
              const Double_t etaRef( refGen_ ? etaGenSymm : etaSymm );
              const Double_t weightPt( scale_ ? weightEtaBin.at( uPt ).at( uEntry ) / histVecPtTrans.at( uPt )->GetSumOfWeights() : weightEtaBin.at( uPt ).at( uEntry ) );
              const Double_t weightEta( scale_ ? weightEtaBin.at( uPt ).at( uEntry ) / histEtaTrans->GetSumOfWeights() : weightEtaBin.at( uPt ).at( uEntry ) );
              const Double_t weightEtaPt( scale_ ? weightEtaBin.at( uPt ).at( uEntry ) / histEtaPtTrans->GetSumOfWeights() : weightEtaBin.at( uPt ).at( uEntry ) );
              histEtaTransMapPt->Fill( ptRef, value, weightEtaPt );
              histVecPtTransMapEta.at( uPt )->Fill( etaRef, value, weightEtaPt );
              histTransMapPt->Fill( ptRef, value, weightPt );
              histTransMapEta->Fill( etaRef, value, weightEta );
            } // loop: uEntry < ptEtaBin.at( uPt ).size()
          }
          if ( ! ( scale_ && histEtaPtTransRestr->GetSumOfWeights() == 0. ) ) {
            for ( unsigned uEntry = 0; uEntry < sizePt.at( uPt ); ++uEntry ) {
              const Double_t ptRef( refGen_ ? ptGenEtaBin.at( uPt ).at( uEntry ) : ptEtaBin.at( uPt ).at( uEntry ) );
              if ( ptRef >= minPt_ && reco::deltaR( etaGenEtaBin.at( uPt ).at( uEntry ), phiGenEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDR_ ) {
                const Double_t value( refGen_ ? ptGenEtaBin.at( uPt ).at( uEntry ) - ptEtaBin.at( uPt ).at( uEntry ) : ptEtaBin.at( uPt ).at( uEntry ) - ptGenEtaBin.at( uPt ).at( uEntry ) );
                const Double_t etaGenSymm( useSymm_ ? std::fabs( etaGenEtaBin.at( uPt ).at( uEntry ) ) : etaGenEtaBin.at( uPt ).at( uEntry ) );
                const Double_t etaSymm( useSymm_ ? std::fabs( etaEtaBin.at( uPt ).at( uEntry ) ) : etaEtaBin.at( uPt ).at( uEntry ) );
                const Double_t etaRef( refGen_ ? etaGenSymm : etaSymm );
                const Double_t weightPt( scale_ ? weightEtaBin.at( uPt ).at( uEntry ) / histVecPtTransRestr.at( uPt )->GetSumOfWeights() : weightEtaBin.at( uPt ).at( uEntry ) );
                const Double_t weightEta( scale_ ? weightEtaBin.at( uPt ).at( uEntry ) / histEtaTransRestr->GetSumOfWeights() : weightEtaBin.at( uPt ).at( uEntry ) );
                const Double_t weightEtaPt( scale_ ? weightEtaBin.at( uPt ).at( uEntry ) / histEtaPtTransRestr->GetSumOfWeights() : weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaTransRestrMapPt->Fill( ptRef, value, weightEtaPt );
                histVecPtTransRestrMapEta.at( uPt )->Fill( etaRef, value, weightEtaPt);
                histTransRestrMapPt->Fill( ptRef, value, weightPt );
                histTransRestrMapEta->Fill( etaRef, value, weightEta );
              }
            } // loop: uEntry < ptEtaBin.at( uPt ).size()
          }

          if ( scale_ ) {
            if ( histEtaPtTrans->GetSumOfWeights() != 0. ) histEtaPtTrans->Scale( 1. / histEtaPtTrans->GetSumOfWeights() );
            if ( histEtaPtTransRestr->GetSumOfWeights() != 0. ) histEtaPtTransRestr->Scale( 1. / histEtaPtTransRestr->GetSumOfWeights() );
          }

          const std::string nameEtaPtTransRebin( nameEtaPtTrans + "Rebin" );
          const Double_t meanEtaPtTrans( histEtaPtTrans->GetMean() );
          const Double_t widthEtaPtTrans( std::fabs( histEtaPtTrans->GetRMS() ) );
          if ( fitNonRestr_ && widthEtaPtTrans == 0. && verbose_ > 2 ) {
            std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                      << "    no histogram \"width\" in '" << nameEtaPtTrans << "'" << std::endl;
          }
          const Double_t rangeEtaPtTransRebin( widthEtaPtTrans == 0. ? widthFactor_ * std::fabs( histEtaPtTrans->GetXaxis()->GetXmax() ) : widthFactor_ * widthEtaPtTrans ); // FIXME: tune, incl. under- and overflow, remove hard-coding
          TH1D * histEtaPtTransRebin( new TH1D( nameEtaPtTransRebin.c_str(), titleEtaPtTrans.c_str(), histBins_, -rangeEtaPtTransRebin + meanEtaPtTrans, rangeEtaPtTransRebin + meanEtaPtTrans ) );
          histEtaPtTransRebin->SetXTitle( titleTrans.c_str() );
          histEtaPtTransRebin->SetYTitle( titleEvents.c_str() );

          const std::string nameEtaPtTransRestrRebin( nameEtaPtTransRestr + "Rebin" );
          const Double_t meanEtaPtTransRestr( histEtaPtTransRestr->GetMean() );
          const Double_t widthEtaPtTransRestr( std::fabs( histEtaPtTransRestr->GetRMS() ) );
          if ( widthEtaPtTransRestr == 0. && verbose_ > 2 ) {
            std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                      << "    no histogram \"width\" in '" << nameEtaPtTransRestr << "'" << std::endl;
          }
          const Double_t rangeEtaPtTransRestrRebin( widthEtaPtTransRestr == 0. ? widthFactor_ * std::fabs( histEtaPtTransRestr->GetXaxis()->GetXmax() ) : widthFactor_ * widthEtaPtTransRestr ); // FIXME: tune, incl. under- and overflow, remove hard-coding
          TH1D * histEtaPtTransRestrRebin( new TH1D( nameEtaPtTransRestrRebin.c_str(), titleEtaPtTrans.c_str(), histBins_, -rangeEtaPtTransRestrRebin + meanEtaPtTransRestr, rangeEtaPtTransRestrRebin + meanEtaPtTransRestr ) );
          histEtaPtTransRestrRebin->SetXTitle( titleTrans.c_str() );
          histEtaPtTransRestrRebin->SetYTitle( titleEvents.c_str() );

          for ( unsigned uEntry = 0; uEntry < sizePt.at( uPt ); ++uEntry ) {
            const Double_t value( refGen_ ? ptGenEtaBin.at( uPt ).at( uEntry ) - ptEtaBin.at( uPt ).at( uEntry ) : ptEtaBin.at( uPt ).at( uEntry ) - ptGenEtaBin.at( uPt ).at( uEntry ) );
            const Double_t ptRef( refGen_ ? ptGenEtaBin.at( uPt ).at( uEntry ) : ptEtaBin.at( uPt ).at( uEntry ) );
            if ( fitNonRestr_ ) {
              histEtaPtTransRebin->Fill( value, weightEtaBin.at( uPt ).at( uEntry ) );
            }
            if ( ptRef >= minPt_ && reco::deltaR( etaGenEtaBin.at( uPt ).at( uEntry ), phiGenEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDR_ ) {
              histEtaPtTransRestrRebin->Fill( value, weightEtaBin.at( uPt ).at( uEntry ) );
            }
          } // loop: uEntry < ptEtaBin.at( uPt ).size()

          if ( scale_ ) {
            if ( histEtaPtTransRebin->GetSumOfWeights() != 0. ) histEtaPtTransRebin->Scale( 1. / histEtaPtTransRebin->GetSumOfWeights() );
            if ( histEtaPtTransRestrRebin->GetSumOfWeights() != 0. ) histEtaPtTransRestrRebin->Scale( 1. / histEtaPtTransRestrRebin->GetSumOfWeights() );
          }

        } // loop: uPt < nPtBins_

        if ( scale_ ) {
          if ( histEtaTrans->GetSumOfWeights() != 0. ) histEtaTrans->Scale( 1. / histEtaTrans->GetSumOfWeights() );
          if ( histEtaTransMapPt->GetSumOfWeights() != 0. ) histEtaTransMapPt->Scale( 1. / histEtaTransMapPt->GetSumOfWeights() );
          if ( histEtaTransRestr->GetSumOfWeights() != 0. ) histEtaTransRestr->Scale( 1. / histEtaTransRestr->GetSumOfWeights() );
          if ( histEtaTransRestrMapPt->GetSumOfWeights() != 0. ) histEtaTransRestrMapPt->Scale( 1. / histEtaTransRestrMapPt->GetSumOfWeights() );
        }

        const std::string nameEtaTransRebin( nameEtaTrans + "Rebin" );
        const Double_t meanEtaTrans( histEtaTrans->GetMean() );
        const Double_t widthEtaTrans( std::fabs( histEtaTrans->GetRMS() ) );
        if ( fitNonRestr_ && widthEtaTrans == 0. && verbose_ > 2 ) {
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    no histogram \"width\" in '" << nameEtaTrans << "'" << std::endl;
        }
        const Double_t rangeEtaTransRebin( widthEtaTrans == 0. ? widthFactor_ * std::fabs( histEtaTrans->GetXaxis()->GetXmax() ) : widthFactor_ * widthEtaTrans ); // FIXME: tune, incl. under- and overflow, remove hard-coding
        TH1D * histEtaTransRebin( new TH1D( nameEtaTransRebin.c_str(), titleEtaTrans.c_str(), histBins_, -rangeEtaTransRebin + meanEtaTrans, rangeEtaTransRebin + meanEtaTrans ) );
        histEtaTransRebin->SetXTitle( titleTrans.c_str() );
        histEtaTransRebin->SetYTitle( titleEvents.c_str() );
        const std::string nameEtaTransRebinMapPt( nameEtaTransRebin + "_Map_Pt" );
        TH2D * histEtaTransRebinMapPt( new TH2D( nameEtaTransRebinMapPt.c_str(), titleEtaTrans.c_str(), nPtBins_, ptBins_.data(), histBins_, -rangeEtaTransRebin + meanEtaTrans, rangeEtaTransRebin + meanEtaTrans ) );
        histEtaTransRebinMapPt->SetXTitle( titlePt.c_str() );
        histEtaTransRebinMapPt->SetYTitle( titleTrans.c_str() );
        histEtaTransRebinMapPt->SetZTitle( titleEvents.c_str() );

        const std::string nameEtaTransRestrRebin( nameEtaTransRestr + "Rebin" );
        const Double_t meanEtaTransRestr( histEtaTransRestr->GetMean() );
        const Double_t widthEtaTransRestr( std::fabs( histEtaTransRestr->GetRMS() ) );
        if ( widthEtaTransRestr == 0. && verbose_ > 2 ) {
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    no histogram \"width\" in '" << nameEtaTransRestr << "'" << std::endl;
        }
        const Double_t rangeEtaTransRestrRebin( widthEtaTransRestr == 0. ? widthFactor_ * std::fabs( histEtaTransRestr->GetXaxis()->GetXmax() ) : widthFactor_ * widthEtaTransRestr ); // FIXME: tune, incl. under- and overflow, remove hard-coding
        TH1D * histEtaTransRestrRebin( new TH1D( nameEtaTransRestrRebin.c_str(), titleEtaTrans.c_str(), histBins_, -rangeEtaTransRestrRebin + meanEtaTransRestr, rangeEtaTransRestrRebin + meanEtaTransRestr ) );
        histEtaTransRestrRebin->SetXTitle( titleTrans.c_str() );
        histEtaTransRestrRebin->SetYTitle( titleEvents.c_str() );
        const std::string nameEtaTransRestrRebinMapPt( nameEtaTransRestrRebin + "_Map_Pt" );
        TH2D * histEtaTransRestrRebinMapPt( new TH2D( nameEtaTransRestrRebinMapPt.c_str(), titleEtaTrans.c_str(), nPtBins_, ptBins_.data(), histBins_, -rangeEtaTransRestrRebin + meanEtaTrans, rangeEtaTransRestrRebin + meanEtaTrans ) );
        histEtaTransRestrRebinMapPt->SetXTitle( titlePt.c_str() );
        histEtaTransRestrRebinMapPt->SetYTitle( titleTrans.c_str() );
        histEtaTransRestrRebinMapPt->SetZTitle( titleEvents.c_str() );

        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
          const std::string nameEtaPt( nameEta + "_" + baseTitlePt + binPt );
          const std::string nameEtaPtTrans( nameEtaPt + "_Trans" );
          for ( unsigned uEntry = 0; uEntry < sizePt.at( uPt ); ++uEntry ) {
            const Double_t value( refGen_ ? ptGenEtaBin.at( uPt ).at( uEntry ) - ptEtaBin.at( uPt ).at( uEntry ) : ptEtaBin.at( uPt ).at( uEntry ) - ptGenEtaBin.at( uPt ).at( uEntry ) );
            const Double_t ptRef( refGen_ ? ptGenEtaBin.at( uPt ).at( uEntry ) : ptEtaBin.at( uPt ).at( uEntry ) );
            if ( fitNonRestr_ ) {
              histEtaTransRebin->Fill( value, weightEtaBin.at( uPt ).at( uEntry ) );
              const std::string nameEtaPtTransRebin( nameEtaPtTrans + "Rebin" );
              TH1D * histEtaPtTransRebin( ( TH1D* )( dirOutEta_->Get( nameEtaPtTransRebin.c_str() ) ) );
              if ( histEtaPtTransRebin != 0 && ! ( scale_ && histEtaPtTransRebin->GetSumOfWeights() == 0. ) ) {
                const Double_t weight( scale_ ? weightEtaBin.at( uPt ).at( uEntry ) / histEtaPtTransRebin->GetSumOfWeights() : weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaTransRebinMapPt->Fill( ptRef, value, weight );
              }
            }
            if ( ptRef >= minPt_ && reco::deltaR( etaGenEtaBin.at( uPt ).at( uEntry ), phiGenEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDR_ ) {
              histEtaTransRestrRebin->Fill( value, weightEtaBin.at( uPt ).at( uEntry ) );
              const std::string nameEtaPtTransRestrRebin( nameEtaPtTrans + "RestrRebin" );
              TH1D * histEtaPtTransRestrRebin( ( TH1D* )( dirOutEta_->Get( nameEtaPtTransRestrRebin.c_str() ) ) );
              if ( histEtaPtTransRestrRebin != 0 && ! ( scale_ && histEtaPtTransRestrRebin->GetSumOfWeights() == 0. ) ) {
                const Double_t weight( scale_ ? weightEtaBin.at( uPt ).at( uEntry ) / histEtaPtTransRestrRebin->GetSumOfWeights() : weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaTransRestrRebinMapPt->Fill( ptRef, value, weight );
              }
            }
          } // loop: uEntry < ptEtaBin.at( uPt ).size()

        } // loop: uPt < nPtBins_

        if ( scale_ ) {
          if ( histEtaTransRebin->GetSumOfWeights() != 0. ) histEtaTransRebin->Scale( 1. / histEtaTransRebin->GetSumOfWeights() );
          if ( histEtaTransRebinMapPt->GetSumOfWeights() != 0. ) histEtaTransRebinMapPt->Scale( 1. / histEtaTransRebinMapPt->GetSumOfWeights() );
          if ( histEtaTransRestrRebin->GetSumOfWeights() != 0. ) histEtaTransRestrRebin->Scale( 1. / histEtaTransRestrRebin->GetSumOfWeights() );
          if ( histEtaTransRestrRebinMapPt->GetSumOfWeights() != 0. ) histEtaTransRestrRebinMapPt->Scale( 1. / histEtaTransRestrRebinMapPt->GetSumOfWeights() );
        }

        if ( plot_ ) {
          if ( fitNonRestr_ ) {
            histEtaTransRebinMapPt->Draw();
            c1.Print( std::string( pathPlots_ + histEtaTransRebinMapPt->GetName() + ".png" ).c_str() );
          }
          histEtaTransRestrRebinMapPt->Draw();
          c1.Print( std::string( pathPlots_ + histEtaTransRestrRebinMapPt->GetName() + ".png" ).c_str() );
        }

      } // loop: keyEta

      if ( scale_ ) {
        if ( histTrans->GetSumOfWeights() != 0. ) histTrans->Scale( 1. / histTrans->GetSumOfWeights() );
        if ( histTransMapPt->GetSumOfWeights() != 0. ) histTransMapPt->Scale( 1. / histTransMapPt->GetSumOfWeights() );
        if ( histTransMapEta->GetSumOfWeights() != 0. ) histTransMapEta->Scale( 1. / histTransMapEta->GetSumOfWeights() );
        if ( histTransRestr->GetSumOfWeights() != 0. ) histTransRestr->Scale( 1. / histTransRestr->GetSumOfWeights() );
        if ( histTransRestrMapPt->GetSumOfWeights() != 0. ) histTransRestrMapPt->Scale( 1. / histTransRestrMapPt->GetSumOfWeights() );
        if ( histTransRestrMapEta->GetSumOfWeights() != 0. ) histTransRestrMapEta->Scale( 1. / histTransRestrMapEta->GetSumOfWeights() );
        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          if ( histVecPtTrans.at( uPt )->GetSumOfWeights() != 0. ) histVecPtTrans.at( uPt )->Scale( 1. / histVecPtTrans.at( uPt )->GetSumOfWeights() );
          if ( histVecPtTransMapEta.at( uPt )->GetSumOfWeights() != 0. ) histVecPtTransMapEta.at( uPt )->Scale( 1. / histVecPtTransMapEta.at( uPt )->GetSumOfWeights() );
          if ( histVecPtTransRestr.at( uPt )->GetSumOfWeights() != 0. ) histVecPtTransRestr.at( uPt )->Scale( 1. / histVecPtTransRestr.at( uPt )->GetSumOfWeights() );
          if ( histVecPtTransRestrMapEta.at( uPt )->GetSumOfWeights() != 0. ) histVecPtTransRestrMapEta.at( uPt )->Scale( 1. / histVecPtTransRestrMapEta.at( uPt )->GetSumOfWeights() );
        }
      }

      dirOutFit_->cd();

      if ( sizeEtaBins != nEtaBins_ ) {
        std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                  << "    mismatch of eta binning for object category '" << objCat << "':" << std::endl
                  << "        bins in directory structure: " << sizeEtaBins                << std::endl
                  << "        bins in binning histogram  : " << nEtaBins_                  << std::endl;
        returnStatus_ += 0x100000;
      }

      const std::string nameTransRebin( nameTrans + "Rebin" );
      const Double_t meanTrans( histTrans->GetMean() );
      const Double_t widthTrans( std::fabs( histTrans->GetRMS() ) );
      if ( fitNonRestr_ && widthTrans == 0. && verbose_ > 2 ) {
        std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                  << "    no histogram \"width\" in '" << nameTrans << "'" << std::endl;
      }
      const Double_t rangeTransRebin( widthTrans == 0. ? widthFactor_ * std::fabs( histTrans->GetXaxis()->GetXmax() ) : widthFactor_ * widthTrans ); // FIXME: tune, incl. under- and overflow, remove hard-coding
      TH1D * histTransRebin( new TH1D( nameTransRebin.c_str(), objCat.c_str(), histBins_, -rangeTransRebin + meanTrans, rangeTransRebin + meanTrans ) );
      histTransRebin->SetXTitle( titleTrans.c_str() );
      histTransRebin->SetYTitle( titleEvents.c_str() );
      const std::string nameTransRebinMapPt( nameTransRebin + "_Map_Pt" );
      TH2D * histTransRebinMapPt( new TH2D( nameTransRebinMapPt.c_str(), objCat.c_str(), nPtBins_, ptBins_.data(), histBins_, -rangeTransRebin + meanTrans, rangeTransRebin + meanTrans ) );
      histTransRebinMapPt->SetXTitle( titlePt.c_str() );
      histTransRebinMapPt->SetYTitle( titleTrans.c_str() );
      histTransRebinMapPt->SetZTitle( titleEvents.c_str() );
      const std::string nameTransRebinMapEta( nameTransRebin + "_Map_Eta" );
      TH2D * histTransRebinMapEta( new TH2D( nameTransRebinMapEta.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data(), histBins_, -rangeTransRebin + meanTrans, rangeTransRebin + meanTrans ) );
      histTransRebinMapEta->SetXTitle( titleEta.c_str() );
      histTransRebinMapEta->SetYTitle( titleTrans.c_str() );
      histTransRebinMapEta->SetZTitle( titleEvents.c_str() );

      const std::string nameTransRestrRebin( nameTransRestr + "Rebin" );
      const Double_t meanTransRestr( histTransRestr->GetMean() );
      const Double_t widthTransRestr( std::fabs( histTransRestr->GetRMS() ) );
      if ( widthTransRestr == 0. && verbose_ > 2 ) {
        std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                  << "    no histogram \"width\" in '" << nameTransRestr << "'" << std::endl;
      }
      const Double_t rangeTransRestrRebin( widthTransRestr == 0. ? widthFactor_ * std::fabs( histTransRestr->GetXaxis()->GetXmax() ) : widthFactor_ * widthTransRestr ); // FIXME: tune, incl. under- and overflow, remove hard-coding
      TH1D * histTransRestrRebin( new TH1D( nameTransRestrRebin.c_str(), objCat.c_str(), histBins_, -rangeTransRestrRebin + meanTransRestr, rangeTransRestrRebin + meanTransRestr ) );
      histTransRestrRebin->SetXTitle( titleTrans.c_str() );
      histTransRestrRebin->SetYTitle( titleEvents.c_str() );
      const std::string nameTransRestrRebinMapPt( nameTransRestrRebin + "_Map_Pt" );
      TH2D * histTransRestrRebinMapPt( new TH2D( nameTransRestrRebinMapPt.c_str(), objCat.c_str(), nPtBins_, ptBins_.data(), histBins_, -rangeTransRestrRebin + meanTrans, rangeTransRestrRebin + meanTrans ) );
      histTransRestrRebinMapPt->SetXTitle( titlePt.c_str() );
      histTransRestrRebinMapPt->SetYTitle( titleTrans.c_str() );
      histTransRestrRebinMapPt->SetZTitle( titleEvents.c_str() );
      const std::string nameTransRestrRebinMapEta( nameTransRestrRebin + "_Map_Eta" );
      TH2D * histTransRestrRebinMapEta( new TH2D( nameTransRestrRebinMapEta.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data(), histBins_, -rangeTransRestrRebin + meanTrans, rangeTransRestrRebin + meanTrans ) );
      histTransRestrRebinMapEta->SetXTitle( titleEta.c_str() );
      histTransRestrRebinMapEta->SetYTitle( titleTrans.c_str() );
      histTransRestrRebinMapEta->SetZTitle( titleEvents.c_str() );

      std::vector< TH1D * > histVecPtTransRebin;
      std::vector< TH2D * > histVecPtTransRebinMapEta;
      std::vector< TH1D * > histVecPtTransRestrRebin;
      std::vector< TH2D * > histVecPtTransRestrRebinMapEta;
      for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
        const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
        const std::string namePt( name + "_" + baseTitlePt + binPt );
        const std::string namePtTrans( namePt + "_Trans" );
        const std::string namePtTransRebin( namePtTrans + "Rebin" );
        const std::string titlePtTrans( objCat + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq " + titlePtT + " < " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
        const Double_t meanPtTrans( histVecPtTrans.at( uPt )->GetMean() );
        const Double_t widthPtTrans( std::fabs( histVecPtTrans.at( uPt )->GetRMS() ) );
        if ( fitNonRestr_ && widthPtTrans == 0. && verbose_ > 2 ) {
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    no histogram \"width\" in '" << namePtTrans << "'" << std::endl;
        }
        const Double_t rangePtTransRebin( widthPtTrans == 0. ? widthFactor_ * std::fabs( histVecPtTrans.at( uPt )->GetXaxis()->GetXmax() ) : widthFactor_ * widthPtTrans ); // FIXME: tune, incl. under- and overflow, remove hard-coding
        TH1D * histPtTransRebin( new TH1D( namePtTransRebin.c_str(), titlePtTrans.c_str(), histBins_, -rangePtTransRebin + meanPtTrans, rangePtTransRebin + meanPtTrans ) );
        histPtTransRebin->SetXTitle( titleTrans.c_str() );
        histPtTransRebin->SetYTitle( titleEvents.c_str() );
        histVecPtTransRebin.push_back( histPtTransRebin );
        const std::string namePtTransRebinMapEta( namePtTransRebin + "_Map_Eta" );
        TH2D * histPtTransRebinMapEta( new TH2D( namePtTransRebinMapEta.c_str(), titlePtTrans.c_str(), nEtaBins_, etaBins_.data(), histBins_, -rangePtTransRebin + meanPtTrans, rangePtTransRebin + meanPtTrans ) );
        histPtTransRebinMapEta->SetXTitle( titleEta.c_str() );
        histPtTransRebinMapEta->SetYTitle( titleTrans.c_str() );
        histPtTransRebinMapEta->SetZTitle( titleEvents.c_str() );
        histVecPtTransRebinMapEta.push_back( histPtTransRebinMapEta );
        const std::string namePtTransRestr( namePtTrans + "Restr" );
        const std::string namePtTransRestrRebin( namePtTransRestr + "Rebin" );
        const Double_t meanPtTransRestr( histVecPtTransRestr.at( uPt )->GetMean() );
        const Double_t widthPtTransRestr( std::fabs( histVecPtTransRestr.at( uPt )->GetRMS() ) );
        if ( widthPtTransRestr == 0. && verbose_ > 2 ) {
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    no histogram \"width\" in '" << namePtTransRestr << "'" << std::endl;
        }
        const Double_t rangePtTransRestrRebin( widthPtTransRestr == 0. ? widthFactor_ * std::fabs( histVecPtTransRestr.at( uPt )->GetXaxis()->GetXmax() ) : widthFactor_ * widthPtTransRestr ); // FIXME: tune, incl. under- and overflow, remove hard-coding
        TH1D * histPtTransRestrRebin( new TH1D( namePtTransRestrRebin.c_str(), titlePtTrans.c_str(), histBins_, -rangePtTransRestrRebin + meanPtTransRestr, rangePtTransRestrRebin + meanPtTransRestr ) );
        histPtTransRestrRebin->SetXTitle( titleTrans.c_str() );
        histPtTransRestrRebin->SetYTitle( titleEvents.c_str() );
        histVecPtTransRestrRebin.push_back( histPtTransRestrRebin );
        const std::string namePtTransRestrRebinMapEta( namePtTransRestrRebin + "_Map_Eta" );
        TH2D * histPtTransRestrRebinMapEta( new TH2D( namePtTransRestrRebinMapEta.c_str(), titlePtTrans.c_str(), nEtaBins_, etaBins_.data(), histBins_, -rangePtTransRestrRebin + meanPtTrans, rangePtTransRestrRebin + meanPtTrans ) );
        histPtTransRestrRebinMapEta->SetXTitle( titleEta.c_str() );
        histPtTransRestrRebinMapEta->SetYTitle( titleTrans.c_str() );
        histPtTransRestrRebinMapEta->SetZTitle( titleEvents.c_str() );
        histVecPtTransRestrRebinMapEta.push_back( histPtTransRestrRebinMapEta );
      }  // loop: uPt < nPtBins_

      // Loop over eta bins
      nextInListFit.Reset();
      while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
        if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
        const std::string binEta( keyEta->GetName() );
        const unsigned uEta( std::atoi( binEta.substr( 3 ).data() ) );

        for ( unsigned uEntry = 0; uEntry < sizeEta_.at( uEta ); ++uEntry ) {
          if ( ptData_.at( uEta ).at( uEntry ) == -9. || ptGenData_.at( uEta ).at( uEntry ) == -9. ) continue; // no match
          const Double_t ptVal( useNonT_ ? ptData_.at( uEta ).at( uEntry ) * std::cosh( etaData_.at( uEta ).at( uEntry ) ) : ptData_.at( uEta ).at( uEntry ) );
          const Double_t ptGenVal( useNonT_ ? ptGenData_.at( uEta ).at( uEntry ) * std::cosh( etaGenData_.at( uEta ).at( uEntry ) ) : ptGenData_.at( uEta ).at( uEntry ) );
          const Double_t value( refGen_ ? ptGenVal - ptVal : ptVal - ptGenVal );
          const Double_t ptRef( refGen_ ? ptGenVal : ptVal );

          if ( fitNonRestr_ ) {
            histTransRebin->Fill( value, weightData_.at( uEta ).at( uEntry ) );
          }
          if ( ptRef >= minPt_ && reco::deltaR( etaGenData_.at( uEta ).at( uEntry ), phiGenData_.at( uEta ).at( uEntry ), etaData_.at( uEta ).at( uEntry ), phiData_.at( uEta ).at( uEntry ) ) <= maxDR_ ) {
            histTransRestrRebin->Fill( value, weightData_.at( uEta ).at( uEntry ) );
          }
          for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
            if ( ptBins_.at( uPt ) <= ptRef && ptRef < ptBins_.at( uPt + 1 ) ) {
              if ( fitNonRestr_ ) {
                histVecPtTransRebin.at( uPt )->Fill( value, weightData_.at( uEta ).at( uEntry ) );
              }
              if ( ptRef >= minPt_ && reco::deltaR( etaGenData_.at( uEta ).at( uEntry ), phiGenData_.at( uEta ).at( uEntry ), etaData_.at( uEta ).at( uEntry ), phiData_.at( uEta ).at( uEntry ) ) <= maxDR_ ) {
                histVecPtTransRestrRebin.at( uPt )->Fill( value, weightData_.at( uEta ).at( uEntry ) );
              }
              break;
            }
          } // loop: uPt < nPtBins_

        } // loop: uEntry < sizeEta_.at( uEta )

      } // loop: keyEta
      nextInListFit.Reset();
      while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
        if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
        const std::string binEta( keyEta->GetName() );
        const unsigned uEta( std::atoi( binEta.substr( 3 ).data() ) );
        TDirectory * dirOutEta_( ( TDirectory* )( dirOutFit_->Get( binEta.c_str() ) ) );
        const std::string nameEta( name + "_" + binEta );

        for ( unsigned uEntry = 0; uEntry < sizeEta_.at( uEta ); ++uEntry ) {
          if ( ptData_.at( uEta ).at( uEntry ) == -9. || ptGenData_.at( uEta ).at( uEntry ) == -9. ) continue; // no match
          const Double_t ptVal( useNonT_ ? ptData_.at( uEta ).at( uEntry ) * std::cosh( etaData_.at( uEta ).at( uEntry ) ) : ptData_.at( uEta ).at( uEntry ) );
          const Double_t ptGenVal( useNonT_ ? ptGenData_.at( uEta ).at( uEntry ) * std::cosh( etaGenData_.at( uEta ).at( uEntry ) ) : ptGenData_.at( uEta ).at( uEntry ) );
          const Double_t value( refGen_ ? ptGenVal - ptVal : ptVal - ptGenVal );
          const Double_t ptRef( refGen_ ? ptGenVal : ptVal );
          const Double_t etaGenSymm( useSymm_ ? std::fabs( etaGenData_.at( uEta ).at( uEntry ) ) : etaGenData_.at( uEta ).at( uEntry ) );
          const Double_t etaSymm( useSymm_ ? std::fabs( etaData_.at( uEta ).at( uEntry ) ) : etaData_.at( uEta ).at( uEntry ) );
          const Double_t etaRef( refGen_ ? etaGenSymm : etaSymm );

          if ( fitNonRestr_ ) {
            const std::string nameEtaTrans( nameEta + "_Trans" );
            const std::string nameEtaTransRebin( nameEtaTrans + "Rebin" );
            TH1D * histEtaTransRebin( ( TH1D* )( dirOutEta_->Get( nameEtaTransRebin.c_str() ) ) );
            if ( histEtaTransRebin != 0 && ! ( scale_ && histEtaTransRebin->GetSumOfWeights() == 0. ) ) {
              const Double_t weight( scale_ ? weightData_.at( uEta ).at( uEntry ) / histEtaTransRebin->GetSumOfWeights() : weightData_.at( uEta ).at( uEntry ) );
              histTransRebinMapEta->Fill( etaRef, value, weight );
            }
          }
          if ( ptRef >= minPt_ && reco::deltaR( etaGenData_.at( uEta ).at( uEntry ), phiGenData_.at( uEta ).at( uEntry ), etaData_.at( uEta ).at( uEntry ), phiData_.at( uEta ).at( uEntry ) ) <= maxDR_ ) {
            const std::string nameEtaTransRestr( nameEta + "_TransRestr" );
            const std::string nameEtaTransRestrRebin( nameEtaTransRestr + "Rebin" );
            TH1D * histEtaTransRestrRebin( ( TH1D* )( dirOutEta_->Get( nameEtaTransRestrRebin.c_str() ) ) );
            if ( histEtaTransRestrRebin != 0 && ! ( scale_ && histEtaTransRestrRebin->GetSumOfWeights() == 0. ) ) {
              const Double_t weight( scale_ ? weightData_.at( uEta ).at( uEntry ) / histEtaTransRestrRebin->GetSumOfWeights() : weightData_.at( uEta ).at( uEntry ) );
              histTransRestrRebinMapEta->Fill( etaRef, value, weight );
            }
          }
          for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
            if ( ptBins_.at( uPt ) <= ptRef && ptRef < ptBins_.at( uPt + 1 ) ) {
              const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
              const std::string nameEtaPt( nameEta + "_" + baseTitlePt + binPt );
              const std::string nameEtaPtTrans( nameEtaPt + "_Trans" );
              if ( fitNonRestr_ ) {
                if ( histVecPtTransRebin.at( uPt) != 0 && ! ( scale_ && histVecPtTransRebin.at( uPt)->GetSumOfWeights() == 0. ) ) {
                  const Double_t weight( scale_ ? weightData_.at( uEta ).at( uEntry ) / histVecPtTransRebin.at( uPt)->GetSumOfWeights() : weightData_.at( uEta ).at( uEntry ) );
                  histTransRebinMapPt->Fill( ptRef, value, weight );
                }
                const std::string nameEtaPtTransRebin( nameEtaPtTrans + "Rebin" );
                TH1D * histEtaPtTransRebin( ( TH1D* )( dirOutEta_->Get( nameEtaPtTransRebin.c_str() ) ) );
                if ( histEtaPtTransRebin != 0 && ! ( scale_ && histEtaPtTransRebin->GetSumOfWeights() == 0. ) ) {
                  const Double_t weight( scale_ ? weightData_.at( uEta ).at( uEntry ) / histEtaPtTransRebin->GetSumOfWeights() : weightData_.at( uEta ).at( uEntry ) );
                  histVecPtTransRebinMapEta.at( uPt )->Fill( etaRef, value, weight );
                }
              }
              if ( ptRef >= minPt_ && reco::deltaR( etaGenData_.at( uEta ).at( uEntry ), phiGenData_.at( uEta ).at( uEntry ), etaData_.at( uEta ).at( uEntry ), phiData_.at( uEta ).at( uEntry ) ) <= maxDR_ ) {
                if ( histVecPtTransRestrRebin.at( uPt) != 0 && ! ( scale_ && histVecPtTransRestrRebin.at( uPt)->GetSumOfWeights() == 0. ) ) {
                  const Double_t weight( scale_ ? weightData_.at( uEta ).at( uEntry ) / histVecPtTransRestrRebin.at( uPt)->GetSumOfWeights() : weightData_.at( uEta ).at( uEntry ) );
                  histTransRestrRebinMapPt->Fill( ptRef, value, weight );
                }
                const std::string nameEtaPtTransRestrRebin( nameEtaPtTrans + "RestrRebin" );
                TH1D * histEtaPtTransRestrRebin( ( TH1D* )( dirOutEta_->Get( nameEtaPtTransRestrRebin.c_str() ) ) );
                if ( histEtaPtTransRestrRebin != 0 && ! ( scale_ && histEtaPtTransRestrRebin->GetSumOfWeights() == 0. ) ) {
                  const Double_t weight( scale_ ? weightData_.at( uEta ).at( uEntry ) / histEtaPtTransRestrRebin->GetSumOfWeights() : weightData_.at( uEta ).at( uEntry ) );
                  histVecPtTransRestrRebinMapEta.at( uPt )->Fill( etaRef, value, weight );
                }
              }
              break;
            }
          } // loop: uPt < nPtBins_

        } // loop: uEntry < sizeEta_.at( uEta )

      } // loop: keyEta

      if ( scale_ ) {
        if ( histTransRebin->GetSumOfWeights() != 0. ) histTransRebin->Scale( 1. / histTransRebin->GetSumOfWeights() );
        if ( histTransRebinMapPt->GetSumOfWeights() != 0. ) histTransRebinMapPt->Scale( 1. / histTransRebinMapPt->GetSumOfWeights() );
        if ( histTransRebinMapEta->GetSumOfWeights() != 0. ) histTransRebinMapEta->Scale( 1. / histTransRebinMapEta->GetSumOfWeights() );
        if ( histTransRestrRebin->GetSumOfWeights() != 0. ) histTransRestrRebin->Scale( 1. / histTransRestrRebin->GetSumOfWeights() );
        if ( histTransRestrRebinMapPt->GetSumOfWeights() != 0. ) histTransRestrRebinMapPt->Scale( 1. / histTransRestrRebinMapPt->GetSumOfWeights() );
        if ( histTransRestrRebinMapEta->GetSumOfWeights() != 0. ) histTransRestrRebinMapEta->Scale( 1. / histTransRestrRebinMapEta->GetSumOfWeights() );
        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          if ( histVecPtTransRebin.at( uPt )->GetSumOfWeights() != 0. ) histVecPtTransRebin.at( uPt )->Scale( 1. / histVecPtTransRebin.at( uPt )->GetSumOfWeights() );
          if ( histVecPtTransRebinMapEta.at( uPt )->GetSumOfWeights() != 0. ) histVecPtTransRebinMapEta.at( uPt )->Scale( 1. / histVecPtTransRebinMapEta.at( uPt )->GetSumOfWeights() );
          if ( histVecPtTransRestrRebin.at( uPt )->GetSumOfWeights() != 0. ) histVecPtTransRestrRebin.at( uPt )->Scale( 1. / histVecPtTransRestrRebin.at( uPt )->GetSumOfWeights() );
          if ( histVecPtTransRestrRebinMapEta.at( uPt )->GetSumOfWeights() != 0. ) histVecPtTransRestrRebinMapEta.at( uPt )->Scale( 1. / histVecPtTransRestrRebinMapEta.at( uPt )->GetSumOfWeights() );
        }
      }

      if ( plot_ ) {
        if ( fitNonRestr_ ) {
          histTransRebinMapPt->Draw();
          c1.Print( std::string( pathPlots_ + histTransRebinMapPt->GetName() + ".png" ).c_str() );
          histTransRebinMapEta->Draw();
          c1.Print( std::string( pathPlots_ + histTransRebinMapEta->GetName() + ".png" ).c_str() );
          for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
            histVecPtTransRebinMapEta.at( uPt )->Draw();
            c1.Print( std::string( pathPlots_ + histVecPtTransRebinMapEta.at( uPt )->GetName() + ".png" ).c_str() );
          }
        }
        histTransRestrRebinMapPt->Draw();
        c1.Print( std::string( pathPlots_ + histTransRestrRebinMapPt->GetName() + ".png" ).c_str() );
        histTransRestrRebinMapEta->Draw();
        c1.Print( std::string( pathPlots_ + histTransRestrRebinMapEta->GetName() + ".png" ).c_str() );
        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          histVecPtTransRestrRebinMapEta.at( uPt )->Draw();
          c1.Print( std::string( pathPlots_ + histVecPtTransRestrRebinMapEta.at( uPt )->GetName() + ".png" ).c_str() );
        }
      }

    }  // loop: keyFit


    // Fit transfer functions

    if ( doFit_ ) {
      if ( verbose_ > 1 ) {
        std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                  << "    transfer function determination for " << objCat << " started" << std::endl;
      }

      // Loop over fit versions
      nextInListProp.Reset();
      while ( TKey * keyFit = ( TKey* )nextInListProp() ) {
        if ( std::string( keyFit->GetClassName() ) != nameDirClass ) continue;
        const std::string subFit( keyFit->GetName() );
        if ( subFit.find( "Inv" ) != std::string::npos ) continue; // nothing to do for inverse
        // These are real switches: depending on configuration, only one setting combination can be run at a time
        if ( useAlt_  == ( subFit.find( "Alt" )  == std::string::npos ) ) continue;
        if ( useSymm_ == ( subFit.find( "Symm" ) == std::string::npos ) ) continue;
        if ( refGen_  == ( subFit.find( "Gen" )  == std::string::npos ) ) continue;
        TDirectory * dirOutFit_( ( TDirectory* )( dirOutPt_->Get( subFit.c_str() ) ) );
        if ( ! dirOutFit_ ) {
          std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                    << "    fit '" << subFit << "' does not exist in output file" << std::endl;
          returnStatus_ += 0x100000;
          continue;
        }
        dirOutFit_->cd();

        std::string name( objCat + "_" + baseTitlePt + "_" + subFit );

        // Transfer function parameters
        const std::string part( refGen_ ? "_parton" : "_reco" );
        std::stringstream comment( std::ios_base::out );
        my::TransferFunction transferPt( fitFunction_, dependencyFunction_, std::string( titlePtT + part ) );
        comment << baseTitlePt << part << " <= " << fitMaxPt_;
        transferPt.SetComment( comment.str() );
        my::TransferFunctionCollection transferVecEtaPt( nEtaBins_, transferPt );
        my::TransferFunction transferPtRestr( transferPt );
        comment << ", " << baseTitlePt << part << " >= " << minPt_ << ", " << "DeltaR(parton, reco) <= " << maxDR_;
        transferPtRestr.SetComment( comment.str() );
        my::TransferFunctionCollection transferVecEtaPtRestr( nEtaBins_, transferPtRestr );

        const unsigned nPar( transferPt.NParFit() );
        const unsigned nDep( transferPt.NParDependency() );

        const std::string nameTrans( name + "_Trans" );
        const std::string nameTransRebin( nameTrans + "Rebin" );
        TH1D * histTransRebin( ( TH1D* )( dirOutFit_->Get( nameTransRebin.c_str() ) ) );
        if ( fitNonRestr_ && histTransRebin != 0 ) {
          const std::string nameTransRebinFit( nameTransRebin + "_fit" );
          if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameTransRebinFit << std::endl;
          TF1 * fitTransRebin( new TF1( nameTransRebinFit.c_str(), fitFunction_.c_str(), std::max( histTransRebin->GetXaxis()->GetXmin(), histTransRebin->GetMean() - histTransRebin->GetRMS() * fitRange_ ), std::min( histTransRebin->GetXaxis()->GetXmax(), histTransRebin->GetMean() + histTransRebin->GetRMS() * fitRange_ ) ) );
          setParametersFit( objCat, fitTransRebin, histTransRebin, fitFuncId_, scale_ );
          TFitResultPtr fitTransRebinResultPtr( histTransRebin->Fit( fitTransRebin, fitOptions_.c_str() ) );
          if ( fitTransRebinResultPtr >= 0 ) {
            if ( fitTransRebinResultPtr->Status() == 0 && fitTransRebinResultPtr->Prob() > 0. && fitTransRebinResultPtr->Ndf() != 0. ) {
              if ( checkParametersFit( objCat, fitTransRebin, fitFuncId_ ) ) {
                mixParametersFit( transferPt, fitTransRebin );
              }
              else {
                for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
                  transferPt.SetParameter( uPar, fitTransRebin->GetParameter( uPar ) );
                }
              }
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; dirOutFit_->pwd();
                std::cout << "    '" << nameTransRebin << "' status " << fitTransRebinResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; dirOutFit_->pwd();
              std::cout << "    '" << nameTransRebin << std::endl;
            }
          }
          if ( plot_ ) {
            histTransRebin->Draw();
            c1.Print( std::string( pathPlots_ + histTransRebin->GetName() + ".png" ).c_str() );
          }
        }

        const std::string nameTransRestr( nameTrans + "Restr" );
        const std::string nameTransRestrRebin( nameTransRestr + "Rebin" );
        TH1D * histTransRestrRebin( ( TH1D* )( dirOutFit_->Get( nameTransRestrRebin.c_str() ) ) );
        if ( histTransRestrRebin != 0 ) {
          const std::string nameTransRestrRebinFit( nameTransRestrRebin + "_fit" );
          if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameTransRestrRebinFit << std::endl;
          TF1 * fitTransRestrRebin( new TF1( nameTransRestrRebinFit.c_str(), fitFunction_.c_str(), std::max( histTransRestrRebin->GetXaxis()->GetXmin(), histTransRestrRebin->GetMean() - histTransRestrRebin->GetRMS() * fitRange_ ), std::min( histTransRestrRebin->GetXaxis()->GetXmax(), histTransRestrRebin->GetMean() + histTransRestrRebin->GetRMS() * fitRange_ ) ) );
          setParametersFit( objCat, fitTransRestrRebin, histTransRestrRebin, fitFuncId_, scale_ );
          TFitResultPtr fitTransRestrRebinResultPtr( histTransRestrRebin->Fit( fitTransRestrRebin, fitOptions_.c_str() ) );
          if ( fitTransRestrRebinResultPtr >= 0 ) {
            if ( fitTransRestrRebinResultPtr->Status() == 0 && fitTransRestrRebinResultPtr->Prob() > 0. && fitTransRestrRebinResultPtr->Ndf() != 0. ) {
              if ( checkParametersFit( objCat, fitTransRestrRebin, fitFuncId_ ) ) {
                mixParametersFit( transferPtRestr, fitTransRestrRebin );
              }
              else {
                for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
                  transferPtRestr.SetParameter( uPar, fitTransRestrRebin->GetParameter( uPar ) );
                }
              }
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; dirOutFit_->pwd();
                std::cout << "    '" << nameTransRestrRebin << "' status " << fitTransRestrRebinResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; dirOutFit_->pwd();
              std::cout << "    '" << nameTransRestrRebin << std::endl;
            }
          }
          if ( plot_ ) {
            histTransRestrRebin->Draw();
            c1.Print( std::string( pathPlots_ + histTransRestrRebin->GetName() + ".png" ).c_str() );
          }
        }

        // Loop over pt bins

        std::vector< TH1D * > histVecTransRebinPtFitMap;
        std::vector< TH1D * > histVecTransRestrRebinPtFitMap;
        for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
          const std::string parFit( boost::lexical_cast< std::string >( uPar ) );
          const std::string nameTransRebinPtFitMap( nameTransRebin + baseTitlePt + "_FitMap_Par" + parFit );
          const std::string titleTransRebinPtFitMap( objCat + ", par. " + parFit );
          TH1D * histTransRebinPtFitMap( new TH1D( nameTransRebinPtFitMap.c_str(), titleTransRebinPtFitMap.c_str(), nPtBins_, ptBins_.data() ) );
          histTransRebinPtFitMap->SetXTitle( titlePt.c_str() );
          histVecTransRebinPtFitMap.push_back( histTransRebinPtFitMap );
          const std::string nameTransRestrRebinPtFitMap( nameTransRestrRebin + baseTitlePt + "_FitMap_Par" + parFit );
          TH1D * histTransRestrRebinPtFitMap( new TH1D( nameTransRestrRebinPtFitMap.c_str(), titleTransRebinPtFitMap.c_str(), nPtBins_, ptBins_.data() ) );
          histTransRestrRebinPtFitMap->SetXTitle( titlePt.c_str() );
          histVecTransRestrRebinPtFitMap.push_back( histTransRestrRebinPtFitMap );
        }
        const std::string titleTransRebinPtFitMapProb( objCat + ", fit prob." );
        const std::string nameTransRebinPtFitMapProb( nameTransRebin + baseTitlePt + "_FitMap_Prob" );
        TH1D * histTransRebinPtFitMapProb( new TH1D( nameTransRebinPtFitMapProb.c_str(), titleTransRebinPtFitMapProb.c_str(), nPtBins_, ptBins_.data() ) );
        histTransRebinPtFitMapProb->SetXTitle( titlePt.c_str() );
        histTransRebinPtFitMapProb->SetYTitle( titleProb.c_str() );
        const std::string nameTransRestrRebinPtFitMapProb( nameTransRestrRebin + baseTitlePt + "_FitMap_Prob" );
        TH1D * histTransRestrRebinPtFitMapProb( new TH1D( nameTransRestrRebinPtFitMapProb.c_str(), titleTransRebinPtFitMapProb.c_str(), nPtBins_, ptBins_.data() ) );
        histTransRestrRebinPtFitMapProb->SetXTitle( titlePt.c_str() );
        histTransRestrRebinPtFitMapProb->SetYTitle( titleProb.c_str() );

        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
          const std::string namePt( name + "_" + baseTitlePt + binPt );

          const std::string namePtTransRebin( namePt + "_TransRebin" );
          TH1D * histPtTransRebin( ( TH1D* )( dirOutFit_->Get( namePtTransRebin.c_str() ) ) );
          if ( fitNonRestr_ && histPtTransRebin != 0 ) {
            const std::string namePtTransRebinFit( namePtTransRebin + "_fit" );
            if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << namePtTransRebinFit << std::endl;
            TF1 * fitPtTransRebin( new TF1( namePtTransRebinFit.c_str(), fitFunction_.c_str(), std::max( histPtTransRebin->GetXaxis()->GetXmin(), histPtTransRebin->GetMean() - histPtTransRebin->GetRMS() * fitRange_ ), std::min( histPtTransRebin->GetXaxis()->GetXmax(), histPtTransRebin->GetMean() + histPtTransRebin->GetRMS() * fitRange_ ) ) );
            setParametersFit( objCat, fitPtTransRebin, histPtTransRebin, fitFuncId_, scale_ );
            TFitResultPtr fitPtTransRebinResultPtr( histPtTransRebin->Fit( fitPtTransRebin, fitOptions_.c_str() ) );
            if ( fitPtTransRebinResultPtr >= 0 ) {
              if ( fitPtTransRebinResultPtr->Status() == 0 && fitPtTransRebinResultPtr->Prob() > 0. && fitPtTransRebinResultPtr->Ndf() != 0. ) {
                if ( checkParametersFit( objCat, fitPtTransRebin, fitFuncId_ ) ) {
                  fillMixedParametersFit( uPt + 1, histVecTransRebinPtFitMap, fitPtTransRebinResultPtr );
                }
                else {
                  for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
                    histVecTransRebinPtFitMap.at( uPar )->SetBinContent( uPt + 1, fitPtTransRebinResultPtr->Parameter( uPar ) );
                    histVecTransRebinPtFitMap.at( uPar )->SetBinError( uPt + 1, fitPtTransRebinResultPtr->ParError( uPar ) );
                  }
                }
                histTransRebinPtFitMapProb->SetBinContent( uPt + 1, log10( fitPtTransRebinResultPtr->Prob() ) );
              }
              else {
                histTransRebinPtFitMapProb->SetBinContent( uPt + 1, 1 );
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; dirOutFit_->pwd();
                  std::cout << "    '" << namePtTransRebin << "' status " << fitPtTransRebinResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; dirOutFit_->pwd();
                std::cout << "    '" << namePtTransRebin << std::endl;
              }
            }
          }

          const std::string namePtTransRestr( namePt + "_TransRestr" );
          const std::string namePtTransRestrRebin( namePtTransRestr + "Rebin" );
          TH1D * histPtTransRestrRebin( ( TH1D* )( dirOutFit_->Get( namePtTransRestrRebin.c_str() ) ) );
          if ( histPtTransRestrRebin != 0 ) {
            const std::string namePtTransRestrRebinFit( namePtTransRestrRebin + "_fit" );
            if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << namePtTransRestrRebinFit << std::endl;
            TF1 * fitPtTransRestrRebin( new TF1( namePtTransRestrRebinFit.c_str(), fitFunction_.c_str(), std::max( histPtTransRestrRebin->GetXaxis()->GetXmin(), histPtTransRestrRebin->GetMean() - histPtTransRestrRebin->GetRMS() * fitRange_ ), std::min( histPtTransRestrRebin->GetXaxis()->GetXmax(), histPtTransRestrRebin->GetMean() + histPtTransRestrRebin->GetRMS() * fitRange_ ) ) );
            setParametersFit( objCat, fitPtTransRestrRebin, histPtTransRestrRebin, fitFuncId_, scale_ );
            TFitResultPtr fitPtTransRestrRebinResultPtr( histPtTransRestrRebin->Fit( fitPtTransRestrRebin, fitOptions_.c_str() ) );
            if ( fitPtTransRestrRebinResultPtr >= 0 ) {
              if ( fitPtTransRestrRebinResultPtr->Status() == 0 && fitPtTransRestrRebinResultPtr->Prob() > 0. && fitPtTransRestrRebinResultPtr->Ndf() != 0. ) {
                if ( checkParametersFit( objCat, fitPtTransRestrRebin, fitFuncId_ ) ) {
                  fillMixedParametersFit( uPt + 1, histVecTransRestrRebinPtFitMap, fitPtTransRestrRebinResultPtr );
                }
                else {
                  for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
                    histVecTransRestrRebinPtFitMap.at( uPar )->SetBinContent( uPt + 1, fitPtTransRestrRebinResultPtr->Parameter( uPar ) );
                    histVecTransRestrRebinPtFitMap.at( uPar )->SetBinError( uPt + 1, fitPtTransRestrRebinResultPtr->ParError( uPar ) );
                  }
                }
                histTransRestrRebinPtFitMapProb->SetBinContent( uPt + 1, log10( fitPtTransRestrRebinResultPtr->Prob() ) );
              }
              else {
                histTransRestrRebinPtFitMapProb->SetBinContent( uPt + 1, 1 );
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; dirOutFit_->pwd();
                  std::cout << "    '" << namePtTransRestrRebin << "' status " << fitPtTransRestrRebinResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; dirOutFit_->pwd();
                std::cout << "    '" << namePtTransRestrRebin << std::endl;
              }
            }
          }

          if ( plot_ ) {
            if ( fitNonRestr_ ) {
              histPtTransRebin->Draw();
              c1.Print( std::string( pathPlots_ + histPtTransRebin->GetName() + ".png" ).c_str() );
            }
            histPtTransRestrRebin->Draw();
            c1.Print( std::string( pathPlots_ + histPtTransRestrRebin->GetName() + ".png" ).c_str() );
          }

        } // loop: uPt < nPtBins_

        for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
          const std::string parFit( boost::lexical_cast< std::string >( uPar ) );

          if ( fitNonRestr_ ) {
            const std::string nameTransRebinPtFitMap( name + "_TransRebin" + baseTitlePt + "_FitMap_Par" + parFit );
            const std::string nameTransRebinPtFitMapFit( nameTransRebinPtFitMap + "_fit" );
            if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameTransRebinPtFitMapFit << std::endl;
            TF1 * fitTransRebinPtFitMap( new TF1( nameTransRebinPtFitMapFit.c_str(), dependencyFunction_.c_str(), ptBins_.front(), fitMaxPt_ ) );
            setParametersDependency( objCat, fitTransRebinPtFitMap, histVecTransRebinPtFitMap.at( uPar ), fitFuncId_, depFuncId_, uPar );
            TFitResultPtr fitTransRebinPtFitMapResultPtr( histVecTransRebinPtFitMap.at( uPar )->Fit( fitTransRebinPtFitMap, fitOptions_.c_str() ) );
            if ( fitTransRebinPtFitMapResultPtr >= 0 ) {
              if ( fitTransRebinPtFitMapResultPtr->Status() == 0 && fitTransRebinPtFitMapResultPtr->Prob() > 0. && fitTransRebinPtFitMapResultPtr->Ndf() != 0. ) {
                for ( unsigned uDep = 0; uDep < nDep; ++uDep ) {
                  transferPt.SetParameter( uPar, uDep, fitTransRebinPtFitMap->GetParameter( uDep ) );
                }
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; dirOutFit_->pwd();
                  std::cout << "    '" << nameTransRebinPtFitMap << "' status " << fitTransRebinPtFitMapResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; dirOutFit_->pwd();
                std::cout << "    '" << nameTransRebinPtFitMap << std::endl;
              }
            }
          }

          const std::string nameTransRestrRebinPtFitMap( name + "_TransRestrRebin" + baseTitlePt + "_FitMap_Par" + parFit );
          const std::string nameTransRestrRebinPtFitMapFit( nameTransRestrRebinPtFitMap + "_fit" );
          if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameTransRestrRebinPtFitMapFit << std::endl;
          TF1 * fitTransRestrRebinPtFitMap( new TF1( nameTransRestrRebinPtFitMapFit.c_str(), dependencyFunction_.c_str(), ptBins_.front(), fitMaxPt_ ) );
          setParametersDependency( objCat, fitTransRestrRebinPtFitMap, histVecTransRestrRebinPtFitMap.at( uPar ), fitFuncId_, depFuncId_, uPar );
          TFitResultPtr fitTransRestrRebinPtFitMapResultPtr( histVecTransRestrRebinPtFitMap.at( uPar )->Fit( fitTransRestrRebinPtFitMap, fitOptions_.c_str() ) );
          if ( fitTransRestrRebinPtFitMapResultPtr >= 0 ) {
            if ( fitTransRestrRebinPtFitMapResultPtr->Status() == 0 && fitTransRestrRebinPtFitMapResultPtr->Prob() > 0. && fitTransRestrRebinPtFitMapResultPtr->Ndf() != 0. ) {
              for ( unsigned uDep = 0; uDep < nDep; ++uDep ) {
                transferPtRestr.SetParameter( uPar, uDep, fitTransRestrRebinPtFitMap->GetParameter( uDep ) );
              }
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; dirOutFit_->pwd();
                std::cout << "    '" << nameTransRestrRebinPtFitMap << "' status " << fitTransRestrRebinPtFitMapResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; dirOutFit_->pwd();
              std::cout << "    '" << nameTransRestrRebinPtFitMap << std::endl;
            }
          }
        } // loop: uPar < nPar

        dirOutFit_->cd();
        const std::string nameTransferFunctionPt( name + "_TransferFunction" );
        if ( fitNonRestr_ ) {
          TF2 * transferFunctionPt( new TF2( ( ( TF2& )( *( transferPt.Function( norm_ ).Clone( nameTransferFunctionPt.c_str() ) ) ) ) ) );
          transferFunctionPt->SetRange( -2. * widthFactor_ * histMax_, 0., 2. * widthFactor_ * histMax_, 2. * ptBins_.back() );
          transferFunctionPt->SetNpx();
          transferFunctionPt->SetNpy();
          transferFunctionPt->GetXaxis()->SetTitle( titleTrans.c_str() );
          transferFunctionPt->GetYaxis()->SetTitle( titlePt.c_str() );
          if ( overwrite_ ) transferFunctionPt->Write( 0, TObject::kOverwrite );
          else              transferFunctionPt->Write();
          if ( plot_ ) {
            transferFunctionPt->Draw( "Surf3Z" );
            c1.Print( std::string( pathPlots_ + transferFunctionPt->GetName() + ".png" ).c_str() );
          }
        }
        const std::string nameTransferFunctionPtRestr( nameTransferFunctionPt + "Restr" );
        TF2 * transferFunctionPtRestr( new TF2( ( ( TF2& )( *( transferPtRestr.Function( norm_ ).Clone( nameTransferFunctionPtRestr.c_str() ) ) ) ) ) );
        transferFunctionPtRestr->SetRange( -2. * widthFactor_ * histMax_, 0., 2. * widthFactor_ * histMax_, 2. * ptBins_.back() );
        transferFunctionPtRestr->SetNpx();
        transferFunctionPtRestr->SetNpy();
        transferFunctionPtRestr->GetXaxis()->SetTitle( titleTrans.c_str() );
        transferFunctionPtRestr->GetYaxis()->SetTitle( titlePt.c_str() );
        if ( overwrite_ ) transferFunctionPtRestr->Write( 0, TObject::kOverwrite );
        else              transferFunctionPtRestr->Write();
        if ( plot_ ) {
          transferFunctionPtRestr->Draw( "Surf3Z" );
          c1.Print( std::string( pathPlots_ + transferFunctionPtRestr->GetName() + ".png" ).c_str() );
        }

        if ( plot_ ) {
          for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
            if ( fitNonRestr_ ) {
              histVecTransRebinPtFitMap.at( uPar )->Draw();
              c1.Print( std::string( pathPlots_ + histVecTransRebinPtFitMap.at( uPar )->GetName() + ".png" ).c_str() );
            }
            histVecTransRestrRebinPtFitMap.at( uPar )->Draw();
            c1.Print( std::string( pathPlots_ + histVecTransRestrRebinPtFitMap.at( uPar )->GetName() + ".png" ).c_str() );
          }
          if ( fitNonRestr_ ) {
            histTransRebinPtFitMapProb->Draw();
            c1.Print( std::string( pathPlots_ + histTransRebinPtFitMapProb->GetName() + ".png" ).c_str() );
          }
          histTransRestrRebinPtFitMapProb->Draw();
          c1.Print( std::string( pathPlots_ + histTransRestrRebinPtFitMapProb->GetName() + ".png" ).c_str() );
        }

        // Loop over eta bins

        std::vector< TH1D * > histVecTransRebinEtaFitMap;
        std::vector< TH1D * > histVecTransRestrRebinEtaFitMap;
        for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
          const std::string parFit( boost::lexical_cast< std::string >( uPar ) );
          const std::string nameTransRebinEtaFitMap( nameTransRebin + "Eta_FitMap_Par" + parFit );
          const std::string titleTransRebinEtaFitMap( objCat + ", par. " + parFit );
          TH1D * histTransRebinEtaFitMap( new TH1D( nameTransRebinEtaFitMap.c_str(), titleTransRebinEtaFitMap.c_str(), nEtaBins_, etaBins_.data() ) );
          histTransRebinEtaFitMap->SetXTitle( titleEta.c_str() );
          histVecTransRebinEtaFitMap.push_back( histTransRebinEtaFitMap );
          const std::string nameTransRestrRebinEtaFitMap( name + "_TransRestrRebinEta_FitMap_Par" + parFit );
          TH1D * histTransRestrRebinEtaFitMap( new TH1D( nameTransRestrRebinEtaFitMap.c_str(), titleTransRebinEtaFitMap.c_str(), nEtaBins_, etaBins_.data() ) );
          histTransRestrRebinEtaFitMap->SetXTitle( titleEta.c_str() );
          histVecTransRestrRebinEtaFitMap.push_back( histTransRestrRebinEtaFitMap );
        }
        const std::string titleTransRebinEtaFitMapProb( objCat + ", fit prob." );
        const std::string nameTransRebinEtaFitMapProb( nameTransRebin + "Eta_FitMap_Prob" );
        TH1D * histTransRebinEtaFitMapProb( new TH1D( nameTransRebinEtaFitMapProb.c_str(), titleTransRebinEtaFitMapProb.c_str(), nEtaBins_, etaBins_.data() ) );
        histTransRebinEtaFitMapProb->SetXTitle( titleEta.c_str() );
        histTransRebinEtaFitMapProb->SetYTitle( titleProb.c_str() );
        const std::string nameTransRestrRebinEtaFitMapProb( nameTransRestrRebin + "Eta_FitMap_Prob" );
        TH1D * histTransRestrRebinEtaFitMapProb( new TH1D( nameTransRestrRebinEtaFitMapProb.c_str(), titleTransRebinEtaFitMapProb.c_str(), nEtaBins_, etaBins_.data() ) );
        histTransRestrRebinEtaFitMapProb->SetXTitle( titleEta.c_str() );
        histTransRestrRebinEtaFitMapProb->SetYTitle( titleProb.c_str() );

        std::vector< std::vector< TH1D * > > histVecVecTransRebinEtaParMap;
        std::vector< std::vector< TH1D * > > histVecVecTransRestrRebinEtaParMap;
        if ( fitEtaBins_ ) {
          for ( unsigned uDep = 0; uDep < nDep; ++uDep ) {
            const std::string parDep( boost::lexical_cast< std::string >( uDep ) );
            std::vector< TH1D * > histVecTransRebinEtaParMap;
            std::vector< TH1D * > histVecTransRestrRebinEtaParMap;
            for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
              const std::string parFit( boost::lexical_cast< std::string >( uPar ) );
              const std::string nameTransRebinEtaParMap( name + "_TransRebinEta_ParMap_Par" + parFit );
              const std::string titleTransRebinEtaParMap( objCat + ", par. " + parFit + ", " );
              TH1D * histTransRebinEtaParMap( new TH1D( std::string( nameTransRebinEtaParMap + "-" + parDep ).c_str(), std::string( titleTransRebinEtaParMap + parDep ).c_str(), nEtaBins_, etaBins_.data() ) );
              histTransRebinEtaParMap->SetXTitle( titleEta.c_str() );
              histVecTransRebinEtaParMap.push_back( histTransRebinEtaParMap );
              const std::string nameTransRestrRebinEtaParMap( name + "_TransRestrRebinEta_ParMap_Par" + parFit );
              TH1D * histTransRestrRebinEtaParMap( new TH1D( std::string( nameTransRestrRebinEtaParMap + "-" + parDep ).c_str(), std::string( titleTransRebinEtaParMap + parDep ).c_str(), nEtaBins_, etaBins_.data() ) );
              histTransRestrRebinEtaParMap->SetXTitle( titleEta.c_str() );
              histVecTransRestrRebinEtaParMap.push_back( histTransRestrRebinEtaParMap );
            }
            histVecVecTransRebinEtaParMap.push_back( histVecTransRebinEtaParMap );
            histVecVecTransRestrRebinEtaParMap.push_back( histVecTransRestrRebinEtaParMap );
          }
        }

        TList * listFit( dirOutFit_->GetListOfKeys() );
        TIter nextInListFit( listFit );
        while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
          if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
          const std::string binEta( keyEta->GetName() );
          const unsigned uEta( std::atoi( binEta.substr( 3 ).data() ) );
          TDirectory * dirOutEta_( ( TDirectory* )( dirOutFit_->Get( binEta.c_str() ) ) );
          if ( ! dirOutEta_ ) {
            std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                      << "    fit '" << binEta << "' does not exist in output file" << std::endl;
            returnStatus_ += 0x200000;
            continue;
          }
          dirOutEta_->cd();

          const std::string nameEta( name + "_" + binEta );

          const std::string nameEtaTrans( nameEta + "_Trans" );
          const std::string nameEtaTransRebin( nameEtaTrans + "Rebin" );
          if ( fitNonRestr_ ) {
            TH1D * histEtaTransRebin( ( TH1D* )( dirOutEta_->Get( nameEtaTransRebin.c_str() ) ) );
            if ( histEtaTransRebin != 0 ) {
              const std::string nameEtaTransRebinFit( nameEtaTransRebin + "_fit" );
              if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameEtaTransRebinFit << std::endl;
              TF1 * fitEtaTransRebin( new TF1( nameEtaTransRebinFit.c_str(), fitFunction_.c_str(), std::max( histEtaTransRebin->GetXaxis()->GetXmin(), histEtaTransRebin->GetMean() - histEtaTransRebin->GetRMS() * fitRange_ ), std::min( histEtaTransRebin->GetXaxis()->GetXmax(), histEtaTransRebin->GetMean() + histEtaTransRebin->GetRMS() * fitRange_ ) ) );
              setParametersFit( objCat, fitEtaTransRebin, histEtaTransRebin, fitFuncId_, scale_ );
              TFitResultPtr fitEtaTransRebinResultPtr( histEtaTransRebin->Fit( fitEtaTransRebin, fitOptions_.c_str() ) );
              if ( fitEtaTransRebinResultPtr >= 0 ) {
                if ( fitEtaTransRebinResultPtr->Status() == 0 && fitEtaTransRebinResultPtr->Prob() > 0. && fitEtaTransRebinResultPtr->Ndf() != 0. ) {
                  if ( checkParametersFit( objCat, fitEtaTransRebin, fitFuncId_ ) ) {
                    mixParametersFit( transferVecEtaPt.at( uEta ), fitEtaTransRebin );
                    fillMixedParametersFit( uEta + 1, histVecTransRebinEtaFitMap, fitEtaTransRebinResultPtr );
                  }
                  else {
                    for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
                      histVecTransRebinEtaFitMap.at( uPar )->SetBinContent( uEta + 1, fitEtaTransRebinResultPtr->Parameter( uPar ) );
                      histVecTransRebinEtaFitMap.at( uPar )->SetBinError( uEta + 1, fitEtaTransRebinResultPtr->ParError( uPar ) );
                      transferVecEtaPt.at( uEta ).SetParameter( uPar, fitEtaTransRebin->GetParameter( uPar ) );
                    }
                  }
                  histTransRebinEtaFitMapProb->SetBinContent( uEta + 1, log10( fitEtaTransRebinResultPtr->Prob() ) );
                }
                else {
                  histTransRebinEtaFitMapProb->SetBinContent( uEta + 1, 1 );
                  if ( verbose_ > 2 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    failing fit in directory '"; dirOutEta_->pwd();
                    std::cout << "    '" << nameEtaTransRebin << "' status " << fitEtaTransRebinResultPtr->Status() << std::endl;
                  }
                }
              }
              else {
                if ( verbose_ > 1 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    missing fit in directory '"; dirOutEta_->pwd();
                  std::cout << "    '" << nameEtaTransRebin << std::endl;
                }
              }
            }
            if ( plot_ ) {
              histEtaTransRebin->Draw();
              c1.Print( std::string( pathPlots_ + histEtaTransRebin->GetName() + ".png" ).c_str() );
            }
          }

          const std::string nameEtaTransRestr( nameEta + "_TransRestr" );
          const std::string nameEtaTransRestrRebin( nameEtaTransRestr + "Rebin" );
          TH1D * histEtaTransRestrRebin( ( TH1D* )( dirOutEta_->Get( nameEtaTransRestrRebin.c_str() ) ) );
          if ( histEtaTransRestrRebin != 0 ) {
            const std::string nameEtaTransRestrRebinFit( nameEtaTransRestrRebin + "_fit" );
            if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameEtaTransRestrRebinFit << std::endl;
            TF1 * fitEtaTransRestrRebin( new TF1( nameEtaTransRestrRebinFit.c_str(), fitFunction_.c_str(), std::max( histEtaTransRestrRebin->GetXaxis()->GetXmin(), histEtaTransRestrRebin->GetMean() - histEtaTransRestrRebin->GetRMS() * fitRange_ ), std::min( histEtaTransRestrRebin->GetXaxis()->GetXmax(), histEtaTransRestrRebin->GetMean() + histEtaTransRestrRebin->GetRMS() * fitRange_ ) ) );
            setParametersFit( objCat, fitEtaTransRestrRebin, histEtaTransRestrRebin, fitFuncId_, scale_ );
            TFitResultPtr fitEtaTransRestrRebinResultPtr( histEtaTransRestrRebin->Fit( fitEtaTransRestrRebin, fitOptions_.c_str() ) );
            if ( fitEtaTransRestrRebinResultPtr >= 0 ) {
              if ( fitEtaTransRestrRebinResultPtr->Status() == 0 && fitEtaTransRestrRebinResultPtr->Prob() > 0. && fitEtaTransRestrRebinResultPtr->Ndf() != 0. ) {
                if ( checkParametersFit( objCat, fitEtaTransRestrRebin, fitFuncId_ ) ) {
                  mixParametersFit( transferVecEtaPtRestr.at( uEta ), fitEtaTransRestrRebin );
                  fillMixedParametersFit( uEta + 1, histVecTransRestrRebinEtaFitMap, fitEtaTransRestrRebinResultPtr );
                }
                else {
                  for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
                    histVecTransRestrRebinEtaFitMap.at( uPar )->SetBinContent( uEta + 1, fitEtaTransRestrRebinResultPtr->Parameter( uPar ) );
                    histVecTransRestrRebinEtaFitMap.at( uPar )->SetBinError( uEta + 1, fitEtaTransRestrRebinResultPtr->ParError( uPar ) );
                    transferVecEtaPtRestr.at( uEta ).SetParameter( uPar, fitEtaTransRestrRebin->GetParameter( uPar ) );
                  }
                }
                histTransRestrRebinEtaFitMapProb->SetBinContent( uEta + 1, log10( fitEtaTransRestrRebinResultPtr->Prob() ) );
              }
              else {
                histTransRestrRebinEtaFitMapProb->SetBinContent( uEta + 1, 1 );
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; dirOutEta_->pwd();
                  std::cout << "    '" << nameEtaTransRestrRebin << "' status " << fitEtaTransRestrRebinResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; dirOutEta_->pwd();
                std::cout << "    '" << nameEtaTransRestrRebin << std::endl;
              }
            }
          }
          if ( plot_ ) {
            histEtaTransRestrRebin->Draw();
            c1.Print( std::string( pathPlots_ + histEtaTransRestrRebin->GetName() + ".png" ).c_str() );
          }

          if ( fitEtaBins_ ) {

            // Loop over pt bins

            std::vector< TH1D * > histVecTransRebinEtaPtFitMap;
            std::vector< TH1D * > histVecTransRestrRebinEtaPtFitMap;
            for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
              const std::string parFit( boost::lexical_cast< std::string >( uPar ) );
              const std::string nameTransRebinEtaPtFitMap( nameEta + "_TransRebin" + baseTitlePt + "_FitMap_Par" + parFit );
              const std::string titleTransRebinEtaPtFitMap( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta < " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) + ", par. " + parFit );
              TH1D * histTransRebinEtaPtFitMap( new TH1D( nameTransRebinEtaPtFitMap.c_str(), titleTransRebinEtaPtFitMap.c_str(), nPtBins_, ptBins_.data() ) );
              histTransRebinEtaPtFitMap->SetXTitle( titlePt.c_str() );
              histVecTransRebinEtaPtFitMap.push_back( histTransRebinEtaPtFitMap );
              const std::string nameTransRestrRebinEtaPtFitMap( nameEta + "_TransRestrRebin" + baseTitlePt + "_FitMap_Par" + parFit );
              TH1D * histTransRestrRebinEtaPtFitMap( new TH1D( nameTransRestrRebinEtaPtFitMap.c_str(), titleTransRebinEtaPtFitMap.c_str(), nPtBins_, ptBins_.data() ) );
              histTransRestrRebinEtaPtFitMap->SetXTitle( titlePt.c_str() );
              histVecTransRestrRebinEtaPtFitMap.push_back( histTransRestrRebinEtaPtFitMap );
            }
            const std::string titleTransRebinEtaPtFitMapProb( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta < " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) + ", fit prob." );
            const std::string nameTransRebinEtaPtFitMapProb( nameEtaTransRebin + baseTitlePt + "_FitMap_Prob" );
            TH1D * histTransRebinEtaPtFitMapProb( new TH1D( nameTransRebinEtaPtFitMapProb.c_str(), titleTransRebinEtaPtFitMapProb.c_str(), nPtBins_, ptBins_.data() ) );
            histTransRebinEtaPtFitMapProb->SetXTitle( titlePt.c_str() );
            histTransRebinEtaPtFitMapProb->SetYTitle( titleProb.c_str() );
            const std::string nameTransRestrRebinEtaPtFitMapProb( nameEtaTransRestrRebin + baseTitlePt + "_FitMap_Prob" );
            TH1D * histTransRestrRebinEtaPtFitMapProb( new TH1D( nameTransRestrRebinEtaPtFitMapProb.c_str(), titleTransRebinEtaPtFitMapProb.c_str(), nPtBins_, ptBins_.data() ) );
            histTransRestrRebinEtaPtFitMapProb->SetXTitle( titlePt.c_str() );
            histTransRestrRebinEtaPtFitMapProb->SetYTitle( titleProb.c_str() );

            for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
              const std::string binPt( boost::lexical_cast< std::string >( uPt ) );

              const std::string nameEtaPt( nameEta + "_" + baseTitlePt + binPt );
              const std::string nameEtaPtTrans( nameEtaPt + "_Trans" );

              if ( fitNonRestr_ ) {
                const std::string nameEtaPtTransRebin( nameEtaPtTrans + "Rebin" );
                TH1D * histEtaPtTransRebin( ( TH1D* )( dirOutEta_->Get( nameEtaPtTransRebin.c_str() ) ) );
                if ( histEtaPtTransRebin != 0 ) {
                  const std::string nameEtaPtTransRebinFit( nameEtaPtTransRebin + "_fit" );
                  if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameEtaPtTransRebinFit << std::endl;
                  TF1 * fitEtaPtTransRebin( new TF1( nameEtaPtTransRebinFit.c_str(), fitFunction_.c_str(), std::max( histEtaPtTransRebin->GetXaxis()->GetXmin(), histEtaPtTransRebin->GetMean() - histEtaPtTransRebin->GetRMS() * fitRange_ ), std::min( histEtaPtTransRebin->GetXaxis()->GetXmax(), histEtaPtTransRebin->GetMean() + histEtaPtTransRebin->GetRMS() * fitRange_ ) ) );
                  setParametersFit( objCat, fitEtaPtTransRebin, histEtaPtTransRebin, fitFuncId_, scale_ );
                  TFitResultPtr fitEtaPtTransRebinResultPtr( histEtaPtTransRebin->Fit( fitEtaPtTransRebin, fitOptions_.c_str() ) );
                  if ( fitEtaPtTransRebinResultPtr >= 0 ) {
                    if ( fitEtaPtTransRebinResultPtr->Status() == 0 && fitEtaPtTransRebinResultPtr->Prob() > 0. && fitEtaPtTransRebinResultPtr->Ndf() != 0. ) {
                      if ( checkParametersFit( objCat, fitEtaPtTransRebin, fitFuncId_ ) ) {
                        fillMixedParametersFit( uPt + 1, histVecTransRebinEtaPtFitMap, fitEtaPtTransRebinResultPtr );
                      }
                      else {
                        for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
                          histVecTransRebinEtaPtFitMap.at( uPar )->SetBinContent( uPt + 1, fitEtaPtTransRebinResultPtr->Parameter( uPar ) );
                          histVecTransRebinEtaPtFitMap.at( uPar )->SetBinError( uPt + 1, fitEtaPtTransRebinResultPtr->ParError( uPar ) );
                        }
                      }
                      histTransRebinEtaPtFitMapProb->SetBinContent( uPt + 1, log10( fitEtaPtTransRebinResultPtr->Prob() ) );
                    }
                    else {
                      histTransRebinEtaPtFitMapProb->SetBinContent( uPt + 1, 1 );
                      if ( verbose_ > 2 ) {
                        std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                                  << "    failing fit in directory '"; dirOutEta_->pwd();
                        std::cout << "    '" << nameEtaPtTransRebin << "' status " << fitEtaPtTransRebinResultPtr->Status() << std::endl;
                      }
                    }
                  }
                  else {
                    if ( verbose_ > 1 ) {
                      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                                << "    missing fit in directory '"; dirOutEta_->pwd();
                      std::cout << "    '" << nameEtaPtTransRebin << std::endl;
                    }
                  }
                  if ( plot_ ) {
                    histEtaPtTransRebin->Draw();
                    c1.Print( std::string( pathPlots_ + histEtaPtTransRebin->GetName() + ".png" ).c_str() );
                  }
                }
              }

              const std::string nameEtaPtTransRestrRebin( nameEtaPtTrans + "RestrRebin" );
              TH1D * histEtaPtTransRestrRebin( ( TH1D* )( dirOutEta_->Get( nameEtaPtTransRestrRebin.c_str() ) ) );
              if ( histEtaPtTransRestrRebin != 0 ) {
                const std::string nameEtaPtTransRestrRebinFit( nameEtaPtTransRestrRebin + "_fit" );
                if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameEtaPtTransRestrRebinFit << std::endl;
                TF1 * fitEtaPtTransRestrRebin( new TF1( nameEtaPtTransRestrRebinFit.c_str(), fitFunction_.c_str(), std::max( histEtaPtTransRestrRebin->GetXaxis()->GetXmin(), histEtaPtTransRestrRebin->GetMean() - histEtaPtTransRestrRebin->GetRMS() * fitRange_ ), std::min( histEtaPtTransRestrRebin->GetXaxis()->GetXmax(), histEtaPtTransRestrRebin->GetMean() + histEtaPtTransRestrRebin->GetRMS() * fitRange_ ) ) );
                setParametersFit( objCat, fitEtaPtTransRestrRebin, histEtaPtTransRestrRebin, fitFuncId_, scale_ );
                TFitResultPtr fitEtaPtTransRestrRebinResultPtr( histEtaPtTransRestrRebin->Fit( fitEtaPtTransRestrRebin, fitOptions_.c_str() ) );
                if ( fitEtaPtTransRestrRebinResultPtr >= 0 ) {
                  if ( fitEtaPtTransRestrRebinResultPtr->Status() == 0 && fitEtaPtTransRestrRebinResultPtr->Prob() > 0. && fitEtaPtTransRestrRebinResultPtr->Ndf() != 0. ) {
                    if ( checkParametersFit( objCat, fitEtaPtTransRestrRebin, fitFuncId_ ) ) {
                      fillMixedParametersFit( uPt + 1, histVecTransRestrRebinEtaPtFitMap, fitEtaPtTransRestrRebinResultPtr );
                    }
                    else {
                      for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
                        histVecTransRestrRebinEtaPtFitMap.at( uPar )->SetBinContent( uPt + 1, fitEtaPtTransRestrRebinResultPtr->Parameter( uPar ) );
                        histVecTransRestrRebinEtaPtFitMap.at( uPar )->SetBinError( uPt + 1, fitEtaPtTransRestrRebinResultPtr->ParError( uPar ) );
                      }
                    }
                    histTransRestrRebinEtaPtFitMapProb->SetBinContent( uPt + 1, log10( fitEtaPtTransRestrRebinResultPtr->Prob() ) );
                  }
                  else {
                    histTransRestrRebinEtaPtFitMapProb->SetBinContent( uPt + 1, 1 );
                    if ( verbose_ > 2 ) {
                      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                                << "    failing fit in directory '"; dirOutEta_->pwd();
                      std::cout << "    '" << nameEtaPtTransRestrRebin << "' status " << fitEtaPtTransRestrRebinResultPtr->Status() << std::endl;
                    }
                  }
                }
                else {
                  if ( verbose_ > 1 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    missing fit in directory '"; dirOutEta_->pwd();
                    std::cout << "    '" << nameEtaPtTransRestrRebin << std::endl;
                  }
                }
                if ( plot_ ) {
                  histEtaPtTransRestrRebin->Draw();
                  c1.Print( std::string( pathPlots_ + histEtaPtTransRestrRebin->GetName() + ".png" ).c_str() );
                }
              }

            } // loop: uPt < nPtBins_

            if ( plot_ ) {
              for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
                if ( fitNonRestr_ ) {
                  histVecTransRebinEtaPtFitMap.at( uPar )->Draw();
                  c1.Print( std::string( pathPlots_ + histVecTransRebinEtaPtFitMap.at( uPar )->GetName() + ".png" ).c_str() );
                }
                histVecTransRestrRebinEtaPtFitMap.at( uPar )->Draw();
                c1.Print( std::string( pathPlots_ + histVecTransRestrRebinEtaPtFitMap.at( uPar )->GetName() + ".png" ).c_str() );
              }
              if ( fitNonRestr_ ) {
                histTransRebinEtaPtFitMapProb->Draw();
                c1.Print( std::string( pathPlots_ + histTransRebinEtaPtFitMapProb->GetName() + ".png" ).c_str() );
              }
              histTransRestrRebinEtaPtFitMapProb->Draw();
              c1.Print( std::string( pathPlots_ + histTransRestrRebinEtaPtFitMapProb->GetName() + ".png" ).c_str() );
            }

            for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
              const std::string parFit( boost::lexical_cast< std::string >( uPar ) );

              if ( fitNonRestr_ ) {
                const std::string nameTransRebinEtaPtFitMap( nameEta + "_TransRebin" + baseTitlePt + "_FitMap_Par" + parFit );
                const std::string nameTransRebinEtaPtFitMapFit( nameTransRebinEtaPtFitMap + "_fit" );
                if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameTransRebinEtaPtFitMapFit << std::endl;
                TF1 * fitTransRebinEtaPtFitMap( new TF1( nameTransRebinEtaPtFitMapFit.c_str(), dependencyFunction_.c_str(), ptBins_.front(), fitMaxPt_ ) );
                setParametersDependency( objCat, fitTransRebinEtaPtFitMap, histVecTransRebinEtaPtFitMap.at( uPar ), fitFuncId_, depFuncId_, uPar );
                TFitResultPtr fitTransRebinEtaPtFitMapResultPtr( histVecTransRebinEtaPtFitMap.at( uPar )->Fit( fitTransRebinEtaPtFitMap, fitOptions_.c_str() ) );
                if ( fitTransRebinEtaPtFitMapResultPtr >= 0 ) {
                  if ( fitTransRebinEtaPtFitMapResultPtr->Status() == 0 && fitTransRebinEtaPtFitMapResultPtr->Prob() > 0. && fitTransRebinEtaPtFitMapResultPtr->Ndf() != 0. ) {
                    for ( unsigned uDep = 0; uDep < nDep; ++uDep ) {
                      transferVecEtaPt.at( uEta ).SetParameter( uPar, uDep, fitTransRebinEtaPtFitMapResultPtr->Parameter( uDep ) );
                      histVecVecTransRebinEtaParMap.at( uDep ).at( uPar )->SetBinContent( uEta + 1, fitTransRebinEtaPtFitMapResultPtr->Parameter( uDep ) );
                      histVecVecTransRebinEtaParMap.at( uDep ).at( uPar )->SetBinError( uEta + 1, fitTransRebinEtaPtFitMapResultPtr->ParError( uDep ) );
                    }
                  }
                  else {
                    if ( verbose_ > 2 ) {
                      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                                << "    failing fit in directory '"; dirOutEta_->pwd();
                      std::cout << "    '" << nameTransRebinEtaPtFitMap << "' status " << fitTransRebinEtaPtFitMapResultPtr->Status() << std::endl;
                    }
                  }
                }
                else {
                  if ( verbose_ > 1 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    missing fit in directory '"; dirOutEta_->pwd();
                    std::cout << "    '" << nameTransRebinEtaPtFitMap << std::endl;
                  }
                }
              }

              const std::string nameTransRestrRebinEtaPtFitMap( nameEta + "_TransRestrRebin" + baseTitlePt + "_FitMap_Par" + parFit );
              const std::string nameTransRestrRebinEtaPtFitMapFit( nameTransRestrRebinEtaPtFitMap + "_fit" );
              if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameTransRestrRebinEtaPtFitMapFit << std::endl;
              TF1 * fitTransRestrRebinEtaPtFitMap( new TF1( nameTransRestrRebinEtaPtFitMapFit.c_str(), dependencyFunction_.c_str(), ptBins_.front(), fitMaxPt_ ) );
              setParametersDependency( objCat, fitTransRestrRebinEtaPtFitMap, histVecTransRestrRebinEtaPtFitMap.at( uPar ), fitFuncId_, depFuncId_, uPar );
              TFitResultPtr fitTransRestrRebinEtaPtFitMapResultPtr( histVecTransRestrRebinEtaPtFitMap.at( uPar )->Fit( fitTransRestrRebinEtaPtFitMap, fitOptions_.c_str() ) );
              if ( fitTransRestrRebinEtaPtFitMapResultPtr >= 0 ) {
                if ( fitTransRestrRebinEtaPtFitMapResultPtr->Status() == 0 && fitTransRestrRebinEtaPtFitMapResultPtr->Prob() > 0. && fitTransRestrRebinEtaPtFitMapResultPtr->Ndf() != 0. ) {
                  for ( unsigned uDep = 0; uDep < nDep; ++uDep ) {
                    transferVecEtaPtRestr.at( uEta ).SetParameter( uPar, uDep, fitTransRestrRebinEtaPtFitMapResultPtr->Parameter( uDep ) );
                    histVecVecTransRestrRebinEtaParMap.at( uDep ).at( uPar )->SetBinContent( uEta + 1, fitTransRestrRebinEtaPtFitMapResultPtr->Parameter( uDep ) );
                    histVecVecTransRestrRebinEtaParMap.at( uDep ).at( uPar )->SetBinError( uEta + 1, fitTransRestrRebinEtaPtFitMapResultPtr->ParError( uDep ) );
                  }
                }
                else {
                  if ( verbose_ > 2 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    failing fit in directory '"; dirOutEta_->pwd();
                    std::cout << "    '" << nameTransRestrRebinEtaPtFitMap << "' status " << fitTransRestrRebinEtaPtFitMapResultPtr->Status() << std::endl;
                  }
                }
              }
              else {
                if ( verbose_ > 1 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    missing fit in directory '"; dirOutEta_->pwd();
                  std::cout << "    '" << nameTransRestrRebinEtaPtFitMap << std::endl;
                }
              }
            } // loop: uPar < nPar

          }

          dirOutEta_->cd();
          const std::string nameEtaTransferFunctionPt( nameEta + "_TransferFunction" );
          if ( fitNonRestr_ ) {
            TF2 * transferFunctionEtaPt( new TF2( ( ( TF2& )( *( transferVecEtaPt.at( uEta ).Function( norm_ ).Clone( nameEtaTransferFunctionPt.c_str() ) ) ) ) ) );
            transferFunctionEtaPt->SetRange( -2. * widthFactor_ * histMax_, 0., 2. * widthFactor_ * histMax_, 2. * ptBins_.back() );
            transferFunctionEtaPt->SetNpx();
            transferFunctionEtaPt->SetNpy();
            transferFunctionEtaPt->GetXaxis()->SetTitle( titleTrans.c_str() );
            transferFunctionEtaPt->GetYaxis()->SetTitle( titlePt.c_str() );
            if ( overwrite_ ) transferFunctionEtaPt->Write( 0, TObject::kOverwrite );
            else              transferFunctionEtaPt->Write();
            if ( plot_ ) {
              transferFunctionEtaPt->Draw( "Surf3Z" );
              c1.Print( std::string( pathPlots_ + transferFunctionEtaPt->GetName() + ".png" ).c_str() );
            }
          }
          const std::string nameEtaTransferFunctionPtRestr( nameEtaTransferFunctionPt + "Restr" );
          TF2 * transferFunctionEtaPtRestr( new TF2( ( ( TF2& )( *( transferVecEtaPtRestr.at( uEta ).Function( norm_ ).Clone( nameEtaTransferFunctionPtRestr.c_str() ) ) ) ) ) );
          transferFunctionEtaPtRestr->SetRange( -2. * widthFactor_ * histMax_, 0., 2. * widthFactor_ * histMax_, 2. * ptBins_.back() );
          transferFunctionEtaPtRestr->SetNpx();
          transferFunctionEtaPtRestr->SetNpy();
          transferFunctionEtaPtRestr->GetXaxis()->SetTitle( titleTrans.c_str() );
          transferFunctionEtaPtRestr->GetYaxis()->SetTitle( titlePt.c_str() );
          if ( overwrite_ ) transferFunctionEtaPtRestr->Write( 0, TObject::kOverwrite );
          else              transferFunctionEtaPtRestr->Write();
          if ( plot_ ) {
            transferFunctionEtaPtRestr->Draw( "Surf3Z" );
            c1.Print( std::string( pathPlots_ + transferFunctionEtaPtRestr->GetName() + ".png" ).c_str() );
          }

        } // loop: keyEta

        if ( plot_ ) {
          if ( fitEtaBins_ ) {
            for ( unsigned uDep = 0; uDep < nDep; ++uDep ) {
              for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
                if ( fitNonRestr_ ) {
                  histVecVecTransRebinEtaParMap.at( uDep ).at( uPar )->Draw();
                  c1.Print( std::string( pathPlots_ + histVecVecTransRebinEtaParMap.at( uDep ).at( uPar )->GetName() + ".png" ).c_str() );
                }
                histVecVecTransRestrRebinEtaParMap.at( uDep ).at( uPar )->Draw();
                c1.Print( std::string( pathPlots_ + histVecVecTransRestrRebinEtaParMap.at( uDep ).at( uPar )->GetName() + ".png" ).c_str() );
              }
            }
          }
          for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
            if ( fitNonRestr_ ) {
              histVecTransRebinEtaFitMap.at( uPar )->Draw();
              c1.Print( std::string( pathPlots_ + histVecTransRebinEtaFitMap.at( uPar )->GetName() + ".png" ).c_str() );
            }
            histVecTransRestrRebinEtaFitMap.at( uPar )->Draw();
            c1.Print( std::string( pathPlots_ + histVecTransRestrRebinEtaFitMap.at( uPar )->GetName() + ".png" ).c_str() );
          }
          if ( fitNonRestr_ ) {
            histTransRebinEtaFitMapProb->Draw();
            c1.Print( std::string( pathPlots_ + histTransRebinEtaFitMapProb->GetName() + ".png" ).c_str() );
          }
          histTransRestrRebinEtaFitMapProb->Draw();
          c1.Print( std::string( pathPlots_ + histTransRestrRebinEtaFitMapProb->GetName() + ".png" ).c_str() );
        }

        if ( writeFiles_ ) {

          // File name
          std::string nameOut( pathOut_ + "/gentTransferFunction_" + sample_ );
          if ( usePileUp_ ) nameOut.append( "_PileUp" );
          if ( refSel_)     nameOut.append( "_Ref" );
          nameOut.append( "_" + name + ".txt" );

          ofstream fileOut;
          fileOut.open( nameOut.c_str(), std::ios_base::out );

          if ( fitNonRestr_ ) {
            fileOut << transferPt.Print() << std::endl << std::endl;
          }
          fileOut << transferPtRestr.Print() << std::endl;

          if ( fitEtaBins_ ) {
            fileOut << "================================================================================" << std::endl << std::endl;
            for ( unsigned uEta = 0; uEta < nEtaBins_; ++uEta ) {
              if ( uEta > 0 ) fileOut << "--------------------------------------------------------------------------------" << std::endl << std::endl;
              fileOut << "for " << etaBins_.at( uEta ) << " <= eta < " << etaBins_.at( uEta + 1 ) << std::endl << std::endl;

              if ( fitNonRestr_ ) {
                fileOut << transferVecEtaPt.at( uEta ).Print() << std::endl;
              }
              fileOut << transferVecEtaPtRestr.at( uEta ).Print() << std::endl;

            }
          }

          fileOut.close();
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    written transfer function file:" << std::endl
                    << "        " << nameOut << std::endl;

        }

      } // loop: keyFit

    }

  } // loop: uCat < objCats_.size()


  // Write and close ROOT files
  Int_t writeOut_( 0 );
  if ( overwrite_ ) {
    writeOut_ = fileOut_->Write( 0, TObject::kOverwrite );
  }
  else {
    writeOut_ = fileOut_->Write();
  }
  fileIn_->Close();
  fileOut_->Close();

  if ( verbose_ > 0 )
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "    return status   " << returnStatus_ << std::endl
              << "    " << writeOut_ << " bytes written to output file" << std::endl;
  return returnStatus_;

}


// Initialise parameters for fit function
void setParametersFit( std::string objCat, TF1 * fit, TH1D * histo, std::string fitFuncId, bool scale )
{
  //. This function assumes fit functions of the forms
  // - [0]*exp(-0.5*((x-[1])/[2])**2)/([2]*sqrt(2*pi)) (single Gaissian) or
  // - [0]*(exp(-0.5*((x-[1])/[2])**2)+[3]*exp(-0.5*((x-[4])/[5])**2))/(([2]+[3]*[5])*sqrt(2*pi)) (double Gaussian)

  // Starting points
  const Double_t c( scale ? 1. : histo->GetSumOfWeights() );         // Constant
  const Double_t m( histo->GetMean() );                              // Mean
  const Double_t p( histo->GetBinCenter( histo->GetMaximumBin() ) ); // Peak
  const Double_t s( histo->GetRMS() );                               // RMS
  Double_t fitMin, fitMax;
  fit->GetRange( fitMin, fitMax );                                   // function's fit limits

  // Single Gaussian case
  if ( fitFuncId == "sGauss" ) {
    fit->SetParameter( 0, c );
    fit->SetParName( 0, "c" );
    fit->SetParameter( 1, p );
    fit->SetParName( 1, "#mu" );
    fit->SetParameter( 2, sqrt( s - m ) );
    fit->SetParName( 2, "#sigma" );
  }

  // Double Gaussian case
  if ( fitFuncId == "dGauss" ) {
    fit->SetParName( 0, "Signal c" );
    fit->SetParName( 1, "Signal #mu" );
    fit->SetParName( 2, "Signal #sigma" );
    fit->SetParName( 3, "Bkg c" );
    fit->SetParName( 4, "Bkg #mu" );
    fit->SetParName( 5, "Bkg #sigma" );
    if ( objCat == "UdscJet" ) {
      fit->SetParameter( 0, 10. * c );
      fit->SetParameter( 1, p );
      fit->SetParLimits( 1, fitMin, fitMax );
      fit->SetParameter( 2, sqrt( s - m ) );
      fit->SetParLimits( 2, 0., 2. * s );
      fit->SetParameter( 3, 0.25 );
      fit->SetParLimits( 3, 0., 100. );
      fit->SetParameter( 4, 2 * m - p );
      fit->SetParLimits( 4, fitMin, fitMax );
      fit->SetParameter( 5, ( fitMax - fitMin ) / 4. );
      fit->SetParLimits( 5, 0., fitMax - fitMin );
    }
    else if ( objCat == "BJet" ) {
      fit->SetParameter( 0, 10. * c );
      fit->SetParameter( 1, p );
      fit->SetParLimits( 1, fitMin, fitMax );
      fit->SetParameter( 2, sqrt( s - m ) );
      fit->SetParameter( 3, 0.25 );
      fit->SetParLimits( 3, 0., 100. );
      fit->SetParameter( 4, 2 * m - p );
      fit->SetParLimits( 4, fitMin, fitMax );
      fit->SetParameter( 5, ( fitMax - fitMin ) / 4. );
      fit->SetParLimits( 5, 0., fitMax - fitMin );
    }
    else if ( objCat == "Elec" ) {
      fit->SetParameter( 0, 10. * c );
//       fit->SetParLimits( 0, 0., 10. * c * ( s - m ) );
      fit->SetParameter( 1, p );
      fit->SetParameter( 2, sqrt( s - m ) );
//       fit->SetParLimits( 2, 0., fitMax - fitMin );
      fit->SetParameter( 3, 0.25 );
      fit->SetParLimits( 3, 0., 100. );
      fit->SetParameter( 4, 2 * m - p );
      fit->SetParLimits( 4, fitMin, fitMax );
      fit->SetParameter( 5, ( fitMax - fitMin ) / 4. );
      fit->SetParLimits( 5, 0., fitMax - fitMin );
    }
    else if ( objCat == "Mu" ) {
      fit->SetParameter( 0, 10. * c );
//       fit->SetParLimits( 0, 0., 10. * c * ( s - m ) );
      fit->SetParameter( 1, p );
      fit->SetParameter( 2, sqrt( s - m ) );
//       fit->SetParLimits( 2, 0., fitMax - fitMin );
      fit->SetParameter( 3, 0.25 );
      fit->SetParLimits( 3, 0., 100. );
      fit->SetParameter( 4, 2 * m - p );
      fit->SetParLimits( 4, fitMin, fitMax );
      fit->SetParameter( 5, ( fitMax - fitMin ) / 4. );
      fit->SetParLimits( 5, 0., fitMax - fitMin );
    }
  }

}


// Check correct assignment of signal and background to the fit function parameters
bool checkParametersFit( std::string objCat, TF1 * fit, std::string fitFuncId )
{
  //. This function assumes fit functions of the forms
  // - [0]*exp(-0.5*((x-[1])/[2])**2)/([2]*sqrt(2*pi)) (single Gaissian) or
  // - [0]*(exp(-0.5*((x-[1])/[2])**2)+[3]*exp(-0.5*((x-[4])/[5])**2))/(([2]+[3]*[5])*sqrt(2*pi)) (double Gaussian)

  // Check, if background is described at all in function
  if ( fitFuncId == "sGauss" ) return false;
//   if ( fit->GetParameter( 2 ) < fit->GetParameter( 5 ) || std::fabs( fit->GetParameter( 3 ) ) < 1. ) return false; // test 1,5; default
//   if ( std::fabs( fit->GetParameter( 3 ) ) * std::fabs( fit->GetParameter( 5 ) ) < std::fabs( fit->GetParameter( 2 ) ) ) return false; // test 2,6
//   if ( std::fabs( fit->GetParameter( 3 ) ) < 1. && std::fabs( fit->GetParameter( 3 ) ) * std::fabs( fit->GetParameter( 5 ) ) < std::fabs( fit->GetParameter( 2 ) ) ) return false; // test 3,7
  if ( fit->GetParameter( 2 ) < fit->GetParameter( 5 ) && std::fabs( fit->GetParameter( 3 ) ) < 1. ) return false; // test 4,8
// //   if ( fit->GetParameter( 2 ) < fit->GetParameter( 5 ) && std::fabs( fit->GetParameter( 3 ) ) * std::fabs( fit->GetParameter( 5 ) ) < std::fabs( fit->GetParameter( 2 ) ) ) return false;
  std::cout << std::endl
            << " --> INFO:" << std::endl
            << "    function " << fit->GetName() << " mixed parameters." << std::endl;
  return true; // incorrect assignment assumed!
}


// Interchange signal and background parameters of fit function
void mixParametersFit( my::TransferFunction & transfer, TF1 * fit )
{
  //. This function assumes fit functions of the form
  // - [0]*(exp(-0.5*((x-[1])/[2])**2) + [3]*exp(-0.5*((x-[4])/[5])**2))/(([2]+[3]*[5])*sqrt(2*pi)) (double Gaussian)

  transfer.SetParameter( 1, fit->GetParameter( 4 ) );
  transfer.SetParameter( 2, fit->GetParameter( 5 ) );
  transfer.SetParameter( 3, 1. / fit->GetParameter( 3 ) );
  transfer.SetParameter( 4, fit->GetParameter( 1 ) );
  transfer.SetParameter( 5, fit->GetParameter( 2 ) );
}


// Interchange signal and background parameters of fit function in histogram filling
void fillMixedParametersFit( int bin, std::vector< TH1D * > & histoVec, TFitResultPtr & fitResultPtr )
{
  //. This function assumes fit functions of the form
  // - [0]*(exp(-0.5*((x-[1])/[2])**2) + [3]*exp(-0.5*((x-[4])/[5])**2))/(([2]+[3]*[5])*sqrt(2*pi)) (double Gaussian)

  histoVec.at( 0 )->SetBinContent( bin, fitResultPtr->Parameter( 0 ) );
  histoVec.at( 0 )->SetBinError( bin, fitResultPtr->ParError( 0 ) );
  histoVec.at( 1 )->SetBinContent( bin, fitResultPtr->Parameter( 4 ) );
  histoVec.at( 1 )->SetBinError( bin, fitResultPtr->ParError( 4 ) );
  histoVec.at( 2 )->SetBinContent( bin, fitResultPtr->Parameter( 5 ) );
  histoVec.at( 2 )->SetBinError( bin, fitResultPtr->ParError( 5 ) );
  histoVec.at( 3 )->SetBinContent( bin, 1. / fitResultPtr->Parameter( 3 ) );
  histoVec.at( 3 )->SetBinError( bin, fitResultPtr->ParError( 3 ) / ( fitResultPtr->Parameter( 3 ) * fitResultPtr->Parameter( 3 ) ) );
  histoVec.at( 4 )->SetBinContent( bin, fitResultPtr->Parameter( 1 ) );
  histoVec.at( 4 )->SetBinError( bin, fitResultPtr->ParError( 1 ) );
  histoVec.at( 5 )->SetBinContent( bin, fitResultPtr->Parameter( 2 ) );
  histoVec.at( 5 )->SetBinError( bin, fitResultPtr->ParError( 2 ) );
}


// Initialise parametrs for dependency function
void setParametersDependency( std::string objCat, TF1 * dep, TH1D * histo, std::string fitFuncId, std::string depFuncId, unsigned par )
{
  //. This function assumes fit functions of the forms
  // - [0]*exp(-0.5*((x-[1])/[2])**2)/([2]*sqrt(2*pi)) (single Gaissian) or
  // - [0]*(exp(-0.5*((x-[1])/[2])**2)+[3]*exp(-0.5*((x-[4])/[5])**2))/(([2]+[3]*[5])*sqrt(2*pi)) (double Gaussian)
  //. This function assumes dependency functions of the form
  // - [0]+[1]*x (linear)
  // - [0]+[1]*x+[2]*x**2 (squared)

  // Starting points
  Double_t x1( histo->GetBinCenter( 5 ) );
  Double_t y1( histo->GetBinContent( 5 ) );
  Double_t x2( histo->GetBinCenter( histo->GetNbinsX() - 2 ) );
  Double_t y2( histo->GetBinContent( histo->GetNbinsX() - 2 ) );
  if ( depFuncId == "linear" || depFuncId == "squared" ) {
    // Constant
    dep->SetParameter( 0, ( x2 * y1 - x1 * y2 ) / ( x2 - x1 ) );
    dep->SetParName( 0, "Constant a" );
    // Slope
    dep->SetParameter( 1, ( y2 - y1 ) / ( x2 - x1 ) );
    dep->SetParName( 1, "Slope b" );
  }
    // Curvature
  if ( depFuncId == "squared" ) {
    dep->SetParameter( 2, 0. );
    dep->SetParName( 2, "Curvature c" );
  }
  if ( depFuncId == "resolution" ) {
    dep->SetParameter( 0, 0. );
    dep->SetParName( 0, "Noise N" );
    dep->SetParameter( 1, 0. );
    dep->SetParName( 1, "Resolution R" );
    dep->SetParameter( 2, 0. );
    dep->SetParName( 2, "Constant C" );
  }

  if ( depFuncId == "linear" ) {
    if ( objCat == "UdscJet" ) {
    }
    else if ( objCat == "BJet" ) {
    }
    else if ( objCat == "Elec" ) {
      // Constant
      if ( fitFuncId == "sGauss" || fitFuncId == "dGauss" ) {
        if ( par % 3 == 0 || par % 3 == 2 ) {
          if ( dep->GetParameter( 0 ) < 0. ) dep->SetParameter( 0, 0.1 );
          dep->SetParLimits( 0, 0., 100. );
        }
      }
      // Slope
      if ( fitFuncId == "sGauss" || fitFuncId == "dGauss" ) {
        if ( par % 3 == 0 || par % 3 == 2 ) {
          if ( dep->GetParameter( 1 ) < 0. ) dep->SetParameter( 1, 0.1 );
          dep->SetParLimits( 1, 0., 100. );
        }
      }
    }
    else if ( objCat == "Mu" ) {
      // Constant
      if ( fitFuncId == "sGauss" || fitFuncId == "dGauss" ) {
        if ( par % 3 == 0 || par % 3 == 2 ) {
          if ( dep->GetParameter( 0 ) < 0. ) dep->SetParameter( 0, 0.1 );
          dep->SetParLimits( 0, 0., 100. );
        }
      }
      // Slope
      if ( fitFuncId == "sGauss" || fitFuncId == "dGauss" ) {
        if ( par % 3 == 0 || par % 3 == 2 ) {
          if ( dep->GetParameter( 1 ) < 0. ) dep->SetParameter( 1, 0.1 );
          dep->SetParLimits( 1, 0., 100. );
        }
      }
    }
  }

  else if ( depFuncId == "squared" ) {
    if ( objCat == "UdscJet" ) {
    }
    else if ( objCat == "BJet" ) {
    }
    else if ( objCat == "Elec" ) {
      // Constant
      if ( fitFuncId == "sGauss" || fitFuncId == "dGauss" ) {
        if ( par % 3 == 0 || par % 3 == 2 ) {
          if ( dep->GetParameter( 0 ) < 0. ) dep->SetParameter( 0, 0.1 );
          dep->SetParLimits( 0, 0., 100. );
        }
      }
      // Slope
      if ( fitFuncId == "sGauss" || fitFuncId == "dGauss" ) {
        if ( par % 3 == 0 || par % 3 == 2 ) {
          if ( dep->GetParameter( 1 ) < 0. ) dep->SetParameter( 1, 0.1 );
          dep->SetParLimits( 1, 0., 100. );
        }
      }
      // Curvature
      if ( fitFuncId == "sGauss" || fitFuncId == "dGauss" ) {
        if ( par % 3 == 1 ) {
//           dep->FixParameter( 2, 0. );
        }
      }
    }
    else if ( objCat == "Mu" ) {
      // Constant
      if ( fitFuncId == "sGauss" || fitFuncId == "dGauss" ) {
        if ( par % 3 == 0 || par % 3 == 2 ) {
          if ( dep->GetParameter( 0 ) < 0. ) dep->SetParameter( 0, 0.1 );
          dep->SetParLimits( 0, 0., 100. );
        }
      }
      // Slope
      if ( fitFuncId == "sGauss" || fitFuncId == "dGauss" ) {
        if ( par % 3 == 0 || par % 3 == 2 ) {
          if ( dep->GetParameter( 1 ) < 0. ) dep->SetParameter( 1, 0.1 );
          dep->SetParLimits( 1, 0., 100. );
        }
      }
      // Curvature
      if ( fitFuncId == "sGauss" || fitFuncId == "dGauss" ) {
        if ( par % 3 == 1 ) {
//           dep->FixParameter( 2, 0. );
        }
      }
    }
  }

  else if ( depFuncId == "resolution" ) {
    if ( objCat == "UdscJet" ) {
    }
    else if ( objCat == "BJet" ) {
    }
    else if ( objCat == "Elec" ) {
      // Constant
      if ( fitFuncId == "sGauss" || fitFuncId == "dGauss" ) {
        if ( par % 3 == 0 || par % 3 == 2 ) {
        }
      }
      // Slope
      if ( fitFuncId == "sGauss" || fitFuncId == "dGauss" ) {
        if ( par % 3 == 0 || par % 3 == 2 ) {
        }
      }
      // Curvature
      if ( fitFuncId == "sGauss" || fitFuncId == "dGauss" ) {
        if ( par % 3 == 1 ) {
//           dep->FixParameter( 2, 0. );
        }
      }
    }
    else if ( objCat == "Mu" ) {
      // Noise
      if ( fitFuncId == "sGauss" || fitFuncId == "dGauss" ) {
        if ( par % 3 == 0 || par % 3 == 2 ) {
        }
      }
      // Resolution
      if ( fitFuncId == "sGauss" || fitFuncId == "dGauss" ) {
        if ( par % 3 == 0 || par % 3 == 2 ) {
        }
      }
      // Constant
      if ( fitFuncId == "sGauss" || fitFuncId == "dGauss" ) {
        if ( par % 3 == 1 ) {
//           dep->FixParameter( 2, 0. );
        }
      }
    }
  }

}
