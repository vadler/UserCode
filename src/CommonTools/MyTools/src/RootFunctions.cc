#include "CommonTools/MyTools/interface/RootFunctions.h"

#include "TMath.h"


Double_t my::Line::operator()( Double_t * x, Double_t * par )
{

  Double_t A( par[ 0 ] );
  Double_t B( par[ 1 ] );

  Double_t value( A + B * x[ 0 ] );

  return value;

}


Double_t my::Parabola::operator()( Double_t * x, Double_t * par )
{

  Double_t A( par[ 0 ] );
  Double_t B( par[ 1 ] );
  Double_t C( par[ 2 ] );

  Double_t value( A + B * x[ 0 ] + C * TMath::Power( x[ 0 ], 2. ) );

  return value;

}


Double_t my::ResolutionLike::operator()( Double_t * x, Double_t * par )
{

  Double_t value( -1.);

  Double_t N( par[ 0 ] );
  Double_t R( par[ 1 ] );
  Double_t C( par[ 2 ] );

  if ( x[ 0 ] >= 0. ) {
    value = TMath::Sqrt( TMath::Power( N, 2. ) + TMath::Power( R, 2. ) * x[ 0 ] + TMath::Power( C, 2. ) * TMath::Power( x[ 0 ], 2. ) );
  }

  return value;

}


Double_t my::SingleGaussian::operator()( Double_t * x, Double_t * par )
{

  Double_t value( -1.);

  Double_t N( par[ 0 ] );
  Double_t M( par[ 1 ] );
  Double_t S( par[ 2 ] );

  if ( N >= 0. && S > 0. ) {
    value = N * TMath::Exp( -0.5 * TMath::Power( ( x[ 0 ] - M ) / S, 2. ) ) / ( S * TMath::Sqrt( TMath::TwoPi() ) );
  }

  return value;

}


Double_t my::DoubleGaussian::operator()( Double_t * x, Double_t * par )
{

  Double_t value( -1.);

  Double_t N0( par[ 0 ] );
  Double_t M1( par[ 1 ] );
  Double_t S1( par[ 2 ] );
  Double_t N2( par[ 3 ] );
  Double_t M2( par[ 4 ] );
  Double_t S2( par[ 5 ] );

  if ( N0 >= 0. && S1 > 0. && N2 >= 0. && S2 > 0. ) {
    value = N0 * ( TMath::Exp( -0.5 * TMath::Power( ( x[ 0 ] - M1 ) / S1 , 2. ) ) + N2 * TMath::Exp( -0.5 * TMath::Power( ( x[ 0 ] - M2 ) / S2, 2. ) ) ) / ( ( S1 + N2 * S2 )*TMath::Sqrt( TMath::TwoPi() ) );
  }

  return value;

}


Double_t my::LowerCrystalBall::operator()( Double_t * x, Double_t * par )
{

  Double_t value( -1. );

  Double_t M( par[ 1 ] );
  Double_t S( par[ 2 ] );
  Double_t A( TMath::Power( par[ 4 ] / TMath::Abs( par[ 3 ] ), par[ 4 ] ) * TMath::Exp( -( TMath::Power( TMath::Abs( par[ 3 ] ), 2. ) / 2. ) ) );
  Double_t B( par[ 4 ] / TMath::Abs( par[ 3 ] ) - TMath::Abs( par[ 3 ] ) );
  Double_t C( ( par[ 4 ] / TMath::Abs( par[ 3 ] ) ) * ( 1. / ( par[ 4 ] - 1. ) ) * TMath::Exp( -( TMath::Power( TMath::Abs( par[ 3 ] ), 2. ) / 2. ) ) );
  Double_t D( TMath::Sqrt( TMath::PiOver2() ) * ( 1. + TMath::Erf( TMath::Abs( par[ 3 ] ) / TMath::Sqrt( TMath::Pi() ) ) ) );
  Double_t E( ( x[ 0 ] - M ) / S );
  Double_t N( par[ 0 ] / ( par[ 2 ] * ( C + D ) ) );

  if ( N >= 0. && S > 0. ) {
    if ( E <= -par[ 3 ] ) {
      value = N * A * TMath::Power( ( B - E ), -par[ 4 ] );
    }
    else {
      value = N * TMath::Exp( -0.5 * TMath::Power( E, 2. ) );
    }
  }

  return value;

}


Double_t my::UpperCrystalBall::operator()( Double_t * x, Double_t * par )
{

  Double_t value( -1. );

  Double_t M( par[ 1 ] );
  Double_t S( par[ 2 ] );
  Double_t A( TMath::Power( par[ 4 ] / TMath::Abs( par[ 3 ] ), par[ 4 ] ) * TMath::Exp( -( TMath::Power( TMath::Abs( par[ 3 ] ), 2. ) / 2. ) ) );
  Double_t B( par[ 4 ] / TMath::Abs( par[ 3 ] ) - TMath::Abs( par[ 3 ] ) );
  Double_t C( ( par[ 4 ] / TMath::Abs( par[ 3 ] ) ) * ( 1. / ( par[ 4 ] - 1. ) ) * TMath::Exp( -( TMath::Power( TMath::Abs( par[ 3 ] ), 2. ) / 2. ) ) );
  Double_t D( TMath::Sqrt( TMath::PiOver2() ) * ( 1. + TMath::Erf( TMath::Abs( par[ 3 ] ) / TMath::Sqrt( TMath::Pi() ) ) ) );
  Double_t E( ( x[ 0 ] - M ) / S );
  Double_t N( par[ 0 ] / ( par[ 2 ] * ( C + D ) ) );

  if ( N >= 0. && S > 0. ) {
    if ( E >= par[ 3 ] ) {
      value = N * A * TMath::Power( ( B + E ), -par[ 4 ] );
    }
    else {
      value = N * TMath::Exp( -0.5 * TMath::Power( E, 2. ) );
    }
  }

  return value;

}


Double_t my::DoubleCrystalBall::operator()( Double_t * x, Double_t * par )
{

  Double_t value( -1. );

  return value;

}
