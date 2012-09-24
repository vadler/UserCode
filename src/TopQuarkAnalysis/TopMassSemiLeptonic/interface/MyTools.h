#ifndef PhysicsTools_PatAlgos_PATTriggerProducer_h
#define PhysicsTools_PatAlgos_PATTriggerProducer_h


#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TF2.h>


namespace my {

  void setParametersFitFrac( TF1 * fit, TH1D * histo, bool useBkgFunction = false );
  void setParametersFitDelta( TF1 * fit, TH1D * histo );
  void setParametersFitTransfer1D( TF1 * fit, TH1D * histo, bool useBkgFunction = false );
  void setParametersFitTransfer2D( TF2 * fit, TH2D * histo, bool useBkgFunction = false );
  void setParametersBkg( TF1 * fit, TF1 * bkg );

}


#endif
