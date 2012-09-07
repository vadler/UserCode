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
#include "TopQuarkAnalysis/TopMassSemiLeptonic/interface/MyTools.h"


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
              << "    cms.PSet 'process' missing in " << argv[ 1 ] << std::endl;
    returnStatus_ += 0x2;
    return returnStatus_;
  }

  // Get the configurations
  const edm::ParameterSet & process_( edm::readPSetsFrom( argv[ 1 ] )->getParameter< edm::ParameterSet >( "process" ) );
  const unsigned verbose_( process_.getParameter< unsigned >( "verbose" ) );
  const std::vector< std::string > objCats_( process_.getParameter< std::vector< std::string > >( "objectCategories" ) );   // object categories
  const bool overwrite_(  process_.getParameter< bool >( "overwrite" ));
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
  const std::string sample_( io_.getParameter< std::string >( "sample" ) );
  // Configuration for histogram binning
  const edm::ParameterSet & histos_( process_.getParameter< edm::ParameterSet >( "histos" ) );
  const double widthFactor_( histos_.getParameter< double >( "widthFactor" ) );
  // Configuration for fitting transfer functions
  const edm::ParameterSet & fit_( process_.getParameter< edm::ParameterSet >( "fit" ) );
  const bool fitNonRestr_( fit_.getParameter< bool >( "fitNonRestr" ) );
  const bool fitEtaPt_( fit_.getParameter< bool >( "fitEtaPt" ) );
  const double minPtParton_( fit_.getParameter< double >( "minPtParton" ) );
  const double maxDRParton_( fit_.getParameter< double >( "maxDRParton" ) );
  // Configuration for fitting starting transfer functions
  const edm::ParameterSet & transfer1D_( process_.getParameter< edm::ParameterSet >( "transfer1D" ) );
  const bool fit1D_( transfer1D_.getParameter< bool >( "fit" ) );
  const bool plot1D_( transfer1D_.getParameter< bool >( "plot" ) );
  double fitMaxPt1D_( transfer1D_.getParameter< double >( "fitMaxPt" ) );
  const std::string fitFunction1D_( transfer1D_.getParameter< std::string >( "fitFunction" ) );
  std::string fitOptions1D_( transfer1D_.getParameter< std::string >( "fitOptions" ) );
  const double fitRange1D_( std::min( transfer1D_.getParameter< double >( "fitRange" ), widthFactor_ ) );
  const std::string fitTransfer1D_( transfer1D_.getParameter< std::string >( "fitTransfer" ) );
  const bool writeFiles1D_( transfer1D_.getParameter< bool >( "writeFiles" ) );
  const std::string pathOut1D_( transfer1D_.getParameter< std::string >( "pathOut" ) );

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
  if      ( verbose_ < 3 ) fitOptions1D_.append( "Q" );
  else if ( verbose_ > 4 ) fitOptions1D_.append( "V" );
  const std::string nameVar( useNonT_ ? "P" : "Pt" );
  const std::string titleVar( useNonT_ ? "p" : "p_{t}" );
  const std::string titlePt( refGen_ ? "p_{t}^{GEN} (GeV)" : "p_{t} (GeV)" );
  const std::string titleEta( refGen_ ? "#eta^{GEN}" : "#eta" );
  const std::string titleTrans( refGen_ ? "#Delta" + titleVar + "^{GEN} (GeV)" : "#Delta" + titleVar + "^{RECO} (GeV)" );
  const std::string titleTransMean( "#mu of " + titleTrans );
  const std::string titleTransNorm( "c of " + titleTrans );
  const std::string titleTransSigma( "#sigma of " + titleTrans );
  const std::string titleEvents( "events" );
  const std::string titleFits( "fits" );


  // Open input file

  if ( verbose_ > 0 )
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "    using input file '" << inFile_ << "'" << std::endl;

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

  // Loops through directory structure

  typedef std::vector< std::vector< Double_t > > DataCont;

  // Loop over configured object categories
  for ( unsigned uCat = 0; uCat < objCats_.size(); ++uCat ) {
    const std::string objCat( objCats_.at( uCat ) );
    if ( objCat != "UdscJet" && objCat != "BJet" ) continue;
    TDirectory * dirCat_( ( TDirectory* )( dirSel_->Get( objCat.c_str() ) ) );
    if ( ! dirCat_ ) {
      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                << "    object category '" << objCat << "' does not exist in input file" << std::endl;
      returnStatus_ += 0x100;
      continue;
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
    if ( fitMaxPt1D_ > ptBins_.back() ) fitMaxPt1D_ = ptBins_.back();

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
    std::vector< unsigned > sizeEta_( nEtaBins_ );
    for ( Int_t iEntry = 0; iEntry < nEntries; ++iEntry ) {
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

    // Histogram binning
    const unsigned histBins_( histos_.getParameter< unsigned >( std::string( objCat + "PtBins" ) ) );
    const double   histMax_( histos_.getParameter< double >( std::string( objCat + "PtMax" ) ) );

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

      const std::string name( objCat + "_" + nameVar + "_" + subFit );

      const std::string nameTrans( name + "_Trans" );
      TH1D * histTrans( new TH1D( nameTrans.c_str(), objCat.c_str(), histBins_, -histMax_, histMax_ ) );
      histTrans->SetXTitle( titleTrans.c_str() );
      histTrans->SetYTitle( titleEvents.c_str() );

      const std::string nameTransRestr( nameTrans + "Restr" );
      TH1D * histTransRestr( new TH1D( nameTransRestr.c_str(), objCat.c_str(), histBins_, -histMax_, histMax_ ) );
      histTransRestr->SetXTitle( titleTrans.c_str() );
      histTransRestr->SetYTitle( titleEvents.c_str() );

      std::vector< TH1D * > histVecPtTrans;
      std::vector< TH1D * > histVecPtTransRestr;
      for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
        const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
        const std::string namePt( name + "_Pt" + binPt );
        const std::string namePtTrans( namePt + "_Trans" );
        const std::string titlePtTrans( objCat + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq p_{t} < " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
        TH1D * histPtTrans( new TH1D( namePtTrans.c_str(), titlePtTrans.c_str(), histBins_, -histMax_, histMax_ ) );
        histPtTrans->SetXTitle( titleTrans.c_str() );
        histPtTrans->SetYTitle( titleEvents.c_str() );
        histVecPtTrans.push_back( histPtTrans );
        const std::string namePtTransRestr( namePtTrans + "Restr" );
        TH1D * histPtTransRestr( new TH1D( namePtTransRestr.c_str(), titlePtTrans.c_str(), histBins_, -histMax_, histMax_ ) );
        histPtTransRestr->SetXTitle( titleTrans.c_str() );
        histPtTransRestr->SetYTitle( titleEvents.c_str() );
        histVecPtTransRestr.push_back( histPtTransRestr );
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

        const std::string nameEtaTrans( nameEta + "_Trans" );
        const std::string titleEtaTrans( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta < " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) );
        TH1D * histEtaTrans( new TH1D( nameEtaTrans.c_str(), titleEtaTrans.c_str(), histBins_, -histMax_, histMax_ ) );
        histEtaTrans->SetXTitle( titleTrans.c_str() );
        histEtaTrans->SetYTitle( titleEvents.c_str() );

        const std::string nameEtaTransRestr( nameEtaTrans + "Restr" );
        TH1D * histEtaTransRestr( new TH1D( nameEtaTransRestr.c_str(), titleEtaTrans.c_str(), histBins_, -histMax_, histMax_ ) );
        histEtaTransRestr->SetXTitle( titleTrans.c_str() );
        histEtaTransRestr->SetYTitle( titleEvents.c_str() );

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
          for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
            if ( ptBins_.at( uPt ) <= ptData_.at( uEta ).at( uEntry ) && ptData_.at( uEta ).at( uEntry ) < ptBins_.at( uPt + 1 ) ) {
              sizePt.at( uPt ) += 1;
              Double_t ptVal( useNonT_ ? ptData_.at( uEta ).at( uEntry ) * std::cosh( etaData_.at( uEta ).at( uEntry ) ) : ptData_.at( uEta ).at( uEntry ) );
              Double_t ptGenVal( useNonT_ ? ptGenData_.at( uEta ).at( uEntry ) * std::cosh( etaGenData_.at( uEta ).at( uEntry ) ) : ptGenData_.at( uEta ).at( uEntry ) );
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
          if ( sizePt.at( uPt ) == 0 ) {
            if ( verbose_ > 2 ) {
              std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                        << "    empty bin in '" << nameEta << "' for p_t bin " << uPt  << std::endl;
            }
          }

          for ( unsigned uEntry = 0; uEntry < sizePt.at( uPt ); ++uEntry ) {
            if ( ptEtaBin.at( uPt ).at( uEntry ) == -9. || ptGenEtaBin.at( uPt ).at( uEntry ) == -9. ) continue; // no match
            if ( refGen_ ) {
              if ( fitNonRestr_ ) {
                histVecPtTrans.at( uPt )->Fill( ptGenEtaBin.at( uPt ).at( uEntry ) - ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaTrans->Fill( ptGenEtaBin.at( uPt ).at( uEntry ) - ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histTrans->Fill( ptGenEtaBin.at( uPt ).at( uEntry ) - ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
              if ( ptGenEtaBin.at( uPt ).at( uEntry ) >= minPtParton_ && reco::deltaR( etaGenEtaBin.at( uPt ).at( uEntry ), phiGenEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDRParton_ ) {
                histVecPtTransRestr.at( uPt )->Fill( ptGenEtaBin.at( uPt ).at( uEntry ) - ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaTransRestr->Fill( ptGenEtaBin.at( uPt ).at( uEntry ) - ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histTransRestr->Fill( ptGenEtaBin.at( uPt ).at( uEntry ) - ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
            }
            else {
              if ( fitNonRestr_ ) {
                histVecPtTrans.at( uPt )->Fill( ptEtaBin.at( uPt ).at( uEntry ) - ptGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaTrans->Fill( ptEtaBin.at( uPt ).at( uEntry ) - ptGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histTrans->Fill( ptEtaBin.at( uPt ).at( uEntry ) - ptGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
              if ( ptGenEtaBin.at( uPt ).at( uEntry ) >= minPtParton_ && reco::deltaR( etaGenEtaBin.at( uPt ).at( uEntry ), phiGenEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDRParton_ ) {
                histVecPtTransRestr.at( uPt )->Fill( ptEtaBin.at( uPt ).at( uEntry ) - ptGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaTransRestr->Fill( ptEtaBin.at( uPt ).at( uEntry ) - ptGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histTransRestr->Fill( ptEtaBin.at( uPt ).at( uEntry ) - ptGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
            }
          } // loop: uEntry < ptEtaBin.at( uPt ).size()

          if ( fitEtaPt_ ) {
            const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
            const std::string nameEtaPt( nameEta + "_Pt" + binPt );

            const std::string nameEtaPtTrans( nameEtaPt + "_Trans" );
            const std::string titleEtaPtTrans( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta < " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq p_{t} < " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
            TH1D * histEtaPtTrans( new TH1D( nameEtaPtTrans.c_str(), titleEtaPtTrans.c_str(), histBins_, -histMax_, histMax_  ) );
            histEtaPtTrans->SetXTitle( titleTrans.c_str() );
            histEtaPtTrans->SetYTitle( titleEvents.c_str() );

            const std::string nameEtaPtTransRestr( nameEtaPtTrans + "Restr" );
            TH1D * histEtaPtTransRestr( new TH1D( nameEtaPtTransRestr.c_str(), titleEtaPtTrans.c_str(), histBins_, -histMax_, histMax_  ) );
            histEtaPtTransRestr->SetXTitle( titleTrans.c_str() );
            histEtaPtTransRestr->SetYTitle( titleEvents.c_str() );

            for ( unsigned uEntry = 0; uEntry < sizePt.at( uPt ); ++uEntry ) {
              if ( ptData_.at( uEta ).at( uEntry ) == -9. || ptGenData_.at( uEta ).at( uEntry ) == -9. ) continue; // no match
              if ( refGen_ ) {
                if ( fitNonRestr_ ) {
                  histEtaPtTrans->Fill( ptGenEtaBin.at( uPt ).at( uEntry ) - ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                }
                if ( ptGenEtaBin.at( uPt ).at( uEntry ) >= minPtParton_ && reco::deltaR( etaGenEtaBin.at( uPt ).at( uEntry ), phiGenEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDRParton_ ) {
                  histEtaPtTransRestr->Fill( ptGenEtaBin.at( uPt ).at( uEntry ) - ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                }
              }
              else {
                if ( fitNonRestr_ ) {
                  histEtaPtTrans->Fill( ptEtaBin.at( uPt ).at( uEntry ) - ptGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                }
                if ( ptGenEtaBin.at( uPt ).at( uEntry ) >= minPtParton_ && reco::deltaR( etaGenEtaBin.at( uPt ).at( uEntry ), phiGenEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDRParton_ ) {
                  histEtaPtTransRestr->Fill( ptEtaBin.at( uPt ).at( uEntry ) - ptGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                }
              }
            } // loop: uEntry < ptEtaBin.at( uPt ).size()

            const std::string nameEtaPtTransRebin( nameEtaPtTrans + "Rebin" );
            const Double_t meanEtaPtTrans( histEtaPtTrans->GetMean() );
            const Double_t widthEtaPtTrans( std::fabs( histEtaPtTrans->GetRMS() ) );
            if ( widthEtaPtTrans == 0. && verbose_ > 2 ) {
              std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                        << "    no histogram \"width\" in '" << nameEtaPtTrans << "'" << std::endl;
            }
            const Double_t rangeEtaPtTransRebin( widthEtaPtTrans == 0. ? widthFactor_ * std::fabs( histEtaPtTrans->GetXaxis()->GetXmax() ) : widthFactor_ * widthEtaPtTrans ); // FIXME: tune, incl. under- and overflow, remove hard-coding
            TH1D * histEtaPtTransRebin( new TH1D( nameEtaPtTransRebin.c_str(), titleEtaPtTrans.c_str(), histBins_, -rangeEtaPtTransRebin + meanEtaPtTrans, rangeEtaPtTransRebin + meanEtaPtTrans  ) );
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
            TH1D * histEtaPtTransRestrRebin( new TH1D( nameEtaPtTransRestrRebin.c_str(), titleEtaPtTrans.c_str(), histBins_, -rangeEtaPtTransRestrRebin + meanEtaPtTransRestr, rangeEtaPtTransRestrRebin + meanEtaPtTransRestr  ) );
            histEtaPtTransRestrRebin->SetXTitle( titleTrans.c_str() );
            histEtaPtTransRestrRebin->SetYTitle( titleEvents.c_str() );

            for ( unsigned uEntry = 0; uEntry < sizePt.at( uPt ); ++uEntry ) {
              if ( ptEtaBin.at( uPt ).at( uEntry ) == -9. || ptGenEtaBin.at( uPt ).at( uEntry ) == -9. ) continue; // no match
              if ( refGen_ ) {
                if ( fitNonRestr_ ) {
                  histEtaPtTrans->Fill( ptGenEtaBin.at( uPt ).at( uEntry ) - ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                }
                if ( ptGenEtaBin.at( uPt ).at( uEntry ) >= minPtParton_ && reco::deltaR( etaGenEtaBin.at( uPt ).at( uEntry ), phiGenEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDRParton_ ) {
                  histEtaPtTransRestr->Fill( ptGenEtaBin.at( uPt ).at( uEntry ) - ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                }
              }
              else {
                if ( fitNonRestr_ ) {
                  histEtaPtTrans->Fill( ptEtaBin.at( uPt ).at( uEntry ) - ptGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                }
                if ( ptGenEtaBin.at( uPt ).at( uEntry ) >= minPtParton_ && reco::deltaR( etaGenEtaBin.at( uPt ).at( uEntry ), phiGenEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDRParton_ ) {
                  histEtaPtTransRestr->Fill( ptEtaBin.at( uPt ).at( uEntry ) - ptGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                }
              }
            } // loop: uEntry < ptEtaBin.at( uPt ).size()
          }

        } // loop: uPt < nPtBins_

        const std::string nameEtaTransRebin( nameEtaTrans + "Rebin" );
        const Double_t meanEtaTrans( histEtaTrans->GetMean() );
        const Double_t widthEtaTrans( std::fabs( histEtaTrans->GetRMS() ) );
        if ( widthEtaTrans == 0. && verbose_ > 2 ) {
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    no histogram \"width\" in '" << nameEtaTrans << "'" << std::endl;
        }
        const Double_t rangeEtaTransRebin( widthEtaTrans == 0. ? widthFactor_ * std::fabs( histEtaTrans->GetXaxis()->GetXmax() ) : widthFactor_ * widthEtaTrans ); // FIXME: tune, incl. under- and overflow, remove hard-coding
        TH1D * histEtaTransRebin( new TH1D( nameEtaTransRebin.c_str(), titleEtaTrans.c_str(), histBins_, -rangeEtaTransRebin + meanEtaTrans, rangeEtaTransRebin + meanEtaTrans  ) );
        histEtaTransRebin->SetXTitle( titleTrans.c_str() );
        histEtaTransRebin->SetYTitle( titleEvents.c_str() );

        const std::string nameEtaTransRestrRebin( nameEtaTransRestr + "Rebin" );
        const Double_t meanEtaTransRestr( histEtaTransRestr->GetMean() );
        const Double_t widthEtaTransRestr( std::fabs( histEtaTransRestr->GetRMS() ) );
        if ( widthEtaTransRestr == 0. && verbose_ > 2 ) {
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    no histogram \"width\" in '" << nameEtaTransRestr << "'" << std::endl;
        }
        const Double_t rangeEtaTransRestrRebin( widthEtaTransRestr == 0. ? widthFactor_ * std::fabs( histEtaTransRestr->GetXaxis()->GetXmax() ) : widthFactor_ * widthEtaTransRestr ); // FIXME: tune, incl. under- and overflow, remove hard-coding
        TH1D * histEtaTransRestrRebin( new TH1D( nameEtaTransRestrRebin.c_str(), titleEtaTrans.c_str(), histBins_, -rangeEtaTransRestrRebin + meanEtaTransRestr, rangeEtaTransRestrRebin + meanEtaTransRestr  ) );
        histEtaTransRestrRebin->SetXTitle( titleTrans.c_str() );
        histEtaTransRestrRebin->SetYTitle( titleEvents.c_str() );

        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          for ( unsigned uEntry = 0; uEntry < sizePt.at( uPt ); ++uEntry ) {
            if ( ptEtaBin.at( uPt ).at( uEntry ) == -9. || ptGenEtaBin.at( uPt ).at( uEntry ) == -9. ) continue; // no match
            if ( refGen_ ) {
              if ( fitNonRestr_ ) {
                histEtaTransRebin->Fill( ptGenEtaBin.at( uPt ).at( uEntry ) - ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
              if ( ptGenEtaBin.at( uPt ).at( uEntry ) >= minPtParton_ && reco::deltaR( etaGenEtaBin.at( uPt ).at( uEntry ), phiGenEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDRParton_ ) {
                histEtaTransRestrRebin->Fill( ptGenEtaBin.at( uPt ).at( uEntry ) - ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
            }
            else {
              if ( fitNonRestr_ ) {
                histEtaTransRebin->Fill( ptEtaBin.at( uPt ).at( uEntry ) - ptGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
              if ( ptGenEtaBin.at( uPt ).at( uEntry ) >= minPtParton_ && reco::deltaR( etaGenEtaBin.at( uPt ).at( uEntry ), phiGenEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDRParton_ ) {
                histEtaTransRestrRebin->Fill( ptEtaBin.at( uPt ).at( uEntry ) - ptGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
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
      }

      const std::string nameTransRebin( nameTrans + "Rebin" );
      const Double_t meanTrans( histTrans->GetMean() );
      const Double_t widthTrans( std::fabs( histTrans->GetRMS() ) );
      if ( widthTrans == 0. && verbose_ > 2 ) {
        std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                  << "    no histogram \"width\" in '" << nameTrans << "'" << std::endl;
      }
      const Double_t rangeTransRebin( widthTrans == 0. ? widthFactor_ * std::fabs( histTrans->GetXaxis()->GetXmax() ) : widthFactor_ * widthTrans ); // FIXME: tune, incl. under- and overflow, remove hard-coding
      TH1D * histTransRebin( new TH1D( nameTransRebin.c_str(), objCat.c_str(), histBins_, -rangeTransRebin + meanTrans, rangeTransRebin + meanTrans  ) );
      histTransRebin->SetXTitle( titleTrans.c_str() );
      histTransRebin->SetYTitle( titleEvents.c_str() );

      const std::string nameTransRestrRebin( nameTransRestr + "Rebin" );
      const Double_t meanTransRestr( histTransRestr->GetMean() );
      const Double_t widthTransRestr( std::fabs( histTransRestr->GetRMS() ) );
      if ( widthTransRestr == 0. && verbose_ > 2 ) {
        std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                  << "    no histogram \"width\" in '" << nameTransRestr << "'" << std::endl;
      }
      const Double_t rangeTransRestrRebin( widthTransRestr == 0. ? widthFactor_ * std::fabs( histTransRestr->GetXaxis()->GetXmax() ) : widthFactor_ * widthTransRestr ); // FIXME: tune, incl. under- and overflow, remove hard-coding
      TH1D * histTransRestrRebin( new TH1D( nameTransRestrRebin.c_str(), objCat.c_str(), histBins_, -rangeTransRestrRebin + meanTransRestr, rangeTransRestrRebin + meanTransRestr  ) );
      histTransRestrRebin->SetXTitle( titleTrans.c_str() );
      histTransRestrRebin->SetYTitle( titleEvents.c_str() );

      std::vector< TH1D * > histVecPtTransRebin;
      std::vector< TH1D * > histVecPtTransRestrRebin;
      for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
        const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
        const std::string namePt( name + "_Pt" + binPt );
        const std::string namePtTrans( namePt + "_Trans" );
        const std::string namePtTransRebin( namePtTrans + "Rebin" );
        const std::string titlePtTrans( objCat + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq p_{t} < " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
        const Double_t meanPtTrans( histVecPtTrans.at( uPt )->GetMean() );
        const Double_t widthPtTrans( std::fabs( histVecPtTrans.at( uPt )->GetRMS() ) );
        if ( widthPtTrans == 0. && verbose_ > 2 ) {
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    no histogram \"width\" in '" << namePtTrans << "'" << std::endl;
        }
        const Double_t rangePtTransRebin( widthPtTrans == 0. ? widthFactor_ * std::fabs( histVecPtTrans.at( uPt )->GetXaxis()->GetXmax() ) : widthFactor_ * widthPtTrans ); // FIXME: tune, incl. under- and overflow, remove hard-coding
        TH1D * histPtTransRebin( new TH1D( namePtTransRebin.c_str(), titlePtTrans.c_str(), histBins_, -rangePtTransRebin + meanPtTrans, rangePtTransRebin + meanPtTrans  ) );
        histPtTransRebin->SetXTitle( titleTrans.c_str() );
        histPtTransRebin->SetYTitle( titleEvents.c_str() );
        histVecPtTransRebin.push_back( histPtTransRebin );
        const std::string namePtTransRestr( namePtTrans + "Restr" );
        const std::string namePtTransRestrRebin( namePtTransRestr + "Rebin" );
        const Double_t meanPtTransRestr( histVecPtTransRestr.at( uPt )->GetMean() );
        const Double_t widthPtTransRestr( std::fabs( histVecPtTransRestr.at( uPt )->GetRMS() ) );
        if ( widthPtTransRestr == 0. && verbose_ > 2 ) {
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    no histogram \"width\" in '" << namePtTransRestr << "'" << std::endl;
        }
        const Double_t rangePtTransRestrRebin( widthPtTransRestr == 0. ? widthFactor_ * std::fabs( histVecPtTransRestr.at( uPt )->GetXaxis()->GetXmax() ) : widthFactor_ * widthPtTransRestr ); // FIXME: tune, incl. under- and overflow, remove hard-coding
        TH1D * histPtTransRestrRebin( new TH1D( namePtTransRestrRebin.c_str(), titlePtTrans.c_str(), histBins_, -rangePtTransRestrRebin + meanPtTransRestr, rangePtTransRestrRebin + meanPtTransRestr  ) );
        histPtTransRestrRebin->SetXTitle( titleTrans.c_str() );
        histPtTransRestrRebin->SetYTitle( titleEvents.c_str() );
        histVecPtTransRestrRebin.push_back( histPtTransRestrRebin );
      }  // loop: uPt < nPtBins_

      // Loop over eta bins
      nextInListFit.Reset();
      while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
        if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
        const std::string binEta( keyEta->GetName() );
        const unsigned uEta( std::atoi( binEta.substr( 3 ).data() ) );

        for ( unsigned uEntry = 0; uEntry < sizeEta_.at( uEta ); ++uEntry ) {
          if ( ptData_.at( uEta ).at( uEntry ) == -9. || ptGenData_.at( uEta ).at( uEntry ) == -9. ) continue; // no match
          if ( refGen_ ) {
            if ( fitNonRestr_ ) {
              histTransRebin->Fill( ptGenData_.at( uEta ).at( uEntry ) - ptData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
            }
            if ( ptGenData_.at( uEta ).at( uEntry ) >= minPtParton_ && reco::deltaR( etaGenData_.at( uEta ).at( uEntry ), phiGenData_.at( uEta ).at( uEntry ), etaData_.at( uEta ).at( uEntry ), phiData_.at( uEta ).at( uEntry ) ) <= maxDRParton_ ) {
              histTransRestrRebin->Fill( ptGenData_.at( uEta ).at( uEntry ) - ptData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
            }
          }
          else {
            if ( fitNonRestr_ ) {
              histTransRebin->Fill( ptData_.at( uEta ).at( uEntry ) - ptGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
            }
            if ( ptGenData_.at( uEta ).at( uEntry ) >= minPtParton_ && reco::deltaR( etaGenData_.at( uEta ).at( uEntry ), phiGenData_.at( uEta ).at( uEntry ), etaData_.at( uEta ).at( uEntry ), phiData_.at( uEta ).at( uEntry ) ) <= maxDRParton_ ) {
              histTransRestrRebin->Fill( ptData_.at( uEta ).at( uEntry ) - ptGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
            }
          }
          for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
            if ( refGen_ ) {
              if ( ptBins_.at( uPt ) <= ptGenData_.at( uEta ).at( uEntry ) && ptGenData_.at( uEta ).at( uEntry ) < ptBins_.at( uPt + 1 ) ) {
                if ( fitNonRestr_ ) {
                  histVecPtTransRebin.at( uPt )->Fill( ptGenData_.at( uEta ).at( uEntry ) - ptData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                }
                if ( ptGenData_.at( uEta ).at( uEntry ) >= minPtParton_ && reco::deltaR( etaGenData_.at( uEta ).at( uEntry ), phiGenData_.at( uEta ).at( uEntry ), etaData_.at( uEta ).at( uEntry ), phiData_.at( uEta ).at( uEntry ) ) <= maxDRParton_ ) {
                  histVecPtTransRestrRebin.at( uPt )->Fill( ptGenData_.at( uEta ).at( uEntry ) - ptData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                }
                break;
              }
            }
            else {
              if ( ptBins_.at( uPt ) <= ptData_.at( uEta ).at( uEntry ) && ptData_.at( uEta ).at( uEntry ) < ptBins_.at( uPt + 1 ) ) {
                if ( fitNonRestr_ ) {
                  histVecPtTransRebin.at( uPt )->Fill( ptData_.at( uEta ).at( uEntry ) - ptGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                }
                if ( ptGenData_.at( uEta ).at( uEntry ) >= minPtParton_ && reco::deltaR( etaGenData_.at( uEta ).at( uEntry ), phiGenData_.at( uEta ).at( uEntry ), etaData_.at( uEta ).at( uEntry ), phiData_.at( uEta ).at( uEntry ) ) <= maxDRParton_ ) {
                  histVecPtTransRestrRebin.at( uPt )->Fill( ptData_.at( uEta ).at( uEntry ) - ptGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                }
                break;
              }
            }
          } // loop: uPt < nPtBins_

        }

      } // loop: keyEta

    }  // loop: keyFit


    // Fit transfer functions

    if ( fit1D_ ) {
      if ( verbose_ > 1 ) {
        std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                  << "    1D transfer function determination for " << objCat << " started" << std::endl;
      }
      TCanvas c1( "c1" );
      c1.cd();
      const std::string fileName( "file" + objCat + ".pdf" );
      const std::string fileOpen( fileName + "[" );
      const std::string fileClose( fileName + "]" );
      if ( plot1D_ ) c1.Print( fileOpen.c_str() );

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
        dirFit_->cd();

        const std::string name( objCat + "_" + nameVar + "_" + subFit );

        // Transfer function parameters
        TF1 * fitTest( new TF1( "test", fitFunction1D_.c_str() ) );
        const unsigned nPar( fitTest->GetNumberFreeParameters() );
        std::vector< double > parVec;
        std::vector< double > parVecA;
        std::vector< double > parVecB;
        std::vector< double > parVecRestr;
        std::vector< double > parVecARestr;
        std::vector< double > parVecBRestr;
        for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
          parVec.push_back( -999999. );
          parVecA.push_back( -999999. );
          parVecB.push_back( -999999. );
          parVecRestr.push_back( -999999. );
          parVecARestr.push_back( -999999. );
          parVecBRestr.push_back( -999999. );
        }

        const std::string nameTrans( name + "_Trans" );
        const std::string nameTransRebin( nameTrans + "Rebin" );
        TH1D * histTransRebin( ( TH1D* )( dirFit_->Get( nameTransRebin.c_str() ) ) );
        if ( fitNonRestr_ && histTransRebin != 0 ) {
          const std::string nameTransRebinFit( nameTransRebin + "_fit" );
          TF1 * fitTransRebin( new TF1( nameTransRebinFit.c_str(), fitFunction1D_.c_str(), std::max( histTransRebin->GetXaxis()->GetXmin(), histTransRebin->GetMean() - histTransRebin->GetRMS() * fitRange1D_ ), std::min( histTransRebin->GetXaxis()->GetXmax(), histTransRebin->GetMean() + histTransRebin->GetRMS() * fitRange1D_ ) ) );
          my::setParametersFitDelta( fitTransRebin, histTransRebin );
          TFitResultPtr fitTransRebinResultPtr( histTransRebin->Fit( fitTransRebin, fitOptions1D_.c_str() ) );
          if ( fitTransRebinResultPtr >= 0 ) {
            if ( fitTransRebinResultPtr->Status() == 0 && fitTransRebinResultPtr->Ndf() != 0. ) {
              for ( unsigned uPar = 1; uPar < nPar; ++uPar ) {
                parVec.at( uPar ) = fitTransRebin->GetParameter( uPar );
              }
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << nameTransRebin << "' status " << fitTransRebinResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; dirFit_->pwd();
              std::cout << "    '" << nameTransRebin << std::endl;
            }
          }
          histTransRebin->Draw();
          if ( plot1D_ ) c1.Print( fileName.c_str() );
        }

        const std::string nameTransRestr( nameTrans + "Restr" );
        const std::string nameTransRestrRebin( nameTransRestr + "Rebin" );
        TH1D * histTransRestrRebin( ( TH1D* )( dirFit_->Get( nameTransRestrRebin.c_str() ) ) );
        if ( histTransRestrRebin != 0 ) {
          const std::string nameTransRestrRebinFit( nameTransRestrRebin + "_fit" );
          TF1 * fitTransRestrRebin( new TF1( nameTransRestrRebinFit.c_str(), fitFunction1D_.c_str(), std::max( histTransRestrRebin->GetXaxis()->GetXmin(), histTransRestrRebin->GetMean() - histTransRestrRebin->GetRMS() * fitRange1D_ ), std::min( histTransRestrRebin->GetXaxis()->GetXmax(), histTransRestrRebin->GetMean() + histTransRestrRebin->GetRMS() * fitRange1D_ ) ) );
          my::setParametersFitDelta( fitTransRestrRebin, histTransRestrRebin );
          TFitResultPtr fitTransRestrRebinResultPtr( histTransRestrRebin->Fit( fitTransRestrRebin, fitOptions1D_.c_str() ) );
          if ( fitTransRestrRebinResultPtr >= 0 ) {
            if ( fitTransRestrRebinResultPtr->Status() == 0 && fitTransRestrRebinResultPtr->Ndf() != 0. ) {
              for ( unsigned uPar = 1; uPar < nPar; ++uPar ) {
                parVecRestr.at( uPar ) = fitTransRestrRebin->GetParameter( uPar );
              }
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << nameTransRestrRebin << "' status " << fitTransRestrRebinResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; dirFit_->pwd();
              std::cout << "    '" << nameTransRestrRebin << std::endl;
            }
          }
          histTransRestrRebin->Draw();
          if ( plot1D_ ) c1.Print( fileName.c_str() );
        }

        // Loop over pt bins

        std::vector< TH1D * > histVecTransRebinPtFitMap;
        std::vector< TH1D * > histVecTransRestrRebinPtFitMap;
        for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
          const std::string parFit( boost::lexical_cast< std::string >( uPar ) );
          const std::string nameTransRebinPtFitMap( name + "_TransRebinPt_FitMap_Par" + parFit );
          const std::string titleTransRebinPtFitMap( objCat + ", par. " + parFit );
          TH1D * histTransRebinPtFitMap( new TH1D( nameTransRebinPtFitMap.c_str(), titleTransRebinPtFitMap.c_str(), nPtBins_, ptBins_.data() ) );
          histTransRebinPtFitMap->SetXTitle( titlePt.c_str() );
          histVecTransRebinPtFitMap.push_back( histTransRebinPtFitMap );
          const std::string nameTransRestrRebinPtFitMap( name + "_TransRestrRebinPt_FitMap_Par" + parFit );
          TH1D * histTransRestrRebinPtFitMap( new TH1D( nameTransRestrRebinPtFitMap.c_str(), titleTransRebinPtFitMap.c_str(), nPtBins_, ptBins_.data() ) );
          histTransRestrRebinPtFitMap->SetXTitle( titlePt.c_str() );
          histVecTransRestrRebinPtFitMap.push_back( histTransRestrRebinPtFitMap );
        }

        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
          const std::string namePt( name + "_Pt" + binPt );

          const std::string namePtTransRebin( namePt + "_TransRebin" );
          TH1D * histPtTransRebin( ( TH1D* )( dirFit_->Get( namePtTransRebin.c_str() ) ) );
          if ( fitNonRestr_ && histPtTransRebin != 0 ) {
            const std::string namePtTransRebinFit( namePtTransRebin + "_fit" );
            TF1 * fitPtTransRebin( new TF1( namePtTransRebinFit.c_str(), fitFunction1D_.c_str(), std::max( histPtTransRebin->GetXaxis()->GetXmin(), histPtTransRebin->GetMean() - histPtTransRebin->GetRMS() * fitRange1D_ ), std::min( histPtTransRebin->GetXaxis()->GetXmax(), histPtTransRebin->GetMean() + histPtTransRebin->GetRMS() * fitRange1D_ ) ) );
            my::setParametersFitDelta( fitPtTransRebin, histPtTransRebin );
            TFitResultPtr fitPtTransRebinResultPtr( histPtTransRebin->Fit( fitPtTransRebin, fitOptions1D_.c_str() ) );
            if ( fitPtTransRebinResultPtr >= 0 ) {
              if ( fitPtTransRebinResultPtr->Status() == 0 && fitPtTransRebinResultPtr->Ndf() != 0. ) {
                for ( unsigned uPar = 1; uPar < nPar; ++uPar ) {
                  histVecTransRebinPtFitMap.at( uPar )->SetBinContent( uPt + 1, fitPtTransRebinResultPtr->Parameter( uPar ) );
                  histVecTransRebinPtFitMap.at( uPar )->SetBinError( uPt + 1, fitPtTransRebinResultPtr->ParError( uPar ) );
                }
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; dirFit_->pwd();
                  std::cout << "    '" << namePtTransRebin << "' status " << fitPtTransRebinResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << namePtTransRebin << std::endl;
              }
            }
            histPtTransRebin->Draw();
            if ( plot1D_ ) c1.Print( fileName.c_str() );
          }

          const std::string namePtTransRestr( namePt + "_TransRestr" );
          const std::string namePtTransRestrRebin( namePtTransRestr + "Rebin" );
          TH1D * histPtTransRestrRebin( ( TH1D* )( dirFit_->Get( namePtTransRestrRebin.c_str() ) ) );
          if ( histPtTransRestrRebin != 0 ) {
            const std::string namePtTransRestrRebinFit( namePtTransRestrRebin + "_fit" );
            TF1 * fitPtTransRestrRebin( new TF1( namePtTransRestrRebinFit.c_str(), fitFunction1D_.c_str(), std::max( histPtTransRestrRebin->GetXaxis()->GetXmin(), histPtTransRestrRebin->GetMean() - histPtTransRestrRebin->GetRMS() * fitRange1D_ ), std::min( histPtTransRestrRebin->GetXaxis()->GetXmax(), histPtTransRestrRebin->GetMean() + histPtTransRestrRebin->GetRMS() * fitRange1D_ ) ) );
            my::setParametersFitDelta( fitPtTransRestrRebin, histPtTransRestrRebin );
            TFitResultPtr fitPtTransRestrRebinResultPtr( histPtTransRestrRebin->Fit( fitPtTransRestrRebin, fitOptions1D_.c_str() ) );
            if ( fitPtTransRestrRebinResultPtr >= 0 ) {
              if ( fitPtTransRestrRebinResultPtr->Status() == 0 && fitPtTransRestrRebinResultPtr->Ndf() != 0. ) {
                for ( unsigned uPar = 1; uPar < nPar; ++uPar ) {
                  histVecTransRestrRebinPtFitMap.at( uPar )->SetBinContent( uPt + 1, fitPtTransRestrRebinResultPtr->Parameter( uPar ) );
                  histVecTransRestrRebinPtFitMap.at( uPar )->SetBinError( uPt + 1, fitPtTransRestrRebinResultPtr->ParError( uPar ) );
                }
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; dirFit_->pwd();
                  std::cout << "    '" << namePtTransRestrRebin << "' status " << fitPtTransRestrRebinResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << namePtTransRestrRebin << std::endl;
              }
            }
            histPtTransRestrRebin->Draw();
            if ( plot1D_ ) c1.Print( fileName.c_str() );
          }

        } // loop: uPt < nPtBins_

        for ( unsigned uPar = 1; uPar < nPar; ++uPar ) {
          const std::string parFit( boost::lexical_cast< std::string >( uPar ) );
          if ( fitNonRestr_ ) {
            const std::string nameTransRebinPtFitMap( name + "_TransRebinPt_FitMap_Par" + parFit );
            const std::string nameTransRebinPtFitMapFit( nameTransRebinPtFitMap + "_fit" );
            TF1 * fitTransRebinPtFitMap( new TF1( nameTransRebinPtFitMapFit.c_str(), fitTransfer1D_.c_str(), ptBins_.front(), fitMaxPt1D_ ) );
            TFitResultPtr fitTransRebinPtFitMapResultPtr( histVecTransRebinPtFitMap.at( uPar )->Fit( fitTransRebinPtFitMap, fitOptions1D_.c_str() ) );
            if ( fitTransRebinPtFitMapResultPtr >= 0 ) {
              if ( fitTransRebinPtFitMapResultPtr->Status() == 0 && fitTransRebinPtFitMapResultPtr->Ndf() != 0. ) {
                parVecA.at( uPar ) = fitTransRebinPtFitMap->GetParameter( 0 );
                parVecB.at( uPar ) = fitTransRebinPtFitMap->GetParameter( 1 );
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; dirFit_->pwd();
                  std::cout << "    '" << nameTransRebinPtFitMap << "' status " << fitTransRebinPtFitMapResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << nameTransRebinPtFitMap << std::endl;
              }
            }
            histVecTransRebinPtFitMap.at( uPar )->Draw();
            if ( plot1D_ ) c1.Print( fileName.c_str() );
          }

          const std::string nameTransRestrRebinPtFitMap( name + "_TransRestrRebinPt_FitMap_Par" + parFit );
          const std::string nameTransRestrRebinPtFitMapFit( nameTransRestrRebinPtFitMap + "_fit" );
          TF1 * fitTransRestrRebinPtFitMap( new TF1( nameTransRestrRebinPtFitMapFit.c_str(), fitTransfer1D_.c_str(), ptBins_.front(), fitMaxPt1D_ ) );
          TFitResultPtr fitTransRestrRebinPtFitMapResultPtr( histVecTransRestrRebinPtFitMap.at( uPar )->Fit( fitTransRestrRebinPtFitMap, fitOptions1D_.c_str() ) );
          if ( fitTransRestrRebinPtFitMapResultPtr >= 0 ) {
            if ( fitTransRestrRebinPtFitMapResultPtr->Status() == 0 && fitTransRestrRebinPtFitMapResultPtr->Ndf() != 0. ) {
              parVecARestr.at( uPar ) = fitTransRestrRebinPtFitMap->GetParameter( 0 );
              parVecBRestr.at( uPar ) = fitTransRestrRebinPtFitMap->GetParameter( 1 );
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; dirFit_->pwd();
                std::cout << "    '" << nameTransRestrRebinPtFitMap << "' status " << fitTransRestrRebinPtFitMapResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; dirFit_->pwd();
              std::cout << "    '" << nameTransRestrRebinPtFitMap << std::endl;
            }
          }
          histVecTransRestrRebinPtFitMap.at( uPar )->Draw();
          if ( plot1D_ ) c1.Print( fileName.c_str() );
        }

        // Loop over eta bins

        std::vector< TH1D * > histVecTransRebinEtaFitMap;
        std::vector< TH1D * > histVecTransRestrRebinEtaFitMap;
        for ( unsigned uPar = 0; uPar < nPar; ++uPar ) {
          const std::string parFit( boost::lexical_cast< std::string >( uPar ) );
          const std::string nameTransRebinEtaFitMap( name + "_TransRebinEta_FitMap_Par" + parFit );
          const std::string titleTransRebinEtaFitMap( objCat + ", par. " + parFit );
          TH1D * histTransRebinEtaFitMap( new TH1D( nameTransRebinEtaFitMap.c_str(), titleTransRebinEtaFitMap.c_str(), nEtaBins_, etaBins_.data() ) );
          histTransRebinEtaFitMap->SetXTitle( titleEta.c_str() );
          histVecTransRebinEtaFitMap.push_back( histTransRebinEtaFitMap );
          const std::string nameTransRestrRebinEtaFitMap( name + "_TransRestrRebinEta_FitMap_Par" + parFit );
          TH1D * histTransRestrRebinEtaFitMap( new TH1D( nameTransRestrRebinEtaFitMap.c_str(), titleTransRebinEtaFitMap.c_str(), nEtaBins_, etaBins_.data() ) );
          histTransRestrRebinEtaFitMap->SetXTitle( titleEta.c_str() );
          histVecTransRestrRebinEtaFitMap.push_back( histTransRestrRebinEtaFitMap );
        }

        TList * listFit( dirFit_->GetListOfKeys() );
        TIter nextInListFit( listFit );
        while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
          if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
          const std::string binEta( keyEta->GetName() );
          const unsigned uEta( std::atoi( binEta.substr( 3 ).data() ) );
          TDirectory * dirEta_( ( TDirectory* )( dirFit_->Get( binEta.c_str() ) ) );
          dirEta_->cd();

          const std::string nameEta( name + "_" + binEta );

          const std::string nameEtaTransRebin( nameEta + "_TransRebin" );
          TH1D * histEtaTransRebin( ( TH1D* )( dirEta_->Get( nameEtaTransRebin.c_str() ) ) );
          if ( fitNonRestr_ && histEtaTransRebin != 0 ) {
            const std::string nameEtaTransRebinFit( nameEtaTransRebin + "_fit" );
            TF1 * fitEtaTransRebin( new TF1( nameEtaTransRebinFit.c_str(), fitFunction1D_.c_str(), std::max( histEtaTransRebin->GetXaxis()->GetXmin(), histEtaTransRebin->GetMean() - histEtaTransRebin->GetRMS() * fitRange1D_ ), std::min( histEtaTransRebin->GetXaxis()->GetXmax(), histEtaTransRebin->GetMean() + histEtaTransRebin->GetRMS() * fitRange1D_ ) ) );
            my::setParametersFitDelta( fitEtaTransRebin, histEtaTransRebin );
            TFitResultPtr fitEtaTransRebinResultPtr( histEtaTransRebin->Fit( fitEtaTransRebin, fitOptions1D_.c_str() ) );
            if ( fitEtaTransRebinResultPtr >= 0 ) {
              if ( fitEtaTransRebinResultPtr->Status() == 0 && fitEtaTransRebinResultPtr->Ndf() != 0. ) {
                for ( unsigned uPar = 1; uPar < nPar; ++uPar ) {
                  histVecTransRebinEtaFitMap.at( uPar )->SetBinContent( uEta + 1, fitEtaTransRebinResultPtr->Parameter( uPar ) );
                  histVecTransRebinEtaFitMap.at( uPar )->SetBinError( uEta + 1, fitEtaTransRebinResultPtr->ParError( uPar ) );
                }
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; dirEta_->pwd();
                  std::cout << "    '" << nameEtaTransRebin << "' status " << fitEtaTransRebinResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; dirEta_->pwd();
                std::cout << "    '" << nameEtaTransRebin << std::endl;
              }
            }
          }

          const std::string nameEtaTransRestr( nameEta + "_TransRestr" );
          const std::string nameEtaTransRestrRebin( nameEtaTransRestr + "Rebin" );
          TH1D * histEtaTransRestrRebin( ( TH1D* )( dirEta_->Get( nameEtaTransRestrRebin.c_str() ) ) );
          if ( histEtaTransRestrRebin != 0 ) {
            const std::string nameEtaTransRestrRebinFit( nameEtaTransRestrRebin + "_fit" );
            TF1 * fitEtaTransRestrRebin( new TF1( nameEtaTransRestrRebinFit.c_str(), fitFunction1D_.c_str(), std::max( histEtaTransRestrRebin->GetXaxis()->GetXmin(), histEtaTransRestrRebin->GetMean() - histEtaTransRestrRebin->GetRMS() * fitRange1D_ ), std::min( histEtaTransRestrRebin->GetXaxis()->GetXmax(), histEtaTransRestrRebin->GetMean() + histEtaTransRestrRebin->GetRMS() * fitRange1D_ ) ) );
            my::setParametersFitDelta( fitEtaTransRestrRebin, histEtaTransRestrRebin );
            TFitResultPtr fitEtaTransRestrRebinResultPtr( histEtaTransRestrRebin->Fit( fitEtaTransRestrRebin, fitOptions1D_.c_str() ) );
            if ( fitEtaTransRestrRebinResultPtr >= 0 ) {
              if ( fitEtaTransRestrRebinResultPtr->Status() == 0 && fitEtaTransRestrRebinResultPtr->Ndf() != 0. ) {
                for ( unsigned uPar = 1; uPar < nPar; ++uPar ) {
                  histVecTransRestrRebinEtaFitMap.at( uPar )->SetBinContent( uEta + 1, fitEtaTransRestrRebinResultPtr->Parameter( uPar ) );
                  histVecTransRestrRebinEtaFitMap.at( uPar )->SetBinError( uEta + 1, fitEtaTransRestrRebinResultPtr->ParError( uPar ) );
                }
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; dirEta_->pwd();
                  std::cout << "    '" << nameEtaTransRestrRebin << "' status " << fitEtaTransRestrRebinResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; dirEta_->pwd();
                std::cout << "    '" << nameEtaTransRestrRebin << std::endl;
              }
            }
          }


        } // loop: keyEta

        for ( unsigned uPar = 1; uPar < nPar; ++uPar ) {
          histVecTransRestrRebinPtFitMap.at( uPar )->Draw();
          if ( plot1D_ ) c1.Print( fileName.c_str() );
          histVecTransRestrRebinEtaFitMap.at( uPar )->Draw();
          if ( plot1D_ ) c1.Print( fileName.c_str() );
        }

        if ( writeFiles1D_ ) {

          // File name
          std::string nameOut( pathOut1D_ + "/gentTransferFunction1D_" + sample_ + "_" + name );
          if ( usePileUp_ ) nameOut.append( "_PileUp" );
          if ( refSel_)     nameOut.append( "_Ref" );
          nameOut.append( ".txt" );

          ofstream fileOut;
          fileOut.open( nameOut.c_str(), std::ios_base::out );

          fileOut << std::endl << "fitted function:";
          fileOut << std::endl << fitFunction1D_;
          fileOut << std::endl << "for p_t_parton <= " << fitMaxPt1D_;
          fileOut << std::endl << std::endl;


          if ( fitNonRestr_ ) {
            fileOut << std::endl << "unrestricted";
            fileOut << std::endl;

            fileOut << std::endl << "total";
            for ( unsigned uPar = 1; uPar < nPar; ++uPar ) {
              fileOut << std::endl << "transfer_" << uPar << " = ";
              fileOut << std::setprecision( 4 );
              if ( parVec.at( uPar ) == -999999. ) fileOut << "NAN";
              else                                 fileOut << parVec.at( uPar );
            }
            fileOut << std::endl;

            fileOut << std::endl << "over p_t";
            for ( unsigned uPar = 1; uPar < nPar; ++uPar ) {
              fileOut << std::endl << "transfer_" << uPar << " = ";
              fileOut << std::setprecision( 4 );
              if ( parVecA.at( uPar ) == -999999. ) fileOut << "NAN";
              else                                  fileOut << parVecA.at( uPar );
              fileOut << " + ";
              if ( parVecB.at( uPar ) == -999999. ) fileOut << "NAN";
              else                                  fileOut << parVecB.at( uPar );
              fileOut << " * E_parton";
            }
            fileOut << std::endl << std::endl;
          }

          fileOut << std::endl << "restricted";
          fileOut << std::endl << "p_t_parton          >= " << minPtParton_;
          fileOut << std::endl << "DeltaR(parton, jet) <= " << maxDRParton_;
          fileOut << std::endl;

          fileOut << std::endl << "total";
          for ( unsigned uPar = 1; uPar < nPar; ++uPar ) {
            fileOut << std::endl << "transfer_" << uPar << " = ";
            fileOut << std::setprecision( 4 );
            if ( parVecRestr.at( uPar ) == -999999. ) fileOut << "NAN";
            else                                      fileOut << parVecRestr.at( uPar );
          }
          fileOut << std::endl;

           fileOut << std::endl << "over p_t";
          for ( unsigned uPar = 1; uPar < nPar; ++uPar ) {
            fileOut << std::endl << "transfer_" << uPar << " = ";
            fileOut << std::setprecision( 4 );
            if ( parVecARestr.at( uPar ) == -999999. ) fileOut << "NAN";
            else                                       fileOut << parVecARestr.at( uPar );
            fileOut << " + ";
            if ( parVecBRestr.at( uPar ) == -999999. ) fileOut << "NAN";
            else                                       fileOut << parVecBRestr.at( uPar );
            fileOut << " * E_parton";
            fileOut << std::endl;
          }
          fileOut << std::endl;

          fileOut.close();
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    written 1D transfer function file:" << std::endl
                    << "        " << nameOut << std::endl;

        }

      } // loop: keyFit

      if ( plot1D_ ) c1.Print( fileClose.c_str() );

    }

  } // loop: uCat < objCats_.size()


  // Write and close input file
  if ( overwrite_ ) fileIn_->Write( 0, TObject::kOverwrite );
  else              fileIn_->Write();
  fileIn_->Close();

  if ( verbose_ > 0 )
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "    return status " << returnStatus_ << std::endl;
  return returnStatus_;

}
