#include <string>
#include <vector>
#include <iostream>

#include <TROOT.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>


int main(  int argc, char * argv[] )
{

  // Input file
  TFile * fileIn( TFile::Open( "file:output/TopHitFitAnalyzer_RelValTTbar.root" ) );

  fileIn->Close();

  // Ooutput file
  TFile fileOut( "output/plotTopHitFitAnalyzer_RelValTTbar.root" );

  fileOut.Close();

  return 0;

}
