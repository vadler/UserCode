import FWCore.ParameterSet.Config as cms

layer1Triggers = cms.EDProducer( "PATTriggerProducer",
    processName    = cms.string( 'HLT' ),
    triggerResults = cms.InputTag( "TriggerResults" ),
    triggerEvent   = cms.InputTag( "hltTriggerSummaryAOD" )
)

