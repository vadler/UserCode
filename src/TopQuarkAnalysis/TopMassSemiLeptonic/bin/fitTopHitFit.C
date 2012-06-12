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
  const std::string pathOut_( io_.getParameter< std::string >( "pathOut" ) );
  // Configuration for histogram binning
  const edm::ParameterSet & histos_( process_.getParameter< edm::ParameterSet >( "histos" ) );
  const double widthFactor_( histos_.getParameter< double >( "widthFactor" ) );
  // Configuration for fitting resolution functions
  const edm::ParameterSet & resFuncs_( process_.getParameter< edm::ParameterSet >( "resFuncs" ) );
  const bool fitResFuncs_( resFuncs_.getParameter< bool >( "fit" ) );
  const std::string resFunc_( resFuncs_.getParameter< std::string >( "resolutionFunction" ) );
  const std::string resFuncInv_( resFuncs_.getParameter< std::string >( "resolutionFunctionInverse" ) );
  const std::string resFuncRel_( resFuncs_.getParameter< std::string >( "resolutionFunctionRel" ) );
  const std::string resFuncInvRel_( resFuncs_.getParameter< std::string >( "resolutionFunctionInverseRel" ) );
  const std::string resFuncInvInv_( resFuncs_.getParameter< std::string >( "resolutionFunctionInverseInv" ) );
  const std::string resFuncInvInvRel_( resFuncs_.getParameter< std::string >( "resolutionFunctionInverseInvRel" ) );
  const bool onlyExisting_( resFuncs_.getParameter< bool >( "onlyExisting" ) );
  const bool writeFiles_( resFuncs_.getParameter< bool >( "writeFiles" ) && onlyExisting_ );
  // Configuration for fitting L5L7 JECs
  const edm::ParameterSet & jecsL5L7_( process_.getParameter< edm::ParameterSet >( "jecsL5L7" ) );
  const bool fitJecsL5L7_( jecsL5L7_.getParameter< bool >( "fit" ) );

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
  std::vector< std::string > yTitles;
  yTitles.push_back( "#Deltap_{t} (GeV)" );
  yTitles.push_back( "#Delta#eta" );
  yTitles.push_back( "#Delta#phi" );
  std::vector< std::string > yRelTitles;
  yRelTitles.push_back( "#frac{#Deltap_{t}}{p_{t}}" );
  yRelTitles.push_back( "" );
  yRelTitles.push_back( "" );
  std::vector< std::string > yTitlesAlt;
  yTitlesAlt.push_back( "#Deltap_{t}^{alt.} (GeV)" );
  yTitlesAlt.push_back( "#Delta#eta^{alt.}" );
  yTitlesAlt.push_back( "#Delta#phi^{alt.}" );
  std::vector< std::string > yRelTitlesAlt;
  yRelTitlesAlt.push_back( "frac{#Deltap_{t}^{alt.}}{p_{t}^{alt.}}" );
  yRelTitlesAlt.push_back( "" );
  yRelTitlesAlt.push_back( "" );
  std::vector< std::string > yTitlesInv;
  yTitlesInv.push_back( "#Delta#frac{1}{p_{t}} (#frac{1}{GeV})" );
  yTitlesInv.push_back( "#Delta#eta" );
  yTitlesInv.push_back( "#Delta#phi" );
  std::vector< std::string > yTitlesAltInv;
  yTitlesAltInv.push_back( "#Delta#frac{1}{p_{t}^{alt.}} (#frac{1}{GeV})" );
  yTitlesAltInv.push_back( "#Delta#eta^{alt.}" );
  yTitlesAltInv.push_back( "#Delta#phi^{alt.}" );
  const std::string zTitle( "events" );


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

        std::string yTitle( yTitles.at( uProp ) );
        std::string yRelTitle( yRelTitles.at( uProp ) );
        if ( inverse ) {
          if ( useAlt_ ) yTitle = yTitlesAltInv.at( uProp );
          else           yTitle = yTitlesInv.at( uProp );
          yRelTitle = "";
        }
        else if ( useAlt_ ) {
          yTitle    = yTitlesAlt.at( uProp );
          yRelTitle = yRelTitlesAlt.at( uProp );
        }

        const unsigned deltaBins( inverse ? propInvDeltaBins_ : propDeltaBins_ );
        const double   deltaMax( inverse ? propInvDeltaMax_ : propDeltaMax_ );

        const std::string nameDelta( name + "_Delta" );
        const std::string titleDelta( objCat );
        TH1D * histDelta( new TH1D( nameDelta.c_str(), titleDelta.c_str(), deltaBins, -deltaMax, deltaMax ) );
        histDelta->SetXTitle( yTitle.c_str() );
        histDelta->SetYTitle( zTitle.c_str() );

        const std::string nameDeltaRel( nameDelta + "Rel" );
        TH1D * histDeltaRel( new TH1D( nameDeltaRel.c_str(), titleDelta.c_str(), deltaBins, -1., 1. ) );
        histDeltaRel->SetXTitle( yRelTitle.c_str() );
        histDeltaRel->SetYTitle( zTitle.c_str() );

        std::vector< TH1D * > histVecPtDelta;
        std::vector< TH1D * > histVecPtDeltaRel;
        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
          const std::string namePt( objCat + "_" + kinProp + "_" + subFit + "_Pt" + binPt );
          const std::string namePtDelta( namePt + "_Delta" );
          const std::string titlePtDelta( objCat + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq #eta #leq " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
          TH1D * histPtDelta( new TH1D( namePtDelta.c_str(), titlePtDelta.c_str(), deltaBins, -deltaMax, deltaMax ) );
          histPtDelta->SetXTitle( yTitle.c_str() );
          histPtDelta->SetYTitle( zTitle.c_str() );
          histVecPtDelta.push_back( histPtDelta );
          if ( ! inverse && kinProp == "Pt" ) {
            const std::string namePtDeltaRel( namePtDelta + "Rel" );
            TH1D * histPtDeltaRel( new TH1D( namePtDeltaRel.c_str(), titlePtDelta.c_str(), deltaBins, -1., 1. ) );
            histPtDeltaRel->SetXTitle( yRelTitle.c_str() );
            histPtDeltaRel->SetYTitle( zTitle.c_str() );
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

          const std::string nameEta( objCat + "_" + kinProp + "_" + subFit + "_" + binEta );

          const std::string nameEtaDelta( nameEta + "_Delta" );
          const std::string titleEtaDelta( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) );
          TH1D * histEtaDelta( new TH1D( nameEtaDelta.c_str(), titleEtaDelta.c_str(), deltaBins, -deltaMax, deltaMax ) );
          histEtaDelta->SetXTitle( yTitle.c_str() );
          histEtaDelta->SetYTitle( zTitle.c_str() );

          const std::string nameEtaDeltaRel( nameEtaDelta + "Rel" );
          TH1D * histEtaDeltaRel( new TH1D( nameEtaDeltaRel.c_str(), titleEtaDelta.c_str(), deltaBins, -1., 1. ) );
          histEtaDeltaRel->SetXTitle( yRelTitle.c_str() );
          histEtaDeltaRel->SetYTitle( zTitle.c_str() );

          // Split data into p_t bins
          dataCont weightEtaBin( nPtBins_ );
          dataCont ptEtaBin( nPtBins_ );
          dataCont propEtaBin( nPtBins_ );
          dataCont propGenEtaBin( nPtBins_ );
          std::vector< unsigned > sizePt( nPtBins_ );
          for ( unsigned uEntry = 0; uEntry < sizeEta_.at( uEta ); ++uEntry ) { // FIXME: WRONG for useSymm_ ???
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
            const std::string nameEtaPt( objCat + "_" + kinProp + "_" + subFit + "_" + binEta + "_Pt" + binPt );

            const std::string nameEtaPtDelta( nameEtaPt + "_Delta" );
            const std::string titleEtaPtDelta( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) + ", " + boost::lexical_cast< std::string >( ptBins_.at( uPt ) ) + " GeV #leq #eta #leq " + boost::lexical_cast< std::string >( ptBins_.at( uPt + 1 ) ) + " GeV" );
            TH1D * histEtaPtDelta( new TH1D( nameEtaPtDelta.c_str(), titleEtaPtDelta.c_str(), deltaBins, -deltaMax, deltaMax ) );
            histEtaPtDelta->SetXTitle( yTitle.c_str() );
            histEtaPtDelta->SetYTitle( zTitle.c_str() );

            const std::string nameEtaPtDeltaRel( nameEtaPtDelta + "Rel" );
            TH1D * histEtaPtDeltaRel( new TH1D( nameEtaPtDeltaRel.c_str(), titleEtaPtDelta.c_str(), deltaBins, -1., 1. ) );
            histEtaPtDeltaRel->SetXTitle( yRelTitle.c_str() );
            histEtaPtDeltaRel->SetYTitle( zTitle.c_str() );

            for ( unsigned uEntry = 0; uEntry < sizePt.at( uPt ); ++uEntry ) {
              if ( refGen_ ) {
                if ( inverse && kinProp == "Pt" ) histEtaPtDelta->Fill( 1. / propGenEtaBin.at( uPt ).at( uEntry ) - 1. / propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                else                              histEtaPtDelta->Fill( propGenEtaBin.at( uPt ).at( uEntry ) - propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                if ( ! inverse && kinProp == "Pt" ) histEtaPtDeltaRel->Fill( ( propGenEtaBin.at( uPt ).at( uEntry ) - propEtaBin.at( uPt ).at( uEntry ) ) / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
              else {
                if ( inverse && kinProp == "Pt" ) histEtaPtDelta->Fill( 1. / propEtaBin.at( uPt ).at( uEntry ) - 1. / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                else                              histEtaPtDelta->Fill( propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                if ( ! inverse && kinProp == "Pt" ) histEtaPtDeltaRel->Fill( ( propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ) ) / propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
            } // loop: uEntry < ptEtaBin.at( uPt ).size()

            histDelta->Add( histEtaPtDelta );
            histVecPtDelta.at( uPt )->Add( histEtaPtDelta );
            histEtaDelta->Add( histEtaPtDelta );
            if ( ! inverse && kinProp == "Pt" ) {
              histDeltaRel->Add( histEtaPtDeltaRel );
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
                if ( ! inverse && kinProp == "Pt" ) histEtaPtDeltaRelRebin->Fill( ( propGenEtaBin.at( uPt ).at( uEntry ) - propEtaBin.at( uPt ).at( uEntry ) ) / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
              else {
                if ( inverse && kinProp == "Pt" ) histEtaPtDeltaRebin->Fill( 1. / propEtaBin.at( uPt ).at( uEntry ) - 1. / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                else                              histEtaPtDeltaRebin->Fill( propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                if ( ! inverse && kinProp == "Pt" ) histEtaPtDeltaRelRebin->Fill( ( propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ) ) / propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
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
                if ( ! inverse && kinProp == "Pt" ) histEtaDeltaRelRebin->Fill( ( propGenEtaBin.at( uPt ).at( uEntry ) - propEtaBin.at( uPt ).at( uEntry ) ) / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
              }
              else {
                if ( inverse && kinProp == "Pt" ) histEtaDeltaRebin->Fill( 1. / propEtaBin.at( uPt ).at( uEntry ) - 1. / propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                else                              histEtaDeltaRebin->Fill( propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
                if ( ! inverse && kinProp == "Pt" ) histEtaDeltaRelRebin->Fill( ( propEtaBin.at( uPt ).at( uEntry ) - propGenEtaBin.at( uPt ).at( uEntry ) ) / propEtaBin.at( uPt ).at( uEntry ), weightEtaBin.at( uPt ).at( uEntry ) );
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
        std::vector< TH1D * > histVecPtDeltaRelRebin;
        for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
          const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
          const std::string namePt( objCat + "_" + kinProp + "_" + subFit + "_Pt" + binPt );

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
          histPtDeltaRebin->SetXTitle( yTitle.c_str() );
          histPtDeltaRebin->SetYTitle( zTitle.c_str() );
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
            histPtDeltaRelRebin->SetXTitle( yRelTitle.c_str() );
            histPtDeltaRelRebin->SetYTitle( zTitle.c_str() );
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

          for ( unsigned uEntry = 0; uEntry < sizeEta_.at( uEta ); ++uEntry ) { // FIXME: WRONG for useSymm_ ???
            if ( propData_.at( uEta ).at( uEntry ) == -5. ) continue; // no match
            if ( refGen_ ) {
              if ( inverse && kinProp == "Pt" ) histDeltaRebin->Fill( 1. / propGenData_.at( uEta ).at( uEntry ) - 1. / propData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
              else                              histDeltaRebin->Fill( propGenData_.at( uEta ).at( uEntry ) - propData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
              if ( ! inverse && kinProp == "Pt" ) histDeltaRelRebin->Fill( ( propGenData_.at( uEta ).at( uEntry ) - propData_.at( uEta ).at( uEntry ) ) / propGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
            }
            else {
              if ( inverse && kinProp == "Pt" ) histDeltaRebin->Fill( 1. / propData_.at( uEta ).at( uEntry ) - 1. / propGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
              else                              histDeltaRebin->Fill( propData_.at( uEta ).at( uEntry ) - propGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
              if ( ! inverse && kinProp == "Pt" ) histDeltaRelRebin->Fill( ( propData_.at( uEta ).at( uEntry ) - propGenData_.at( uEta ).at( uEntry ) ) /propData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
            }
            for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
              if ( ptBins_.at( uPt ) <= ptData_.at( uEta ).at( uEntry ) && ptData_.at( uEta ).at( uEntry ) < ptBins_.at( uPt + 1 ) ) {
                if ( refGen_ ) {
                  if ( inverse && kinProp == "Pt" ) histVecPtDeltaRebin.at( uPt )->Fill( 1. / propGenData_.at( uEta ).at( uEntry ) - 1. / propData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                  else                              histVecPtDeltaRebin.at( uPt )->Fill( propGenData_.at( uEta ).at( uEntry ) - propData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                  if ( ! inverse && kinProp == "Pt" ) histVecPtDeltaRelRebin.at( uPt )->Fill( ( propGenData_.at( uEta ).at( uEntry ) - propData_.at( uEta ).at( uEntry ) ) / propGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                }
                else {
                  if ( inverse && kinProp == "Pt" ) histVecPtDeltaRebin.at( uPt )->Fill( 1. / propData_.at( uEta ).at( uEntry ) - 1. / propGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                  else                              histVecPtDeltaRebin.at( uPt )->Fill( propData_.at( uEta ).at( uEntry ) - propGenData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
                  if ( ! inverse && kinProp == "Pt" ) histVecPtDeltaRelRebin.at( uPt )->Fill( ( propData_.at( uEta ).at( uEntry ) - propGenData_.at( uEta ).at( uEntry ) ) / propData_.at( uEta ).at( uEntry ), weightData_.at( uEta ).at( uEntry ) );
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

          // Fit performance histograms

          // Single resolutions maps
          const std::string nameDeltaFitChi2Map( name + "_DeltaFitChi2Map" );
          TH2D * histDeltasFitChi2Map( new TH2D( nameDeltaFitChi2Map.c_str(), titleChi2.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
          histDeltasFitChi2Map->SetXTitle( titleEta.c_str() );
          histDeltasFitChi2Map->SetYTitle( titlePt.c_str() );
          histDeltasFitChi2Map->SetZTitle( titleChi2.c_str() );
          const std::string nameDeltaFitProbMap( name + "_DeltaFitProbMap" );
          TH2D * histDeltasFitProbMap( new TH2D( nameDeltaFitProbMap.c_str(), titleProb.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
          histDeltasFitProbMap->SetXTitle( titleEta.c_str() );
          histDeltasFitProbMap->SetYTitle( titlePt.c_str() );
          histDeltasFitProbMap->SetZTitle( titleProb.c_str() );
          const std::string nameDeltaFitProb( name + "_DeltasNtup_fitProb" );
          TH1D * histDeltaFitProb( new TH1D( nameDeltaFitProb.c_str(), titleProb.c_str(), 20, 0., 1. ) );
          histDeltaFitProb->SetXTitle( titleProb.c_str() );
          // Failing fits
          const std::string nameDeltasFitMissingMap( name + "_eDeltasFitMissingMap" );
          TH2D * histDeltasFitMissingMap( new TH2D( nameDeltasFitMissingMap.c_str(), titleProb.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
          histDeltasFitMissingMap->SetXTitle( titleEta.c_str() );
          histDeltasFitMissingMap->SetYTitle( titlePt.c_str() );
          const std::string nameDeltasFitBadProbMap( name + "_DeltasFitBadProbMap" );
          TH2D * histDeltasFitBadProbMap( new TH2D( nameDeltasFitBadProbMap.c_str(), titleProb.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
          histDeltasFitBadProbMap->SetXTitle( titleEta.c_str() );
          histDeltasFitBadProbMap->SetYTitle( titlePt.c_str() );
          histDeltasFitBadProbMap->SetZTitle( titleProb.c_str() );
          const std::string nameDeltasNFitBadNdfMap( name + "_DeltasNFitBadNdfMap" );
          TH2D * histDeltasNFitBadNdfMap( new TH2D( nameDeltasNFitBadNdfMap.c_str(), titleNdf.c_str(), nEtaBins_, etaBins_.data(), nPtBins_, ptBins_.data() ) );
          histDeltasNFitBadNdfMap->SetXTitle( titleEta.c_str() );
          histDeltasNFitBadNdfMap->SetYTitle( titlePt.c_str() );
          histDeltasNFitBadNdfMap->SetZTitle( titleNdf.c_str() );
          const std::string nameDeltasFitBadProb( name + "_DeltasFitBadProb" );
          TH1D * histDeltasFitBadProb( new TH1D( nameDeltasFitBadProb.c_str(), titleProb.c_str(), 20, 0., 1. ) );
          histDeltasFitBadProb->SetXTitle( titleProb.c_str() );
          const std::string nameDeltasFitBadNdf( name + "_DeltasFitBadNdf" );
          TH1D * histDeltasFitBadNdf( new TH1D( nameDeltasFitBadNdf.c_str(), titleNdf.c_str(), 20, 0., 20. ) );
          histDeltasFitBadNdf->SetXTitle( titleNdf.c_str() );

          // Resolution functions
          const std::string nameSigmaFitChi2Map( name + "_SigmaFitChi2Map" );
          TH1D * histSigmaFitChi2Map( new TH1D( nameSigmaFitChi2Map.c_str(), titleChi2.c_str(), nEtaBins_, etaBins_.data() ) );
          histSigmaFitChi2Map->SetXTitle( titleEta.c_str() );
          histSigmaFitChi2Map->SetYTitle( titleChi2.c_str() );
          const std::string nameSigmaFitProbMap( name + "_SigmaFitProbMap" );
          TH1D * histSigmaFitProbMap( new TH1D( nameSigmaFitProbMap.c_str(), titleProb.c_str(), nEtaBins_, etaBins_.data() ) );
          histSigmaFitProbMap->SetXTitle( titleEta.c_str() );
          histSigmaFitProbMap->SetYTitle( titleProb.c_str() );
          const std::string nameSigmaFitProb( name + "_SigmaFitProb" );
          TH1D * histSigmaFitProb( new TH1D( nameSigmaFitProb.c_str(), titleProb.c_str(), 20, 0., 1. ) );
          histSigmaFitProb->SetXTitle( titleProb.c_str() );
          // Failing fits
          const std::string nameSigmaFitMissingMap( name + "_SigmaFitMissingMap" );
          TH1D * histSigmaFitMissingMap( new TH1D( nameSigmaFitMissingMap.c_str(), titleProb.c_str(), nEtaBins_, etaBins_.data() ) );
          histSigmaFitMissingMap->SetXTitle( titleEta.c_str() );
          const std::string nameSigmaFitBadProbMap( name + "_SigmaFitBadProbMap" );
          TH1D * histSigmaFitBadProbMap( new TH1D( nameSigmaFitBadProbMap.c_str(), titleProb.c_str(), nEtaBins_, etaBins_.data() ) );
          histSigmaFitBadProbMap->SetXTitle( titleEta.c_str() );
          histSigmaFitBadProbMap->SetYTitle( titleProb.c_str() );
          const std::string nameSigmaFitBadNdfMap( name + "_SigmaFitBadNdfMap" );
          TH1D * histSigmaFitBadNdfMap( new TH1D( nameSigmaFitBadNdfMap.c_str(), titleNdf.c_str(), nEtaBins_, etaBins_.data() ) );
          histSigmaFitBadNdfMap->SetXTitle( titleEta.c_str() );
          histSigmaFitBadNdfMap->SetYTitle( titleNdf.c_str() );
          histSigmaFitBadNdfMap->SetXTitle( titleProb.c_str() );
          const std::string nameSigmaFitBadNdf( name + "_SigmaFitBadNdff" );
          TH1D * histSigmaFitBadNdf( new TH1D( nameSigmaFitBadNdf.c_str(), titleNdf.c_str(), 20, 0., 20. ) );
          histSigmaFitBadNdf->SetXTitle( titleNdf.c_str() );

          // Loop over eta bins
          TList * listFit( dirFit_->GetListOfKeys() );
          TIter nextInListFit( listFit );
          while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
            if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
            const std::string binEta( keyEta->GetName() );
            const unsigned uEta( std::atoi( binEta.substr( 3 ).data() ) );
            dirFit_->cd( binEta.c_str() );

            const std::string nameEta( objCat + "_" + kinProp + "_" + subFit + "_" + binEta );
            const std::string formula( inverse ? resFuncInv_ : resFunc_ );
            const std::string formulaRel( inverse ? resFuncInvRel_ : resFuncRel_ );

            const std::string nameDeltaFitChi2( name + "_DeltaFitChi2" );
            TH1D * histDeltaFitChi2( new TH1D( nameDeltaFitChi2.c_str(), titleChi2.c_str(), nPtBins_, ptBins_.data() ) );
            histDeltaFitChi2->SetXTitle( titlePt.c_str() );
            histDeltaFitChi2->SetYTitle( titleChi2.c_str() );
            const std::string nameDeltaFitProb( name + "_DeltaFitProb" );
            TH1D * histDeltaFitProb( new TH1D( nameDeltaFitProb.c_str(), titleProb.c_str(), nPtBins_, ptBins_.data() ) );
            histDeltaFitProb->SetXTitle( titlePt.c_str() );
            histDeltaFitProb->SetYTitle( titleProb.c_str() );

            const std::string nameEtaSigma( nameEta + "_Sigma" );
            const std::string titleEtaSigma( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) );
            TH1D * histEtaSigma( new TH1D( nameEtaSigma.c_str(), titleEtaSigma.c_str(), nPtBins_, ptBins_.data() ) );

            for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
              const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
              const std::string nameEtaPt( objCat + "_" + kinProp + "_" + subFit + "_" + binEta + "_Pt" + binPt );

              const std::string nameEtaPtDelta( nameEtaPt + "_Delta" );
              const std::string nameEtaPtDeltaRebin( nameEtaPtDelta + "Rebin" );
              TH1D * histEtaPtDeltaRebin( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaPtDeltaRebin.c_str() ) ) );

              const std::string nameEtaPtDeltaFit( nameEtaPtDelta + "_fit" );
              const std::string nameEtaPtDeltaRebinFit( nameEtaPtDeltaRebin + "_fit" );
              TF1 * fitEtaPtDeltaRebin( new TF1( nameEtaPtDeltaRebinFit.c_str(), "gaus", histEtaPtDeltaRebin->GetXaxis()->GetXmin(), histEtaPtDeltaRebin->GetXaxis()->GetXmax() ) );
              TFitResultPtr fitEtaPtDeltaResultPtr( histEtaPtDeltaRebin->Fit( fitEtaPtDeltaRebin, optionsFit_.c_str() ) );
              if ( fitEtaPtDeltaResultPtr >= 0 ) {
                if ( fitEtaPtDeltaResultPtr->Status() == 0 && fitEtaPtDeltaResultPtr->Ndf() != 0. ) {
                  histDeltasFitChi2Map->SetBinContent( uEta + 1, uPt + 1, fitEtaPtDeltaResultPtr->Chi2() / fitEtaPtDeltaResultPtr->Ndf() );
                  histDeltaFitChi2->SetBinContent( uPt + 1, fitEtaPtDeltaResultPtr->Chi2() / fitEtaPtDeltaResultPtr->Ndf() );
                  histDeltasFitProbMap->SetBinContent( uEta + 1, uPt + 1, fitEtaPtDeltaResultPtr->Prob() );
                  histDeltaFitProb->Fill( fitEtaPtDeltaResultPtr->Prob() );
                  histDeltaFitProb->SetBinContent( uPt + 1, fitEtaPtDeltaResultPtr->Prob() );
                  histEtaSigma->SetBinContent( uPt + 1, fitEtaPtDeltaResultPtr->Parameter( 2 ) );
                  histEtaSigma->SetBinError( uPt + 1, fitEtaPtDeltaResultPtr->ParError( 2 ) );
                }
                else {
                  if ( fitEtaPtDeltaResultPtr->Prob() == 0. ) {
                    histDeltasFitBadProbMap->SetBinContent( uEta + 1, uPt + 1, -1. );
                    histDeltasFitBadProb->AddBinContent( 0 );
                  }
                  else {
                    histDeltasFitBadProbMap->SetBinContent( uEta + 1, uPt + 1, fitEtaPtDeltaResultPtr->Prob() );
                    histDeltasFitBadProb->Fill( fitEtaPtDeltaResultPtr->Prob() );
                  }
                  histDeltasNFitBadNdfMap->SetBinContent( uEta + 1, uPt + 1, fitEtaPtDeltaResultPtr->Ndf() );
                  histDeltasFitBadNdf->Fill( fitEtaPtDeltaResultPtr->Ndf() );
                  if ( verbose_ > 2 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    failing fit in directory '"; gDirectory->pwd();
                    std::cout << "    '" << nameEtaPtDeltaRebin << "' status " << fitEtaPtDeltaResultPtr->Status() << std::endl;
                  }
                }
              }
              else {
                const Int_t iBin( histDeltasFitMissingMap->GetBin( uEta + 1, uPt + 1 ) );
                histDeltasFitMissingMap->AddBinContent( iBin );
                if ( verbose_ > 1 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    missing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << nameEtaPtDeltaRebin << std::endl;
                }
              }
            } // loop: uPt < nPtBins_

            const std::string nameEtaSigmaFit( nameEtaSigma + "_fit" );
            TF1 * fitEtaSigmaFit( new TF1( nameEtaSigmaFit.c_str(), formula.c_str() ) );
            fitEtaSigmaFit->SetRange( histEtaSigma->GetXaxis()->GetXmin(), histEtaSigma->GetXaxis()->GetXmax() );
            TFitResultPtr fitEtaSigmaFitResultPtr( histEtaSigma->Fit( fitEtaSigmaFit, optionsFitSigma_.c_str() ) );
            if ( fitEtaSigmaFitResultPtr >= 0 ) {
              if ( ( fitEtaSigmaFitResultPtr->Status() == 0 || fitEtaSigmaFitResultPtr->Status() == 4000 ) && fitEtaSigmaFitResultPtr->Ndf() != 0. ) { // ignore errors from IMPROVE
                histSigmaFitChi2Map->SetBinContent( uEta + 1, fitEtaSigmaFitResultPtr->Chi2() / fitEtaSigmaFitResultPtr->Ndf() );
                histSigmaFitProbMap->SetBinContent( uEta + 1, fitEtaSigmaFitResultPtr->Prob() );
                histSigmaFitProb->Fill( fitEtaSigmaFitResultPtr->Prob() );
                if ( ! inverse && kinProp == "Pt" ) {
                  const Double_t * params( fitEtaSigmaFitResultPtr->GetParams() );
                  const std::string nameEtaSigmaFitRel( nameEtaSigmaFit + "Rel" );
                  TF1 * fitEtaSigmaRelFit( new TF1( nameEtaSigmaFitRel.c_str(), formulaRel.c_str(), histEtaSigma->GetXaxis()->GetXmin(), histEtaSigma->GetXaxis()->GetXmax() ) );
                  fitEtaSigmaRelFit->SetParameters( params );
                  if ( overwrite_ ) fitEtaSigmaRelFit->Write( 0, TObject::kOverwrite );
                  else              fitEtaSigmaRelFit->Write();
                  if ( inverse ) {
                    const std::string nameEtaSigmaFitInv( nameEtaSigmaFit + "Inv" );
                    TF1 * fitEtaSigmaInvFit( new TF1( nameEtaSigmaFitInv.c_str(), resFuncInvInv_.c_str(), histEtaSigma->GetXaxis()->GetXmin(), histEtaSigma->GetXaxis()->GetXmax() ) );
                    fitEtaSigmaInvFit->SetParameters( params );
                    if ( overwrite_ ) fitEtaSigmaInvFit->Write( 0, TObject::kOverwrite );
                    else              fitEtaSigmaInvFit->Write();
                    const std::string nameEtaSigmaFitInvRel( nameEtaSigmaFitInv + "Rel" );
                    TF1 * fitEtaSigmaInvRelFit( new TF1( nameEtaSigmaFitInvRel.c_str(), resFuncInvInvRel_.c_str(), histEtaSigma->GetXaxis()->GetXmin(), histEtaSigma->GetXaxis()->GetXmax() ) );
                    fitEtaSigmaInvRelFit->SetParameters( params );
                    if ( overwrite_ ) fitEtaSigmaInvRelFit->Write( 0, TObject::kOverwrite );
                    else              fitEtaSigmaInvRelFit->Write();
                  }
                }
                if ( writeFiles_ ) {
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
                  histSigmaFitBadProbMap->SetBinContent( uEta + 1, -1. );
//                   histSigmaNtupBadProb->AddBinContent( 0 );
                }
                else {
                  histSigmaFitBadProbMap->SetBinContent( uEta + 1, fitEtaSigmaFitResultPtr->Prob() );
//                   histSigmaNtupBadProb->Fill( fitEtaSigmaFitResultPtr->Prob() );
                }
                histSigmaFitBadNdfMap->SetBinContent( uEta + 1, fitEtaSigmaFitResultPtr->Ndf() );
                histSigmaFitBadNdf->Fill( fitEtaSigmaFitResultPtr->Ndf() );
                if ( writeFiles_ ) {
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
              histSigmaFitMissingMap->AddBinContent( uEta + 1 );
              if ( writeFiles_ ) {
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
      if ( writeFiles_ ) {

        // File name
        std::string nameOut( pathOut_ + "/gentResolution_Fall11_R4_" + objCat ); // FIXME: hard-coding
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

            const std::string nameDelta( name + "_Delta" );
            const std::string nameDeltaRebin( nameDelta + "Rebin" );
            TH1D * histDeltaRebin( dynamic_cast< TH1D* >( gDirectory->Get( nameDeltaRebin.c_str() ) ) );

            const std::string nameDeltaFit( nameDelta + "_fit" );
            const std::string nameDeltaRebinFit( nameDeltaRebin + "_fit" );
            TF1 * fitDeltaRebin( new TF1( nameDeltaRebinFit.c_str(), "gaus", histDeltaRebin->GetXaxis()->GetXmin(), histDeltaRebin->GetXaxis()->GetXmax() ) );
            TFitResultPtr fitDeltaResultPtr( histDeltaRebin->Fit( fitDeltaRebin, optionsFit_.c_str() ) );
            if ( fitDeltaResultPtr >= 0 ) {
              if ( fitDeltaResultPtr->Status() == 0 && fitDeltaResultPtr->Ndf() != 0. ) {
              }
              else {
                if ( fitDeltaResultPtr->Prob() == 0. ) {
                }
                else {
                }
                if ( verbose_ > 2 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    failing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << nameDeltaRebin << "' status " << fitDeltaResultPtr->Status() << std::endl;
                }
              }
            }
            else {
              if ( verbose_ > 1 ) {
                std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                          << "    missing fit in directory '"; gDirectory->pwd();
                std::cout << "    '" << nameDeltaRebin << std::endl;
              }
            }

            const std::string nameMean( name + "_Mean" );
            const std::string titleMean( objCat );
            TH1D * histMean( new TH1D( nameMean.c_str(), titleMean.c_str(), nPtBins_, ptBins_.data() ) );
            for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
              const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
              const std::string namePt( objCat + "_" + kinProp + "_" + subFit + "_Pt" + binPt );

              const std::string namePtDelta( namePt + "_Delta" );
              const std::string namePtDeltaRebin( namePtDelta + "Rebin" );
              TH1D * histPtDeltaRebin( dynamic_cast< TH1D* >( gDirectory->Get( namePtDeltaRebin.c_str() ) ) );

              const std::string namePtDeltaFit( namePtDelta + "_fit" );
              const std::string namePtDeltaRebinFit( namePtDeltaRebin + "_fit" );
              TF1 * fitPtDeltaRebin( new TF1( namePtDeltaRebinFit.c_str(), "gaus", histPtDeltaRebin->GetXaxis()->GetXmin(), histPtDeltaRebin->GetXaxis()->GetXmax() ) );
              TFitResultPtr fitPtDeltaResultPtr( histPtDeltaRebin->Fit( fitPtDeltaRebin, optionsFit_.c_str() ) );
              if ( fitPtDeltaResultPtr >= 0 ) {
                if ( fitPtDeltaResultPtr->Status() == 0 && fitPtDeltaResultPtr->Ndf() != 0. ) {
                  histMean->SetBinContent( uPt + 1, fitPtDeltaResultPtr->Parameter( 1 ) );
                  histMean->SetBinError( uPt + 1, fitPtDeltaResultPtr->ParError( 1 ) );
                }
                else {
                  if ( fitPtDeltaResultPtr->Prob() == 0. ) {
                  }
                  else {
                  }
                  if ( verbose_ > 2 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    failing fit in directory '"; gDirectory->pwd();
                    std::cout << "    '" << namePtDeltaRebin << "' status " << fitPtDeltaResultPtr->Status() << std::endl;
                  }
                }
              }
              else {
                if ( verbose_ > 1 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    missing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << namePtDeltaRebin << std::endl;
                }
              }
            } // loop: uPt < nPtBins_

            if ( kinProp == "Pt" ) {
              const std::string nameDeltaRel( nameDelta + "Rel" );
              const std::string nameDeltaRelRebin( nameDeltaRel + "Rebin" );
              TH1D * histDeltaRelRebin( dynamic_cast< TH1D* >( gDirectory->Get( nameDeltaRelRebin.c_str() ) ) );
              const std::string nameDeltaRelFit( nameDeltaRel + "_fit" );
              const std::string nameDeltaRelRebinFit( nameDeltaRelRebin + "_fit" );
              TF1 * fitDeltaRelRebin( new TF1( nameDeltaRelRebinFit.c_str(), "gaus", histDeltaRelRebin->GetXaxis()->GetXmin(), histDeltaRelRebin->GetXaxis()->GetXmax() ) );
              TFitResultPtr fitDeltaRelResultPtr( histDeltaRelRebin->Fit( fitDeltaRelRebin, optionsFit_.c_str() ) );
              if ( fitDeltaRelResultPtr >= 0 ) {
                if ( fitDeltaRelResultPtr->Status() == 0 && fitDeltaRelResultPtr->Ndf() != 0. ) {
                }
                else {
                  if ( fitDeltaRelResultPtr->Prob() == 0. ) {
                  }
                  else {
                  }
                  if ( verbose_ > 2 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    failing fit in directory '"; gDirectory->pwd();
                    std::cout << "    '" << nameDeltaRelRebin << "' status " << fitDeltaRelResultPtr->Status() << std::endl;
                  }
                }
              }
              else {
                if ( verbose_ > 1 ) {
                  std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                            << "    missing fit in directory '"; gDirectory->pwd();
                  std::cout << "    '" << nameDeltaRelRebin << std::endl;
                }
              }

              const std::string nameMeanRel( nameMean + "Rel" );
              TH1D * histMeanRel( new TH1D( nameMeanRel.c_str(), titleMean.c_str(), nPtBins_, ptBins_.data() ) );
              for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
                const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
                const std::string namePt( objCat + "_" + kinProp + "_" + subFit + "_Pt" + binPt );

                const std::string namePtDeltaRel( namePt + "_DeltaRel" );
                const std::string namePtDeltaRelRebin( namePtDeltaRel + "Rebin" );
                TH1D * histPtDeltaRelRebin( dynamic_cast< TH1D* >( gDirectory->Get( namePtDeltaRelRebin.c_str() ) ) );

                const std::string namePtDeltaRelFit( namePtDeltaRel + "_fit" );
                const std::string namePtDeltaRelRebinFit( namePtDeltaRelRebin + "_fit" );
                TF1 * fitPtDeltaRelRebin( new TF1( namePtDeltaRelRebinFit.c_str(), "gaus", histPtDeltaRelRebin->GetXaxis()->GetXmin(), histPtDeltaRelRebin->GetXaxis()->GetXmax() ) );
                TFitResultPtr fitPtDeltaRelResultPtr( histPtDeltaRelRebin->Fit( fitPtDeltaRelRebin, optionsFit_.c_str() ) );
                if ( fitPtDeltaRelResultPtr >= 0 ) {
                  if ( fitPtDeltaRelResultPtr->Status() == 0 && fitPtDeltaRelResultPtr->Ndf() != 0. ) {
                    histMeanRel->SetBinContent( uPt + 1, fitPtDeltaRelResultPtr->Parameter( 1 ) );
                    histMeanRel->SetBinError( uPt + 1, fitPtDeltaRelResultPtr->ParError( 1 ) );
                  }
                  else {
                    if ( fitPtDeltaRelResultPtr->Prob() == 0. ) {
                    }
                    else {
                    }
                    if ( verbose_ > 2 ) {
                      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                                << "    failing fit in directory '"; gDirectory->pwd();
                      std::cout << "    '" << namePtDeltaRelRebin << "' status " << fitPtDeltaRelResultPtr->Status() << std::endl;
                    }
                  }
                }
                else {
                  if ( verbose_ > 1 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    missing fit in directory '"; gDirectory->pwd();
                    std::cout << "    '" << namePtDeltaRelRebin << std::endl;
                  }
                }
              } // loop: uPt < nPtBins_
            }

            // Loop over eta bins
            TList * listFit( dirFit_->GetListOfKeys() );
            TIter nextInListFit( listFit );
            while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
              if ( std::string( keyEta->GetClassName() ) != nameDirClass ) continue;
              const std::string binEta( keyEta->GetName() );
              const unsigned uEta( std::atoi( binEta.substr( 3 ).data() ) );
              dirFit_->cd( binEta.c_str() );

              const std::string nameEta( objCat + "_" + kinProp + "_" + subFit + "_" + binEta );

              const std::string nameEtaMean( nameEta + "_Mean" );
              const std::string titleEtaMean( objCat + ", " + boost::lexical_cast< std::string >( etaBins_.at( uEta ) ) + " #leq #eta #leq " + boost::lexical_cast< std::string >( etaBins_.at( uEta + 1 ) ) );
              TH1D * histEtaMean( new TH1D( nameEtaMean.c_str(), titleEtaMean.c_str(), nPtBins_, ptBins_.data() ) );
              for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
                const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
                const std::string nameEtaPt( objCat + "_" + kinProp + "_" + subFit + "_" + binEta + "_Pt" + binPt );

                const std::string nameEtaPtDelta( nameEtaPt + "_Delta" );
                const std::string nameEtaPtDeltaRebin( nameEtaPtDelta + "Rebin" );
                TH1D * histEtaPtDeltaRebin( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaPtDeltaRebin.c_str() ) ) );

                const std::string nameEtaPtDeltaFit( nameEtaPtDelta + "_fit" );
                const std::string nameEtaPtDeltaRebinFit( nameEtaPtDeltaRebin + "_fit" );
                TF1 * fitEtaPtDeltaRebin( new TF1( nameEtaPtDeltaRebinFit.c_str(), "gaus", histEtaPtDeltaRebin->GetXaxis()->GetXmin(), histEtaPtDeltaRebin->GetXaxis()->GetXmax() ) );
                TFitResultPtr fitEtaPtDeltaResultPtr( histEtaPtDeltaRebin->Fit( fitEtaPtDeltaRebin, optionsFit_.c_str() ) );
                if ( fitEtaPtDeltaResultPtr >= 0 ) {
                  if ( fitEtaPtDeltaResultPtr->Status() == 0 && fitEtaPtDeltaResultPtr->Ndf() != 0. ) {
                    histEtaMean->SetBinContent( uPt + 1, fitEtaPtDeltaResultPtr->Parameter( 1 ) );
                    histEtaMean->SetBinError( uPt + 1, fitEtaPtDeltaResultPtr->ParError( 1 ) );
                  }
                  else {
                    if ( fitEtaPtDeltaResultPtr->Prob() == 0. ) {
                    }
                    else {
                    }
                    if ( verbose_ > 2 ) {
                      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                                << "    failing fit in directory '"; gDirectory->pwd();
                      std::cout << "    '" << nameEtaPtDeltaRebin << "' status " << fitEtaPtDeltaResultPtr->Status() << std::endl;
                    }
                  }
                }
                else {
                  if ( verbose_ > 1 ) {
                    std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                              << "    missing fit in directory '"; gDirectory->pwd();
                    std::cout << "    '" << nameEtaPtDeltaRebin << std::endl;
                  }
                }
              } // loop: uPt < nPtBins_

              if ( kinProp == "Pt" ) {
                const std::string nameEtaMeanRel( nameEtaMean + "Rel" );
                TH1D * histEtaMeanRel( new TH1D( nameEtaMeanRel.c_str(), titleEtaMean.c_str(), nPtBins_, ptBins_.data() ) );
                for ( unsigned uPt = 0; uPt < nPtBins_; ++uPt ) {
                  const std::string binPt( boost::lexical_cast< std::string >( uPt ) );
                  const std::string nameEtaPt( objCat + "_" + kinProp + "_" + subFit + "_" + binEta + "_Pt" + binPt );

                  const std::string nameEtaPtDeltaRel( nameEtaPt + "_DeltaRel" );
                  const std::string nameEtaPtDeltaRelRebin( nameEtaPtDeltaRel + "Rebin" );
                  TH1D * histEtaPtDeltaRelRebin( dynamic_cast< TH1D* >( gDirectory->Get( nameEtaPtDeltaRelRebin.c_str() ) ) );

                  const std::string nameEtaPtDeltaRelFit( nameEtaPtDeltaRel + "_fit" );
                  const std::string nameEtaPtDeltaRelRebinFit( nameEtaPtDeltaRelRebin + "_fit" );
                  TF1 * fitEtaPtDeltaRelRebin( new TF1( nameEtaPtDeltaRelRebinFit.c_str(), "gaus", histEtaPtDeltaRelRebin->GetXaxis()->GetXmin(), histEtaPtDeltaRelRebin->GetXaxis()->GetXmax() ) );
                  TFitResultPtr fitEtaPtDeltaRelResultPtr( histEtaPtDeltaRelRebin->Fit( fitEtaPtDeltaRelRebin, optionsFit_.c_str() ) );
                  if ( fitEtaPtDeltaRelResultPtr >= 0 ) {
                    if ( fitEtaPtDeltaRelResultPtr->Status() == 0 && fitEtaPtDeltaRelResultPtr->Ndf() != 0. ) {
                      histEtaMeanRel->SetBinContent( uPt + 1, fitEtaPtDeltaRelResultPtr->Parameter( 1 ) );
                      histEtaMeanRel->SetBinError( uPt + 1, fitEtaPtDeltaRelResultPtr->ParError( 1 ) );
                    }
                    else {
                      if ( fitEtaPtDeltaRelResultPtr->Prob() == 0. ) {
                      }
                      else {
                      }
                      if ( verbose_ > 2 ) {
                        std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                                  << "    failing fit in directory '"; gDirectory->pwd();
                        std::cout << "    '" << nameEtaPtDeltaRelRebin << "' status " << fitEtaPtDeltaRelResultPtr->Status() << std::endl;
                      }
                    }
                  }
                  else {
                    if ( verbose_ > 1 ) {
                      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                                << "    missing fit in directory '"; gDirectory->pwd();
                      std::cout << "    '" << nameEtaPtDeltaRelRebin << std::endl;
                    }
                  }

                } // loop: uPt < nPtBins_

              }

            } // loop: keyEta

          } // loop: keyFit

          ++uProp;

        } // loop: keyProp

      }
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
