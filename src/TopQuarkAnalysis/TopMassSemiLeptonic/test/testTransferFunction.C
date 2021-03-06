#include <cassert>
#include <vector>
#include <iostream>

#include "CommonTools/MyTools/interface/RootFunctions.h"
#include "TopQuarkAnalysis/TopMassSemiLeptonic/interface/TransferFunction.h"


int main( int argc, char * argv[] )
{

  int returnStatus_( 0 );
  std::vector< double > pars_3;
  pars_3.push_back( 1. );
  pars_3.push_back( 1. );
  pars_3.push_back( 1. );
  std::vector< double > pars_5;
  pars_5.push_back( 1. );
  pars_5.push_back( 1. );
  pars_5.push_back( 1. );
  pars_5.push_back( 1. );
  pars_5.push_back( 1. );
  std::vector< double > pars_7;
  pars_7.push_back( 1. );
  pars_7.push_back( 1. );
  pars_7.push_back( 1. );
  pars_7.push_back( 1. );
  pars_7.push_back( 1. );
  pars_7.push_back( 1. );
  pars_7.push_back( 1. );

  my::TransferFunction testFunc0( "0" );
  assert( testFunc0.NParFit()        == 0 );
  assert( testFunc0.NParDependency() == 0 );
  assert( testFunc0.Eval( 0. )       == 0. );
  assert( testFunc0.Eval( 1. )       == 0. );

  my::TransferFunction testFuncGauss1( "gaus", "[0]+[1]*x" );
  assert(   testFuncGauss1.SetParameters( 0, pars_3 ) );
  assert(   testFuncGauss1.SetParameters( 1, pars_3 ) );
  assert( ! testFuncGauss1.SetParameters( 2, pars_3 ) );
  assert( testFuncGauss1.NParFit()        == 3 );
  assert( testFuncGauss1.NParDependency() == 2 );
  assert( testFuncGauss1.Eval( 0., 0. )   == testFuncGauss1.Eval( 0. ) );
  assert( testFuncGauss1.Eval( 0., 1. )   != testFuncGauss1.Eval( 0. ) );
  assert( testFuncGauss1.Eval( 0., 1. )   == 1. );

  my::TransferFunction testFuncGauss2( testFuncGauss1 );
  assert(   testFuncGauss2.SetParameters( 0, pars_3 ) );
  assert(   testFuncGauss2.SetParameters( 1, pars_3 ) );
  assert( ! testFuncGauss2.SetParameters( 2, pars_3 ) );
  assert( testFuncGauss2.NParFit()        == testFuncGauss1.NParFit() );
  assert( testFuncGauss2.NParDependency() == testFuncGauss1.NParDependency() );
  assert( testFuncGauss2.Eval( 0., 0. )   == testFuncGauss1.Eval( 0., 0. ) );
  assert( testFuncGauss2.Eval( 0., 1. )   == testFuncGauss1.Eval( 0., 1. ) );

  TF1 * gauss3( new TF1( "gauss3", "gaus" ) );
  TF1 * line3( new TF1( "line3", "[0]+[1]*x" ) );
  my::TransferFunction testFuncGauss3( gauss3, line3 );
  assert(   testFuncGauss3.SetParameters( 0, pars_3 ) );
  assert(   testFuncGauss3.SetParameters( 1, pars_3 ) );
  assert( ! testFuncGauss3.SetParameters( 2, pars_3 ) );
  assert( testFuncGauss3.NParFit()        == testFuncGauss1.NParFit() );
  assert( testFuncGauss3.NParDependency() == testFuncGauss1.NParDependency() );
  assert( testFuncGauss3.Eval( 0., 0. )   == testFuncGauss1.Eval( 0., 0. ) );
  assert( testFuncGauss3.Eval( 0., 1. )   == testFuncGauss1.Eval( 0., 1. ) );

  my::TransferFunction testFuncGauss4( testFuncGauss3 );
  assert(   testFuncGauss4.SetParameters( 0, pars_3 ) );
  assert(   testFuncGauss4.SetParameters( 1, pars_3 ) );
  assert( ! testFuncGauss4.SetParameters( 2, pars_3 ) );
  assert( testFuncGauss4.NParFit()        == testFuncGauss1.NParFit() );
  assert( testFuncGauss4.NParDependency() == testFuncGauss1.NParDependency() );
  assert( testFuncGauss4.Eval( 0., 0. )   == testFuncGauss1.Eval( 0., 0. ) );
  assert( testFuncGauss4.Eval( 0., 1. )   == testFuncGauss1.Eval( 0., 1. ) );

  my::SingleGaussian * myGauss( new my::SingleGaussian() );
  TF1 * gauss5( new TF1( "gauss5", myGauss, 0., 1., my::SingleGaussian::NPar() ) );
  my::Line * myLine( new my::Line() );
  TF1 * line5( new TF1( "line5", myLine, 0., 1., my::Line::NPar() ) );
  my::TransferFunction testFuncGauss5( gauss5, line5 );
  assert(   testFuncGauss5.SetParameters( 0, pars_3 ) );
  assert(   testFuncGauss5.SetParameters( 1, pars_3 ) );
  assert( ! testFuncGauss5.SetParameters( 2, pars_3 ) );
  assert( ( Int_t )testFuncGauss5.NParFit() == my::SingleGaussian::NPar() );
  assert( testFuncGauss5.NParFit()          == testFuncGauss1.NParFit() );
  assert( testFuncGauss5.NParDependency()   == testFuncGauss1.NParDependency() );
  assert( testFuncGauss5.Eval( 0., 0. )     != testFuncGauss1.Eval( 0., 0. ) );
  assert( testFuncGauss5.Eval( 0., 1. )     != testFuncGauss1.Eval( 0., 1. ) );
  assert( testFuncGauss5.Eval( 0., 0. ) / testFuncGauss5.Eval( 0., 1. ) == testFuncGauss1.Eval( 0., 0. ) / testFuncGauss1.Eval( 0., 1. ) );
  assert( testFuncGauss5.Formula().empty() );

  my::LowerCrystalBall * myCrystalBall( new my::LowerCrystalBall() );
  TF1 * crystal6( new TF1( "crystal6", myCrystalBall, 0., 1., my::LowerCrystalBall::NPar() ) );
  TF1 * line6( new TF1( "line6", myLine, 0., 1., my::Line::NPar() ) );
  my::TransferFunction testFuncCrystal6( crystal6, line6 );
  assert(   testFuncCrystal6.SetParameters( 0, pars_5 ) );
  assert( ! testFuncCrystal6.SetParameters( 0, pars_7 ) );
  assert(   testFuncCrystal6.SetParameters( 1, pars_5 ) );
  assert( ! testFuncCrystal6.SetParameters( 2, pars_5 ) );
  assert( ( Int_t )testFuncCrystal6.NParFit() == my::LowerCrystalBall::NPar() );
  assert( testFuncCrystal6.NParDependency()   == testFuncGauss5.NParDependency() );

  delete myGauss;
  delete myLine;
  delete myCrystalBall;
  delete gauss3;
  delete line3;
  delete gauss5;
  delete line5;
  delete crystal6;
  delete line6;

  std::cout << std::endl << argv[ 0 ] << " --> SUCCESS!" << std::endl << std::endl;

  return returnStatus_;

}
