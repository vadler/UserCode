import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cfi import *

layer1TriggerEvent = cms.Sequence(layer1Triggers)
 