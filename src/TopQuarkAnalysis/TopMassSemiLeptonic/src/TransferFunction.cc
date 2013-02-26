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
: fitFunction_( transfer.GetFitFunction() )
, fitFunctionString_( transfer.FitFunctionString() )
, dependencyFunction_( transfer.GetDependencyFunction() )
, dependencyFunctionString_( transfer.DependencyFunctionString() )
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
, fitFunctionString_( fitFunction )
, dependencyFunction_( "dependencyFunction", dependencyFunction.c_str() )
, dependencyFunctionString_( dependencyFunction )
, dependency_( dependency )
, comment_()
{
  ClearParameters();
}

// Constructor from TF1s
TransferFunction::TransferFunction( TF1 * fitFunction, TF1 * dependencyFunction, const std::string & dependency )
: fitFunction_( *fitFunction )
, fitFunctionString_( "" )
, dependencyFunction_( *dependencyFunction )
, dependencyFunctionString_( "" )
, dependency_( dependency )
, comment_()
{
  ClearParameters();
  fitFunction_.SetName( "fitFunction" );
  dependencyFunction_.SetName( "dependencyFunction" );
}


// Methods

// Setters

void TransferFunction::SetFitFunction( const std::string & fitFunctionString, bool clear )
{
  fitFunction_       = TF1( "fitFunction", fitFunctionString.c_str() );
  fitFunctionString_ = fitFunctionString;
  clear ? ClearParameters() : ResizeParameters();
}

void TransferFunction::SetFitFunction( TF1 * fitFunction, bool clear )
{
  fitFunction_ = TF1( *fitFunction );
  fitFunction_.SetName( "fitFunction" );
  fitFunctionString_ = fitFunction->GetTitle();
  clear ? ClearParameters() : ResizeParameters();
}

void TransferFunction::SetFitFunctionString( const std::string & fitFunctionString )
{
  if ( FitFunction().empty() ) fitFunctionString_ = fitFunctionString;
}

void TransferFunction::SetDependencyFunction( const std::string & dependencyFunctionString, bool clear )
{
  dependencyFunction_       = TF1( "dependencyFunction", dependencyFunctionString.c_str() );
  dependencyFunctionString_ = dependencyFunctionString;
  clear ? ClearParameters() : ResizeParameters();
}

void TransferFunction::SetDependencyFunction( TF1 * dependencyFunction, bool clear )
{
  dependencyFunction_ = TF1( *dependencyFunction );
  dependencyFunction_.SetName( "dependencyFunction" );
  dependencyFunctionString_ = dependencyFunction->GetTitle();
  clear ? ClearParameters() : ResizeParameters();
}

void TransferFunction::SetDependencyFunctionString( const std::string & dependencyFunctionString )
{
  if ( DependencyFunction().empty() ) dependencyFunctionString_ = dependencyFunctionString;
}

bool TransferFunction::SetParameter( unsigned i, double par )
{
  if ( i < NParFit() ) {
    pars1D_.at( i ) = par;
    return true;
  }
  return false;
}

bool TransferFunction::SetParameter( unsigned i, unsigned j, double par )
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

bool TransferFunction::SetParameters( unsigned j, std::vector< double > pars )
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
  for ( unsigned i = 0; i < NParFit(); ++i ) {
    pars1D_.at( i ) = transferFunctionInitConst;
    for ( unsigned j = 0; j < NParDependency(); ++j ) {
      pars2D_.at( j ).at( i ) = transferFunctionInitConst;
    }
  }
}

void TransferFunction::ResizeParameters()
{
  pars1D_.resize( GetFitFunction().GetNpar() );
  pars2D_.resize( GetDependencyFunction().GetNpar(), std::vector< double >( GetFitFunction().GetNpar() ) );
}

// Getters

double TransferFunction::Parameter( unsigned i, unsigned j ) const
{
  if ( i < NParFit() && j < NParDependency() ) return Parameters2D().at( j ).at( i );
  return transferFunctionInitConst;
}

std::vector< double > TransferFunction::Parameters( unsigned j ) const
{
  if ( j < NParDependency() ) return Parameters2D().at( j );
  return std::vector< double >();
}

// Evaluate

