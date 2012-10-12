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
// #include "TopQuarkAnalysis/TopMassSemiLeptonic/interface/MyTools.h"
#include "TopQuarkAnalysis/TopMassSemiLeptonic/interface/TransferFunction.h" // DEBUG


// Initialise parameters for fit function
void setParametersFit( TF1 * fit, TH1D * histo, bool useBkgFunction = false );
// Initialise parametrs for background function
void setParametersBkg( TF1 * fit, TH1D * histo );


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
  // Configuration for fitting transfer functions
  const edm::ParameterSet & fit_( process_.getParameter< edm::ParameterSet >( "fit" ) );
  const bool fitNonRestr_( fit_.getParameter< bool >( "fitNonRestr" ) );
  const bool fitEtaBins_( fit_.getParameter< bool >( "fitEtaBins" ) );
  const double minPtGenJet_( fit_.getParameter< double >( "minPtGenJet" ) );
  const double maxDRGenJet_( fit_.getParameter< double >( "maxDRGenJet" ) );
  // Configuration for fitting L5 JECs
  const edm::ParameterSet & jecL5_( process_.getParameter< edm::ParameterSet >( "jecL5" ) );
  const bool doFit_( jecL5_.getParameter< bool >( "doFit" ) );
  const std::string fitFunction_( jecL5_.getParameter< std::string >( "fitFunction" ) );
  std::string fitOptions_( jecL5_.getParameter< std::string >( "fitOptions" ) );
  const double fitRange_( std::min( jecL5_.getParameter< double >( "fitRange" ), widthFactor_ ) );
  const std::string bkgFunction_( jecL5_.getParameter< std::string >( "bkgFunction" ) );
  const bool useBkgFunction_( bkgFunction_ != "" || bkgFunction_ != "0" );
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
  std::string evtSel_( "analyzeHitFit" );
  if ( refSel_ ) evtSel_.append( "Reference" );
  const std::string nameDirClass( "TDirectoryFile" );
  const std::string nameFuncClass( "TF1" );
  if      ( verbose_ < 3 ) fitOptions_.append( "Q" );
  else if ( verbose_ > 4 ) fitOptions_.append( "V" );
  const std::string baseVarL( useAlt_ ? "E" : "p" );
  const std::string baseVarC( useAlt_ ? "E" : "P" );
  const std::string nameVar( useNonT_ ? baseVarC : baseVarC + "t" );
  const std::string titleVar( useNonT_ ? baseVarL : baseVarL + "_{t}" );
  const std::string titleX( refGenJet_ ? titleVar + "^{GenJet} (GeV)" : titleVar + " (GeV)" );
  const std::string titleEta( refGenJet_ ? "#eta^{GenJet}" : "#eta" );
  const std::string titleFracL5( refGenJet_ ? "#frac{" + titleVar + "^{RECO}}{" + titleVar + "^{GenJet}}" : "#frac{" + titleVar + "^{GenJet}}{" + titleVar + "^{RECO}}" );
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
      data_->SetBranchAddress( "PtAltGenJet" , &ptGenJetData );
      data_->SetBranchAddress( "EtaAltGenJet", &etaGenJetData );
      data_->SetBranchAddress( "PhiAltGenJet", &phiGenJetData );
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

      std::string name( objCat + "_" + nameVar + "_" + subFit );

      const std::string nameFracL5( name + "_FracL5" );
      TH1D * histFracL5( new TH1D( nameFracL5.c_str(), objCat.c_str(), histBins_, 0., histMax_ ) );
      histFracL5->SetXTitle( titleFracL5.c_str() );
      histFracL5->SetYTitle( titleEvents.c_str() );

      const std::string nameFracL5Restr( nameFracL5 + "Restr" );
      TH1D * histFracL5Restr( new TH1D( nameFracL5Restr.c_str(), objCat.c_str(), histBins_, 0., histMax_ ) );
      histFracL5Restr->SetXTitle( titleFracL5.c_str() );
      histFracL5Restr->SetYTitle( titleEvents.c_str() );

      std::vector< TH1D * > histVecPtFracL5;
      std::vector< TH1D * > histVecPtFracL5Restr;
      for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
        const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
        const std::string namePt( name + "_" + nameVar + binPt );
        const std::string namePtFracL5( namePt + "_FracL5" );
        const std::string titlePtFracL5( objCat + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq " + titleVar + " < " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
        TH1D * histPtFracL5( new TH1D( namePtFracL5.c_str(), titlePtFracL5.c_str(), histBins_, 0., histMax_ ) );
        histPtFracL5->SetXTitle( titleFracL5.c_str() );
        histPtFracL5->SetYTitle( titleEvents.c_str() );
        histVecPtFracL5.push_back( histPtFracL5 );
        const std::string namePtFracL5Restr( namePtFracL5 + "Restr" );
        TH1D * histPtFracL5Restr( new TH1D( namePtFracL5Restr.c_str(), titlePtFracL5.c_str(), histBins_, 0., histMax_ ) );
        histPtFracL5Restr->SetXTitle( titleFracL5.c_str() );
        histPtFracL5Restr->SetYTitle( titleEvents.c_str() );
        histVecPtFracL5Restr.push_back( histPtFracL5Restr );
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

        const std::string nameEtaFracL5Restr( nameEtaFracL5 + "Restr" );
        TH1D * histEtaFracL5Restr( new TH1D( nameEtaFracL5Restr.c_str(), titleEtaFracL5.c_str(), histBins_, 0., histMax_ ) );
        histEtaFracL5Restr->SetXTitle( titleFracL5.c_str() );
        histEtaFracL5Restr->SetYTitle( titleEvents.c_str() );

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
                        << "    empty bin " << uPt << " in '" << nameEta << "' for " + titleVar + " bin " << uPt  << std::endl;
            }
          }

          const std::string nameEtaPt( nameEta + "_" + nameVar + binPt );

          const std::string nameEtaPtFracL5( nameEtaPt + "_FracL5" );
          const std::string titleEtaPtFracL5( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta < " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq " + titleVar + " < " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
          TH1D * histEtaPtFracL5( new TH1D( nameEtaPtFracL5.c_str(), titleEtaPtFracL5.c_str(), histBins_, 0., histMax_  ) );
          histEtaPtFracL5->SetXTitle( titleFracL5.c_str() );
          histEtaPtFracL5->SetYTitle( titleEvents.c_str() );

          const std::string nameEtaPtFracL5Restr( nameEtaPtFracL5 + "Restr" );
          TH1D * histEtaPtFracL5Restr( new TH1D( nameEtaPtFracL5Restr.c_str(), titleEtaPtFracL5.c_str(), histBins_, 0., histMax_  ) );
          histEtaPtFracL5Restr->SetXTitle( titleFracL5.c_str() );
          histEtaPtFracL5Restr->SetYTitle( titleEvents.c_str() );

          for ( unsigned uEntry = 0; uEntry < sizePt.at( uPt ); ++uEntry ) {
            if ( refGenJet_ ) {
              if ( fitNonRestr_ ) {
                histEtaPtFracL5->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenJetEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histVecPtFracL5.at( uPt )->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenJetEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaFracL5->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenJetEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histFracL5->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenJetEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
              if ( ptGenJetEtaBin.at( uPt ).at( uEntry ) >= minPtGenJet_ && reco::deltaR( etaGenJetEtaBin.at( uPt ).at( uEntry ), phiGenJetEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDRGenJet_ ) {
                histEtaPtFracL5Restr->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenJetEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histVecPtFracL5Restr.at( uPt )->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenJetEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaFracL5Restr->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenJetEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histFracL5Restr->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenJetEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
            }
            else {
              if ( fitNonRestr_ ) {
                histEtaPtFracL5->Fill( ptGenJetEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histVecPtFracL5.at( uPt )->Fill( ptGenJetEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaFracL5->Fill( ptGenJetEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histFracL5->Fill( ptGenJetEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
              if ( ptGenJetEtaBin.at( uPt ).at( uEntry ) >= minPtGenJet_ && reco::deltaR( etaGenJetEtaBin.at( uPt ).at( uEntry ), phiGenJetEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDRGenJet_ ) {
                histEtaPtFracL5Restr->Fill( ptGenJetEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histVecPtFracL5Restr.at( uPt )->Fill( ptGenJetEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaFracL5Restr->Fill( ptGenJetEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histFracL5Restr->Fill( ptGenJetEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
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
      TCanvas c1( "c1" );
      c1.cd();

      // Loop over fit versions
      nextInListProp.Reset();
      while ( TKey * keyFit = ( TKey* )nextInListProp() ) {
        if ( std::string( keyFit->GetClassName() ) != nameDirClass ) continue;
        const std::string subFit( keyFit->GetName() );
        if ( subFit.find( "Inv" ) != std::string::npos ) continue; // nothing to do for inverse
        // These are real switches: depending on configuration, only one setting combination can be run at a time
        if ( useAlt_  == ( subFit.find( "Alt" )  == std::string::npos ) ) continue;
        if ( useSymm_ == ( subFit.find( "Symm" ) == std::string::npos ) ) continue;
        if ( refGenJet_  == ( subFit.find( "Gen" )  == std::string::npos ) ) continue;
        TDirectory * dirFit_( ( TDirectory* )( dirPt_->Get( subFit.c_str() ) ) );
        TDirectory * dirOutFit_( ( TDirectory* )( dirOutPt_->Get( subFit.c_str() ) ) );
        if ( ! dirOutFit_ ) {
          dirOutPt_->cd();
          dirOutFit_ = new TDirectory( subFit.c_str(), "" );
        }
        dirFit_->cd();

        std::string name( objCat + "_" + nameVar + "_" + subFit );

        // JEC L5 parameters
        const std::string type( refGenJet_ ? "_genJet" : "_recoJet" );
        std::stringstream comment( std::ios_base::out );
        my::TransferFunction transferPt( fitFunction_, bkgFunction_, std::string( titleVar + type ) );
        comment << nameVar << type;
        transferPt.SetComment( comment.str() );
        my::TransferFunctionCollection transferVecEtaPt( nEtaBins_, transferPt );
        my::TransferFunction transferPtRestr( transferPt );
        comment << ", " << nameVar << type << " >= " << minPtGenJet_ << ", " << "DeltaR(genJet, recoJet) <= " << maxDRGenJet_;
        transferPtRestr.SetComment( comment.str() );
        my::TransferFunctionCollection transferVecEtaPtRestr( nEtaBins_, transferPtRestr );

        const unsigned nPar( transferPt.NParFit() );

        const std::string nameFracL5( name + "_FracL5" );
        TH1D * histFracL5( ( TH1D* )( dirFit_->Get( nameFracL5.c_str() ) ) );
        if ( fitNonRestr_ && histFracL5 != 0 ) {
          const std::string nameFracL5Fit( nameFracL5 + "_fit" );
          TF1 * fitFracL5( new TF1( nameFracL5Fit.c_str(), fitFunction_.c_str(), 0., std::min( histFracL5->GetXaxis()->GetXmax(), histFracL5->GetMean() + histFracL5->GetRMS() * fitRange_ ) ) );
          setParametersFit( fitFracL5, histFracL5 );
          TFitResultPtr fitFracL5ResultPtr( histFracL5->Fit( fitFracL5, fitOptions_.c_str() ) );
          if ( fitFracL5ResultPtr >= 0 ) {
            if ( fitFracL5ResultPtr->Status() == 0 && fitFracL5ResultPtr->Ndf() != 0. ) {
              for ( unsigned uPar = 1; uPar < nPar; ++uPar ) {
                transferPt.SetParameter( uPar, fitFracL5->GetParameter( uPar ) );
              }
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

        const std::string nameFracL5Restr( nameFracL5 + "Restr" );
        TH1D * histFracL5Restr( ( TH1D* )( dirFit_->Get( nameFracL5Restr.c_str() ) ) );
        if ( histFracL5Restr != 0 ) {
          const std::string nameFracL5RestrFit( nameFracL5Restr + "_fit" );
          TF1 * fitFracL5Restr( new TF1( nameFracL5RestrFit.c_str(), fitFunction_.c_str(), 0., std::min( histFracL5Restr->GetXaxis()->GetXmax(), histFracL5Restr->GetMean() + histFracL5Restr->GetRMS() * fitRange_ ) ) );
          setParametersFit( fitFracL5Restr, histFracL5Restr );
          TFitResultPtr fitFracL5RestrResultPtr( histFracL5Restr->Fit( fitFracL5Restr, fitOptions_.c_str() ) );
          if ( fitFracL5RestrResultPtr >= 0 ) {
            if ( fitFracL5RestrResultPtr->Status() == 0 && fitFracL5RestrResultPtr->Ndf() != 0. ) {
              for ( unsigned uPar = 1; uPar < nPar; ++uPar ) {
                transferPtRestr.SetParameter( uPar, fitFracL5Restr->GetParameter( uPar ) );
              }
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

        // Loop over pt bins

        std::vector< TH1D * > histVecFracL5PtFitMap;
        std::vector< TH1D * > histVecFracL5RestrPtFitMap;
        for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
          const std::string parFit( boost::lexical_cast< std::string >( uPar ) );
          const std::string nameFracL5PtFitMap( nameFracL5 + nameVar + "_FitMap_Par" + parFit );
          const std::string titleFracL5PtFitMap( objCat + ", par. " + parFit );
          TH1D * histFracL5PtFitMap( new TH1D( nameFracL5PtFitMap.c_str(), titleFracL5PtFitMap.c_str(), nPtBins_, ptBins_.data() ) );
          histFracL5PtFitMap->SetXTitle( titleX.c_str() );
          histVecFracL5PtFitMap.push_back( histFracL5PtFitMap );
          const std::string nameFracL5RestrPtFitMap( nameFracL5Restr + nameVar + "_FitMap_Par" + parFit );
          TH1D * histFracL5RestrPtFitMap( new TH1D( nameFracL5RestrPtFitMap.c_str(), titleFracL5PtFitMap.c_str(), nPtBins_, ptBins_.data() ) );
          histFracL5RestrPtFitMap->SetXTitle( titleX.c_str() );
          histVecFracL5RestrPtFitMap.push_back( histFracL5RestrPtFitMap );
        }
        const std::string titleFracL5PtFitMapProb( objCat + ", fit prob." );
        const std::string nameFracL5PtFitMapProb( nameFracL5 + nameVar + "_FitMap_Prob" );
        TH1D * histFracL5PtFitMapProb( new TH1D( nameFracL5PtFitMapProb.c_str(), titleFracL5PtFitMapProb.c_str(), nPtBins_, ptBins_.data() ) );
        histFracL5PtFitMapProb->SetXTitle( titleX.c_str() );
        histFracL5PtFitMapProb->SetYTitle( titleProb.c_str() );
        const std::string nameFracL5RestrPtFitMapProb( nameFracL5Restr + nameVar + "_FitMap_Prob" );
        TH1D * histFracL5RestrPtFitMapProb( new TH1D( nameFracL5RestrPtFitMapProb.c_str(), titleFracL5PtFitMapProb.c_str(), nPtBins_, ptBins_.data() ) );
        histFracL5RestrPtFitMapProb->SetXTitle( titleX.c_str() );
        histFracL5RestrPtFitMapProb->SetYTitle( titleProb.c_str() );

        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
          const std::string namePt( name + "_" + nameVar + binPt );

          const std::string namePtFracL5( namePt + "_FracL5" );
          TH1D * histPtFracL5( ( TH1D* )( dirFit_->Get( namePtFracL5.c_str() ) ) );
          if ( fitNonRestr_ && histPtFracL5 != 0 ) {
            const std::string namePtFracL5Fit( namePtFracL5 + "_fit" );
            TF1 * fitPtFracL5( new TF1( namePtFracL5Fit.c_str(), fitFunction_.c_str(), 0., std::min( histPtFracL5->GetXaxis()->GetXmax(), histPtFracL5->GetMean() + histPtFracL5->GetRMS() * fitRange_ ) ) );
            setParametersFit( fitPtFracL5, histPtFracL5 );
            TFitResultPtr fitPtFracL5ResultPtr( histPtFracL5->Fit( fitPtFracL5, fitOptions_.c_str() ) );
            if ( fitPtFracL5ResultPtr >= 0 ) {
              if ( fitPtFracL5ResultPtr->Status() == 0 && fitPtFracL5ResultPtr->Ndf() != 0. ) {
                for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
                  histVecFracL5PtFitMap.at( uPar )->SetBinContent( uPt + 1, fitPtFracL5ResultPtr->Parameter( uPar ) );
                  histVecFracL5PtFitMap.at( uPar )->SetBinError( uPt + 1, fitPtFracL5ResultPtr->ParError( uPar ) );
                }
                if ( fitPtFracL5ResultPtr->Prob() > 0 ) histFracL5PtFitMapProb->SetBinContent( uPt + 1, log10( fitPtFracL5ResultPtr->Prob() ) );
                else histFracL5PtFitMapProb->SetBinContent( uPt + 1, 1 );
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
            TF1 * fitPtFracL5Restr( new TF1( namePtFracL5RestrFit.c_str(), fitFunction_.c_str(), 0., std::min( histPtFracL5Restr->GetXaxis()->GetXmax(), histPtFracL5Restr->GetMean() + histPtFracL5Restr->GetRMS() * fitRange_ ) ) );
            setParametersFit( fitPtFracL5Restr, histPtFracL5Restr );
            TFitResultPtr fitPtFracL5RestrResultPtr( histPtFracL5Restr->Fit( fitPtFracL5Restr, fitOptions_.c_str() ) );
            if ( fitPtFracL5RestrResultPtr >= 0 ) {
              if ( fitPtFracL5RestrResultPtr->Status() == 0 && fitPtFracL5RestrResultPtr->Ndf() != 0. ) {
                for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
                  histVecFracL5RestrPtFitMap.at( uPar )->SetBinContent( uPt + 1, fitPtFracL5RestrResultPtr->Parameter( uPar ) );
                  histVecFracL5RestrPtFitMap.at( uPar )->SetBinError( uPt + 1, fitPtFracL5RestrResultPtr->ParError( uPar ) );
                }
                if ( fitPtFracL5RestrResultPtr->Prob() > 0 ) histFracL5RestrPtFitMapProb->SetBinContent( uPt + 1, log10( fitPtFracL5RestrResultPtr->Prob() ) );
                else histFracL5RestrPtFitMapProb->SetBinContent( uPt + 1, 1 );
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

        // Loop over eta bins

        std::vector< TH1D * > histVecFracL5EtaFitMap;
        std::vector< TH1D * > histVecFracL5RestrEtaFitMap;
        for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
          const std::string parFit( boost::lexical_cast< std::string >( uPar ) );
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
        if ( fitEtaBins_ ) {
          for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
            const std::string parFit( boost::lexical_cast< std::string >( uPar ) );
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

          const std::string nameEtaFracL5( nameEta + "_FracL5" );
          if ( fitNonRestr_ ) {
            TH1D * histEtaFracL5( ( TH1D* )( dirEta_->Get( nameEtaFracL5.c_str() ) ) );
            if ( fitNonRestr_ && histEtaFracL5 != 0 ) {
              const std::string nameEtaFracL5Fit( nameEtaFracL5 + "_fit" );
              TF1 * fitEtaFracL5( new TF1( nameEtaFracL5Fit.c_str(), fitFunction_.c_str(), 0., std::min( histEtaFracL5->GetXaxis()->GetXmax(), histEtaFracL5->GetMean() + histEtaFracL5->GetRMS() * fitRange_ ) ) );
              setParametersFit( fitEtaFracL5, histEtaFracL5 );
              TFitResultPtr fitEtaFracL5ResultPtr( histEtaFracL5->Fit( fitEtaFracL5, fitOptions_.c_str() ) );
              if ( fitEtaFracL5ResultPtr >= 0 ) {
                if ( fitEtaFracL5ResultPtr->Status() == 0 && fitEtaFracL5ResultPtr->Ndf() != 0. ) {
                  for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
                    histVecFracL5EtaFitMap.at( uPar )->SetBinContent( uEta + 1, fitEtaFracL5ResultPtr->Parameter( uPar ) );
                    histVecFracL5EtaFitMap.at( uPar )->SetBinError( uEta + 1, fitEtaFracL5ResultPtr->ParError( uPar ) );
                    transferVecEtaPt.at( uEta ).SetParameter( uPar, fitEtaFracL5->GetParameter( uPar ) );
                  }
                  if ( fitEtaFracL5ResultPtr->Prob() > 0 ) histFracL5EtaFitMapProb->SetBinContent( uEta + 1, log10( fitEtaFracL5ResultPtr->Prob() ) );
                  else histFracL5EtaFitMapProb->SetBinContent( uEta + 1, 1 );
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
            TF1 * fitEtaFracL5Restr( new TF1( nameEtaFracL5RestrFit.c_str(), fitFunction_.c_str(), 0., std::min( histEtaFracL5Restr->GetXaxis()->GetXmax(), histEtaFracL5Restr->GetMean() + histEtaFracL5Restr->GetRMS() * fitRange_ ) ) );
            setParametersFit( fitEtaFracL5Restr, histEtaFracL5Restr );
            TFitResultPtr fitEtaFracL5RestrResultPtr( histEtaFracL5Restr->Fit( fitEtaFracL5Restr, fitOptions_.c_str() ) );
            if ( fitEtaFracL5RestrResultPtr >= 0 ) {
              if ( fitEtaFracL5RestrResultPtr->Status() == 0 && fitEtaFracL5RestrResultPtr->Ndf() != 0. ) {
                for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
                  histVecFracL5RestrEtaFitMap.at( uPar )->SetBinContent( uEta + 1, fitEtaFracL5RestrResultPtr->Parameter( uPar ) );
                  histVecFracL5RestrEtaFitMap.at( uPar )->SetBinError( uEta + 1, fitEtaFracL5RestrResultPtr->ParError( uPar ) );
                  transferVecEtaPtRestr.at( uEta ).SetParameter( uPar, fitEtaFracL5Restr->GetParameter( uPar ) );
                }
                if ( fitEtaFracL5RestrResultPtr->Prob() > 0 ) histFracL5RestrEtaFitMapProb->SetBinContent( uEta + 1, log10( fitEtaFracL5RestrResultPtr->Prob() ) );
                else histFracL5RestrEtaFitMapProb->SetBinContent( uEta + 1, 1 );
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

          if ( fitEtaBins_ ) {

            // Loop over pt bins

            std::vector< TH1D * > histVecFracL5EtaPtFitMap;
            std::vector< TH1D * > histVecFracL5RestrEtaPtFitMap;
            for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
              const std::string parFit( boost::lexical_cast< std::string >( uPar ) );
              const std::string nameFracL5EtaPtFitMap( nameEta + "_FracL5" + nameVar + "_FitMap_Par" + parFit );
              const std::string titleFracL5EtaPtFitMap( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta < " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) + ", par. " + parFit );
              TH1D * histFracL5EtaPtFitMap( new TH1D( nameFracL5EtaPtFitMap.c_str(), titleFracL5EtaPtFitMap.c_str(), nPtBins_, ptBins_.data() ) );
              histFracL5EtaPtFitMap->SetXTitle( titleX.c_str() );
              histVecFracL5EtaPtFitMap.push_back( histFracL5EtaPtFitMap );
              const std::string nameFracL5RestrEtaPtFitMap( nameEta + "_FracL5Restr" + nameVar + "_FitMap_Par" + parFit );
              TH1D * histFracL5RestrEtaPtFitMap( new TH1D( nameFracL5RestrEtaPtFitMap.c_str(), titleFracL5EtaPtFitMap.c_str(), nPtBins_, ptBins_.data() ) );
              histFracL5RestrEtaPtFitMap->SetXTitle( titleX.c_str() );
              histVecFracL5RestrEtaPtFitMap.push_back( histFracL5RestrEtaPtFitMap );
            }
            const std::string titleFracL5EtaPtFitMapProb( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta < " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) + ", fit prob." );
            const std::string nameFracL5EtaPtFitMapProb( nameEtaFracL5 + nameVar + "_FitMap_Prob" );
            TH1D * histFracL5EtaPtFitMapProb( new TH1D( nameFracL5EtaPtFitMapProb.c_str(), titleFracL5EtaPtFitMapProb.c_str(), nPtBins_, ptBins_.data() ) );
            histFracL5EtaPtFitMapProb->SetXTitle( titleX.c_str() );
            histFracL5EtaPtFitMapProb->SetYTitle( titleProb.c_str() );
            const std::string nameFracL5RestrEtaPtFitMapProb( nameEtaFracL5Restr + nameVar + "_FitMap_Prob" );
            TH1D * histFracL5RestrEtaPtFitMapProb( new TH1D( nameFracL5RestrEtaPtFitMapProb.c_str(), titleFracL5EtaPtFitMapProb.c_str(), nPtBins_, ptBins_.data() ) );
            histFracL5RestrEtaPtFitMapProb->SetXTitle( titleX.c_str() );
            histFracL5RestrEtaPtFitMapProb->SetYTitle( titleProb.c_str() );

            for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
              const std::string binPt( boost::lexical_cast< std::string >( uPt ) );

              const std::string nameEtaPt( nameEta + "_" + nameVar + binPt );
              const std::string nameEtaPtFracL5( nameEtaPt + "_FracL5" );

              if ( fitNonRestr_ ) {
                TH1D * histEtaPtFracL5( ( TH1D* )( dirEta_->Get( nameEtaPtFracL5.c_str() ) ) );
                if ( histEtaPtFracL5 != 0 ) {
                  const std::string nameEtaPtFracL5Fit( nameEtaPtFracL5 + "_fit" );
                  TF1 * fitEtaPtFracL5( new TF1( nameEtaPtFracL5Fit.c_str(), fitFunction_.c_str(), 0., std::min( histEtaPtFracL5->GetXaxis()->GetXmax(), histEtaPtFracL5->GetMean() + histEtaPtFracL5->GetRMS() * fitRange_ ) ) );
                  setParametersFit( fitEtaPtFracL5, histEtaPtFracL5 );
                  TFitResultPtr fitEtaPtFracL5ResultPtr( histEtaPtFracL5->Fit( fitEtaPtFracL5, fitOptions_.c_str() ) );
                  if ( fitEtaPtFracL5ResultPtr >= 0 ) {
                    if ( fitEtaPtFracL5ResultPtr->Status() == 0 && fitEtaPtFracL5ResultPtr->Ndf() != 0. ) {
                      for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
                        histVecFracL5EtaPtFitMap.at( uPar )->SetBinContent( uPt + 1, fitEtaPtFracL5ResultPtr->Parameter( uPar ) );
                        histVecFracL5EtaPtFitMap.at( uPar )->SetBinError( uPt + 1, fitEtaPtFracL5ResultPtr->ParError( uPar ) );
                      }
                      if ( fitEtaPtFracL5ResultPtr->Prob() > 0 ) histFracL5EtaPtFitMapProb->SetBinContent( uPt + 1, log10( fitEtaPtFracL5ResultPtr->Prob() ) );
                      else histFracL5EtaPtFitMapProb->SetBinContent( uPt + 1, 1 );
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
                TF1 * fitEtaPtFracL5Restr( new TF1( nameEtaPtFracL5RestrFit.c_str(), fitFunction_.c_str(), 0., std::min( histEtaPtFracL5Restr->GetXaxis()->GetXmax(), histEtaPtFracL5Restr->GetMean() + histEtaPtFracL5Restr->GetRMS() * fitRange_ ) ) );
                setParametersFit( fitEtaPtFracL5Restr, histEtaPtFracL5Restr );
                TFitResultPtr fitEtaPtFracL5RestrResultPtr( histEtaPtFracL5Restr->Fit( fitEtaPtFracL5Restr, fitOptions_.c_str() ) );
                if ( fitEtaPtFracL5RestrResultPtr >= 0 ) {
                  if ( fitEtaPtFracL5RestrResultPtr->Status() == 0 && fitEtaPtFracL5RestrResultPtr->Ndf() != 0. ) {
                    for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
                      histVecFracL5RestrEtaPtFitMap.at( uPar )->SetBinContent( uPt + 1, fitEtaPtFracL5RestrResultPtr->Parameter( uPar ) );
                      histVecFracL5RestrEtaPtFitMap.at( uPar )->SetBinError( uPt + 1, fitEtaPtFracL5RestrResultPtr->ParError( uPar ) );
                    }
                    if ( fitEtaPtFracL5RestrResultPtr->Prob() > 0 ) histFracL5RestrEtaPtFitMapProb->SetBinContent( uPt + 1, log10( fitEtaPtFracL5RestrResultPtr->Prob() ) );
                    else histFracL5RestrEtaPtFitMapProb->SetBinContent( uPt + 1, 1 );
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

            if ( plot_ ) {
              for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
                if ( fitNonRestr_ ) {
                  histVecFracL5EtaPtFitMap.at( uPar )->Draw();
                  c1.Print( std::string( pathPlots_ + histVecFracL5EtaPtFitMap.at( uPar )->GetName() + ".png" ).c_str() );
                }
                histVecFracL5RestrEtaPtFitMap.at( uPar )->Draw();
                c1.Print( std::string( pathPlots_ + histVecFracL5RestrEtaPtFitMap.at( uPar )->GetName() + ".png" ).c_str() );
              }
              if ( fitNonRestr_ ) {
                histFracL5EtaPtFitMapProb->Draw();
                c1.Print( std::string( pathPlots_ + histFracL5EtaPtFitMapProb->GetName() + ".png" ).c_str() );
              }
              histFracL5RestrEtaPtFitMapProb->Draw();
              c1.Print( std::string( pathPlots_ + histFracL5RestrEtaPtFitMapProb->GetName() + ".png" ).c_str() );
            }

          }

        } // loop: keyEta

        if ( plot_ ) {
          if ( fitEtaBins_ ) {
            for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
              if ( fitNonRestr_ ) {
                histVecFracL5EtaParAMap.at( uPar )->Draw();
                c1.Print( std::string( pathPlots_ + histVecFracL5EtaParAMap.at( uPar )->GetName() + ".png" ).c_str() );
                histVecFracL5EtaParBMap.at( uPar )->Draw();
                c1.Print( std::string( pathPlots_ + histVecFracL5EtaParBMap.at( uPar )->GetName() + ".png" ).c_str() );
              }
              histVecFracL5RestrEtaParAMap.at( uPar )->Draw();
              c1.Print( std::string( pathPlots_ + histVecFracL5RestrEtaParAMap.at( uPar )->GetName() + ".png" ).c_str() );
              histVecFracL5RestrEtaParBMap.at( uPar )->Draw();
              c1.Print( std::string( pathPlots_ + histVecFracL5RestrEtaParBMap.at( uPar )->GetName() + ".png" ).c_str() );
            }
          }
          for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
            if ( fitNonRestr_ ) {
              histVecFracL5EtaFitMap.at( uPar )->Draw();
              c1.Print( std::string( pathPlots_ + histVecFracL5EtaFitMap.at( uPar )->GetName() + ".png" ).c_str() );
            }
            histVecFracL5RestrEtaFitMap.at( uPar )->Draw();
            c1.Print( std::string( pathPlots_ + histVecFracL5RestrEtaFitMap.at( uPar )->GetName() + ".png" ).c_str() );
          }
          if ( fitNonRestr_ ) {
            histFracL5EtaFitMapProb->Draw();
            c1.Print( std::string( pathPlots_ + histFracL5EtaFitMapProb->GetName() + ".png" ).c_str() );
          }
          histFracL5RestrEtaFitMapProb->Draw();
          c1.Print( std::string( pathPlots_ + histFracL5RestrEtaFitMapProb->GetName() + ".png" ).c_str() );
        }

        if ( writeFiles_ ) {

          // File name
          std::string nameOut( pathOut_ + "/gentJecL5_" + sample_ );
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
  // This function assumes fit functions of the forms
  // -

  // Starting points
  Double_t c( histo->Integral() );                             // Constant
  Double_t m( histo->GetMean() );                              // Mean
  Double_t p( histo->GetBinCenter( histo->GetMaximumBin() ) ); // Peak
  Double_t s( histo->GetRMS() );                               // RMS
  // Gaussian part
  // Constant
  fit->SetParameter( 0, c );
  fit->SetParLimits( 0, 0., 2. * c * s );
  fit->SetParName( 0, "Constant c" );
  // Mean
  fit->SetParameter( 1, p ); // No double peak structure in this case
  fit->SetParLimits( 1, -1. * s, 1. * s );
  fit->SetParName( 1, "Gaussian #mu" );
  // Sigma
  fit->SetParameter( 2, s );
  fit->SetParLimits( 2, 0., 2. * s );
  fit->SetParName( 2, "Gaussian #sigma" );
  // Additional part
  if ( useBkgFunction ) {
    Double_t fitMin, fitMax;
    fit->GetRange( fitMin, fitMax );
  }
}


// Initialise parametrs for background function
void setParametersBkg( TF1 * fit, TH1D * histo )
{
  //. This function assumes background functions of the form
  // - [0]+[1]*x (line)

  // Starting points
  // Bins are away from unstable low/high momentum regions
  Double_t x1( histo->GetBinCenter( 5 ) );
  Double_t y1( histo->GetBinContent( 5 ) );
  Double_t x2( histo->GetBinCenter( histo->GetNbinsX() - 2 ) );
  Double_t y2( histo->GetBinContent( histo->GetNbinsX() - 2 ) );
//   // Linear part
//   // Constant
//   fit->SetParameter( 0, ( x2 * y1 - x1 * y2 ) / ( x2 - x1 ) );
//   fit->SetParName( 0, "Constant a" );
//   // Slope
//   fit->SetParameter( 1, ( y2 - y1 ) / ( x2 - x1 ) );
//   fit->SetParName( 1, "Slope b" );
}
