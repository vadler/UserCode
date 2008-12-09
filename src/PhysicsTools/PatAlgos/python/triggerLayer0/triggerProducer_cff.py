import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.triggerLayer0.triggerObjectProducer_cfi import *
from PhysicsTools.PatAlgos.triggerLayer0.triggerFilterProducer_cfi import *
from PhysicsTools.PatAlgos.triggerLayer0.triggerPathProducer_cfi   import *

patLayer0Trigger = cms.Sequence(
    patTriggerObjects +
    patTriggerFilters +
    patTriggerPaths
)
 