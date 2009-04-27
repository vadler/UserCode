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
#         'file:/afs/cern.ch/user/h/hegner/public/test2.root' 
#         'rfio:/castor/cern.ch/cms/store/relval/CMSSW_3_1_0_pre4/RelValTTbar/GEN-SIM-RECO/IDEAL_30X_v1/0003/00E48100-3A16-DE11-A693-001617DBCF6A.root'
        '/store/relval/CMSSW_3_1_0_pre6/RelValTTbar/GEN-SIM-RECO/IDEAL_31X_v1/0002/12B74DD2-F932-DE11-921C-0016177CA7A0.root',
        '/store/relval/CMSSW_3_1_0_pre6/RelValTTbar/GEN-SIM-RECO/IDEAL_31X_v1/0002/406012BC-FB32-DE11-AD0F-000423D94990.root',
        '/store/relval/CMSSW_3_1_0_pre6/RelValTTbar/GEN-SIM-RECO/IDEAL_31X_v1/0002/50D4BADB-FA32-DE11-BA01-000423D98DC4.root',
        '/store/relval/CMSSW_3_1_0_pre6/RelValTTbar/GEN-SIM-RECO/IDEAL_31X_v1/0002/6E122CEB-FA32-DE11-9055-000423D99264.root',
        '/store/relval/CMSSW_3_1_0_pre6/RelValTTbar/GEN-SIM-RECO/IDEAL_31X_v1/0002/8823A234-FC32-DE11-A251-001617E30F58.root',
        '/store/relval/CMSSW_3_1_0_pre6/RelValTTbar/GEN-SIM-RECO/IDEAL_31X_v1/0002/98E8674A-FB32-DE11-BD4A-000423D99896.root',
        '/store/relval/CMSSW_3_1_0_pre6/RelValTTbar/GEN-SIM-RECO/IDEAL_31X_v1/0002/9A2FD94C-2033-DE11-B10A-001617DF785A.root',
        '/store/relval/CMSSW_3_1_0_pre6/RelValTTbar/GEN-SIM-RECO/IDEAL_31X_v1/0002/F64ABC78-1733-DE11-866C-000423D98E6C.root'
    )
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( 100 )
)

process.load( "Configuration.StandardSequences.Geometry_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.globaltag = cms.string( 'IDEAL_31X::All' )
process.load( "Configuration.StandardSequences.MagneticField_cff" )

# # HLT analyzers
# process.load( "HLTrigger.HLTcore.hltEventAnalyzerAOD_cfi" )
# process.load( "HLTrigger.HLTcore.triggerSummaryAnalyzerAOD_cfi" )

# PAT Layer 0 & 1
process.load( "PhysicsTools.PatAlgos.patSequences_cff" )
# replacements to make the muons work
process.allLayer1Jets.addDiscriminators    = False
process.allLayer1Jets.discriminatorSources = []

process.p = cms.Path(
#     process.hltEventAnalyzerAOD       +
#     process.triggerSummaryAnalyzerAOD +
    process.patDefaultSequence
)

# Output module configuration
from PhysicsTools.PatAlgos.patEventContent_cff import *
process.out = cms.OutputModule( "PoolOutputModule",
    fileName       = cms.untracked.string( '/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_1_0_pre6/output/my_PatLayer1_fromAOD_full.root' ),
    SelectEvents   = cms.untracked.PSet(
        SelectEvents = cms.vstring( 'p' )
    ),
    outputCommands = cms.untracked.vstring( 'drop *', *patEventContent )
)

# Trigger
from PhysicsTools.PatAlgos.tools.trigTools import *
### switch START ###
# ## no stand-alone trigger objects
# switchOnTrigger( process )
## all; default to run myTriggerTest_cfg.py afterwards
switchOnTriggerAll( process )
switchOnTriggerMatchEmbedding( process )
process.out.outputCommands += [ 'keep edmTriggerResults_TriggerResults_*_HLT'
                              , 'keep *_hltTriggerSummaryAOD_*_*'
                              ]
# ## stand-alone trigger objects only
# switchOnTriggerStandAlone( process )
# ## embedded trigger object matches only
# switchOnTriggerMatchEmbedding( process )
### switch END ###

process.outpath = cms.EndPath(
    process.out
)
