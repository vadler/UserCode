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
#include <TDirectory.h>
#include <TF1.h>

#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include "TopQuarkAnalysis/TopHitFit/interface/EtaDepResolution.h"


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
  const std::vector< std::string > objCats_( exist_.getParameter< std::vector< std::string > >( "objectCategories" ) );     // object categories
  const std::vector< double >      objLimits_( exist_.getParameter< std::vector< double > >( "objectPtLimits" ) );          // object upper edge of p_t range to display
  const std::vector< std::string > resFiles_( exist_.getParameter< std::vector< std::string > >( "resolutionFiles" ) );     // "file-in-path" location of input resolution files
  const std::vector< std::string > kinProps_( exist_.getParameter< std::vector< std::string > >( "kinematicProperties" ) ); // kinematic variables
  const std::string                resFunc_( exist_.getParameter< std::string >( "resolutionFunction" ) );
  const std::string                resFuncInv_( exist_.getParameter< std::string >( "resolutionFunctionInverse" ) );
  const std::string                resFuncRel_( exist_.getParameter< std::string >( "resolutionFunctionRel" ) );
  const std::string                resFuncInvRel_( exist_.getParameter< std::string >( "resolutionFunctionInverseRel" ) );
  const std::string                resFuncInvInv_( exist_.getParameter< std::string >( "resolutionFunctionInverseInv" ) );
  const std::string                resFuncInvInvRel_( exist_.getParameter< std::string >( "resolutionFunctionInverseInvRel" ) );
  const std::string                outFile_( exist_.getParameter< std::string >( "outputFile" ) );
  assert( objCats_.size() == resFiles_.size() );
  assert( objLimits_.size() == resFiles_.size() );

  // Configuration for existing resolution functions

  // Open output file
  TFile * outFile( TFile::Open( outFile_.c_str(), "RECREATE" ) );

  // Loop over objects and quantities

  for ( unsigned iCat = 0; iCat < objCats_.size(); ++iCat ) {
    const std::string objCat( objCats_.at( iCat ) );
    TDirectory * dirCat( outFile->mkdir( objCat.c_str(), objCat.c_str() ) );

    const edm::FileInPath resFile( resFiles_.at( iCat ) );
//     if ( ! resFile ) { // FIXME
//       std::cout << argv[ 0 ] << " --> WARNING:" << std::endl
//                 << "   object category '" << objCat << "' does not exist in input file" << std::endl;
//       continue;
//     }

    if ( objCat == "MET" ) {

      const hitfit::Defaults_Text text( resFile.fullPath() );
      const hitfit::Resolution res( text.get_string( "met_resolution" ) );
      std::string kinProp( kinProps_.at( 0 ) );

      TDirectory * dirProp( dirCat->mkdir( kinProp.c_str(), kinProp.c_str() ) );
      dirProp->cd();

      const std::string name( "fitExist_" + objCat + "_" + kinProp );
      TF1 * func( new TF1( name.c_str(), res.inverse() ? resFuncInv_.c_str() : resFunc_.c_str(), 0., objLimits_.at( iCat ) ) );
      func->SetParameters( res.C(), res.R(), res.N() );
      func->Write();
      TF1 * funcRel( new TF1( std::string( name + "_Rel" ).c_str(), res.inverse() ? resFuncInvInvRel_.c_str() : resFuncRel_.c_str(), 0., objLimits_.at( iCat ) ) );
      funcRel->SetParameters( res.C(), res.R(), res.N() );
      funcRel->Write();

    }

    else {

      const hitfit::EtaDepResolution res( resFile.fullPath() );
      const std::vector< hitfit::EtaDepResElement > resElems( res.GetEtaDepResElement() );

      for ( unsigned iResElem = 0; iResElem < resElems.size(); ++iResElem ) {
        const std::string binEta( "Eta" + boost::lexical_cast< std::string >( iResElem ) );

        const hitfit::Vector_Resolution vecRes( resElems.at( iResElem ).GetResolution() );

        for ( unsigned iProp = 0; iProp < kinProps_.size(); ++iProp ) {
          std::string kinProp( kinProps_.at( iProp ) );
          hitfit::Resolution res;
          if ( kinProp == "Pt" ) {
            res = hitfit::Resolution( vecRes.p_res() );
          }
          else if ( kinProp == "Eta" ) {
            res = hitfit::Resolution( vecRes.eta_res() );
          }
          else if ( kinProp == "Phi" ) {
            res = hitfit::Resolution( vecRes.phi_res() );
          }
          TDirectory * dirProp;
          if ( iResElem == 0 ) dirProp = dirCat->mkdir( kinProp.c_str(), kinProp.c_str() );
          else                 dirProp = ( TDirectory* )( dirCat->Get( kinProp.c_str() ) );
          dirProp->cd();

          const std::string inverse( res.inverse() ? "_Inv" : "" );
          const std::string name( "fitExist_" + objCat + inverse + "_" + kinProp + "_" + binEta );
          TF1 * func( new TF1( name.c_str(), res.inverse() ? resFuncInv_.c_str() : resFunc_.c_str(), 0., objLimits_.at( iCat ) ) );
          func->SetParameters( res.C(), res.R(), res.N() );
          func->Write();
          if ( kinProp == "Pt" ) {
            TF1 * funcRel( new TF1( std::string( name + "_Rel" ).c_str(), res.inverse() ? resFuncInvRel_.c_str() : resFuncRel_.c_str(), 0., objLimits_.at( iCat ) ) );
            funcRel->SetParameters( res.C(), res.R(), res.N() );
            funcRel->Write();
            if ( res.inverse() ) {
              TF1 * funcInv( new TF1( std::string( name + "_Inv" ).c_str(), resFuncInvInv_.c_str(), 0., objLimits_.at( iCat ) ) );
              funcInv->SetParameters( res.C(), res.R(), res.N() );
              funcInv->Write();
              TF1 * funcInvRel( new TF1( std::string( name + "_InvRel" ).c_str(), resFuncInvInvRel_.c_str(), 0., objLimits_.at( iCat ) ) );
              funcInvRel->SetParameters( res.C(), res.R(), res.N() );
              funcInvRel->Write();
            }
          }

        }
      }

    }

  }

    // Close output file
    outFile->Write();
    outFile->Close();

  return 0;

}
