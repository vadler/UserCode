#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <iostream>

#include <TROOT.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>

#include "TopQuarkAnalysis/TopMassSemiLeptonic/interface/Helpers.h"


int main(  int argc, char * argv[] )
{

  // Constants
  std::vector< std::string > evtSel_; // availble analyzer labels
  evtSel_.push_back( "topHitFitResolutionFunctions" );
  evtSel_.push_back( "topHitFitResolutionFunctions_Reference" );
  std::vector< std::string > objCat_; // object categories
  objCat_.push_back( "Lep" );
  objCat_.push_back( "UdscJet" );
  objCat_.push_back( "BJet" );
  objCat_.push_back( "Nu" );
  std::vector< unsigned > etaBins_; // bins per object categorY
  etaBins_.push_back( 50 ); // muons
//   etaBins_.push_back( 48 ); // electrons
  etaBins_.push_back( 54 );
  etaBins_.push_back( 54 );
  etaBins_.push_back( 1 );
  std::vector< std::string > kinProp_; // kinematic quantities
  kinProp_.push_back( "Pt" );
  kinProp_.push_back( "PtInv" );
  kinProp_.push_back( "Eta" );
  kinProp_.push_back( "Phi" );
//   kinProp_.push_back( "Pz" );
//   kinProp_.push_back( "M" );

  // Input file
  TFile * file_( TFile::Open( "file:../output/fitTopHitFitResolutionFunctions_RelValTTbar.root", "UPDATE" ) );

  for ( unsigned iSel = 0; iSel < evtSel_.size(); ++iSel ) {
//   for ( unsigned iSel = 0; iSel < 1; ++iSel ) {
    const std::string evtSel( evtSel_.at( iSel ) );
    file_->cd(); file_->Cd( evtSel.c_str() );
    gDirectory->pwd();
    TDirectory * curSel( gDirectory );
    for ( unsigned iCat = 0; iCat < objCat_.size(); ++iCat ) {
//     for ( unsigned iCat = 0; iCat < 1; ++iCat ) {
      const std::string objCat( objCat_.at( iCat ) );
      for ( unsigned iProp = 0; iProp < kinProp_.size(); ++iProp ) {
//       for ( unsigned iProp = 1; iProp < 2; ++iProp ) {
        const std::string kinProp( kinProp_.at( iProp ) );
        const std::string dirName( "Dir_" + objCat + kinProp );
        curSel->cd(); curSel->Cd( dirName.c_str() );
        gDirectory->pwd();
        for ( unsigned iBin = etaBins_.at( iCat )/4; iBin < etaBins_.at( iCat )*3/4; ++iBin ) {
          std::string name( objCat + "Pt_" + objCat + kinProp + "_" + my::helpers::to_string( iBin ) );
          TH2D * hist2D( ( TH2D* )( gDirectory->Get( name.c_str() ) ) );
          const std::string title( hist2D->GetTitle() );
          hist2D->FitSlicesY( 0, 1, hist2D->GetNbinsX(), 1 );

          TH1D * hist1D( ( TH1D* )( gDirectory->Get( name.append( "_2" ).c_str() ) ) ); // sigmas of the slice fits
          const std::string nameFunc( "Func_" + name );
          const std::string leq( " #leq " );
          unsigned index( title.find( leq ) );
          const double etaMin( std::atof( title.substr( 0, index ).c_str() ) );
          index = title.rfind( leq ) + leq.size();
          const double etaMax( std::atof( title.substr( index ).c_str() ) );
          const double eta( std::fabs(etaMin ) < std::fabs(etaMax ) ? std::fabs(etaMin ) : std::fabs(etaMax ) );
//           const std::string formula( iProp == 1 ? "sqrt(([0]*[0]/x+[1]*[1])/x+[2]*[2])/cosh(" + my::helpers::to_string( eta ) + ")" : "sqrt(([0]*[0]*x+[1]*[1])*x+[2]*[2])/cosh(" + my::helpers::to_string( eta ) + ")" );
          const std::string formula( iProp == 1 ? "sqrt(([0]*[0]/x+[1]*[1])/x+[2]*[2])" : "sqrt(([0]*[0]*x+[1]*[1])*x+[2]*[2])" );
          TF1 * func( new TF1( nameFunc.c_str(), formula.c_str() ) );
          const double minX( hist1D->GetXaxis()->GetXmin() );
          const double maxX( hist1D->GetXaxis()->GetXmax() );
          func->SetRange( minX, maxX );
          hist1D->Fit( func, "QR" );
        }
      }
    }
  }

  file_->Write();
  file_->Close();

  return 0;

}
