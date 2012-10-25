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
void setParametersFit( TF1 * fit, TH1D * histo );


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
  const unsigned histBins_( histos_.getParameter< unsigned >( "PullPtBins" ) );
  const double histMax_( histos_.getParameter< double >( "PullPtMax" ) );
  // Configuration for fitting transfer functions
  const edm::ParameterSet & fit_( process_.getParameter< edm::ParameterSet >( "fit" ) );
  const bool fitNonRestr_( fit_.getParameter< bool >( "fitNonRestr" ) );
  const bool fitEtaBins_( fit_.getParameter< bool >( "fitEtaBins" ) );
  // Configuration for transfer functions; need to be identical to those used for the determination
  const edm::ParameterSet & transfer_( process_.getParameter< edm::ParameterSet >( "transfer" ) );
  double fitMaxPt_( transfer_.getParameter< double >( "fitMaxPt" ) );
  const std::string fitFunction_( transfer_.getParameter< std::string >( "fitFunction" ) );
  const int norm_( transfer_.getParameter< int >( "norm" ) );
  const std::string dependencyFunction_( transfer_.getParameter< std::string >( "dependencyFunction" ) );
  const double minPt_( transfer_.getParameter< double >( "minPt" ) );
  const double maxDR_( transfer_.getParameter< double >( "maxDR" ) );
  // Configuration for pull distributions
  const edm::ParameterSet & transferPull_( process_.getParameter< edm::ParameterSet >( "transferPull" ) );
  const bool doFit_( transferPull_.getParameter< bool >( "doFit" ) );
  std::string fitOptions_( transferPull_.getParameter< std::string >( "fitOptions" ) );
  const double fitRange_( std::min( transferPull_.getParameter< double >( "fitRange" ), histMax_ ) );

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
  if      ( verbose_ < 3 ) fitOptions_.append( "Q" );
  else if ( verbose_ > 4 ) fitOptions_.append( "V" );
  const std::string baseTitlePtL( useAlt_ ? "E" : "p" );
  const std::string baseTitlePtU( useAlt_ ? "E" : "P" );
  const std::string baseTitlePt( useNonT_ ? baseTitlePtU : baseTitlePtU + "t" );
  const std::string titlePtT( useNonT_ ? baseTitlePtL : baseTitlePtL + "_{t}" );
  std::string titlePtRef( refGen_ ? titlePtT + "^{GEN}" : titlePtT );
  std::string titlePtNonRef( refGen_ ? titlePtT : titlePtT + "^{GEN}" );
  const std::string baseTitleEta( useSymm_ ? "|#eta|" : "#eta" );
  const std::string titleEta( refGen_ ? baseTitleEta + "^{GEN}" : baseTitleEta + "^{RECO}" );
  const std::string titleTransPull( "#frac{#Delta" + titlePtRef + "_{smeared} - " + titlePtNonRef + "}{#sigma}" );
  const std::string titleTransPullMean( "#mu of " + titleTransPull );
  const std::string titleTransPullSigma( "#sigma of " + titleTransPull );
  titlePtRef    += " (GeV)";
  titlePtNonRef += " (GeV)";
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
      dirFit_->cd();

      std::string name( objCat + "_" + baseTitlePt + "_" + subFit );

      const std::string part( refGen_ ? "_parton" : "_reco" );
      my::TransferFunction transferBase( fitFunction_, dependencyFunction_, std::string( titlePtT + part ) );
      const unsigned nPar( transferBase.NParFit() );
      const unsigned nDep( transferBase.NParDependency() );

      my::TransferFunction transfer( transferBase );
      const std::string nameTransRebin( name + "_TransRebin" );
      TH1D * histTransRebin( ( TH1D* )( dirFit_->Get( nameTransRebin.c_str() ) ) );
      TF1 * fitTransRebin( 0 );
      if ( fitNonRestr_ ) {
        const std::string nameTransRebinFit( nameTransRebin + "_fit" );
        if ( histTransRebin != 0 ) {
          fitTransRebin = histTransRebin->GetFunction( nameTransRebinFit.c_str() );
          if ( fitTransRebin != 0 ) {
            for ( unsigned iPar = 0; iPar < nPar; ++iPar ) {
              transfer.SetParameter( iPar, fitTransRebin->GetParameter( iPar ) );
            }
          }
          else {
            std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                      << "    fit function '" << nameTransRebinFit << "' does not exist in " << nameTransRebin << std::endl;
            returnStatus_ += 0x200000;
          }
        }
        else {
          std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                    << "    histogram '" << nameTransRebin << "' does not exist in input file" << std::endl;
          returnStatus_ += 0x100000;
        }
      }
      const std::string nameTransferFunction( name + "_TransferFunction" );
      TF2 * transferFunction( ( TF2* )( dirFit_->Get( nameTransferFunction.c_str() ) ) );
      if ( fitNonRestr_ ) {
        if ( transferFunction != 0 ) {
          unsigned cPar( 0 );
          for ( unsigned iPar = 0; iPar < nPar; ++iPar ) {
            for ( unsigned iDep = 0; iDep < nDep; ++iDep ) {
              if ( norm_ == ( int ) iPar ) {
                transfer.SetParameter( iPar, iDep, my::transferFunctionInitConst );
              }
              else {
                transfer.SetParameter( iPar, iDep, transferFunction->GetParameter( ( int )cPar ) );
                ++cPar;
              }
            }
          }
        }
        else {
          std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                    << "    transfer function '" << nameTransferFunction << "' does not exist in input file" << std::endl;
          returnStatus_ += 0x300000;
        }
      }

      my::TransferFunction transferRestr( transferBase );
      const std::string nameTransRestrRebin( name + "_TransRestrRebin" );
      TH1D * histTransRestrRebin( ( TH1D* )( dirFit_->Get( nameTransRestrRebin.c_str() ) ) );
      TF1 * fitTransRestrRebin( 0 );
      const std::string nameTransRestrRebinFit( nameTransRestrRebin + "_fit" );
      if ( histTransRestrRebin != 0 ) {
        fitTransRestrRebin = histTransRestrRebin->GetFunction( nameTransRestrRebinFit.c_str() );
        if ( fitTransRestrRebin != 0 ) {
          for ( unsigned iPar = 0; iPar < nPar; ++iPar ) {
            transferRestr.SetParameter( iPar, fitTransRestrRebin->GetParameter( iPar ) );
          }
        }
        else {
          std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                    << "    fit function '" << nameTransRestrRebinFit << "' does not exist in " << nameTransRestrRebin << std::endl;
          returnStatus_ += 0x200000;
        }
      }
      else {
        std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                  << "    histogram '" << nameTransRestrRebin << "' does not exist in input file" << std::endl;
        returnStatus_ += 0x100000;
      }
      const std::string nameTransferFunctionRestr( nameTransferFunction + "Restr" );
      TF2 * transferFunctionRestr( ( TF2* )( dirFit_->Get( nameTransferFunctionRestr.c_str() ) ) );
      if ( transferFunctionRestr != 0 ) {
        unsigned cPar( 0 );
        for ( unsigned iPar = 0; iPar < nPar; ++iPar ) {
          for ( unsigned iDep = 0; iDep < nDep; ++iDep ) {
            if ( norm_ == ( int ) iPar ) {
              transferRestr.SetParameter( iPar, iDep, my::transferFunctionInitConst );
            }
            else {
              transferRestr.SetParameter( iPar, iDep, transferFunctionRestr->GetParameter( ( int )cPar ) );
              ++cPar;
            }
          }
        }
      }
      else {
        std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                  << "    transfer function '" << nameTransferFunctionRestr << "' does not exist in input file" << std::endl;
        returnStatus_ += 0x300000;
      }

      const std::string nameTransPull( name + "_TransPull" );
      TH1D * histTransPull( new TH1D( nameTransPull.c_str(), objCat.c_str(), histBins_, -histMax_, histMax_ ) );
      histTransPull->SetXTitle( titleTransPull.c_str() );
      histTransPull->SetYTitle( titleEvents.c_str() );
      const std::string nameTransPullPt( nameTransPull + "_Pt" );
      TH1D * histTransPullPt( new TH1D( nameTransPullPt.c_str(), objCat.c_str(), histBins_, -histMax_, histMax_ ) );
      histTransPullPt->SetXTitle( titleTransPull.c_str() );
      histTransPullPt->SetYTitle( titleEvents.c_str() );
      const std::string nameTransPullEtaPt( nameTransPull + "_EtaPt" );
      TH1D * histTransPullEtaPt( new TH1D( nameTransPullEtaPt.c_str(), objCat.c_str(), histBins_, -histMax_, histMax_ ) );
      histTransPullEtaPt->SetXTitle( titleTransPull.c_str() );
      histTransPullEtaPt->SetYTitle( titleEvents.c_str() );
      const std::string nameTransPullEta( nameTransPull + "_Eta" );
      TH1D * histTransPullEta( new TH1D( nameTransPullEta.c_str(), objCat.c_str(), histBins_, -histMax_, histMax_ ) );
      histTransPullEta->SetXTitle( titleTransPull.c_str() );
      histTransPullEta->SetYTitle( titleEvents.c_str() );
      const std::string nameTransPullMapPt( nameTransPull + "_Map_Pt" );
      TH2D * histTransPullMapPt( new TH2D( nameTransPullMapPt.c_str(), objCat.c_str(), nPtBins_, ptBins_.data(), histBins_, -histMax_, histMax_ ) );
      histTransPullMapPt->SetXTitle( titlePtRef.c_str() );
      histTransPullMapPt->SetYTitle( titleTransPull.c_str() );
      histTransPullMapPt->SetZTitle( titleEvents.c_str() );
      const std::string nameTransPullMapEta( nameTransPull + "_Map_Eta" );
      TH2D * histTransPullMapEta( new TH2D( nameTransPullMapEta.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data(), histBins_, -histMax_, histMax_ ) );
      histTransPullMapEta->SetXTitle( titleEta.c_str() );
      histTransPullMapEta->SetYTitle( titleTransPull.c_str() );
      histTransPullMapEta->SetZTitle( titleEvents.c_str() );

      const std::string nameTransPullRestr( nameTransPull + "Restr" );
      TH1D * histTransPullRestr( new TH1D( nameTransPullRestr.c_str(), objCat.c_str(), histBins_, -histMax_, histMax_ ) );
      histTransPullRestr->SetXTitle( titleTransPull.c_str() );
      histTransPullRestr->SetYTitle( titleEvents.c_str() );
      const std::string nameTransPullRestrPt( nameTransPullRestr + "_Pt" );
      TH1D * histTransPullRestrPt( new TH1D( nameTransPullRestrPt.c_str(), objCat.c_str(), histBins_, -histMax_, histMax_ ) );
      histTransPullRestrPt->SetXTitle( titleTransPull.c_str() );
      histTransPullRestrPt->SetYTitle( titleEvents.c_str() );
      const std::string nameTransPullRestrEtaPt( nameTransPullRestr + "_EtaPt" );
      TH1D * histTransPullRestrEtaPt( new TH1D( nameTransPullRestrEtaPt.c_str(), objCat.c_str(), histBins_, -histMax_, histMax_ ) );
      histTransPullRestrEtaPt->SetXTitle( titleTransPull.c_str() );
      histTransPullRestrEtaPt->SetYTitle( titleEvents.c_str() );
      const std::string nameTransPullRestrEta( nameTransPullRestr + "_Eta" );
      TH1D * histTransPullRestrEta( new TH1D( nameTransPullRestrEta.c_str(), objCat.c_str(), histBins_, -histMax_, histMax_ ) );
      histTransPullRestrEta->SetXTitle( titleTransPull.c_str() );
      histTransPullRestrEta->SetYTitle( titleEvents.c_str() );
      const std::string nameTransPullRestrMapPt( nameTransPullRestr + "_Map_Pt" );
      TH2D * histTransPullRestrMapPt( new TH2D( nameTransPullRestrMapPt.c_str(), objCat.c_str(), nPtBins_, ptBins_.data(), histBins_, -histMax_, histMax_ ) );
      histTransPullRestrMapPt->SetXTitle( titlePtRef.c_str() );
      histTransPullRestrMapPt->SetYTitle( titleTransPull.c_str() );
      histTransPullRestrMapPt->SetZTitle( titleEvents.c_str() );
      const std::string nameTransPullRestrMapEta( nameTransPullRestr + "_Map_Eta" );
      TH2D * histTransPullRestrMapEta( new TH2D( nameTransPullRestrMapEta.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data(), histBins_, -histMax_, histMax_ ) );
      histTransPullRestrMapEta->SetXTitle( titleEta.c_str() );
      histTransPullRestrMapEta->SetYTitle( titleTransPull.c_str() );
      histTransPullRestrMapEta->SetZTitle( titleEvents.c_str() );

      std::vector< TH1D * > histVecPtTransPull;
      std::vector< TH2D * > histVecPtTransPullMapEta;
      std::vector< TH1D * > histVecPtTransPullRestr;
      std::vector< TH2D * > histVecPtTransPullRestrMapEta;
      for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
        const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
        const std::string namePt( name + "_" + baseTitlePt + binPt );
        const std::string namePtTransPull( namePt + "_TransPull" );
        const std::string titlePtTransPull( objCat + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq " + titlePtT + " < " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
        TH1D * histPtTransPull( new TH1D( namePtTransPull.c_str(), titlePtTransPull.c_str(), histBins_, -histMax_, histMax_ ) );
        histPtTransPull->SetXTitle( titleTransPull.c_str() );
        histPtTransPull->SetYTitle( titleEvents.c_str() );
        histVecPtTransPull.push_back( histPtTransPull );
        const std::string namePtTransPullMapEta( namePtTransPull + "_Map_Eta" );
        TH2D * histPtTransPullMapEta( new TH2D( namePtTransPullMapEta.c_str(), titlePtTransPull.c_str(), nEtaBins_, etaBins_.data(), histBins_, -histMax_, histMax_ ) );
        histPtTransPullMapEta->SetXTitle( titleEta.c_str() );
        histPtTransPullMapEta->SetYTitle( titleTransPull.c_str() );
        histPtTransPullMapEta->SetZTitle( titleEvents.c_str() );
        histVecPtTransPullMapEta.push_back( histPtTransPullMapEta );
        const std::string namePtTransPullRestr( namePtTransPull + "Restr" );
        TH1D * histPtTransPullRestr( new TH1D( namePtTransPullRestr.c_str(), titlePtTransPull.c_str(), histBins_, -histMax_, histMax_ ) );
        histPtTransPullRestr->SetXTitle( titleTransPull.c_str() );
        histPtTransPullRestr->SetYTitle( titleEvents.c_str() );
        histVecPtTransPullRestr.push_back( histPtTransPullRestr );
        const std::string namePtTransPullRestrMapEta( namePtTransPullRestr + "_Map_Eta" );
        TH2D * histPtTransPullRestrMapEta( new TH2D( namePtTransPullRestrMapEta.c_str(), titlePtTransPull.c_str(), nEtaBins_, etaBins_.data(), histBins_, -histMax_, histMax_ ) );
        histPtTransPullRestrMapEta->SetXTitle( titleEta.c_str() );
        histPtTransPullRestrMapEta->SetYTitle( titleTransPull.c_str() );
        histPtTransPullRestrMapEta->SetZTitle( titleEvents.c_str() );
        histVecPtTransPullRestrMapEta.push_back( histPtTransPullRestrMapEta );
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
        TDirectory * dirEta_( ( TDirectory* )( dirFit_->Get( binEta.c_str() ) ) );
        dirEta_->cd();
        if ( verbose_ > 1 ) gDirectory->pwd();

        const std::string nameEta( name + "_" + binEta );

        my::TransferFunction transferEta( transferBase );
        const std::string nameEtaTransRebin( nameEta + "_TransRebin" );
        TH1D * histEtaTransRebin( ( TH1D* )( dirEta_->Get( nameEtaTransRebin.c_str() ) ) );
        TF1 * fitEtaTransRebin( 0 );
        if ( fitNonRestr_ ) {
          const std::string nameEtaTransRebinFit( nameEtaTransRebin + "_fit" );
          if ( histEtaTransRebin != 0 ) {
            fitEtaTransRebin = histEtaTransRebin->GetFunction( nameEtaTransRebinFit.c_str() );
            if ( fitEtaTransRebin != 0 ) {
              for ( unsigned iPar = 0; iPar < nPar; ++iPar ) {
                transferEta.SetParameter( iPar, fitEtaTransRebin->GetParameter( iPar ) );
              }
            }
            else {
              std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                        << "    fit function '" << nameEtaTransRebinFit << "' does not exist in " << nameEtaTransRebin << std::endl;
              returnStatus_ += 0x200000;
            }
          }
          else {
            std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                      << "    histogram '" << nameEtaTransRebin << "' does not exist in input file" << std::endl;
            returnStatus_ += 0x100000;
          }
        }
        const std::string nameEtaTransferFunctionEta( nameEta + "_TransferFunction" );
        TF2 * transferFunctionEta( ( TF2* )( dirEta_->Get( nameEtaTransferFunctionEta.c_str() ) ) );
        if ( fitNonRestr_ ) {
          if ( transferFunctionEta != 0 ) {
            unsigned cPar( 0 );
            for ( unsigned iPar = 0; iPar < nPar; ++iPar ) {
              for ( unsigned iDep = 0; iDep < nDep; ++iDep ) {
                if ( norm_ == ( int ) iPar ) {
                  transferEta.SetParameter( iPar, iDep, my::transferFunctionInitConst );
                }
                else {
                  transferEta.SetParameter( iPar, iDep, transferFunctionEta->GetParameter( ( int )cPar ) );
                  ++cPar;
                }
              }
            }
          }
          else {
            std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                      << "    transfer function '" << nameEtaTransferFunctionEta << "' does not exist in input file" << std::endl;
            returnStatus_ += 0x300000;
          }
        }

        my::TransferFunction transferEtaRestr( transferBase );
        const std::string nameEtaTransRestrRebin( nameEta + "_TransRestrRebin" );
        TH1D * histEtaTransRestrRebin( ( TH1D* )( dirEta_->Get( nameEtaTransRestrRebin.c_str() ) ) );
        TF1 * fitEtaTransRestrRebin( 0 );
        const std::string nameEtaTransRestrRebinFit( nameEtaTransRestrRebin + "_fit" );
        if ( histEtaTransRestrRebin != 0 ) {
          fitEtaTransRestrRebin = histEtaTransRestrRebin->GetFunction( nameEtaTransRestrRebinFit.c_str() );
          if ( fitEtaTransRestrRebin != 0 ) {
            for ( unsigned iPar = 0; iPar < nPar; ++iPar ) {
              transferEtaRestr.SetParameter( iPar, fitEtaTransRestrRebin->GetParameter( iPar ) );
            }
          }
          else {
            std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                      << "    fit function '" << nameEtaTransRestrRebinFit << "' does not exist in " << nameEtaTransRestrRebin << std::endl;
            returnStatus_ += 0x200000;
          }
        }
        else {
          std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                    << "    histogram '" << nameEtaTransRestrRebin << "' does not exist in input file" << std::endl;
          returnStatus_ += 0x100000;
        }
        const std::string nameEtaTransferFunctionEtaRestr( nameEtaTransferFunctionEta + "Restr" );
        TF2 * transferFunctionEtaRestr( ( TF2* )( dirEta_->Get( nameEtaTransferFunctionEtaRestr.c_str() ) ) );
        if ( transferFunctionEtaRestr != 0 ) {
          unsigned cPar( 0 );
          for ( unsigned iPar = 0; iPar < nPar; ++iPar ) {
            for ( unsigned iDep = 0; iDep < nDep; ++iDep ) {
              if ( norm_ == ( int ) iPar ) {
                transferEtaRestr.SetParameter( iPar, iDep, my::transferFunctionInitConst );
              }
              else {
                transferEtaRestr.SetParameter( iPar, iDep, transferFunctionEtaRestr->GetParameter( ( int )cPar ) );
                ++cPar;
              }
            }
          }
        }
        else {
          std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                    << "    transfer function '" << nameEtaTransferFunctionEtaRestr << "' does not exist in input file" << std::endl;
          returnStatus_ += 0x300000;
        }

        const std::string nameEtaTransPull( nameEta + "_TransPull" );
        const std::string titleEtaTransPull( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta < " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) );
        TH1D * histEtaTransPull( new TH1D( nameEtaTransPull.c_str(), titleEtaTransPull.c_str(), histBins_, -histMax_, histMax_ ) );
        histEtaTransPull->SetXTitle( titleTransPull.c_str() );
        histEtaTransPull->SetYTitle( titleEvents.c_str() );
        const std::string nameEtaTransPullPt( nameEtaTransPull + "_Pt" );
        TH1D * histEtaTransPullPt( new TH1D( nameEtaTransPullPt.c_str(), titleEtaTransPull.c_str(), histBins_, -histMax_, histMax_ ) );
        histEtaTransPullPt->SetXTitle( titleTransPull.c_str() );
        histEtaTransPullPt->SetYTitle( titleEvents.c_str() );
        const std::string nameEtaTransPullMapPt( nameEtaTransPull + "_Map_Pt" );
        TH2D * histEtaTransPullMapPt( new TH2D( nameEtaTransPullMapPt.c_str(), titleEtaTransPull.c_str(), nPtBins_, ptBins_.data(), histBins_, -histMax_, histMax_ ) );
        histEtaTransPullMapPt->SetXTitle( titlePtRef.c_str() );
        histEtaTransPullMapPt->SetYTitle( titleTransPull.c_str() );
        histEtaTransPullMapPt->SetZTitle( titleEvents.c_str() );

        const std::string nameEtaTransPullRestr( nameEtaTransPull + "Restr" );
        TH1D * histEtaTransPullRestr( new TH1D( nameEtaTransPullRestr.c_str(), titleEtaTransPull.c_str(), histBins_, -histMax_, histMax_ ) );
        histEtaTransPullRestr->SetXTitle( titleTransPull.c_str() );
        histEtaTransPullRestr->SetYTitle( titleEvents.c_str() );
        const std::string nameEtaTransPullRestrPt( nameEtaTransPullRestr + "_Pt" );
        TH1D * histEtaTransPullRestrPt( new TH1D( nameEtaTransPullRestrPt.c_str(), titleEtaTransPull.c_str(), histBins_, -histMax_, histMax_ ) );
        histEtaTransPullRestrPt->SetXTitle( titleTransPull.c_str() );
        histEtaTransPullRestrPt->SetYTitle( titleEvents.c_str() );
        const std::string nameEtaTransPullRestrMapPt( nameEtaTransPullRestr + "_Map_Pt" );
        TH2D * histEtaTransPullRestrMapPt( new TH2D( nameEtaTransPullRestrMapPt.c_str(), titleEtaTransPull.c_str(), nPtBins_, ptBins_.data(), histBins_, -histMax_, histMax_ ) );
        histEtaTransPullRestrMapPt->SetXTitle( titlePtRef.c_str() );
        histEtaTransPullRestrMapPt->SetYTitle( titleTransPull.c_str() );
        histEtaTransPullRestrMapPt->SetZTitle( titleEvents.c_str() );

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

          const std::string nameEtaPtTransPull( nameEtaPt + "_TransPull" );
          const std::string titleEtaPtTransPull( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta < " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq " + titlePtT + " < " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
          TH1D * histEtaPtTransPull( new TH1D( nameEtaPtTransPull.c_str(), titleEtaPtTransPull.c_str(), histBins_, -histMax_, histMax_ ) );
          histEtaPtTransPull->SetXTitle( titleTransPull.c_str() );
          histEtaPtTransPull->SetYTitle( titleEvents.c_str() );

          const std::string nameEtaPtTransPullRestr( nameEtaPtTransPull + "Restr" );
          TH1D * histEtaPtTransPullRestr( new TH1D( nameEtaPtTransPullRestr.c_str(), titleEtaPtTransPull.c_str(), histBins_, -histMax_, histMax_ ) );
          histEtaPtTransPullRestr->SetXTitle( titleTransPull.c_str() );
          histEtaPtTransPullRestr->SetYTitle( titleEvents.c_str() );

          Double_t smear;
          Double_t sigma;
          for ( unsigned uEntry = 0; uEntry < sizePt.at( uPt ); ++uEntry ) {
            const Double_t ptRef( refGen_ ? ptGenEtaBin.at( uPt ).at( uEntry ) : ptEtaBin.at( uPt ).at( uEntry ) );
            const Double_t ptNonRef( refGen_ ? ptEtaBin.at( uPt ).at( uEntry ) : ptGenEtaBin.at( uPt ).at( uEntry ) );
            const Double_t etaGenSymm( useSymm_ ? std::fabs( etaGenEtaBin.at( uPt ).at( uEntry ) ) : etaGenEtaBin.at( uPt ).at( uEntry ) );
            const Double_t etaSymm( useSymm_ ? std::fabs( etaEtaBin.at( uPt ).at( uEntry ) ) : etaEtaBin.at( uPt ).at( uEntry ) );
            const Double_t etaRef( refGen_ ? etaGenSymm : etaSymm );
            if ( fitNonRestr_ ) {
              if ( fitEtaBins_ ) {
                smear = transferEta.Function( ptRef ).GetRandom();
                sigma = transferEta.Sigma( ptRef );
                histEtaPtTransPull->Fill( ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaTransPullPt->Fill( ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
                histTransPullEtaPt->Fill( ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
              }
                smear = fitEtaTransRebin->GetRandom();
                sigma = transferEta.Sigma();
                histEtaTransPull->Fill( ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaTransPullMapPt->Fill( ptRef, ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
                histTransPullEta->Fill( ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
//               }
              smear = transfer.Function( ptRef ).GetRandom();
              sigma = transfer.Sigma( ptRef );
              histVecPtTransPull.at( uPt )->Fill( ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
              histVecPtTransPullMapEta.at( uPt )->Fill( etaRef, ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
              histTransPullPt->Fill( ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
              smear = fitTransRebin->GetRandom();
              sigma = transferRestr.Sigma();
              histTransPull->Fill( ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
              histTransPullMapPt->Fill( ptRef, ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
              histTransPullMapEta->Fill( etaRef, ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
            }
            if ( ptRef >= minPt_ && reco::deltaR( etaGenEtaBin.at( uPt ).at( uEntry ), phiGenEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDR_ ) {
              if ( fitEtaBins_ ) {
                smear = transferEtaRestr.Function( ptRef ).GetRandom();
                sigma = transferEtaRestr.Sigma( ptRef );
                histEtaPtTransPullRestr->Fill( ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaTransPullRestrPt->Fill( ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
                histTransPullRestrEtaPt->Fill( ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
              }
                smear = fitEtaTransRestrRebin->GetRandom();
                sigma = transferEtaRestr.Sigma();
                histEtaTransPullRestr->Fill( ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaTransPullRestrMapPt->Fill( ptRef, ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
                histTransPullRestrEta->Fill( ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
//               }
              smear = transferRestr.Function( ptRef ).GetRandom();
              sigma = transferRestr.Sigma( ptRef );
              histVecPtTransPullRestr.at( uPt )->Fill( ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
              histVecPtTransPullRestrMapEta.at( uPt )->Fill( etaRef, ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ));
              histTransPullRestrPt->Fill( ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
              smear = fitTransRestrRebin->GetRandom();
              sigma = transferRestr.Sigma();
              histTransPullRestr->Fill( ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
              histTransPullRestrMapPt->Fill( ptRef, ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
              histTransPullRestrMapEta->Fill( etaRef, ( ptRef - ptNonRef - smear ) / sigma, weightEtaBin.at( uPt ).at( uEntry ) );
            }
          } // loop: uEntry < ptEtaBin.at( uPt ).size()

        } // loop: uPt < nPtBins_

      } // loop: keyEta

      dirFit_->cd();

      if ( sizeEtaBins != nEtaBins_ ) {
        std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                  << "    mismatch of eta binning for object category '" << objCat << "':" << std::endl
                  << "        bins in directory structure: " << sizeEtaBins                << std::endl
                  << "        bins in binning histogram  : " << nEtaBins_                  << std::endl;
        returnStatus_ += 0x100000;
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
        TDirectory * dirFit_( ( TDirectory* )( dirPt_->Get( subFit.c_str() ) ) );
        TDirectory * dirOutFit_( ( TDirectory* )( dirOutPt_->Get( subFit.c_str() ) ) );
        if ( ! dirOutFit_ ) {
          dirOutPt_->cd();
          dirOutFit_ = new TDirectory( subFit.c_str(), "" );
        }
        dirFit_->cd();

        std::string name( objCat + "_" + baseTitlePt + "_" + subFit );

        const std::string nameTransPull( name + "_TransPull" );
        const std::string nameTransPullPt( nameTransPull + "_Pt" );
        const std::string nameTransPullEtaPt( nameTransPull + "_EtaPt" );
        const std::string nameTransPullEta( nameTransPull + "_Eta" );
        TH1D * histTransPull( ( TH1D* )( dirFit_->Get( nameTransPull.c_str() ) ) );
        if ( fitNonRestr_ && histTransPull != 0 ) {
          const std::string nameTransPullFit( nameTransPull + "_fit" );
          if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameTransPullFit << std::endl;
          TF1 * fitTransPull( new TF1( nameTransPullFit.c_str(), "gaus", std::max( histTransPull->GetXaxis()->GetXmin(), histTransPull->GetMean() - histTransPull->GetRMS() * fitRange_ ), std::min( histTransPull->GetXaxis()->GetXmax(), histTransPull->GetMean() + histTransPull->GetRMS() * fitRange_ ) ) );
          setParametersFit( fitTransPull, histTransPull );
          TFitResultPtr fitTransPullResultPtr( histTransPull->Fit( fitTransPull, fitOptions_.c_str() ) );
          if ( fitTransPullResultPtr >= 0 ) {
            if ( fitTransPullResultPtr->Status() == 0 && fitTransPullResultPtr->Ndf() != 0. ) {
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << nameTransPull << "' status " << fitTransPullResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; dirFit_->pwd();
              std::cout << "    '" << nameTransPull << std::endl;
            }
          }
          if ( plot_ ) {
            histTransPull->Draw();
            c1.Print( std::string( pathPlots_ + histTransPull->GetName() + ".png" ).c_str() );
          }
        }
        TH1D * histTransPullPt( ( TH1D* )( dirFit_->Get( nameTransPullPt.c_str() ) ) );
        if ( fitNonRestr_ && histTransPullPt != 0 ) {
          const std::string nameTransPullPtFit( nameTransPullPt + "_fit" );
          if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameTransPullPtFit << std::endl;
          TF1 * fitTransPullPt( new TF1( nameTransPullPtFit.c_str(), "gaus", std::max( histTransPullPt->GetXaxis()->GetXmin(), histTransPullPt->GetMean() - histTransPullPt->GetRMS() * fitRange_ ), std::min( histTransPullPt->GetXaxis()->GetXmax(), histTransPullPt->GetMean() + histTransPullPt->GetRMS() * fitRange_ ) ) );
          setParametersFit( fitTransPullPt, histTransPullPt );
          TFitResultPtr fitTransPullPtResultPtr( histTransPullPt->Fit( fitTransPullPt, fitOptions_.c_str() ) );
          if ( fitTransPullPtResultPtr >= 0 ) {
            if ( fitTransPullPtResultPtr->Status() == 0 && fitTransPullPtResultPtr->Ndf() != 0. ) {
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << nameTransPullPt << "' status " << fitTransPullPtResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; dirFit_->pwd();
              std::cout << "    '" << nameTransPullPt << std::endl;
            }
          }
          if ( plot_ ) {
            histTransPullPt->Draw();
            c1.Print( std::string( pathPlots_ + histTransPullPt->GetName() + ".png" ).c_str() );
          }
        }
        TH1D * histTransPullEtaPt( ( TH1D* )( dirFit_->Get( nameTransPullEtaPt.c_str() ) ) );
        if ( fitNonRestr_ && histTransPullEtaPt != 0 ) {
          const std::string nameTransPullEtaPtFit( nameTransPullEtaPt + "_fit" );
          if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameTransPullEtaPtFit << std::endl;
          TF1 * fitTransPullEtaPt( new TF1( nameTransPullEtaPtFit.c_str(), "gaus", std::max( histTransPullEtaPt->GetXaxis()->GetXmin(), histTransPullEtaPt->GetMean() - histTransPullEtaPt->GetRMS() * fitRange_ ), std::min( histTransPullEtaPt->GetXaxis()->GetXmax(), histTransPullEtaPt->GetMean() + histTransPullEtaPt->GetRMS() * fitRange_ ) ) );
          setParametersFit( fitTransPullEtaPt, histTransPullEtaPt );
          TFitResultPtr fitTransPullEtaPtResultPtr( histTransPullEtaPt->Fit( fitTransPullEtaPt, fitOptions_.c_str() ) );
          if ( fitTransPullEtaPtResultPtr >= 0 ) {
            if ( fitTransPullEtaPtResultPtr->Status() == 0 && fitTransPullEtaPtResultPtr->Ndf() != 0. ) {
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << nameTransPullEtaPt << "' status " << fitTransPullEtaPtResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; dirFit_->pwd();
              std::cout << "    '" << nameTransPullEtaPt << std::endl;
            }
          }
          if ( plot_ ) {
            histTransPullEtaPt->Draw();
            c1.Print( std::string( pathPlots_ + histTransPullEtaPt->GetName() + ".png" ).c_str() );
          }
        }
        TH1D * histTransPullEta( ( TH1D* )( dirFit_->Get( nameTransPullEta.c_str() ) ) );
        if ( fitNonRestr_ && histTransPullEta != 0 ) {
          const std::string nameTransPullEtaFit( nameTransPullEta + "_fit" );
          if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameTransPullEtaFit << std::endl;
          TF1 * fitTransPullEta( new TF1( nameTransPullEtaFit.c_str(), "gaus", std::max( histTransPullEta->GetXaxis()->GetXmin(), histTransPullEta->GetMean() - histTransPullEta->GetRMS() * fitRange_ ), std::min( histTransPullEta->GetXaxis()->GetXmax(), histTransPullEta->GetMean() + histTransPullEta->GetRMS() * fitRange_ ) ) );
          setParametersFit( fitTransPullEta, histTransPullEta );
          TFitResultPtr fitTransPullEtaResultPtr( histTransPullEta->Fit( fitTransPullEta, fitOptions_.c_str() ) );
          if ( fitTransPullEtaResultPtr >= 0 ) {
            if ( fitTransPullEtaResultPtr->Status() == 0 && fitTransPullEtaResultPtr->Ndf() != 0. ) {
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << nameTransPullEta << "' status " << fitTransPullEtaResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; dirFit_->pwd();
              std::cout << "    '" << nameTransPullEta << std::endl;
            }
          }
          if ( plot_ ) {
            histTransPullEta->Draw();
            c1.Print( std::string( pathPlots_ + histTransPullEta->GetName() + ".png" ).c_str() );
          }
        }

        const std::string nameTransPullRestr( nameTransPull + "Restr" );
        const std::string nameTransPullRestrPt( nameTransPullRestr + "_Pt" );
        const std::string nameTransPullRestrEtaPt( nameTransPullRestr + "_EtaPt" );
        const std::string nameTransPullRestrEta( nameTransPullRestr + "_Eta" );
        TH1D * histTransPullRestr( ( TH1D* )( dirFit_->Get( nameTransPullRestr.c_str() ) ) );
        if ( histTransPullRestr != 0 ) {
          const std::string nameTransPullRestrFit( nameTransPullRestr + "_fit" );
          if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameTransPullRestrFit << std::endl;
          TF1 * fitTransPullRestr( new TF1( nameTransPullRestrFit.c_str(), "gaus", std::max( histTransPullRestr->GetXaxis()->GetXmin(), histTransPullRestr->GetMean() - histTransPullRestr->GetRMS() * fitRange_ ), std::min( histTransPullRestr->GetXaxis()->GetXmax(), histTransPullRestr->GetMean() + histTransPullRestr->GetRMS() * fitRange_ ) ) );
          setParametersFit( fitTransPullRestr, histTransPullRestr );
          TFitResultPtr fitTransPullRestrResultPtr( histTransPullRestr->Fit( fitTransPullRestr, fitOptions_.c_str() ) );
          if ( fitTransPullRestrResultPtr >= 0 ) {
            if ( fitTransPullRestrResultPtr->Status() == 0 && fitTransPullRestrResultPtr->Ndf() != 0. ) {
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << nameTransPullRestr << "' status " << fitTransPullRestrResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; dirFit_->pwd();
              std::cout << "    '" << nameTransPullRestr << std::endl;
            }
          }
          if ( plot_ ) {
            histTransPullRestr->Draw();
            c1.Print( std::string( pathPlots_ + histTransPullRestr->GetName() + ".png" ).c_str() );
          }
        }
        TH1D * histTransPullRestrPt( ( TH1D* )( dirFit_->Get( nameTransPullRestrPt.c_str() ) ) );
        if ( histTransPullRestrPt != 0 ) {
          const std::string nameTransPullRestrPtFit( nameTransPullRestrPt + "_fit" );
          if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameTransPullRestrPtFit << std::endl;
          TF1 * fitTransPullRestrPt( new TF1( nameTransPullRestrPtFit.c_str(), "gaus", std::max( histTransPullRestrPt->GetXaxis()->GetXmin(), histTransPullRestrPt->GetMean() - histTransPullRestrPt->GetRMS() * fitRange_ ), std::min( histTransPullRestrPt->GetXaxis()->GetXmax(), histTransPullRestrPt->GetMean() + histTransPullRestrPt->GetRMS() * fitRange_ ) ) );
          setParametersFit( fitTransPullRestrPt, histTransPullRestrPt );
          TFitResultPtr fitTransPullRestrPtResultPtr( histTransPullRestrPt->Fit( fitTransPullRestrPt, fitOptions_.c_str() ) );
          if ( fitTransPullRestrPtResultPtr >= 0 ) {
            if ( fitTransPullRestrPtResultPtr->Status() == 0 && fitTransPullRestrPtResultPtr->Ndf() != 0. ) {
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << nameTransPullRestrPt << "' status " << fitTransPullRestrPtResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; dirFit_->pwd();
              std::cout << "    '" << nameTransPullRestrPt << std::endl;
            }
          }
          if ( plot_ ) {
            histTransPullRestrPt->Draw();
            c1.Print( std::string( pathPlots_ + histTransPullRestrPt->GetName() + ".png" ).c_str() );
          }
        }
        TH1D * histTransPullRestrEtaPt( ( TH1D* )( dirFit_->Get( nameTransPullRestrEtaPt.c_str() ) ) );
        if ( histTransPullRestrEtaPt != 0 ) {
          const std::string nameTransPullRestrEtaPtFit( nameTransPullRestrEtaPt + "_fit" );
          if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameTransPullRestrEtaPtFit << std::endl;
          TF1 * fitTransPullRestrEtaPt( new TF1( nameTransPullRestrEtaPtFit.c_str(), "gaus", std::max( histTransPullRestrEtaPt->GetXaxis()->GetXmin(), histTransPullRestrEtaPt->GetMean() - histTransPullRestrEtaPt->GetRMS() * fitRange_ ), std::min( histTransPullRestrEtaPt->GetXaxis()->GetXmax(), histTransPullRestrEtaPt->GetMean() + histTransPullRestrEtaPt->GetRMS() * fitRange_ ) ) );
          setParametersFit( fitTransPullRestrEtaPt, histTransPullRestrEtaPt );
          TFitResultPtr fitTransPullRestrEtaPtResultPtr( histTransPullRestrEtaPt->Fit( fitTransPullRestrEtaPt, fitOptions_.c_str() ) );
          if ( fitTransPullRestrEtaPtResultPtr >= 0 ) {
            if ( fitTransPullRestrEtaPtResultPtr->Status() == 0 && fitTransPullRestrEtaPtResultPtr->Ndf() != 0. ) {
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << nameTransPullRestrEtaPt << "' status " << fitTransPullRestrEtaPtResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; dirFit_->pwd();
              std::cout << "    '" << nameTransPullRestrEtaPt << std::endl;
            }
          }
          if ( plot_ ) {
            histTransPullRestrEtaPt->Draw();
            c1.Print( std::string( pathPlots_ + histTransPullRestrEtaPt->GetName() + ".png" ).c_str() );
          }
        }
        TH1D * histTransPullRestrEta( ( TH1D* )( dirFit_->Get( nameTransPullRestrEta.c_str() ) ) );
        if ( histTransPullRestrEta != 0 ) {
          const std::string nameTransPullRestrEtaFit( nameTransPullRestrEta + "_fit" );
          if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameTransPullRestrEtaFit << std::endl;
          TF1 * fitTransPullRestrEta( new TF1( nameTransPullRestrEtaFit.c_str(), "gaus", std::max( histTransPullRestrEta->GetXaxis()->GetXmin(), histTransPullRestrEta->GetMean() - histTransPullRestrEta->GetRMS() * fitRange_ ), std::min( histTransPullRestrEta->GetXaxis()->GetXmax(), histTransPullRestrEta->GetMean() + histTransPullRestrEta->GetRMS() * fitRange_ ) ) );
          setParametersFit( fitTransPullRestrEta, histTransPullRestrEta );
          TFitResultPtr fitTransPullRestrEtaResultPtr( histTransPullRestrEta->Fit( fitTransPullRestrEta, fitOptions_.c_str() ) );
          if ( fitTransPullRestrEtaResultPtr >= 0 ) {
            if ( fitTransPullRestrEtaResultPtr->Status() == 0 && fitTransPullRestrEtaResultPtr->Ndf() != 0. ) {
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << nameTransPullRestrEta << "' status " << fitTransPullRestrEtaResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; dirFit_->pwd();
              std::cout << "    '" << nameTransPullRestrEta << std::endl;
            }
          }
          if ( plot_ ) {
            histTransPullRestrEta->Draw();
            c1.Print( std::string( pathPlots_ + histTransPullRestrEta->GetName() + ".png" ).c_str() );
          }
        }

        // Loop over pt bins

        std::vector< TH1D * > histVecTransPullPtFitMap;
        std::vector< TH1D * > histVecTransPullRestrPtFitMap;
        for ( unsigned uPar = 0; uPar < 3; ++uPar ) {
          const std::string parFit( boost::lexical_cast< std::string >( uPar ) );
          const std::string nameTransPullPtFitMap( nameTransPull + baseTitlePt + "_FitMap_Par" + parFit );
          const std::string titleTransPullPtFitMap( objCat + ", par. " + parFit );
          TH1D * histTransPullPtFitMap( new TH1D( nameTransPullPtFitMap.c_str(), titleTransPullPtFitMap.c_str(), nPtBins_, ptBins_.data() ) );
          histTransPullPtFitMap->SetXTitle( titlePtRef.c_str() );
          histVecTransPullPtFitMap.push_back( histTransPullPtFitMap );
          const std::string nameTransPullRestrPtFitMap( nameTransPullRestr + baseTitlePt + "_FitMap_Par" + parFit );
          TH1D * histTransPullRestrPtFitMap( new TH1D( nameTransPullRestrPtFitMap.c_str(), titleTransPullPtFitMap.c_str(), nPtBins_, ptBins_.data() ) );
          histTransPullRestrPtFitMap->SetXTitle( titlePtRef.c_str() );
          histVecTransPullRestrPtFitMap.push_back( histTransPullRestrPtFitMap );
        }
        const std::string titleTransPullPtFitMapProb( objCat + ", fit prob." );
        const std::string nameTransPullPtFitMapProb( nameTransPull + baseTitlePt + "_FitMap_Prob" );
        TH1D * histTransPullPtFitMapProb( new TH1D( nameTransPullPtFitMapProb.c_str(), titleTransPullPtFitMapProb.c_str(), nPtBins_, ptBins_.data() ) );
        histTransPullPtFitMapProb->SetXTitle( titlePtRef.c_str() );
        histTransPullPtFitMapProb->SetYTitle( titleProb.c_str() );
        const std::string nameTransPullRestrPtFitMapProb( nameTransPullRestr + baseTitlePt + "_FitMap_Prob" );
        TH1D * histTransPullRestrPtFitMapProb( new TH1D( nameTransPullRestrPtFitMapProb.c_str(), titleTransPullPtFitMapProb.c_str(), nPtBins_, ptBins_.data() ) );
        histTransPullRestrPtFitMapProb->SetXTitle( titlePtRef.c_str() );
        histTransPullRestrPtFitMapProb->SetYTitle( titleProb.c_str() );

        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
          const std::string namePt( name + "_" + baseTitlePt + binPt );

          const std::string namePtTransPull( namePt + "_TransPull" );
          TH1D * histPtTransPull( ( TH1D* )( dirFit_->Get( namePtTransPull.c_str() ) ) );
          if ( fitNonRestr_ && histPtTransPull != 0 ) {
            const std::string namePtTransPullFit( namePtTransPull + "_fit" );
            if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << namePtTransPullFit << std::endl;
            TF1 * fitPtTransPull( new TF1( namePtTransPullFit.c_str(), "gaus", std::max( histPtTransPull->GetXaxis()->GetXmin(), histPtTransPull->GetMean() - histPtTransPull->GetRMS() * fitRange_ ), std::min( histPtTransPull->GetXaxis()->GetXmax(), histPtTransPull->GetMean() + histPtTransPull->GetRMS() * fitRange_ ) ) );
            setParametersFit( fitPtTransPull, histPtTransPull );
            TFitResultPtr fitPtTransPullResultPtr( histPtTransPull->Fit( fitPtTransPull, fitOptions_.c_str() ) );
            if ( fitPtTransPullResultPtr >= 0 ) {
              if ( fitPtTransPullResultPtr->Status() == 0 && fitPtTransPullResultPtr->Ndf() != 0. ) {
                for ( unsigned uPar = 0; uPar < 3; ++uPar ) {
                  histVecTransPullPtFitMap.at( uPar )->SetBinContent( uPt + 1, fitPtTransPullResultPtr->Parameter( uPar ) );
                  histVecTransPullPtFitMap.at( uPar )->SetBinError( uPt + 1, fitPtTransPullResultPtr->ParError( uPar ) );
                }
                if ( fitPtTransPullResultPtr->Prob() > 0 ) histTransPullPtFitMapProb->SetBinContent( uPt + 1, log10( fitPtTransPullResultPtr->Prob() ) );
                else histTransPullPtFitMapProb->SetBinContent( uPt + 1, 1 );
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; dirFit_->pwd();
                  std::cout << "    '" << namePtTransPull << "' status " << fitPtTransPullResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << namePtTransPull << std::endl;
              }
            }
          }

          const std::string namePtTransPullRestr( namePt + "_TransPullRestr" );
          TH1D * histPtTransPullRestr( ( TH1D* )( dirFit_->Get( namePtTransPullRestr.c_str() ) ) );
          if ( histPtTransPullRestr != 0 ) {
            const std::string namePtTransPullRestrFit( namePtTransPullRestr + "_fit" );
            if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << namePtTransPullRestrFit << std::endl;
            TF1 * fitPtTransPullRestr( new TF1( namePtTransPullRestrFit.c_str(), "gaus", std::max( histPtTransPullRestr->GetXaxis()->GetXmin(), histPtTransPullRestr->GetMean() - histPtTransPullRestr->GetRMS() * fitRange_ ), std::min( histPtTransPullRestr->GetXaxis()->GetXmax(), histPtTransPullRestr->GetMean() + histPtTransPullRestr->GetRMS() * fitRange_ ) ) );
            setParametersFit( fitPtTransPullRestr, histPtTransPullRestr );
            TFitResultPtr fitPtTransPullRestrResultPtr( histPtTransPullRestr->Fit( fitPtTransPullRestr, fitOptions_.c_str() ) );
            if ( fitPtTransPullRestrResultPtr >= 0 ) {
              if ( fitPtTransPullRestrResultPtr->Status() == 0 && fitPtTransPullRestrResultPtr->Ndf() != 0. ) {
                for ( unsigned uPar = 0; uPar < 3; ++uPar ) {
                  histVecTransPullRestrPtFitMap.at( uPar )->SetBinContent( uPt + 1, fitPtTransPullRestrResultPtr->Parameter( uPar ) );
                  histVecTransPullRestrPtFitMap.at( uPar )->SetBinError( uPt + 1, fitPtTransPullRestrResultPtr->ParError( uPar ) );
                }
                if ( fitPtTransPullRestrResultPtr->Prob() > 0 ) histTransPullRestrPtFitMapProb->SetBinContent( uPt + 1, log10( fitPtTransPullRestrResultPtr->Prob() ) );
                else histTransPullRestrPtFitMapProb->SetBinContent( uPt + 1, 1 );
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; dirFit_->pwd();
                  std::cout << "    '" << namePtTransPullRestr << "' status " << fitPtTransPullRestrResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << namePtTransPullRestr << std::endl;
              }
            }
          }

          if ( plot_ ) {
            if ( fitNonRestr_ ) {
              histPtTransPull->Draw();
              c1.Print( std::string( pathPlots_ + histPtTransPull->GetName() + ".png" ).c_str() );
            }
            histPtTransPullRestr->Draw();
            c1.Print( std::string( pathPlots_ + histPtTransPullRestr->GetName() + ".png" ).c_str() );
          }

        } // loop: uPt < nPtBins_

        dirFit_->cd();

        if ( plot_ ) {
          for ( unsigned uPar = 0; uPar < 3; ++uPar ) {
            if ( fitNonRestr_ ) {
              histVecTransPullPtFitMap.at( uPar )->Draw();
              c1.Print( std::string( pathPlots_ + histVecTransPullPtFitMap.at( uPar )->GetName() + ".png" ).c_str() );
            }
            histVecTransPullRestrPtFitMap.at( uPar )->Draw();
            c1.Print( std::string( pathPlots_ + histVecTransPullRestrPtFitMap.at( uPar )->GetName() + ".png" ).c_str() );
          }
          if ( fitNonRestr_ ) {
            histTransPullPtFitMapProb->Draw();
            c1.Print( std::string( pathPlots_ + histTransPullPtFitMapProb->GetName() + ".png" ).c_str() );
          }
          histTransPullRestrPtFitMapProb->Draw();
          c1.Print( std::string( pathPlots_ + histTransPullRestrPtFitMapProb->GetName() + ".png" ).c_str() );
        }

        // Loop over eta bins

        std::vector< TH1D * > histVecTransPullEtaFitMap;
        std::vector< TH1D * > histVecTransPullRestrEtaFitMap;
        for ( unsigned uPar = 0; uPar < 3; ++uPar ) {
          const std::string parFit( boost::lexical_cast< std::string >( uPar ) );
          const std::string nameTransPullEtaFitMap( nameTransPull + "Eta_FitMap_Par" + parFit );
          const std::string titleTransPullEtaFitMap( objCat + ", par. " + parFit );
          TH1D * histTransPullEtaFitMap( new TH1D( nameTransPullEtaFitMap.c_str(), titleTransPullEtaFitMap.c_str(), nEtaBins_, etaBins_.data() ) );
          histTransPullEtaFitMap->SetXTitle( titleEta.c_str() );
          histVecTransPullEtaFitMap.push_back( histTransPullEtaFitMap );
          const std::string nameTransPullRestrEtaFitMap( name + "_TransPullRestrEta_FitMap_Par" + parFit );
          TH1D * histTransPullRestrEtaFitMap( new TH1D( nameTransPullRestrEtaFitMap.c_str(), titleTransPullEtaFitMap.c_str(), nEtaBins_, etaBins_.data() ) );
          histTransPullRestrEtaFitMap->SetXTitle( titleEta.c_str() );
          histVecTransPullRestrEtaFitMap.push_back( histTransPullRestrEtaFitMap );
        }
        const std::string titleTransPullEtaFitMapProb( objCat + ", fit prob." );
        const std::string nameTransPullEtaFitMapProb( nameTransPull + "Eta_FitMap_Prob" );
        TH1D * histTransPullEtaFitMapProb( new TH1D( nameTransPullEtaFitMapProb.c_str(), titleTransPullEtaFitMapProb.c_str(), nEtaBins_, etaBins_.data() ) );
        histTransPullEtaFitMapProb->SetXTitle( titleEta.c_str() );
        histTransPullEtaFitMapProb->SetYTitle( titleProb.c_str() );
        const std::string nameTransPullRestrEtaFitMapProb( nameTransPullRestr + "Eta_FitMap_Prob" );
        TH1D * histTransPullRestrEtaFitMapProb( new TH1D( nameTransPullRestrEtaFitMapProb.c_str(), titleTransPullEtaFitMapProb.c_str(), nEtaBins_, etaBins_.data() ) );
        histTransPullRestrEtaFitMapProb->SetXTitle( titleEta.c_str() );
        histTransPullRestrEtaFitMapProb->SetYTitle( titleProb.c_str() );

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

          const std::string nameEtaTransPull( nameEta + "_TransPull" );
          const std::string nameEtaTransPullPt( nameEtaTransPull + "_Pt" );
          if ( fitNonRestr_ ) {
            TH1D * histEtaTransPull( ( TH1D* )( dirEta_->Get( nameEtaTransPull.c_str() ) ) );
            if ( histEtaTransPull != 0 ) {
              const std::string nameEtaTransPullFit( nameEtaTransPull + "_fit" );
              if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameEtaTransPullFit << std::endl;
              TF1 * fitEtaTransPull( new TF1( nameEtaTransPullFit.c_str(), "gaus", std::max( histEtaTransPull->GetXaxis()->GetXmin(), histEtaTransPull->GetMean() - histEtaTransPull->GetRMS() * fitRange_ ), std::min( histEtaTransPull->GetXaxis()->GetXmax(), histEtaTransPull->GetMean() + histEtaTransPull->GetRMS() * fitRange_ ) ) );
              setParametersFit( fitEtaTransPull, histEtaTransPull );
              TFitResultPtr fitEtaTransPullResultPtr( histEtaTransPull->Fit( fitEtaTransPull, fitOptions_.c_str() ) );
              if ( fitEtaTransPullResultPtr >= 0 ) {
                if ( fitEtaTransPullResultPtr->Status() == 0 && fitEtaTransPullResultPtr->Ndf() != 0. ) {
                  for ( unsigned uPar = 0; uPar < 3; ++uPar ) {
                    histVecTransPullEtaFitMap.at( uPar )->SetBinContent( uEta + 1, fitEtaTransPullResultPtr->Parameter( uPar ) );
                    histVecTransPullEtaFitMap.at( uPar )->SetBinError( uEta + 1, fitEtaTransPullResultPtr->ParError( uPar ) );
                  }
                  if ( fitEtaTransPullResultPtr->Prob() > 0 ) histTransPullEtaFitMapProb->SetBinContent( uEta + 1, log10( fitEtaTransPullResultPtr->Prob() ) );
                  else histTransPullEtaFitMapProb->SetBinContent( uEta + 1, 1 );
                }
                else {
                  if ( verbose_ > 2 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    failing fit in directory '"; dirEta_->pwd();
                    std::cout << "    '" << nameEtaTransPull << "' status " << fitEtaTransPullResultPtr->Status() << std::endl;
                  }
                }
              }
              else {
                if ( verbose_ > 1 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    missing fit in directory '"; dirEta_->pwd();
                  std::cout << "    '" << nameEtaTransPull << std::endl;
                }
              }
              if ( plot_ ) {
                histEtaTransPull->Draw();
                c1.Print( std::string( pathPlots_ + histEtaTransPull->GetName() + ".png" ).c_str() );
              }
            }
            TH1D * histEtaTransPullPt( ( TH1D* )( dirEta_->Get( nameEtaTransPullPt.c_str() ) ) );
            if ( histEtaTransPullPt != 0 ) {
              const std::string nameEtaTransPullPtFit( nameEtaTransPullPt + "_fit" );
              if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameEtaTransPullPtFit << std::endl;
              TF1 * fitEtaTransPullPt( new TF1( nameEtaTransPullPtFit.c_str(), "gaus", std::max( histEtaTransPullPt->GetXaxis()->GetXmin(), histEtaTransPullPt->GetMean() - histEtaTransPullPt->GetRMS() * fitRange_ ), std::min( histEtaTransPullPt->GetXaxis()->GetXmax(), histEtaTransPullPt->GetMean() + histEtaTransPullPt->GetRMS() * fitRange_ ) ) );
              setParametersFit( fitEtaTransPullPt, histEtaTransPullPt );
              TFitResultPtr fitEtaTransPullPtResultPtr( histEtaTransPullPt->Fit( fitEtaTransPullPt, fitOptions_.c_str() ) );
              if ( fitEtaTransPullPtResultPtr >= 0 ) {
                if ( fitEtaTransPullPtResultPtr->Status() == 0 && fitEtaTransPullPtResultPtr->Ndf() != 0. ) {
                  for ( unsigned uPar = 0; uPar < 3; ++uPar ) {
                    histVecTransPullEtaFitMap.at( uPar )->SetBinContent( uEta + 1, fitEtaTransPullPtResultPtr->Parameter( uPar ) );
                    histVecTransPullEtaFitMap.at( uPar )->SetBinError( uEta + 1, fitEtaTransPullPtResultPtr->ParError( uPar ) );
                  }
                  if ( fitEtaTransPullPtResultPtr->Prob() > 0 ) histTransPullEtaFitMapProb->SetBinContent( uEta + 1, log10( fitEtaTransPullPtResultPtr->Prob() ) );
                  else histTransPullEtaFitMapProb->SetBinContent( uEta + 1, 1 );
                }
                else {
                  if ( verbose_ > 2 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    failing fit in directory '"; dirEta_->pwd();
                    std::cout << "    '" << nameEtaTransPullPt << "' status " << fitEtaTransPullPtResultPtr->Status() << std::endl;
                  }
                }
              }
              else {
                if ( verbose_ > 1 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    missing fit in directory '"; dirEta_->pwd();
                  std::cout << "    '" << nameEtaTransPullPt << std::endl;
                }
              }
              if ( plot_ ) {
                histEtaTransPullPt->Draw();
                c1.Print( std::string( pathPlots_ + histEtaTransPullPt->GetName() + ".png" ).c_str() );
              }
            }
          }

          const std::string nameEtaTransPullRestr( nameEta + "_TransPullRestr" );
          const std::string nameEtaTransPullRestrPt( nameEtaTransPullRestr + "_Pt" );
          TH1D * histEtaTransPullRestr( ( TH1D* )( dirEta_->Get( nameEtaTransPullRestr.c_str() ) ) );
          if ( histEtaTransPullRestr != 0 ) {
            const std::string nameEtaTransPullRestrFit( nameEtaTransPullRestr + "_fit" );
            if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameEtaTransPullRestrFit << std::endl;
            TF1 * fitEtaTransPullRestr( new TF1( nameEtaTransPullRestrFit.c_str(), "gaus", std::max( histEtaTransPullRestr->GetXaxis()->GetXmin(), histEtaTransPullRestr->GetMean() - histEtaTransPullRestr->GetRMS() * fitRange_ ), std::min( histEtaTransPullRestr->GetXaxis()->GetXmax(), histEtaTransPullRestr->GetMean() + histEtaTransPullRestr->GetRMS() * fitRange_ ) ) );
            setParametersFit( fitEtaTransPullRestr, histEtaTransPullRestr );
            TFitResultPtr fitEtaTransPullRestrResultPtr( histEtaTransPullRestr->Fit( fitEtaTransPullRestr, fitOptions_.c_str() ) );
            if ( fitEtaTransPullRestrResultPtr >= 0 ) {
              if ( fitEtaTransPullRestrResultPtr->Status() == 0 && fitEtaTransPullRestrResultPtr->Ndf() != 0. ) {
                for ( unsigned uPar = 0; uPar < 3; ++uPar ) {
                  histVecTransPullRestrEtaFitMap.at( uPar )->SetBinContent( uEta + 1, fitEtaTransPullRestrResultPtr->Parameter( uPar ) );
                  histVecTransPullRestrEtaFitMap.at( uPar )->SetBinError( uEta + 1, fitEtaTransPullRestrResultPtr->ParError( uPar ) );
                }
                if ( fitEtaTransPullRestrResultPtr->Prob() > 0 ) histTransPullRestrEtaFitMapProb->SetBinContent( uEta + 1, log10( fitEtaTransPullRestrResultPtr->Prob() ) );
                else histTransPullRestrEtaFitMapProb->SetBinContent( uEta + 1, 1 );
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; dirEta_->pwd();
                  std::cout << "    '" << nameEtaTransPullRestr << "' status " << fitEtaTransPullRestrResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; dirEta_->pwd();
                std::cout << "    '" << nameEtaTransPullRestr << std::endl;
              }
            }
            if ( plot_ ) {
              histEtaTransPullRestr->Draw();
              c1.Print( std::string( pathPlots_ + histEtaTransPullRestr->GetName() + ".png" ).c_str() );
            }
          }
          TH1D * histEtaTransPullRestrPt( ( TH1D* )( dirEta_->Get( nameEtaTransPullRestrPt.c_str() ) ) );
          if ( histEtaTransPullRestrPt != 0 ) {
            const std::string nameEtaTransPullRestrPtFit( nameEtaTransPullRestrPt + "_fit" );
            if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameEtaTransPullRestrPtFit << std::endl;
            TF1 * fitEtaTransPullRestrPt( new TF1( nameEtaTransPullRestrPtFit.c_str(), "gaus", std::max( histEtaTransPullRestrPt->GetXaxis()->GetXmin(), histEtaTransPullRestrPt->GetMean() - histEtaTransPullRestrPt->GetRMS() * fitRange_ ), std::min( histEtaTransPullRestrPt->GetXaxis()->GetXmax(), histEtaTransPullRestrPt->GetMean() + histEtaTransPullRestrPt->GetRMS() * fitRange_ ) ) );
            setParametersFit( fitEtaTransPullRestrPt, histEtaTransPullRestrPt );
            TFitResultPtr fitEtaTransPullRestrPtResultPtr( histEtaTransPullRestrPt->Fit( fitEtaTransPullRestrPt, fitOptions_.c_str() ) );
            if ( fitEtaTransPullRestrPtResultPtr >= 0 ) {
              if ( fitEtaTransPullRestrPtResultPtr->Status() == 0 && fitEtaTransPullRestrPtResultPtr->Ndf() != 0. ) {
                for ( unsigned uPar = 0; uPar < 3; ++uPar ) {
                  histVecTransPullRestrEtaFitMap.at( uPar )->SetBinContent( uEta + 1, fitEtaTransPullRestrPtResultPtr->Parameter( uPar ) );
                  histVecTransPullRestrEtaFitMap.at( uPar )->SetBinError( uEta + 1, fitEtaTransPullRestrPtResultPtr->ParError( uPar ) );
                }
                if ( fitEtaTransPullRestrPtResultPtr->Prob() > 0 ) histTransPullRestrEtaFitMapProb->SetBinContent( uEta + 1, log10( fitEtaTransPullRestrPtResultPtr->Prob() ) );
                else histTransPullRestrEtaFitMapProb->SetBinContent( uEta + 1, 1 );
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; dirEta_->pwd();
                  std::cout << "    '" << nameEtaTransPullRestrPt << "' status " << fitEtaTransPullRestrPtResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; dirEta_->pwd();
                std::cout << "    '" << nameEtaTransPullRestrPt << std::endl;
              }
            }
            if ( plot_ ) {
              histEtaTransPullRestrPt->Draw();
              c1.Print( std::string( pathPlots_ + histEtaTransPullRestrPt->GetName() + ".png" ).c_str() );
            }
          }

          if ( fitEtaBins_ ) {

            // Loop over pt bins

            std::vector< TH1D * > histVecTransPullEtaPtFitMap;
            std::vector< TH1D * > histVecTransPullRestrEtaPtFitMap;
            for ( unsigned uPar = 0; uPar < 3; ++uPar ) {
              const std::string parFit( boost::lexical_cast< std::string >( uPar ) );
              const std::string nameTransPullEtaPtFitMap( nameEta + "_TransPull" + baseTitlePt + "_FitMap_Par" + parFit );
              const std::string titleTransPullEtaPtFitMap( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta < " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) + ", par. " + parFit );
              TH1D * histTransPullEtaPtFitMap( new TH1D( nameTransPullEtaPtFitMap.c_str(), titleTransPullEtaPtFitMap.c_str(), nPtBins_, ptBins_.data() ) );
              histTransPullEtaPtFitMap->SetXTitle( titlePtRef.c_str() );
              histVecTransPullEtaPtFitMap.push_back( histTransPullEtaPtFitMap );
              const std::string nameTransPullRestrEtaPtFitMap( nameEta + "_TransPullRestr" + baseTitlePt + "_FitMap_Par" + parFit );
              TH1D * histTransPullRestrEtaPtFitMap( new TH1D( nameTransPullRestrEtaPtFitMap.c_str(), titleTransPullEtaPtFitMap.c_str(), nPtBins_, ptBins_.data() ) );
              histTransPullRestrEtaPtFitMap->SetXTitle( titlePtRef.c_str() );
              histVecTransPullRestrEtaPtFitMap.push_back( histTransPullRestrEtaPtFitMap );
            }
            const std::string titleTransPullEtaPtFitMapProb( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta < " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) + ", fit prob." );
            const std::string nameTransPullEtaPtFitMapProb( nameEtaTransPull + baseTitlePt + "_FitMap_Prob" );
            TH1D * histTransPullEtaPtFitMapProb( new TH1D( nameTransPullEtaPtFitMapProb.c_str(), titleTransPullEtaPtFitMapProb.c_str(), nPtBins_, ptBins_.data() ) );
            histTransPullEtaPtFitMapProb->SetXTitle( titlePtRef.c_str() );
            histTransPullEtaPtFitMapProb->SetYTitle( titleProb.c_str() );
            const std::string nameTransPullRestrEtaPtFitMapProb( nameEtaTransPullRestr + baseTitlePt + "_FitMap_Prob" );
            TH1D * histTransPullRestrEtaPtFitMapProb( new TH1D( nameTransPullRestrEtaPtFitMapProb.c_str(), titleTransPullEtaPtFitMapProb.c_str(), nPtBins_, ptBins_.data() ) );
            histTransPullRestrEtaPtFitMapProb->SetXTitle( titlePtRef.c_str() );
            histTransPullRestrEtaPtFitMapProb->SetYTitle( titleProb.c_str() );

            for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
              const std::string binPt( boost::lexical_cast< std::string >( uPt ) );

              const std::string nameEtaPt( nameEta + "_" + baseTitlePt + binPt );
              const std::string nameEtaPtTransPull( nameEtaPt + "_TransPull" );

              if ( fitNonRestr_ ) {
                const std::string nameEtaPtTransPull( nameEtaPtTransPull + "" );
                TH1D * histEtaPtTransPull( ( TH1D* )( dirEta_->Get( nameEtaPtTransPull.c_str() ) ) );
                if ( histEtaPtTransPull != 0 ) {
                  const std::string nameEtaPtTransPullFit( nameEtaPtTransPull + "_fit" );
                  if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameEtaPtTransPullFit << std::endl;
                  TF1 * fitEtaPtTransPull( new TF1( nameEtaPtTransPullFit.c_str(), "gaus", std::max( histEtaPtTransPull->GetXaxis()->GetXmin(), histEtaPtTransPull->GetMean() - histEtaPtTransPull->GetRMS() * fitRange_ ), std::min( histEtaPtTransPull->GetXaxis()->GetXmax(), histEtaPtTransPull->GetMean() + histEtaPtTransPull->GetRMS() * fitRange_ ) ) );
                  setParametersFit( fitEtaPtTransPull, histEtaPtTransPull );
                  TFitResultPtr fitEtaPtTransPullResultPtr( histEtaPtTransPull->Fit( fitEtaPtTransPull, fitOptions_.c_str() ) );
                  if ( fitEtaPtTransPullResultPtr >= 0 ) {
                    if ( fitEtaPtTransPullResultPtr->Status() == 0 && fitEtaPtTransPullResultPtr->Ndf() != 0. ) {
                      for ( unsigned uPar = 0; uPar < 3; ++uPar ) {
                        histVecTransPullEtaPtFitMap.at( uPar )->SetBinContent( uPt + 1, fitEtaPtTransPullResultPtr->Parameter( uPar ) );
                        histVecTransPullEtaPtFitMap.at( uPar )->SetBinError( uPt + 1, fitEtaPtTransPullResultPtr->ParError( uPar ) );
                      }
                      if ( fitEtaPtTransPullResultPtr->Prob() > 0 ) histTransPullEtaPtFitMapProb->SetBinContent( uPt + 1, log10( fitEtaPtTransPullResultPtr->Prob() ) );
                      else histTransPullEtaPtFitMapProb->SetBinContent( uPt + 1, 1 );
                    }
                    else {
                      if ( verbose_ > 2 ) {
                        std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                                  << "    failing fit in directory '"; dirEta_->pwd();
                        std::cout << "    '" << nameEtaPtTransPull << "' status " << fitEtaPtTransPullResultPtr->Status() << std::endl;
                      }
                    }
                  }
                  else {
                    if ( verbose_ > 1 ) {
                      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                                << "    missing fit in directory '"; dirEta_->pwd();
                      std::cout << "    '" << nameEtaPtTransPull << std::endl;
                    }
                  }
                  if ( plot_ ) {
                    histEtaPtTransPull->Draw();
                    c1.Print( std::string( pathPlots_ + histEtaPtTransPull->GetName() + ".png" ).c_str() );
                  }
                }
              }

              const std::string nameEtaPtTransPullRestr( nameEtaPtTransPull + "Restr" );
              TH1D * histEtaPtTransPullRestr( ( TH1D* )( dirEta_->Get( nameEtaPtTransPullRestr.c_str() ) ) );
              if ( histEtaPtTransPullRestr != 0 ) {
                const std::string nameEtaPtTransPullRestrFit( nameEtaPtTransPullRestr + "_fit" );
                if ( verbose_ > 2 ) std::cout << argv[ 0 ] << " --> FIT: " << nameEtaPtTransPullRestrFit << std::endl;
                TF1 * fitEtaPtTransPullRestr( new TF1( nameEtaPtTransPullRestrFit.c_str(), "gaus", std::max( histEtaPtTransPullRestr->GetXaxis()->GetXmin(), histEtaPtTransPullRestr->GetMean() - histEtaPtTransPullRestr->GetRMS() * fitRange_ ), std::min( histEtaPtTransPullRestr->GetXaxis()->GetXmax(), histEtaPtTransPullRestr->GetMean() + histEtaPtTransPullRestr->GetRMS() * fitRange_ ) ) );
                setParametersFit( fitEtaPtTransPullRestr, histEtaPtTransPullRestr );
                TFitResultPtr fitEtaPtTransPullRestrResultPtr( histEtaPtTransPullRestr->Fit( fitEtaPtTransPullRestr, fitOptions_.c_str() ) );
                if ( fitEtaPtTransPullRestrResultPtr >= 0 ) {
                  if ( fitEtaPtTransPullRestrResultPtr->Status() == 0 && fitEtaPtTransPullRestrResultPtr->Ndf() != 0. ) {
                    for ( unsigned uPar = 0; uPar < 3; ++uPar ) {
                      histVecTransPullRestrEtaPtFitMap.at( uPar )->SetBinContent( uPt + 1, fitEtaPtTransPullRestrResultPtr->Parameter( uPar ) );
                      histVecTransPullRestrEtaPtFitMap.at( uPar )->SetBinError( uPt + 1, fitEtaPtTransPullRestrResultPtr->ParError( uPar ) );
                    }
                    if ( fitEtaPtTransPullRestrResultPtr->Prob() > 0 ) histTransPullRestrEtaPtFitMapProb->SetBinContent( uPt + 1, log10( fitEtaPtTransPullRestrResultPtr->Prob() ) );
                    else histTransPullRestrEtaPtFitMapProb->SetBinContent( uPt + 1, 1 );
                  }
                  else {
                    if ( verbose_ > 2 ) {
                      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                                << "    failing fit in directory '"; dirEta_->pwd();
                      std::cout << "    '" << nameEtaPtTransPullRestr << "' status " << fitEtaPtTransPullRestrResultPtr->Status() << std::endl;
                    }
                  }
                }
                else {
                  if ( verbose_ > 1 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    missing fit in directory '"; dirEta_->pwd();
                    std::cout << "    '" << nameEtaPtTransPullRestr << std::endl;
                  }
                }
                if ( plot_ ) {
                  histEtaPtTransPullRestr->Draw();
                  c1.Print( std::string( pathPlots_ + histEtaPtTransPullRestr->GetName() + ".png" ).c_str() );
                }
              }

            } // loop: uPt < nPtBins_

            if ( plot_ ) {
              for ( unsigned uPar = 0; uPar < 3; ++uPar ) {
                if ( fitNonRestr_ ) {
                  histVecTransPullEtaPtFitMap.at( uPar )->Draw();
                  c1.Print( std::string( pathPlots_ + histVecTransPullEtaPtFitMap.at( uPar )->GetName() + ".png" ).c_str() );
                }
                histVecTransPullRestrEtaPtFitMap.at( uPar )->Draw();
                c1.Print( std::string( pathPlots_ + histVecTransPullRestrEtaPtFitMap.at( uPar )->GetName() + ".png" ).c_str() );
              }
              if ( fitNonRestr_ ) {
                histTransPullEtaPtFitMapProb->Draw();
                c1.Print( std::string( pathPlots_ + histTransPullEtaPtFitMapProb->GetName() + ".png" ).c_str() );
              }
              histTransPullRestrEtaPtFitMapProb->Draw();
              c1.Print( std::string( pathPlots_ + histTransPullRestrEtaPtFitMapProb->GetName() + ".png" ).c_str() );
            }

          }

          dirEta_->cd();

        } // loop: keyEta

        if ( plot_ ) {
          for ( unsigned uPar = 0; uPar < 3; ++uPar ) {
            if ( fitNonRestr_ ) {
              histVecTransPullEtaFitMap.at( uPar )->Draw();
              c1.Print( std::string( pathPlots_ + histVecTransPullEtaFitMap.at( uPar )->GetName() + ".png" ).c_str() );
            }
            histVecTransPullRestrEtaFitMap.at( uPar )->Draw();
            c1.Print( std::string( pathPlots_ + histVecTransPullRestrEtaFitMap.at( uPar )->GetName() + ".png" ).c_str() );
          }
          if ( fitNonRestr_ ) {
            histTransPullEtaFitMapProb->Draw();
            c1.Print( std::string( pathPlots_ + histTransPullEtaFitMapProb->GetName() + ".png" ).c_str() );
          }
          histTransPullRestrEtaFitMapProb->Draw();
          c1.Print( std::string( pathPlots_ + histTransPullRestrEtaFitMapProb->GetName() + ".png" ).c_str() );
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
void setParametersFit( TF1 * fit, TH1D * histo )
{
  //. This function assumes fit functions of the forms
  // - [0]*exp(-0.5*((x-[1])/[2])**2)/([2]*sqrt(2*pi)) (single Gaissian)
  // Starting points
  Double_t c( histo->GetSumOfWeights() );
  Double_t m( histo->GetMean() );
  Double_t s( histo->GetRMS() );
  // Constant
  fit->SetParameter( 0, c );
  fit->SetParLimits( 0, 0., 2. * c * s );
  fit->SetParName( 0, "Gaussian c" );
  // Mean
  fit->SetParameter( 1, m );
  fit->SetParLimits( 1, -1. * s, 1. * s );
  fit->SetParName( 1, "Gaussian #mu" );
  // Sigma
  fit->SetParameter( 2, s );
  fit->SetParLimits( 2, 0., 2. * s );
  fit->SetParName( 2, "Gaussian #sigma" );
}
