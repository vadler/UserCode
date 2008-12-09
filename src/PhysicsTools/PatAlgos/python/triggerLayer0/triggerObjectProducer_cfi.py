import FWCore.ParameterSet.Config as cms

patTriggerObjects = cms.EDProducer( "PATTriggerObjectProducer",
    processName  = cms.string( 'HLT' ),
    triggerEvent = cms.InputTag( "hltTriggerSummaryAOD" )
)

