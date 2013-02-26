#ifndef CommonTools_MyTools_RootFunctions_h
#define CommonTools_MyTools_RootFunctions_h


#include "TROOT.h"


namespace my {

  /// Base functions

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

  /// Function implementations

  /// Line with the following parameters:
  /// - [0]: y-intercept
  /// - [1]: gradient
  class Line : public Function {
    public:
      virtual Double_t operator()( Double_t * x, Double_t * par );
      inline static Int_t NPar() { return 2; };
  };

  /// Parabola with the following parameters:
  /// - [0]: y-intercept
  /// - [1]: gradient
  /// - [2]: curvature
  class Parabola : public Function {
    public:
      virtual Double_t operator()( Double_t * x, Double_t * par );
      inline static Int_t NPar() { return 3; };
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
  };

  /// Normalised single Gaussian with the following parameters:
  /// - [0]: normation factor
  /// - [1]: Gaussian mean
  /// - [2]: Gaussian sigma
  /// Returns -1. in case of error
  class SingleGaussian : public Resolution {
    public:
      virtual Double_t operator()( Double_t * x, Double_t * par );
      inline static Int_t NPar() { return 3; };
      virtual Double_t Sigma( Double_t * par );
  };

  /// Normalised double Gaussian with the following parameters:
  /// - [0]: normation factor
  /// - [1]: first Gaussian mean
  /// - [2]: first Gaussian sigma
  /// - [3]: additional normation factor for second Gaussian
  /// - [4]: second Gaussian mean
  /// - [5]: second Gaussian sigma
  /// Returns -1. in case of error
  class DoubleGaussian : public Resolution {
    public:
      virtual Double_t operator()( Double_t * x, Double_t * par );
      inline static Int_t NPar() { return 6; };
      virtual Double_t Sigma( Double_t * par );
  };

  /// Normalised low sided Crystal Ball function with the following parameters:
  /// - [0]: normation factor
  /// - [1]: Gaussian mean
  /// - [2]: Gaussian sigma
  /// - [3]: transition position
  /// - [4]: power law parameter
  /// Returns -1. in case of error
  class LowCrystalBall : public Resolution {
    public:
      virtual Double_t operator()( Double_t * x, Double_t * par );
      inline static Int_t NPar() { return 5; };
      virtual Double_t Sigma( Double_t * par );
  };

  /// Normalised double sided Crystal Ball function with the following parameters:
  /// - [0]: normation factor
  /// - [1]: Gaussian mean
  /// - [2]: Gaussian sigma
  /// - [3]: transition position first power law
  /// - [4]: first power law parameter
  /// - [5]: transition position second power law
  /// - [6]: second power law parameter
  /// Returns -1. in case of error
  class DoubleCrystalBall : public Resolution {
    public:
      virtual Double_t operator()( Double_t * x, Double_t * par );
      inline static Int_t NPar() { return 7; };
      virtual Double_t Sigma( Double_t * par );
  };

}


#endif
