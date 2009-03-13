import FWCore.ParameterSet.Config as cms

patTrigger = cms.EDProducer( "PATTriggerProducer",
    processName    = cms.string( 'HLT' ),
    triggerResults = cms.InputTag( "TriggerResults" ),
    triggerEvent   = cms.InputTag( "hltTriggerSummaryAOD" ),
    # trigger path
    addPathModuleTags = cms.bool( False ) # setting this 'True' stores the names of all modules as strings (~8kB/ev.)
)

