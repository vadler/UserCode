from PhysicsTools.PatAlgos.patTemplate_cfg import *
#process.GlobalTag.globaltag = 'STARTUP31X_V8::All'
#process.source.fileNames    = [ '/store/relval/CMSSW_3_4_0_pre1/RelValTTbar/GEN-SIM-RECO/STARTUP31X_V8-v1/0007/BAB355DB-DFB4-DE11-AD73-0019B9F72F97.root' ]
# process.maxEvents.input     = ...
# process.out.outputCommands  = [ ... ]
process.out.fileName        = '/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_5_6/output/myPatLayer1_fromAOD_full.root'
# process.options.wantSummary = False

# # memory check
# process.SimpleMemoryCheck = cms.Service( "SimpleMemoryCheck",
# #     oncePerEventMode = cms.untracked.bool( True ),
#     ignoreTotal      = cms.untracked.int32( 0 )
# )

# # HLT analyzers
# process.load( "HLTrigger.HLTcore.hltEventAnalyzerAOD_cfi" )
# process.load( "HLTrigger.HLTcore.triggerSummaryAnalyzerAOD_cfi" )

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
                              , 'keep L1GlobalTriggerReadoutRecord_gtDigis_*_*'
                              , 'keep *_l1GtRecord_*_*'
                              , 'keep *_l1extraParticles_*_*'
                              ]
# ## stand-alone trigger objects only
# switchOnTriggerStandAlone( process )
# ## embedded trigger object matches only
# switchOnTriggerMatchEmbedding( process )
### switch END ###
