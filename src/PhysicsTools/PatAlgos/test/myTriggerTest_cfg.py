import FWCore.ParameterSet.Config as cms

process = cms.Process( "TEST" )

process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool( True )
)

process.source = cms.Source( "PoolSource",
    fileNames = cms.untracked.vstring(
        'file:/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_2_2_3/output/PATLayer1_Output.fromAOD_full.root'
    )
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( 1 )
)

process.TFileService = cms.Service( "TFileService",
    fileName = cms.string( '/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_2_2_3/output/myTriggerTest.root' )
)

process.test = cms.EDAnalyzer( "myTriggerTest",
    hltProcessName = cms.string( 'HLT' ),
    triggerResults = cms.InputTag( "TriggerResults" ),
    triggerEvent   = cms.InputTag( "hltTriggerSummaryAOD" ),
    patProcessName = cms.string( 'PAT' ),
    layer0Trigger  = cms.InputTag( "patTrigger" ),
    layer1Trigger  = cms.InputTag( "patTriggerEvent" )
)

process.p = cms.Path(
    process.test
)
