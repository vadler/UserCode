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

# # memory check
# process.SimpleMemoryCheck = cms.Service( "SimpleMemoryCheck",
# #     oncePerEventMode = cms.untracked.bool( True ),
#     ignoreTotal      = cms.untracked.int32( 0 )
# )

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

# # HLT analyzers
# process.load( "HLTrigger.HLTcore.hltEventAnalyzerAOD_cfi" )
# process.load( "HLTrigger.HLTcore.triggerSummaryAnalyzerAOD_cfi" )

# PAT Layer 0 & 1
process.load( "PhysicsTools.PatAlgos.patLayer0_cff" )
process.load( "PhysicsTools.PatAlgos.patLayer1_cff" )

process.p = cms.Path(
#     process.hltEventAnalyzerAOD       +
#     process.triggerSummaryAnalyzerAOD +
    process.patLayer0 *  
    process.patLayer1
)

# Output module configuration
process.out = cms.OutputModule("PoolOutputModule",
    fileName       = cms.untracked.string( '/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_2_2_0/output/PATLayer1_Output.fromAOD_full.root' ),
    SelectEvents   = cms.untracked.PSet(
        SelectEvents = cms.vstring( 'p' )
    ),
    outputCommands = cms.untracked.vstring( 'drop *' )
)
process.load( "PhysicsTools.PatAlgos.patLayer1_EventContent_cff" )
process.out.outputCommands.extend( process.patLayer1EventContent.outputCommands )

process.outpath = cms.EndPath( process.out )

