#include <string>
#include <iostream>

#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TH1D.h>

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
  if ( ! edm::readPSetsFrom( argv[ 1 ] )->existsAs< edm::ParameterSet >( "process" ) ) {
    std::cout << argv[ 0 ] << " --> ERROR:" << std::endl
              << "   cms.PSet 'process' missing in " << argv[ 1 ] << std::endl;
    return 1;
  }

  // Get the configurations
  const edm::ParameterSet &   process_( edm::readPSetsFrom( argv[ 1 ] )->getParameter< edm::ParameterSet >( "process" ) );
  const std::string           outFile_( process_.getParameter< std::string >( "outputFile" ) );
  const std::vector< double > pileUp_( process_.getParameter< std::vector< double > >( "pileUpDistribution" ) );

  // Open output file
  TFile * outFile( TFile::Open( outFile_.c_str(), "RECREATE" ) );

  // Create and fill histogram
  std::vector< double > pileUpBins;
  pileUpBins.push_back( 0. );
  for ( double dBin = 0; dBin < pileUp_.size(); ++dBin ) {
    pileUpBins.push_back( dBin + 1. );
  }
  TH1D * histPileUp( new TH1D( "pileup", "Pile-up", pileUp_.size(), pileUpBins.data() ) );
  for ( unsigned iBin = 0; iBin < pileUp_.size(); ++iBin ) {
    histPileUp->Fill( iBin + 1, pileUp_.at( iBin ) );
  }

  // Close output file
  outFile->Write();
  outFile->Close();

  return 0;

}
