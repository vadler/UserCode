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
  const std::string titleProb( "Probability" );
  const std::string titleNdf( "ndf" );
  const std::string titlePt( refGen_ ? "p_{t}^GEN (GeV)" : "p_{t} (GeV)" );
  const std::string titleEta( refGen_ ? "#eta^GEN" : "#eta" );
  std::string titleFrac( refGen_ ? "#frac{p_{t}^{RECO}}{p_{t}^{GEN}}" : "#frac{p_{t}^{GEN}}{p_{t}^{RECO}}" );
  if ( useAlt_ ) {
    titleFrac = ( refGen_ ? "#frac{p_{t}^{RECO,alt.}}{p_{t}^{GEN}}" : "#frac{p_{t}^{GEN}}{p_{t}^{RECO,alt.}}" );
  }
  const std::string titleEvents( "events" );

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

      const std::string nameFrac( name + "_Frac" );
      TH1D * histFrac( new TH1D( nameFrac.c_str(), objCat.c_str(), nPtBins, 0., 2. ) );
      histFrac->SetXTitle( titleFrac.c_str() );
      histFrac->SetYTitle( titleEvents.c_str() );

      const std::string nameFracRestr( nameFrac + "Restr" );
      TH1D * histFracRestr( new TH1D( nameFracRestr.c_str(), objCat.c_str(), nPtBins, 0., 2. ) );
      histFracRestr->SetXTitle( titleFrac.c_str() );
      histFracRestr->SetYTitle( titleEvents.c_str() );

      std::vector< TH1D * > histVecPtFrac;
      std::vector< TH1D * > histVecPtFracRestr;
      for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
        const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
        const std::string namePt( name + "_Pt" + binPt );
        const std::string titlePtDelta( objCat + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq p_{t} #leq " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
        const std::string namePtFrac( namePt + "_Frac" );
        TH1D * histPtFrac( new TH1D( namePtFrac.c_str(), titlePtDelta.c_str(), nPtBins, 0., 2. ) );
        histPtFrac->SetXTitle( titleFrac.c_str() );
        histPtFrac->SetYTitle( titleEvents.c_str() );
        histVecPtFrac.push_back( histPtFrac );
        const std::string namePtFracRestr( namePtFrac + "Restr" );
        TH1D * histPtFracRestr( new TH1D( namePtFracRestr.c_str(), titlePtDelta.c_str(), nPtBins, 0., 2. ) );
        histPtFracRestr->SetXTitle( titleFrac.c_str() );
        histPtFracRestr->SetYTitle( titleEvents.c_str() );
        histVecPtFracRestr.push_back( histPtFracRestr );
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

        const std::string nameEtaFrac( nameEta + "_Frac" );
        const std::string titleEtaFrac( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) );
        TH1D * histEtaFrac( new TH1D( nameEtaFrac.c_str(), titleEtaFrac.c_str(), nPtBins, 0., 2. ) );
        histEtaFrac->SetXTitle( titleFrac.c_str() );
        histEtaFrac->SetYTitle( titleEvents.c_str() );

        const std::string nameEtaFracRestr( nameEtaFrac + "Restr" );
        TH1D * histEtaFracRestr( new TH1D( nameEtaFracRestr.c_str(), titleEtaFrac.c_str(), nPtBins, 0., 2. ) );
        histEtaFracRestr->SetXTitle( titleFrac.c_str() );
        histEtaFracRestr->SetYTitle( titleEvents.c_str() );

        // Split data into p_t bins
        DataCont weightEtaBin( nPtBins_ );
        DataCont ptEtaBin( nPtBins_ );
        DataCont ptGenEtaBin( nPtBins_ );
        DataCont etaEtaBin( nPtBins_ );
        DataCont etaGenEtaBin( nPtBins_ );
        DataCont phiEtaBin( nPtBins_ );
        DataCont phiGenEtaBin( nPtBins_ );
        DataCont propEtaBin( nPtBins_ );
        DataCont propGenEtaBin( nPtBins_ );
        std::vector< unsigned > sizePt( nPtBins_ );
        for ( unsigned uEntry = 0; uEntry < sizeEta_.at( uEta ); ++uEntry ) {
          if ( ptData_.at( uEta ).at( uEntry ) == -5. ) continue; // no match
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
              propEtaBin.at( uPt ).push_back( ptData_.at( uEta ).at( uEntry ) );
              propGenEtaBin.at( uPt ).push_back( ptGenData_.at( uEta ).at( uEntry ) );
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

          const std::string nameEtaPtFrac( nameEtaPt + "_Frac" );
          const std::string titleEtaPtFrac( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq p_{t} #leq " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
          TH1D * histEtaPtFrac( new TH1D( nameEtaPtFrac.c_str(), titleEtaPtFrac.c_str(), nPtBins, 0., 2. ) );
          histEtaPtFrac->SetXTitle( titleFrac.c_str() );
          histEtaPtFrac->SetYTitle( titleEvents.c_str() );

          const std::string nameEtaPtFracRestr( nameEtaPtFrac + "Restr" );
          TH1D * histEtaPtFracRestr( new TH1D( nameEtaPtFracRestr.c_str(), titleEtaPtFrac.c_str(), nPtBins, 0., 2. ) );
          histEtaPtFracRestr->SetXTitle( titleFrac.c_str() );
          histEtaPtFracRestr->SetYTitle( titleEvents.c_str() );

          for ( unsigned uEntry = 0; uEntry < sizePt.at( uPt ); ++uEntry ) {
            if ( refGen_ ) {
              histEtaPtFrac->Fill( propEtaBin.at( uPt ).at( uEntry ) / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              if ( ptGenEtaBin.at( uPt ).at( uEntry ) >= minPtParton_ && reco::deltaR( etaGenEtaBin.at( uPt ).at( uEntry ), phiGenEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDRParton_ )
                histEtaPtFracRestr->Fill( propEtaBin.at( uPt ).at( uEntry ) / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
            }
            else {
              histEtaPtFrac->Fill( propGenEtaBin.at( uPt ).at( uEntry ) / propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              if ( ptGenEtaBin.at( uPt ).at( uEntry ) >= minPtParton_ && reco::deltaR( etaGenEtaBin.at( uPt ).at( uEntry ), phiGenEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDRParton_ )
                histEtaPtFracRestr->Fill( propGenEtaBin.at( uPt ).at( uEntry ) / propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
            }
          } // loop: uEntry < ptEtaBin.at( uPt ).size()

          histFrac->Add( histEtaPtFrac );
          histFracRestr->Add( histEtaPtFracRestr );
          histEtaFrac->Add( histEtaPtFrac );
          histEtaFracRestr->Add( histEtaPtFracRestr );
          histVecPtFrac.at( uPt )->Add( histEtaPtFrac );
          histVecPtFracRestr.at( uPt )->Add( histEtaPtFracRestr );

          histEtaPtFrac->Scale( 1. / histEtaPtFrac->Integral() );
          histEtaPtFracRestr->Scale( 1. / histEtaPtFracRestr->Integral() );

        } // loop: uPt < nPtBins_

        histEtaFrac->Scale( 1. / histEtaFrac->Integral() );
        histEtaFracRestr->Scale( 1. / histEtaFracRestr->Integral() );

      } // loop: keyEta

      dirFit_->cd();

      if ( sizeEtaBins != nEtaBins_ ) {
        std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                  << "    mismatch of eta binning for object category '" << objCat << "':" << std::endl
                  << "        bins in directory structure: " << sizeEtaBins                << std::endl
                  << "        bins in binning histogram  : " << nEtaBins_                  << std::endl;
      }

      histFrac->Scale( 1. / histFrac->Integral() );
      histFracRestr->Scale( 1. / histFracRestr->Integral() );
      for ( unsigned uPt = 0; uPt < histVecPtFrac.size(); ++uPt ) {
        histVecPtFrac.at( uPt )->Scale( 1. / histVecPtFrac.at( uPt )->Integral() );
      }
      for ( unsigned uPt = 0; uPt < histVecPtFracRestr.size(); ++uPt ) {
        histVecPtFracRestr.at( uPt )->Scale( 1. / histVecPtFracRestr.at( uPt )->Integral() );
      }

    } // loop: keyFit


    // Fit L5L7 JECs

    if ( objCat == "UdscJet" || objCat == "BJet" ) {
      if ( verbose_ > 1 ) {
        std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                  << "    JEC L5L7 determination for " << objCat << " started" << std::endl;
      }

      // Loop over kinematic properties

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

        // Fit performance histograms

        const std::string titleMean( "#mu of " + titleFrac );
        const std::string titleSigma( "#sigma of " + titleFrac );

        // Single JEC bins
        const std::string nameFracEtaFitMean( name + "_FracEta_FitMeanMap" );
        TH1D * histFracEtaFitMean( new TH1D( nameFracEtaFitMean.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data() ) );
        histFracEtaFitMean->SetXTitle( titleEta.c_str() );
        histFracEtaFitMean->SetYTitle( titleMean.c_str() );
        const std::string nameFracRestrEtaFitMean( name + "_FracRestrEta_FitMeanMap" );
        TH1D * histFracRestrEtaFitMean( new TH1D( nameFracRestrEtaFitMean.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data() ) );
        histFracRestrEtaFitMean->SetXTitle( titleEta.c_str() );
        histFracRestrEtaFitMean->SetYTitle( titleMean.c_str() );

        const std::string nameFrac( name + "_Frac" );
        TH1D * histFrac( dynamic_cast< TH1D* >( gDirectory->Get( nameFrac.c_str() ) ) );

        if ( histFrac != 0 ) {
          const std::string nameFracFit( nameFrac + "_fit" );
          TF1 * fitFrac( new TF1( nameFracFit.c_str(), fitFunction_.c_str(), std::max( histFrac->GetXaxis()->GetXmin(), histFrac->GetMean() - histFrac->GetRMS() * fitRange_ ), std::min( histFrac->GetXaxis()->GetXmax(), histFrac->GetMean() + histFrac->GetRMS() * fitRange_ ) ) );
          my::setParametersFit( fitFrac, histFrac );
          TFitResultPtr fitFracResultPtr( histFrac->Fit( fitFrac, fitOptions_.c_str() ) );
          if ( fitFracResultPtr >= 0 ) {
            if ( fitFracResultPtr->Status() == 0 && fitFracResultPtr->Ndf() != 0. ) {
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << nameFrac << "' status " << fitFracResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; gDirectory->pwd();
              std::cout << "    '" << nameFrac << std::endl;
            }
          }
          const std::string nameFracBkg( nameFrac + "_bkg" );
          TF1 * bkgFrac( new TF1( nameFracBkg.c_str(), bkgFunction_.c_str(), std::max( histFrac->GetXaxis()->GetXmin(), histFrac->GetMean() - histFrac->GetRMS() * fitRange_ ), std::min( histFrac->GetXaxis()->GetXmax(), histFrac->GetMean() + histFrac->GetRMS() * fitRange_ ) ) );
          my::setParametersBkg( fitFrac, bkgFrac );
          if ( overwrite_ ) bkgFrac->Write( 0, TObject::kOverwrite );
          else              bkgFrac->Write();
        }

        const std::string nameFracRestr( nameFrac + "Restr" );
        TH1D * histFracRestr( dynamic_cast< TH1D* >( gDirectory->Get( nameFracRestr.c_str() ) ) );

        if ( histFracRestr != 0 ) {
          const std::string nameFracRestrFit( nameFracRestr + "_fit" );
          TF1 * fitFracRestr( new TF1( nameFracRestrFit.c_str(), fitFunction_.c_str(), std::max( histFracRestr->GetXaxis()->GetXmin(), histFracRestr->GetMean() - histFracRestr->GetRMS() * fitRange_ ), std::min( histFracRestr->GetXaxis()->GetXmax(), histFracRestr->GetMean() + histFracRestr->GetRMS() * fitRange_ ) ) );
          my::setParametersFit( fitFracRestr, histFracRestr );
          TFitResultPtr fitFracRestrResultPtr( histFracRestr->Fit( fitFracRestr, fitOptions_.c_str() ) );
          if ( fitFracRestrResultPtr >= 0 ) {
            if ( fitFracRestrResultPtr->Status() == 0 && fitFracRestrResultPtr->Ndf() != 0. ) {
            }
            else {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    failing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << nameFracRestr << "' status " << fitFracRestrResultPtr->Status() << std::endl;
              }
            }
          }
          else {
            if ( verbose_ > 1 ) {
              std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                        << "    missing fit in directory '"; gDirectory->pwd();
              std::cout << "    '" << nameFracRestr << std::endl;
            }
          }
          const std::string nameFracRestrBkg( nameFracRestr + "_bkg" );
          TF1 * bkgFracRestr( new TF1( nameFracRestrBkg.c_str(), bkgFunction_.c_str(), std::max( histFracRestr->GetXaxis()->GetXmin(), histFracRestr->GetMean() - histFracRestr->GetRMS() * fitRange_ ), std::min( histFracRestr->GetXaxis()->GetXmax(), histFracRestr->GetMean() + histFracRestr->GetRMS() * fitRange_ ) ) );
          my::setParametersBkg( fitFracRestr, bkgFracRestr );
          if ( overwrite_ ) bkgFracRestr->Write( 0, TObject::kOverwrite );
          else              bkgFracRestr->Write();
        }

        const std::string nameFracPtFitMean( name + "_FracPt_FitMeanMap" );
        TH1D * histFracMean( new TH1D( nameFracPtFitMean.c_str(), objCat.c_str(), nPtBins_, ptBins_.data() ) );
        histFracMean->SetXTitle( titlePt.c_str() );
        histFracMean->SetYTitle( titleMean.c_str() );
        const std::string nameFracRestrPtFitMean( name + "_FracRestrPt_FitMeanMap" );
        TH1D * histFracRestrMean( new TH1D( nameFracRestrPtFitMean.c_str(), objCat.c_str(), nPtBins_, ptBins_.data() ) );
        histFracRestrMean->SetXTitle( titlePt.c_str() );
        histFracRestrMean->SetYTitle( titleMean.c_str() );
        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
          const std::string namePt( name + "_Pt" + binPt );

          const std::string namePtFrac( namePt + "_Frac" );
          TH1D * histPtFrac( dynamic_cast< TH1D* >( gDirectory->Get( namePtFrac.c_str() ) ) );

          if ( histPtFrac != 0 ) {
            const std::string namePtFracFit( namePtFrac + "_fit" );
            TF1 * fitPtFrac( new TF1( namePtFracFit.c_str(), fitFunction_.c_str(), std::max( histPtFrac->GetXaxis()->GetXmin(), histPtFrac->GetMean() - histPtFrac->GetRMS() * fitRange_ ), std::min( histPtFrac->GetXaxis()->GetXmax(), histPtFrac->GetMean() + histPtFrac->GetRMS() * fitRange_ ) ) );
            my::setParametersFit( fitPtFrac, histPtFrac );
            TFitResultPtr fitPtFracResultPtr( histPtFrac->Fit( fitPtFrac, fitOptions_.c_str() ) );
            if ( fitPtFracResultPtr >= 0 ) {
              if ( fitPtFracResultPtr->Status() == 0 && fitPtFracResultPtr->Ndf() != 0. ) {
                histFracMean->SetBinContent( uPt + 1, fitPtFracResultPtr->Parameter( 0 ) );
                histFracMean->SetBinError( uPt + 1, fitPtFracResultPtr->ParError( 0 ) );
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << namePtFrac << "' status " << fitPtFracResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << namePtFrac << std::endl;
              }
            }
            const std::string namePtFracBkg( namePtFrac + "_bkg" );
            TF1 * bkgPtFrac( new TF1( namePtFracBkg.c_str(), bkgFunction_.c_str(), std::max( histPtFrac->GetXaxis()->GetXmin(), histPtFrac->GetMean() - histPtFrac->GetRMS() * fitRange_ ), std::min( histPtFrac->GetXaxis()->GetXmax(), histPtFrac->GetMean() + histPtFrac->GetRMS() * fitRange_ ) ) );
            my::setParametersBkg( fitPtFrac, bkgPtFrac );
            if ( overwrite_ ) bkgPtFrac->Write( 0, TObject::kOverwrite );
            else              bkgPtFrac->Write();
          }

          const std::string namePtFracRestr( namePtFrac + "Restr" );
          TH1D * histPtFracRestr( dynamic_cast< TH1D* >( gDirectory->Get( namePtFracRestr.c_str() ) ) );

          if ( histPtFracRestr != 0 ) {
            const std::string namePtFracRestrFit( namePtFracRestr + "_fit" );
            TF1 * fitPtFracRestr( new TF1( namePtFracRestrFit.c_str(), fitFunction_.c_str(), std::max( histPtFracRestr->GetXaxis()->GetXmin(), histPtFracRestr->GetMean() - histPtFracRestr->GetRMS() * fitRange_ ), std::min( histPtFracRestr->GetXaxis()->GetXmax(), histPtFracRestr->GetMean() + histPtFracRestr->GetRMS() * fitRange_ ) ) );
            my::setParametersFit( fitPtFracRestr, histPtFracRestr );
            TFitResultPtr fitPtFracRestrResultPtr( histPtFracRestr->Fit( fitPtFracRestr, fitOptions_.c_str() ) );
            if ( fitPtFracRestrResultPtr >= 0 ) {
              if ( fitPtFracRestrResultPtr->Status() == 0 && fitPtFracRestrResultPtr->Ndf() != 0. ) {
                histFracRestrMean->SetBinContent( uPt + 1, fitPtFracRestrResultPtr->Parameter( 0 ) );
                histFracRestrMean->SetBinError( uPt + 1, fitPtFracRestrResultPtr->ParError( 0 ) );
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << namePtFracRestr << "' status " << fitPtFracRestrResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << namePtFracRestr << std::endl;
              }
            }
            const std::string namePtFracRestrBkg( namePtFracRestr + "_bkg" );
            TF1 * bkgPtFracRestr( new TF1( namePtFracRestrBkg.c_str(), bkgFunction_.c_str(), std::max( histPtFracRestr->GetXaxis()->GetXmin(), histPtFracRestr->GetMean() - histPtFracRestr->GetRMS() * fitRange_ ), std::min( histPtFracRestr->GetXaxis()->GetXmax(), histPtFracRestr->GetMean() + histPtFracRestr->GetRMS() * fitRange_ ) ) );
            my::setParametersBkg( fitPtFracRestr, bkgPtFracRestr );
            if ( overwrite_ ) bkgPtFracRestr->Write( 0, TObject::kOverwrite );
            else              bkgPtFracRestr->Write();
          }
        } // loop: uPt < nPtBins_

        // Loop over eta bins
        TList * listFit( dirFit_->GetListOfKeys() );
        TIter nextInListFit( listFit );
        while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
          if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
          const std::string binEta( keyEta->GetName() );
          const unsigned uEta( std::atoi( binEta.substr( 3 ).data() ) );
          dirFit_->cd( binEta.c_str() );

          const std::string nameEta( name + "_" + binEta );

          const std::string nameEtaFrac( nameEta + "_Frac" );
          TH1D * histEtaFrac( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaFrac.c_str() ) ) );

          if ( histEtaFrac != 0 ) {
            const std::string nameEtaFracFit( nameEtaFrac + "_fit" );
            TF1 * fitEtaFrac( new TF1( nameEtaFracFit.c_str(), fitFunction_.c_str(), std::max( histEtaFrac->GetXaxis()->GetXmin(), histEtaFrac->GetMean() - histEtaFrac->GetRMS() * fitRange_ ), std::min( histEtaFrac->GetXaxis()->GetXmax(), histEtaFrac->GetMean() + histEtaFrac->GetRMS() * fitRange_ ) ) );
            my::setParametersFit( fitEtaFrac, histEtaFrac );
            TFitResultPtr fitEtaFracResultPtr( histEtaFrac->Fit( fitEtaFrac, fitOptions_.c_str() ) );
            if ( fitEtaFracResultPtr >= 0 ) {
              if ( fitEtaFracResultPtr->Status() == 0 && fitEtaFracResultPtr->Ndf() != 0. ) {
                histFracEtaFitMean->SetBinContent( uEta + 1, fitEtaFracResultPtr->Parameter( 0 ) );
                histFracEtaFitMean->SetBinError( uEta + 1, fitEtaFracResultPtr->ParError( 0 ) );
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << nameEtaFrac << "' status " << fitEtaFracResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << nameEtaFrac << std::endl;
              }
            }
            const std::string nameEtaFracBkg( nameEtaFrac + "_bkg" );
            TF1 * bkgEtaFrac( new TF1( nameEtaFracBkg.c_str(), bkgFunction_.c_str(), std::max( histEtaFrac->GetXaxis()->GetXmin(), histEtaFrac->GetMean() - histEtaFrac->GetRMS() * fitRange_ ), std::min( histEtaFrac->GetXaxis()->GetXmax(), histEtaFrac->GetMean() + histEtaFrac->GetRMS() * fitRange_ ) ) );
            my::setParametersBkg( fitEtaFrac, bkgEtaFrac );
            if ( overwrite_ ) bkgEtaFrac->Write( 0, TObject::kOverwrite );
            else              bkgEtaFrac->Write();
          }

          const std::string nameEtaFracRestr( nameEtaFrac + "Restr" );
          TH1D * histEtaFracRestr( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaFracRestr.c_str() ) ) );

          if ( histEtaFracRestr != 0 ) {
            const std::string nameEtaFracRestrFit( nameEtaFracRestr + "_fit" );
            TF1 * fitEtaFracRestr( new TF1( nameEtaFracRestrFit.c_str(), fitFunction_.c_str(), std::max( histEtaFracRestr->GetXaxis()->GetXmin(), histEtaFracRestr->GetMean() - histEtaFracRestr->GetRMS() * fitRange_ ), std::min( histEtaFracRestr->GetXaxis()->GetXmax(), histEtaFracRestr->GetMean() + histEtaFracRestr->GetRMS() * fitRange_ ) ) );
            my::setParametersFit( fitEtaFracRestr, histEtaFracRestr );
            TFitResultPtr fitEtaFracRestrResultPtr( histEtaFracRestr->Fit( fitEtaFracRestr, fitOptions_.c_str() ) );
            if ( fitEtaFracRestrResultPtr >= 0 ) {
              if ( fitEtaFracRestrResultPtr->Status() == 0 && fitEtaFracRestrResultPtr->Ndf() != 0. ) {
                histFracRestrEtaFitMean->SetBinContent( uEta + 1, fitEtaFracRestrResultPtr->Parameter( 0 ) );
                histFracRestrEtaFitMean->SetBinError( uEta + 1, fitEtaFracRestrResultPtr->ParError( 0 ) );
              }
              else {
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << nameEtaFracRestr << "' status " << fitEtaFracRestrResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << nameEtaFracRestr << std::endl;
              }
            }
            const std::string nameEtaFracRestrBkg( nameEtaFracRestr + "_bkg" );
            TF1 * bkgEtaFracRestr( new TF1( nameEtaFracRestrBkg.c_str(), bkgFunction_.c_str(), std::max( histEtaFracRestr->GetXaxis()->GetXmin(), histEtaFracRestr->GetMean() - histEtaFracRestr->GetRMS() * fitRange_ ), std::min( histEtaFracRestr->GetXaxis()->GetXmax(), histEtaFracRestr->GetMean() + histEtaFracRestr->GetRMS() * fitRange_ ) ) );
            my::setParametersBkg( fitEtaFracRestr, bkgEtaFracRestr );
            if ( overwrite_ ) bkgEtaFracRestr->Write( 0, TObject::kOverwrite );
            else              bkgEtaFracRestr->Write();
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
