#ifndef TopQuarkPhysics_TopMassSemiLeptonic_TransferFunction_h
#define TopQuarkPhysics_TopMassSemiLeptonic_TransferFunction_h


// -*- C++ -*-
//
// Package:    TopMassSemiLeptonic
// Class:      my::TransferFunction
//
// $Id:$
//
/**
  \class    my::TransferFunction TransferFunction.h "TopQuarkAnalsyis/TopMassSemiLeptonic/interface/TransferFunction.h"
  \brief    Representation of transfer functions in 1 and 2 dimensions

   my::TransferFunction implements a representation of functions in
   - 1-D: as simple function with fixed parameters
   - 2-D: as function, whose parameters' dependencies are described by another 1-D function

  \author   Volker Adler
  \version  $Id:$
*/


#include <vector>
#include <string>

#include <TF2.h>


namespace my {

  /// Constants

  /// Initialiser
  /// This is just an arbitrarily large negative number (here: -999999.) to
  /// signal, that no real value has been assigned yet.
  /// It is necessary, since e.g. "0." is a possible real value.
  static const double transferFunctionInitConst( -999999. );

  class TransferFunction {

      ///
      /// Data Members
      ///

      /// Fit function
      /// Function to be fit to the momentum difference distributions.
      TFormula fitFunction_;

      /// Dependency function
      /// Function to be fit distributions of fit parameters for the fit
      /// function over a dependency variable.
      TFormula dependencyFunction_;

      /// Name of the dependency variable (e.g. "E_parton", "eta").
      std::string dependency_;

      /// Comment
      /// This string is added to the Print() output, describing e.g. applied
      /// cuts etc.
      std::string comment_;

      /// Parameter vector for 1D
      /// It holds the results of the fit of the fit function, independently
      /// of the dependency variable.
      std::vector< double > pars1D_;

      /// Parameter vectors for 2D
      /// It holds the results of the fit of thhe dependency function for each
      /// parameter of the fit function.
      std::vector< std::vector< double > > pars2D_;

    public:

      ///
      /// Constructors and Desctructor
      ///

      /// Default constructor
      TransferFunction() { TransferFunction( "" ); };

      /// Constructor from TransferFunction (copy c'tor)
      TransferFunction( const TransferFunction & transfer );

      /// Constructor from strings
      /// Functions are entered in the format for filling a TFormula (s. data
      /// members).
      TransferFunction( const std::string & fitFunction, const std::string & dependencyFunction = "0", const std::string & dependency = "x" );

      /// Destructor
      virtual ~TransferFunction() {};

      ///
      /// Methods
      ///

      /// Setters

      /// Set the fit function.
      /// The function is entered in the format for filling a TFormula (s. data
      /// members).
      /// The corresponding parameter vectors are resized according to the
      /// number of function parameters. If 'clear' is 'false', the existing
      /// values are not overwritten.
      void SetFitFunction( const std::string & fitFunction, bool clear = true );

      /// Set the dependency function.
      /// The function is entered in the format for filling a TFormula (s. data
      /// members).
      /// The corresponding vector of parameter vectors are resized according
      /// to the number of function parameters. If 'clear' is 'false', the
      /// existing values are not overwritten.
      void SetDependencyFunction( const std::string & dependencyFunction, bool clear = true );

      /// Set the name of the dependency variable (e.g. "E_parton", "eta").
      void SetDependency( const std::string & dependency ) { dependency_ = dependency; };

      /// Set the comment string.
      void SetComment( const std::string & comment ) { comment_ = comment; };

      /// Set a fit parameter of the fit function, which is independent of the
      /// dependency variable.
      /// 'i' denotes the parameter number as defined in the fit function
      /// string. If 'i' is beyond the number of available parameters in the
      /// fit function, 'false' is returned.
      bool SetParameter( unsigned i, double par );

      /// Set a fit parameter of the dependency function.
      /// 'i' denotes the parameter number as defined in the fit function
      /// string; 'j' the parameter number as defines in the dependency
      /// function string. If 'i' is beyond the number of available parameters
      /// in the fit function or 'j' is beyond the number of available
      /// parameters in the dependency function, 'false' is returned.
      bool SetParameter( unsigned i, unsigned j, double par );

      /// Set all fit parameter of the fit function, which is independent of
      /// the dependency variable.
      /// The order of the vector elements has to correspond to the parameter
      /// numbers as defined in the fit function string. If the length of the
      /// vector does not agree with the number of available parameters in the
      /// fit function, 'false' is returned.
      bool SetParameters( std::vector< double > pars );

