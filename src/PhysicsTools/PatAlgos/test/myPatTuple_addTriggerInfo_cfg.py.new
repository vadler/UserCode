from PhysicsTools.PatAlgos.patTemplate_cfg import *
# process.GlobalTag.globaltag = ...
# process.source.fileNames    = [ ... ]
process.maxEvents.input     = 100
# process.out.outputCommands  = [ ... ]
process.out.fileName        = '/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_8_0_pre7/output/myPatTuple_addTriggerInfo.root'
# process.options.wantSummary = False

# # memory check
# process.SimpleMemoryCheck = cms.Service( "SimpleMemoryCheck"
# # , oncePerEventMode = cms.untracked.bool( True )
# , ignoreTotal      = cms.untracked.int32( 0 )
# )

# # HLT analyzers
# process.load( "HLTrigger.HLTcore.hltEventAnalyzerAOD_cfi" )
# process.load( "HLTrigger.HLTcore.triggerSummaryAnalyzerAOD_cfi" )

process.p = cms.Path(
# + process.hltEventAnalyzerAOD
# + process.triggerSummaryAnalyzerAOD
# + process.patDefaultSequence
  process.patDefaultSequence
)

# Trigger
from PhysicsTools.PatAlgos.tools.trigTools import *
### switch START ###
# ## no stand-alone trigger objects
# switchOnTrigger( process )
## all; default to run myTriggerTest_cfg.py afterwards
switchOnTriggerAll( process )
process.patTrigger.addL1Algos          = cms.bool( True )
process.patTrigger.addPathModuleLabels = cms.bool( True )
process.patTrigger.saveL1Refs          = cms.bool( True )
switchOnTriggerMatchEmbedding( process )
process.out.outputCommands += [
  'keep edmTriggerResults_TriggerResults_*_HLT'
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
