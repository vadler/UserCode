import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.triggerLayer0.triggerProducer_cfi   import *

patLayer0Trigger = cms.Sequence(
    patTrigger
)
 