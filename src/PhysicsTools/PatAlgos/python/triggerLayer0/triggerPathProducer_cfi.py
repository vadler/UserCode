import FWCore.ParameterSet.Config as cms

patTriggerPaths = cms.EDProducer( "PATTriggerPathProducer",
    processName    = cms.string( 'HLT' ),
    triggerResults = cms.InputTag( "TriggerResults" )
)

