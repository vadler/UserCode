#include <cstdlib>
#include <cmath>
#include <cassert>
#include <string>
#include <vector>
#include <iostream>

#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TF1.h>

#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include "TopQuarkAnalysis/TopHitFit/interface/EtaDepResolution.h"

#include "TopQuarkAnalysis/TopMassSemiLeptonic/interface/Helpers.h"


int main(  int argc, char * argv[] )
{

  // Load libraries
  gSystem->Load( "libFWCoreFWLite" );
  AutoLibraryLoader::enable();

  // Check configuration file
  if ( argc < 2 ) {
    std::cout << argv[ 0 ] << " --> Usage:" << std::endl
              << "    " << argv[ 0 ] << " [CONFIG_FILE.py]" << std::endl;
    return 0;
  }
  if ( ! edm::readPSetsFrom( argv[ 1 ] )->existsAs< edm::ParameterSet >( "process" ) ) {
    std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
              << "   cms.PSet 'process' missing in " << argv[ 1 ] << std::endl;
    return 1;
  }

  // Get the configurations
  const edm::ParameterSet &        process_( edm::readPSetsFrom( argv[ 1 ] )->getParameter< edm::ParameterSet >( "process" ) );
  const edm::ParameterSet &        exist_( process_.getParameter< edm::ParameterSet >( "existing" ) );
  const std::vector< std::string > objCats_( exist_.getParameter< std::vector< std::string > >( "objectCategories" ) ); // object categories
  const std::vector< double >      objLimits_( exist_.getParameter< std::vector< double > >( "objectPtLimits" ) );      // object upper edge of p_t range to display
  const std::vector< std::string > resFiles_( exist_.getParameter< std::vector< std::string > >( "resolutionFiles" ) ); // "file-in-path" location of input resolution files
  const std::string                resFunc_( exist_.getParameter< std::string >( "resolutionFunction" ) );
  const std::string                resFuncInv_( exist_.getParameter< std::string >( "resolutionFunctionInverse" ) );
  const std::string                outFile_( exist_.getParameter< std::string >( "outputFile" ) );
  assert( objCats_.size() == resFiles_.size() );
  assert( objLimits_.size() == resFiles_.size() );

  // Set constants
  // kinematic quantities
  std::vector< std::string > kinProps_;
  kinProps_.push_back( "Pt" );
  kinProps_.push_back( "Eta" );
  kinProps_.push_back( "Phi" );

  // Configuration for existing resolution functions

  // Open output file
  TFile * outFile( TFile::Open( outFile_.c_str(), "RECREATE" ) );

  // Loop over objects and quantities

  for ( unsigned iCat = 0; iCat < objCats_.size(); ++iCat ) {
    const std::string objCat( objCats_.at( iCat ) );

    outFile->cd();
    gDirectory->pwd();
    TDirectory * curCat( gDirectory->mkdir( objCat.c_str(), objCat.c_str() ) );

    const edm::FileInPath resFile( resFiles_.at( iCat ) );

    if ( objCat == "MET" ) {

      const hitfit::Defaults_Text text( resFile.fullPath() );
      const hitfit::Resolution res( text.get_string( "met_resolution" ) );
      std::string kinProp( kinProps_.at( 0 ) );

      curCat->cd();
      gDirectory->pwd();
      gDirectory->mkdir( kinProp.c_str(), kinProp.c_str() );
      gDirectory->Cd( kinProp.c_str() );
      gDirectory->pwd();

      const std::string name( "fitExist_" + objCat + "_" + kinProp );
      const std::string title( objCat + ", " + kinProp + ( res.inverse() ? ", inv." : "" ) );
      TF1 * func = new TF1( name.c_str(), res.inverse() ? resFuncInv_.c_str() : resFunc_.c_str(), 0., objLimits_.at( iCat ) );
      func->SetParameters( res.C(), res.R(), res.N() );
      func->Write();

    }

    else {

      const hitfit::EtaDepResolution res( resFile.fullPath() );
      const std::vector< hitfit::EtaDepResElement > resElems( res.GetEtaDepResElement() );

      for ( unsigned iResElem = 0; iResElem < resElems.size(); ++iResElem ) {
        const std::string resElem( my::helpers::to_string( iResElem ) );

        const hitfit::Vector_Resolution vecRes( resElems.at( iResElem ).GetResolution() );
        const double etaMin( resElems.at( iResElem ).EtaMin() );
        const double etaMax( resElems.at( iResElem ).EtaMax() );

        for ( unsigned iProp = 0; iProp < kinProps_.size(); ++iProp ) {
          std::string kinProp( kinProps_.at( iProp ) );
          bool inverse( false );
          hitfit::Resolution res;
          if ( kinProp == "Pt" ) {
            res = hitfit::Resolution( vecRes.p_res() );
            if ( res.inverse() ) inverse = true;
          }
          else if ( kinProp == "Eta" ) {
            res = hitfit::Resolution( vecRes.eta_res() );
          }
          else if ( kinProp == "Phi" ) {
            res = hitfit::Resolution( vecRes.phi_res() );
          }
          if ( inverse ) kinProp.append( std::string( "Inv" ) );

          curCat->cd();
          gDirectory->pwd();
          if ( iResElem == 0 ) gDirectory->mkdir( kinProp.c_str(), kinProp.c_str() );
          gDirectory->Cd( kinProp.c_str() );
          gDirectory->pwd();

          const std::string binEta( "Eta" + resElem );
          gDirectory->mkdir( binEta.c_str(), binEta.c_str() );
          gDirectory->Cd( binEta.c_str() );
          gDirectory->pwd();

          const std::string name( "fitExist_" + objCat + "_" + kinProp + "_" + binEta );
          const std::string title( objCat + ", " + kinProp + ", " + my::helpers::to_string( etaMin ) + " #leq #eta #leq " + my::helpers::to_string( etaMax ) + ( inverse ? ", inv." : "" ) );
          TF1 * func = new TF1( name.c_str(), inverse ? resFuncInv_.c_str() : resFunc_.c_str(), 0., objLimits_.at( iCat ) );
          func->SetParameters( res.C(), res.R(), res.N() );
          func->Write();

        }
      }

    }

  }

    // Close output file
    outFile->Write();
    outFile->Close();

  return 0;

}