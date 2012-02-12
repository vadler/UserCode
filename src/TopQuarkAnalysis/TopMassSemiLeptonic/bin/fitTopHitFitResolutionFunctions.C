#include <cstdlib>
#include <cmath>
#include <cassert>
#include <string>
#include <vector>
#include <iostream>

#include "boost/lexical_cast.hpp"

#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TKey.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>

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
              << "   cms.PSet 'process' missing in " << argv[ 1 ] << std::endl;
    returnStatus_ += 0x2;
    return returnStatus_;
  }

  // Get the configurations
  const edm::ParameterSet & process_( edm::readPSetsFrom( argv[ 1 ] )->getParameter< edm::ParameterSet >( "process" ) );
  const unsigned verbose_( process_.getParameter< unsigned >( "verbose" ) );
  const std::vector< std::string > objCats_( process_.getParameter< std::vector< std::string > >( "objectCategories" ) );   // object categories
  const bool refGen_(  process_.getParameter< bool >( "refGen" ));
  const bool useSymm_(  process_.getParameter< bool >( "useSymm" ));
  // Configuration for fitting new resolution functions
  const edm::ParameterSet & fitter_( process_.getParameter< edm::ParameterSet >( "fitter" ) );
  const std::string inFile_( fitter_.getParameter< std::string >( "inputFile" ) );                                          // input file with RECO-GEN distributions
  const std::string evtSel_( fitter_.getParameter< std::string >( "selection" ) );
  const std::string resFunc_( fitter_.getParameter< std::string >( "resolutionFunction" ) );
  const std::string resFuncInv_( fitter_.getParameter< std::string >( "resolutionFunctionInverse" ) );
  // Configuration for existing resolution functions
  const bool useExisting_( process_.getParameter< bool >( "useExisting" ) );

    if ( verbose_ > 0 )
      std::cout << std::endl
                << argv[ 0 ] << " --> INFO:" << std::endl
                << "   called for object categories ";
      for ( unsigned iCat = 0; iCat < objCats_.size() - 1; ++iCat )
        std::cout << "'" << objCats_.at( iCat ) << "', ";
      std::cout << "'" << objCats_.back() << "'" << std::endl;

  // Set constants
  const std::string dirClassName_( "TDirectoryFile" );
  const std::string funcClassName_( "TF1" );
  std::string fitOptions_( "MR" );
  if      ( verbose_ < 2 ) fitOptions_.append( "Q" );
  else if ( verbose_ > 3 ) fitOptions_.append( "V" );


  // Get existing resolution functions

  if ( useExisting_ ) {

    // Configuration for existing resolution functions
    const edm::ParameterSet & exist_( process_.getParameter< edm::ParameterSet >( "existing" ) );
    const std::string resolutionFile_( exist_.getParameter< std::string >( "resolutionFile" ) );

    if ( verbose_ > 0 )
      std::cout << std::endl
                << argv[ 0 ] << " --> INFO:" << std::endl
                << "   accessing existing resolution functions from resolution file '" << resolutionFile_ << "'" << std::endl;

    // Open output file
    TFile * resolutionFile( TFile::Open( resolutionFile_.c_str() ) );
    if ( resolutionFile ) {
      if ( verbose_ > 2 ) gDirectory->pwd();

      // Loop over objects and quantities

      for ( unsigned iCat = 0; iCat < objCats_.size(); ++iCat ) {
        const std::string objCat( objCats_.at( iCat ) );
        TDirectory * dirCat( ( TDirectory* )( resolutionFile->Get( objCat.c_str() ) ) );
        if ( ! dirCat ) {
          std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                    << "   object category '" << objCat << "' does not exist in resolution file" << std::endl;
          continue;
        }

        TList * listCat( dirCat->GetListOfKeys() );
        if ( verbose_ > 3 ) listCat->Print();
        TIter nextInListCat( listCat );
        while ( TKey * keyProp = ( TKey* )nextInListCat() ) {
          if ( std::string( keyProp->GetClassName() ) != dirClassName_ ) continue;
          const std::string kinProp( keyProp->GetName() );
          TDirectory * dirProp( ( TDirectory* )( dirCat->Get( kinProp.c_str() ) ) );

          TList * listProp( dirProp->GetListOfKeys() );
          if ( verbose_ > 3 ) listProp->Print();
          TIter nextInListProp( listProp );
          while ( TKey * keyEta = ( TKey* )nextInListProp() ) {
            if ( std::string( keyEta->GetClassName() ) != funcClassName_ ) continue;
            const std::string name( keyEta->GetName() );
            if ( name.find( "InvFake" ) != std::string::npos ) continue;

//             TF1 * func( ( TF1* )( dirProp->Get( name.c_str() ) ) );

          } // loop: nextInListProp()

        } // loop: nextInListCat()

      } // loop: iCat < objCats_.size()

      // Close output file
      resolutionFile->Close();

    } // ( resolutionFile )

    else {
      std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
                << "   resolution file '" << resolutionFile_ << "' missing" << std::endl;
      returnStatus_ += 0x100;
    } // !( resolutionFile )

  } // ( useExisting_ )


  // Fit the new resolution functions

  // Open input file
  TFile * inFile( TFile::Open( inFile_.c_str(), "UPDATE" ) );
  if ( ! inFile ) {
    std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
              << "   input file '" << inFile_ << "' missing" << std::endl;
    returnStatus_ += 0x10;
    return returnStatus_;
  }
  TDirectory * dirSel = ( TDirectory* )( inFile->Get( evtSel_.c_str() ) );

  // Loops through directory structure

  if ( verbose_ > 0 )
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "   fitting resolution functions from input file '" << inFile_ << "'" << std::endl;

  typedef std::vector< std::vector< Double_t > > dataCont;

  // Loop over configured object categories
  for ( unsigned iCat = 0; iCat < objCats_.size(); ++iCat ) {
    const std::string objCat( objCats_.at( iCat ) );
    TDirectory * dirCat( ( TDirectory* )( dirSel->Get( objCat.c_str() ) ) );
    if ( ! dirCat ) {
      std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                << "   object category '" << objCat << "' does not exist in input file" << std::endl;
      continue;
    }

    // Get binning per object category

    // Eta binning
    std::vector< double > etaBins_;
    TH1D * hist_EtaBins( ( TH1D* )( dirCat->Get( std::string( objCat + "_binsEta" ).c_str() ) ) );
    for ( int iEta = 0; iEta < hist_EtaBins->GetNbinsX(); ++iEta ) {
      etaBins_.push_back( hist_EtaBins->GetBinLowEdge( iEta + 1 ) );
    }
    etaBins_.push_back( hist_EtaBins->GetBinLowEdge( hist_EtaBins->GetNbinsX() ) + hist_EtaBins->GetBinWidth( hist_EtaBins->GetNbinsX() ) );

    // Pt binning
    std::vector< double > ptBins_;
    TH1D * hist_PtBins( ( TH1D* )( dirCat->Get( std::string( objCat + "_binsPt" ).c_str() ) ) );
    for ( int iPt = 0; iPt < hist_PtBins->GetNbinsX(); ++iPt ) {
      ptBins_.push_back( hist_PtBins->GetBinLowEdge( iPt + 1 ) );
    }
    ptBins_.push_back( hist_PtBins->GetBinLowEdge( hist_PtBins->GetNbinsX() ) + hist_PtBins->GetBinWidth( hist_PtBins->GetNbinsX() ) );

    // Read general n-tuple data
    dataCont momEData_( etaBins_.size() - 1 );
    dataCont momEGenData_( etaBins_.size() - 1 );
    Double_t momEData, momEGenData;
    Int_t    binEta;
    TTree * data_( ( TTree* )( dirCat->Get( std::string( objCat + "_data" ).c_str() ) ) );
    data_->SetBranchAddress( "MomE", &momEData );
    data_->SetBranchAddress( "MomEGen", &momEGenData );
    if ( useSymm_ )
      if ( refGen_ ) data_->SetBranchAddress( "BinEtaSymmGen", &binEta );
      else           data_->SetBranchAddress( "BinEtaSymm", &binEta );
    else
      if ( refGen_ ) data_->SetBranchAddress( "BinEtaGen", &binEta );
      else           data_->SetBranchAddress( "BinEta", &binEta );
    Int_t nEntries( ( Int_t )data_->GetEntries() );
    std::vector< unsigned > sizeEta_( etaBins_.size() - 1 );
    for ( Int_t iEntry = 0; iEntry < nEntries; ++iEntry ) {
      data_->GetEntry( iEntry );
      assert( binEta < ( Int_t )( etaBins_.size() - 1 ) ); // has to fit (and be consistent)
      if ( binEta == -1 ) continue; // FIXME: eta out of range in analyzer; should be solved more consistently
      sizeEta_.at( binEta ) += 1;
      momEData_.at( binEta ).push_back( momEData );
      momEGenData_.at( binEta ).push_back( momEGenData );
    }

    // Loop over kinematic properties
    TList * listCat( dirCat->GetListOfKeys() );
    if ( verbose_ > 3 ) listCat->Print();
    TIter nextInListCat( listCat );
    while ( TKey * keyProp = ( TKey* )nextInListCat() ) {
      if ( std::string( keyProp->GetClassName() ) != dirClassName_ ) continue;
      const std::string kinProp( keyProp->GetName() );
      TDirectory * dirProp( ( TDirectory* )( dirCat->Get( kinProp.c_str() ) ) );

      // Read kinematic property n-tuple data
      dataCont propData_( etaBins_.size() - 1 );
      dataCont propGenData_( etaBins_.size() - 1 );
      Double_t propData, propGenData;
      std::string kinBranch;
      if ( kinProp == "Pt" ) kinBranch = "MomE"; //FIXME: in analyzer
      else                   kinBranch = kinProp;
      data_->SetBranchAddress( kinBranch.c_str(), &propData );
      data_->SetBranchAddress( std::string( kinBranch + "Gen" ).c_str(), &propGenData );
      for ( Int_t iEntry = 0; iEntry < nEntries; ++iEntry ) {
        data_->GetEntry( iEntry );
        if ( binEta == -1 ) continue; // FIXME: eta out of range in analyzer; should be solved more consistently
        propData_.at( binEta ).push_back( propData );
        propGenData_.at( binEta ).push_back( propGenData );
      }

      // Loop over fit versions
      TList * listProp( dirProp->GetListOfKeys() );
      if ( verbose_ > 3 ) listProp->Print();
      TIter nextInListProp( listProp );
      while ( TKey * keyFit = ( TKey* )nextInListProp() ) {
        if ( std::string( keyFit->GetClassName() ) != dirClassName_ ) continue;
        const std::string subFit( keyFit->GetName() );
        TDirectory * dirFit = ( TDirectory* )( dirProp->Get( subFit.c_str() ) );

        // Inversion flag from directory name
        const bool inverse( subFit.substr( subFit.size() - 3 ) == "Inv" );

        // Loop over eta bins
        TList * listFit( dirFit->GetListOfKeys() );
        if ( verbose_ > 3 ) listFit->Print();
        if ( listFit->GetSize() != ( Int_t )( etaBins_.size() - 1 ) ) {
          std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
                    << "   mismatch of eta binning for object category '" << objCat << "':" << std::endl
                    << "       bins in directory structure: " << listFit->GetSize()         << std::endl
                    << "       bins in binning histogram  : " << etaBins_.size() - 1         << std::endl;
        }
        TIter nextInListFit( listFit );
        while ( TKey * keyEta = ( TKey* )nextInListFit() ) {
          if ( std::string( keyEta->GetClassName() ) != dirClassName_ ) continue;
          const std::string binEta( keyEta->GetName() );
          const unsigned iEta( std::atoi( binEta.substr( 3 ).data() ) );
          dirFit->cd( binEta.c_str() );
          if ( verbose_ > 1 ) gDirectory->pwd();

          // Do the fits

          const std::string name( objCat + "_" + kinProp + "_" + subFit + "_" + binEta );

          // Direct fit in slices of the 2-dim histogram
          TH2D * hist2D( ( TH2D* )( gDirectory->Get( name.c_str() ) ) );
          hist2D->FitSlicesY( 0, 1, hist2D->GetNbinsX(), 1 );
          TH1D * histSigma( ( TH1D* )( gDirectory->Get( std::string( name + "_2" ).c_str() ) ) ); // sigmas of the slice fits
          const std::string nameFunc( "fit_" + name );
          const std::string formula( inverse ? resFuncInv_ : resFunc_ );
          TF1 * func( new TF1( nameFunc.c_str(), formula.c_str() ) );
          func->SetRange( histSigma->GetXaxis()->GetXmin(), histSigma->GetXaxis()->GetXmax() );
          histSigma->Fit( func, fitOptions_.c_str() );

          // FIXME: can be removed soon
          // Extract individual 1-dim histograms per p_t-bin and fit
          const std::string nameSigma( name + "_Sigma" );
          TH1D * histSigmaPt( new TH1D( *( ( TH1D* )( histSigma->Clone( nameSigma.c_str() ) ) ) ) );
          histSigmaPt->Reset( "ICE" ); // emoty the contents to use it as "template" only
          for ( unsigned iPt = 0; iPt < ptBins_.size() - 1; ++iPt ) {
            const std::string binPt( boost::lexical_cast< std::string >( iPt ) );

            const std::string namePt( name + "_Pt_" + binPt );
            const std::string titlePt( std::string( hist2D->GetTitle() ) + ", " + boost::lexical_cast< std::string >( hist2D->GetXaxis()->GetBinLowEdge( iPt + 1 ) ) + " GeV #leq p_{t} < " + boost::lexical_cast< std::string >( hist2D->GetXaxis()->GetBinUpEdge( iPt + 1 ) ) + " GeV" );
            const Int_t nBins( hist2D->GetNbinsY() );
            TH1D * hist1DPt( new TH1D( namePt.c_str(), titlePt.c_str(), nBins, hist2D->GetYaxis()->GetXmin(), hist2D->GetYaxis()->GetXmax() ) );
            hist1DPt->SetXTitle( hist2D->GetYaxis()->GetTitle() );
            hist1DPt->SetYTitle( hist2D->GetZaxis()->GetTitle() );
            for ( Int_t iBin = 0; iBin < nBins; ++iBin ) {
              hist1DPt->SetBinContent( iBin, hist2D->GetBinContent( iPt + 1, iBin + 1 ) ); // skip underflow bin
            }

            const std::string nameFuncGauss( "gauss_" + namePt );
            TF1 * funcGauss( new TF1( nameFuncGauss.c_str(), "gaus", hist1DPt->GetXaxis()->GetXmin(), hist1DPt->GetXaxis()->GetXmax() ) );
            Int_t fitStatus( hist1DPt->Fit( funcGauss, "QR" ) );
            TF1 * funcFit( hist1DPt->GetFunction( nameFuncGauss.c_str() ) );
            if ( fitStatus == 0 ) {
              histSigmaPt->SetBinContent( iPt + 1, funcFit->GetParameter( 2 ) );
              histSigmaPt->SetBinError( iPt + 1, funcFit->GetParError( 2 ) );
            }
          } // loop: iPt < ptBins_.size() - 1
          const std::string nameFuncSigma( "fit_" + nameSigma );
          TF1 * funcSigma( new TF1( nameFuncSigma.c_str(), formula.c_str() ) );
          funcSigma->SetRange( histSigmaPt->GetXaxis()->GetXmin(), histSigmaPt->GetXaxis()->GetXmax() );
          histSigmaPt->Fit( funcSigma, fitOptions_.c_str() );

          // Create new 1-dim histograms from n-tuple
          const std::string nameSigmaNTup( name + "NTup_Sigma" );
          TH1D * histSigmaPtNTup( new TH1D( *( ( TH1D* )( histSigma->Clone( nameSigmaNTup.c_str() ) ) ) ) );
          histSigmaPtNTup->Reset( "ICE" ); // emoty the contents to use it as "template" only     dataCont momEData_( etaBins_.size() - 1 );
          // Split data into p_t bins
          dataCont momEEtaBin( ptBins_.size() - 1 );
          dataCont propEtaBin( ptBins_.size() - 1 );
          dataCont propGenEtaBin( ptBins_.size() - 1 );
          std::vector< unsigned > sizePt( ptBins_.size() - 1 );
          for ( unsigned iEntryEtaBin = 0; iEntryEtaBin < sizeEta_.at( iEta ); ++iEntryEtaBin ) {
            for ( unsigned iPt = 0; iPt < ptBins_.size() - 1; ++iPt ) {
              if ( ptBins_.at( iPt ) <= momEData_.at( iEta ).at( iEntryEtaBin ) && momEData_.at( iEta ).at( iEntryEtaBin ) < ptBins_.at( iPt + 1 ) ) {
                sizePt.at( iPt ) += 1;
                momEEtaBin.at( iPt ).push_back( momEData_.at( iEta ).at( iEntryEtaBin ) );
                propEtaBin.at( iPt ).push_back( propData_.at( iEta ).at( iEntryEtaBin ) );
                propGenEtaBin.at( iPt ).push_back( propGenData_.at( iEta ).at( iEntryEtaBin ) );
                break;
              }
            } // loop: iPt < ptBins_.size() - 1
          } // loop: iEntryEtaBin < nEntries
          for ( unsigned iPt = 0; iPt < ptBins_.size() - 1; ++iPt ) {
            if ( sizePt.at( iPt ) == 0 ) {
              if ( verbose_ > 2 ) {
                std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                          << "   empty bin in '" << name << "'" << std::endl
                          << "       for p_t bin" << iPt  << std::endl;
              }
              continue;
            }
            const std::string binPt( boost::lexical_cast< std::string >( iPt ) );

            const std::string namePt( name + "_Pt_" + binPt );
            TH1D * hist1DPt( ( TH1D* )( gDirectory->Get( namePt.c_str() ) ) );
            const std::string namePtNTup( name + "_PtNTup_" + binPt );
//             const std::string titlePtNTup( std::string( hist2D->GetTitle() ) + ", " + boost::lexical_cast< std::string >( hist2D->GetXaxis()->GetBinLowEdge( iPt + 1 ) ) + " GeV #leq p_{t} < " + boost::lexical_cast< std::string >( hist2D->GetXaxis()->GetBinUpEdge( iPt + 1 ) ) + " GeV" );
//             const Int_t nBinsNTup( hist2D->GetNbinsY() ); // FIXME: tune number of bins,
//             const Double_t sigma( std::fabs( histSigma->GetBinContent( iPt + 1 ) ) );
            const std::string titlePtNTup( hist1DPt->GetTitle() );
            const Int_t nBinsNTup( hist1DPt->GetNbinsX() ); // FIXME: tune number of bins,
            const Double_t sigma( std::fabs( histSigma->GetBinContent( iPt + 1 ) ) );
            const Double_t width( sigma == 0. ? std::fabs( hist1DPt->GetRMS() ) : sigma ); // FIXME: Hmm...
            if ( width == 0. && verbose_ > 2 ) {
              std::cout << argv[ 0 ] << " --> INFO:" << std::endl
                        << "   no sigma nor RMS in '" << name << "'" << std::endl
                        << "       for p_t bin" << iPt  << std::endl;
            }
            const Double_t range( width == 0. ? 3. * std::fabs( hist1DPt->GetXaxis()->GetXmax() ) : 3. * width ); // FIXME: tune, incl. under- and overflow, remove hard-coding
            TH1D * hist1DPtNTup( new TH1D( namePtNTup.c_str(), titlePtNTup.c_str(), nBinsNTup, -range, range ) );
            hist1DPtNTup->SetXTitle( hist1DPt->GetXaxis()->GetTitle() );
            hist1DPtNTup->SetYTitle( hist1DPt->GetYaxis()->GetTitle() );
            for ( unsigned iEntryEtaBin = 0; iEntryEtaBin < sizePt.at( iPt ); ++iEntryEtaBin ) {
              if ( inverse ) hist1DPtNTup->Fill( 1. / propEtaBin.at( iPt ).at( iEntryEtaBin ) - 1. / propGenEtaBin.at( iPt ).at( iEntryEtaBin ) );
              else           hist1DPtNTup->Fill( propEtaBin.at( iPt ).at( iEntryEtaBin ) - propGenEtaBin.at( iPt ).at( iEntryEtaBin ) );
            }

            const std::string nameFuncGaussNTup( "gauss_" + namePtNTup );
            TF1 * funcGaussNTup( new TF1( nameFuncGaussNTup.c_str(), "gaus", hist1DPtNTup->GetXaxis()->GetXmin(), hist1DPtNTup->GetXaxis()->GetXmax() ) );
            Int_t fitStatusNTup( hist1DPtNTup->Fit( funcGaussNTup, "QR" ) );
            TF1 * funcFitNTup( hist1DPtNTup->GetFunction( nameFuncGaussNTup.c_str() ) );
            if ( fitStatusNTup == 0 ) {
              histSigmaPtNTup->SetBinContent( iPt + 1, funcFitNTup->GetParameter( 2 ) );
              histSigmaPtNTup->SetBinError( iPt + 1, funcFitNTup->GetParError( 2 ) );
            }  // loop: iPt < ptBins_.size() - 1
          }  // loop: iEntryEtaBin < sizePt.at( iPt )
          const std::string nameFuncSigmaNTup( "fit_" + nameSigmaNTup );
          TF1 * funcSigmaNTup( new TF1( nameFuncSigmaNTup.c_str(), formula.c_str() ) );
          funcSigmaNTup->SetRange( histSigmaPtNTup->GetXaxis()->GetXmin(), histSigmaPtNTup->GetXaxis()->GetXmax() );
          histSigmaPtNTup->Fit( funcSigmaNTup, fitOptions_.c_str() );

        } // loop: keyEta

      } // loop: keyFit

    } // loop: keyProp

  } // loop: iCat < objCats_.size()

  // Write and close input file
  inFile->Write();
  inFile->Close();

  if ( verbose_ > 0 )
    std::cout << std::endl
              << argv[ 0 ] << " --> INFO:" << std::endl
              << "   return status " << returnStatus_ << std::endl;
  return returnStatus_;

}