TF2 TransferFunction::Function( int norm ) const
{
  TString fitStr( FitFunction() );
  std::vector< double > pars;
  for ( unsigned i = 0; i < NParFit(); ++i ) {
    if ( ( int )i == norm ) continue;
    TString depStr( DependencyFunction() );
    for ( unsigned j = 0; j < NParDependency(); ++j ) {
      TString parStr( "[" + boost::lexical_cast< std::string >( j ) + "]" );
      TString parStrNew( "{" + boost::lexical_cast< std::string >( pars.size() ) + "}" ); // FIXME: Just stupid manipulation.
      depStr.ReplaceAll( parStr, parStrNew );
      depStr.ReplaceAll( "x", "y" );
      depStr.ReplaceAll( "eyp", "exp" ); // Fixing unwanted replacements
      pars.push_back( Parameters2D().at( j ).at( i ) );
    }
    depStr.Prepend( "(" );
    depStr.Append( ")" );
    TString parStr( "[" + boost::lexical_cast< std::string >( i ) + "]" );
    fitStr.ReplaceAll( parStr, depStr );
  }
  for ( unsigned i = 0; i < NParFit(); ++i ) {
    TString parStr( "[" + boost::lexical_cast< std::string >( i ) + "]" );
    fitStr.ReplaceAll( parStr, "1." );
  }
  fitStr.ReplaceAll( "{", "[" ); // FIXME: Just stupid manipulation (s. above).
  fitStr.ReplaceAll( "}", "]" ); // FIXME: Just stupid manipulation (s. above).
  TF2 function( "", fitStr );
  for ( unsigned i = 0; i < pars.size(); ++i ) {
    function.SetParameter( ( Int_t )i, pars.at( i ) );
  }
  return function;
}

TF1 TransferFunction::Function( double dependencyValue, int norm ) const
{
  TString fitStr;
  if ( FitFunction().empty() && ! FitFunctionString().empty() ) {
    fitStr = TString( FitFunctionString() );
  }
  else {
    fitStr = TString( FitFunction() );
  }
  std::vector< double > pars;
  for ( unsigned i = 0; i < NParFit(); ++i ) {
    if ( ( int )i == norm ) continue;
    TF1 depFunc( GetDependencyFunction() );
    for ( unsigned j = 0; j < NParDependency(); ++j ) {
      depFunc.SetParameter( ( Int_t )j, ( Double_t )( Parameter( i, j ) ) );
    }
    TString parStr( "[" + boost::lexical_cast< std::string >( i ) + "]" );
    TString parStrNew( "{" + boost::lexical_cast< std::string >( pars.size() ) + "}" ); // FIXME: Just stupid manipulation.
    fitStr.ReplaceAll( parStr, parStrNew );
    pars.push_back( depFunc.Eval( dependencyValue ) );
  }
  for ( unsigned i = 0; i < NParFit(); ++i ) {
    TString parStr( "[" + boost::lexical_cast< std::string >( i ) + "]" );
    fitStr.ReplaceAll( parStr, "1." );
  }
  fitStr.ReplaceAll( "{", "[" ); // FIXME: Just stupid manipulation (s. above).
  fitStr.ReplaceAll( "}", "]" ); // FIXME: Just stupid manipulation (s. above).
  TF1 function( "", fitStr );
  for ( unsigned i = 0; i < pars.size(); ++i ) {
    function.SetParameter( ( Int_t )i, pars.at( i ) );
  }
  return function;
}

std::string TransferFunction::Formula( int norm ) const
{
  TString fitStr;
  if ( FitFunction().empty() && ! FitFunctionString().empty() ) {
    fitStr = TString( FitFunctionString() );
  }
  else {
    fitStr = TString( FitFunction() );
  }
  for ( unsigned i = 0; i < NParFit(); ++i ) {
    TString parStr( "[" + boost::lexical_cast< std::string >( i ) + "]" );
    if ( ( int )i == norm ) {
      fitStr.ReplaceAll( parStr, "1." );
      continue;
    }
    TString depParStr( PrintDependency( i, false ) );
    depParStr.ReplaceAll( Dependency(), "y" );
    depParStr.Prepend( "(" );
    depParStr.Append( ")" );
    fitStr.ReplaceAll( parStr, depParStr );
  }
  return std::string( fitStr.Data() );
}

