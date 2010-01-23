import FWCore.ParameterSet.Config as cms

process = cms.Process( "TEST" )

process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool( True )
)

process.source = cms.Source( "PoolSource",
    fileNames = cms.untracked.vstring(
        'file:/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_5_0_pre3/output/myPatLayer1Test_fromAOD_full.root'
    )
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( -1 )
)

process.TFileService = cms.Service( "TFileService",
    fileName = cms.string( '/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_5_0_pre3/output/myTriggerTestTest.root' )
)

process.triggerTest = cms.EDAnalyzer( "myTriggerTest",
    hltProcessName  = cms.string( 'HLT' ),
    triggerResults  = cms.InputTag( "TriggerResults" ),
    triggerEvent    = cms.InputTag( "hltTriggerSummaryAOD" ),
    patProcessName  = cms.string( 'PAT' ),
    patTrigger      = cms.InputTag( "patTrigger" ),
    patTriggerEvent = cms.InputTag( "patTriggerEventTest" ),
    # general
    testPathModuleTags = cms.bool( True ),
    displayNumbers     = cms.bool( True ),
    # objects
    displayObjects = cms.bool( True ),
    # stand-alone objects
    displayObjectsStandAlone = cms.bool( True ),
    # filters
    displayFilters = cms.bool( True ),
    # paths
    displayPaths = cms.bool( True ),
    # event
    displayEvent = cms.bool( True ),
    # matching
    displayMatches = cms.bool( True ),
    # embedding
    displayEmbedding      = cms.bool( True ),
    patPhotons            = cms.InputTag( "cleanPatPhotons" ),
    patElectrons          = cms.InputTag( "cleanPatElectrons" ),
    patMuons              = cms.InputTag( "cleanPatMuons" ),
    patTaus               = cms.InputTag( "cleanPatTaus" ),
    patJets               = cms.InputTag( "cleanPatJets" ),
    patMETs               = cms.InputTag( "patMETs" ),
    patPhotonsEmbedding   = cms.InputTag( "cleanPatPhotonsTriggerTestMatch" ),
    patElectronsEmbedding = cms.InputTag( "cleanPatElectronsTriggerTestMatch" ),
    patMuonsEmbedding     = cms.InputTag( "cleanPatMuonsTriggerTestMatch" ),
    patTausEmbedding      = cms.InputTag( "cleanPatTausTriggerTestMatch" ),
    patJetsEmbedding      = cms.InputTag( "cleanPatJetsTriggerTestMatch" ),
    patMETsEmbedding      = cms.InputTag( "patMETsTriggerTestMatch" )
)

process.p = cms.Path(
    process.triggerTest
)
