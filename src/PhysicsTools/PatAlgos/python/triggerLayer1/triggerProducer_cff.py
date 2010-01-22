import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cfi import *
from PhysicsTools.PatAlgos.triggerLayer1.triggerMatcher_cfi import *
from PhysicsTools.PatAlgos.triggerLayer1.triggerMatchEmbedder_cfi import *
from PhysicsTools.PatAlgos.triggerLayer1.triggerEventProducer_cfi import *

patTriggerSequence = cms.Sequence(
    patTrigger        *
    patTriggerMatcher *
#     patTriggerMatchEmbedder *
    patTriggerEvent
)

patTriggerTestSequence = cms.Sequence(
    patTrigger        *
    patTriggerTestMatcher *
#     patTriggerTestMatchEmbedder *
    patTriggerEventTest
)