std::string TransferFunction::Formula( double dependencyValue, int norm ) const
{
  TF1 fitFunc;
  if ( FitFunction().empty() && ! FitFunctionString().empty() ) {
    fitFunc = TF1( "fitFunc", FitFunctionString().c_str() );
  }
  else {
    fitFunc = TF1( GetFitFunction() );
  }
  for ( unsigned i = 0; i < NParFit(); ++i ) {
    if ( ( int )i == norm ) {
      fitFunc.SetParameter( ( Int_t )i, 1. );
      continue;
    }
    TF1 depFunc( GetDependencyFunction() );
    for ( unsigned j = 0; j < NParDependency(); ++j ) {
      depFunc.SetParameter( ( Int_t )j, ( Double_t )( Parameter( i, j ) ) );
    }
    fitFunc.SetParameter( ( Int_t )i, ( Double_t )( depFunc.Eval( dependencyValue ) ) );
  }
  return std::string( fitFunc.GetExpFormula( "p" ).Data() );
}

double TransferFunction::Eval( double value, int norm ) const
{
  TF1 fitFunc;
  if ( FitFunction().empty() && ! FitFunctionString().empty() ) {
    fitFunc = TF1( "fitFunc", FitFunctionString().c_str() );
  }
  else {
    fitFunc = TF1( GetFitFunction() );
  }
  for ( unsigned i = 0; i < NParFit(); ++i ) {
    if ( ( int )i == norm ) {
      fitFunc.SetParameter( ( Int_t )i, 1. );
      continue;
    }
    fitFunc.SetParameter( ( Int_t )i, ( Double_t )( Parameter( i ) ) );
  }
  return fitFunc.Eval( value );
}

double TransferFunction::Eval( double dependencyValue, double value, int norm ) const
{
  TF1 fitFunc;
  if ( FitFunction().empty() && ! FitFunctionString().empty() ) {
    fitFunc = TF1( "fitFunc", FitFunctionString().c_str() );
  }
  else {
    fitFunc = TF1( GetFitFunction() );
  }
  for ( unsigned i = 0; i < NParFit(); ++i ) {
    if ( ( int )i == norm ) {
      fitFunc.SetParameter( ( Int_t )i, 1. );
      continue;
    }
    TF1 depFunc( GetDependencyFunction() );
    for ( unsigned j = 0; j < NParDependency(); ++j ) {
      depFunc.SetParameter( ( Int_t )j, ( Double_t )( Parameter( i, j ) ) );
    }
    fitFunc.SetParameter( ( Int_t )i, ( Double_t )( depFunc.Eval( dependencyValue ) ) );
  }
  return fitFunc.Eval( value );
}

// double TransferFunction::Sigma( double dependencyValue ) const
// {
//   // FIXME: This is too simple. Add checks!
//   TF1 depFunc( GetDependencyFunction() );
//   for ( unsigned j = 0; j < NParDependency(); ++j ) {
//     depFunc.SetParameter( ( Int_t )j, ( Double_t )( Parameter( 2, j ) ) );
//   }
//   const double par2( depFunc.Eval( dependencyValue ) );
//   for ( unsigned j = 0; j < NParDependency(); ++j ) {
//     depFunc.SetParameter( ( Int_t )j, ( Double_t )( Parameter( 3, j ) ) );
//   }
//   const double par3( depFunc.Eval( dependencyValue ) );
//   for ( unsigned j = 0; j < NParDependency(); ++j ) {
//     depFunc.SetParameter( ( Int_t )j, ( Double_t )( Parameter( 5, j ) ) );
//   }
//   const double par5( depFunc.Eval( dependencyValue ) );
//   return par2 + par3 * par5;
// }

// Communication

