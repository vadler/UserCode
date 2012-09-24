#include "TopQuarkAnalysis/TopMassSemiLeptonic/interface/MyTools.h"


void my::setParametersFitFrac( TF1 * fit, TH1D * histo, bool useBkgFunction )
{
  // Starting points
//   double m( histo->GetMean() );
//   double c( histo->GetBinContent( histo->FindBin( histo->GetMean() ) ) );
//   double m( histo->GetBinCenter( histo->GetMaximumBin() ) );
//   double c( histo->GetBinContent( histo->GetMaximumBin() ) );
  double m( 1. );
  double c( histo->GetBinContent( histo->FindBin( 1. ) ) );
// //   double c( histo->GetBinContent( histo->GetMaximumBin() ) * std::sqrt( TMath::TwoPi() ) * histo->GetRMS() );
  double s( histo->GetRMS() );
  // Gaussian part
  fit->SetParameter( 0, m );
  fit->SetParLimits( 0, 0., 2. );
  fit->SetParName( 0, "Gaussian #Mu" );
  fit->SetParameter( 1, c );
  fit->SetParLimits( 1, 0., 2. * c );
  fit->SetParName( 1, "Gaussian c" );
  fit->SetParameter( 2, s );
  fit->SetParLimits( 2, 0., 2. );
  fit->SetParName( 2, "Gaussian #sigma" );
  // Additional part
  if ( useBkgFunction ) {
//     fit->SetParameter( 3, log( (m-2.*s)*(m-2.*s) / std::sqrt( s*s + (m-2.*s)*(m-2.*s) ) ) );
    fit->SetParameter( 3, 0. );
    fit->SetParName( 3, "bkg #mu" );
    fit->SetParameter( 4, c ); // just a rough guess, which should serve both cases (highest peak in "signal" or background)
    fit->SetParLimits( 4, 0., 99. );
    fit->SetParName( 4, "bkg c" );
//     fit->SetParameter( 5, std::sqrt( log( s*s / (m-2.*s)*(m-2.*s) + 1. ) ) );
    fit->SetParameter( 5, 1. );
    fit->SetParLimits( 5, 0., 99. );
    fit->SetParName( 5, "bkg #sigma" );
  }
}


void my::setParametersFitDelta( TF1 * fit, TH1D * histo )
{
  // Starting points
  Double_t c( histo->Integral() );
  Double_t m( histo->GetMean() );
  Double_t s( histo->GetRMS() );
  // Gaussian part
  fit->SetParameter( 0, c );
//   fit->SetParLimits( 0, 0., 2. * c );
  fit->SetParName( 0, "Constant" );
  fit->SetParameter( 1, m );
  fit->SetParLimits( 1, -2. * s, 2. * s );
  fit->SetParName( 1, "Gaussian #Mu" );
  fit->SetParameter( 2, s );
  fit->SetParLimits( 2, 0., 2. * s );
  fit->SetParName( 2, "Gaussian #sigma" );
}


void my::setParametersFitTransfer1D( TF1 * fit, TH1D * histo, bool useBkgFunction )
{
  // Starting points
  Double_t c( histo->Integral() );
  Double_t m( histo->GetMean() );
  Double_t s( histo->GetRMS() );
  // Gaussian part
  fit->SetParameter( 0, c );
//   fit->SetParLimits( 0, 0., 2. * c );
  fit->SetParName( 0, "Constant" );
  fit->SetParameter( 1, m );
  fit->SetParLimits( 1, -2. * s, 2. * s );
  fit->SetParName( 1, "Gaussian #Mu" );
  fit->SetParameter( 2, s );
  fit->SetParLimits( 2, 0., 2. * s );
  fit->SetParName( 2, "Gaussian #sigma" );
  // Additional part
  if ( useBkgFunction ) {
  }
}


void my::setParametersFitTransfer2D( TF2 * fit, TH2D * histo, bool useBkgFunction )
{
  // Starting points
  // Additional part
  if ( useBkgFunction ) {
  }
}


void my::setParametersBkg( TF1 * fit, TF1 * bkg )
{
  Int_t n( fit->GetNumberFreeParameters() );
  for ( Int_t i = 3; i < n; ++i ) {
    bkg->SetParameter( i, fit->GetParameter( i ) );
  }
}

