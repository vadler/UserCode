#ifndef TopQuarkPhysics_TopMassSemiLeptonic_MyTools_h
#define TopQuarkPhysics_TopMassSemiLeptonic_MyTools_h


#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TF2.h>


namespace my {

  void setParametersFitFrac( TF1 * fit, TH1D * histo, bool useBkgFunction = false );
  void setParametersFitDelta( TF1 * fit, TH1D * histo );
  void setParametersBkg( TF1 * fit, TF1 * bkg );

}


#endif
