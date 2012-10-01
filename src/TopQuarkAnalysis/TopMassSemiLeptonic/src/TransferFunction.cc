//
// $Id:$
//


#include "TopQuarkAnalysis/TopMassSemiLeptonic/interface/TransferFunction.h"

#include <sstream>
#include "boost/lexical_cast.hpp"


using namespace my;


// Constructors and Destructor

// Constructor from TransferFunction (copy c'tor)
TransferFunction::TransferFunction( const TransferFunction & transfer )
: initConst_( -999999. )
, fitFunction_( "fitFunction", transfer.FitFunction().c_str() )
, dependencyFunction_( "dependencyFunction", transfer.DependencyFunction().c_str() )
, dependency_( transfer.Dependency() )
, comment_( transfer.Comment() )
{
  ClearParameters();
  pars1D_ = transfer.Parameters1D();
  pars2D_ = transfer.Parameters2D();
}

// Constructor from strings
TransferFunction::TransferFunction( const std::string & fitFunction, const std::string & dependencyFunction, const std::string & dependency )
: initConst_( -999999. )
, fitFunction_( "fitFunction", fitFunction.c_str() )
, dependencyFunction_( "dependencyFunction", dependencyFunction.c_str() )
, dependency_( dependency )
, comment_()
{
  ClearParameters();
}


// Methods

// Setters

void TransferFunction::SetFitFunction( const std::string & fitFunction, bool clear )
{
  fitFunction_ = TFormula( "fitFunction", fitFunction.c_str() );
  clear ? ClearParameters() : ResizeParameters();
}

void TransferFunction::SetDependencyFunction( const std::string & dependencyFunction, bool clear )
{
  dependencyFunction_ = TFormula( "dependencyFunction", dependencyFunction.c_str() );
  clear ? ClearParameters() : ResizeParameters();
}

bool TransferFunction::SetParameter( double par, size_t i, int j )
{
  if ( i >= NParFit() ) return false;
  if ( j < 0 ) {
    pars1D_.at( i ) = par;
    return true;
  }
  if ( j >= ( int )NParDependency() ) return false;
  pars2D_.at( j ).at( i ) = par;
  return true;
}

bool TransferFunction::SetParameters( std::vector< double > pars, int j )
{
  if ( j < 0 ) {
    if ( pars.size() != NParFit() ) return false;
    pars1D_ = pars;
    return true;
  }
  if ( j >= ( int )NParDependency() ) return false;
  if ( pars.size() != NParFit() ) return false;
  pars2D_.at( j ) = pars;
  return true;
}

bool TransferFunction::SetParameters( std::vector< std::vector< double > > pars )
{
  if ( pars.size() != NParDependency() ) return false;
  for ( unsigned j = 0; j < NParDependency(); ++j ) {
    if ( pars.at( j ).size() != NParFit() ) {
      pars2D_.clear();
      pars2D_.resize( dependencyFunction_.GetNpar(), std::vector< double >( fitFunction_.GetNpar() ) );
      return false;
    }
    pars2D_.at( j ) = pars.at( j );
  }
  return true;
}

void TransferFunction::ClearParameters()
{
  pars1D_.clear();
  pars2D_.clear();
  ResizeParameters();
  for ( size_t i = 0; i < NParFit(); ++i ) {
    pars1D_.at( i ) = initConst_;
    for ( size_t j = 0; j < NParDependency(); ++j ) {
      pars2D_.at( j ).at( i ) = initConst_;
    }
  }
}

void TransferFunction::ResizeParameters()
{
  pars1D_.resize( fitFunction_.GetNpar() );
  pars2D_.resize( dependencyFunction_.GetNpar(), std::vector< double >( fitFunction_.GetNpar() ) );
}

// Evaluate

std::string TransferFunction::Formula( double dependencyValue ) const
{
  TFormula fitFunc( fitFunction_ );
  TFormula depFunc( dependencyFunction_ );
  for ( size_t i = 0; i < NParFit(); ++i ) {
    for ( size_t j = 0; j < NParDependency(); ++j ) {
      depFunc.SetParameter( ( Int_t )j, ( Double_t )( pars2D_.at( j ).at( i ) ) );
    }
    fitFunc.SetParameter( ( Int_t )i, ( Double_t )( depFunc.Eval( dependencyValue ) ) );
  }
  return std::string( fitFunc.GetExpFormula( "p" ).Data() );
}

double TransferFunction::Eval( double dependencyValue, double value ) const
{
  TFormula fitFunc( fitFunction_ );
  TFormula depFunc( dependencyFunction_ );
  for ( size_t i = 0; i < NParFit(); ++i ) {
    for ( size_t j = 0; j < NParDependency(); ++j ) {
      depFunc.SetParameter( ( Int_t )j, ( Double_t )( pars2D_.at( j ).at( i ) ) );
    }
    fitFunc.SetParameter( ( Int_t )i, ( Double_t )( depFunc.Eval( dependencyValue ) ) );
  }
  return fitFunc.Eval( value );
}

std::string TransferFunction::Print() const
{
  std::stringstream print( std::ios_base::out );
  print << std::endl;
  print << "Formula: \t" << FitFunction() << std::endl;
  print << "Comment: \t" << Comment() << std::endl;
  print << std::endl;
  print << "Parameters 1D:" << std::endl;
  for ( size_t i = 0; i < NParFit(); ++i ) {
    print << "[" << i << "]: \t";
    if ( pars1D_.at( i ) == initConst_ ) print << "NAN";
    else print << pars1D_.at( i );
    print << std::endl;
  }
  print << std::endl;
  print << "Parameters 2D:" << std::endl;
  for ( size_t i = 0; i < NParFit(); ++i ) {
    TFormula depFunc( dependencyFunction_ );
    for ( size_t j = 0; j < NParDependency(); ++j ) {
      depFunc.SetParameter( ( Int_t )j, ( Double_t )( pars2D_.at( j ).at( i ) ) );
    }
    TString str( depFunc.GetExpFormula( "p" ) );
    str.ReplaceAll( "x", dependency_.c_str() );
    str.ReplaceAll( boost::lexical_cast< std::string >( initConst_ ).c_str(), "NAN" );
    print << "[" << i << "]: \t";
    print << str << std::endl;
  }
  return print.str();
}

TransferFunction TransferFunction::Read( const std::string & stream )
{
  TransferFunction transfer;
  return transfer;
}
