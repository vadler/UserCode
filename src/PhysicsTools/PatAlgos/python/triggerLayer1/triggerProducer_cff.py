import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.triggerLayer1.triggerEventProducer_cfi import *

patLayer1Trigger = cms.Sequence(
    patTriggerEvent
)
 