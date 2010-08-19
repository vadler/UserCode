from PhysicsTools.PatAlgos.patTemplate_cfg import *
# process.GlobalTag.globaltag = ...
# process.source.fileNames    = [ ... ]
process.maxEvents.input     = 100
# process.out.outputCommands  = [ ... ]
process.out.fileName        = '/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_9_0_pre2/output/myPatTuple_addTriggerInfo.root'
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
#   process.hltEventAnalyzerAOD
# + process.triggerSummaryAnalyzerAOD
# + process.patDefaultSequence
  process.patDefaultSequence
)

# Trigger
from PhysicsTools.PatAlgos.tools.trigTools import *
from PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff import *
process.hallo                     = patTrigger.clone()
process.hallo.saveL1Refs          = cms.bool( True )
process.hallo.addL1Algos          = cms.bool( True )
process.hallo.addPathModuleLabels = cms.bool( True )
process.tschuess           = patTriggerEvent.clone()
process.tschuess.condGtTag = cms.InputTag( 'conditionsInEdm' )
process.tschuess.l1GtTag   = cms.InputTag( 'gtDigis' )
switchOnTrigger( process, None, 'hallo', 'tschuess', None )
switchOnTriggerStandAlone( process, None, 'hallo', None )
print process.patDefaultSequence
print process.out.outputCommands
process.maxEvents.input = 10
# switchOnTrigger( process )
# switchOnTriggerStandAlone( process )
# process.patTrigger.addL1Algos          = cms.bool( True )
# process.patTrigger.saveL1Refs          = cms.bool( True )
# process.patTrigger.addPathModuleLabels = cms.bool( True )
# process.patTriggerEvent.condGtTag = cms.InputTag( 'conditionsInEdm' )
# process.patTriggerEvent.l1GtTag   = cms.InputTag( 'gtDigis' )
# switchOnTriggerMatchEmbedding( process )
