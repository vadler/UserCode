import FWCore.ParameterSet.Config as cms

process = cms.Process( "PAT" )

# initialize MessageLogger and output report
process.load( "FWCore.MessageLogger.MessageLogger_cfi" )
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.categories.append( 'PATSummaryTables' )
process.MessageLogger.cerr.INFO = cms.untracked.PSet(
    default          = cms.untracked.PSet(
        limit = cms.untracked.int32( 0 )
    ),
    PATSummaryTables = cms.untracked.PSet(
        limit = cms.untracked.int32( -1 )
    )
)
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool( True )
)

# source
process.source = cms.Source( "PoolSource", 
    fileNames = cms.untracked.vstring(
        'rfio:/castor/cern.ch/cms/store/relval/CMSSW_3_1_0_pre4/RelValTTbar/GEN-SIM-RECO/IDEAL_30X_v1/0003/00E48100-3A16-DE11-A693-001617DBCF6A.root'
    )
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( 100 )
)

process.load( "Configuration.StandardSequences.Geometry_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.globaltag = cms.string( 'IDEAL_30X::All' )
process.load( "Configuration.StandardSequences.MagneticField_cff" )

# PAT Layer 0 & 1
process.load( "PhysicsTools.PatAlgos.patSequences_cff" )

process.p = cms.Path(
    process.patDefaultSequence
)

# Output module configuration
from PhysicsTools.PatAlgos.patEventContent_cff import *
process.out = cms.OutputModule( "PoolOutputModule",
    fileName       = cms.untracked.string( 'PATLayer1_Output.fromAOD_triggerInfo_full.root' ),
    SelectEvents   = cms.untracked.PSet(
        SelectEvents = cms.vstring( 'p' )
    ),
    outputCommands = cms.untracked.vstring( 'drop *', *patEventContent )
)

# Trigger
from PhysicsTools.PatAlgos.tools.trigTools import *
switchOffTriggerMatchingOld( process )
switchOnTrigger( process )

process.outpath = cms.EndPath(
    process.out
)
