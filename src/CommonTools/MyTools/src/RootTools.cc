#include "CommonTools/MyTools/interface/RootTools.h"


void my::setPlotEnvironment( TStyle * style )
{

  style->SetPalette( 1, 0 );
  style->SetCanvasColor( kWhite );
  style->SetPadColor( kWhite) ;
  style->SetPadTickX( 1 );
  style->SetPadTickY( 1 );
  style->SetPadTopMargin( .075 );
  style->SetPadRightMargin( .075 );
  style->SetPadBottomMargin( .15 );
  style->SetPadLeftMargin( .15 );
  style->SetTitleSize( .06, "XYZ" );
  style->SetTitleFillColor( kWhite );
  style->SetTitleBorderSize( 1 );
  style->SetStatColor( kWhite );
  style->SetStatBorderSize( 1 );
  style->SetOptStat( 1 );
  style->SetOptFit( 0 );
  style->SetMarkerStyle( kFullDotSmall );

}
