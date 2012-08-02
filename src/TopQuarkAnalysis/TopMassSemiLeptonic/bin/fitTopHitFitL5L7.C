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


namespace my
{

  void setParametersFit( TF1 * fit, TH1D * histo )
  {
//     double m( histo->GetMean() );
//     double c( histo->GetBinContent( histo->FindBin( histo->GetMean() ) ) );
//     double m( histo->GetBinCenter( histo->GetMaximumBin() ) );
//     double c( histo->GetBinContent( histo->GetMaximumBin() ) );
    double m( 1. );
    double c( histo->GetBinContent( histo->FindBin( 1. ) ) );
// //     double c( histo->GetBinContent( histo->GetMaximumBin() ) * std::sqrt( TMath::TwoPi() ) * histo->GetRMS() );
    double s( histo->GetRMS() );
    // Gaussian part
    fit->SetParameter( 0, m );
    fit->SetParLimits( 0, 0., 2. );
    fit->SetParameter( 1, c );
    fit->SetParLimits( 1, 0., 2. * c );
    fit->SetParameter( 2, s );
    fit->SetParLimits( 2, 0., 999999. );
    // Additional part
//     fit->SetParameter( 3, log( (m-2.*s)*(m-2.*s) / std::sqrt( s*s + (m-2.*s)*(m-2.*s) ) ) );
    fit->SetParameter( 3, 0. );
    fit->SetParameter( 4, c ); // just a rough guess, which should serve both cases (highest peak in "signal" or background)
    fit->SetParLimits( 4, 0., 2. * c );
//     fit->SetParameter( 5, std::sqrt( log( s*s / (m-2.*s)*(m-2.*s) + 1. ) ) );
    fit->SetParameter( 5, 1. );
    fit->SetParLimits( 5, 0., 999999. );
    // Parameter names
    fit->SetParNames( "Gaussian #Mu", "Gaussian c", "Gaussian #sigma", "log-normal #mu", "log-normal c", "log-normal #sigma" );
  }