std::string TransferFunction::Print( bool useNan ) const
{
  std::stringstream print( std::ios_base::out );
  print << std::endl;
  if ( FitFunction().empty() ) print << "FitFunction       : \t" << FitFunctionString() << "\t(constructed from C++ class)" << std::endl;
  else                         print << "FitFunction       : \t" << FitFunction() << std::endl;
  if ( DependencyFunction().empty() ) print << "DependencyFunction: \t" << DependencyFunctionString() << "\t(constructed from C++ class) \ton " << Dependency() << std::endl;
  else                                print << "DependencyFunction: \t" << DependencyFunction() << " \ton " << Dependency() << std::endl;
  print << "Comment           : \t" << Comment() << std::endl << std::endl;

  print << "Parameters 1D:" << std::endl;
  for ( unsigned i = 0; i < NParFit(); ++i ) {
    print << "[" << i << "]: \t";
    if ( useNan && Parameter( i ) == transferFunctionInitConst ) print << "NAN";
    else print << Parameter( i );
    print << std::endl;
  }
  print << "[all]: \t" << PrintFit1D( useNan ) << std::endl;
  print << std::endl;

  print << "Parameters 2D (DependencyFunction):" << std::endl;
  for ( unsigned i = 0; i < NParFit(); ++i ) {
    print << "[" << i << "]: \t"  << PrintDependency( i, useNan ) << std::endl;
  }
  print << "[all]: \t" << PrintFit2D( useNan ) << std::endl;

  return print.str();
}

std::string TransferFunction::PrintFit1D( bool useNan ) const
{
  TString fitStr;
  if ( FitFunction().empty() && ! FitFunctionString().empty() ) {
    fitStr = TString( FitFunctionString() );
  }
  else {
    fitStr = TString( FitFunction() );
  }
  if ( fitStr.Length() > 0 ) {
    for ( unsigned i = 0; i < NParFit(); ++i ) {
      TString valStr( boost::lexical_cast< std::string >( Parameter( i ) ) );
      if ( useNan ) valStr.ReplaceAll( boost::lexical_cast< std::string >( transferFunctionInitConst ).c_str(), "NAN" );
      valStr.Prepend( "(" );
      valStr.Append( ")" );
      TString parStr( "[" + boost::lexical_cast< std::string >( i ) + "]" );
      fitStr.ReplaceAll( parStr, valStr );
    }
  }
  return std::string( fitStr.Data() );
}

std::string TransferFunction::PrintFit2D( bool useNan ) const
{
  TString fitStr;
  if ( FitFunction().empty() && ! FitFunctionString().empty() ) {
    fitStr = TString( FitFunctionString() );
  }
  else {
    fitStr = TString( FitFunction() );
  }
  if ( fitStr.Length() > 0 ) {
    for ( unsigned i = 0; i < NParFit(); ++i ) {
      TString depParStr( PrintDependency( i, useNan ) );
      depParStr.Prepend( "(" );
      depParStr.Append( ")" );
      TString parStr( "[" + boost::lexical_cast< std::string >( i ) + "]" );
      fitStr.ReplaceAll( parStr, depParStr );
    }
  }
  return std::string( fitStr.Data() );
}

std::string TransferFunction::PrintDependency( unsigned i, bool useNan ) const
{
  TF1 depFunc;
  if ( DependencyFunction().empty() && ! DependencyFunctionString().empty() ) {
    depFunc = TF1( "depFunc", DependencyFunctionString().c_str() );
  }
  else {
    depFunc = TF1( GetDependencyFunction() );
  }
  for ( unsigned j = 0; j < NParDependency(); ++j ) {
    depFunc.SetParameter( ( Int_t )j, ( Double_t )( Parameter( i, j ) ) );
  }

  if ( ! DependencyFunction().empty() || ! DependencyFunctionString().empty() ) {
    TString depStr( depFunc.GetExpFormula( "p" ) );
    depStr.ReplaceAll( "x", Dependency() );
    TString failStr( "e" + Dependency() + "p" );
    depStr.ReplaceAll( failStr, "x" ); // Fixing unwanted replacements
    if ( useNan ) depStr.ReplaceAll( boost::lexical_cast< std::string >( transferFunctionInitConst ).c_str(), "NAN" );
    return std::string( depStr.Data() );
  }
  else {
    std::stringstream print( std::ios_base::out );
    for ( unsigned j = 0; j < NParDependency(); ++j ) {
      if ( j > 0 ) print << " \t";
      if ( useNan && Parameter( i, j ) == transferFunctionInitConst )
        print << "NAN";
      else
        print << Parameter( i, j );
    }
    return print.str();
  }
}

TransferFunction TransferFunction::Read( const std::string & stream )
{
  TransferFunction transfer;
  return transfer;
}
