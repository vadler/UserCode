#include "PluginManager/ModuleDef.h"
#include "FWCore/Framework/interface/InputSourceMacros.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "IOMC/Input/interface/MCFileSource.h"
#include "IOMC/Input/interface/MCFileSource_12.h"


  using edm::MCFileSource;
  using edm::MCFileSource_12;
  DEFINE_SEAL_MODULE();
  DEFINE_ANOTHER_FWK_INPUT_SOURCE(MCFileSource);
  DEFINE_ANOTHER_FWK_INPUT_SOURCE(MCFileSource_12);

