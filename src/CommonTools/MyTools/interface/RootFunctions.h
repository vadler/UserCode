#ifndef CommonTools_MyTools_RootFunctions_h
#define CommonTools_MyTools_RootFunctions_h


#include <string>

#include "TROOT.h"


/// Base functions

namespace my {

  /// Generic function base class
  class Function {
    public:
      virtual Double_t operator()( Double_t * x, Double_t * par ) = 0;
  };

  /// Generic resolution function base class
  class Resolution : public Function {
    public:
      virtual Double_t Sigma( Double_t * par ) = 0;
  };

}

/// Dependency implementations

namespace my {

  /// Line with the following parameters:
  /// - [0]: y-intercept
  /// - [1]: gradient
  class Line : public Function {
    public:
      virtual Double_t operator()( Double_t * x, Double_t * par );
      inline static Int_t NPar() { return 2; };
      inline static std::string String() { return std::string( "[0]+[1]*x" ); };
  };

  /// Parabola with the following parameters:
  /// - [0]: y-intercept
  /// - [1]: gradient
  /// - [2]: curvature
  class Parabola : public Function {
    public:
      virtual Double_t operator()( Double_t * x, Double_t * par );
      inline static Int_t NPar() { return 3; };
      inline static std::string String() { return std::string( "[0]+[1]*x+[2]*x**2" ); };
  };

  /// Resolution function for x>=0. with the following parameters:
  /// - [0]: noise
  /// - [1]: resolution
  /// - [2]: constant
  /// Returns -1. in case of error
  class ResolutionLike : public Function {
    public:
      virtual Double_t operator()( Double_t * x, Double_t * par );
      inline static Int_t NPar() { return 3; };
      inline static std::string String() { return std::string( "sqrt([0]**2+[1]**2*x+[2]**2*x**2)" ); };
  };

}

/// Resolution implementations

namespace my {

  /// Normalised single Gaussian with the following parameters:
  /// - [0]: normalisation factor
  /// - [1]: Gaussian mean
  /// - [2]: Gaussian sigma
  /// Returns -1. in case of error
  class SingleGaussian : public Resolution {
    public:
      virtual Double_t operator()( Double_t * x, Double_t * par );
      inline static Int_t NPar() { return 3; };
      inline static std::string String() { return std::string( "[0]*exp(-0.5*((x-[1])/[2])**2)/([2]*sqrt(2*pi))" ); };
      virtual Double_t Sigma( Double_t * par ) { return par[ 2 ]; };
  };

  /// Normalised double Gaussian with the following parameters:
  /// - [0]: normalisation factor
  /// - [1]: first Gaussian mean
  /// - [2]: first Gaussian sigma
  /// - [3]: additional normalisation factor for second Gaussian
  /// - [4]: second Gaussian mean
  /// - [5]: second Gaussian sigma
  /// Returns -1. in case of error
  class DoubleGaussian : public Resolution {
    public:
      virtual Double_t operator()( Double_t * x, Double_t * par );
      inline static Int_t NPar() { return 6; };
      inline static std::string String() { return std::string( "[0]*(exp(-0.5*((x-[1])/[2])**2)+[3]*exp(-0.5*((x-[4])/[5])**2))/(([2]+[3]*[5])*sqrt(2*pi))" ); };
      virtual Double_t Sigma( Double_t * par ) { return par[ 2 ] + par[ 3 ] * par[ 5 ]; };
  };

  /// Normalised low sided Crystal Ball function with the following parameters:
  /// - [0]: normalisation factor
  /// - [1]: Gaussian mean
  /// - [2]: Gaussian sigma
  /// - [3]: transition position; >= 0.
  /// - [4]: power law parameter
  /// Returns -1. in case of error
  class LowerCrystalBall : public Resolution {
    public:
      virtual Double_t operator()( Double_t * x, Double_t * par );
      inline static Int_t NPar() { return 5; };
      inline static std::string String() { return std::string( "" ); };
      virtual Double_t Sigma( Double_t * par ) { return par[ 2 ]; };
  };

  /// Normalised high sided Crystal Ball function with the following parameters:
  /// - [0]: normalisation factor
  /// - [1]: Gaussian mean
  /// - [2]: Gaussian sigma
  /// - [3]: transition position; >= 0.
  /// - [4]: power law parameter
  /// Returns -1. in case of error
  class UpperCrystalBall : public Resolution {
    public:
      virtual Double_t operator()( Double_t * x, Double_t * par );
      inline static Int_t NPar() { return 5; };
      inline static std::string String() { return std::string( "" ); };
      virtual Double_t Sigma( Double_t * par ) { return par[ 2 ]; };
  };

  /// Normalised double sided Crystal Ball function with the following parameters:
  /// - [0]: normalisation factor
  /// - [1]: Gaussian mean
  /// - [2]: Gaussian sigma
  /// - [3]: transition position first power law; >= 0.
  /// - [4]: first power law parameter
  /// - [5]: transition position second power law; >= 0.
  /// - [6]: second power law parameter
  /// Returns -1. in case of error
  class DoubleCrystalBall : public Resolution {
    public:
      virtual Double_t operator()( Double_t * x, Double_t * par );
      inline static Int_t NPar() { return 7; };
      inline static std::string String() { return std::string( "" ); };
      virtual Double_t Sigma( Double_t * par ) { return par[ 2 ]; };
  };

}


#endif
