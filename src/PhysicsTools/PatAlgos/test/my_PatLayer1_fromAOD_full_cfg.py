from PhysicsTools.PatAlgos.patTemplate_cfg import *
# process.GlobalTag.globaltag =  ...
# process.source.fileNames    = [ ... ]
# process.maxEvents.input     = ...
# process.out.outputCommands  = [ ... ]
process.out.fileName        = '/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_3_0_pre3/output/my_PatLayer1_fromAOD_full.root'
# process.options.wantSummary = False

# # memory check
# process.SimpleMemoryCheck = cms.Service( "SimpleMemoryCheck",
# #     oncePerEventMode = cms.untracked.bool( True ),
#     ignoreTotal      = cms.untracked.int32( 0 )
# )

# # HLT analyzers
# process.load( "HLTrigger.HLTcore.hltEventAnalyzerAOD_cfi" )
# process.load( "HLTrigger.HLTcore.triggerSummaryAnalyzerAOD_cfi" )

# PAT
process.load("PhysicsTools.PatAlgos.patSequences_cff")

process.p = cms.Path(
#     process.hltEventAnalyzerAOD       +
#     process.triggerSummaryAnalyzerAOD +
    process.patDefaultSequence
)

# Trigger
from PhysicsTools.PatAlgos.tools.trigTools import *
### switch START ###
# ## no stand-alone trigger objects
# switchOnTrigger( process )
## all; default to run myTriggerTest_cfg.py afterwards
switchOnTriggerAll( process )
process.patTrigger.addPathModuleLabels = True
switchOnTriggerMatchEmbedding( process )
process.out.outputCommands += [ 'keep edmTriggerResults_TriggerResults_*_HLT'
                              , 'keep *_hltTriggerSummaryAOD_*_*'
                              ]
# ## stand-alone trigger objects only
# switchOnTriggerStandAlone( process )
# ## embedded trigger object matches only
# switchOnTriggerMatchEmbedding( process )
### switch END ###
