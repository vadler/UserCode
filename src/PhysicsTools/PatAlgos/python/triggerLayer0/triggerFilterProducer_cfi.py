import FWCore.ParameterSet.Config as cms

patTriggerFilters = cms.EDProducer( "PATTriggerFilterProducer",
    processName  = cms.string( 'HLT' ),
    triggerEvent = cms.InputTag( "hltTriggerSummaryAOD" )
)

