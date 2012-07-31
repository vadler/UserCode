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
#include "TopQuarkAnalysis/TopHitFit/interface/EtaDepResolution.h"


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
  const std::string resolutionFile_( io_.getParameter< std::string >( "resolutionFile" ) );
  // Configuration for histogram binning
  const edm::ParameterSet & histos_( process_.getParameter< edm::ParameterSet >( "histos" ) );
  std::string fitOptions_( histos_.getParameter< std::string >( "fitOptions" ) );
  const double widthFactor_( histos_.getParameter< double >( "widthFactor" ) );
  // Configuration for fitting resolution functions
  const edm::ParameterSet & resFuncs_( process_.getParameter< edm::ParameterSet >( "resFuncs" ) );
  const bool fitResFuncs_( resFuncs_.getParameter< bool >( "fit" ) );
  const std::string fitFunctionResFuncs_( resFuncs_.getParameter< std::string >( "fitFunction" ) );
  std::string fitOptionsSigmaResFuncs_( resFuncs_.getParameter< std::string >( "fitOptionsSigma" ) );
  const double fitRangeResFuncs_( std::min( resFuncs_.getParameter< double >( "fitRange" ), widthFactor_ ) );
  const std::string resFunc_( resFuncs_.getParameter< std::string >( "resolutionFunction" ) );
  const std::string resFuncInv_( resFuncs_.getParameter< std::string >( "resolutionFunctionInverse" ) );
  const std::string resFuncRel_( resFuncs_.getParameter< std::string >( "resolutionFunctionRel" ) );
  const std::string resFuncInvRel_( resFuncs_.getParameter< std::string >( "resolutionFunctionInverseRel" ) );
  const std::string resFuncInvInv_( resFuncs_.getParameter< std::string >( "resolutionFunctionInverseInv" ) );
  const std::string resFuncInvInvRel_( resFuncs_.getParameter< std::string >( "resolutionFunctionInverseInvRel" ) );
  const bool onlyExisting_( resFuncs_.getParameter< bool >( "onlyExisting" ) );
  const bool writeResolutionFiles_( resFuncs_.getParameter< bool >( "writeFiles" ) && onlyExisting_ );
  const std::string pathOutResolution_( resFuncs_.getParameter< std::string >( "pathOut" ) );
  // Configuration for fitting L5L7 JECs
  const edm::ParameterSet & jecsL5L7_( process_.getParameter< edm::ParameterSet >( "jecsL5L7" ) );
  const bool fitJecsL5L7_( jecsL5L7_.getParameter< bool >( "fit" ) );
  const std::string fitFunctionJecsL5L7_( jecsL5L7_.getParameter< std::string >( "fitFunction" ) );
  std::string fitOptionsL5L7_( jecsL5L7_.getParameter< std::string >( "fitOptions" ) );
  const double fitRangeJecsL5L7_( std::min( jecsL5L7_.getParameter< double >( "fitRange" ), widthFactor_ ) );
  const std::string bkgFunctionJecsL5L7_( jecsL5L7_.getParameter< std::string >( "bkgFunction" ) );
  const double minPtPartonL5L7_( jecsL5L7_.getParameter< double >( "minPtParton" ) );
  const double maxDRPartonL5L7_( jecsL5L7_.getParameter< double >( "maxDRParton" ) );
  const bool writeJecsL5L7Files_( jecsL5L7_.getParameter< bool >( "writeFiles" ) && onlyExisting_ );
  const std::string pathOutJecsL5L7_( jecsL5L7_.getParameter< std::string >( "pathOut" ) );

  std::vector< std::vector< bool > > nominalInv_( objCats_.size() );

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
  if      ( verbose_ < 2 ) fitOptionsSigmaResFuncs_.append( "Q" );
  else if ( verbose_ > 3 ) fitOptionsSigmaResFuncs_.append( "V" );
  if      ( verbose_ < 3 ) fitOptionsL5L7_.append( "Q" );
  else if ( verbose_ > 4 ) fitOptionsL5L7_.append( "V" );
  const std::string titleChi2( "#chi^{2} / ndf" );
  const std::string titleProb( "Probability" );
  const std::string titleNdf( "ndf" );
  std::string titlePt( "p_{t} (GeV)" );
  std::string titleEta( "#eta" );
  std::string titlePhi( "#phi" );
  std::vector< std::string > titlesDelta;
  titlesDelta.push_back( "#Deltap_{t} (GeV)" );
  titlesDelta.push_back( "#Delta#eta" );
  titlesDelta.push_back( "#Delta#phi" );
  std::vector< std::string > titlesDeltaInv;
  titlesDeltaInv.push_back( "#Delta#frac{1}{p_{t}} (#frac{1}{GeV})" );
  titlesDeltaInv.push_back( "#Delta#eta" );
  titlesDeltaInv.push_back( "#Delta#phi" );
  std::vector< std::string > titlesFrac;
  titlesFrac.push_back( "#frac{p_{t}^{GEN}}{p_{t}^{RECO}}" );
  titlesFrac.push_back( "" );
  titlesFrac.push_back( "" );
  std::vector< std::string > titlesFracGen;
  titlesFracGen.push_back( "#frac{p_{t}^{RECO}}{p_{t}^{GEN}}" );
  titlesFracGen.push_back( "" );
  titlesFracGen.push_back( "" );
  std::vector< std::string > titlesDeltaRel;
  titlesDeltaRel.push_back( "#frac{#Deltap_{t}}{p_{t}}" );
  titlesDeltaRel.push_back( "" );
  titlesDeltaRel.push_back( "" );
  std::vector< std::string > titlesDeltaAlt;
  titlesDeltaAlt.push_back( "#Deltap_{t}^{alt.} (GeV)" );
  titlesDeltaAlt.push_back( "#Delta#eta^{alt.}" );
  titlesDeltaAlt.push_back( "#Delta#phi^{alt.}" );
  std::vector< std::string > titlesDeltaAltInv;
  titlesDeltaAltInv.push_back( "#Delta#frac{1}{p_{t}^{alt.}} (#frac{1}{GeV})" );
  titlesDeltaAltInv.push_back( "#Delta#eta^{alt.}" );
  titlesDeltaAltInv.push_back( "#Delta#phi^{alt.}" );
  std::vector< std::string > titlesFracAlt;
  titlesFracAlt.push_back( "#frac{p_{t}^{GEN}}{p_{t}^{RECO,alt.}}" );
  titlesFracAlt.push_back( "" );
  titlesFracAlt.push_back( "" );
  std::vector< std::string > titlesFracAltGen;
  titlesFracAltGen.push_back( "#frac{p_{t}^{RECO,alt.}}{p_{t}^{GEN}}" );
  titlesFracAltGen.push_back( "" );
  titlesFracAltGen.push_back( "" );
  std::vector< std::string > titlesDeltaAltRel;
  titlesDeltaAltRel.push_back( "frac{#Deltap_{t}^{alt.}}{p_{t}^{alt.}}" );
  titlesDeltaAltRel.push_back( "" );
  titlesDeltaAltRel.push_back( "" );
  const std::string titleEvents( "events" );


  // Use existing resolution functions

  if ( verbose_ > 0 )
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "    accessing existing resolution functions from resolution file '" << resolutionFile_ << "'" << std::endl;

  // Open resolution file
  TFile * resolutionFile( TFile::Open( resolutionFile_.c_str() ) );
  if ( resolutionFile ) {
    if ( verbose_ > 2 ) gDirectory->pwd();

    // Loop over objects and quantities

    for ( unsigned uCat = 0; uCat < objCats_.size(); ++uCat ) {
      const std::string objCat( objCats_.at( uCat ) );
      TDirectory * dirCat_( dynamic_cast< TDirectory* >( resolutionFile->Get( objCat.c_str() ) ) );
      if ( ! dirCat_ ) {
        std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                  << "    object category '" << objCat << "' does not exist in resolution file" << std::endl;
        continue;
      }

      TList * listCat( dirCat_->GetListOfKeys() );
      if ( verbose_ > 3 ) listCat->Print();
      TIter nextInListCat( listCat );
      while ( TKey * keyProp = ( TKey* )nextInListCat() ) {
        if ( std::string( keyProp->GetClassName() ) != nameDirClass ) continue;
        const std::string kinProp( keyProp->GetName() );
        TDirectory * dirProp_( dynamic_cast< TDirectory* >( dirCat_->Get( kinProp.c_str() ) ) );

        TList * listProp( dirProp_->GetListOfKeys() );
        if ( verbose_ > 3 ) listProp->Print();
        TIter nextInListProp( listProp );
        bool bEta( false );
        while ( TKey * keyEta = ( TKey* )nextInListProp() ) {
          if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
          const std::string binEta( keyEta->GetName() );
          TDirectory * dirEta_( dynamic_cast< TDirectory* >( dirProp_->Get( binEta.c_str() ) ) );

          const std::string name( "fitExist_" + objCat + "_" + kinProp + "_" + binEta );
          const std::string nameInv( "fitExist_" + objCat + "_Inv_" + kinProp + "_" + binEta );

          TList * listEta( dirEta_->GetListOfKeys() );
          if ( verbose_ > 3 ) listEta->Print();
          TIter nextInListEta( listEta );
          TF1 * fitSigma( 0 );
          TF1 * fitSigmaInv( 0 );
          while ( TKey * keyFunc = ( TKey* )nextInListEta() ) {
            if ( std::string( keyFunc->GetClassName() ) != nameFuncClass ) continue;
            fitSigma    = dynamic_cast< TF1* >( dirEta_->Get( name.c_str() ) );
            fitSigmaInv = dynamic_cast< TF1* >( dirEta_->Get( nameInv.c_str() ) );
          }
          if ( ( fitSigma && fitSigmaInv ) || ( ! fitSigma && ! fitSigmaInv ) ) {
            std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                      << "    inconsistent resolution functions in '" << objCat << "', '" << kinProp << "', '" << binEta << std::endl;
            continue;
          }
          if ( ! bEta ) {
            if ( verbose_ > 3 ) {
              std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                        << "    found non-inverted'" << name << "': " << ( fitSigma != 0 ) << std::endl
                        << "    found inverted '" << nameInv << "': " << ( fitSigmaInv != 0 ) << std::endl;
            }
            nominalInv_.at( uCat ).push_back( fitSigmaInv ? true : false );
            bEta = true;
          }

        } // loop: nextInListProp()

        dirProp_->cd();
        if (! bEta  ) {
          std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                    << "    no resolution function in '"; gDirectory->pwd();
          returnStatus_ += 0x200;
        }

      } // loop: nextInListCat()

    } // loop: uCat < objCats_.size()

    // Close output file
    resolutionFile->Close();

  } // ( resolutionFile )

  else {
    std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
              << "    resolution file '" << resolutionFile_ << "' missing" << std::endl;
    returnStatus_ += 0x100;
  } // !( resolutionFile )

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

  typedef std::vector< std::vector< Double_t > > dataCont;

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

    // Read general n-tuple data
    dataCont weightData_( nEtaBins_ );
    dataCont ptData_( nEtaBins_ );
    dataCont ptGenData_( nEtaBins_ );
    dataCont etaData_( nEtaBins_ );
    dataCont etaGenData_( nEtaBins_ );
    dataCont phiData_( nEtaBins_ );
    dataCont phiGenData_( nEtaBins_ );
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

    // Loop over kinematic properties

    TList * listCat( dirCat_->GetListOfKeys() );
    if ( verbose_ > 3 ) listCat->Print();
    TIter nextInListCat( listCat );
    unsigned uProp( 0 );
    while ( TKey * keyProp = ( TKey* )nextInListCat() ) {
      if ( std::string( keyProp->GetClassName() ) != nameDirClass ) continue;
      const std::string kinProp( keyProp->GetName() );
      TDirectory * dirProp_( dynamic_cast< TDirectory* >( dirCat_->Get( kinProp.c_str() ) ) );

      // Histogram binning
      const unsigned propDeltaBins_( histos_.getParameter< unsigned >( std::string( objCat + "Delta" + kinProp + "Bins" ) ) );
      const double   propDeltaMax_( histos_.getParameter< double >( std::string( objCat + "Delta" + kinProp + "Max" ) ) );
      const unsigned propInvDeltaBins_( histos_.getParameter< unsigned >( std::string( objCat + "Delta" + kinProp + "InvBins" ) ) );
      const double   propInvDeltaMax_( histos_.getParameter< double >( std::string( objCat + "Delta" + kinProp + "InvMax" ) ) );

      // Read kinematic property n-tuple data
      dataCont propData_( nEtaBins_ );
      dataCont propGenData_( nEtaBins_ );
      Double_t propData;
      Double_t propGenData;
      data_->SetBranchAddress( kinProp.c_str(), &propData );
      data_->SetBranchAddress( std::string( kinProp + "Gen" ).c_str(), &propGenData );
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
        // These are real switches: depending on configuration, only one setting combination can be run at a time
        if ( useAlt_  == ( subFit.find( "Alt" )  == std::string::npos ) ) continue;
        if ( useSymm_ == ( subFit.find( "Symm" ) == std::string::npos ) ) continue;
        if ( refGen_  == ( subFit.find( "Gen" )  == std::string::npos ) ) continue;
        TDirectory * dirFit_( dynamic_cast< TDirectory* >( dirProp_->Get( subFit.c_str() ) ) );
        dirFit_->cd();

        const std::string name( objCat + "_" + kinProp + "_" + subFit );

        // Inversion flags
        const bool inverse( subFit.find( "Inv" ) != std::string::npos );

        std::string titleDelta( titlesDelta.at( uProp ) );
        std::string titleFrac( titlesFrac.at( uProp ) );
        std::string titleDeltaRel( titlesDeltaRel.at( uProp ) );
        if ( inverse ) {
          if ( useAlt_ ) titleDelta = titlesDeltaAltInv.at( uProp );
          else           titleDelta = titlesDeltaInv.at( uProp );
          titleFrac     = "";
          titleDeltaRel = "";
        }
        else if ( useAlt_ ) {
          titleDelta    = titlesDeltaAlt.at( uProp );
          titleFrac     = titlesFracAlt.at( uProp );
          titleDeltaRel = titlesDeltaAltRel.at( uProp );
        }
        if ( refGen_ ) {
          titlePt = "p_{t}^GEN (GeV)";
          titleEta = "#eta^GEN";
          titlePhi = "#phi^GEN";
          if ( useAlt_ ) {
            if ( ! inverse ) titleFrac = titlesFracAltGen.at( uProp );
          }
          else {
            if ( ! inverse ) titleFrac = titlesFracGen.at( uProp );
          }
        }

        const unsigned deltaBins( inverse ? propInvDeltaBins_ : propDeltaBins_ );
        const double   deltaMax( inverse ? propInvDeltaMax_ : propDeltaMax_ );

        const std::string nameDelta( name + "_Delta" );
        TH1D * histDelta( new TH1D( nameDelta.c_str(), objCat.c_str(), deltaBins, -deltaMax, deltaMax ) );
        histDelta->SetXTitle( titleDelta.c_str() );
        histDelta->SetYTitle( titleEvents.c_str() );

        const std::string nameFrac( name + "_Frac" );
        TH1D * histFrac( new TH1D( nameFrac.c_str(), objCat.c_str(), deltaBins, 0., 2. ) );
        histFrac->SetXTitle( titleFrac.c_str() );
        histFrac->SetYTitle( titleEvents.c_str() );

        const std::string nameFracRestr( nameFrac + "Restr" );
        TH1D * histFracRestr( new TH1D( nameFracRestr.c_str(), objCat.c_str(), deltaBins, 0., 2. ) );
        histFracRestr->SetXTitle( titleFrac.c_str() );
        histFracRestr->SetYTitle( titleEvents.c_str() );

        const std::string nameDeltaRel( nameDelta + "Rel" );
        TH1D * histDeltaRel( new TH1D( nameDeltaRel.c_str(), objCat.c_str(), deltaBins, -1., 1. ) );
        histDeltaRel->SetXTitle( titleDeltaRel.c_str() );
        histDeltaRel->SetYTitle( titleEvents.c_str() );

        const std::string nameDeltaRefMap( nameDelta + "RefMap" );
        TH2D * histDeltaRefMap( new TH2D( nameDeltaRefMap.c_str(), objCat.c_str(), nPtBins_, ptBins_.data(), deltaBins, -deltaMax, deltaMax ) );
        histDeltaRefMap->SetXTitle( titlePt.c_str() );
        histDeltaRefMap->SetYTitle( std::string( "-" + titleDelta ).c_str() );
        histDeltaRefMap->SetZTitle( titleEvents.c_str() );

        std::vector< TH1D * > histVecPtDelta;
        std::vector< TH1D * > histVecPtFrac;
        std::vector< TH1D * > histVecPtFracRestr;
        std::vector< TH1D * > histVecPtDeltaRel;
        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
          const std::string namePt( name + "_Pt" + binPt );
          const std::string namePtDelta( namePt + "_Delta" );
          const std::string titlePtDelta( objCat + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq p_{t} #leq " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
          TH1D * histPtDelta( new TH1D( namePtDelta.c_str(), titlePtDelta.c_str(), deltaBins, -deltaMax, deltaMax ) );
          histPtDelta->SetXTitle( titleDelta.c_str() );
          histPtDelta->SetYTitle( titleEvents.c_str() );
          histVecPtDelta.push_back( histPtDelta );
          if ( ! inverse && kinProp == "Pt" ) {
            const std::string namePtFrac( namePt + "_Frac" );
            TH1D * histPtFrac( new TH1D( namePtFrac.c_str(), titlePtDelta.c_str(), deltaBins, 0., 2. ) );
            histPtFrac->SetXTitle( titleFrac.c_str() );
            histPtFrac->SetYTitle( titleEvents.c_str() );
            histVecPtFrac.push_back( histPtFrac );
            const std::string namePtFracRestr( namePtFrac + "Restr" );
            TH1D * histPtFracRestr( new TH1D( namePtFracRestr.c_str(), titlePtDelta.c_str(), deltaBins, 0., 2. ) );
            histPtFracRestr->SetXTitle( titleFrac.c_str() );
            histPtFracRestr->SetYTitle( titleEvents.c_str() );
            histVecPtFracRestr.push_back( histPtFracRestr );
            const std::string namePtDeltaRel( namePtDelta + "Rel" );
            TH1D * histPtDeltaRel( new TH1D( namePtDeltaRel.c_str(), titlePtDelta.c_str(), deltaBins, -1., 1. ) );
            histPtDeltaRel->SetXTitle( titleDeltaRel.c_str() );
            histPtDeltaRel->SetYTitle( titleEvents.c_str() );
            histVecPtDeltaRel.push_back( histPtDeltaRel );
          }
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

          const std::string nameEtaDelta( nameEta + "_Delta" );
          const std::string titleEtaDelta( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) );
          TH1D * histEtaDelta( new TH1D( nameEtaDelta.c_str(), titleEtaDelta.c_str(), deltaBins, -deltaMax, deltaMax ) );
          histEtaDelta->SetXTitle( titleDelta.c_str() );
          histEtaDelta->SetYTitle( titleEvents.c_str() );

          const std::string nameEtaFrac( nameEta + "_Frac" );
          TH1D * histEtaFrac( new TH1D( nameEtaFrac.c_str(), titleEtaDelta.c_str(), deltaBins, 0., 2. ) );
          histEtaFrac->SetXTitle( titleFrac.c_str() );
          histEtaFrac->SetYTitle( titleEvents.c_str() );

          const std::string nameEtaFracRestr( nameEtaFrac + "Restr" );
          TH1D * histEtaFracRestr( new TH1D( nameEtaFracRestr.c_str(), titleEtaDelta.c_str(), deltaBins, 0., 2. ) );
          histEtaFracRestr->SetXTitle( titleFrac.c_str() );
          histEtaFracRestr->SetYTitle( titleEvents.c_str() );

          const std::string nameEtaDeltaRel( nameEtaDelta + "Rel" );
          TH1D * histEtaDeltaRel( new TH1D( nameEtaDeltaRel.c_str(), titleEtaDelta.c_str(), deltaBins, -1., 1. ) );
          histEtaDeltaRel->SetXTitle( titleDeltaRel.c_str() );
          histEtaDeltaRel->SetYTitle( titleEvents.c_str() );

          const std::string nameEtaDeltaRefMap( nameEtaDelta + "RefMap" );
          TH2D * histEtaDeltaRefMap( new TH2D( nameEtaDeltaRefMap.c_str(), titleEtaDelta.c_str(), nPtBins_, ptBins_.data(), deltaBins, -deltaMax, deltaMax ) );
          histEtaDeltaRefMap->SetXTitle( titlePt.c_str() );
          histEtaDeltaRefMap->SetYTitle( std::string( "-" + titleDelta ).c_str() );
          histEtaDeltaRefMap->SetZTitle( titleEvents.c_str() );

          // Split data into p_t bins
          dataCont weightEtaBin( nPtBins_ );
          dataCont ptEtaBin( nPtBins_ );
          dataCont ptGenEtaBin( nPtBins_ );
          dataCont etaEtaBin( nPtBins_ );
          dataCont etaGenEtaBin( nPtBins_ );
          dataCont phiEtaBin( nPtBins_ );
          dataCont phiGenEtaBin( nPtBins_ );
          dataCont propEtaBin( nPtBins_ );
          dataCont propGenEtaBin( nPtBins_ );
          std::vector< unsigned > sizePt( nPtBins_ );
          for ( unsigned uEntry = 0; uEntry < sizeEta_.at( uEta ); ++uEntry ) {
            if ( propData_.at( uEta ).at( uEntry ) == -5. ) continue; // no match
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
                propEtaBin.at( uPt ).push_back( propData_.at( uEta ).at( uEntry ) );
                propGenEtaBin.at( uPt ).push_back( propGenData_.at( uEta ).at( uEntry ) );
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

            const std::string nameEtaPtDelta( nameEtaPt + "_Delta" );
            const std::string titleEtaPtDelta( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq p_{t} #leq " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
            TH1D * histEtaPtDelta( new TH1D( nameEtaPtDelta.c_str(), titleEtaPtDelta.c_str(), deltaBins, -deltaMax, deltaMax ) );
            histEtaPtDelta->SetXTitle( titleDelta.c_str() );
            histEtaPtDelta->SetYTitle( titleEvents.c_str() );

            const std::string nameEtaPtFrac( nameEtaPt + "_Frac" );
            TH1D * histEtaPtFrac( new TH1D( nameEtaPtFrac.c_str(), titleEtaPtDelta.c_str(), deltaBins, 0., 2. ) );
            histEtaPtFrac->SetXTitle( titleFrac.c_str() );
            histEtaPtFrac->SetYTitle( titleEvents.c_str() );

            const std::string nameEtaPtFracRestr( nameEtaPtFrac + "Restr" );
            TH1D * histEtaPtFracRestr( new TH1D( nameEtaPtFracRestr.c_str(), titleEtaPtDelta.c_str(), deltaBins, 0., 2. ) );
            histEtaPtFracRestr->SetXTitle( titleFrac.c_str() );
            histEtaPtFracRestr->SetYTitle( titleEvents.c_str() );

            const std::string nameEtaPtDeltaRel( nameEtaPtDelta + "Rel" );
            TH1D * histEtaPtDeltaRel( new TH1D( nameEtaPtDeltaRel.c_str(), titleEtaPtDelta.c_str(), deltaBins, -1., 1. ) );
            histEtaPtDeltaRel->SetXTitle( titleDeltaRel.c_str() );
            histEtaPtDeltaRel->SetYTitle( titleEvents.c_str() );

            for ( unsigned uEntry = 0; uEntry < sizePt.at( uPt ); ++uEntry ) {
              if ( refGen_ ) {
                if ( inverse && kinProp == "Pt" ) histEtaPtDelta->Fill( 1. / propGenEtaBin.at( uPt ).at( uEntry ) - 1. / propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                else                              histEtaPtDelta->Fill( propGenEtaBin.at( uPt ).at( uEntry ) - propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                if ( ! inverse && kinProp == "Pt" ) {
                  histEtaPtFrac->Fill( propEtaBin.at( uPt ).at( uEntry ) / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                  if ( ptGenEtaBin.at( uPt ).at( uEntry ) >= minPtPartonL5L7_ && reco::deltaR( etaGenEtaBin.at( uPt ).at( uEntry ), phiGenEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDRPartonL5L7_ )
                    histEtaPtFracRestr->Fill( propEtaBin.at( uPt ).at( uEntry ) / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                  histEtaPtDeltaRel->Fill( ( propGenEtaBin.at( uPt ).at( uEntry ) - propEtaBin.at( uPt ).at( uEntry ) ) / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                  histDeltaRefMap->Fill( propGenEtaBin.at( uPt ).at( uEntry ), propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                  histEtaDeltaRefMap->Fill( propGenEtaBin.at( uPt ).at( uEntry ), propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                }
              }
              else {
                if ( inverse && kinProp == "Pt" ) histEtaPtDelta->Fill( 1. / propEtaBin.at( uPt ).at( uEntry ) - 1. / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                else                              histEtaPtDelta->Fill( propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                if ( ! inverse && kinProp == "Pt" ) {
                  histEtaPtFrac->Fill( propGenEtaBin.at( uPt ).at( uEntry ) / propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                  if ( ptGenEtaBin.at( uPt ).at( uEntry ) >= minPtPartonL5L7_ && reco::deltaR( etaGenEtaBin.at( uPt ).at( uEntry ), phiGenEtaBin.at( uPt ).at( uEntry ), etaEtaBin.at( uPt ).at( uEntry ), phiEtaBin.at( uPt ).at( uEntry ) ) <= maxDRPartonL5L7_ )
                    histEtaPtFracRestr->Fill( propGenEtaBin.at( uPt ).at( uEntry ) / propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                  histEtaPtDeltaRel->Fill( ( propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ) ) / propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                  histDeltaRefMap->Fill( propEtaBin.at( uPt ).at( uEntry ), propGenEtaBin.at( uPt ).at( uEntry ) - propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                  histEtaDeltaRefMap->Fill( propEtaBin.at( uPt ).at( uEntry ), propGenEtaBin.at( uPt ).at( uEntry ) - propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                }
              }
            } // loop: uEntry < ptEtaBin.at( uPt ).size()

            histDelta->Add( histEtaPtDelta );
            histVecPtDelta.at( uPt )->Add( histEtaPtDelta );
            histEtaDelta->Add( histEtaPtDelta );
            if ( ! inverse && kinProp == "Pt" ) {
              histFrac->Add( histEtaPtFrac );
              histFracRestr->Add( histEtaPtFracRestr );
              histEtaFrac->Add( histEtaPtFrac );
              histEtaFracRestr->Add( histEtaPtFracRestr );
              histVecPtFrac.at( uPt )->Add( histEtaPtFrac );
              histVecPtFracRestr.at( uPt )->Add( histEtaPtFracRestr );
              histDeltaRel->Add( histEtaPtDeltaRel );
              histEtaDeltaRel->Add( histEtaPtDeltaRel );
              histVecPtDeltaRel.at( uPt )->Add( histEtaPtDeltaRel );
            }

            const std::string nameEtaPtDeltaRebin( nameEtaPtDelta + "Rebin" );
//             const Int_t deltaBinsRebin( inverse ? propInvDeltaBins_ : propDeltaBins_ ); // FIXME: tune number of bins
            const Int_t deltaBinsRebin( deltaBins );
            const Double_t meanEtaPtDelta( histEtaPtDelta->GetMean() );
            const Double_t widthEtaPtDelta( std::fabs( histEtaPtDelta->GetRMS() ) );
            if ( widthEtaPtDelta == 0. && verbose_ > 2 ) {
              std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                        << "    no histogram \"width\" in '" << nameEtaPtDelta << "'" << std::endl;
            }
            const Double_t rangeEtaPtDeltaRebin( widthEtaPtDelta == 0. ? widthFactor_ * std::fabs( histEtaPtDelta->GetXaxis()->GetXmax() ) : widthFactor_ * widthEtaPtDelta ); // FIXME: tune, incl. under- and overflow, remove hard-coding
            TH1D * histEtaPtDeltaRebin( new TH1D( nameEtaPtDeltaRebin.c_str(), titleEtaPtDelta.c_str(), deltaBinsRebin, -rangeEtaPtDeltaRebin + meanEtaPtDelta, rangeEtaPtDeltaRebin + meanEtaPtDelta ) );
            histEtaPtDeltaRebin->SetXTitle( histEtaPtDelta->GetXaxis()->GetTitle() );
            histEtaPtDeltaRebin->SetYTitle( histEtaPtDelta->GetYaxis()->GetTitle() );

            const std::string nameEtaPtDeltaRelRebin( nameEtaPtDeltaRel + "Rebin" );
//             const Int_t deltaBinsRebinRel( inverse ? propInvDeltaBins_ : propDeltaBins_ ); // FIXME: tune number of bins
            const Int_t deltaBinsRebinRel( deltaBins );
            const Double_t meanEtaPtDeltaRel( histEtaPtDeltaRel->GetMean() );
            const Double_t widthEtaPtDeltaRel( std::fabs( histEtaPtDeltaRel->GetRMS() ) );
            if ( widthEtaPtDeltaRel == 0. && verbose_ > 2 ) {
              std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                        << "    no histogram \"width\" in '" << nameEtaPtDeltaRel << "'" << std::endl;
            }
            const Double_t rangeEtaPtDeltaRelRebin( widthEtaPtDeltaRel == 0. ? widthFactor_ * std::fabs( histEtaPtDeltaRel->GetXaxis()->GetXmax() ) : widthFactor_ * widthEtaPtDeltaRel ); // FIXME: tune, incl. under- and overflow, remove hard-coding
            TH1D * histEtaPtDeltaRelRebin( new TH1D( nameEtaPtDeltaRelRebin.c_str(), titleEtaPtDelta.c_str(), deltaBinsRebinRel, -rangeEtaPtDeltaRelRebin + meanEtaPtDeltaRel, rangeEtaPtDeltaRelRebin + meanEtaPtDeltaRel ) );
            histEtaPtDeltaRelRebin->SetXTitle( histEtaPtDeltaRel->GetXaxis()->GetTitle() );
            histEtaPtDeltaRelRebin->SetYTitle( histEtaPtDeltaRel->GetYaxis()->GetTitle() );

            for ( unsigned uEntry = 0; uEntry < sizePt.at( uPt ); ++uEntry ) {
              if ( refGen_ ) {
                if ( inverse && kinProp == "Pt" ) histEtaPtDeltaRebin->Fill( 1. / propGenEtaBin.at( uPt ).at( uEntry ) - 1. / propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                else                              histEtaPtDeltaRebin->Fill( propGenEtaBin.at( uPt ).at( uEntry ) - propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                if ( ! inverse && kinProp == "Pt" ) {
                  histEtaPtDeltaRelRebin->Fill( ( propGenEtaBin.at( uPt ).at( uEntry ) - propEtaBin.at( uPt ).at( uEntry ) ) / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                }
              }
              else {
                if ( inverse && kinProp == "Pt" ) histEtaPtDeltaRebin->Fill( 1. / propEtaBin.at( uPt ).at( uEntry ) - 1. / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                else                              histEtaPtDeltaRebin->Fill( propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                if ( ! inverse && kinProp == "Pt" ) {
                  histEtaPtDeltaRelRebin->Fill( ( propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ) ) / propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                }
              }
            } // loop: uEntry < ptEtaBin.at( uPt ).size()

            histEtaPtFrac->Scale( 1. / histEtaPtFrac->Integral() );
            histEtaPtFracRestr->Scale( 1. / histEtaPtFracRestr->Integral() );

          } // loop: uPt < nPtBins_

          const std::string nameEtaDeltaRebin( nameEtaDelta + "Rebin" );
//           const Int_t deltaBinsRebin( inverse ? propInvDeltaBins_ : propDeltaBins_ ); // FIXME: tune number of bins
          const Int_t deltaBinsRebin( deltaBins );
          const Double_t meanEtaDelta( histEtaDelta->GetMean() );
          const Double_t widthEtaDelta( std::fabs( histEtaDelta->GetRMS() ) );
          if ( widthEtaDelta == 0. && verbose_ > 2 ) {
            std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                      << "    no histogram \"width\" in '" << nameEtaDelta << "'" << std::endl;
          }
          const Double_t rangeEtaDeltaRebin( widthEtaDelta == 0. ? widthFactor_ * std::fabs( histEtaDelta->GetXaxis()->GetXmax() ) : widthFactor_ * widthEtaDelta ); // FIXME: tune, incl. under- and overflow, remove hard-coding
          TH1D * histEtaDeltaRebin( new TH1D( nameEtaDeltaRebin.c_str(), titleEtaDelta.c_str(), deltaBinsRebin, meanEtaDelta - rangeEtaDeltaRebin, meanEtaDelta + rangeEtaDeltaRebin ) );
          histEtaDeltaRebin->SetXTitle( histEtaDelta->GetXaxis()->GetTitle() );
          histEtaDeltaRebin->SetYTitle( histEtaDelta->GetYaxis()->GetTitle() );

          const std::string nameEtaDeltaRelRebin( nameEtaDeltaRel + "Rebin" );
//           const Int_t deltaBinsRebin( inverse ? propInvDeltaBins_ : propDeltaBins_ ); // FIXME: tune number of bins
          const Int_t deltaBinsRebinRel( deltaBins );
          const Double_t meanEtaDeltaRel( histEtaDeltaRel->GetMean() );
          const Double_t widthEtaDeltaRel( std::fabs( histEtaDeltaRel->GetRMS() ) );
          if ( widthEtaDeltaRel == 0. && verbose_ > 2 ) {
            std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                      << "    no histogram \"width\" in '" << nameEtaDeltaRel << "'" << std::endl;
          }
          const Double_t rangeEtaDeltaRelRebin( widthEtaDeltaRel == 0. ? widthFactor_ * std::fabs( histEtaDeltaRel->GetXaxis()->GetXmax() ) : widthFactor_ * widthEtaDeltaRel ); // FIXME: tune, incl. under- and overflow, remove hard-coding
          TH1D * histEtaDeltaRelRebin( new TH1D( nameEtaDeltaRelRebin.c_str(), titleEtaDelta.c_str(), deltaBinsRebinRel, -rangeEtaDeltaRelRebin + meanEtaDeltaRel, rangeEtaDeltaRelRebin + meanEtaDeltaRel ) );
          histEtaDeltaRelRebin->SetXTitle( histEtaDeltaRel->GetXaxis()->GetTitle() );
          histEtaDeltaRelRebin->SetYTitle( histEtaDeltaRel->GetYaxis()->GetTitle() );

          for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
            for ( unsigned uEntry = 0; uEntry < sizePt.at( uPt ); ++uEntry ) {
              if ( refGen_ ) {
                if ( inverse && kinProp == "Pt" ) histEtaDeltaRebin->Fill( 1. / propGenEtaBin.at( uPt ).at( uEntry ) - 1. / propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                else                              histEtaDeltaRebin->Fill( propGenEtaBin.at( uPt ).at( uEntry ) - propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                if ( ! inverse && kinProp == "Pt" ) {
                  histEtaDeltaRelRebin->Fill( ( propGenEtaBin.at( uPt ).at( uEntry ) - propEtaBin.at( uPt ).at( uEntry ) ) / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                }
              }
              else {
                if ( inverse && kinProp == "Pt" ) histEtaDeltaRebin->Fill( 1. / propEtaBin.at( uPt ).at( uEntry ) - 1. / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                else                              histEtaDeltaRebin->Fill( propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                if ( ! inverse && kinProp == "Pt" ) {
                  histEtaDeltaRelRebin->Fill( ( propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ) ) / propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                }
              }
            } // loop: uEntry < ptEtaBin.at( uPt ).size()
          } // loop: uPt < nPtBins_

          histEtaFrac->Scale( 1. / histEtaFrac->Integral() );
          histEtaFracRestr->Scale( 1. / histEtaFracRestr->Integral() );

        } // loop: keyEta

        dirFit_->cd();

        const std::string nameDeltaRebin( nameDelta + "Rebin" );
//         const Int_t deltaBinsRebin( inverse ? propInvDeltaBins_ : propDeltaBins_ ); // FIXME: tune number of bins
        const Int_t deltaBinsRebin( deltaBins );
        const Double_t meanDelta( histDelta->GetMean() );
        const Double_t widthDelta( std::fabs( histDelta->GetRMS() ) );
        if ( widthDelta == 0. && verbose_ > 2 ) {
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    no histogram \"width\" in '" << nameDelta << "'" << std::endl;
        }
        const Double_t rangeDeltaRebin( widthDelta == 0. ? widthFactor_ * std::fabs( histDelta->GetXaxis()->GetXmax() ) : widthFactor_ * widthDelta ); // FIXME: tune, incl. under- and overflow, remove hard-coding
        TH1D * histDeltaRebin( new TH1D( nameDeltaRebin.c_str(), objCat.c_str(), deltaBinsRebin, meanDelta - rangeDeltaRebin, meanDelta + rangeDeltaRebin ) );
        histDeltaRebin->SetXTitle( histDelta->GetXaxis()->GetTitle() );
        histDeltaRebin->SetYTitle( histDelta->GetYaxis()->GetTitle() );

        const std::string nameDeltaRelRebin( nameDeltaRel + "Rebin" );
//         const Int_t deltaBinsRebinRel( inverse ? propInvDeltaBins_ : propDeltaBins_ ); // FIXME: tune number of bins
        const Int_t deltaBinsRebinRel( deltaBins );
        const Double_t meanDeltaRel( histDeltaRel->GetMean() );
        const Double_t widthDeltaRel( std::fabs( histDeltaRel->GetRMS() ) );
        if ( widthDeltaRel == 0. && verbose_ > 2 ) {
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    no histogram \"width\" in '" << nameDeltaRel << "'" << std::endl;
        }
        const Double_t rangeDeltaRelRebin( widthDeltaRel == 0. ? widthFactor_ * std::fabs( histDeltaRel->GetXaxis()->GetXmax() ) : widthFactor_ * widthDeltaRel ); // FIXME: tune, incl. under- and overflow, remove hard-coding
        TH1D * histDeltaRelRebin( new TH1D( nameDeltaRelRebin.c_str(), objCat.c_str(), deltaBinsRebinRel, -rangeDeltaRelRebin + meanDeltaRel, rangeDeltaRelRebin + meanDeltaRel ) );
        histDeltaRelRebin->SetXTitle( histDeltaRel->GetXaxis()->GetTitle() );
        histDeltaRelRebin->SetYTitle( histDeltaRel->GetYaxis()->GetTitle() );

        std::vector< TH1D * > histVecPtDeltaRebin;
        std::vector< TH1D * > histVecPtDeltaRelRebin;
        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
          const std::string namePt( name + "_Pt" + binPt );

          const std::string namePtDelta( namePt + "_Delta" );
          const std::string titlePtDelta( objCat + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq p_{t} #leq " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
          const std::string namePtDeltaRebin( namePtDelta + "Rebin" );
//           const Int_t deltaBinsRebin( inverse ? propInvDeltaBins_ : propDeltaBins_ ); // FIXME: tune number of bins
          const Int_t deltaBinsRebin( deltaBins );
          const Double_t meanPtDelta( histVecPtDelta.at( uPt )->GetMean() );
          const Double_t widthPtDelta( std::fabs( histVecPtDelta.at( uPt )->GetRMS() ) );
          if ( widthPtDelta == 0. && verbose_ > 2 ) {
            std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                      << "    no histogram \"width\" in '" << namePtDelta << "'" << std::endl;
          }
          const Double_t rangePtDeltaRebin( widthPtDelta == 0. ? widthFactor_ * std::fabs( histVecPtDelta.at( uPt )->GetXaxis()->GetXmax() ) : widthFactor_ * widthPtDelta ); // FIXME: tune, incl. under- and overflow, remove hard-coding
          TH1D * histPtDeltaRebin( new TH1D( namePtDeltaRebin.c_str(), titlePtDelta.c_str(), deltaBinsRebin, -rangePtDeltaRebin + meanPtDelta, rangePtDeltaRebin + meanPtDelta ) );
          histPtDeltaRebin->SetXTitle( titleDelta.c_str() );
          histPtDeltaRebin->SetYTitle( titleEvents.c_str() );
          histVecPtDeltaRebin.push_back( histPtDeltaRebin );

          if ( ! inverse && kinProp == "Pt" ) {
            const std::string namePtDeltaRel( namePtDelta + "Rel" );
            const std::string namePtDeltaRelRebin( namePtDeltaRel + "Rebin" );
//             const Int_t deltaBinsRebin( inverse ? propInvDeltaBins_ : propDeltaBins_ ); // FIXME: tune number of bins
            const Double_t meanPtDeltaRel( histVecPtDeltaRel.at( uPt )->GetMean() );
            const Double_t widthPtDeltaRel( std::fabs( histVecPtDeltaRel.at( uPt )->GetRMS() ) );
            if ( widthPtDeltaRel == 0. && verbose_ > 2 ) {
              std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                        << "    no histogram \"width\" in '" << namePtDeltaRel << "'" << std::endl;
            }
            const Double_t rangePtDeltaRelRebin( widthPtDeltaRel == 0. ? widthFactor_ * std::fabs( histVecPtDeltaRel.at( uPt )->GetXaxis()->GetXmax() ) : widthFactor_ * widthPtDeltaRel ); // FIXME: tune, incl. under- and overflow, remove hard-coding
            TH1D * histPtDeltaRelRebin( new TH1D( namePtDeltaRelRebin.c_str(), titlePtDelta.c_str(), deltaBinsRebinRel, -rangePtDeltaRelRebin + meanPtDeltaRel, rangePtDeltaRelRebin + meanPtDeltaRel ) );
            histPtDeltaRelRebin->SetXTitle( titleDeltaRel.c_str() );
            histPtDeltaRelRebin->SetYTitle( titleEvents.c_str() );
            histVecPtDeltaRelRebin.push_back( histPtDeltaRelRebin );
          }

        } // loop: uPt < nPtBins_

        if ( sizeEtaBins != nEtaBins_ ) {
          std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                    << "    mismatch of eta binning for object category '" << objCat << "':" << std::endl
                    << "        bins in directory structure: " << sizeEtaBins                << std::endl
                    << "        bins in binning histogram  : " << nEtaBins_                  << std::endl;
        }

        nextInListFit = listFit;
        while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
          if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
          const std::string binEta( keyEta->GetName() );
          const unsigned uEta( std::atoi( binEta.substr( 3 ).data() ) );

          for ( unsigned uEntry = 0; uEntry < sizeEta_.at( uEta ); ++uEntry ) {
            if ( propData_.at( uEta ).at( uEntry ) == -5. ) continue; // no match
            if ( refGen_ ) {
              if ( inverse && kinProp == "Pt" ) histDeltaRebin->Fill( 1. / propGenData_.at( uEta ).at( uEntry ) - 1. / propData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
              else                              histDeltaRebin->Fill( propGenData_.at( uEta ).at( uEntry ) - propData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
              if ( ! inverse && kinProp == "Pt" ) {
                histDeltaRelRebin->Fill( ( propGenData_.at( uEta ).at( uEntry ) - propData_.at( uEta ).at( uEntry ) ) / propGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
              }
            }
            else {
              if ( inverse && kinProp == "Pt" ) histDeltaRebin->Fill( 1. / propData_.at( uEta ).at( uEntry ) - 1. / propGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
              else                              histDeltaRebin->Fill( propData_.at( uEta ).at( uEntry ) - propGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
              if ( ! inverse && kinProp == "Pt" ) {
                histDeltaRelRebin->Fill( ( propData_.at( uEta ).at( uEntry ) - propGenData_.at( uEta ).at( uEntry ) ) /propData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
              }
            }
            for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
              if ( ptBins_.at( uPt ) <= ptData_.at( uEta ).at( uEntry ) && ptData_.at( uEta ).at( uEntry ) < ptBins_.at( uPt + 1 ) ) {
                if ( refGen_ ) {
                  if ( inverse && kinProp == "Pt" ) histVecPtDeltaRebin.at( uPt )->Fill( 1. / propGenData_.at( uEta ).at( uEntry ) - 1. / propData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                  else                              histVecPtDeltaRebin.at( uPt )->Fill( propGenData_.at( uEta ).at( uEntry ) - propData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                  if ( ! inverse && kinProp == "Pt" ) {
                    histVecPtDeltaRelRebin.at( uPt )->Fill( ( propGenData_.at( uEta ).at( uEntry ) - propData_.at( uEta ).at( uEntry ) ) / propGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                  }
                }
                else {
                  if ( inverse && kinProp == "Pt" ) histVecPtDeltaRebin.at( uPt )->Fill( 1. / propData_.at( uEta ).at( uEntry ) - 1. / propGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                  else                              histVecPtDeltaRebin.at( uPt )->Fill( propData_.at( uEta ).at( uEntry ) - propGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                  if ( ! inverse && kinProp == "Pt" ) {
                    histVecPtDeltaRelRebin.at( uPt )->Fill( ( propData_.at( uEta ).at( uEntry ) - propGenData_.at( uEta ).at( uEntry ) ) / propData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                  }
                }
                break;
              }

            } // loop: uPt < nPtBins
          } // loop: uEntry < sizeEta_.at( uEta )

        } // loop: keyEta

        histFrac->Scale( 1. / histFrac->Integral() );
        histFracRestr->Scale( 1. / histFracRestr->Integral() );
        for ( unsigned uPt = 0; uPt < histVecPtFrac.size(); ++uPt ) {
          histVecPtFrac.at( uPt )->Scale( 1. / histVecPtFrac.at( uPt )->Integral() );
        }
        for ( unsigned uPt = 0; uPt < histVecPtFracRestr.size(); ++uPt ) {
          histVecPtFracRestr.at( uPt )->Scale( 1. / histVecPtFracRestr.at( uPt )->Integral() );
        }

      } // loop: keyFit

      ++uProp;

    } // loop: keyProp


    // Fit resolution functions

    if ( fitResFuncs_ ) {
      if ( verbose_ > 1 ) {
        std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                  << "    resolution function determination for " << objCat << " started" << std::endl;
      }

      // Save fit results for text output
      std::vector< std::vector< double > > Cs, Rs, Ns; // resolution parameter vectors

      // Loop over kinematic properties

      TList * listCat( dirCat_->GetListOfKeys() );
      TIter nextInListCat( listCat );
      unsigned uProp( 0 );
      while ( TKey * keyProp = ( TKey* )nextInListCat() ) {
        if ( std::string( keyProp->GetClassName() ) != nameDirClass ) continue;
        if ( onlyExisting_ && ! ( uProp < nominalInv_.at( uCat ).size() ) ) {
          if ( verbose_ > 2 ) {
            std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                      << "    only " << nominalInv_.at( uCat ).size() << " kinematic properties available for " << objCat << std::endl
                      << "    but  " << uProp + 1 << " requested" << std::endl;
          }
          break;
        }
        const std::string kinProp( keyProp->GetName() );
        TDirectory * dirProp_( dynamic_cast< TDirectory* >( dirCat_->Get( kinProp.c_str() ) ) );

        // Histogram binning
        const double propDeltaMax_( histos_.getParameter< double >( std::string( objCat + "Delta" + kinProp + "Max" ) ) );
        const double propInvDeltaMax_( histos_.getParameter< double >( std::string( objCat + "Delta" + kinProp + "InvMax" ) ) );

        Cs.push_back( std::vector< double >( nEtaBins_ ) );
        Rs.push_back( std::vector< double >( nEtaBins_ ) );
        Ns.push_back( std::vector< double >( nEtaBins_ ) );

        // Loop over fit versions
        TList * listProp( dirProp_->GetListOfKeys() );
        TIter nextInListProp( listProp );
        while ( TKey * keyFit = ( TKey* )nextInListProp() ) {
          if ( std::string( keyFit->GetClassName() ) != nameDirClass ) continue;
          const std::string subFit( keyFit->GetName() );
          // These are real switches: depending on configuration, only one setting combination can be run at a time
          if ( useAlt_  == ( subFit.find( "Alt" )  == std::string::npos ) ) continue;
          if ( useSymm_ == ( subFit.find( "Symm" ) == std::string::npos ) ) continue;
          if ( refGen_  == ( subFit.find( "Gen" )  == std::string::npos ) ) continue;
          TDirectory * dirFit_( dynamic_cast< TDirectory* >( dirProp_->Get( subFit.c_str() ) ) );
          dirFit_->cd();

          // Inversion flags
          const bool inverse( subFit.find( "Inv" ) != std::string::npos );
          if ( onlyExisting_ ) {
            if ( nominalInv_.at( uCat ).at( uProp ) != inverse ) {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                          << "    skipping unnominal directory '"; gDirectory->pwd();
              }
              continue;
            }
            else if ( verbose_ > 3 ) {
              std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                        << "    entering nominal directory '"; gDirectory->pwd();
            }
          }

          const std::string name( objCat + "_" + kinProp + "_" + subFit );

          std::string titleDelta( titlesDelta.at( uProp ) );
          std::string titleFrac( titlesFrac.at( uProp ) );
          std::string titleDeltaRel( titlesDeltaRel.at( uProp ) );
          if ( inverse ) {
            if ( useAlt_ ) titleDelta = titlesDeltaAltInv.at( uProp );
            else           titleDelta = titlesDeltaInv.at( uProp );
            titleFrac     = "";
            titleDeltaRel = "";
          }
          else if ( useAlt_ ) {
            titleDelta    = titlesDeltaAlt.at( uProp );
            titleFrac     = titlesFracAlt.at( uProp );
            titleDeltaRel = titlesDeltaAltRel.at( uProp );
          }
          if ( refGen_ ) {
            titlePt = "p_{t}^GEN (GeV)";
            titleEta = "#eta^GEN";
            titlePhi = "#phi^GEN";
          }

          // Fit performance histograms

          const std::string titleMean( "#mu of " + titleDelta );
          const std::string titleSigma( "#sigma of " + titleDelta );
          const double deltaMax( inverse ? propInvDeltaMax_ : propDeltaMax_ );

          // Single resolutions bins
          const std::string nameDeltaEtaPtFitChi2Map( name + "_DeltaEtaPt_FitChi2Map" );
          TH2D * histDeltaEtaPtFitChi2Map( new TH2D( nameDeltaEtaPtFitChi2Map.c_str(), titleChi2.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
          histDeltaEtaPtFitChi2Map->SetXTitle( titleEta.c_str() );
          histDeltaEtaPtFitChi2Map->SetYTitle( titlePt.c_str() );
          histDeltaEtaPtFitChi2Map->SetZTitle( titleChi2.c_str() );
          const std::string nameDeltaEtaPtFitSigmaMap( name + "_DeltaEtaPt_FitSigmaMap" );
          TH2D * histDeltaEtaPtFitSigmaMap( new TH2D( nameDeltaEtaPtFitSigmaMap.c_str(), titleSigma.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
          histDeltaEtaPtFitSigmaMap->SetXTitle( titleEta.c_str() );
          histDeltaEtaPtFitSigmaMap->SetYTitle( titlePt.c_str() );
          histDeltaEtaPtFitSigmaMap->SetZTitle( titleSigma.c_str() );
          const std::string nameDeltaEtaPtFitProbMap( name + "_DeltaEtaPt_FitProbMap" );
          TH2D * histDeltaEtaPtFitProbMap( new TH2D( nameDeltaEtaPtFitProbMap.c_str(), titleProb.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
          histDeltaEtaPtFitProbMap->SetXTitle( titleEta.c_str() );
          histDeltaEtaPtFitProbMap->SetYTitle( titlePt.c_str() );
          histDeltaEtaPtFitProbMap->SetZTitle( titleProb.c_str() );
          const std::string nameDeltaEtaPtFitSigma( name + "_DeltaEtaPt_FitSigma" );
          TH1D * histDeltaEtaPtFitSigma( new TH1D( nameDeltaEtaPtFitSigma.c_str(), titleProb.c_str(), 20, 0., deltaMax ) );
          histDeltaEtaPtFitSigma->SetXTitle( titleSigma.c_str() );
          const std::string nameDeltaEtaPtFitProb( name + "_DeltaEtaPt_FitProb" );
          TH1D * histDeltaEtaPtFitProb( new TH1D( nameDeltaEtaPtFitProb.c_str(), titleProb.c_str(), 20, 0., 1. ) );
          histDeltaEtaPtFitProb->SetXTitle( titleProb.c_str() );
          // Failing fits
          const std::string nameDeltaEtaPtFitMissingMap( name + "_DeltaEtaPt_FitMissingMap" );
          TH2D * histDeltaEtaPtFitMissingMap( new TH2D( nameDeltaEtaPtFitMissingMap.c_str(), titleProb.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
          histDeltaEtaPtFitMissingMap->SetXTitle( titleEta.c_str() );
          histDeltaEtaPtFitMissingMap->SetYTitle( titlePt.c_str() );
          const std::string nameDeltaEtaPtFitBadProbMap( name + "_DeltaEtaPt_FitBadProbMap" );
          TH2D * histDeltaEtaPtFitBadProbMap( new TH2D( nameDeltaEtaPtFitBadProbMap.c_str(), titleProb.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
          histDeltaEtaPtFitBadProbMap->SetXTitle( titleEta.c_str() );
          histDeltaEtaPtFitBadProbMap->SetYTitle( titlePt.c_str() );
          histDeltaEtaPtFitBadProbMap->SetZTitle( titleProb.c_str() );
          const std::string nameDeltaEtaPtFitBadNdfMap( name + "_DeltaEtaPt_FitBadNdfMap" );
          TH2D * histDeltaEtaPtFitBadNdfMap( new TH2D( nameDeltaEtaPtFitBadNdfMap.c_str(), titleNdf.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
          histDeltaEtaPtFitBadNdfMap->SetXTitle( titleEta.c_str() );
          histDeltaEtaPtFitBadNdfMap->SetYTitle( titlePt.c_str() );
          histDeltaEtaPtFitBadNdfMap->SetZTitle( titleNdf.c_str() );
          const std::string nameDeltaEtaPtFitBadProb( name + "_DeltaEtaPt_FitBadProb" );
          TH1D * histDeltaEtaPtFitBadProb( new TH1D( nameDeltaEtaPtFitBadProb.c_str(), titleProb.c_str(), 20, 0., 1. ) );
          histDeltaEtaPtFitBadProb->SetXTitle( titleProb.c_str() );
          const std::string nameDeltaEtaPtFitBadNdf( name + "_DeltaEtaPt_FitBadNdf" );
          TH1D * histDeltaEtaPtFitBadNdf( new TH1D( nameDeltaEtaPtFitBadNdf.c_str(), titleNdf.c_str(), 20, 0., 20. ) );
          histDeltaEtaPtFitBadNdf->SetXTitle( titleNdf.c_str() );

          // Resolution functions
          const std::string nameSigmaEtaFitChi2Map( name + "_SigmaEta_FitChi2Map" );
          TH1D * histSigmaEtaFitChi2Map( new TH1D( nameSigmaEtaFitChi2Map.c_str(), titleChi2.c_str(), nEtaBins_, etaBins_.data() ) );
          histSigmaEtaFitChi2Map->SetXTitle( titleEta.c_str() );
          histSigmaEtaFitChi2Map->SetYTitle( titleChi2.c_str() );
          const std::string nameSigmaEtaFitProbMap( name + "_SigmaEta_FitProbMap" );
          TH1D * histSigmaEtaFitProbMap( new TH1D( nameSigmaEtaFitProbMap.c_str(), titleProb.c_str(), nEtaBins_, etaBins_.data() ) );
          histSigmaEtaFitProbMap->SetXTitle( titleEta.c_str() );
          histSigmaEtaFitProbMap->SetYTitle( titleProb.c_str() );
          const std::string nameSigmaEtaFitProb( name + "_SigmaEta_FitProb" );
          TH1D * histSigmaEtaFitProb( new TH1D( nameSigmaEtaFitProb.c_str(), titleProb.c_str(), 20, 0., 1. ) );
          histSigmaEtaFitProb->SetXTitle( titleProb.c_str() );
          // Failing fits
          const std::string nameSigmaEtaFitMissingMap( name + "_SigmaEta_FitMissingMap" );
          TH1D * histSigmaEtaFitMissingMap( new TH1D( nameSigmaEtaFitMissingMap.c_str(), titleProb.c_str(), nEtaBins_, etaBins_.data() ) );
          histSigmaEtaFitMissingMap->SetXTitle( titleEta.c_str() );
          const std::string nameSigmaEtaFitBadProbMap( name + "_SigmaEta_FitBadProbMap" );
          TH1D * histSigmaEtaFitBadProbMap( new TH1D( nameSigmaEtaFitBadProbMap.c_str(), titleProb.c_str(), nEtaBins_, etaBins_.data() ) );
          histSigmaEtaFitBadProbMap->SetXTitle( titleEta.c_str() );
          histSigmaEtaFitBadProbMap->SetYTitle( titleProb.c_str() );
          const std::string nameSigmaEtaFitBadNdfMap( name + "_SigmaEta_FitBadNdfMap" );
          TH1D * histSigmaEtaFitBadNdfMap( new TH1D( nameSigmaEtaFitBadNdfMap.c_str(), titleNdf.c_str(), nEtaBins_, etaBins_.data() ) );
          histSigmaEtaFitBadNdfMap->SetXTitle( titleEta.c_str() );
          histSigmaEtaFitBadNdfMap->SetYTitle( titleNdf.c_str() );
          histSigmaEtaFitBadNdfMap->SetXTitle( titleProb.c_str() );
          const std::string nameSigmaEtaFitBadNdf( name + "_SigmaEta_FitBadNdff" );
          TH1D * histSigmaEtaFitBadNdf( new TH1D( nameSigmaEtaFitBadNdf.c_str(), titleNdf.c_str(), 20, 0., 20. ) );
          histSigmaEtaFitBadNdf->SetXTitle( titleNdf.c_str() );

          // Loop over eta bins
          TList * listFit( dirFit_->GetListOfKeys() );
          TIter nextInListFit( listFit );
          while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
            if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
            const std::string binEta( keyEta->GetName() );
            const unsigned uEta( std::atoi( binEta.substr( 3 ).data() ) );
            dirFit_->cd( binEta.c_str() );

            const std::string nameEta( name + "_" + binEta );
            const std::string formula( inverse ? resFuncInv_ : resFunc_ );
            const std::string formulaRel( inverse ? resFuncInvRel_ : resFuncRel_ );

            // Fit performance histograms

            const std::string nameDeltaFitChi2( nameEta + "_DeltaFitChi2" );
            TH1D * histDeltaFitChi2( new TH1D( nameDeltaFitChi2.c_str(), titleChi2.c_str(), nPtBins_, ptBins_.data() ) );
            histDeltaFitChi2->SetXTitle( titlePt.c_str() );
            histDeltaFitChi2->SetYTitle( titleChi2.c_str() );
            const std::string nameDeltaFitProb( nameEta + "_DeltaFitProb" );
            TH1D * histDeltaPtFitProb( new TH1D( nameDeltaFitProb.c_str(), titleProb.c_str(), nPtBins_, ptBins_.data() ) );
            histDeltaPtFitProb->SetXTitle( titlePt.c_str() );
            histDeltaPtFitProb->SetYTitle( titleProb.c_str() );

            const std::string nameSigmaEta( nameEta + "_Sigma" );
            const std::string titleSigmaEta( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) );
            TH1D * histSigmaEta( new TH1D( nameSigmaEta.c_str(), titleSigmaEta.c_str(), nPtBins_, ptBins_.data() ) );
            histSigmaEta->SetXTitle( titlePt.c_str() );

            for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
              const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
              const std::string nameEtaPt( nameEta + "_Pt" + binPt );

              const std::string nameEtaPtDelta( nameEtaPt + "_Delta" );
              const std::string nameEtaPtDeltaRebin( nameEtaPtDelta + "Rebin" );
              TH1D * histEtaPtDeltaRebin( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaPtDeltaRebin.c_str() ) ) );

              const std::string nameEtaPtDeltaFit( nameEtaPtDelta + "_fit" );
              const std::string nameEtaPtDeltaRebinFit( nameEtaPtDeltaRebin + "_fit" );
              TF1 * fitEtaPtDeltaRebin( new TF1( nameEtaPtDeltaRebinFit.c_str(), fitFunctionResFuncs_.c_str(), std::max( histEtaPtDeltaRebin->GetXaxis()->GetXmin(), histEtaPtDeltaRebin->GetMean() - histEtaPtDeltaRebin->GetRMS() * fitRangeResFuncs_ ), std::min( histEtaPtDeltaRebin->GetXaxis()->GetXmax(), histEtaPtDeltaRebin->GetMean() + histEtaPtDeltaRebin->GetRMS() * fitRangeResFuncs_ ) ) );
              TFitResultPtr fitEtaPtDeltaResultPtr( histEtaPtDeltaRebin->Fit( fitEtaPtDeltaRebin, fitOptions_.c_str() ) );
              if ( fitEtaPtDeltaResultPtr >= 0 ) {
                if ( fitEtaPtDeltaResultPtr->Status() == 0 && fitEtaPtDeltaResultPtr->Ndf() != 0. ) {
                  histDeltaEtaPtFitChi2Map->SetBinContent( uEta + 1, uPt + 1, fitEtaPtDeltaResultPtr->Chi2() / fitEtaPtDeltaResultPtr->Ndf() );
                  histDeltaFitChi2->SetBinContent( uPt + 1, fitEtaPtDeltaResultPtr->Chi2() / fitEtaPtDeltaResultPtr->Ndf() );
                  histDeltaEtaPtFitSigmaMap->SetBinContent( uEta + 1, uPt + 1, fitEtaPtDeltaResultPtr->Parameter( 2 ) );
                  histDeltaEtaPtFitProbMap->SetBinContent( uEta + 1, uPt + 1, fitEtaPtDeltaResultPtr->Prob() );
                  histDeltaEtaPtFitSigma->Fill( fitEtaPtDeltaResultPtr->Parameter( 2 ) );
                  histDeltaEtaPtFitProb->Fill( fitEtaPtDeltaResultPtr->Prob() );
                  histDeltaPtFitProb->SetBinContent( uPt + 1, fitEtaPtDeltaResultPtr->Prob() );
                  histSigmaEta->SetBinContent( uPt + 1, fitEtaPtDeltaResultPtr->Parameter( 2 ) );
                  histSigmaEta->SetBinError( uPt + 1, fitEtaPtDeltaResultPtr->ParError( 2 ) );
                }
                else {
                  if ( fitEtaPtDeltaResultPtr->Prob() == 0. ) {
                    histDeltaEtaPtFitBadProbMap->SetBinContent( uEta + 1, uPt + 1, -1. );
                    histDeltaEtaPtFitBadProb->AddBinContent( 0 );
                  }
                  else {
                    histDeltaEtaPtFitBadProbMap->SetBinContent( uEta + 1, uPt + 1, fitEtaPtDeltaResultPtr->Prob() );
                    histDeltaEtaPtFitBadProb->Fill( fitEtaPtDeltaResultPtr->Prob() );
                  }
                  histDeltaEtaPtFitBadNdfMap->SetBinContent( uEta + 1, uPt + 1, fitEtaPtDeltaResultPtr->Ndf() );
                  histDeltaEtaPtFitBadNdf->Fill( fitEtaPtDeltaResultPtr->Ndf() );
                  if ( verbose_ > 2 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    failing fit in directory '"; gDirectory->pwd();
                    std::cout << "    '" << nameEtaPtDeltaRebin << "' status " << fitEtaPtDeltaResultPtr->Status() << std::endl;
                  }
                }
              }
              else {
                const Int_t iBin( histDeltaEtaPtFitMissingMap->GetBin( uEta + 1, uPt + 1 ) );
                histDeltaEtaPtFitMissingMap->AddBinContent( iBin );
                if ( verbose_ > 1 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    missing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << nameEtaPtDeltaRebin << std::endl;
                }
              }
            } // loop: uPt < nPtBins_

            const std::string nameEtaSigmaFit( nameSigmaEta + "_fit" );
            TF1 * fitEtaSigmaFit( new TF1( nameEtaSigmaFit.c_str(), formula.c_str() ) );
            fitEtaSigmaFit->SetRange( histSigmaEta->GetXaxis()->GetXmin(), histSigmaEta->GetXaxis()->GetXmax() );
            TFitResultPtr fitEtaSigmaFitResultPtr( histSigmaEta->Fit( fitEtaSigmaFit, fitOptionsSigmaResFuncs_.c_str() ) );
            if ( fitEtaSigmaFitResultPtr >= 0 ) {
              if ( ( fitEtaSigmaFitResultPtr->Status() == 0 || fitEtaSigmaFitResultPtr->Status() == 4000 ) && fitEtaSigmaFitResultPtr->Ndf() != 0. ) { // ignore errors from IMPROVE
                histSigmaEtaFitChi2Map->SetBinContent( uEta + 1, fitEtaSigmaFitResultPtr->Chi2() / fitEtaSigmaFitResultPtr->Ndf() );
                histSigmaEtaFitProbMap->SetBinContent( uEta + 1, fitEtaSigmaFitResultPtr->Prob() );
                histSigmaEtaFitProb->Fill( fitEtaSigmaFitResultPtr->Prob() );
                if ( kinProp == "Pt" ) {
                  const Double_t * params( fitEtaSigmaFitResultPtr->GetParams() );
                  const std::string nameEtaSigmaFitRel( nameEtaSigmaFit + "Rel" );
                  TF1 * fitEtaSigmaRelFit( new TF1( nameEtaSigmaFitRel.c_str(), formulaRel.c_str(), histSigmaEta->GetXaxis()->GetXmin(), histSigmaEta->GetXaxis()->GetXmax() ) );
                  fitEtaSigmaRelFit->SetParameters( params );
                  if ( overwrite_ ) fitEtaSigmaRelFit->Write( 0, TObject::kOverwrite );
                  else              fitEtaSigmaRelFit->Write();
                  if ( inverse ) {
                    const std::string nameEtaSigmaFitInv( nameEtaSigmaFit + "Inv" );
                    TF1 * fitEtaSigmaInvFit( new TF1( nameEtaSigmaFitInv.c_str(), resFuncInvInv_.c_str(), histSigmaEta->GetXaxis()->GetXmin(), histSigmaEta->GetXaxis()->GetXmax() ) );
                    fitEtaSigmaInvFit->SetParameters( params );
                    if ( overwrite_ ) fitEtaSigmaInvFit->Write( 0, TObject::kOverwrite );
                    else              fitEtaSigmaInvFit->Write();
                    const std::string nameEtaSigmaFitInvRel( nameEtaSigmaFitInv + "Rel" );
                    TF1 * fitEtaSigmaInvRelFit( new TF1( nameEtaSigmaFitInvRel.c_str(), resFuncInvInvRel_.c_str(), histSigmaEta->GetXaxis()->GetXmin(), histSigmaEta->GetXaxis()->GetXmax() ) );
                    fitEtaSigmaInvRelFit->SetParameters( params );
                    if ( overwrite_ ) fitEtaSigmaInvRelFit->Write( 0, TObject::kOverwrite );
                    else              fitEtaSigmaInvRelFit->Write();
                  }
                }
                if ( writeResolutionFiles_ ) {
                  Cs.at( uProp ).at( uEta ) = std::fabs( fitEtaSigmaFit->GetParameter( 0 ) );
                  Rs.at( uProp ).at( uEta ) = std::fabs( fitEtaSigmaFit->GetParameter( 1 ) );
                  Ns.at( uProp ).at( uEta ) = std::fabs( fitEtaSigmaFit->GetParameter( 2 ) );
                }
                if ( verbose_ > 3 && fitEtaSigmaFitResultPtr->Status() == 4000 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    IMPROVE error in directory '"; gDirectory->pwd();
                  std::cout << "    '" << nameEtaSigmaFit << "' status " << fitEtaSigmaFitResultPtr->Status() << std::endl;
                }
              }
              else {
                if ( fitEtaSigmaFitResultPtr->Prob() == 0. ) {
                  histSigmaEtaFitBadProbMap->SetBinContent( uEta + 1, -1. );
//                   histSigmaNtupBadProb->AddBinContent( 0 );
                }
                else {
                  histSigmaEtaFitBadProbMap->SetBinContent( uEta + 1, fitEtaSigmaFitResultPtr->Prob() );
//                   histSigmaNtupBadProb->Fill( fitEtaSigmaFitResultPtr->Prob() );
                }
                histSigmaEtaFitBadNdfMap->SetBinContent( uEta + 1, fitEtaSigmaFitResultPtr->Ndf() );
                histSigmaEtaFitBadNdf->Fill( fitEtaSigmaFitResultPtr->Ndf() );
                if ( writeResolutionFiles_ ) {
                  Cs.at( uProp ).at( uEta ) = -1.;
                  Rs.at( uProp ).at( uEta ) = -1.;
                  Ns.at( uProp ).at( uEta ) = -1.;
                }
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << nameEtaSigmaFit << "' status " << fitEtaSigmaFitResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              histSigmaEtaFitMissingMap->AddBinContent( uEta + 1 );
              if ( writeResolutionFiles_ ) {
                Cs.at( uProp ).at( uEta ) = -1.;
                Rs.at( uProp ).at( uEta ) = -1.;
                Ns.at( uProp ).at( uEta ) = -1.;
              }
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << nameEtaSigmaFit << std::endl;
              }
            }

          } // loop: keyEta

        } // loop: keyFit

        ++uProp;

      } // loop: keyProp

      // Write fit results to text files
      if ( writeResolutionFiles_ ) {

        // File name
        std::string nameOut( pathOutResolution_ + "/gentResolution_" + sample_ + "_" + objCat );
        if ( usePileUp_ ) nameOut.append( "_PileUp" );
        if ( useAlt_ )    nameOut.append( "_Alt" );
        if ( useSymm_ )   nameOut.append( "_Symm" );
        if ( refGen_ )    nameOut.append( "_Gen" );
        if ( refSel_)     nameOut.append( "_Ref" );
        nameOut.append( ".txt" );

        ofstream fileOut;
        fileOut.open( nameOut.c_str(), std::ios_base::out );
        if ( objMetLike ) {
          fileOut << std::endl << "met_resolution = ";
          fileOut << std::setprecision( 6 );
          if ( Cs.at( 0 ).at( 0 ) < 0. ) fileOut << "NAN";
          else                           fileOut << Cs.at( 0 ).at( 0 );
          fileOut << ",";
          if ( Rs.at( 0 ).at( 0 ) < 0. ) fileOut << "NAN";
          else                           fileOut << Rs.at( 0 ).at( 0 );
          fileOut << ",";
          if ( Ns.at( 0 ).at( 0 ) < 0. ) fileOut << "NAN";
          else                           fileOut << Ns.at( 0 ).at( 0 );
        }
        else {
          unsigned nEta( 0 );
          if ( useSymm_ ) {
            for ( unsigned uEta = nEtaBins_; uEta > 0; --uEta ) {
              fileOut << std::endl << "etadep_etamin";
              fileOut.width( 3 );
              fileOut << std::left << nEta << "= ";
              fileOut << std::fixed << std::setprecision( 2 ) << -etaBins_.at( uEta );
              fileOut << std::endl << "etadep_etamax";
              fileOut.width( 3 );
              fileOut << std::left << nEta << "= ";
              fileOut << std::fixed << std::setprecision( 2 ) << -etaBins_.at( uEta - 1 );
              fileOut << std::endl << "etadep_vecres";
              fileOut.width( 3 );
              fileOut << std::left << nEta << "= ";
              for ( unsigned uProp = 0; uProp < nominalInv_.at( uCat ).size(); ++uProp ) {
                if ( uProp > 0 ) fileOut << "/";
                if ( nominalInv_.at( uCat ).at( uProp ) ) fileOut << "-";
                fileOut << std::setprecision( 8 );
                if ( Cs.at( uProp ).at( uEta - 1 ) < 0. ) fileOut << "NAN";
                else                                      fileOut << Cs.at( uProp ).at( uEta - 1 );
                fileOut << ",";
                if ( Rs.at( uProp ).at( uEta - 1 ) < 0. ) fileOut << "NAN";
                else                                      fileOut << Rs.at( uProp ).at( uEta - 1 );
                fileOut << ",";
                if ( Ns.at( uProp ).at( uEta - 1 ) < 0. ) fileOut << "NAN";
                else                                      fileOut << Ns.at( uProp ).at( uEta - 1 );
              }
              fileOut << "/et"; // FIXME: determine from existing
              fileOut << std::endl;
              ++nEta;
            }
          }
          for ( unsigned uEta = 0; uEta < nEtaBins_; ++uEta ) {
            fileOut << std::endl << "etadep_etamin";
            fileOut.width( 3 );
            fileOut << std::left << nEta << "= ";
            fileOut << std::fixed << std::setprecision( 2 ) << etaBins_.at( uEta );
            fileOut << std::endl << "etadep_etamax";
            fileOut.width( 3 );
            fileOut << std::left << nEta << "= ";
            fileOut << std::fixed << std::setprecision( 2 ) << etaBins_.at( uEta + 1 );
            fileOut << std::endl << "etadep_vecres";
            fileOut.width( 3 );
            fileOut << std::left << nEta << "= ";
            for ( unsigned uProp = 0; uProp < nominalInv_.at( uCat ).size(); ++uProp ) {
              if ( uProp > 0 ) fileOut << "/";
              if ( nominalInv_.at( uCat ).at( uProp ) ) fileOut << "-";
              fileOut << std::setprecision( 8 );
              if ( Cs.at( uProp ).at( uEta ) < 0. ) fileOut << "NAN";
              else                                  fileOut << Cs.at( uProp ).at( uEta );
              fileOut << ",";
              if ( Rs.at( uProp ).at( uEta ) < 0. ) fileOut << "NAN";
              else                                  fileOut << Rs.at( uProp ).at( uEta );
              fileOut << ",";
              if ( Ns.at( uProp ).at( uEta ) < 0. ) fileOut << "NAN";
              else                                  fileOut << Ns.at( uProp ).at( uEta );
            }
            fileOut << "/et"; // FIXME: determine from existing
            fileOut << std::endl;
            ++nEta;
          }
        }
        fileOut.close();

        std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                  << "    written resolution file for object category '" << objCat << "':" << std::endl
                  << "        " << nameOut << std::endl;

      }

    }


    // Fit L5L7 JECs

    if ( fitJecsL5L7_ ) {
      if ( objCat == "UdscJet" || objCat == "BJet" ) {
        if ( verbose_ > 1 ) {
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    JEC L5L7 determination for " << objCat << " started" << std::endl;
        }

        // Loop over kinematic properties

        TList * listCat( dirCat_->GetListOfKeys() );
        TIter nextInListCat( listCat );
        unsigned uProp( 0 );
        while ( TKey * keyProp = ( TKey* )nextInListCat() ) {
          if ( std::string( keyProp->GetClassName() ) != nameDirClass ) continue;
          if ( ! ( uProp < nominalInv_.at( uCat ).size() ) ) {
            if ( verbose_ > 2 ) {
              std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                        << "    only " << nominalInv_.at( uCat ).size() << " kinematic properties available for " << objCat << std::endl
                        << "    but  " << uProp + 1 << " requested" << std::endl;
            }
            break;
          }
          const std::string kinProp( keyProp->GetName() );
          if ( kinProp != "Pt" ) continue;
          TDirectory * dirProp_( dynamic_cast< TDirectory* >( dirCat_->Get( kinProp.c_str() ) ) );

          // Loop over fit versions
          TList * listProp( dirProp_->GetListOfKeys() );
          TIter nextInListProp( listProp );
          while ( TKey * keyFit = ( TKey* )nextInListProp() ) {
            if ( std::string( keyFit->GetClassName() ) != nameDirClass ) continue;
            const std::string subFit( keyFit->GetName() );
            if ( subFit.find( "Inv" ) != std::string::npos ) continue; // nothing to do for inverse
            // These are real switches: depending on configuration, only one setting combination can be run at a time
            if ( useAlt_  == ( subFit.find( "Alt" )  == std::string::npos ) ) continue;
            if ( useSymm_ == ( subFit.find( "Symm" ) == std::string::npos ) ) continue;
            if ( refGen_  == ( subFit.find( "Gen" )  == std::string::npos ) ) continue;
            TDirectory * dirFit_( dynamic_cast< TDirectory* >( dirProp_->Get( subFit.c_str() ) ) );
            dirFit_->cd();

            const std::string name( objCat + "_" + kinProp + "_" + subFit );

            std::string titleDelta( titlesDelta.at( uProp ) );
            std::string titleFrac( titlesFrac.at( uProp ) );
            std::string titleDeltaRel( titlesDeltaRel.at( uProp ) );
            if( useAlt_ ) {
              titleDelta    = titlesDeltaAlt.at( uProp );
              titleFrac     = titlesFracAlt.at( uProp );
              titleDeltaRel = titlesDeltaAltRel.at( uProp );
            }
            if ( refGen_ ) {
              titlePt = "p_{t}^GEN (GeV)";
              titleEta = "#eta^GEN";
              titlePhi = "#phi^GEN";
            }

            // Fit performance histograms

            const std::string titleMean( "#mu of " + titleFrac );
            const std::string titleSigma( "#sigma of " + titleFrac );

            // Single JEC bins
//             const std::string nameFracEtaPtFitMeanMap( name + "_FracEtaPt_FitMeanMap" );
//             TH2D * histFracEtaPtFitMeanMap( new TH2D( nameFracEtaPtFitMeanMap.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
//             histFracEtaPtFitMeanMap->SetXTitle( titleEta.c_str() );
//             histFracEtaPtFitMeanMap->SetYTitle( titlePt.c_str() );
//             const std::string nameFracEtaPtFitMean( name + "_FracEtaPt_FitMean" );
//             TH1D * histFracEtaPtFitMean( new TH1D( nameFracEtaPtFitMean.c_str(), titleProb.c_str(), 20, 0.75, 1.25 ) );
//             histFracEtaPtFitMean->SetXTitle( titleMean.c_str() );
            const std::string nameFracEtaFitMean( name + "_FracEta_FitMeanMap" );
            TH1D * histFracEtaFitMean( new TH1D( nameFracEtaFitMean.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data() ) );
            histFracEtaFitMean->SetXTitle( titleEta.c_str() );
            histFracEtaFitMean->SetYTitle( titleMean.c_str() );
//             const std::string nameFracRestrEtaPtFitMeanMap( name + "_FracRestrEtaPt_FitMeanMap" );
//             TH2D * histFracRestrEtaPtFitMeanMap( new TH2D( nameFracRestrEtaPtFitMeanMap.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
//             histFracRestrEtaPtFitMeanMap->SetXTitle( titleEta.c_str() );
//             histFracRestrEtaPtFitMeanMap->SetYTitle( titlePt.c_str() );
//             const std::string nameFracRestrEtaPtFitMean( name + "_FracRestrEtaPt_FitMean" );
//             TH1D * histFracRestrEtaPtFitMean( new TH1D( nameFracRestrEtaPtFitMean.c_str(), titleProb.c_str(), 20, 0.75, 1.25 ) );
//             histFracRestrEtaPtFitMean->SetXTitle( titleMean.c_str() );
            const std::string nameFracRestrEtaFitMean( name + "_FracRestrEta_FitMeanMap" );
            TH1D * histFracRestrEtaFitMean( new TH1D( nameFracRestrEtaFitMean.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data() ) );
            histFracRestrEtaFitMean->SetXTitle( titleEta.c_str() );
            histFracRestrEtaFitMean->SetYTitle( titleMean.c_str() );

            const std::string nameFrac( name + "_Frac" );
            TH1D * histFrac( dynamic_cast< TH1D* >( gDirectory->Get( nameFrac.c_str() ) ) );

            if ( histFrac != 0 ) {
              const std::string nameFracFit( nameFrac + "_fit" );
              TF1 * fitFrac( new TF1( nameFracFit.c_str(), fitFunctionJecsL5L7_.c_str(), std::max( histFrac->GetXaxis()->GetXmin(), histFrac->GetMean() - histFrac->GetRMS() * fitRangeJecsL5L7_ ), std::min( histFrac->GetXaxis()->GetXmax(), histFrac->GetMean() + histFrac->GetRMS() * fitRangeJecsL5L7_ ) ) );
              my::setParametersFit( fitFrac, histFrac );
              TFitResultPtr fitFracResultPtr( histFrac->Fit( fitFrac, fitOptionsL5L7_.c_str() ) );
              if ( fitFracResultPtr >= 0 ) {
                if ( fitFracResultPtr->Status() == 0 && fitFracResultPtr->Ndf() != 0. ) {
                }
                else {
                  if ( fitFracResultPtr->Prob() == 0. ) {
                  }
                  else {
                  }
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
              TF1 * bkgFrac( new TF1( nameFracBkg.c_str(), bkgFunctionJecsL5L7_.c_str(), std::max( histFrac->GetXaxis()->GetXmin(), histFrac->GetMean() - histFrac->GetRMS() * fitRangeJecsL5L7_ ), std::min( histFrac->GetXaxis()->GetXmax(), histFrac->GetMean() + histFrac->GetRMS() * fitRangeJecsL5L7_ ) ) );
              my::setParametersBkg( fitFrac, bkgFrac );
              if ( overwrite_ ) bkgFrac->Write( 0, TObject::kOverwrite );
              else              bkgFrac->Write();
            }

            const std::string nameFracRestr( nameFrac + "Restr" );
            TH1D * histFracRestr( dynamic_cast< TH1D* >( gDirectory->Get( nameFracRestr.c_str() ) ) );

            if ( histFracRestr != 0 ) {
              const std::string nameFracRestrFit( nameFracRestr + "_fit" );
              TF1 * fitFracRestr( new TF1( nameFracRestrFit.c_str(), fitFunctionJecsL5L7_.c_str(), std::max( histFracRestr->GetXaxis()->GetXmin(), histFracRestr->GetMean() - histFracRestr->GetRMS() * fitRangeJecsL5L7_ ), std::min( histFracRestr->GetXaxis()->GetXmax(), histFracRestr->GetMean() + histFracRestr->GetRMS() * fitRangeJecsL5L7_ ) ) );
              my::setParametersFit( fitFracRestr, histFracRestr );
              TFitResultPtr fitFracRestrResultPtr( histFracRestr->Fit( fitFracRestr, fitOptionsL5L7_.c_str() ) );
              if ( fitFracRestrResultPtr >= 0 ) {
                if ( fitFracRestrResultPtr->Status() == 0 && fitFracRestrResultPtr->Ndf() != 0. ) {
                }
                else {
                  if ( fitFracRestrResultPtr->Prob() == 0. ) {
                  }
                  else {
                  }
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
              TF1 * bkgFracRestr( new TF1( nameFracRestrBkg.c_str(), bkgFunctionJecsL5L7_.c_str(), std::max( histFracRestr->GetXaxis()->GetXmin(), histFracRestr->GetMean() - histFracRestr->GetRMS() * fitRangeJecsL5L7_ ), std::min( histFracRestr->GetXaxis()->GetXmax(), histFracRestr->GetMean() + histFracRestr->GetRMS() * fitRangeJecsL5L7_ ) ) );
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
                TF1 * fitPtFrac( new TF1( namePtFracFit.c_str(), fitFunctionJecsL5L7_.c_str(), std::max( histPtFrac->GetXaxis()->GetXmin(), histPtFrac->GetMean() - histPtFrac->GetRMS() * fitRangeJecsL5L7_ ), std::min( histPtFrac->GetXaxis()->GetXmax(), histPtFrac->GetMean() + histPtFrac->GetRMS() * fitRangeJecsL5L7_ ) ) );
                my::setParametersFit( fitPtFrac, histPtFrac );
                TFitResultPtr fitPtFracResultPtr( histPtFrac->Fit( fitPtFrac, fitOptionsL5L7_.c_str() ) );
                if ( fitPtFracResultPtr >= 0 ) {
                  if ( fitPtFracResultPtr->Status() == 0 && fitPtFracResultPtr->Ndf() != 0. ) {
                    histFracMean->SetBinContent( uPt + 1, fitPtFracResultPtr->Parameter( 0 ) );
                    histFracMean->SetBinError( uPt + 1, fitPtFracResultPtr->ParError( 0 ) );
                  }
                  else {
                    if ( fitPtFracResultPtr->Prob() == 0. ) {
                    }
                    else {
                    }
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
                TF1 * bkgPtFrac( new TF1( namePtFracBkg.c_str(), bkgFunctionJecsL5L7_.c_str(), std::max( histPtFrac->GetXaxis()->GetXmin(), histPtFrac->GetMean() - histPtFrac->GetRMS() * fitRangeJecsL5L7_ ), std::min( histPtFrac->GetXaxis()->GetXmax(), histPtFrac->GetMean() + histPtFrac->GetRMS() * fitRangeJecsL5L7_ ) ) );
                my::setParametersBkg( fitPtFrac, bkgPtFrac );
                if ( overwrite_ ) bkgPtFrac->Write( 0, TObject::kOverwrite );
                else              bkgPtFrac->Write();
              }

              const std::string namePtFracRestr( namePtFrac + "Restr" );
              TH1D * histPtFracRestr( dynamic_cast< TH1D* >( gDirectory->Get( namePtFracRestr.c_str() ) ) );

              if ( histPtFracRestr != 0 ) {
                const std::string namePtFracRestrFit( namePtFracRestr + "_fit" );
                TF1 * fitPtFracRestr( new TF1( namePtFracRestrFit.c_str(), fitFunctionJecsL5L7_.c_str(), std::max( histPtFracRestr->GetXaxis()->GetXmin(), histPtFracRestr->GetMean() - histPtFracRestr->GetRMS() * fitRangeJecsL5L7_ ), std::min( histPtFracRestr->GetXaxis()->GetXmax(), histPtFracRestr->GetMean() + histPtFracRestr->GetRMS() * fitRangeJecsL5L7_ ) ) );
                my::setParametersFit( fitPtFracRestr, histPtFracRestr );
                TFitResultPtr fitPtFracRestrResultPtr( histPtFracRestr->Fit( fitPtFracRestr, fitOptionsL5L7_.c_str() ) );
                if ( fitPtFracRestrResultPtr >= 0 ) {
                  if ( fitPtFracRestrResultPtr->Status() == 0 && fitPtFracRestrResultPtr->Ndf() != 0. ) {
                    histFracRestrMean->SetBinContent( uPt + 1, fitPtFracRestrResultPtr->Parameter( 0 ) );
                    histFracRestrMean->SetBinError( uPt + 1, fitPtFracRestrResultPtr->ParError( 0 ) );
                  }
                  else {
                    if ( fitPtFracRestrResultPtr->Prob() == 0. ) {
                    }
                    else {
                    }
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
                TF1 * bkgPtFracRestr( new TF1( namePtFracRestrBkg.c_str(), bkgFunctionJecsL5L7_.c_str(), std::max( histPtFracRestr->GetXaxis()->GetXmin(), histPtFracRestr->GetMean() - histPtFracRestr->GetRMS() * fitRangeJecsL5L7_ ), std::min( histPtFracRestr->GetXaxis()->GetXmax(), histPtFracRestr->GetMean() + histPtFracRestr->GetRMS() * fitRangeJecsL5L7_ ) ) );
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
                TF1 * fitEtaFrac( new TF1( nameEtaFracFit.c_str(), fitFunctionJecsL5L7_.c_str(), std::max( histEtaFrac->GetXaxis()->GetXmin(), histEtaFrac->GetMean() - histEtaFrac->GetRMS() * fitRangeJecsL5L7_ ), std::min( histEtaFrac->GetXaxis()->GetXmax(), histEtaFrac->GetMean() + histEtaFrac->GetRMS() * fitRangeJecsL5L7_ ) ) );
                my::setParametersFit( fitEtaFrac, histEtaFrac );
                TFitResultPtr fitEtaFracResultPtr( histEtaFrac->Fit( fitEtaFrac, fitOptionsL5L7_.c_str() ) );
                if ( fitEtaFracResultPtr >= 0 ) {
                  if ( fitEtaFracResultPtr->Status() == 0 && fitEtaFracResultPtr->Ndf() != 0. ) {
                    histFracEtaFitMean->SetBinContent( uEta + 1, fitEtaFracResultPtr->Parameter( 0 ) );
                    histFracEtaFitMean->SetBinError( uEta + 1, fitEtaFracResultPtr->ParError( 0 ) );
                  }
                  else {
                    if ( fitEtaFracResultPtr->Prob() == 0. ) {
                    }
                    else {
                    }
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
                TF1 * bkgEtaFrac( new TF1( nameEtaFracBkg.c_str(), bkgFunctionJecsL5L7_.c_str(), std::max( histEtaFrac->GetXaxis()->GetXmin(), histEtaFrac->GetMean() - histEtaFrac->GetRMS() * fitRangeJecsL5L7_ ), std::min( histEtaFrac->GetXaxis()->GetXmax(), histEtaFrac->GetMean() + histEtaFrac->GetRMS() * fitRangeJecsL5L7_ ) ) );
                my::setParametersBkg( fitEtaFrac, bkgEtaFrac );
                if ( overwrite_ ) bkgEtaFrac->Write( 0, TObject::kOverwrite );
                else              bkgEtaFrac->Write();
              }

              const std::string nameEtaFracRestr( nameEtaFrac + "Restr" );
              TH1D * histEtaFracRestr( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaFracRestr.c_str() ) ) );

              if ( histEtaFracRestr != 0 ) {
                const std::string nameEtaFracRestrFit( nameEtaFracRestr + "_fit" );
                TF1 * fitEtaFracRestr( new TF1( nameEtaFracRestrFit.c_str(), fitFunctionJecsL5L7_.c_str(), std::max( histEtaFracRestr->GetXaxis()->GetXmin(), histEtaFracRestr->GetMean() - histEtaFracRestr->GetRMS() * fitRangeJecsL5L7_ ), std::min( histEtaFracRestr->GetXaxis()->GetXmax(), histEtaFracRestr->GetMean() + histEtaFracRestr->GetRMS() * fitRangeJecsL5L7_ ) ) );
                my::setParametersFit( fitEtaFracRestr, histEtaFracRestr );
                TFitResultPtr fitEtaFracRestrResultPtr( histEtaFracRestr->Fit( fitEtaFracRestr, fitOptionsL5L7_.c_str() ) );
                if ( fitEtaFracRestrResultPtr >= 0 ) {
                  if ( fitEtaFracRestrResultPtr->Status() == 0 && fitEtaFracRestrResultPtr->Ndf() != 0. ) {
                    histFracRestrEtaFitMean->SetBinContent( uEta + 1, fitEtaFracRestrResultPtr->Parameter( 0 ) );
                    histFracRestrEtaFitMean->SetBinError( uEta + 1, fitEtaFracRestrResultPtr->ParError( 0 ) );
                  }
                  else {
                    if ( fitEtaFracRestrResultPtr->Prob() == 0. ) {
                    }
                    else {
                    }
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
                TF1 * bkgEtaFracRestr( new TF1( nameEtaFracRestrBkg.c_str(), bkgFunctionJecsL5L7_.c_str(), std::max( histEtaFracRestr->GetXaxis()->GetXmin(), histEtaFracRestr->GetMean() - histEtaFracRestr->GetRMS() * fitRangeJecsL5L7_ ), std::min( histEtaFracRestr->GetXaxis()->GetXmax(), histEtaFracRestr->GetMean() + histEtaFracRestr->GetRMS() * fitRangeJecsL5L7_ ) ) );
                my::setParametersBkg( fitEtaFracRestr, bkgEtaFracRestr );
                if ( overwrite_ ) bkgEtaFracRestr->Write( 0, TObject::kOverwrite );
                else              bkgEtaFracRestr->Write();
              }

//               const std::string nameFracPtMean( nameEta + "_FracPt_Mean" );
//               const std::string titleFracPtMean( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) );
//               TH1D * histFracPtMean( new TH1D( nameFracPtMean.c_str(), titleFracPtMean.c_str(), nPtBins_, ptBins_.data() ) );
//               histFracPtMean->SetXTitle( titlePt.c_str() );
//               histFracPtMean->SetYTitle( titleMean.c_str() );
//               const std::string nameFracRestrPtMean( nameEta + "_FracRestrPt_Mean" );
//               TH1D * histFracRestrPtMean( new TH1D( nameFracRestrPtMean.c_str(), titleFracPtMean.c_str(), nPtBins_, ptBins_.data() ) );
//               histFracRestrPtMean->SetXTitle( titlePt.c_str() );
//               histFracRestrPtMean->SetYTitle( titleMean.c_str() );
//               for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
//                 const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
//                 const std::string nameEtaPt( nameEta + "_Pt" + binPt );
//
//                 const std::string nameEtaPtFrac( nameEtaPt + "_Frac" );
//                 TH1D * histEtaPtFrac( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaPtFrac.c_str() ) ) );
//
//                 if ( histEtaPtFrac != 0 ) {
//                   const std::string nameEtaPtFracFit( nameEtaPtFrac + "_fit" );
//                   TF1 * fitEtaPtFrac( new TF1( nameEtaPtFracFit.c_str(), fitFunctionJecsL5L7_.c_str(), std::max( histEtaPtFrac->GetXaxis()->GetXmin(), histEtaPtFrac->GetMean() - histEtaPtFrac->GetRMS() * fitRangeJecsL5L7_ ), std::min( histEtaPtFrac->GetXaxis()->GetXmax(), histEtaPtFrac->GetMean() + histEtaPtFrac->GetRMS() * fitRangeJecsL5L7_ ) ) );
//                   my::setParametersFit( fitEtaPtFrac, histEtaPtFrac );
//                   TFitResultPtr fitEtaPtFracResultPtr( histEtaPtFrac->Fit( fitEtaPtFrac, fitOptionsL5L7_.c_str() ) );
//                   if ( fitEtaPtFracResultPtr >= 0 ) {
//                     if ( fitEtaPtFracResultPtr->Status() == 0 && fitEtaPtFracResultPtr->Ndf() != 0. ) {
//                       histFracPtMean->SetBinContent( uPt + 1, fitEtaPtFracResultPtr->Parameter( 0 ) );
//                       histFracPtMean->SetBinError( uPt + 1, fitEtaPtFracResultPtr->ParError( 0 ) );
//                       histFracEtaPtFitMeanMap->SetBinContent( uEta + 1, uPt + 1, fitEtaPtFracResultPtr->Parameter( 0 ) );
//                     }
//                     else {
//                       if ( fitEtaPtFracResultPtr->Prob() == 0. ) {
//                       }
//                       else {
//                       }
//                       if ( verbose_ > 2 ) {
//                         std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
//                                   << "    failing fit in directory '"; gDirectory->pwd();
//                         std::cout << "    '" << nameEtaPtFrac << "' status " << fitEtaPtFracResultPtr->Status() << std::endl;
//                       }
//                     }
//                   }
//                   else {
//                     if ( verbose_ > 1 ) {
//                       std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
//                                 << "    missing fit in directory '"; gDirectory->pwd();
//                       std::cout << "    '" << nameEtaPtFrac << std::endl;
//                     }
//                   }
//                   histFracEtaPtFitMean->Fill( fitEtaPtFracResultPtr->Parameter( 0 ) );
//                   const std::string nameEtaPtFracBkg( nameEtaPtFrac + "_bkg" );
//                   TF1 * bkgEtaPtFrac( new TF1( nameEtaPtFracBkg.c_str(), bkgFunctionJecsL5L7_.c_str(), std::max( histEtaPtFrac->GetXaxis()->GetXmin(), histEtaPtFrac->GetMean() - histEtaPtFrac->GetRMS() * fitRangeJecsL5L7_ ), std::min( histEtaPtFrac->GetXaxis()->GetXmax(), histEtaPtFrac->GetMean() + histEtaPtFrac->GetRMS() * fitRangeJecsL5L7_ ) ) );
//                   my::setParametersBkg( fitEtaPtFrac, bkgEtaPtFrac );
//                   if ( overwrite_ ) bkgEtaPtFrac->Write( 0, TObject::kOverwrite );
//                   else              bkgEtaPtFrac->Write();
//                 }
//
//                 const std::string nameEtaPtFracRestr( nameEtaPtFrac + "Restr" );
//                 TH1D * histEtaPtFracRestr( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaPtFracRestr.c_str() ) ) );
//
//                 if ( histEtaPtFracRestr != 0 ) {
//                   const std::string nameEtaPtFracRestrFit( nameEtaPtFracRestr + "_fit" );
//                   TF1 * fitEtaPtFracRestr( new TF1( nameEtaPtFracRestrFit.c_str(), fitFunctionJecsL5L7_.c_str(), std::max( histEtaPtFracRestr->GetXaxis()->GetXmin(), histEtaPtFracRestr->GetMean() - histEtaPtFracRestr->GetRMS() * fitRangeJecsL5L7_ ), std::min( histEtaPtFracRestr->GetXaxis()->GetXmax(), histEtaPtFracRestr->GetMean() + histEtaPtFracRestr->GetRMS() * fitRangeJecsL5L7_ ) ) );
//                   my::setParametersFit( fitEtaPtFracRestr, histEtaPtFracRestr );
//                   TFitResultPtr fitEtaPtFracRestrResultPtr( histEtaPtFracRestr->Fit( fitEtaPtFracRestr, fitOptionsL5L7_.c_str() ) );
//                   if ( fitEtaPtFracRestrResultPtr >= 0 ) {
//                     if ( fitEtaPtFracRestrResultPtr->Status() == 0 && fitEtaPtFracRestrResultPtr->Ndf() != 0. ) {
//                       histFracRestrPtMean->SetBinContent( uPt + 1, fitEtaPtFracRestrResultPtr->Parameter( 0 ) );
//                       histFracRestrPtMean->SetBinError( uPt + 1, fitEtaPtFracRestrResultPtr->ParError( 0 ) );
//                       histFracRestrEtaPtFitMeanMap->SetBinContent( uEta + 1, uPt + 1, fitEtaPtFracRestrResultPtr->Parameter( 0 ) );
//                       histFracRestrEtaPtFitMean->Fill( fitEtaPtFracRestrResultPtr->Parameter( 0 ) );
//                     }
//                     else {
//                       if ( fitEtaPtFracRestrResultPtr->Prob() == 0. ) {
//                       }
//                       else {
//                       }
//                       if ( verbose_ > 2 ) {
//                         std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
//                                   << "    failing fit in directory '"; gDirectory->pwd();
//                         std::cout << "    '" << nameEtaPtFracRestr << "' status " << fitEtaPtFracRestrResultPtr->Status() << std::endl;
//                       }
//                     }
//                   }
//                   else {
//                     if ( verbose_ > 1 ) {
//                       std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
//                                 << "    missing fit in directory '"; gDirectory->pwd();
//                       std::cout << "    '" << nameEtaPtFracRestr << std::endl;
//                     }
//                   }
//                   const std::string nameEtaPtFracRestrBkg( nameEtaPtFracRestr + "_bkg" );
//                   TF1 * bkgEtaPtFracRestr( new TF1( nameEtaPtFracRestrBkg.c_str(), bkgFunctionJecsL5L7_.c_str(), std::max( histEtaPtFracRestr->GetXaxis()->GetXmin(), histEtaPtFracRestr->GetMean() - histEtaPtFracRestr->GetRMS() * fitRangeJecsL5L7_ ), std::min( histEtaPtFracRestr->GetXaxis()->GetXmax(), histEtaPtFracRestr->GetMean() + histEtaPtFracRestr->GetRMS() * fitRangeJecsL5L7_ ) ) );
//                   my::setParametersBkg( fitEtaPtFracRestr, bkgEtaPtFracRestr );
//                   if ( overwrite_ ) bkgEtaPtFracRestr->Write( 0, TObject::kOverwrite );
//                   else              bkgEtaPtFracRestr->Write();
//                 }
//               } // loop: uPt < nPtBins_

            } // loop: keyEta

            if ( writeJecsL5L7Files_ ) {

              // File name
              std::string nameOut( pathOutJecsL5L7_ + "/gentL5L7_" + sample_ + "_" + name );
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

          ++uProp;

        } // loop: keyProp

      } // loop: uCat < objCats_.size()

    }

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
