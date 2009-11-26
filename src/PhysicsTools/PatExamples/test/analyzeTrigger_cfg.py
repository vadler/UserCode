import FWCore.ParameterSet.Config as cms

process = cms.Process( "TEST2" )

process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool( True )
)

process.source = cms.Source( "PoolSource",
#     fileNames = cms.untracked.vstring( 'file:/afs/naf.desy.de/user/v/vadler/public/edmTrigger.root' )
    fileNames = cms.untracked.vstring( 'file:edmTrigger.root' )
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( -1 )
)

process.TFileService = cms.Service( "TFileService",
    fileName = cms.string( 'rootTrigger.root' )
)

process.triggerAnalysis = cms.EDAnalyzer( "TriggerAnalyzer",
    triggerEvent = cms.InputTag( "patTriggerEvent" ),
    muons        = cms.InputTag( "selectedLayer1Muons" ),
    muonMatch    = cms.string( 'muonTriggerMatchHLTMuons' ),
    muonID       = cms.uint32( 93 )
)

process.p = cms.Path(
    process.triggerAnalysis
)