  void setParametersBkg( TF1 * fit, TF1 * bkg )
  {
    Int_t n( fit->GetNumberFreeParameters() );
    for ( Int_t i = 3; i < n; ++i ) {
      bkg->SetParameter( i, fit->GetParameter( i ) );
    }
  }

}


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
  // Configuration for fitting L5L7 JECs
  const edm::ParameterSet & fit_( process_.getParameter< edm::ParameterSet >( "fit" ) );
  const bool fitNonRestr_( fit_.getParameter< bool >( "fitNonRestr" ) );
  const bool fitEtaPt_( fit_.getParameter< bool >( "fitEtaPt" ) );
  const std::string fitFunction_( fit_.getParameter< std::string >( "fitFunction" ) );
  std::string fitOptions_( fit_.getParameter< std::string >( "fitOptions" ) );
  const double fitRange_( std::min( fit_.getParameter< double >( "fitRange" ), widthFactor_ ) );
  const std::string bkgFunction_( fit_.getParameter< std::string >( "bkgFunction" ) );
  const double minPtParton_( fit_.getParameter< double >( "minPtParton" ) );
  const double maxDRParton_( fit_.getParameter< double >( "maxDRParton" ) );
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
  const std::string titlePt( refGen_ ? "p_{t}^GEN (GeV)" : "p_{t} (GeV)" );
  const std::string titleEta( refGen_ ? "#eta^GEN" : "#eta" );
  std::string titleFracL5L7( refGen_ ? "#frac{p_{t}^{RECO}}{p_{t}^{GEN}}" : "#frac{p_{t}^{GEN}}{p_{t}^{RECO}}" );
  if ( useAlt_ ) {
    titleFracL5L7 = ( refGen_ ? "#frac{p_{t}^{RECO,alt.}}{p_{t}^{GEN}}" : "#frac{p_{t}^{GEN}}{p_{t}^{RECO,alt.}}" );
  }
  const std::string titleFracL5L7Mean( "#mu of " + titleFracL5L7 );
  const std::string titleFracL5L7Sigma( "#sigma of " + titleFracL5L7 );
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
      if ( useAlt_  == ( subFit.find( "Alt" )  == std::string::npos ) ) continue;
      if ( useSymm_ == ( subFit.find( "Symm" ) == std::string::npos ) ) continue;
      if ( refGen_  == ( subFit.find( "Gen" )  == std::string::npos ) ) continue;
      TDirectory * dirFit_( dynamic_cast< TDirectory* >( dirPt_->Get( subFit.c_str() ) ) );
      dirFit_->cd();

      const std::string name( objCat + "_Pt_" + subFit );

      const std::string nameFracL5L7( name + "_FracL5L7" );
      TH1D * histFracL5L7( new TH1D( nameFracL5L7.c_str(), objCat.c_str(), nPtBins, 0., 2. ) );
      histFracL5L7->SetXTitle( titleFracL5L7.c_str() );
      histFracL5L7->SetYTitle( titleEvents.c_str() );

      const std::string nameFracL5L7Restr( nameFracL5L7 + "Restr" );
      TH1D * histFracL5L7Restr( new TH1D( nameFracL5L7Restr.c_str(), objCat.c_str(), nPtBins, 0., 2. ) );
      histFracL5L7Restr->SetXTitle( titleFracL5L7.c_str() );
      histFracL5L7Restr->SetYTitle( titleEvents.c_str() );

      std::vector< TH1D * > histVecPtFracL5L7;
      std::vector< TH1D * > histVecPtFracL5L7Restr;
      for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
        const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
        const std::string namePt( name + "_Pt" + binPt );
        const std::string titlePtDelta( objCat + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq p_{t} #leq " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
        const std::string namePtFracL5L7( namePt + "_FracL5L7" );
        TH1D * histPtFracL5L7( new TH1D( namePtFracL5L7.c_str(), titlePtDelta.c_str(), nPtBins, 0., 2. ) );
        histPtFracL5L7->SetXTitle( titleFracL5L7.c_str() );
        histPtFracL5L7->SetYTitle( titleEvents.c_str() );
        histVecPtFracL5L7.push_back( histPtFracL5L7 );
        const std::string namePtFracL5L7Restr( namePtFracL5L7 + "Restr" );
        TH1D * histPtFracL5L7Restr( new TH1D( namePtFracL5L7Restr.c_str(), titlePtDelta.c_str(), nPtBins, 0., 2. ) );
        histPtFracL5L7Restr->SetXTitle( titleFracL5L7.c_str() );
        histPtFracL5L7Restr->SetYTitle( titleEvents.c_str() );
        histVecPtFracL5L7Restr.push_back( histPtFracL5L7Restr );
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

        const std::string nameEtaFracL5L7( nameEta + "_FracL5L7" );
        const std::string titleEtaFracL5L7( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) );
        TH1D * histEtaFracL5L7( new TH1D( nameEtaFracL5L7.c_str(), titleEtaFracL5L7.c_str(), nPtBins, 0., 2. ) );
        histEtaFracL5L7->SetXTitle( titleFracL5L7.c_str() );
        histEtaFracL5L7->SetYTitle( titleEvents.c_str() );

        const std::string nameEtaFracL5L7Restr( nameEtaFracL5L7 + "Restr" );
        TH1D * histEtaFracL5L7Restr( new TH1D( nameEtaFracL5L7Restr.c_str(), titleEtaFracL5L7.c_str(), nPtBins, 0., 2. ) );
        histEtaFracL5L7Restr->SetXTitle( titleFracL5L7.c_str() );
        histEtaFracL5L7Restr->SetYTitle( titleEvents.c_str() );

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
              weightEtaBin.at( uPt ).push_back( weightData_.at( uEta ).at( uEntry ) );
              ptEtaBin.at( uPt ).push_back( ptData_.at( uEta ).at( uEntry ) );
              ptGenEtaBin.at( uPt ).push_back( ptGenData_.at( uEta ).at( uEntry ) );
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
          const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
          const std::string nameEtaPt( nameEta + "_Pt" + binPt );

          const std::string nameEtaPtFracL5L7( nameEtaPt + "_FracL5L7" );
          const std::string titleEtaPtFracL5L7( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq p_{t} #leq " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
          TH1D * histEtaPtFracL5L7( new TH1D( nameEtaPtFracL5L7.c_str(), titleEtaPtFracL5L7.c_str(), nPtBins, 0., 2. ) );
          histEtaPtFracL5L7->SetXTitle( titleFracL5L7.c_str() );
          histEtaPtFracL5L7->SetYTitle( titleEvents.c_str() );

          const std::string nameEtaPtFracL5L7Restr( nameEtaPtFracL5L7 + "Restr" );
          TH1D * histEtaPtFracL5L7Restr( new TH1D( nameEtaPtFracL5L7Restr.c_str(), titleEtaPtFracL5L7.c_str(), nPtBins, 0., 2. ) );
          histEtaPtFracL5L7Restr->SetXTitle( titleFracL5L7.c_str() );
          histEtaPtFracL5L7Restr->SetYTitle( titleEvents.c_str() );

          for ( unsigned uEntry = 0; uEntry < sizePt.at( uPt ); ++uEntry ) {
            if ( refGen_ ) {
              if ( fitNonRestr_ ) {
                if ( fitEtaPt_ ) histEtaPtFracL5L7->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histVecPtFracL5L7.at( uPt )->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaFracL5L7->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histFracL5L7->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
              if ( ptGenEtaBin.at( uPt ).at( uEntry ) >= minPtParton_ && reco::deltaR( etaGenEtaBin.at( uPt ).at( uEntry ), phiGenEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDRParton_ ) {
                if ( fitEtaPt_ ) histEtaPtFracL5L7Restr->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histVecPtFracL5L7Restr.at( uPt )->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaFracL5L7Restr->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histFracL5L7Restr->Fill( ptEtaBin.at( uPt ).at( uEntry ) / ptGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
            }
            else {
              if ( fitNonRestr_ ) {
                if ( fitEtaPt_ ) histEtaPtFracL5L7->Fill( ptGenEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histVecPtFracL5L7.at( uPt )->Fill( ptGenEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaFracL5L7->Fill( ptGenEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histFracL5L7->Fill( ptGenEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
              if ( ptGenEtaBin.at( uPt ).at( uEntry ) >= minPtParton_ && reco::deltaR( etaGenEtaBin.at( uPt ).at( uEntry ), phiGenEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDRParton_ ) {
                if ( fitEtaPt_ ) histEtaPtFracL5L7Restr->Fill( ptGenEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histVecPtFracL5L7Restr.at( uPt )->Fill( ptGenEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histEtaFracL5L7Restr->Fill( ptGenEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                histFracL5L7Restr->Fill( ptGenEtaBin.at( uPt ).at( uEntry ) / ptEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
            }
          } // loop: uEntry < ptEtaBin.at( uPt ).size()

          if ( fitEtaPt_ ) {
            if ( fitNonRestr_ ) histEtaPtFracL5L7->Scale( 1. / histEtaPtFracL5L7->Integral() );
            histEtaPtFracL5L7Restr->Scale( 1. / histEtaPtFracL5L7Restr->Integral() );
          }

        } // loop: uPt < nPtBins_

        if ( fitNonRestr_ ) histEtaFracL5L7->Scale( 1. / histEtaFracL5L7->Integral() );
        histEtaFracL5L7Restr->Scale( 1. / histEtaFracL5L7Restr->Integral() );

      } // loop: keyEta

      if ( fitNonRestr_ ) histFracL5L7->Scale( 1. / histFracL5L7->Integral() );
      histFracL5L7Restr->Scale( 1. / histFracL5L7Restr->Integral() );
      if ( fitNonRestr_ ) {
        for ( unsigned uPt = 0; uPt < histVecPtFracL5L7.size(); ++uPt ) {
          histVecPtFracL5L7.at( uPt )->Scale( 1. / histVecPtFracL5L7.at( uPt )->Integral() );
        }
      }
      for ( unsigned uPt = 0; uPt < histVecPtFracL5L7Restr.size(); ++uPt ) {
        histVecPtFracL5L7Restr.at( uPt )->Scale( 1. / histVecPtFracL5L7Restr.at( uPt )->Integral() );
      }

      dirFit_->cd();

      if ( sizeEtaBins != nEtaBins_ ) {
        std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                  << "    mismatch of eta binning for object category '" << objCat << "':" << std::endl
                  << "        bins in directory structure: " << sizeEtaBins                << std::endl
                  << "        bins in binning histogram  : " << nEtaBins_                  << std::endl;
      }

    } // loop: keyFit


    // Fit L5L7 JECs

    if ( objCat == "UdscJet" || objCat == "BJet" ) {
      if ( verbose_ > 1 ) {
        std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                  << "    JEC L5L7 determination for " << objCat << " started" << std::endl;
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
        if ( useAlt_  == ( subFit.find( "Alt" )  == std::string::npos ) ) continue;
        if ( useSymm_ == ( subFit.find( "Symm" ) == std::string::npos ) ) continue;
        if ( refGen_  == ( subFit.find( "Gen" )  == std::string::npos ) ) continue;
        TDirectory * dirFit_( dynamic_cast< TDirectory* >( dirPt_->Get( subFit.c_str() ) ) );
        dirFit_->cd();

        const std::string name( objCat + "_Pt_" + subFit );

        const std::string nameFracL5L7( name + "_FracL5L7" );
        TH1D * histFracL5L7( dynamic_cast< TH1D* >( gDirectory->Get( nameFracL5L7.c_str() ) ) );
        if ( fitNonRestr_ && histFracL5L7 != 0 ) {
          const std::string nameFracL5L7Fit( nameFracL5L7 + "_fit" );
          TF1 * fitFracL5L7( new TF1( nameFracL5L7Fit.c_str(), fitFunction_.c_str(), std::max( histFracL5L7->GetXaxis()->GetXmin(), histFracL5L7->GetMean() - histFracL5L7->GetRMS() * fitRange_ ), std::min( histFracL5L7->GetXaxis()->GetXmax(), histFracL5L7->GetMean() + histFracL5L7->GetRMS() * fitRange_ ) ) );
          my::setParametersFit( fitFracL5L7, histFracL5L7 );
          TFitResultPtr fitFracL5L7ResultPtr( histFracL5L7->Fit( fitFracL5L7, fitOptions_.c_str() ) );
          if ( fitFracL5L7ResultPtr >= 0 ) {
            if ( fitFracL5L7ResultPtr->Status() == 0 && fitFracL5L7ResultPtr->Ndf() != 0. ) {
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << nameFracL5L7 << "' status " << fitFracL5L7ResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; gDirectory->pwd();
              std::cout << "    '" << nameFracL5L7 << std::endl;
            }
          }
          const std::string nameFracL5L7Bkg( nameFracL5L7 + "_bkg" );
          TF1 * bkgFracL5L7( new TF1( nameFracL5L7Bkg.c_str(), bkgFunction_.c_str(), std::max( histFracL5L7->GetXaxis()->GetXmin(), histFracL5L7->GetMean() - histFracL5L7->GetRMS() * fitRange_ ), std::min( histFracL5L7->GetXaxis()->GetXmax(), histFracL5L7->GetMean() + histFracL5L7->GetRMS() * fitRange_ ) ) );
          my::setParametersBkg( fitFracL5L7, bkgFracL5L7 );
          if ( overwrite_ ) bkgFracL5L7->Write( 0, TObject::kOverwrite );
          else              bkgFracL5L7->Write();
        }

        const std::string nameFracL5L7Restr( nameFracL5L7 + "Restr" );
        TH1D * histFracL5L7Restr( dynamic_cast< TH1D* >( gDirectory->Get( nameFracL5L7Restr.c_str() ) ) );
        if ( histFracL5L7Restr != 0 ) {
          const std::string nameFracL5L7RestrFit( nameFracL5L7Restr + "_fit" );
          TF1 * fitFracL5L7Restr( new TF1( nameFracL5L7RestrFit.c_str(), fitFunction_.c_str(), std::max( histFracL5L7Restr->GetXaxis()->GetXmin(), histFracL5L7Restr->GetMean() - histFracL5L7Restr->GetRMS() * fitRange_ ), std::min( histFracL5L7Restr->GetXaxis()->GetXmax(), histFracL5L7Restr->GetMean() + histFracL5L7Restr->GetRMS() * fitRange_ ) ) );
          my::setParametersFit( fitFracL5L7Restr, histFracL5L7Restr );
          TFitResultPtr fitFracL5L7RestrResultPtr( histFracL5L7Restr->Fit( fitFracL5L7Restr, fitOptions_.c_str() ) );
          if ( fitFracL5L7RestrResultPtr >= 0 ) {
            if ( fitFracL5L7RestrResultPtr->Status() == 0 && fitFracL5L7RestrResultPtr->Ndf() != 0. ) {
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << nameFracL5L7Restr << "' status " << fitFracL5L7RestrResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; gDirectory->pwd();
              std::cout << "    '" << nameFracL5L7Restr << std::endl;
            }
          }
          const std::string nameFracL5L7RestrBkg( nameFracL5L7Restr + "_bkg" );
          TF1 * bkgFracL5L7Restr( new TF1( nameFracL5L7RestrBkg.c_str(), bkgFunction_.c_str(), std::max( histFracL5L7Restr->GetXaxis()->GetXmin(), histFracL5L7Restr->GetMean() - histFracL5L7Restr->GetRMS() * fitRange_ ), std::min( histFracL5L7Restr->GetXaxis()->GetXmax(), histFracL5L7Restr->GetMean() + histFracL5L7Restr->GetRMS() * fitRange_ ) ) );
          my::setParametersBkg( fitFracL5L7Restr, bkgFracL5L7Restr );
          if ( overwrite_ ) bkgFracL5L7Restr->Write( 0, TObject::kOverwrite );
          else              bkgFracL5L7Restr->Write();
        }

        // Loop over pt bins

        // Fit performance histograms
        const std::string nameFracL5L7PtFitMeanMap( name + "_FracL5L7Pt_FitMeanMap" );
        TH1D * histFracL5L7PtFitMeanMap( new TH1D( nameFracL5L7PtFitMeanMap.c_str(), objCat.c_str(), nPtBins_, ptBins_.data() ) );
        histFracL5L7PtFitMeanMap->SetXTitle( titlePt.c_str() );
        histFracL5L7PtFitMeanMap->SetYTitle( titleFracL5L7Mean.c_str() );
        const std::string nameFracL5L7RestrPtFitMeanMap( name + "_FracL5L7RestrPt_FitMeanMap" );
        TH1D * histFracL5L7RestrPtFitMeanMap( new TH1D( nameFracL5L7RestrPtFitMeanMap.c_str(), objCat.c_str(), nPtBins_, ptBins_.data() ) );
        histFracL5L7RestrPtFitMeanMap->SetXTitle( titlePt.c_str() );
        histFracL5L7RestrPtFitMeanMap->SetYTitle( titleFracL5L7Mean.c_str() );

        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
          const std::string namePt( name + "_Pt" + binPt );

          const std::string namePtFracL5L7( namePt + "_FracL5L7" );
          TH1D * histPtFracL5L7( dynamic_cast< TH1D* >( gDirectory->Get( namePtFracL5L7.c_str() ) ) );
          if ( fitNonRestr_ && histPtFracL5L7 != 0 ) {
            const std::string namePtFracL5L7Fit( namePtFracL5L7 + "_fit" );
            TF1 * fitPtFracL5L7( new TF1( namePtFracL5L7Fit.c_str(), fitFunction_.c_str(), std::max( histPtFracL5L7->GetXaxis()->GetXmin(), histPtFracL5L7->GetMean() - histPtFracL5L7->GetRMS() * fitRange_ ), std::min( histPtFracL5L7->GetXaxis()->GetXmax(), histPtFracL5L7->GetMean() + histPtFracL5L7->GetRMS() * fitRange_ ) ) );
            my::setParametersFit( fitPtFracL5L7, histPtFracL5L7 );
            TFitResultPtr fitPtFracL5L7ResultPtr( histPtFracL5L7->Fit( fitPtFracL5L7, fitOptions_.c_str() ) );
            if ( fitPtFracL5L7ResultPtr >= 0 ) {
              if ( fitPtFracL5L7ResultPtr->Status() == 0 && fitPtFracL5L7ResultPtr->Ndf() != 0. ) {
                histFracL5L7PtFitMeanMap->SetBinContent( uPt + 1, fitPtFracL5L7ResultPtr->Parameter( 0 ) );
                histFracL5L7PtFitMeanMap->SetBinError( uPt + 1, fitPtFracL5L7ResultPtr->ParError( 0 ) );
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << namePtFracL5L7 << "' status " << fitPtFracL5L7ResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << namePtFracL5L7 << std::endl;
              }
            }
            const std::string namePtFracL5L7Bkg( namePtFracL5L7 + "_bkg" );
            TF1 * bkgPtFracL5L7( new TF1( namePtFracL5L7Bkg.c_str(), bkgFunction_.c_str(), std::max( histPtFracL5L7->GetXaxis()->GetXmin(), histPtFracL5L7->GetMean() - histPtFracL5L7->GetRMS() * fitRange_ ), std::min( histPtFracL5L7->GetXaxis()->GetXmax(), histPtFracL5L7->GetMean() + histPtFracL5L7->GetRMS() * fitRange_ ) ) );
            my::setParametersBkg( fitPtFracL5L7, bkgPtFracL5L7 );
            if ( overwrite_ ) bkgPtFracL5L7->Write( 0, TObject::kOverwrite );
            else              bkgPtFracL5L7->Write();
          }

          const std::string namePtFracL5L7Restr( namePtFracL5L7 + "Restr" );
          TH1D * histPtFracL5L7Restr( dynamic_cast< TH1D* >( gDirectory->Get( namePtFracL5L7Restr.c_str() ) ) );
          if ( histPtFracL5L7Restr != 0 ) {
            const std::string namePtFracL5L7RestrFit( namePtFracL5L7Restr + "_fit" );
            TF1 * fitPtFracL5L7Restr( new TF1( namePtFracL5L7RestrFit.c_str(), fitFunction_.c_str(), std::max( histPtFracL5L7Restr->GetXaxis()->GetXmin(), histPtFracL5L7Restr->GetMean() - histPtFracL5L7Restr->GetRMS() * fitRange_ ), std::min( histPtFracL5L7Restr->GetXaxis()->GetXmax(), histPtFracL5L7Restr->GetMean() + histPtFracL5L7Restr->GetRMS() * fitRange_ ) ) );
            my::setParametersFit( fitPtFracL5L7Restr, histPtFracL5L7Restr );
            TFitResultPtr fitPtFracL5L7RestrResultPtr( histPtFracL5L7Restr->Fit( fitPtFracL5L7Restr, fitOptions_.c_str() ) );
            if ( fitPtFracL5L7RestrResultPtr >= 0 ) {
              if ( fitPtFracL5L7RestrResultPtr->Status() == 0 && fitPtFracL5L7RestrResultPtr->Ndf() != 0. ) {
                histFracL5L7RestrPtFitMeanMap->SetBinContent( uPt + 1, fitPtFracL5L7RestrResultPtr->Parameter( 0 ) );
                histFracL5L7RestrPtFitMeanMap->SetBinError( uPt + 1, fitPtFracL5L7RestrResultPtr->ParError( 0 ) );
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << namePtFracL5L7Restr << "' status " << fitPtFracL5L7RestrResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << namePtFracL5L7Restr << std::endl;
              }
            }
            const std::string namePtFracL5L7RestrBkg( namePtFracL5L7Restr + "_bkg" );
            TF1 * bkgPtFracL5L7Restr( new TF1( namePtFracL5L7RestrBkg.c_str(), bkgFunction_.c_str(), std::max( histPtFracL5L7Restr->GetXaxis()->GetXmin(), histPtFracL5L7Restr->GetMean() - histPtFracL5L7Restr->GetRMS() * fitRange_ ), std::min( histPtFracL5L7Restr->GetXaxis()->GetXmax(), histPtFracL5L7Restr->GetMean() + histPtFracL5L7Restr->GetRMS() * fitRange_ ) ) );
            my::setParametersBkg( fitPtFracL5L7Restr, bkgPtFracL5L7Restr );
            if ( overwrite_ ) bkgPtFracL5L7Restr->Write( 0, TObject::kOverwrite );
            else              bkgPtFracL5L7Restr->Write();
          }

        } // loop: uPt < nPtBins_

        // Loop over eta bins

        // Fit performance histograms
        const std::string nameFracL5L7EtaFitMeanMap( name + "_FracL5L7Eta_FitMeanMap" );
        TH1D * histFracL5L7EtaFitMeanMap( new TH1D( nameFracL5L7EtaFitMeanMap.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data() ) );
        histFracL5L7EtaFitMeanMap->SetXTitle( titleEta.c_str() );
        histFracL5L7EtaFitMeanMap->SetYTitle( titleFracL5L7Mean.c_str() );
        const std::string nameFracL5L7EtaPtFitMeanMap( name + "_FracL5L7EtaPt_FitMeanMap" );
        TH2D * histFracL5L7EtaPtFitMeanMap( new TH2D( nameFracL5L7EtaPtFitMeanMap.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
        histFracL5L7EtaPtFitMeanMap->SetXTitle( titleEta.c_str() );
        histFracL5L7EtaPtFitMeanMap->SetYTitle( titlePt.c_str() );
        histFracL5L7EtaPtFitMeanMap->SetZTitle( titleFracL5L7Mean.c_str() );
        const std::string nameFracL5L7EtaPtFitMean( name + "_FracL5L7EtaPt_FitMean" );
        TH1D * histFracL5L7EtaPtFitMean( new TH1D( nameFracL5L7EtaPtFitMean.c_str(), titleMean.c_str(), 20, 0.75, 1.25 ) );
        histFracL5L7EtaPtFitMean->SetXTitle( titleFracL5L7Mean.c_str() );
        histFracL5L7EtaPtFitMean->SetYTitle( titleFits.c_str() );
        const std::string nameFracL5L7EtaPtFitSigma( name + "_FracL5L7EtaPt_FitSigma" );
        TH1D * histFracL5L7EtaPtFitSigma( new TH1D( nameFracL5L7EtaPtFitSigma.c_str(), titleSigma.c_str(), 20, 0., 1. ) );
        histFracL5L7EtaPtFitSigma->SetXTitle( titleFracL5L7Sigma.c_str() );
        histFracL5L7EtaPtFitSigma->SetYTitle( titleFits.c_str() );
        const std::string nameFracL5L7RestrEtaFitMeanMap( name + "_FracL5L7RestrEta_FitMeanMap" );
        TH1D * histFracL5L7RestrEtaFitMeanMap( new TH1D( nameFracL5L7RestrEtaFitMeanMap.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data() ) );
        histFracL5L7RestrEtaFitMeanMap->SetXTitle( titleEta.c_str() );
        histFracL5L7RestrEtaFitMeanMap->SetYTitle( titleFracL5L7Mean.c_str() );
        const std::string nameFracL5L7RestrEtaPtFitMeanMap( name + "_FracL5L7RestrEtaPt_FitMeanMap" );
        TH2D * histFracL5L7RestrEtaPtFitMeanMap( new TH2D( nameFracL5L7RestrEtaPtFitMeanMap.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
        histFracL5L7RestrEtaPtFitMeanMap->SetXTitle( titleEta.c_str() );
        histFracL5L7RestrEtaPtFitMeanMap->SetYTitle( titlePt.c_str() );
        histFracL5L7RestrEtaPtFitMeanMap->SetZTitle( titleFracL5L7Mean.c_str() );
        const std::string nameFracL5L7RestrEtaPtFitMean( name + "_FracL5L7RestrEtaPt_FitMean" );
        TH1D * histFracL5L7RestrEtaPtFitMean( new TH1D( nameFracL5L7RestrEtaPtFitMean.c_str(), titleMean.c_str(), 20, 0.75, 1.25 ) );
        histFracL5L7RestrEtaPtFitMean->SetXTitle( titleFracL5L7Mean.c_str() );
        histFracL5L7RestrEtaPtFitMean->SetYTitle( titleFits.c_str() );
        const std::string nameFracL5L7RestrEtaPtFitSigma( name + "_FracL5L7RestrEtaPt_FitSigma" );
        TH1D * histFracL5L7RestrEtaPtFitSigma( new TH1D( nameFracL5L7RestrEtaPtFitSigma.c_str(), titleSigma.c_str(), 20, 0., 1. ) );
        histFracL5L7RestrEtaPtFitSigma->SetXTitle( titleFracL5L7Sigma.c_str() );
        histFracL5L7RestrEtaPtFitSigma->SetYTitle( titleFits.c_str() );

        TList * listFit( dirFit_->GetListOfKeys() );
        TIter nextInListFit( listFit );
        while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
          if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
          const std::string binEta( keyEta->GetName() );
          const unsigned uEta( std::atoi( binEta.substr( 3 ).data() ) );
          dirFit_->cd( binEta.c_str() );

          const std::string nameEta( name + "_" + binEta );

          const std::string nameEtaFracL5L7( nameEta + "_FracL5L7" );
          TH1D * histEtaFracL5L7( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaFracL5L7.c_str() ) ) );
          if ( fitNonRestr_ && histEtaFracL5L7 != 0 ) {
            const std::string nameEtaFracL5L7Fit( nameEtaFracL5L7 + "_fit" );
            TF1 * fitEtaFracL5L7( new TF1( nameEtaFracL5L7Fit.c_str(), fitFunction_.c_str(), std::max( histEtaFracL5L7->GetXaxis()->GetXmin(), histEtaFracL5L7->GetMean() - histEtaFracL5L7->GetRMS() * fitRange_ ), std::min( histEtaFracL5L7->GetXaxis()->GetXmax(), histEtaFracL5L7->GetMean() + histEtaFracL5L7->GetRMS() * fitRange_ ) ) );
            my::setParametersFit( fitEtaFracL5L7, histEtaFracL5L7 );
            TFitResultPtr fitEtaFracL5L7ResultPtr( histEtaFracL5L7->Fit( fitEtaFracL5L7, fitOptions_.c_str() ) );
            if ( fitEtaFracL5L7ResultPtr >= 0 ) {
              if ( fitEtaFracL5L7ResultPtr->Status() == 0 && fitEtaFracL5L7ResultPtr->Ndf() != 0. ) {
                histFracL5L7EtaFitMeanMap->SetBinContent( uEta + 1, fitEtaFracL5L7ResultPtr->Parameter( 0 ) );
                histFracL5L7EtaFitMeanMap->SetBinError( uEta + 1, fitEtaFracL5L7ResultPtr->ParError( 0 ) );
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << nameEtaFracL5L7 << "' status " << fitEtaFracL5L7ResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << nameEtaFracL5L7 << std::endl;
              }
            }
            const std::string nameEtaFracL5L7Bkg( nameEtaFracL5L7 + "_bkg" );
            TF1 * bkgEtaFracL5L7( new TF1( nameEtaFracL5L7Bkg.c_str(), bkgFunction_.c_str(), std::max( histEtaFracL5L7->GetXaxis()->GetXmin(), histEtaFracL5L7->GetMean() - histEtaFracL5L7->GetRMS() * fitRange_ ), std::min( histEtaFracL5L7->GetXaxis()->GetXmax(), histEtaFracL5L7->GetMean() + histEtaFracL5L7->GetRMS() * fitRange_ ) ) );
            my::setParametersBkg( fitEtaFracL5L7, bkgEtaFracL5L7 );
            if ( overwrite_ ) bkgEtaFracL5L7->Write( 0, TObject::kOverwrite );
            else              bkgEtaFracL5L7->Write();
          }

          const std::string nameEtaFracL5L7Restr( nameEtaFracL5L7 + "Restr" );
          TH1D * histEtaFracL5L7Restr( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaFracL5L7Restr.c_str() ) ) );
          if ( histEtaFracL5L7Restr != 0 ) {
            const std::string nameEtaFracL5L7RestrFit( nameEtaFracL5L7Restr + "_fit" );
            TF1 * fitEtaFracL5L7Restr( new TF1( nameEtaFracL5L7RestrFit.c_str(), fitFunction_.c_str(), std::max( histEtaFracL5L7Restr->GetXaxis()->GetXmin(), histEtaFracL5L7Restr->GetMean() - histEtaFracL5L7Restr->GetRMS() * fitRange_ ), std::min( histEtaFracL5L7Restr->GetXaxis()->GetXmax(), histEtaFracL5L7Restr->GetMean() + histEtaFracL5L7Restr->GetRMS() * fitRange_ ) ) );
            my::setParametersFit( fitEtaFracL5L7Restr, histEtaFracL5L7Restr );
            TFitResultPtr fitEtaFracL5L7RestrResultPtr( histEtaFracL5L7Restr->Fit( fitEtaFracL5L7Restr, fitOptions_.c_str() ) );
            if ( fitEtaFracL5L7RestrResultPtr >= 0 ) {
              if ( fitEtaFracL5L7RestrResultPtr->Status() == 0 && fitEtaFracL5L7RestrResultPtr->Ndf() != 0. ) {
                histFracL5L7RestrEtaFitMeanMap->SetBinContent( uEta + 1, fitEtaFracL5L7RestrResultPtr->Parameter( 0 ) );
                histFracL5L7RestrEtaFitMeanMap->SetBinError( uEta + 1, fitEtaFracL5L7RestrResultPtr->ParError( 0 ) );
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << nameEtaFracL5L7Restr << "' status " << fitEtaFracL5L7RestrResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << nameEtaFracL5L7Restr << std::endl;
              }
            }
            const std::string nameEtaFracL5L7RestrBkg( nameEtaFracL5L7Restr + "_bkg" );
            TF1 * bkgEtaFracL5L7Restr( new TF1( nameEtaFracL5L7RestrBkg.c_str(), bkgFunction_.c_str(), std::max( histEtaFracL5L7Restr->GetXaxis()->GetXmin(), histEtaFracL5L7Restr->GetMean() - histEtaFracL5L7Restr->GetRMS() * fitRange_ ), std::min( histEtaFracL5L7Restr->GetXaxis()->GetXmax(), histEtaFracL5L7Restr->GetMean() + histEtaFracL5L7Restr->GetRMS() * fitRange_ ) ) );
            my::setParametersBkg( fitEtaFracL5L7Restr, bkgEtaFracL5L7Restr );
            if ( overwrite_ ) bkgEtaFracL5L7Restr->Write( 0, TObject::kOverwrite );
            else              bkgEtaFracL5L7Restr->Write();
          }

          // Loop over pt bins

          if ( fitEtaPt_ ) {

            // Fit performance histograms
            const std::string nameEtaFracL5L7PtFitMeanMap( nameEta + "_FracL5L7Pt_FitMeanMap" );
            TH1D * histEtaFracL5L7PtFitMeanMap( new TH1D( nameEtaFracL5L7PtFitMeanMap.c_str(), objCat.c_str(), nPtBins_, ptBins_.data() ) );
            histEtaFracL5L7PtFitMeanMap->SetXTitle( titlePt.c_str() );
            histEtaFracL5L7PtFitMeanMap->SetYTitle( titleFracL5L7Mean.c_str() );
            const std::string nameEtaFracL5L7RestrPtFitMeanMap( nameEta + "_FracL5L7RestrPt_FitMeanMap" );
            TH1D * histEtaFracL5L7RestrPtFitMeanMap( new TH1D( nameEtaFracL5L7RestrPtFitMeanMap.c_str(), objCat.c_str(), nPtBins_, ptBins_.data() ) );
            histEtaFracL5L7RestrPtFitMeanMap->SetXTitle( titlePt.c_str() );
            histEtaFracL5L7RestrPtFitMeanMap->SetYTitle( titleFracL5L7Mean.c_str() );

            for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
              const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
              const std::string nameEtaPt( nameEta + "_Pt" + binPt );

              const std::string nameEtaPtFracL5L7( nameEtaPt + "_FracL5L7" );
              TH1D * histEtaPtFracL5L7( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaPtFracL5L7.c_str() ) ) );
              if ( fitNonRestr_ && histEtaPtFracL5L7 != 0 ) {
                const std::string nameEtaPtFracL5L7Fit( nameEtaPtFracL5L7 + "_fit" );
                TF1 * fitEtaPtFracL5L7( new TF1( nameEtaPtFracL5L7Fit.c_str(), fitFunction_.c_str(), std::max( histEtaPtFracL5L7->GetXaxis()->GetXmin(), histEtaPtFracL5L7->GetMean() - histEtaPtFracL5L7->GetRMS() * fitRange_ ), std::min( histEtaPtFracL5L7->GetXaxis()->GetXmax(), histEtaPtFracL5L7->GetMean() + histEtaPtFracL5L7->GetRMS() * fitRange_ ) ) );
                my::setParametersFit( fitEtaPtFracL5L7, histEtaPtFracL5L7 );
                TFitResultPtr fitEtaPtFracL5L7ResultPtr( histEtaPtFracL5L7->Fit( fitEtaPtFracL5L7, fitOptions_.c_str() ) );
                if ( fitEtaPtFracL5L7ResultPtr >= 0 ) {
                  if ( fitEtaPtFracL5L7ResultPtr->Status() == 0 && fitEtaPtFracL5L7ResultPtr->Ndf() != 0. ) {
                    histEtaFracL5L7PtFitMeanMap->SetBinContent( uPt + 1, fitEtaPtFracL5L7ResultPtr->Parameter( 0 ) );
                    histEtaFracL5L7PtFitMeanMap->SetBinError( uPt + 1, fitEtaPtFracL5L7ResultPtr->ParError( 0 ) );
                    histFracL5L7EtaPtFitMeanMap->SetBinContent( uEta + 1, uPt + 1, fitEtaPtFracL5L7ResultPtr->Parameter( 0 ) );
                    histFracL5L7EtaPtFitMeanMap->SetBinError( uEta + 1, uPt + 1, fitEtaPtFracL5L7ResultPtr->ParError( 0 ) );
                    histFracL5L7EtaPtFitMean->Fill( fitEtaPtFracL5L7ResultPtr->Parameter( 0 ) );
                    histFracL5L7EtaPtFitSigma->Fill( fitEtaPtFracL5L7ResultPtr->ParError( 0 ) );
                  }
                  else {
                    if ( verbose_ > 2 ) {
                      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                                << "    failing fit in directory '"; gDirectory->pwd();
                      std::cout << "    '" << nameEtaPtFracL5L7 << "' status " << fitEtaPtFracL5L7ResultPtr->Status() << std::endl;
                    }
                  }
                }
                else {
                  if ( verbose_ > 1 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    missing fit in directory '"; gDirectory->pwd();
                    std::cout << "    '" << nameEtaPtFracL5L7 << std::endl;
                  }
                }
                const std::string nameEtaPtFracL5L7Bkg( nameEtaPtFracL5L7 + "_bkg" );
                TF1 * bkgEtaPtFracL5L7( new TF1( nameEtaPtFracL5L7Bkg.c_str(), bkgFunction_.c_str(), std::max( histEtaPtFracL5L7->GetXaxis()->GetXmin(), histEtaPtFracL5L7->GetMean() - histEtaPtFracL5L7->GetRMS() * fitRange_ ), std::min( histEtaPtFracL5L7->GetXaxis()->GetXmax(), histEtaPtFracL5L7->GetMean() + histEtaPtFracL5L7->GetRMS() * fitRange_ ) ) );
                my::setParametersBkg( fitEtaPtFracL5L7, bkgEtaPtFracL5L7 );
                if ( overwrite_ ) bkgEtaPtFracL5L7->Write( 0, TObject::kOverwrite );
                else              bkgEtaPtFracL5L7->Write();
              }

              const std::string nameEtaPtFracL5L7Restr( nameEtaPtFracL5L7 + "Restr" );
              TH1D * histEtaPtFracL5L7Restr( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaPtFracL5L7Restr.c_str() ) ) );
              if ( histEtaPtFracL5L7Restr != 0 ) {
                const std::string nameEtaPtFracL5L7RestrFit( nameEtaPtFracL5L7Restr + "_fit" );
                TF1 * fitEtaPtFracL5L7Restr( new TF1( nameEtaPtFracL5L7RestrFit.c_str(), fitFunction_.c_str(), std::max( histEtaPtFracL5L7Restr->GetXaxis()->GetXmin(), histEtaPtFracL5L7Restr->GetMean() - histEtaPtFracL5L7Restr->GetRMS() * fitRange_ ), std::min( histEtaPtFracL5L7Restr->GetXaxis()->GetXmax(), histEtaPtFracL5L7Restr->GetMean() + histEtaPtFracL5L7Restr->GetRMS() * fitRange_ ) ) );
                my::setParametersFit( fitEtaPtFracL5L7Restr, histEtaPtFracL5L7Restr );
                TFitResultPtr fitEtaPtFracL5L7RestrResultPtr( histEtaPtFracL5L7Restr->Fit( fitEtaPtFracL5L7Restr, fitOptions_.c_str() ) );
                if ( fitEtaPtFracL5L7RestrResultPtr >= 0 ) {
                  if ( fitEtaPtFracL5L7RestrResultPtr->Status() == 0 && fitEtaPtFracL5L7RestrResultPtr->Ndf() != 0. ) {
                    histEtaFracL5L7RestrPtFitMeanMap->SetBinContent( uPt + 1, fitEtaPtFracL5L7RestrResultPtr->Parameter( 0 ) );
                    histEtaFracL5L7RestrPtFitMeanMap->SetBinError( uPt + 1, fitEtaPtFracL5L7RestrResultPtr->ParError( 0 ) );
                    histFracL5L7RestrEtaPtFitMeanMap->SetBinContent( uEta + 1, uPt + 1, fitEtaPtFracL5L7RestrResultPtr->Parameter( 0 ) );
                    histFracL5L7RestrEtaPtFitMeanMap->SetBinError( uEta + 1, uPt + 1, fitEtaPtFracL5L7RestrResultPtr->ParError( 0 ) );
                    histFracL5L7RestrEtaPtFitMean->Fill( fitEtaPtFracL5L7RestrResultPtr->Parameter( 0 ) );
                    histFracL5L7RestrEtaPtFitSigma->Fill( fitEtaPtFracL5L7RestrResultPtr->ParError( 0 ) );
                  }
                  else {
                    if ( verbose_ > 2 ) {
                      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                                << "    failing fit in directory '"; gDirectory->pwd();
                      std::cout << "    '" << nameEtaPtFracL5L7Restr << "' status " << fitEtaPtFracL5L7RestrResultPtr->Status() << std::endl;
                    }
                  }
                }
                else {
                  if ( verbose_ > 1 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    missing fit in directory '"; gDirectory->pwd();
                    std::cout << "    '" << nameEtaPtFracL5L7Restr << std::endl;
                  }
                }
                const std::string nameEtaPtFracL5L7RestrBkg( nameEtaPtFracL5L7Restr + "_bkg" );
                TF1 * bkgEtaPtFracL5L7Restr( new TF1( nameEtaPtFracL5L7RestrBkg.c_str(), bkgFunction_.c_str(), std::max( histEtaPtFracL5L7Restr->GetXaxis()->GetXmin(), histEtaPtFracL5L7Restr->GetMean() - histEtaPtFracL5L7Restr->GetRMS() * fitRange_ ), std::min( histEtaPtFracL5L7Restr->GetXaxis()->GetXmax(), histEtaPtFracL5L7Restr->GetMean() + histEtaPtFracL5L7Restr->GetRMS() * fitRange_ ) ) );
                my::setParametersBkg( fitEtaPtFracL5L7Restr, bkgEtaPtFracL5L7Restr );
                if ( overwrite_ ) bkgEtaPtFracL5L7Restr->Write( 0, TObject::kOverwrite );
                else              bkgEtaPtFracL5L7Restr->Write();
              }

            } // loop: uPt < nPtBins_

          }

        } // loop: keyEta

        if ( writeFiles_ ) {

          // File name
          std::string nameOut( pathOut_ + "/gentJecL5L7_" + sample_ + "_" + name );
          if ( usePileUp_ ) nameOut.append( "_PileUp" );
          if ( refSel_)     nameOut.append( "_Ref" );
          nameOut.append( ".txt" );

          ofstream fileOut;
          fileOut.open( nameOut.c_str(), std::ios_base::out );

          fileOut.close();
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    written L5L7 JEC file:" << std::endl
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
