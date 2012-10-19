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
: fitFunction_( "fitFunction", transfer.FitFunction().c_str() )
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
: fitFunction_( "fitFunction", fitFunction.c_str() )
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

bool TransferFunction::SetParameter( size_t i, double par )
{
  if ( i < NParFit() ) {
    pars1D_.at( i ) = par;
    return true;
  }
  return false;
}

bool TransferFunction::SetParameter( size_t i, size_t j, double par )
{
  if ( i < NParFit() && j < NParDependency() ) {
    pars2D_.at( j ).at( i ) = par;
    return true;
  }
  return false;
}

bool TransferFunction::SetParameters( std::vector< double > pars )
{
  if ( pars.size() == NParFit() ) {
    pars1D_ = pars;
    return true;
  }
  return false;
}

bool TransferFunction::SetParameters( size_t j, std::vector< double > pars )
{
  if ( j < NParDependency() && pars.size() == NParFit() ) {
    pars2D_.at( j ) = pars;
    return true;
  }
  return false;
}

void TransferFunction::ClearParameters()
{
  pars1D_.clear();
  pars2D_.clear();
  ResizeParameters();
  for ( size_t i = 0; i < NParFit(); ++i ) {
    pars1D_.at( i ) = transferFunctionInitConst;
    for ( size_t j = 0; j < NParDependency(); ++j ) {
      pars2D_.at( j ).at( i ) = transferFunctionInitConst;
    }
  }
}

void TransferFunction::ResizeParameters()
{
  pars1D_.resize( fitFunction_.GetNpar() );
  pars2D_.resize( dependencyFunction_.GetNpar(), std::vector< double >( fitFunction_.GetNpar() ) );
}

// Getters

double TransferFunction::Parameter( size_t i, size_t j ) const
{
  if ( i < NParFit() && j < NParDependency() ) return Parameters2D().at( j ).at( i );
  return transferFunctionInitConst;
}

std::vector< double > TransferFunction::Parameters( size_t j ) const
{
  if ( j < NParDependency() ) return Parameters2D().at( j );
  return std::vector< double >();
}

// Evaluate

std::string TransferFunction::Formula() const
{
  TString fitStr( FitFunction() );
  for ( size_t i = 0; i < NParFit(); ++i ) {
    TString depStr( PrintDependency( i, false ) );
    depStr.ReplaceAll( Dependency(), "y" );
    depStr.Prepend( "(" );
    depStr.Append( ")" );
    TString parStr( "[" + boost::lexical_cast< std::string >( i ) + "]" );
    fitStr.ReplaceAll( parStr, depStr );
  }
  return std::string( fitStr.Data() );
}

std::string TransferFunction::Formula( double dependencyValue ) const
{
  TFormula fitFunc( fitFunction_ );
  for ( size_t i = 0; i < NParFit(); ++i ) {
    TFormula depFunc( dependencyFunction_ );
    for ( size_t j = 0; j < NParDependency(); ++j ) {
      depFunc.SetParameter( ( Int_t )j, ( Double_t )( Parameter( i, j ) ) );
    }
    fitFunc.SetParameter( ( Int_t )i, ( Double_t )( depFunc.Eval( dependencyValue ) ) );
  }
  return std::string( fitFunc.GetExpFormula( "p" ).Data() );
}

double TransferFunction::Eval( double dependencyValue, double value ) const
{
  TFormula fitFunc( fitFunction_ );
  for ( size_t i = 0; i < NParFit(); ++i ) {
    TFormula depFunc( dependencyFunction_ );
    for ( size_t j = 0; j < NParDependency(); ++j ) {
      depFunc.SetParameter( ( Int_t )j, ( Double_t )( Parameter( i, j ) ) );
    }
    fitFunc.SetParameter( ( Int_t )i, ( Double_t )( depFunc.Eval( dependencyValue ) ) );
  }
  return fitFunc.Eval( value );
}

// Communication

std::string TransferFunction::Print( bool useNan ) const
{
  std::stringstream print( std::ios_base::out );
  print << std::endl;
  print << "FitFunction       : \t" << FitFunction() << std::endl;
  print << "DependencyFunction: \t" << DependencyFunction() << " \ton " << Dependency() << std::endl;
  print << "Comment           : \t" << Comment() << std::endl << std::endl;

  print << "Parameters 1D:" << std::endl;
  for ( size_t i = 0; i < NParFit(); ++i ) {
    print << "[" << i << "]: \t";
    if ( useNan && Parameter( i ) == transferFunctionInitConst ) print << "NAN";
    else print << Parameter( i );
    print << std::endl;
  }
  print << "[all]: \t" << PrintFit1D( useNan ) << std::endl;
  print << std::endl;

  print << "Parameters 2D (DependencyFunction):" << std::endl;
  for ( size_t i = 0; i < NParFit(); ++i ) {
    print << "[" << i << "]: \t"  << PrintDependency( i, useNan ) << std::endl;
  }
  print << "[all]: \t" << PrintFit2D( useNan ) << std::endl;

  return print.str();
}

std::string TransferFunction::PrintFit1D( bool useNan ) const
{
  TString fitStr( FitFunction() );
  for ( size_t i = 0; i < NParFit(); ++i ) {
    TString valStr( boost::lexical_cast< std::string >( Parameter( i ) ) );
    if ( useNan ) valStr.ReplaceAll( boost::lexical_cast< std::string >( transferFunctionInitConst ).c_str(), "NAN" );
    valStr.Prepend( "(" );
    valStr.Append( ")" );
    TString parStr( "[" + boost::lexical_cast< std::string >( i ) + "]" );
    fitStr.ReplaceAll( parStr, valStr );
  }
  return std::string( fitStr.Data() );
}

std::string TransferFunction::PrintFit2D( bool useNan ) const
{
  TString fitStr( FitFunction() );
  for ( size_t i = 0; i < NParFit(); ++i ) {
    TString depStr( PrintDependency( i, useNan ) );
    depStr.Prepend( "(" );
    depStr.Append( ")" );
    TString parStr( "[" + boost::lexical_cast< std::string >( i ) + "]" );
    fitStr.ReplaceAll( parStr, depStr );
  }
  return std::string( fitStr.Data() );
}

std::string TransferFunction::PrintDependency( size_t i, bool useNan ) const
{
    TFormula depFunc( dependencyFunction_ );
    for ( size_t j = 0; j < NParDependency(); ++j ) {
      depFunc.SetParameter( ( Int_t )j, ( Double_t )( Parameter( i, j ) ) );
    }
    TString depStr( depFunc.GetExpFormula( "p" ) );
    depStr.ReplaceAll( "x", Dependency() );
    if ( useNan ) depStr.ReplaceAll( boost::lexical_cast< std::string >( transferFunctionInitConst ).c_str(), "NAN" );
    return std::string( depStr.Data() );
}

TransferFunction TransferFunction::Read( const std::string & stream )
{
  TransferFunction transfer;
  return transfer;
}