      /// Set all fit parameter of the dependency function for a given
      /// parameter.
      /// 'j' denotes the parameter number as defined in the dependency
      /// function string.
      /// The order of the vector elements has to correspond to the parameter
      /// numbers as defined in the fit function string. If'j' is beyond the
      /// number of available parameters in the dependency function or the
      /// length of the vector does not agree with the number of available
      /// parameters in the fit function, 'false' is returned.
      bool SetParameters( unsigned j, std::vector< double > pars );

      /// Resize all vectors according to the number of parameters in the
      /// functions and set them to the initialisation constant.
      void ClearParameters();

      /// Resize all vectors according to the number of parameters in the
      /// functions .
      void ResizeParameters();

      /// Getters

      /// Get the fit function.
      /// The function is returned in the format for filling a TFormula (s.
      /// data members).
      std::string FitFunction() const { return std::string( fitFunction_.GetTitle() ); };

      /// Get the number of parameters in the fit function.
      unsigned NParFit() const { return pars1D_.size(); };

      /// Get the dependency function.
      /// The function is returned in the format for filling a TFormula (s.
      /// data members).
      std::string DependencyFunction() const { return std::string( dependencyFunction_.GetTitle() ); };

      /// Get the number of parameters in the dependency function.
      unsigned NParDependency() const { return pars2D_.size(); };

      /// Get the name of the dependency variable.
      std::string Dependency() const { return dependency_; };

      /// Get the comment string.
      std::string Comment() const { return comment_; };

      /// Get a fit parameter of the fit function, which is independent of the
      /// dependency variable.
      /// 'i' denotes the parameter number as defined in the fit function
      /// string. If 'i' is beyond the number of available parameters in the
      /// fit function, the initialisation constant is returned.
      double Parameter( unsigned i ) const { return i < NParFit() ? pars1D_.at( i ) :  transferFunctionInitConst; };

      /// Get a fit parameter of the dependency function.
      /// 'i' denotes the parameter number as defined in the fit function
      /// string; 'j' the parameter number as defines in the dependency
      /// function string. If 'i' is beyond the number of available parameters
      /// in the fit function or 'j' is beyond the number of available
      /// parameters in the dependency function, the initialisation constant is
      /// returned.
      double Parameter( unsigned i, unsigned j ) const;

      /// Get all fit parameter of the fit function, which is independent of
      /// the dependency variable.
      /// The order of the vector elements corresponds to the parameter numbers
      /// as defined in the fit function string.
      std::vector< double > Parameters() const { return Parameters1D(); };

      /// Get all fit parameter of the dependency function for a given
      /// parameter.
      /// 'j' denotes the parameter number as defined in the dependency
      /// function string.
      /// The order of the vector elements corresponds to the parameter numbers
      /// as defined in the fit function string. If'j' is beyond the
      /// number of available parameters in the dependency function, an empty
      /// vector is returned.
      std::vector< double > Parameters( unsigned j ) const;

      /// Evaluate

      /// Get the 2D transfer function as ROOT function.
      TF2 Function( int norm = 0 ) const;

      /// Get the 1D transfer function as ROOT function, with its parameters
      /// set to the numerical values for the given value of the dependency
      /// variable.
      TF1 Function( double dependencyValue, int norm = 0 ) const;

      /// Get the 2D transfer function as string.
      /// The function is returned in the format for filling a TFormula (s.
      /// data members).
      std::string Formula( int norm = 0 ) const;

      /// Get the 1D transfer function as string, with its parameters set to
      /// the numerical values for the given value of the dependency variable.
      /// The function is returned in the format for filling a TFormula (s.
      /// data members).
      std::string Formula( double dependencyValue, int norm = 0 ) const;

      /// Get the  numerical value of the 1D fit function for given values of
      /// the fit variable.
      double Eval( double value, int norm = 0 ) const;

      /// Get the  numerical value of the 2D fit function for given values of
      /// the fit and dependency variables.
      double Eval( double dependencyValue, double value, int norm = 0 ) const;

      /// Communication

      /// Returns a formatted string for human readability with all important
      /// parameters of the transfer function.
      std::string Print( bool useNan = true ) const;

      /// Returns a formatted string for human readability of the 1D fit
      /// function.
      std::string PrintFit1D( bool useNan = true ) const;

      /// Returns a formatted string for human readability of the 2D fit
      /// function.
      std::string PrintFit2D( bool useNan = true ) const;

      /// Returns a formatted string for human readability of the dependency
      /// function for the given parameter of the fit function.
      std::string PrintDependency( unsigned i, bool useNan = true ) const;

      /// Creates a transfer function from a string as produced by Print().
      /// NOT IMPLEMENTED YET!
      static TransferFunction Read( const std::string & stream );

    private:

      std::vector< double > Parameters1D() const { return pars1D_;};
      std::vector< std::vector< double > > Parameters2D() const { return pars2D_ ;};

  };


  /// Collection of TransferFunction
  typedef std::vector< TransferFunction > TransferFunctionCollection;

}


#endif
