import FWCore.ParameterSet.Config as cms

process = cms.Process( "PAT" )

# initialize MessageLogger and output report
process.load( "FWCore.MessageLogger.MessageLogger_cfi" )
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.categories.append( 'PATSummaryTables' )
process.MessageLogger.cerr.INFO = cms.untracked.PSet(
    default          = cms.untracked.PSet(
        limit = cms.untracked.int32(0)
    ),
    PATSummaryTables = cms.untracked.PSet(
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
process.GlobalTag.globaltag = cms.string('STARTUP_V7::All')
process.load( "Configuration.StandardSequences.MagneticField_cff" )

# PAT Layer 0 & 1
process.load("PhysicsTools.PatAlgos.patSequences_cff")
process.load( "PhysicsTools.PatAlgos.triggerLayer0.triggerProducer_cff" )
process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff" )

process.p = cms.Path(
    process.patDefaultSequence *  
    process.patLayer0Trigger   *  ## FIXME
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
from PhysicsTools.PatAlgos.patEventContent_cff import *
patLayer1EventContentTrigger = [
    'keep *_patTrigger_*_*',
    'keep *_patTriggerEvent_*_*'
]
process.out.outputCommands += patEventContent
process.out.outputCommands += patLayer1EventContentTrigger

process.outpath = cms.EndPath( process.out )
