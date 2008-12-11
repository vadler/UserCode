import FWCore.ParameterSet.Config as cms

patTriggerFilters = cms.EDProducer( "PATTriggerFilterProducer",
    processName    = cms.string( 'HLT' ),
    triggerResults = cms.InputTag( "TriggerResults" ),
    triggerEvent   = cms.InputTag( "hltTriggerSummaryAOD" )
)

