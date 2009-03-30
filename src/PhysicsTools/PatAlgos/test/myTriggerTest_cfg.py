import FWCore.ParameterSet.Config as cms

process = cms.Process( "TEST" )

process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool( True )
)

process.source = cms.Source( "PoolSource",
    fileNames = cms.untracked.vstring(
        'file:/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_2_2_7/output/my_PatLayer1_fromAOD_full.root'
    )
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( -1 )
)

process.TFileService = cms.Service( "TFileService",
    fileName = cms.string( '/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_2_2_7/output/myTriggerTest.root' )
)

process.triggerTest = cms.EDAnalyzer( "myTriggerTest",
    hltProcessName           = cms.string( 'HLT' ),
    triggerResults           = cms.InputTag( "TriggerResults" ),
    triggerEvent             = cms.InputTag( "hltTriggerSummaryAOD" ),
    patProcessName           = cms.string( 'PAT' ),
    patTrigger               = cms.InputTag( "patTrigger" ),
    patTriggerEvent          = cms.InputTag( "patTriggerEvent" ),
    testPathModuleTags       = cms.bool( False ),
    displayNumbers           = cms.bool( False ),
    displayObjects           = cms.bool( False ),
    displayObjectsStandAlone = cms.bool( False ),
    displayFilters           = cms.bool( False ),
    displayPaths             = cms.bool( False ),
    displayEvent             = cms.bool( False ),
    displayMatches           = cms.bool( True )
)

process.p = cms.Path(
    process.triggerTest
)
