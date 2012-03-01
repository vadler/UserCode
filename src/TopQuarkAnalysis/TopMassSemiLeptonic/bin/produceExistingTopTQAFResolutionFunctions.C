// #include <cstdlib>
// #include <cmath>
// #include <cassert>
#include <string>
#include <vector>
#include <iostream>

#include "boost/lexical_cast.hpp"
#include <boost/algorithm/string.hpp>

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
  const unsigned                         verbosity_( process_.getParameter< unsigned >( "verbosity" ) );
  const edm::ParameterSet &              exist_( process_.getParameter< edm::ParameterSet >( "existing" ) );
  const std::vector< double >            objLimits_( exist_.getParameter< std::vector< double > >( "objectPtLimits" ) );          // object upper edge of p_t range to display
  const std::vector< std::string >       kinProps_( exist_.getParameter< std::vector< std::string > >( "kinematicProperties" ) ); // kinematic variables
  const std::vector< edm::ParameterSet > resolutions_( process_.getParameter< std::vector< edm::ParameterSet > >( "resolutions" ) );
  const std::string                      outFile_( exist_.getParameter< std::string >( "outputFile" ) );

  // Constants
  const std::string paramBin_( "bin" );

  // Open output file
  TFile * outFile( TFile::Open( outFile_.c_str(), "RECREATE" ) );

  // Loop over resolutions

  for ( unsigned iRes = 0; iRes < resolutions_.size(); ++iRes ) {
    const edm::ParameterSet resolution( resolutions_.at( iRes ) );
    const std::string label( resolution.getParameter< std::string >( "label" ) );
    if ( verbosity_ > 0 ) std::cout << "Resolution set: " << label << std::endl;

    TDirectory * dirRes( outFile->mkdir( label.c_str(), label.c_str() ) );

    const std::vector< edm::ParameterSet > functions( resolution.getParameter< std::vector< edm::ParameterSet > >( "functions" ) );
    for ( unsigned iBin = 0; iBin < functions.size(); ++iBin ) {
      const edm::ParameterSet binFunctions( functions.at( iBin) );
      const std::string binEta( "Eta" + boost::lexical_cast< std::string >( iBin ) );
      std::string titleBin;
      if ( binFunctions.exists( paramBin_ ) ) {
        titleBin = binFunctions.getParameter< std::string >( paramBin_ );
        if ( verbosity_ > 1 ) {
          std::cout << "  bin " << iBin << ": " << titleBin << std::endl;
        }
      }
      else {
        titleBin = "0" ;
        if ( verbosity_ > 1 ) {
          std::cout << "  bin 0: no eta binning" << std::endl;
        }
      }

      for ( unsigned iProp = 0; iProp < kinProps_.size(); ++iProp ) {
        const std::string kinProp( kinProps_.at( iProp ) );
        std::string kinParam( kinProp );
        boost::to_lower( kinParam );
        std::string formula( binFunctions.getParameter< std::string >( kinParam ) );
        if ( verbosity_ > 2 ) {
          std::cout << "    " << kinParam << ": " << formula << std::endl;
        }

        boost::replace_all( formula, "et", "x" );
        std::string name( "fitExist_" + kinProp + "_" + binEta );
        TDirectory * dirProp;
        dirRes->GetObject( kinProp.c_str(), dirProp );
        if ( ! dirProp ) dirProp = dirRes->mkdir( kinProp.c_str(), kinProp.c_str() );
        TDirectory * dirEta( dirProp->mkdir( binEta.c_str(), binEta.c_str() ) );
        dirEta->cd();
        TF1 * func( new TF1( name.c_str(), formula.c_str(), 0., objLimits_.at( iRes ) ) );
        func->Write();
        if ( kinProp == "Et" ) {
          TF1 * funcRel( new TF1( std::string( name + "_Rel" ).c_str(), std::string( "(" + formula + ")/x" ).c_str(), 0., objLimits_.at( iRes ) ) );
          funcRel->Write();
        }
      }
    }

  }

    // Close output file
    outFile->Write();
    outFile->Close();

  return 0;

}
