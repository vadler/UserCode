import FWCore.ParameterSet.Config as cms

process = cms.Process( "PAT" )

# initialize MessageLogger and output report
process.load( "FWCore.MessageLogger.MessageLogger_cfi" )
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.categories.append( 'PATSummaryTables' )
process.MessageLogger.cerr.INFO = cms.untracked.PSet(
    default          = cms.untracked.PSet(
        limit = cms.untracked.int32( -1 )
    ),
    PATSummaryTables = cms.untracked.PSet(
        limit = cms.untracked.int32( -1 )
    )
)
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool( True )
)

# # memory check
# process.SimpleMemoryCheck = cms.Service( "SimpleMemoryCheck",
# #     oncePerEventMode = cms.untracked.bool( True ),
#     ignoreTotal      = cms.untracked.int32( 0 )
# )

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

# # HLT analyzers
# process.load( "HLTrigger.HLTcore.hltEventAnalyzerAOD_cfi" )
# process.load( "HLTrigger.HLTcore.triggerSummaryAnalyzerAOD_cfi" )

# PAT Layer 0 & 1
process.load( "PhysicsTools.PatAlgos.patSequences_cff" )

process.p = cms.Path(
#     process.hltEventAnalyzerAOD       +
#     process.triggerSummaryAnalyzerAOD +
    process.patDefaultSequence
)

# Output module configuration
from PhysicsTools.PatAlgos.patEventContent_cff import *
process.out = cms.OutputModule( "PoolOutputModule",
    fileName       = cms.untracked.string( '/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_1_0_pre4/output/my_PatLayer1_fromAOD_full.root' ),
    SelectEvents   = cms.untracked.PSet(
        SelectEvents = cms.vstring( 'p' )
    ),
    outputCommands = cms.untracked.vstring( 'drop *', *patEventContent )
)

# Trigger
from PhysicsTools.PatAlgos.tools.trigTools import *
switchOffTriggerMatchingOld(process)
switchOnTrigger(process)
process.out.outputCommands += [ 'keep edmTriggerResults_TriggerResults_*_HLT'
                              , 'keep *_hltTriggerSummaryAOD_*_*' ]

process.outpath = cms.EndPath(
    process.out
)
