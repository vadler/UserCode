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
  const std::string resolutionFile_( io_.getParameter< std::string >( "resolutionFile" ) );
  // Configuration for histogram binning
  const edm::ParameterSet & histos_( process_.getParameter< edm::ParameterSet >( "histos" ) );
  const double widthFactor_( histos_.getParameter< double >( "widthFactor" ) );
  // Configuration for fitting resolution functions
  const edm::ParameterSet & resFuncs_( process_.getParameter< edm::ParameterSet >( "resFuncs" ) );
  const bool fitResFuncs_( resFuncs_.getParameter< bool >( "fit" ) );
  const std::string fitFunctionResFuncs_( resFuncs_.getParameter< std::string >( "fitFunction" ) );
  const double fitRangeResFuncs_( resFuncs_.getParameter< double >( "fitRange" ) );
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
  const double fitRangeJecsL5L7_( jecsL5L7_.getParameter< double >( "fitRange" ) );
  const bool fitJecsL5L7Around1_( jecsL5L7_.getParameter< bool >( "fitAround1" ) );
  const bool writeJecsL5L7Files_( jecsL5L7_.getParameter< bool >( "writeFiles" ) && onlyExisting_ );
  const std::string pathOutJecsL5L7_( jecsL5L7_.getParameter< std::string >( "pathOut" ) );
  // Configuration for fitting 1D transfer functions
  const edm::ParameterSet & transfer1D_( process_.getParameter< edm::ParameterSet >( "transfer1D" ) );
  const bool fitTransfer1D_( transfer1D_.getParameter< bool >( "fit" ) );
  const double fitRangeTransfer1D_( transfer1D_.getParameter< double >( "fitRange" ) );
  const std::string transFunc1D_( transfer1D_.getParameter< std::string >( "transferFunction" ) );
  const bool writeTransferFiles1D_( transfer1D_.getParameter< bool >( "writeFiles" ) && onlyExisting_ );
  const std::string pathOutTransfer1D_( transfer1D_.getParameter< std::string >( "pathOut" ) );
  // Configuration for fitting 2D transfer functions
  const edm::ParameterSet & transfer2D_( process_.getParameter< edm::ParameterSet >( "transfer2D" ) );
  const bool fitTransfer2D_( transfer2D_.getParameter< bool >( "fit" ) );
  const std::string transFunc2D_( transfer2D_.getParameter< std::string >( "transferFunction" ) );
  const bool writeTransferFiles2D_( transfer2D_.getParameter< bool >( "writeFiles" ) && onlyExisting_ );
  const std::string pathOutTransfer2D_( transfer2D_.getParameter< std::string >( "pathOut" ) );

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
  if ( refSel_ ) evtSel_.append( "_Reference" );
  const std::string nameDirClass( "TDirectoryFile" );
  const std::string nameFuncClass( "TF1" );
  std::string optionsFit_( "QRS" );
  std::string optionsFitSigma_( "MRS" );
  if      ( verbose_ < 2 ) optionsFitSigma_.append( "Q" );
  else if ( verbose_ > 3 ) optionsFitSigma_.append( "V" );
  const std::string titleChi2( "#chi^{2} / ndf" );
  const std::string titleProb( "Probability" );
  const std::string titleNdf( "ndf" );
  const std::string titleEta( "#eta" );
  const std::string titlePt( "p_{t} (GeV)" );
  std::vector< std::string > titlesDelta;
  titlesDelta.push_back( "#Deltap_{t} (GeV)" );
  titlesDelta.push_back( "#Delta#eta" );
  titlesDelta.push_back( "#Delta#phi" );
  std::vector< std::string > titlesDeltaInv;
  titlesDeltaInv.push_back( "#Delta#frac{1}{p_{t}} (#frac{1}{GeV})" );
  titlesDeltaInv.push_back( "#Delta#eta" );
  titlesDeltaInv.push_back( "#Delta#phi" );
  std::vector< std::string > titlesFrac;
  titlesFrac.push_back( "#frac{p_{t}^{noRef}}{p_{t}}" );
  titlesFrac.push_back( "" );
  titlesFrac.push_back( "" );
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
  titlesFracAlt.push_back( "#frac{p_{t}^{noRef}}{p_{t}^{alt.}}" );
  titlesFracAlt.push_back( "" );
  titlesFracAlt.push_back( "" );
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
    Double_t ptData;
    Double_t ptGenData;
    Int_t    iEta;
    TTree * data_( dynamic_cast< TTree* >( dirCat_->Get( std::string( objCat + "_data" ).c_str() ) ) );
    if ( useAlt_ ) data_->SetBranchAddress( "PtAlt", &ptData );
    else           data_->SetBranchAddress( "Pt"   , &ptData );
    data_->SetBranchAddress( "PtGen", &ptGenData );
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

        const std::string nameDeltaRel( nameDelta + "Rel" );
        TH1D * histDeltaRel( new TH1D( nameDeltaRel.c_str(), objCat.c_str(), deltaBins, -1., 1. ) );
        histDeltaRel->SetXTitle( titleDeltaRel.c_str() );
        histDeltaRel->SetYTitle( titleEvents.c_str() );

        std::vector< TH1D * > histVecPtDelta;
        std::vector< TH1D * > histVecPtFrac;
        std::vector< TH1D * > histVecPtDeltaRel;
        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
          const std::string namePt( name + "_Pt" + binPt );
          const std::string namePtDelta( namePt + "_Delta" );
          const std::string titlePtDelta( objCat + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq #eta #leq " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
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

          const std::string nameEtaDeltaRel( nameEtaDelta + "Rel" );
          TH1D * histEtaDeltaRel( new TH1D( nameEtaDeltaRel.c_str(), titleEtaDelta.c_str(), deltaBins, -1., 1. ) );
          histEtaDeltaRel->SetXTitle( titleDeltaRel.c_str() );
          histEtaDeltaRel->SetYTitle( titleEvents.c_str() );

          // Split data into p_t bins
          dataCont weightEtaBin( nPtBins_ );
          dataCont ptEtaBin( nPtBins_ );
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
            const std::string titleEtaPtDelta( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq #eta #leq " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
            TH1D * histEtaPtDelta( new TH1D( nameEtaPtDelta.c_str(), titleEtaPtDelta.c_str(), deltaBins, -deltaMax, deltaMax ) );
            histEtaPtDelta->SetXTitle( titleDelta.c_str() );
            histEtaPtDelta->SetYTitle( titleEvents.c_str() );

            const std::string nameEtaPtFrac( nameEtaPt + "_Frac" );
            TH1D * histEtaPtFrac( new TH1D( nameEtaPtFrac.c_str(), titleEtaPtDelta.c_str(), deltaBins, 0., 2. ) );
            histEtaPtFrac->SetXTitle( titleFrac.c_str() );
            histEtaPtFrac->SetYTitle( titleEvents.c_str() );

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
                  histEtaPtDeltaRel->Fill( ( propGenEtaBin.at( uPt ).at( uEntry ) - propEtaBin.at( uPt ).at( uEntry ) ) / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                }
              }
              else {
                if ( inverse && kinProp == "Pt" ) histEtaPtDelta->Fill( 1. / propEtaBin.at( uPt ).at( uEntry ) - 1. / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                else                              histEtaPtDelta->Fill( propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                if ( ! inverse && kinProp == "Pt" ) {
                  histEtaPtFrac->Fill( propGenEtaBin.at( uPt ).at( uEntry ) / propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                  histEtaPtDeltaRel->Fill( ( propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ) ) / propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                }
              }
            } // loop: uEntry < ptEtaBin.at( uPt ).size()

            histDelta->Add( histEtaPtDelta );
            histVecPtDelta.at( uPt )->Add( histEtaPtDelta );
            histEtaDelta->Add( histEtaPtDelta );
            if ( ! inverse && kinProp == "Pt" ) {
              histFrac->Add( histEtaPtFrac );
              histEtaFrac->Add( histEtaPtFrac );
              histDeltaRel->Add( histEtaPtDeltaRel );
              histVecPtFrac.at( uPt )->Add( histEtaPtFrac );
              histVecPtDeltaRel.at( uPt )->Add( histEtaPtDeltaRel );
              histEtaDeltaRel->Add( histEtaPtDeltaRel );
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

            const std::string nameEtaPtFracRebin( nameEtaPtFrac + "Rebin" );
//             const Int_t fracBinsRebin( inverse ? propInvDeltaBins_ : propDeltaBins_ ); // FIXME: tune number of bins
            const Int_t fracBinsRebin( deltaBins );
            const Double_t meanEtaPtFrac( histEtaPtFrac->GetMean() );
            const Double_t widthEtaPtFrac( std::fabs( histEtaPtFrac->GetRMS() ) );
            if ( widthEtaPtFrac == 0. && verbose_ > 2 ) {
              std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                        << "    no histogram \"width\" in '" << nameEtaPtFrac << "'" << std::endl;
            }
            const Double_t rangeEtaPtFracRebin( widthEtaPtFrac == 0. ? widthFactor_ * std::fabs( histEtaPtFrac->GetXaxis()->GetXmax() ) : widthFactor_ * widthEtaPtFrac ); // FIXME: tune, incl. under- and overflow, remove hard-coding
            TH1D * histEtaPtFracRebin( new TH1D( nameEtaPtFracRebin.c_str(), titleEtaPtDelta.c_str(), fracBinsRebin, -rangeEtaPtFracRebin + meanEtaPtFrac, rangeEtaPtFracRebin + meanEtaPtFrac ) );
            histEtaPtFracRebin->SetXTitle( histEtaPtFrac->GetXaxis()->GetTitle() );
            histEtaPtFracRebin->SetYTitle( histEtaPtFrac->GetYaxis()->GetTitle() );

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
                  histEtaPtFracRebin->Fill( propEtaBin.at( uPt ).at( uEntry ) / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                  histEtaPtDeltaRelRebin->Fill( ( propGenEtaBin.at( uPt ).at( uEntry ) - propEtaBin.at( uPt ).at( uEntry ) ) / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                }
              }
              else {
                if ( inverse && kinProp == "Pt" ) histEtaPtDeltaRebin->Fill( 1. / propEtaBin.at( uPt ).at( uEntry ) - 1. / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                else                              histEtaPtDeltaRebin->Fill( propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                if ( ! inverse && kinProp == "Pt" ) {
                  histEtaPtFracRebin->Fill( propGenEtaBin.at( uPt ).at( uEntry ) / propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                  histEtaPtDeltaRelRebin->Fill( ( propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ) ) / propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                }
              }
            } // loop: uEntry < ptEtaBin.at( uPt ).size()
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
          TH1D * histEtaDeltaRebin( new TH1D( nameEtaDeltaRebin.c_str(), titleEtaDelta.c_str(), deltaBinsRebin, -rangeEtaDeltaRebin + meanEtaDelta, rangeEtaDeltaRebin + meanEtaDelta ) );
          histEtaDeltaRebin->SetXTitle( histEtaDelta->GetXaxis()->GetTitle() );
          histEtaDeltaRebin->SetYTitle( histEtaDelta->GetYaxis()->GetTitle() );

          const std::string nameEtaFracRebin( nameEtaFrac + "Rebin" );
//           const Int_t fracBinsRebin( inverse ? propInvDeltaBins_ : propDeltaBins_ ); // FIXME: tune number of bins
          const Int_t fracBinsRebin( deltaBins );
          const Double_t meanEtaFrac( histEtaFrac->GetMean() );
          const Double_t widthEtaFrac( std::fabs( histEtaFrac->GetRMS() ) );
          if ( widthEtaFrac == 0. && verbose_ > 2 ) {
            std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                      << "    no histogram \"width\" in '" << nameEtaFrac << "'" << std::endl;
          }
          const Double_t rangeEtaFracRebin( widthEtaFrac == 0. ? widthFactor_ * std::fabs( histEtaFrac->GetXaxis()->GetXmax() ) : widthFactor_ * widthEtaFrac ); // FIXME: tune, incl. under- and overflow, remove hard-coding
          TH1D * histEtaFracRebin( new TH1D( nameEtaFracRebin.c_str(), titleEtaDelta.c_str(), fracBinsRebin, -rangeEtaFracRebin + meanEtaFrac, rangeEtaFracRebin + meanEtaFrac ) );
          histEtaFracRebin->SetXTitle( histEtaFrac->GetXaxis()->GetTitle() );
          histEtaFracRebin->SetYTitle( histEtaFrac->GetYaxis()->GetTitle() );

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
                  histEtaFracRebin->Fill( propEtaBin.at( uPt ).at( uEntry ) / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                  histEtaDeltaRelRebin->Fill( ( propGenEtaBin.at( uPt ).at( uEntry ) - propEtaBin.at( uPt ).at( uEntry ) ) / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                }
              }
              else {
                if ( inverse && kinProp == "Pt" ) histEtaDeltaRebin->Fill( 1. / propEtaBin.at( uPt ).at( uEntry ) - 1. / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                else                              histEtaDeltaRebin->Fill( propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                if ( ! inverse && kinProp == "Pt" ) {
                  histEtaFracRebin->Fill( propGenEtaBin.at( uPt ).at( uEntry ) / propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                  histEtaDeltaRelRebin->Fill( ( propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ) ) / propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                }
              }
            } // loop: uEntry < ptEtaBin.at( uPt ).size()
          } // loop: uPt < nPtBins_

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
        TH1D * histDeltaRebin( new TH1D( nameDeltaRebin.c_str(), titleDelta.c_str(), deltaBinsRebin, -rangeDeltaRebin + meanDelta, rangeDeltaRebin + meanDelta ) );
        histDeltaRebin->SetXTitle( histDelta->GetXaxis()->GetTitle() );
        histDeltaRebin->SetYTitle( histDelta->GetYaxis()->GetTitle() );

        const std::string nameFracRebin( nameFrac + "Rebin" );
//         const Int_t fracBinsRebin( inverse ? propInvDeltaBins_ : propDeltaBins_ ); // FIXME: tune number of bins
        const Int_t fracBinsRebin( deltaBins );
        const Double_t meanFrac( histFrac->GetMean() );
        const Double_t widthFrac( std::fabs( histFrac->GetRMS() ) );
        if ( widthFrac == 0. && verbose_ > 2 ) {
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    no histogram \"width\" in '" << nameFrac << "'" << std::endl;
        }
        const Double_t rangeFracRebin( widthFrac == 0. ? widthFactor_ * std::fabs( histFrac->GetXaxis()->GetXmax() ) : widthFactor_ * widthFrac ); // FIXME: tune, incl. under- and overflow, remove hard-coding
        TH1D * histFracRebin( new TH1D( nameFracRebin.c_str(), titleDelta.c_str(), fracBinsRebin, -rangeFracRebin + meanFrac, rangeFracRebin + meanFrac ) );
        histFracRebin->SetXTitle( histFrac->GetXaxis()->GetTitle() );
        histFracRebin->SetYTitle( histFrac->GetYaxis()->GetTitle() );

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
        TH1D * histDeltaRelRebin( new TH1D( nameDeltaRelRebin.c_str(), titleDelta.c_str(), deltaBinsRebinRel, -rangeDeltaRelRebin + meanDeltaRel, rangeDeltaRelRebin + meanDeltaRel ) );
        histDeltaRelRebin->SetXTitle( histDeltaRel->GetXaxis()->GetTitle() );
        histDeltaRelRebin->SetYTitle( histDeltaRel->GetYaxis()->GetTitle() );

        std::vector< TH1D * > histVecPtDeltaRebin;
        std::vector< TH1D * > histVecPtFracRebin;
        std::vector< TH1D * > histVecPtDeltaRelRebin;
        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
          const std::string namePt( name + "_Pt" + binPt );

          const std::string namePtDelta( namePt + "_Delta" );
          const std::string titlePtDelta( objCat + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq #eta #leq " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
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
            const std::string namePtFrac( namePt + "_Frac" );
            const std::string namePtFracRebin( namePtFrac + "Rebin" );
//             const Int_t deltaBinsRebin( inverse ? propInvDeltaBins_ : propDeltaBins_ ); // FIXME: tune number of bins
            const Double_t meanPtFrac( histVecPtFrac.at( uPt )->GetMean() );
            const Double_t widthPtFrac( std::fabs( histVecPtFrac.at( uPt )->GetRMS() ) );
            if ( widthPtFrac == 0. && verbose_ > 2 ) {
              std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                        << "    no histogram \"width\" in '" << namePtFrac << "'" << std::endl;
            }
            const Double_t rangePtFracRebin( widthPtFrac == 0. ? widthFactor_ * std::fabs( histVecPtFrac.at( uPt )->GetXaxis()->GetXmax() ) : widthFactor_ * widthPtFrac ); // FIXME: tune, incl. under- and overflow, remove hard-coding
            TH1D * histPtFracRebin( new TH1D( namePtFracRebin.c_str(), titlePtDelta.c_str(), deltaBinsRebinRel, -rangePtFracRebin + meanPtFrac, rangePtFracRebin + meanPtFrac ) );
            histPtFracRebin->SetXTitle( titleDeltaRel.c_str() );
            histPtFracRebin->SetYTitle( titleEvents.c_str() );
            histVecPtFracRebin.push_back( histPtFracRebin );
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
                histFracRebin->Fill( propData_.at( uEta ).at( uEntry ) / propGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                histDeltaRelRebin->Fill( ( propGenData_.at( uEta ).at( uEntry ) - propData_.at( uEta ).at( uEntry ) ) / propGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
              }
            }
            else {
              if ( inverse && kinProp == "Pt" ) histDeltaRebin->Fill( 1. / propData_.at( uEta ).at( uEntry ) - 1. / propGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
              else                              histDeltaRebin->Fill( propData_.at( uEta ).at( uEntry ) - propGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
              if ( ! inverse && kinProp == "Pt" ) {
                histFracRebin->Fill( propGenData_.at( uEta ).at( uEntry ) / propData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                histDeltaRelRebin->Fill( ( propData_.at( uEta ).at( uEntry ) - propGenData_.at( uEta ).at( uEntry ) ) /propData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
              }
            }
            for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
              if ( ptBins_.at( uPt ) <= ptData_.at( uEta ).at( uEntry ) && ptData_.at( uEta ).at( uEntry ) < ptBins_.at( uPt + 1 ) ) {
                if ( refGen_ ) {
                  if ( inverse && kinProp == "Pt" ) histVecPtDeltaRebin.at( uPt )->Fill( 1. / propGenData_.at( uEta ).at( uEntry ) - 1. / propData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                  else                              histVecPtDeltaRebin.at( uPt )->Fill( propGenData_.at( uEta ).at( uEntry ) - propData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                  if ( ! inverse && kinProp == "Pt" ) {
                    histVecPtFracRebin.at( uPt )->Fill( propData_.at( uEta ).at( uEntry ) / propGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                    histVecPtDeltaRelRebin.at( uPt )->Fill( ( propGenData_.at( uEta ).at( uEntry ) - propData_.at( uEta ).at( uEntry ) ) / propGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                  }
                }
                else {
                  if ( inverse && kinProp == "Pt" ) histVecPtDeltaRebin.at( uPt )->Fill( 1. / propData_.at( uEta ).at( uEntry ) - 1. / propGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                  else                              histVecPtDeltaRebin.at( uPt )->Fill( propData_.at( uEta ).at( uEntry ) - propGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                  if ( ! inverse && kinProp == "Pt" ) {
                    histVecPtFracRebin.at( uPt )->Fill( propGenData_.at( uEta ).at( uEntry ) / propData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                    histVecPtDeltaRelRebin.at( uPt )->Fill( ( propData_.at( uEta ).at( uEntry ) - propGenData_.at( uEta ).at( uEntry ) ) / propData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                  }
                }
                break;
              }
            } // loop: uPt < nPtBins_
          } // loop: uEntry < sizeEta_.at( uEta )

        } // loop: keyEta

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
              TFitResultPtr fitEtaPtDeltaResultPtr( histEtaPtDeltaRebin->Fit( fitEtaPtDeltaRebin, optionsFit_.c_str() ) );
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
            TFitResultPtr fitEtaSigmaFitResultPtr( histSigmaEta->Fit( fitEtaSigmaFit, optionsFitSigma_.c_str() ) );
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
        std::string nameOut( pathOutResolution_ + "/gentResolution_Fall11_R4_" + objCat ); // FIXME: hard-coding
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

            // Fit performance histograms

            const std::string titleMean( "#mu of " + titleFrac );
            const std::string titleSigma( "#sigma of " + titleFrac );

            // Single JEC bins
            const std::string nameFracEtaPtFitMeanMap( name + "_FracEtaPt_FitMeanMap" );
            TH2D * histFracEtaPtFitMeanMap( new TH2D( nameFracEtaPtFitMeanMap.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
            histFracEtaPtFitMeanMap->SetXTitle( titleEta.c_str() );
            histFracEtaPtFitMeanMap->SetYTitle( titlePt.c_str() );
            const std::string nameFracEtaPtFitMean( name + "_FracEtaPt_FitMeanMap" );
            TH1D * histFracEtaPtFitMean( new TH1D( nameFracEtaPtFitMean.c_str(), titleProb.c_str(), 20, 0.75, 1.25 ) );
            histFracEtaPtFitMean->SetXTitle( titleMean.c_str() );
            const std::string nameFracEtaFitMean( name + "_FracEta_FitMeanMap" );
            TH1D * histFracEtaFitMean( new TH1D( nameFracEtaFitMean.c_str(), objCat.c_str(), nEtaBins_, etaBins_.data() ) );
            histFracEtaFitMean->SetXTitle( titleEta.c_str() );
            histFracEtaFitMean->SetYTitle( titleMean.c_str() );

            const std::string nameFrac( name + "_Frac" );
            const std::string nameFracRebin( nameFrac + "Rebin" );
            TH1D * histFracRebin( dynamic_cast< TH1D* >( gDirectory->Get( nameFracRebin.c_str() ) ) );

            const std::string nameFracFit( nameFrac + "_fit" );
            const std::string nameFracRebinFit( nameFracRebin + "_fit" );
            const double meanFracRebin( fitJecsL5L7Around1_ ? 1. : histFracRebin->GetMean() );
            TF1 * fitFracRebin( new TF1( nameFracRebinFit.c_str(), fitFunctionJecsL5L7_.c_str(), std::max( histFracRebin->GetXaxis()->GetXmin(), meanFracRebin - histFracRebin->GetRMS() * fitRangeJecsL5L7_ ), std::min( histFracRebin->GetXaxis()->GetXmax(), meanFracRebin + histFracRebin->GetRMS() * fitRangeJecsL5L7_ ) ) );
            TFitResultPtr fitFracResultPtr( histFracRebin->Fit( fitFracRebin, optionsFit_.c_str() ) );
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
                  std::cout << "    '" << nameFracRebin << "' status " << fitFracResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << nameFracRebin << std::endl;
              }
            }

            const std::string nameFracPtFitMean( name + "_FracPt_FitMean" );
            TH1D * histFracMean( new TH1D( nameFracPtFitMean.c_str(), objCat.c_str(), nPtBins_, ptBins_.data() ) );
            histFracMean->SetXTitle( titlePt.c_str() );
            histFracMean->SetYTitle( titleMean.c_str() );
            for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
              const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
              const std::string namePt( objCat + "_" + kinProp + "_" + subFit + "_Pt" + binPt );

              const std::string namePtFrac( namePt + "_Frac" );
              const std::string namePtFracRebin( namePtFrac + "Rebin" );
              TH1D * histPtFracRebin( dynamic_cast< TH1D* >( gDirectory->Get( namePtFracRebin.c_str() ) ) );

              const std::string namePtFracFit( namePtFrac + "_fit" );
              const std::string namePtFracRebinFit( namePtFracRebin + "_fit" );
              const double meanPtFracRebin( fitJecsL5L7Around1_ ? 1. : histPtFracRebin->GetMean() );
              TF1 * fitPtFracRebin( new TF1( namePtFracRebinFit.c_str(), fitFunctionJecsL5L7_.c_str(), std::max( histPtFracRebin->GetXaxis()->GetXmin(), meanPtFracRebin - histPtFracRebin->GetRMS() * fitRangeJecsL5L7_ ), std::min( histPtFracRebin->GetXaxis()->GetXmax(), meanPtFracRebin + histPtFracRebin->GetRMS() * fitRangeJecsL5L7_ ) ) );
              TFitResultPtr fitPtFracResultPtr( histPtFracRebin->Fit( fitPtFracRebin, optionsFit_.c_str() ) );
              if ( fitPtFracResultPtr >= 0 ) {
                if ( fitPtFracResultPtr->Status() == 0 && fitPtFracResultPtr->Ndf() != 0. ) {
                  histFracMean->SetBinContent( uPt + 1, fitPtFracResultPtr->Parameter( 1 ) );
                  histFracMean->SetBinError( uPt + 1, fitPtFracResultPtr->ParError( 1 ) );
                }
                else {
                  if ( fitPtFracResultPtr->Prob() == 0. ) {
                  }
                  else {
                  }
                  if ( verbose_ > 2 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    failing fit in directory '"; gDirectory->pwd();
                    std::cout << "    '" << namePtFracRebin << "' status " << fitPtFracResultPtr->Status() << std::endl;
                  }
                }
              }
              else {
                if ( verbose_ > 1 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    missing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << namePtFracRebin << std::endl;
                }
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
              const std::string nameEtaFracRebin( nameEtaFrac + "Rebin" );
              TH1D * histEtaFracRebin( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaFracRebin.c_str() ) ) );

              const std::string nameEtaFracFit( nameEtaFrac + "_fit" );
              const std::string nameEtaFracRebinFit( nameEtaFracRebin + "_fit" );
              const double meanEtaFracRebin( fitJecsL5L7Around1_ ? 1. : histEtaFracRebin->GetMean() );
              TF1 * fitEtaFracRebin( new TF1( nameEtaFracRebinFit.c_str(), fitFunctionJecsL5L7_.c_str(), std::max( histEtaFracRebin->GetXaxis()->GetXmin(), meanEtaFracRebin - histEtaFracRebin->GetRMS() * fitRangeJecsL5L7_ ), std::min( histEtaFracRebin->GetXaxis()->GetXmax(), meanEtaFracRebin + histEtaFracRebin->GetRMS() * fitRangeJecsL5L7_ ) ) );
              TFitResultPtr fitEtaFracResultPtr( histEtaFracRebin->Fit( fitEtaFracRebin, optionsFit_.c_str() ) );
              if ( fitEtaFracResultPtr >= 0 ) {
                if ( fitEtaFracResultPtr->Status() == 0 && fitEtaFracResultPtr->Ndf() != 0. ) {
                  histFracEtaFitMean->SetBinContent( uEta + 1, fitEtaFracResultPtr->Parameter( 1 ) );
                  histFracEtaFitMean->SetBinError( uEta + 1, fitEtaFracResultPtr->ParError( 1 ) );
                }
                else {
                  if ( fitEtaFracResultPtr->Prob() == 0. ) {
                  }
                  else {
                  }
                  if ( verbose_ > 2 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    failing fit in directory '"; gDirectory->pwd();
                    std::cout << "    '" << nameEtaFracRebin << "' status " << fitEtaFracResultPtr->Status() << std::endl;
                  }
                }
              }
              else {
                if ( verbose_ > 1 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    missing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << nameEtaFracRebin << std::endl;
                }
              }

              const std::string nameFracPtMean( nameEta + "_FracPt_Mean" );
              const std::string titleFracPtMean( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) );
              TH1D * histFracPtMean( new TH1D( nameFracPtMean.c_str(), titleFracPtMean.c_str(), nPtBins_, ptBins_.data() ) );
              histFracPtMean->SetXTitle( titlePt.c_str() );
              histFracPtMean->SetYTitle( titleMean.c_str() );
              for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
                const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
                const std::string nameEtaPt( objCat + "_" + kinProp + "_" + subFit + "_" + binEta + "_Pt" + binPt );

                const std::string nameEtaPtFrac( nameEtaPt + "_Frac" );
                const std::string nameEtaPtFracRebin( nameEtaPtFrac + "Rebin" );
                TH1D * histEtaPtFracRebin( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaPtFracRebin.c_str() ) ) );

                const std::string nameEtaPtFracFit( nameEtaPtFrac + "_fit" );
                const std::string nameEtaPtFracRebinFit( nameEtaPtFracRebin + "_fit" );
                const double meanEtaPtFracRebin( fitJecsL5L7Around1_ ? 1. : histEtaPtFracRebin->GetMean() );
                TF1 * fitEtaPtFracRebin( new TF1( nameEtaPtFracRebinFit.c_str(), fitFunctionJecsL5L7_.c_str(), std::max( histEtaPtFracRebin->GetXaxis()->GetXmin(), meanEtaPtFracRebin - histEtaPtFracRebin->GetRMS() * fitRangeJecsL5L7_ ), std::min( histEtaPtFracRebin->GetXaxis()->GetXmax(), meanEtaPtFracRebin + histEtaPtFracRebin->GetRMS() * fitRangeJecsL5L7_ ) ) );
                TFitResultPtr fitEtaPtFracResultPtr( histEtaPtFracRebin->Fit( fitEtaPtFracRebin, optionsFit_.c_str() ) );
                if ( fitEtaPtFracResultPtr >= 0 ) {
                  if ( fitEtaPtFracResultPtr->Status() == 0 && fitEtaPtFracResultPtr->Ndf() != 0. ) {
                    histFracPtMean->SetBinContent( uPt + 1, fitEtaPtFracResultPtr->Parameter( 1 ) );
                    histFracPtMean->SetBinError( uPt + 1, fitEtaPtFracResultPtr->ParError( 1 ) );
                    histFracEtaPtFitMeanMap->SetBinContent( uEta + 1, uPt + 1, fitEtaPtFracResultPtr->Parameter( 1 ) );
                    histFracEtaPtFitMean->Fill( fitEtaPtFracResultPtr->Parameter( 1 ) );
                  }
                  else {
                    if ( fitEtaPtFracResultPtr->Prob() == 0. ) {
                    }
                    else {
                    }
                    if ( verbose_ > 2 ) {
                      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                                << "    failing fit in directory '"; gDirectory->pwd();
                      std::cout << "    '" << nameEtaPtFracRebin << "' status " << fitEtaPtFracResultPtr->Status() << std::endl;
                    }
                  }
                }
                else {
                  if ( verbose_ > 1 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    missing fit in directory '"; gDirectory->pwd();
                    std::cout << "    '" << nameEtaPtFracRebin << std::endl;
                  }
                }
              } // loop: uPt < nPtBins_

            } // loop: keyEta

            if ( writeJecsL5L7Files_ ) {

              // File name
              std::string nameOut( pathOutJecsL5L7_ + "/gentsL5L7_Fall11_R4_" + name ); // FIXME: hard-coding
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


    // Fit 1D transfer functions

    if ( fitTransfer1D_ ) {
      if ( objCat == "UdscJet" || objCat == "BJet" ) {
        if ( verbose_ > 1 ) {
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    1D transfer function determination for " << objCat << " started" << std::endl;
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

            // Transfer function parameters
            std::vector< std::vector< double > > p1EtaPt, p2EtaPt, p3EtaPt, p4EtaPt, p5EtaPt;
            std::vector< double > p1Eta, p2Eta, p3Eta, p4Eta, p5Eta;
            std::vector< double > p1Pt, p2Pt, p3Pt, p4Pt, p5Pt;
            double p1, p2, p3, p4, p5;
            p1 = -999999.;
            p2 = -999999.;
            p3 = -999999.;
            p4 = -999999.;
            p5 = -999999.;

            std::string titleDelta( titlesDelta.at( uProp ) );
            std::string titleFrac( titlesFrac.at( uProp ) );
            std::string titleDeltaRel( titlesDeltaRel.at( uProp ) );
            if( useAlt_ ) {
              titleDelta    = titlesDeltaAlt.at( uProp );
              titleFrac     = titlesFracAlt.at( uProp );
              titleDeltaRel = titlesDeltaAltRel.at( uProp );
            }

            // Fit performance histograms

            const std::string titleMean( "#mu of " + titleFrac );
            const std::string titleSigma( "#sigma of " + titleFrac );

            const std::string nameDelta( name + "_Delta" );
            const std::string nameDeltaRebin( nameDelta + "Rebin" );
            TH1D * histDeltaRebin( dynamic_cast< TH1D* >( gDirectory->Get( nameDeltaRebin.c_str() ) ) );

            const std::string nameTrans( name + "_Trans" );
            const std::string nameTransRebin( nameTrans + "Rebin" );
            const std::string nameTransFit( nameTrans + "_fit" );
            const std::string nameTransRebinFit( nameTransRebin + "_fit" );
            TF1 * fitTransRebin( new TF1( nameTransRebinFit.c_str(), transFunc1D_.c_str(), std::max( histDeltaRebin->GetXaxis()->GetXmin(), histDeltaRebin->GetMean() - histDeltaRebin->GetRMS() * fitRangeTransfer1D_ ), std::min( histDeltaRebin->GetXaxis()->GetXmax(), histDeltaRebin->GetMean() + histDeltaRebin->GetRMS() * fitRangeTransfer1D_ ) ) );
            TFitResultPtr fitTransResultPtr( histDeltaRebin->Fit( fitTransRebin, optionsFit_.c_str() ) );
            if ( fitTransResultPtr >= 0 ) {
              if ( fitTransResultPtr->Status() == 0 && fitTransResultPtr->Ndf() != 0. ) {
                if ( writeTransferFiles1D_ ) {
                  p1 = fitTransRebin->GetParameter( 1 );
                  p2 = fitTransRebin->GetParameter( 2 );
                  p4 = fitTransRebin->GetParameter( 4 );
                  p5 = fitTransRebin->GetParameter( 5 );
                  if ( fitTransRebin->GetParameter( 0 ) != 0. )
                    p3 = fitTransRebin->GetParameter( 3 ) / fitTransRebin->GetParameter( 0 );
                }
              }
              else {
                if ( fitTransResultPtr->Prob() == 0. ) {
                }
                else {
                }
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << nameTransRebin << "' status " << fitTransResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << nameTransRebin << std::endl;
              }
            }

            for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
              const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
              const std::string namePt( objCat + "_" + kinProp + "_" + subFit + "_Pt" + binPt );

              p1Pt.push_back( -999999. );
              p2Pt.push_back( -999999. );
              p3Pt.push_back( -999999. );
              p4Pt.push_back( -999999. );
              p5Pt.push_back( -999999. );

              const std::string namePtDelta( namePt + "_Delta" );
              const std::string namePtDeltaRebin( namePtDelta + "Rebin" );
              TH1D * histPtDeltaRebin( dynamic_cast< TH1D* >( gDirectory->Get( namePtDeltaRebin.c_str() ) ) );

              const std::string namePtTrans( namePt + "_Trans" );
              const std::string namePtTransRebin( namePtTrans + "Rebin" );
              const std::string namePtTransFit( namePtTrans + "_fit" );
              const std::string namePtTransRebinFit( namePtTransRebin + "_fit" );
              TF1 * fitPtTransRebin( new TF1( namePtTransRebinFit.c_str(), transFunc1D_.c_str(), std::max( histPtDeltaRebin->GetXaxis()->GetXmin(), histPtDeltaRebin->GetMean() - histPtDeltaRebin->GetRMS() * fitRangeTransfer1D_ ), std::min( histPtDeltaRebin->GetXaxis()->GetXmax(), histPtDeltaRebin->GetMean() + histPtDeltaRebin->GetRMS() * fitRangeTransfer1D_ ) ) );
              TFitResultPtr fitPtTransResultPtr( histPtDeltaRebin->Fit( fitPtTransRebin, optionsFit_.c_str() ) );
              if ( fitPtTransResultPtr >= 0 ) {
                if ( fitPtTransResultPtr->Status() == 0 && fitPtTransResultPtr->Ndf() != 0. ) {
                  if ( writeTransferFiles1D_ ) {
                    p1Pt.at( uPt ) = fitPtTransRebin->GetParameter( 1 );
                    p2Pt.at( uPt ) = fitPtTransRebin->GetParameter( 2 );
                    p4Pt.at( uPt ) = fitPtTransRebin->GetParameter( 4 );
                    p5Pt.at( uPt ) = fitPtTransRebin->GetParameter( 5 );
                    if ( fitPtTransRebin->GetParameter( 0 ) != 0. )
                      p3Pt.at( uPt ) = fitPtTransRebin->GetParameter( 3 ) / fitPtTransRebin->GetParameter( 0 );
                  }
                }
                else {
                  if ( fitPtTransResultPtr->Prob() == 0. ) {
                  }
                  else {
                  }
                  if ( verbose_ > 2 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    failing fit in directory '"; gDirectory->pwd();
                    std::cout << "    '" << namePtTransRebin << "' status " << fitPtTransResultPtr->Status() << std::endl;
                  }
                }
              }
              else {
                if ( verbose_ > 1 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    missing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << namePtTransRebin << std::endl;
                }
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

              p1Eta.push_back( -999999. );
              p2Eta.push_back( -999999. );
              p3Eta.push_back( -999999. );
              p4Eta.push_back( -999999. );
              p5Eta.push_back( -999999. );

              const std::string nameEta( name + "_" + binEta );

              const std::string nameEtaDelta( nameEta + "_Delta" );
              const std::string nameEtaDeltaRebin( nameEtaDelta + "Rebin" );
              TH1D * histEtaDeltaRebin( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaDeltaRebin.c_str() ) ) );

              const std::string nameEtaTrans( nameEta + "_Trans" );
              const std::string nameEtaTransRebin( nameEtaTrans + "Rebin" );
              const std::string nameEtaTransFit( nameEtaTrans + "_fit" );
              const std::string nameEtaTransRebinFit( nameEtaTransRebin + "_fit" );
              TF1 * fitEtaTransRebin( new TF1( nameEtaTransRebinFit.c_str(), transFunc1D_.c_str(), std::max( histEtaDeltaRebin->GetXaxis()->GetXmin(), histEtaDeltaRebin->GetMean() - histEtaDeltaRebin->GetRMS() * fitRangeTransfer1D_ ), std::min( histEtaDeltaRebin->GetXaxis()->GetXmax(), histEtaDeltaRebin->GetMean() + histEtaDeltaRebin->GetRMS() * fitRangeTransfer1D_ ) ) );
              TFitResultPtr fitEtaTransResultPtr( histEtaDeltaRebin->Fit( fitEtaTransRebin, optionsFit_.c_str() ) );
              if ( fitEtaTransResultPtr >= 0 ) {
                if ( fitEtaTransResultPtr->Status() == 0 && fitEtaTransResultPtr->Ndf() != 0. ) {
                  if ( writeTransferFiles1D_ ) {
                    p1Eta.at( uEta ) = fitEtaTransRebin->GetParameter( 1 );
                    p2Eta.at( uEta ) = fitEtaTransRebin->GetParameter( 2 );
                    p4Eta.at( uEta ) = fitEtaTransRebin->GetParameter( 4 );
                    p5Eta.at( uEta ) = fitEtaTransRebin->GetParameter( 5 );
                    if ( fitEtaTransRebin->GetParameter( 0 ) != 0. )
                      p3Eta.at( uEta ) = fitEtaTransRebin->GetParameter( 3 ) / fitEtaTransRebin->GetParameter( 0 );
                  }
                }
                else {
                  if ( fitEtaTransResultPtr->Prob() == 0. ) {
                  }
                  else {
                  }
                  if ( verbose_ > 2 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    failing fit in directory '"; gDirectory->pwd();
                    std::cout << "    '" << nameEtaTransRebin << "' status " << fitEtaTransResultPtr->Status() << std::endl;
                  }
                }
              }
              else {
                if ( verbose_ > 1 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    missing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << nameEtaTransRebin << std::endl;
                }
              }

              p1EtaPt.push_back( std::vector< double >( nPtBins_ ) );
              p2EtaPt.push_back( std::vector< double >( nPtBins_ ) );
              p3EtaPt.push_back( std::vector< double >( nPtBins_ ) );
              p4EtaPt.push_back( std::vector< double >( nPtBins_ ) );
              p5EtaPt.push_back( std::vector< double >( nPtBins_ ) );

              for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
                const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
                const std::string nameEtaPt( objCat + "_" + kinProp + "_" + subFit + "_" + binEta + "_Pt" + binPt );

                p1EtaPt.at( uEta ).at( uPt ) = -999999.;
                p2EtaPt.at( uEta ).at( uPt ) = -999999.;
                p3EtaPt.at( uEta ).at( uPt ) = -999999.;
                p4EtaPt.at( uEta ).at( uPt ) = -999999.;
                p5EtaPt.at( uEta ).at( uPt ) = -999999.;

                const std::string nameEtaPtDelta( nameEtaPt + "_Delta" );
                const std::string nameEtaPtDeltaRebin( nameEtaPtDelta + "Rebin" );
                TH1D * histEtaPtDeltaRebin( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaPtDeltaRebin.c_str() ) ) );

                const std::string nameEtaPtTrans( nameEtaPt + "_Trans" );
                const std::string nameEtaPtTransRebin( nameEtaPtTrans + "Rebin" );
                const std::string nameEtaPtTransFit( nameEtaPtTrans + "_fit" );
                const std::string nameEtaPtTransRebinFit( nameEtaPtTransRebin + "_fit" );
                TF1 * fitEtaPtTransRebin( new TF1( nameEtaPtTransRebinFit.c_str(), transFunc1D_.c_str(), std::max( histEtaPtDeltaRebin->GetXaxis()->GetXmin(), histEtaPtDeltaRebin->GetMean() - histEtaPtDeltaRebin->GetRMS() * fitRangeTransfer1D_ ), std::min( histEtaPtDeltaRebin->GetXaxis()->GetXmax(), histEtaPtDeltaRebin->GetMean() + histEtaPtDeltaRebin->GetRMS() * fitRangeTransfer1D_ ) ) );
                TFitResultPtr fitEtaPtTransResultPtr( histEtaPtDeltaRebin->Fit( fitEtaPtTransRebin, optionsFit_.c_str() ) );
                if ( fitEtaPtTransResultPtr >= 0 ) {
                  if ( fitEtaPtTransResultPtr->Status() == 0 && fitEtaPtTransResultPtr->Ndf() != 0. ) {
                    if ( writeTransferFiles1D_ ) {
                      p1EtaPt.at( uEta ).at( uPt ) = fitEtaPtTransRebin->GetParameter( 1 );
                      p2EtaPt.at( uEta ).at( uPt ) = fitEtaPtTransRebin->GetParameter( 2 );
                      p4EtaPt.at( uEta ).at( uPt ) = fitEtaPtTransRebin->GetParameter( 4 );
                      p5EtaPt.at( uEta ).at( uPt ) = fitEtaPtTransRebin->GetParameter( 5 );
                      if ( fitEtaPtTransRebin->GetParameter( 0 ) != 0. )
                        p3EtaPt.at( uEta ).at( uPt ) = fitEtaPtTransRebin->GetParameter( 3 ) / fitEtaPtTransRebin->GetParameter( 0 );
                    }
                  }
                  else {
                    if ( fitEtaPtTransResultPtr->Prob() == 0. ) {
                    }
                    else {
                    }
                    if ( verbose_ > 2 ) {
                      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                                << "    failing fit in directory '"; gDirectory->pwd();
                      std::cout << "    '" << nameEtaPtTransRebin << "' status " << fitEtaPtTransResultPtr->Status() << std::endl;
                    }
                  }
                }
                else {
                  if ( verbose_ > 1 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    missing fit in directory '"; gDirectory->pwd();
                    std::cout << "    '" << nameEtaPtTransRebin << std::endl;
                  }
                }
              } // loop: uPt < nPtBins_

            } // loop: keyEta

            if ( writeTransferFiles1D_ ) {

              // File name
              std::string nameOut( pathOutTransfer1D_ + "/gentTransferFunction1D_Fall11_R4_" + name ); // FIXME: hard-coding
              if ( usePileUp_ ) nameOut.append( "_PileUp" );
              if ( refSel_)     nameOut.append( "_Ref" );
              nameOut.append( ".txt" );

              ofstream fileOut;
              fileOut.open( nameOut.c_str(), std::ios_base::out );

              fileOut << std::endl << "nodep_trans = ";
              fileOut << std::setprecision( 4 );
              if ( p1 == -999999. ) fileOut << "NAN";
              else                  fileOut << p1;
              fileOut << ", ";
              if ( p2 == -999999. ) fileOut << "NAN";
              else                  fileOut << p2;
              fileOut << ", ";
              if ( p3 == -999999. ) fileOut << "NAN";
              else                  fileOut << p3;
              fileOut << ", ";
              if ( p4 == -999999. ) fileOut << "NAN";
              else                  fileOut << p4;
              fileOut << ", ";
              if ( p5 == -999999. ) fileOut << "NAN";
              else                  fileOut << p5;
              fileOut << std::endl << std::endl;

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
                  fileOut << std::endl << "etadep_trans";
                  fileOut.width( 3 );
                  fileOut << std::left << nEta << "= ";
                  fileOut << std::setprecision( 4 );
                  if ( p1Eta.at( uEta - 1 ) == -999999. ) fileOut << "NAN";
                  else                                    fileOut << p1Eta.at( uEta - 1 );
                  fileOut << ", ";
                  if ( p2Eta.at( uEta - 1 ) == -999999. ) fileOut << "NAN";
                  else                                    fileOut << p2Eta.at( uEta - 1 );
                  fileOut << ", ";
                  if ( p3Eta.at( uEta - 1 ) == -999999. ) fileOut << "NAN";
                  else                                    fileOut << p3Eta.at( uEta - 1 );
                  fileOut << ", ";
                  if ( p4Eta.at( uEta - 1 ) == -999999. ) fileOut << "NAN";
                  else                                    fileOut << p4Eta.at( uEta - 1 );
                  fileOut << ", ";
                  if ( p5Eta.at( uEta - 1 ) == -999999. ) fileOut << "NAN";
                  else                                    fileOut << p5Eta.at( uEta - 1 );
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
                fileOut << std::endl << "etadep_trans";
                fileOut.width( 3 );
                fileOut << std::left << nEta << "= ";
                fileOut << std::setprecision( 4 );
                if ( p1Eta.at( uEta ) == -999999. ) fileOut << "NAN";
                else                                fileOut << p1Eta.at( uEta );
                fileOut << ", ";
                if ( p2Eta.at( uEta ) == -999999. ) fileOut << "NAN";
                else                                fileOut << p2Eta.at( uEta );
                fileOut << ", ";
                if ( p3Eta.at( uEta ) == -999999. ) fileOut << "NAN";
                else                                fileOut << p3Eta.at( uEta );
                fileOut << ", ";
                if ( p4Eta.at( uEta ) == -999999. ) fileOut << "NAN";
                else                                fileOut << p4Eta.at( uEta );
                fileOut << ", ";
                if ( p5Eta.at( uEta ) == -999999. ) fileOut << "NAN";
                else                                fileOut << p5Eta.at( uEta );
                fileOut << std::endl;
                ++nEta;
              }
              fileOut << std::endl;

              for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
                fileOut << std::endl << "ptdep_ptmin";
                fileOut.width( 3 );
                fileOut << std::left << uPt << "= ";
                fileOut << std::fixed << std::setprecision( 2 ) << ptBins_.at( uPt );
                fileOut << std::endl << "ptdep_ptmax";
                fileOut.width( 3 );
                fileOut << std::left << uPt << "= ";
                fileOut << std::fixed << std::setprecision( 2 ) << ptBins_.at( uPt + 1 );
                fileOut << std::endl << "ptdep_trans";
                fileOut.width( 3 );
                fileOut << std::left << uPt << "= ";
                fileOut << std::setprecision( 4 );
                if ( p1Pt.at( uPt ) == -999999. ) fileOut << "NAN";
                else                              fileOut << p1Pt.at( uPt );
                fileOut << ", ";
                if ( p2Pt.at( uPt ) == -999999. ) fileOut << "NAN";
                else                              fileOut << p2Pt.at( uPt );
                fileOut << ", ";
                if ( p3Pt.at( uPt ) == -999999. ) fileOut << "NAN";
                else                              fileOut << p3Pt.at( uPt );
                fileOut << ", ";
                if ( p4Pt.at( uPt ) == -999999. ) fileOut << "NAN";
                else                              fileOut << p4Pt.at( uPt );
                fileOut << ", ";
                if ( p5Pt.at( uPt ) == -999999. ) fileOut << "NAN";
                else                              fileOut << p5Pt.at( uPt );
                fileOut << std::endl;
              }
              fileOut << std::endl;

              unsigned nBin( 0 );
              for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
                nEta = 0;
                if ( useSymm_ ) {
                  for ( unsigned uEta = nEtaBins_; uEta > 0; --uEta ) {
                    fileOut << std::endl << "etaptdep_etamin";
                    fileOut.width( 3 );
                    fileOut << std::left << nEta << "= ";
                    fileOut << std::fixed << std::setprecision( 2 ) << -etaBins_.at( uEta );
                    fileOut << std::endl << "etaptdep_etamax";
                    fileOut.width( 3 );
                    fileOut << std::left << nEta << "= ";
                    fileOut << std::fixed << std::setprecision( 2 ) << -etaBins_.at( uEta - 1 );
                    fileOut << std::endl << "etaptdep_ptmin";
                    fileOut.width( 3 );
                    fileOut << std::left << uPt << "= ";
                    fileOut << std::fixed << std::setprecision( 2 ) << ptBins_.at( uPt );
                    fileOut << std::endl << "etaptdep_ptmax";
                    fileOut.width( 3 );
                    fileOut << std::left << uPt << "= ";
                    fileOut << std::fixed << std::setprecision( 2 ) << ptBins_.at( uPt + 1 );
                    fileOut << std::endl << "etaptdep_trans";
                    fileOut.width( 3 );
                    fileOut << std::left << nBin << "= ";
                    fileOut << std::setprecision( 4 );
                    if ( p1EtaPt.at( uEta - 1 ).at( uPt ) == -999999. ) fileOut << "NAN";
                    else                                                fileOut << p1EtaPt.at( uEta - 1 ).at( uPt );
                    fileOut << ", ";
                    if ( p2EtaPt.at( uEta - 1 ).at( uPt ) == -999999. ) fileOut << "NAN";
                    else                                                fileOut << p2EtaPt.at( uEta - 1 ).at( uPt );
                    fileOut << ", ";
                    if ( p3EtaPt.at( uEta - 1 ).at( uPt ) == -999999. ) fileOut << "NAN";
                    else                                                fileOut << p3EtaPt.at( uEta - 1 ).at( uPt );
                    fileOut << ", ";
                    if ( p4EtaPt.at( uEta - 1 ).at( uPt ) == -999999. ) fileOut << "NAN";
                    else                                                fileOut << p4EtaPt.at( uEta - 1 ).at( uPt );
                    fileOut << ", ";
                    if ( p5EtaPt.at( uEta - 1 ).at( uPt ) == -999999. ) fileOut << "NAN";
                    else                                                fileOut << p5EtaPt.at( uEta - 1 ).at( uPt );
                    fileOut << std::endl;
                    ++nEta;
                    ++nBin;
                  }
                }
                for ( unsigned uEta = 0; uEta < nEtaBins_; ++uEta ) {
                  fileOut << std::endl << "etaptdep_etamin";
                  fileOut.width( 3 );
                  fileOut << std::left << nEta << "= ";
                  fileOut << std::fixed << std::setprecision( 2 ) << etaBins_.at( uEta );
                  fileOut << std::endl << "etaptdep_etamax";
                  fileOut.width( 3 );
                  fileOut << std::left << nEta << "= ";
                  fileOut << std::fixed << std::setprecision( 2 ) << etaBins_.at( uEta + 1 );
                  fileOut << std::endl << "etaptdep_ptmin";
                  fileOut.width( 3 );
                  fileOut << std::left << uPt << "= ";
                  fileOut << std::fixed << std::setprecision( 2 ) << ptBins_.at( uPt );
                  fileOut << std::endl << "etaptdep_ptmax";
                  fileOut.width( 3 );
                  fileOut << std::left << uPt << "= ";
                  fileOut << std::fixed << std::setprecision( 2 ) << ptBins_.at( uPt + 1 );
                  fileOut << std::endl << "etaptdep_trans";
                  fileOut.width( 3 );
                  fileOut << std::left << nBin << "= ";
                  fileOut << std::setprecision( 4 );
                  if ( p1EtaPt.at( uEta ).at( uPt ) == -999999. ) fileOut << "NAN";
                  else                                            fileOut << p1EtaPt.at( uEta ).at( uPt );
                  fileOut << ", ";
                  if ( p2EtaPt.at( uEta ).at( uPt ) == -999999. ) fileOut << "NAN";
                  else                                            fileOut << p2EtaPt.at( uEta ).at( uPt );
                  fileOut << ", ";
                  if ( p3EtaPt.at( uEta ).at( uPt ) == -999999. ) fileOut << "NAN";
                  else                                            fileOut << p3EtaPt.at( uEta ).at( uPt );
                  fileOut << ", ";
                  if ( p4EtaPt.at( uEta ).at( uPt ) == -999999. ) fileOut << "NAN";
                  else                                            fileOut << p4EtaPt.at( uEta ).at( uPt );
                  fileOut << ", ";
                  if ( p5EtaPt.at( uEta ).at( uPt ) == -999999. ) fileOut << "NAN";
                  else                                            fileOut << p5EtaPt.at( uEta ).at( uPt );
                  fileOut << std::endl;
                  ++nEta;
                  ++nBin;
                }
                fileOut << std::endl;
              }
              fileOut << std::endl;

              fileOut.close();
              std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                        << "    written 1D transfer function file:" << std::endl
                        << "        " << nameOut << std::endl;

            }

          } // loop: keyFit

          ++uProp;

        } // loop: keyProp

      } // loop: uCat < objCats_.size()

    }


    // Fit 2D transfer functions

    if ( fitTransfer2D_ ) {
      if ( objCat == "UdscJet" || objCat == "BJet" ) {
        if ( verbose_ > 1 ) {
          std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                    << "    1D transfer function determination for " << objCat << " started" << std::endl;
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

            // Transfer function parameters
            std::vector< std::vector< double > > a1EtaPt, a2EtaPt, a3EtaPt, a4EtaPt, a5EtaPt;
            std::vector< double > a1Eta, a2Eta, a3Eta, a4Eta, a5Eta;
            std::vector< double > a1Pt, a2Pt, a3Pt, a4Pt, a5Pt;
            double a1, a2, a3, a4, a5;
            std::vector< std::vector< double > > b1EtaPt, b2EtaPt, b3EtaPt, b4EtaPt, b5EtaPt;
            std::vector< double > b1Eta, b2Eta, b3Eta, b4Eta, b5Eta;
            std::vector< double > b1Pt, b2Pt, b3Pt, b4Pt, b5Pt;
            double b1, b2, b3, b4, b5;
            a1 = -999999.;
            a2 = -999999.;
            a3 = -999999.;
            a4 = -999999.;
            a5 = -999999.;
            b1 = -999999.;
            b2 = -999999.;
            b3 = -999999.;
            b4 = -999999.;
            b5 = -999999.;

            for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
              const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
              const std::string namePt( objCat + "_" + kinProp + "_" + subFit + "_Pt" + binPt );

              a1Pt.push_back( -999999. );
              a2Pt.push_back( -999999. );
              a3Pt.push_back( -999999. );
              a4Pt.push_back( -999999. );
              a5Pt.push_back( -999999. );
              b1Pt.push_back( -999999. );
              b2Pt.push_back( -999999. );
              b3Pt.push_back( -999999. );
              b4Pt.push_back( -999999. );
              b5Pt.push_back( -999999. );
            } // loop: uPt < nPtBins_

            // Loop over eta bins
            TList * listFit( dirFit_->GetListOfKeys() );
            TIter nextInListFit( listFit );
            while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
              if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
              const std::string binEta( keyEta->GetName() );
              const unsigned uEta( std::atoi( binEta.substr( 3 ).data() ) );
              dirFit_->cd( binEta.c_str() );

              a1Eta.push_back( -999999. );
              a2Eta.push_back( -999999. );
              a3Eta.push_back( -999999. );
              a4Eta.push_back( -999999. );
              a5Eta.push_back( -999999. );
              b1Eta.push_back( -999999. );
              b2Eta.push_back( -999999. );
              b3Eta.push_back( -999999. );
              b4Eta.push_back( -999999. );
              b5Eta.push_back( -999999. );

              const std::string nameEta( name + "_" + binEta );

              a1EtaPt.push_back( std::vector< double >( nPtBins_ ) );
              a2EtaPt.push_back( std::vector< double >( nPtBins_ ) );
              a3EtaPt.push_back( std::vector< double >( nPtBins_ ) );
              a4EtaPt.push_back( std::vector< double >( nPtBins_ ) );
              a5EtaPt.push_back( std::vector< double >( nPtBins_ ) );
              b1EtaPt.push_back( std::vector< double >( nPtBins_ ) );
              b2EtaPt.push_back( std::vector< double >( nPtBins_ ) );
              b3EtaPt.push_back( std::vector< double >( nPtBins_ ) );
              b4EtaPt.push_back( std::vector< double >( nPtBins_ ) );
              b5EtaPt.push_back( std::vector< double >( nPtBins_ ) );

              for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
                const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
                const std::string nameEtaPt( objCat + "_" + kinProp + "_" + subFit + "_" + binEta + "_Pt" + binPt );

                a1EtaPt.at( uEta ).at( uPt ) = -999999.;
                a2EtaPt.at( uEta ).at( uPt ) = -999999.;
                a3EtaPt.at( uEta ).at( uPt ) = -999999.;
                a4EtaPt.at( uEta ).at( uPt ) = -999999.;
                a5EtaPt.at( uEta ).at( uPt ) = -999999.;
                b1EtaPt.at( uEta ).at( uPt ) = -999999.;
                b2EtaPt.at( uEta ).at( uPt ) = -999999.;
                b3EtaPt.at( uEta ).at( uPt ) = -999999.;
                b4EtaPt.at( uEta ).at( uPt ) = -999999.;
                b5EtaPt.at( uEta ).at( uPt ) = -999999.;
              } // loop: uPt < nPtBins_

            } // loop: keyEta

            if ( writeTransferFiles2D_ ) {

              // File name
              std::string nameOut( pathOutTransfer2D_ + "/gentTransferFunction2D_Fall11_R4_" + name ); // FIXME: hard-coding
              if ( usePileUp_ ) nameOut.append( "_PileUp" );
              if ( refSel_)     nameOut.append( "_Ref" );
              nameOut.append( ".txt" );

              ofstream fileOut;
              fileOut.open( nameOut.c_str(), std::ios_base::out );

              fileOut << std::endl << "nodep_trans_a = ";
              fileOut << std::setprecision( 4 );
              if ( a1 == -999999. ) fileOut << "NAN";
              else                  fileOut << a1;
              fileOut << ", ";
              if ( a2 == -999999. ) fileOut << "NAN";
              else                  fileOut << a2;
              fileOut << ", ";
              if ( a3 == -999999. ) fileOut << "NAN";
              else                  fileOut << a3;
              fileOut << ", ";
              if ( a4 == -999999. ) fileOut << "NAN";
              else                  fileOut << a4;
              fileOut << ", ";
              if ( a5 == -999999. ) fileOut << "NAN";
              else                  fileOut << a5;
              fileOut << std::endl << "nodep_trans_b = ";
              fileOut << std::setprecision( 4 );
              if ( b1 == -999999. ) fileOut << "NAN";
              else                  fileOut << b1;
              fileOut << ", ";
              if ( b2 == -999999. ) fileOut << "NAN";
              else                  fileOut << b2;
              fileOut << ", ";
              if ( b3 == -999999. ) fileOut << "NAN";
              else                  fileOut << b3;
              fileOut << ", ";
              if ( b4 == -999999. ) fileOut << "NAN";
              else                  fileOut << b4;
              fileOut << ", ";
              if ( b5 == -999999. ) fileOut << "NAN";
              else                  fileOut << b5;
              fileOut << std::endl << std::endl;

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
                  fileOut << std::endl << "etadep_trans_a";
                  fileOut.width( 3 );
                  fileOut << std::left << nEta << "= ";
                  fileOut << std::setprecision( 4 );
                  if ( a1Eta.at( uEta - 1 ) == -999999. ) fileOut << "NAN";
                  else                                    fileOut << a1Eta.at( uEta - 1 );
                  fileOut << ", ";
                  if ( a2Eta.at( uEta - 1 ) == -999999. ) fileOut << "NAN";
                  else                                    fileOut << a2Eta.at( uEta - 1 );
                  fileOut << ", ";
                  if ( a3Eta.at( uEta - 1 ) == -999999. ) fileOut << "NAN";
                  else                                    fileOut << a3Eta.at( uEta - 1 );
                  fileOut << ", ";
                  if ( a4Eta.at( uEta - 1 ) == -999999. ) fileOut << "NAN";
                  else                                    fileOut << a4Eta.at( uEta - 1 );
                  fileOut << ", ";
                  if ( a5Eta.at( uEta - 1 ) == -999999. ) fileOut << "NAN";
                  else                                    fileOut << a5Eta.at( uEta - 1 );
                  fileOut << std::endl << "etadep_trans_b";
                  fileOut.width( 3 );
                  fileOut << std::left << nEta << "= ";
                  fileOut << std::setprecision( 4 );
                  if ( b1Eta.at( uEta - 1 ) == -999999. ) fileOut << "NAN";
                  else                                    fileOut << b1Eta.at( uEta - 1 );
                  fileOut << ", ";
                  if ( b2Eta.at( uEta - 1 ) == -999999. ) fileOut << "NAN";
                  else                                    fileOut << b2Eta.at( uEta - 1 );
                  fileOut << ", ";
                  if ( b3Eta.at( uEta - 1 ) == -999999. ) fileOut << "NAN";
                  else                                    fileOut << b3Eta.at( uEta - 1 );
                  fileOut << ", ";
                  if ( b4Eta.at( uEta - 1 ) == -999999. ) fileOut << "NAN";
                  else                                    fileOut << b4Eta.at( uEta - 1 );
                  fileOut << ", ";
                  if ( b5Eta.at( uEta - 1 ) == -999999. ) fileOut << "NAN";
                  else                                    fileOut << b5Eta.at( uEta - 1 );
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
                fileOut << std::endl << "etadep_trans_a";
                fileOut.width( 3 );
                fileOut << std::left << nEta << "= ";
                fileOut << std::setprecision( 4 );
                if ( a1Eta.at( uEta ) == -999999. ) fileOut << "NAN";
                else                                fileOut << a1Eta.at( uEta );
                fileOut << ", ";
                if ( a2Eta.at( uEta ) == -999999. ) fileOut << "NAN";
                else                                fileOut << a2Eta.at( uEta );
                fileOut << ", ";
                if ( a3Eta.at( uEta ) == -999999. ) fileOut << "NAN";
                else                                fileOut << a3Eta.at( uEta );
                fileOut << ", ";
                if ( a4Eta.at( uEta ) == -999999. ) fileOut << "NAN";
                else                                fileOut << a4Eta.at( uEta );
                fileOut << ", ";
                if ( a5Eta.at( uEta ) == -999999. ) fileOut << "NAN";
                else                                fileOut << a5Eta.at( uEta );
                fileOut << std::endl << "etadep_trans_b";
                fileOut.width( 3 );
                fileOut << std::left << nEta << "= ";
                fileOut << std::setprecision( 4 );
                if ( b1Eta.at( uEta ) == -999999. ) fileOut << "NAN";
                else                                fileOut << b1Eta.at( uEta );
                fileOut << ", ";
                if ( b2Eta.at( uEta ) == -999999. ) fileOut << "NAN";
                else                                fileOut << b2Eta.at( uEta );
                fileOut << ", ";
                if ( b3Eta.at( uEta ) == -999999. ) fileOut << "NAN";
                else                                fileOut << b3Eta.at( uEta );
                fileOut << ", ";
                if ( b4Eta.at( uEta ) == -999999. ) fileOut << "NAN";
                else                                fileOut << b4Eta.at( uEta );
                fileOut << ", ";
                if ( b5Eta.at( uEta ) == -999999. ) fileOut << "NAN";
                else                                fileOut << b5Eta.at( uEta );
                fileOut << std::endl;
                ++nEta;
              }
              fileOut << std::endl;

              for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
                fileOut << std::endl << "ptdep_ptmin";
                fileOut.width( 3 );
                fileOut << std::left << uPt << "= ";
                fileOut << std::fixed << std::setprecision( 2 ) << ptBins_.at( uPt );
                fileOut << std::endl << "ptdep_ptmax";
                fileOut.width( 3 );
                fileOut << std::left << uPt << "= ";
                fileOut << std::fixed << std::setprecision( 2 ) << ptBins_.at( uPt + 1 );
                fileOut << std::endl << "ptdep_trans_a";
                fileOut.width( 3 );
                fileOut << std::left << uPt << "= ";
                fileOut << std::setprecision( 4 );
                if ( a1Pt.at( uPt ) == -999999. ) fileOut << "NAN";
                else                              fileOut << a1Pt.at( uPt );
                fileOut << ", ";
                if ( a2Pt.at( uPt ) == -999999. ) fileOut << "NAN";
                else                              fileOut << a2Pt.at( uPt );
                fileOut << ", ";
                if ( a3Pt.at( uPt ) == -999999. ) fileOut << "NAN";
                else                              fileOut << a3Pt.at( uPt );
                fileOut << ", ";
                if ( a4Pt.at( uPt ) == -999999. ) fileOut << "NAN";
                else                              fileOut << a4Pt.at( uPt );
                fileOut << ", ";
                if ( a5Pt.at( uPt ) == -999999. ) fileOut << "NAN";
                else                              fileOut << a5Pt.at( uPt );
                fileOut << std::endl << "ptdep_trans_b";
                fileOut.width( 3 );
                fileOut << std::left << uPt << "= ";
                fileOut << std::setprecision( 4 );
                if ( b1Pt.at( uPt ) == -999999. ) fileOut << "NAN";
                else                              fileOut << b1Pt.at( uPt );
                fileOut << ", ";
                if ( b2Pt.at( uPt ) == -999999. ) fileOut << "NAN";
                else                              fileOut << b2Pt.at( uPt );
                fileOut << ", ";
                if ( b3Pt.at( uPt ) == -999999. ) fileOut << "NAN";
                else                              fileOut << b3Pt.at( uPt );
                fileOut << ", ";
                if ( b4Pt.at( uPt ) == -999999. ) fileOut << "NAN";
                else                              fileOut << b4Pt.at( uPt );
                fileOut << ", ";
                if ( b5Pt.at( uPt ) == -999999. ) fileOut << "NAN";
                else                              fileOut << b5Pt.at( uPt );
                fileOut << std::endl;
              }
              fileOut << std::endl;

              unsigned nBin( 0 );
              for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
                nEta = 0;
                if ( useSymm_ ) {
                  for ( unsigned uEta = nEtaBins_; uEta > 0; --uEta ) {
                    fileOut << std::endl << "etaptdep_etamin";
                    fileOut.width( 3 );
                    fileOut << std::left << nEta << "= ";
                    fileOut << std::fixed << std::setprecision( 2 ) << -etaBins_.at( uEta );
                    fileOut << std::endl << "etaptdep_etamax";
                    fileOut.width( 3 );
                    fileOut << std::left << nEta << "= ";
                    fileOut << std::fixed << std::setprecision( 2 ) << -etaBins_.at( uEta - 1 );
                    fileOut << std::endl << "etaptdep_ptmin";
                    fileOut.width( 3 );
                    fileOut << std::left << uPt << "= ";
                    fileOut << std::fixed << std::setprecision( 2 ) << ptBins_.at( uPt );
                    fileOut << std::endl << "etaptdep_ptmax";
                    fileOut.width( 3 );
                    fileOut << std::left << uPt << "= ";
                    fileOut << std::fixed << std::setprecision( 2 ) << ptBins_.at( uPt + 1 );
                    fileOut << std::endl << "etaptdep_trans_a";
                    fileOut.width( 3 );
                    fileOut << std::left << nBin << "= ";
                    fileOut << std::setprecision( 4 );
                    if ( a1EtaPt.at( uEta - 1 ).at( uPt ) == -999999. ) fileOut << "NAN";
                    else                                                fileOut << a1EtaPt.at( uEta - 1 ).at( uPt );
                    fileOut << ", ";
                    if ( a2EtaPt.at( uEta - 1 ).at( uPt ) == -999999. ) fileOut << "NAN";
                    else                                                fileOut << a2EtaPt.at( uEta - 1 ).at( uPt );
                    fileOut << ", ";
                    if ( a3EtaPt.at( uEta - 1 ).at( uPt ) == -999999. ) fileOut << "NAN";
                    else                                                fileOut << a3EtaPt.at( uEta - 1 ).at( uPt );
                    fileOut << ", ";
                    if ( a4EtaPt.at( uEta - 1 ).at( uPt ) == -999999. ) fileOut << "NAN";
                    else                                                fileOut << a4EtaPt.at( uEta - 1 ).at( uPt );
                    fileOut << ", ";
                    if ( a5EtaPt.at( uEta - 1 ).at( uPt ) == -999999. ) fileOut << "NAN";
                    else                                                fileOut << a5EtaPt.at( uEta - 1 ).at( uPt );
                    fileOut << std::endl << "etaptdep_trans_b";
                    fileOut.width( 3 );
                    fileOut << std::left << nBin << "= ";
                    fileOut << std::setprecision( 4 );
                    if ( b1EtaPt.at( uEta - 1 ).at( uPt ) == -999999. ) fileOut << "NAN";
                    else                                                fileOut << b1EtaPt.at( uEta - 1 ).at( uPt );
                    fileOut << ", ";
                    if ( b2EtaPt.at( uEta - 1 ).at( uPt ) == -999999. ) fileOut << "NAN";
                    else                                                fileOut << b2EtaPt.at( uEta - 1 ).at( uPt );
                    fileOut << ", ";
                    if ( b3EtaPt.at( uEta - 1 ).at( uPt ) == -999999. ) fileOut << "NAN";
                    else                                                fileOut << b3EtaPt.at( uEta - 1 ).at( uPt );
                    fileOut << ", ";
                    if ( b4EtaPt.at( uEta - 1 ).at( uPt ) == -999999. ) fileOut << "NAN";
                    else                                                fileOut << b4EtaPt.at( uEta - 1 ).at( uPt );
                    fileOut << ", ";
                    if ( b5EtaPt.at( uEta - 1 ).at( uPt ) == -999999. ) fileOut << "NAN";
                    else                                                fileOut << b5EtaPt.at( uEta - 1 ).at( uPt );
                    fileOut << std::endl;
                    ++nEta;
                    ++nBin;
                  }
                }
                for ( unsigned uEta = 0; uEta < nEtaBins_; ++uEta ) {
                  fileOut << std::endl << "etaptdep_etamin";
                  fileOut.width( 3 );
                  fileOut << std::left << nEta << "= ";
                  fileOut << std::fixed << std::setprecision( 2 ) << etaBins_.at( uEta );
                  fileOut << std::endl << "etaptdep_etamax";
                  fileOut.width( 3 );
                  fileOut << std::left << nEta << "= ";
                  fileOut << std::fixed << std::setprecision( 2 ) << etaBins_.at( uEta + 1 );
                  fileOut << std::endl << "etaptdep_ptmin";
                  fileOut.width( 3 );
                  fileOut << std::left << uPt << "= ";
                  fileOut << std::fixed << std::setprecision( 2 ) << ptBins_.at( uPt );
                  fileOut << std::endl << "etaptdep_ptmax";
                  fileOut.width( 3 );
                  fileOut << std::left << uPt << "= ";
                  fileOut << std::fixed << std::setprecision( 2 ) << ptBins_.at( uPt + 1 );
                  fileOut << std::endl << "etaptdep_trans_a";
                  fileOut.width( 3 );
                  fileOut << std::left << nBin << "= ";
                  fileOut << std::setprecision( 4 );
                  if ( a1EtaPt.at( uEta ).at( uPt ) == -999999. ) fileOut << "NAN";
                  else                                            fileOut << a1EtaPt.at( uEta ).at( uPt );
                  fileOut << ", ";
                  if ( a2EtaPt.at( uEta ).at( uPt ) == -999999. ) fileOut << "NAN";
                  else                                            fileOut << a2EtaPt.at( uEta ).at( uPt );
                  fileOut << ", ";
                  if ( a3EtaPt.at( uEta ).at( uPt ) == -999999. ) fileOut << "NAN";
                  else                                            fileOut << a3EtaPt.at( uEta ).at( uPt );
                  fileOut << ", ";
                  if ( a4EtaPt.at( uEta ).at( uPt ) == -999999. ) fileOut << "NAN";
                  else                                            fileOut << a4EtaPt.at( uEta ).at( uPt );
                  fileOut << ", ";
                  if ( a5EtaPt.at( uEta ).at( uPt ) == -999999. ) fileOut << "NAN";
                  else                                            fileOut << a5EtaPt.at( uEta ).at( uPt );
                  fileOut << std::endl << "etaptdep_trans_b";
                  fileOut.width( 3 );
                  fileOut << std::left << nBin << "= ";
                  fileOut << std::setprecision( 4 );
                  if ( b1EtaPt.at( uEta ).at( uPt ) == -999999. ) fileOut << "NAN";
                  else                                            fileOut << b1EtaPt.at( uEta ).at( uPt );
                  fileOut << ", ";
                  if ( b2EtaPt.at( uEta ).at( uPt ) == -999999. ) fileOut << "NAN";
                  else                                            fileOut << b2EtaPt.at( uEta ).at( uPt );
                  fileOut << ", ";
                  if ( b3EtaPt.at( uEta ).at( uPt ) == -999999. ) fileOut << "NAN";
                  else                                            fileOut << b3EtaPt.at( uEta ).at( uPt );
                  fileOut << ", ";
                  if ( b4EtaPt.at( uEta ).at( uPt ) == -999999. ) fileOut << "NAN";
                  else                                            fileOut << b4EtaPt.at( uEta ).at( uPt );
                  fileOut << ", ";
                  if ( b5EtaPt.at( uEta ).at( uPt ) == -999999. ) fileOut << "NAN";
                  else                                            fileOut << b5EtaPt.at( uEta ).at( uPt );
                  fileOut << std::endl;
                  ++nEta;
                  ++nBin;
                }
                fileOut << std::endl;
              }
              fileOut << std::endl;

              fileOut.close();
              std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                        << "    written 2D transfer function file:" << std::endl
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
