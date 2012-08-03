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
  const bool useSymm_( process_.getParameter< bool >( "useSymm" ) );
  const bool refGenJet_( process_.getParameter< bool >( "refGenJet" ) );
  const bool refSel_( process_.getParameter< bool >( "refSel" ) );
  const std::string pileUp_( process_.getParameter< std::string >( "pileUp" ) );
  // Configuration for in- & output
  const edm::ParameterSet & io_( process_.getParameter< edm::ParameterSet >( "io" ) );
  const std::string inFile_( io_.getParameter< std::string >( "inputFile" ) );
  const std::string sample_( io_.getParameter< std::string >( "sample" ) );
  // Configuration for histogram binning
  const edm::ParameterSet & histos_( process_.getParameter< edm::ParameterSet >( "histos" ) );
  const double widthFactor_( histos_.getParameter< double >( "widthFactor" ) );
  // Configuration for fitting L5 JECs
  const edm::ParameterSet & fit_( process_.getParameter< edm::ParameterSet >( "fit" ) );
  const bool fitNonRestr_( fit_.getParameter< bool >( "fitNonRestr" ) );
  const bool fitEtaPt_( fit_.getParameter< bool >( "fitEtaPt" ) );
  const std::string fitFunction_( fit_.getParameter< std::string >( "fitFunction" ) );
  std::string fitOptions_( fit_.getParameter< std::string >( "fitOptions" ) );
  const double fitRange_( std::min( fit_.getParameter< double >( "fitRange" ), widthFactor_ ) );
  const std::string bkgFunction_( fit_.getParameter< std::string >( "bkgFunction" ) );
  const bool useBkgFunction_( bkgFunction_ != "0" );
  const double minPtGenJet_( fit_.getParameter< double >( "minPtGenJet" ) );
  const double minDRGenJet_( fit_.getParameter< double >( "minDRGenJet" ) );
  const bool onlyExisting_( fit_.getParameter< bool >( "onlyExisting" ) );
  const bool writeFiles_( fit_.getParameter< bool >( "writeFiles" ) && onlyExisting_ );
  const std::string pathOut_( fit_.getParameter< std::string >( "pathOut" ) );

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
  const std::string titleChi2( "#chi^{2} / ndf" );
  const std::string titleProb( "Fit robability" );
  const std::string titleMean( "Fit mean" );
  const std::string titleSigma( "Fit uncertainty" );
  const std::string titleNdf( "ndf" );
  const std::string titlePt( refGenJet_ ? "p_{t}^GenJet (GeV)" : "p_{t} (GeV)" );
  const std::string titleEta( refGenJet_ ? "#eta^GenJet" : "#eta" );
  std::string titleFracL5( refGenJet_ ? "#frac{p_{t}^{RECO}}{p_{t}^{GenJet}}" : "#frac{p_{t}^{GenJet}}{p_{t}^{RECO}}" );
  if ( useAlt_ ) {
    titleFracL5 = ( refGenJet_ ? "#frac{p_{t}^{RECO,alt.}}{p_{t}^{GenJet}}" : "#frac{p_{t}^{GenJet}}{p_{t}^{RECO,alt.}}" );
  }
  const std::string titleFracL5Mean( "#mu of " + titleFracL5 );
  const std::string titleFracL5Sigma( "#sigma of " + titleFracL5 );
  const std::string titleEvents( "events" );
  const std::string titleFits( "fits" );

  if ( returnStatus_ != 0 ) return returnStatus_;


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
  TDirectory * dirSel_ = dynamic_cast< TDirectory* >( fileIn_->Get( evtSel_.c_str() ) );
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
    TDirectory * dirCat_( dynamic_cast< TDirectory* >( dirSel_->Get( objCat.c_str() ) ) );
    if ( ! dirCat_ ) {
      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                << "    object category '" << objCat << "' does not exist in input file" << std::endl;
      continue;
    }

    // Get binning per object category

    // Eta binning
    std::vector< double > etaBins_;
    TH1D * histBinsEta( dynamic_cast< TH1D* >( dirCat_->Get( std::string( objCat + "_binsEta" ).c_str() ) ) );
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
    TH1D * histBinsPt( dynamic_cast< TH1D* >( dirCat_->Get( std::string( objCat + "_binsPt" ).c_str() ) ) );
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
    TTree * data_( dynamic_cast< TTree* >( dirCat_->Get( std::string( objCat + "_data" ).c_str() ) ) );
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
    data_->SetBranchAddress( "PtGenJet" , &ptGenJetData );
    data_->SetBranchAddress( "EtaGenJet", &etaGenJetData );
    data_->SetBranchAddress( "PhiGenJet", &phiGenJetData );
    if ( useSymm_ )
      if      ( refGenJet_ ) data_->SetBranchAddress( "BinEtaSymmGenJet", &iEta );
      else if ( useAlt_ ) data_->SetBranchAddress( "BinEtaSymmAlt", &iEta );
      else                data_->SetBranchAddress( "BinEtaSymm"   , &iEta );
    else
      if      ( refGenJet_ ) data_->SetBranchAddress( "BinEtaGenJet", &iEta );
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
      ptGenJetData_.at( iEta ).push_back( ptGenJetData );
      etaData_.at( iEta ).push_back( etaData );
      etaGenJetData_.at( iEta ).push_back( etaGenJetData );
      phiData_.at( iEta ).push_back( phiData );
      phiGenJetData_.at( iEta ).push_back( phiGenJetData );
    }

    TDirectory * dirPt_( dynamic_cast< TDirectory* >( dirCat_->Get( "Pt" ) ) );

    // Histogram binning
    const unsigned nPtBins( histos_.getParameter< unsigned >( std::string( objCat + "PtBins" ) ) );

    // Loop over fit versions
    TList * listProp( dirPt_->GetListOfKeys() );
    if ( verbose_ > 3 ) listProp->Print();
    TIter nextInListProp( listProp );
    while ( TKey * keyFit = ( TKey* )nextInListProp() ) {
      if ( std::string( keyFit->GetClassName() ) != nameDirClass ) continue;
      const std::string subFit( keyFit->GetName() );
      if ( subFit.find( "Inv" ) != std::string::npos ) continue; // nothing to do for inverse
      // These are real switches: depending on configuration, only one setting combination can be run at a time
      if ( useAlt_    == ( subFit.find( "Alt" )  == std::string::npos ) ) continue;
      if ( useSymm_   == ( subFit.find( "Symm" ) == std::string::npos ) ) continue;
      if ( refGenJet_ == ( subFit.find( "Gen" )  == std::string::npos ) ) continue;
      TDirectory * dirFit_( dynamic_cast< TDirectory* >( dirPt_->Get( subFit.c_str() ) ) );
      dirFit_->cd();

      const std::string name( objCat + "_Pt_" + subFit );

      const std::string nameFracL5( name + "_FracL5" );
      TH1D * histFracL5( new TH1D( nameFracL5.c_str(), objCat.c_str(), nPtBins, 0., 2. ) );
      histFracL5->SetXTitle( titleFracL5.c_str() );
      histFracL5->SetYTitle( titleEvents.c_str() );

      const std::string nameFracL5Restr( nameFracL5 + "Restr" );
      TH1D * histFracL5Restr( new TH1D( nameFracL5Restr.c_str(), objCat.c_str(), nPtBins, 0., 2. ) );
      histFracL5Restr->SetXTitle( titleFracL5.c_str() );
      histFracL5Restr->SetYTitle( titleEvents.c_str() );

      std::vector< TH1D * > histVecPtFracL5;
      std::vector< TH1D * > histVecPtFracL5Restr;
      for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
        const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
        const std::string namePt( name + "_Pt" + binPt );
        const std::string titlePtDelta( objCat + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq p_{t} #leq " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
        const std::string namePtFracL5( namePt + "_FracL5" );
        TH1D * histPtFracL5( new TH1D( namePtFracL5.c_str(), titlePtDelta.c_str(), nPtBins, 0., 2. ) );
        histPtFracL5->SetXTitle( titleFracL5.c_str() );
        histPtFracL5->SetYTitle( titleEvents.c_str() );
        histVecPtFracL5.push_back( histPtFracL5 );
        const std::string namePtFracL5Restr( namePtFracL5 + "Restr" );
        TH1D * histPtFracL5Restr( new TH1D( namePtFracL5Restr.c_str(), titlePtDelta.c_str(), nPtBins, 0., 2. ) );
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
        const std::string titleEtaFracL5( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) );
        TH1D * histEtaFracL5( new TH1D( nameEtaFracL5.c_str(), titleEtaFracL5.c_str(), nPtBins, 0., 2. ) );
        histEtaFracL5->SetXTitle( titleFracL5.c_str() );
        histEtaFracL5->SetYTitle( titleEvents.c_str() );

        const std::string nameEtaFracL5Restr( nameEtaFracL5 + "Restr" );
        TH1D * histEtaFracL5Restr( new TH1D( nameEtaFracL5Restr.c_str(), titleEtaFracL5.c_str(), nPtBins, 0., 2. ) );
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
          for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
            if ( ptBins_.at( uPt ) <= ptData_.at( uEta ).at( uEntry ) && ptData_.at( uEta ).at( uEntry ) < ptBins_.at( uPt + 1 ) ) {
              sizePt.at( uPt ) += 1;
              weightEtaBin.at( uPt ).push_back( weightData_.at( uEta ).at( uEntry ) );
              ptEtaBin.at( uPt ).push_back( ptData_.at( uEta ).at( uEntry ) );
              ptGenJetEtaBin.at( uPt ).push_back( ptGenJetData_.at( uEta ).at( uEntry ) );
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
          if ( sizePt.at( uPt ) == 0 ) {
            if ( verbose_ > 2 ) {
              std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                        << "    empty bin in '" << nameEta << "' for p_t bin " << uPt  << std::endl;
            }
          }
          const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
          const std::string nameEtaPt( nameEta + "_Pt" + binPt );

          const std::string nameEtaPtFracL5( nameEtaPt + "_FracL5" );
          const std::string titleEtaPtFracL5( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq p_{t} #leq " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
          TH1D * histEtaPtFracL5( new TH1D( nameEtaPtFracL5.c_str(), titleEtaPtFracL5.c_str(), nPtBins, 0., 2. ) );
          histEtaPtFracL5->SetXTitle( titleFracL5.c_str() );
          histEtaPtFracL5->SetYTitle( titleEvents.c_str() );

          const std::string nameEtaPtFracL5Restr( nameEtaPtFracL5 + "Restr" );
          TH1D * histEtaPtFracL5Restr( new TH1D( nameEtaPtFracL5Restr.c_str(), titleEtaPtFracL5.c_str(), nPtBins, 0., 2. ) );
          histEtaPtFracL5Restr->SetXTitle( titleFracL5.c_str() );
          histEtaPtFracL5Restr->SetYTitle( titleEvents.c_str() );

          for ( unsigned uEntry = 0; uEntry < sizePt.at( uPt ); ++uEntry ) {
            if ( refGenJet_ ) {
              if ( fitNonRestr_ ) {
                if ( fitEtaPt_ ) histEtaPtFracL5->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenJetEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histVecPtFracL5.at( uPt )->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenJetEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaFracL5->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenJetEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histFracL5->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenJetEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
              if ( ptGenJetEtaBin.at( uPt ).at( uEntry ) >= minPtGenJet_ && reco::deltaR( etaGenJetEtaBin.at( uPt ).at( uEntry ), phiGenJetEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= minDRGenJet_ ) {
                if ( fitEtaPt_ ) histEtaPtFracL5Restr->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenJetEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histVecPtFracL5Restr.at( uPt )->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenJetEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaFracL5Restr->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenJetEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histFracL5Restr->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenJetEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
            }
            else {
              if ( fitNonRestr_ ) {
                if ( fitEtaPt_ ) histEtaPtFracL5->Fill( ptGenJetEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histVecPtFracL5.at( uPt )->Fill( ptGenJetEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaFracL5->Fill( ptGenJetEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histFracL5->Fill( ptGenJetEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
              if ( ptGenJetEtaBin.at( uPt ).at( uEntry ) >= minPtGenJet_ && reco::deltaR( etaGenJetEtaBin.at( uPt ).at( uEntry ), phiGenJetEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= minDRGenJet_ ) {
                if ( fitEtaPt_ ) histEtaPtFracL5Restr->Fill( ptGenJetEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histVecPtFracL5Restr.at( uPt )->Fill( ptGenJetEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaFracL5Restr->Fill( ptGenJetEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histFracL5Restr->Fill( ptGenJetEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
            }
          } // loop: uEntry < ptEtaBin.at( uPt ).size()

          if ( fitEtaPt_ ) {
            if ( fitNonRestr_ ) histEtaPtFracL5->Scale( 1. / histEtaPtFracL5->Integral() );
            histEtaPtFracL5Restr->Scale( 1. / histEtaPtFracL5Restr->Integral() );
          }

        } // loop: uPt < nPtBins_

        if ( fitNonRestr_ ) histEtaFracL5->Scale( 1. / histEtaFracL5->Integral() );
        histEtaFracL5Restr->Scale( 1. / histEtaFracL5Restr->Integral() );

      } // loop: keyEta

      if ( fitNonRestr_ ) histFracL5->Scale( 1. / histFracL5->Integral() );
      histFracL5Restr->Scale( 1. / histFracL5Restr->Integral() );
      if ( fitNonRestr_ ) {
        for ( unsigned uPt = 0; uPt < histVecPtFracL5.size(); ++uPt ) {
          histVecPtFracL5.at( uPt )->Scale( 1. / histVecPtFracL5.at( uPt )->Integral() );
        }
      }
      for ( unsigned uPt = 0; uPt < histVecPtFracL5Restr.size(); ++uPt ) {
        histVecPtFracL5Restr.at( uPt )->Scale( 1. / histVecPtFracL5Restr.at( uPt )->Integral() );
      }

      dirFit_->cd();

      if ( sizeEtaBins != nEtaBins_ ) {
        std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                  << "    mismatch of eta binning for object category '" << objCat << "':" << std::endl
                  << "        bins in directory structure: " << sizeEtaBins                << std::endl
                  << "        bins in binning histogram  : " << nEtaBins_                  << std::endl;
      }

    } // loop: keyFit


    // Fit L5 JECs

    if ( objCat == "UdscJet" || objCat == "BJet" ) {
      if ( verbose_ > 1 ) {
        std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                  << "    JEC L5 determination for " << objCat << " started" << std::endl;
      }

      TDirectory * dirPt_( dynamic_cast< TDirectory* >( dirCat_->Get( "Pt" ) ) );

      // Loop over fit versions
      TList * listProp( dirPt_->GetListOfKeys() );
      TIter nextInListProp( listProp );
      while ( TKey * keyFit = ( TKey* )nextInListProp() ) {
        if ( std::string( keyFit->GetClassName() ) != nameDirClass ) continue;
        const std::string subFit( keyFit->GetName() );
        if ( subFit.find( "Inv" ) != std::string::npos ) continue; // nothing to do for inverse
        // These are real switches: depending on configuration, only one setting combination can be run at a time
        if ( useAlt_    == ( subFit.find( "Alt" )  == std::string::npos ) ) continue;
        if ( useSymm_   == ( subFit.find( "Symm" ) == std::string::npos ) ) continue;
        if ( refGenJet_ == ( subFit.find( "Gen" )  == std::string::npos ) ) continue;
        TDirectory * dirFit_( dynamic_cast< TDirectory* >( dirPt_->Get( subFit.c_str() ) ) );
        dirFit_->cd();

        const std::string name( objCat + "_Pt_" + subFit );

        const std::string nameFracL5( name + "_FracL5" );
        TH1D * histFracL5( dynamic_cast< TH1D* >( gDirectory->Get( nameFracL5.c_str() ) ) );
        if ( fitNonRestr_ && histFracL5 != 0 ) {
          const std::string nameFracL5Fit( nameFracL5 + "_fit" );
          TF1 * fitFracL5( new TF1( nameFracL5Fit.c_str(), fitFunction_.c_str(), std::max( histFracL5->GetXaxis()->GetXmin(), histFracL5->GetMean() - histFracL5->GetRMS() * fitRange_ ), std::min( histFracL5->GetXaxis()->GetXmax(), histFracL5->GetMean() + histFracL5->GetRMS() * fitRange_ ) ) );
          my::setParametersFit( fitFracL5, histFracL5, useBkgFunction_ );
          TFitResultPtr fitFracL5ResultPtr( histFracL5->Fit( fitFracL5, fitOptions_.c_str() ) );
          if ( fitFracL5ResultPtr >= 0 ) {
            if ( fitFracL5ResultPtr->Status() == 0 && fitFracL5ResultPtr->Ndf() != 0. ) {
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << nameFracL5 << "' status " << fitFracL5ResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; gDirectory->pwd();
              std::cout << "    '" << nameFracL5 << std::endl;
            }
          }
          const std::string nameFracL5Bkg( nameFracL5 + "_bkg" );
          TF1 * bkgFracL5( new TF1( nameFracL5Bkg.c_str(), bkgFunction_.c_str(), std::max( histFracL5->GetXaxis()->GetXmin(), histFracL5->GetMean() - histFracL5->GetRMS() * fitRange_ ), std::min( histFracL5->GetXaxis()->GetXmax(), histFracL5->GetMean() + histFracL5->GetRMS() * fitRange_ ) ) );
          my::setParametersBkg( fitFracL5, bkgFracL5 );
          if ( overwrite_ ) bkgFracL5->Write( 0, TObject::kOverwrite );
          else              bkgFracL5->Write();
        }

        const std::string nameFracL5Restr( nameFracL5 + "Restr" );
        TH1D * histFracL5Restr( dynamic_cast< TH1D* >( gDirectory->Get( nameFracL5Restr.c_str() ) ) );
        if ( histFracL5Restr != 0 ) {
          const std::string nameFracL5RestrFit( nameFracL5Restr + "_fit" );
          TF1 * fitFracL5Restr( new TF1( nameFracL5RestrFit.c_str(), fitFunction_.c_str(), std::max( histFracL5Restr->GetXaxis()->GetXmin(), histFracL5Restr->GetMean() - histFracL5Restr->GetRMS() * fitRange_ ), std::min( histFracL5Restr->GetXaxis()->GetXmax(), histFracL5Restr->GetMean() + histFracL5Restr->GetRMS() * fitRange_ ) ) );
          my::setParametersFit( fitFracL5Restr, histFracL5Restr, useBkgFunction_ );
          TFitResultPtr fitFracL5RestrResultPtr( histFracL5Restr->Fit( fitFracL5Restr, fitOptions_.c_str() ) );
          if ( fitFracL5RestrResultPtr >= 0 ) {
            if ( fitFracL5RestrResultPtr->Status() == 0 && fitFracL5RestrResultPtr->Ndf() != 0. ) {
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << nameFracL5Restr << "' status " << fitFracL5RestrResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; gDirectory->pwd();
              std::cout << "    '" << nameFracL5Restr << std::endl;
            }
          }
          const std::string nameFracL5RestrBkg( nameFracL5Restr + "_bkg" );
          TF1 * bkgFracL5Restr( new TF1( nameFracL5RestrBkg.c_str(), bkgFunction_.c_str(), std::max( histFracL5Restr->GetXaxis()->GetXmin(), histFracL5Restr->GetMean() - histFracL5Restr->GetRMS() * fitRange_ ), std::min( histFracL5Restr->GetXaxis()->GetXmax(), histFracL5Restr->GetMean() + histFracL5Restr->GetRMS() * fitRange_ ) ) );
          my::setParametersBkg( fitFracL5Restr, bkgFracL5Restr );
          if ( overwrite_ ) bkgFracL5Restr->Write( 0, TObject::kOverwrite );
          else              bkgFracL5Restr->Write();
        }

        // Loop over pt bins

        // Fit performance histograms
        const std::string nameFracL5PtFitMeanMap( name + "_FracL5Pt_FitMeanMap" );
        TH1D * histFracL5PtFitMeanMap( new TH1D( nameFracL5PtFitMeanMap.c_str(), objCat.c_str(), nPtBins_, ptBins_.data() ) );
        histFracL5PtFitMeanMap->SetXTitle( titlePt.c_str() );
        histFracL5PtFitMeanMap->SetYTitle( titleFracL5Mean.c_str() );
        const std::string nameFracL5RestrPtFitMeanMap( name + "_FracL5RestrPt_FitMeanMap" );
        TH1D * histFracL5RestrPtFitMeanMap( new TH1D( nameFracL5RestrPtFitMeanMap.c_str(), objCat.c_str(), nPtBins_, ptBins_.data() ) );
        histFracL5RestrPtFitMeanMap->SetXTitle( titlePt.c_str() );
        histFracL5RestrPtFitMeanMap->SetYTitle( titleFracL5Mean.c_str() );

        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
          const std::string namePt( name + "_Pt" + binPt );

          const std::string namePtFracL5( namePt + "_FracL5" );
          TH1D * histPtFracL5( dynamic_cast< TH1D* >( gDirectory->Get( namePtFracL5.c_str() ) ) );
          if ( fitNonRestr_ && histPtFracL5 != 0 ) {
            const std::string namePtFracL5Fit( namePtFracL5 + "_fit" );
            TF1 * fitPtFracL5( new TF1( namePtFracL5Fit.c_str(), fitFunction_.c_str(), std::max( histPtFracL5->GetXaxis()->GetXmin(), histPtFracL5->GetMean() - histPtFracL5->GetRMS() * fitRange_ ), std::min( histPtFracL5->GetXaxis()->GetXmax(), histPtFracL5->GetMean() + histPtFracL5->GetRMS() * fitRange_ ) ) );
            my::setParametersFit( fitPtFracL5, histPtFracL5, useBkgFunction_ );
            TFitResultPtr fitPtFracL5ResultPtr( histPtFracL5->Fit( fitPtFracL5, fitOptions_.c_str() ) );
            if ( fitPtFracL5ResultPtr >= 0 ) {
              if ( fitPtFracL5ResultPtr->Status() == 0 && fitPtFracL5ResultPtr->Ndf() != 0. ) {
                histFracL5PtFitMeanMap->SetBinContent( uPt + 1, fitPtFracL5ResultPtr->Parameter( 0 ) );
                histFracL5PtFitMeanMap->SetBinError( uPt + 1, fitPtFracL5ResultPtr->ParError( 0 ) );
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << namePtFracL5 << "' status " << fitPtFracL5ResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << namePtFracL5 << std::endl;
              }
            }
            const std::string namePtFracL5Bkg( namePtFracL5 + "_bkg" );
            TF1 * bkgPtFracL5( new TF1( namePtFracL5Bkg.c_str(), bkgFunction_.c_str(), std::max( histPtFracL5->GetXaxis()->GetXmin(), histPtFracL5->GetMean() - histPtFracL5->GetRMS() * fitRange_ ), std::min( histPtFracL5->GetXaxis()->GetXmax(), histPtFracL5->GetMean() + histPtFracL5->GetRMS() * fitRange_ ) ) );
            my::setParametersBkg( fitPtFracL5, bkgPtFracL5 );
            if ( overwrite_ ) bkgPtFracL5->Write( 0, TObject::kOverwrite );
            else              bkgPtFracL5->Write();
          }

          const std::string namePtFracL5Restr( namePtFracL5 + "Restr" );
          TH1D * histPtFracL5Restr( dynamic_cast< TH1D* >( gDirectory->Get( namePtFracL5Restr.c_str() ) ) );
          if ( histPtFracL5Restr != 0 ) {
            const std::string namePtFracL5RestrFit( namePtFracL5Restr + "_fit" );
            TF1 * fitPtFracL5Restr( new TF1( namePtFracL5RestrFit.c_str(), fitFunction_.c_str(), std::max( histPtFracL5Restr->GetXaxis()->GetXmin(), histPtFracL5Restr->GetMean() - histPtFracL5Restr->GetRMS() * fitRange_ ), std::min( histPtFracL5Restr->GetXaxis()->GetXmax(), histPtFracL5Restr->GetMean() + histPtFracL5Restr->GetRMS() * fitRange_ ) ) );
            my::setParametersFit( fitPtFracL5Restr, histPtFracL5Restr, useBkgFunction_ );
            TFitResultPtr fitPtFracL5RestrResultPtr( histPtFracL5Restr->Fit( fitPtFracL5Restr, fitOptions_.c_str() ) );
            if ( fitPtFracL5RestrResultPtr >= 0 ) {
              if ( fitPtFracL5RestrResultPtr->Status() == 0 && fitPtFracL5RestrResultPtr->Ndf() != 0. ) {
                histFracL5RestrPtFitMeanMap->SetBinContent( uPt + 1, fitPtFracL5RestrResultPtr->Parameter( 0 ) );
                histFracL5RestrPtFitMeanMap->SetBinError( uPt + 1, fitPtFracL5RestrResultPtr->ParError( 0 ) );
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << namePtFracL5Restr << "' status " << fitPtFracL5RestrResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << namePtFracL5Restr << std::endl;
              }
            }
            const std::string namePtFracL5RestrBkg( namePtFracL5Restr + "_bkg" );
            TF1 * bkgPtFracL5Restr( new TF1( namePtFracL5RestrBkg.c_str(), bkgFunction_.c_str(), std::max( histPtFracL5Restr->GetXaxis()->GetXmin(), histPtFracL5Restr->GetMean() - histPtFracL5Restr->GetRMS() * fitRange_ ), std::min( histPtFracL5Restr->GetXaxis()->GetXmax(), histPtFracL5Restr->GetMean() + histPtFracL5Restr->GetRMS() * fitRange_ ) ) );
            my::setParametersBkg( fitPtFracL5Restr, bkgPtFracL5Restr );
            if ( overwrite_ ) bkgPtFracL5Restr->Write( 0, TObject::kOverwrite );
            else              bkgPtFracL5Restr->Write();
          }

        } // loop: uPt < nPtBins_

        // Loop over eta bins

        // Fit performance histograms
        const std::string nameFracL5EtaFitMeanMap( name + "_FracL5Eta_FitMeanMap" );
        TH1D * histFracL5EtaFitMeanMap( new TH1D( nameFracL5EtaFitMeanMap.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data() ) );
        histFracL5EtaFitMeanMap->SetXTitle( titleEta.c_str() );
        histFracL5EtaFitMeanMap->SetYTitle( titleFracL5Mean.c_str() );
        const std::string nameFracL5EtaPtFitMeanMap( name + "_FracL5EtaPt_FitMeanMap" );
        TH2D * histFracL5EtaPtFitMeanMap( new TH2D( nameFracL5EtaPtFitMeanMap.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
        histFracL5EtaPtFitMeanMap->SetXTitle( titleEta.c_str() );
        histFracL5EtaPtFitMeanMap->SetYTitle( titlePt.c_str() );
        histFracL5EtaPtFitMeanMap->SetZTitle( titleFracL5Mean.c_str() );
        const std::string nameFracL5EtaPtFitMean( name + "_FracL5EtaPt_FitMean" );
        TH1D * histFracL5EtaPtFitMean( new TH1D( nameFracL5EtaPtFitMean.c_str(), titleMean.c_str(), 20, 0.75, 1.25 ) );
        histFracL5EtaPtFitMean->SetXTitle( titleFracL5Mean.c_str() );
        histFracL5EtaPtFitMean->SetYTitle( titleFits.c_str() );
        const std::string nameFracL5EtaPtFitSigma( name + "_FracL5EtaPt_FitSigma" );
        TH1D * histFracL5EtaPtFitSigma( new TH1D( nameFracL5EtaPtFitSigma.c_str(), titleSigma.c_str(), 20, 0., 1. ) );
        histFracL5EtaPtFitSigma->SetXTitle( titleFracL5Sigma.c_str() );
        histFracL5EtaPtFitSigma->SetYTitle( titleFits.c_str() );
        const std::string nameFracL5RestrEtaFitMeanMap( name + "_FracL5RestrEta_FitMeanMap" );
        TH1D * histFracL5RestrEtaFitMeanMap( new TH1D( nameFracL5RestrEtaFitMeanMap.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data() ) );
        histFracL5RestrEtaFitMeanMap->SetXTitle( titleEta.c_str() );
        histFracL5RestrEtaFitMeanMap->SetYTitle( titleFracL5Mean.c_str() );
        const std::string nameFracL5RestrEtaPtFitMeanMap( name + "_FracL5RestrEtaPt_FitMeanMap" );
        TH2D * histFracL5RestrEtaPtFitMeanMap( new TH2D( nameFracL5RestrEtaPtFitMeanMap.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
        histFracL5RestrEtaPtFitMeanMap->SetXTitle( titleEta.c_str() );
        histFracL5RestrEtaPtFitMeanMap->SetYTitle( titlePt.c_str() );
        histFracL5RestrEtaPtFitMeanMap->SetZTitle( titleFracL5Mean.c_str() );
        const std::string nameFracL5RestrEtaPtFitMean( name + "_FracL5RestrEtaPt_FitMean" );
        TH1D * histFracL5RestrEtaPtFitMean( new TH1D( nameFracL5RestrEtaPtFitMean.c_str(), titleMean.c_str(), 20, 0.75, 1.25 ) );
        histFracL5RestrEtaPtFitMean->SetXTitle( titleFracL5Mean.c_str() );
        histFracL5RestrEtaPtFitMean->SetYTitle( titleFits.c_str() );
        const std::string nameFracL5RestrEtaPtFitSigma( name + "_FracL5RestrEtaPt_FitSigma" );
        TH1D * histFracL5RestrEtaPtFitSigma( new TH1D( nameFracL5RestrEtaPtFitSigma.c_str(), titleSigma.c_str(), 20, 0., 1. ) );
        histFracL5RestrEtaPtFitSigma->SetXTitle( titleFracL5Sigma.c_str() );
        histFracL5RestrEtaPtFitSigma->SetYTitle( titleFits.c_str() );

        TList * listFit( dirFit_->GetListOfKeys() );
        TIter nextInListFit( listFit );
        while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
          if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
          const std::string binEta( keyEta->GetName() );
          const unsigned uEta( std::atoi( binEta.substr( 3 ).data() ) );
          dirFit_->cd( binEta.c_str() );

          const std::string nameEta( name + "_" + binEta );

          const std::string nameEtaFracL5( nameEta + "_FracL5" );
          TH1D * histEtaFracL5( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaFracL5.c_str() ) ) );
          if ( fitNonRestr_ && histEtaFracL5 != 0 ) {
            const std::string nameEtaFracL5Fit( nameEtaFracL5 + "_fit" );
            TF1 * fitEtaFracL5( new TF1( nameEtaFracL5Fit.c_str(), fitFunction_.c_str(), std::max( histEtaFracL5->GetXaxis()->GetXmin(), histEtaFracL5->GetMean() - histEtaFracL5->GetRMS() * fitRange_ ), std::min( histEtaFracL5->GetXaxis()->GetXmax(), histEtaFracL5->GetMean() + histEtaFracL5->GetRMS() * fitRange_ ) ) );
            my::setParametersFit( fitEtaFracL5, histEtaFracL5, useBkgFunction_ );
            TFitResultPtr fitEtaFracL5ResultPtr( histEtaFracL5->Fit( fitEtaFracL5, fitOptions_.c_str() ) );
            if ( fitEtaFracL5ResultPtr >= 0 ) {
              if ( fitEtaFracL5ResultPtr->Status() == 0 && fitEtaFracL5ResultPtr->Ndf() != 0. ) {
                histFracL5EtaFitMeanMap->SetBinContent( uEta + 1, fitEtaFracL5ResultPtr->Parameter( 0 ) );
                histFracL5EtaFitMeanMap->SetBinError( uEta + 1, fitEtaFracL5ResultPtr->ParError( 0 ) );
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << nameEtaFracL5 << "' status " << fitEtaFracL5ResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << nameEtaFracL5 << std::endl;
              }
            }
            const std::string nameEtaFracL5Bkg( nameEtaFracL5 + "_bkg" );
            TF1 * bkgEtaFracL5( new TF1( nameEtaFracL5Bkg.c_str(), bkgFunction_.c_str(), std::max( histEtaFracL5->GetXaxis()->GetXmin(), histEtaFracL5->GetMean() - histEtaFracL5->GetRMS() * fitRange_ ), std::min( histEtaFracL5->GetXaxis()->GetXmax(), histEtaFracL5->GetMean() + histEtaFracL5->GetRMS() * fitRange_ ) ) );
            my::setParametersBkg( fitEtaFracL5, bkgEtaFracL5 );
            if ( overwrite_ ) bkgEtaFracL5->Write( 0, TObject::kOverwrite );
            else              bkgEtaFracL5->Write();
          }

          const std::string nameEtaFracL5Restr( nameEtaFracL5 + "Restr" );
          TH1D * histEtaFracL5Restr( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaFracL5Restr.c_str() ) ) );
          if ( histEtaFracL5Restr != 0 ) {
            const std::string nameEtaFracL5RestrFit( nameEtaFracL5Restr + "_fit" );
            TF1 * fitEtaFracL5Restr( new TF1( nameEtaFracL5RestrFit.c_str(), fitFunction_.c_str(), std::max( histEtaFracL5Restr->GetXaxis()->GetXmin(), histEtaFracL5Restr->GetMean() - histEtaFracL5Restr->GetRMS() * fitRange_ ), std::min( histEtaFracL5Restr->GetXaxis()->GetXmax(), histEtaFracL5Restr->GetMean() + histEtaFracL5Restr->GetRMS() * fitRange_ ) ) );
            my::setParametersFit( fitEtaFracL5Restr, histEtaFracL5Restr, useBkgFunction_ );
            TFitResultPtr fitEtaFracL5RestrResultPtr( histEtaFracL5Restr->Fit( fitEtaFracL5Restr, fitOptions_.c_str() ) );
            if ( fitEtaFracL5RestrResultPtr >= 0 ) {
              if ( fitEtaFracL5RestrResultPtr->Status() == 0 && fitEtaFracL5RestrResultPtr->Ndf() != 0. ) {
                histFracL5RestrEtaFitMeanMap->SetBinContent( uEta + 1, fitEtaFracL5RestrResultPtr->Parameter( 0 ) );
                histFracL5RestrEtaFitMeanMap->SetBinError( uEta + 1, fitEtaFracL5RestrResultPtr->ParError( 0 ) );
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << nameEtaFracL5Restr << "' status " << fitEtaFracL5RestrResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << nameEtaFracL5Restr << std::endl;
              }
            }
            const std::string nameEtaFracL5RestrBkg( nameEtaFracL5Restr + "_bkg" );
            TF1 * bkgEtaFracL5Restr( new TF1( nameEtaFracL5RestrBkg.c_str(), bkgFunction_.c_str(), std::max( histEtaFracL5Restr->GetXaxis()->GetXmin(), histEtaFracL5Restr->GetMean() - histEtaFracL5Restr->GetRMS() * fitRange_ ), std::min( histEtaFracL5Restr->GetXaxis()->GetXmax(), histEtaFracL5Restr->GetMean() + histEtaFracL5Restr->GetRMS() * fitRange_ ) ) );
            my::setParametersBkg( fitEtaFracL5Restr, bkgEtaFracL5Restr );
            if ( overwrite_ ) bkgEtaFracL5Restr->Write( 0, TObject::kOverwrite );
            else              bkgEtaFracL5Restr->Write();
          }

          // Loop over pt bins

          if ( fitEtaPt_ ) {

            // Fit performance histograms
            const std::string nameEtaFracL5PtFitMeanMap( nameEta + "_FracL5Pt_FitMeanMap" );
            TH1D * histEtaFracL5PtFitMeanMap( new TH1D( nameEtaFracL5PtFitMeanMap.c_str(), objCat.c_str(), nPtBins_, ptBins_.data() ) );
            histEtaFracL5PtFitMeanMap->SetXTitle( titlePt.c_str() );
            histEtaFracL5PtFitMeanMap->SetYTitle( titleFracL5Mean.c_str() );
            const std::string nameEtaFracL5RestrPtFitMeanMap( nameEta + "_FracL5RestrPt_FitMeanMap" );
            TH1D * histEtaFracL5RestrPtFitMeanMap( new TH1D( nameEtaFracL5RestrPtFitMeanMap.c_str(), objCat.c_str(), nPtBins_, ptBins_.data() ) );
            histEtaFracL5RestrPtFitMeanMap->SetXTitle( titlePt.c_str() );
            histEtaFracL5RestrPtFitMeanMap->SetYTitle( titleFracL5Mean.c_str() );

            for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
              const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
              const std::string nameEtaPt( nameEta + "_Pt" + binPt );

              const std::string nameEtaPtFracL5( nameEtaPt + "_FracL5" );
              TH1D * histEtaPtFracL5( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaPtFracL5.c_str() ) ) );
              if ( fitNonRestr_ && histEtaPtFracL5 != 0 ) {
                const std::string nameEtaPtFracL5Fit( nameEtaPtFracL5 + "_fit" );
                TF1 * fitEtaPtFracL5( new TF1( nameEtaPtFracL5Fit.c_str(), fitFunction_.c_str(), std::max( histEtaPtFracL5->GetXaxis()->GetXmin(), histEtaPtFracL5->GetMean() - histEtaPtFracL5->GetRMS() * fitRange_ ), std::min( histEtaPtFracL5->GetXaxis()->GetXmax(), histEtaPtFracL5->GetMean() + histEtaPtFracL5->GetRMS() * fitRange_ ) ) );
                my::setParametersFit( fitEtaPtFracL5, histEtaPtFracL5, useBkgFunction_ );
                TFitResultPtr fitEtaPtFracL5ResultPtr( histEtaPtFracL5->Fit( fitEtaPtFracL5, fitOptions_.c_str() ) );
                if ( fitEtaPtFracL5ResultPtr >= 0 ) {
                  if ( fitEtaPtFracL5ResultPtr->Status() == 0 && fitEtaPtFracL5ResultPtr->Ndf() != 0. ) {
                    histEtaFracL5PtFitMeanMap->SetBinContent( uPt + 1, fitEtaPtFracL5ResultPtr->Parameter( 0 ) );
                    histEtaFracL5PtFitMeanMap->SetBinError( uPt + 1, fitEtaPtFracL5ResultPtr->ParError( 0 ) );
                    histFracL5EtaPtFitMeanMap->SetBinContent( uEta + 1, uPt + 1, fitEtaPtFracL5ResultPtr->Parameter( 0 ) );
                    histFracL5EtaPtFitMeanMap->SetBinError( uEta + 1, uPt + 1, fitEtaPtFracL5ResultPtr->ParError( 0 ) );
                    histFracL5EtaPtFitMean->Fill( fitEtaPtFracL5ResultPtr->Parameter( 0 ) );
                    histFracL5EtaPtFitSigma->Fill( fitEtaPtFracL5ResultPtr->ParError( 0 ) );
                  }
                  else {
                    if ( verbose_ > 2 ) {
                      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                                << "    failing fit in directory '"; gDirectory->pwd();
                      std::cout << "    '" << nameEtaPtFracL5 << "' status " << fitEtaPtFracL5ResultPtr->Status() << std::endl;
                    }
                  }
                }
                else {
                  if ( verbose_ > 1 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    missing fit in directory '"; gDirectory->pwd();
                    std::cout << "    '" << nameEtaPtFracL5 << std::endl;
                  }
                }
                const std::string nameEtaPtFracL5Bkg( nameEtaPtFracL5 + "_bkg" );
                TF1 * bkgEtaPtFracL5( new TF1( nameEtaPtFracL5Bkg.c_str(), bkgFunction_.c_str(), std::max( histEtaPtFracL5->GetXaxis()->GetXmin(), histEtaPtFracL5->GetMean() - histEtaPtFracL5->GetRMS() * fitRange_ ), std::min( histEtaPtFracL5->GetXaxis()->GetXmax(), histEtaPtFracL5->GetMean() + histEtaPtFracL5->GetRMS() * fitRange_ ) ) );
                my::setParametersBkg( fitEtaPtFracL5, bkgEtaPtFracL5 );
                if ( overwrite_ ) bkgEtaPtFracL5->Write( 0, TObject::kOverwrite );
                else              bkgEtaPtFracL5->Write();
              }

              const std::string nameEtaPtFracL5Restr( nameEtaPtFracL5 + "Restr" );
              TH1D * histEtaPtFracL5Restr( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaPtFracL5Restr.c_str() ) ) );
              if ( histEtaPtFracL5Restr != 0 ) {
                const std::string nameEtaPtFracL5RestrFit( nameEtaPtFracL5Restr + "_fit" );
                TF1 * fitEtaPtFracL5Restr( new TF1( nameEtaPtFracL5RestrFit.c_str(), fitFunction_.c_str(), std::max( histEtaPtFracL5Restr->GetXaxis()->GetXmin(), histEtaPtFracL5Restr->GetMean() - histEtaPtFracL5Restr->GetRMS() * fitRange_ ), std::min( histEtaPtFracL5Restr->GetXaxis()->GetXmax(), histEtaPtFracL5Restr->GetMean() + histEtaPtFracL5Restr->GetRMS() * fitRange_ ) ) );
                my::setParametersFit( fitEtaPtFracL5Restr, histEtaPtFracL5Restr, useBkgFunction_ );
                TFitResultPtr fitEtaPtFracL5RestrResultPtr( histEtaPtFracL5Restr->Fit( fitEtaPtFracL5Restr, fitOptions_.c_str() ) );
                if ( fitEtaPtFracL5RestrResultPtr >= 0 ) {
                  if ( fitEtaPtFracL5RestrResultPtr->Status() == 0 && fitEtaPtFracL5RestrResultPtr->Ndf() != 0. ) {
                    histEtaFracL5RestrPtFitMeanMap->SetBinContent( uPt + 1, fitEtaPtFracL5RestrResultPtr->Parameter( 0 ) );
                    histEtaFracL5RestrPtFitMeanMap->SetBinError( uPt + 1, fitEtaPtFracL5RestrResultPtr->ParError( 0 ) );
                    histFracL5RestrEtaPtFitMeanMap->SetBinContent( uEta + 1, uPt + 1, fitEtaPtFracL5RestrResultPtr->Parameter( 0 ) );
                    histFracL5RestrEtaPtFitMeanMap->SetBinError( uEta + 1, uPt + 1, fitEtaPtFracL5RestrResultPtr->ParError( 0 ) );
                    histFracL5RestrEtaPtFitMean->Fill( fitEtaPtFracL5RestrResultPtr->Parameter( 0 ) );
                    histFracL5RestrEtaPtFitSigma->Fill( fitEtaPtFracL5RestrResultPtr->ParError( 0 ) );
                  }
                  else {
                    if ( verbose_ > 2 ) {
                      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                                << "    failing fit in directory '"; gDirectory->pwd();
                      std::cout << "    '" << nameEtaPtFracL5Restr << "' status " << fitEtaPtFracL5RestrResultPtr->Status() << std::endl;
                    }
                  }
                }
                else {
                  if ( verbose_ > 1 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    missing fit in directory '"; gDirectory->pwd();
                    std::cout << "    '" << nameEtaPtFracL5Restr << std::endl;
                  }
                }
                const std::string nameEtaPtFracL5RestrBkg( nameEtaPtFracL5Restr + "_bkg" );
                TF1 * bkgEtaPtFracL5Restr( new TF1( nameEtaPtFracL5RestrBkg.c_str(), bkgFunction_.c_str(), std::max( histEtaPtFracL5Restr->GetXaxis()->GetXmin(), histEtaPtFracL5Restr->GetMean() - histEtaPtFracL5Restr->GetRMS() * fitRange_ ), std::min( histEtaPtFracL5Restr->GetXaxis()->GetXmax(), histEtaPtFracL5Restr->GetMean() + histEtaPtFracL5Restr->GetRMS() * fitRange_ ) ) );
                my::setParametersBkg( fitEtaPtFracL5Restr, bkgEtaPtFracL5Restr );
                if ( overwrite_ ) bkgEtaPtFracL5Restr->Write( 0, TObject::kOverwrite );
                else              bkgEtaPtFracL5Restr->Write();
              }

            } // loop: uPt < nPtBins_

          }

        } // loop: keyEta

        if ( writeFiles_ ) {

          // File name
          std::string nameOut( pathOut_ + "/gentJecL5_" + sample_ + "_" + name );
          if ( usePileUp_ ) nameOut.append( "_PileUp" );
          if ( refSel_)     nameOut.append( "_Ref" );
          nameOut.append( ".txt" );

          ofstream fileOut;
          fileOut.open( nameOut.c_str(), std::ios_base::out );

          fileOut.close();
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    written L5 JEC file:" << std::endl
                    << "        " << nameOut << std::endl;
        }

      } // loop: keyFit

    } // loop: uCat < objCats_.size()

  }


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
