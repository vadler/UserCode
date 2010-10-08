import os
cmsswBase = os.getenv( "CMSSW_BASE" )

from PhysicsTools.PatAlgos.patTemplate_cfg import *
process.GlobalTag.globaltag = 'START38_V12::All'
process.out.fileName        = '%s/output/myPatTuple_addTriggerInfo.root'%( cmsswBase )
process.options.wantSummary = False

# # memory check
# process.SimpleMemoryCheck = cms.Service( "SimpleMemoryCheck"
# # , oncePerEventMode = cms.untracked.bool( True )
# , ignoreTotal      = cms.untracked.int32( 0 )
# )

process.p = cms.Path(
  process.patDefaultSequence
)

process.out.outputCommands += [
  'keep edmTriggerResults_TriggerResults_*_HLT'
, 'keep *_hltTriggerSummaryAOD_*_*'
]

# from PhysicsTools.PatAlgos.tools.coreTools import removeCleaning
# removeCleaning( process )

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
process.moin = cleanMuonTriggerMatchPDMu.clone()
process.tach = metTriggerMatchHLTMu3.clone()
# process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cfi" )
# process.patTrigger.saveL1Refs = cms.bool( True )
switchOnTrigger( process )
switchOnTriggerMatching( process )
switchOnTriggerStandAlone( process )
switchOnTriggerMatchingStandAlone( process )
switchOnTriggerMatchEmbedding( process )
switchOnTrigger( process, 'hallo', 'tschuess' )
switchOnTriggerMatching( process, [ 'moin', 'tach' ], 'hallo', 'tschuess' )
switchOnTriggerStandAlone( process, 'hallo' )
switchOnTriggerMatchingStandAlone( process, [ 'moin', 'tach' ], 'hallo' )
switchOnTriggerMatchEmbedding( process, [ 'moin', 'tach' ], 'hallo' )
# removeCleaningFromTriggerMatching( process )
# print
# print 'Path p'
# print '--> %s'%( process.p )
print
print 'Sequence patTriggerSequence'
print '--> %s'%( process.patTriggerSequence )
print
print 'Sequence patTriggerEventSequence'
print '--> %s'%( process.patTriggerEventSequence )
print
print 'PoolOutputModule out.outputCommands'
print '--> %s'%( process.out.outputCommands )
