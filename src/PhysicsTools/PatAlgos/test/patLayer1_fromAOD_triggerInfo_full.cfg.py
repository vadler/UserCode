import FWCore.ParameterSet.Config as cms

process = cms.Process( "PAT" )

# initialize MessageLogger and output report
process.load( "FWCore.MessageLogger.MessageLogger_cfi" )
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.categories.append( 'PATLayer0Summary' )
process.MessageLogger.cerr.INFO = cms.untracked.PSet(
    default          = cms.untracked.PSet(
        limit = cms.untracked.int32(0)
    ),
    PATLayer0Summary = cms.untracked.PSet(
        limit = cms.untracked.int32(-1)
    )
)
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool( True )
)

# source
process.source = cms.Source("PoolSource", 
    fileNames = cms.untracked.vstring(
        'file:/afs/cern.ch/cms/PRS/top/cmssw-data/relval200-for-pat-testing/FullSimTTBar-2_2_X_2008-11-03-STARTUP_V7-AODSIM.100.root'
    )
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( 100 )
)

process.load( "Configuration.StandardSequences.Geometry_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.globaltag = cms.string( 'IDEAL_V9::All' )
process.load( "Configuration.StandardSequences.MagneticField_cff" )

# PAT Layer 0 & 1
process.load( "PhysicsTools.PatAlgos.patLayer0_cff" )
process.load( "PhysicsTools.PatAlgos.triggerLayer0.triggerProducer_cff" )
process.load( "PhysicsTools.PatAlgos.patLayer1_cff" )
process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff" )

process.p = cms.Path(
    process.patLayer0        *  
    process.patLayer0Trigger *
    process.patLayer1        *  
    process.patLayer1Trigger
)

# Output module configuration
process.out = cms.OutputModule("PoolOutputModule",
    fileName       = cms.untracked.string( 'PATLayer1_Output.fromAOD_triggerInfo_full.root' ),
    SelectEvents   = cms.untracked.PSet(
        SelectEvents = cms.vstring( 'p' )
    ),
    outputCommands = cms.untracked.vstring( 'drop *' )
)
process.patLayer1EventContentTrigger = cms.PSet(
    outputCommands = cms.untracked.vstring(
        'keep *_patTrigger_*_*',
        'keep *_patTriggerEvent_*_*'
    )
)
process.load( "PhysicsTools.PatAlgos.patLayer1_EventContent_cff" )
process.out.outputCommands.extend( process.patLayer1EventContent.outputCommands )
process.out.outputCommands.extend( process.patLayer1EventContentTrigger.outputCommands )

process.outpath = cms.EndPath( process.out )
