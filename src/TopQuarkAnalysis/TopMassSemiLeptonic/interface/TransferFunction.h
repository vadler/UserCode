#ifndef TopQuarkPhysics_TopMassSemiLeptonic_TransferFunction_h
#define TopQuarkPhysics_TopMassSemiLeptonic_TransferFunction_h


// -*- C++ -*-
//
// Package:    TopMassSemiLeptoni
// Class:      my::TransferFunction
//
// $Id:$
//
/**
  \class    my::TransferFunction TransferFunction.h "TopQuarkAnalsyis/TopMassSemiLeptonic/interface/TransferFunction.h"
  \brief    Representation of transfer functions in 1 and 2 dimensions

   my::TransferFunction implements a representation of functions in
   - 1-D: as simple function with fixed parameters
   - 2-D: as function, whose parameters' dependency are described by another 1-D function

  \author   Volker Adler
  \version  $Id:$
*/


#include <vector>
#include <string>

#include <TFormula.h>


namespace my {

  class TransferFunction {

      /// Data Members

      /// Initialiser
      const double initConst_;
      /// Fit function
      TFormula fitFunction_;
      /// Dependency function
      TFormula dependencyFunction_;
      /// Dependency variable
      std::string dependency_;
      /// Comment
      std::string comment_;
      /// Parameter vector for 1D
      std::vector< double > pars1D_;
      /// Parameter vectors for 2D
      std::vector< std::vector< double > > pars2D_;

    public:

      /// Constructors and Desctructor

      /// Default constructor
      TransferFunction() : initConst_( -999999. ) { TransferFunction( "" ); };
      /// Constructor from TransferFunction (copy c'tor)
      TransferFunction( const TransferFunction & transfer );
      /// Constructor from strings
      TransferFunction( const std::string & fitFunction, const std::string & dependencyFunction = "0", const std::string & dependency = "x" );

      /// Destructor
      virtual ~TransferFunction() {};

      /// Methods

      /// Setters
      void SetFitFunction( const std::string & fitFunction, bool clear = true );
      void SetDependencyFunction( const std::string & dependencyFunction, bool clear = true );
      void SetDependency( const std::string & dependency ) { dependency_ = dependency; };
      void SetComment( const std::string & comment ) { comment_ = comment; };
      bool SetParameter( double par, size_t i, int j = -1 );
      bool SetParameters( std::vector< double > pars, int j = -1 );
      bool SetParameters( std::vector< std::vector< double > > pars );
      void ClearParameters();
      void ResizeParameters();

      /// Getters
//       std::string FitFunction() const { return std::string( fitFunction_.GetExpFormula().Data() ); };
      std::string FitFunction() const { return std::string( fitFunction_.GetTitle() ); };
      size_t NParFit() const { return pars1D_.size(); };
//       std::string DependencyFunction() const { return std::string( dependencyFunction_.GetExpFormula().Data() ); };
      std::string DependencyFunction() const { return std::string( dependencyFunction_.GetTitle() ); };
      size_t NParDependency() const { return pars2D_.size(); };
      std::string Dependency() const { return dependency_; };
      std::string Comment() const { return comment_; };
      std::vector< double > Parameters1D() const { return pars1D_; };
      std::vector< std::vector< double > > Parameters2D() const { return pars2D_; };
      std::vector< double > Parameters2D( size_t j ) const { if ( j < pars2D_.size() ) return pars2D_.at( j ); else return std::vector< double >(); };

      /// Evaluate
      std::string Formula( double dependencyValue ) const;
      double Eval( double dependencyValue, double value ) const;
      std::string Print() const;
      static TransferFunction Read( const std::string & stream );

  };


  /// Collection of TransferFunction
  typedef std::vector< TransferFunction > TransferFunctionCollection;

}


#endif
