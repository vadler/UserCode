// #include <cstdlib>
// #include <cmath>
// #include <cassert>
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
  boost::shared_ptr< edm::ParameterSet > parameterSet_( edm::readPSetsFrom( argv[ 1 ] ) );
  if ( ! parameterSet_->existsAs< edm::ParameterSet >( "process" ) ) {
    std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
              << "   cms.PSet 'process' missing in " << argv[ 1 ] << std::endl;
    return 1;
  }

  // Get the configurations
  const edm::ParameterSet &              process_( parameterSet_->getParameter< edm::ParameterSet >( "process" ) );
  const unsigned                         verbosity_( process_.getParameter< bool >( "verbosity" ) );
  const edm::ParameterSet &              exist_( process_.getParameter< edm::ParameterSet >( "existing" ) );
  const std::vector< double >            objLimits_( exist_.getParameter< std::vector< double > >( "objectPtLimits" ) );          // object upper edge of p_t range to display
  const std::vector< std::string >       kinProps_( exist_.getParameter< std::vector< std::string > >( "kinematicProperties" ) ); // kinematic variables
  const std::vector< edm::ParameterSet > resolutions_( process_.getParameter< std::vector< edm::ParameterSet > >( "resolutions" ) );
//   const std::string                resFunc_( exist_.getParameter< std::string >( "resolutionFunction" ) );
//   const std::string                resFuncInv_( exist_.getParameter< std::string >( "resolutionFunctionInverse" ) );
//   const std::string                resFuncRel_( exist_.getParameter< std::string >( "resolutionFunctionRel" ) );
//   const std::string                resFuncInvRel_( exist_.getParameter< std::string >( "resolutionFunctionInverseRel" ) );
//   const std::string                resFuncInvInv_( exist_.getParameter< std::string >( "resolutionFunctionInverseInv" ) );
//   const std::string                resFuncInvInvRel_( exist_.getParameter< std::string >( "resolutionFunctionInverseInvRel" ) );
  const std::string                outFile_( exist_.getParameter< std::string >( "outputFile" ) );

  // Open output file
  TFile * outFile( TFile::Open( outFile_.c_str(), "RECREATE" ) );

  // Loop over resolutions

  for ( unsigned iRes = 0; iRes < resolutions_.size(); ++iRes ) {
    const edm::ParameterSet resolution( resolutions_.at( iRes ) );
    const std::string label( resolution.getParameter< std::string >( "label" ) );
    if ( verbosity_ > 0 ) std::cout << "Found resolution set: " << label << std::endl;

    TDirectory * dirRes( outFile->mkdir( label.c_str(), label.c_str() ) );

    const std::vector< edm::ParameterSet > functions( resolution.getParameter< std::vector< edm::ParameterSet > >( "functions" ) );
    if ( functions.size() == 1 ) { // MET
      const edm::ParameterSet binFunctions( functions.at( 0) );
      const std::string titleEt( binFunctions.getParameter< std::string >( "et" ) );
      const std::string titleEta( binFunctions.getParameter< std::string >( "eta" ) );
      const std::string titlePhi( binFunctions.getParameter< std::string >( "phi" ) );
      if ( verbosity_ > 1 ) {
        std::cout << "  bin: no eta binning" << std::endl;
        std::cout << "    et : " << titleEt << std::endl;
        std::cout << "    eta: " << titleEta << std::endl;
        std::cout << "    phi: " << titlePhi << std::endl;
      }
    }
    else {
      for ( unsigned iBin = 0; iBin < functions.size(); ++iBin ) {
        const edm::ParameterSet binFunctions( functions.at( iBin) );
        const std::string titleBin( binFunctions.getParameter< std::string >( "bin" ) );
        const std::string titleEt( binFunctions.getParameter< std::string >( "et" ) );
        const std::string titleEta( binFunctions.getParameter< std::string >( "eta" ) );
        const std::string titlePhi( binFunctions.getParameter< std::string >( "phi" ) );
        if ( verbosity_ > 1 ) {
          std::cout << "  bin: " << titleBin << std::endl;
          std::cout << "    et : " << titleEt << std::endl;
          std::cout << "    eta: " << titleEta << std::endl;
          std::cout << "    phi: " << titlePhi << std::endl;
        }
      }
    }

  }

    // Close output file
    outFile->Write();
    outFile->Close();

  return 0;

}
