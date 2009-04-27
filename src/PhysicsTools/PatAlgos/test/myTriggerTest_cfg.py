import FWCore.ParameterSet.Config as cms

process = cms.Process( "TEST" )

process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool( True )
)

process.source = cms.Source( "PoolSource",
    fileNames = cms.untracked.vstring(
        'file:/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_1_0_pre6/output/my_PatLayer1_fromAOD_full.root'
    )
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( -1 )
)

process.TFileService = cms.Service( "TFileService",
    fileName = cms.string( '/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_1_0_pre6/output/myTriggerTest.root' )
)

process.triggerTest = cms.EDAnalyzer( "myTriggerTest",
    hltProcessName  = cms.string( 'HLT' ),
    triggerResults  = cms.InputTag( "TriggerResults" ),
    triggerEvent    = cms.InputTag( "hltTriggerSummaryAOD" ),
    patProcessName  = cms.string( 'PAT' ),
    patTrigger      = cms.InputTag( "patTrigger" ),
    patTriggerEvent = cms.InputTag( "patTriggerEvent" ),
    # general
    testPathModuleTags = cms.bool( False ),
    displayNumbers     = cms.bool( False ),
    # objects
    displayObjects = cms.bool( False ),
    # stand-alone objects
    displayObjectsStandAlone = cms.bool( False ),
    # filters
    displayFilters = cms.bool( False ),
    # paths
    displayPaths = cms.bool( False ),
    # event
    displayEvent = cms.bool( False ),
    # matching
    displayMatches = cms.bool( True ),
    # embedding
    displayEmbedding      = cms.bool( True ),
    patPhotons            = cms.InputTag( "cleanLayer1Photons" ),
    patElectrons          = cms.InputTag( "cleanLayer1Electrons" ),
    patMuons              = cms.InputTag( "cleanLayer1Muons" ),
    patTaus               = cms.InputTag( "cleanLayer1Taus" ),
    patJets               = cms.InputTag( "cleanLayer1Jets" ),
    patMETs               = cms.InputTag( "layer1METs" ),
    patPhotonsEmbedding   = cms.InputTag( "cleanLayer1PhotonsTriggerMatch" ),
    patElectronsEmbedding = cms.InputTag( "cleanLayer1ElectronsTriggerMatch" ),
    patMuonsEmbedding     = cms.InputTag( "cleanLayer1MuonsTriggerMatch" ),
    patTausEmbedding      = cms.InputTag( "cleanLayer1TausTriggerMatch" ),
    patJetsEmbedding      = cms.InputTag( "cleanLayer1JetsTriggerMatch" ),
    patMETsEmbedding      = cms.InputTag( "layer1METsTriggerMatch" )
)

process.p = cms.Path(
    process.triggerTest
)
