from PhysicsTools.PatAlgos.patTemplate_cfg import *
process.GlobalTag.globaltag = 'START38_V9::All'
process.out.fileName        = '/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_9_0_pre2/output/myPatTuple_addTriggerInfo.root'
process.options.wantSummary = False

# # memory check
# process.SimpleMemoryCheck = cms.Service( "SimpleMemoryCheck"
# # , oncePerEventMode = cms.untracked.bool( True )
# , ignoreTotal      = cms.untracked.int32( 0 )
# )

process.p = cms.Path(
  process.patDefaultSequence
)

# Trigger
from PhysicsTools.PatAlgos.tools.trigTools import *
# from PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff import *
# process.hallo                     = patTrigger.clone()
# process.hallo.saveL1Refs          = cms.bool( True )
# process.hallo.addL1Algos          = cms.bool( True )
# process.hallo.addPathModuleLabels = cms.bool( True )
# process.tschuess           = patTriggerEvent.clone()
# process.tschuess.condGtTag = cms.InputTag( 'conditionsInEdm' )
# process.tschuess.l1GtTag   = cms.InputTag( 'gtDigis' )
# process.moin = cleanMuonTriggerMatchPDMu.clone()
# process.tach = metTriggerMatchHLTMu3.clone()
# process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cfi" )
# process.patTrigger.saveL1Refs = cms.bool( True )
switchOnTrigger( process )
switchOnTriggerMatching( process )
switchOnTriggerStandAlone( process )
switchOnTriggerMatchingStandAlone( process )
switchOnTriggerMatchEmbedding( process )
# switchOnTrigger( process, 'hallo', 'tschuess' )
# switchOnTriggerMatching( process, [ 'moin', 'tach' ], 'hallo', 'tschuess' )
# switchOnTriggerStandAlone( process, 'hallo' )
# switchOnTriggerMatchingStandAlone( process, [ 'moin', 'tach' ], 'hallo' )
# switchOnTriggerMatchEmbedding( process, [ 'moin', 'tach' ], 'hallo' )
print
print 'patDefaultSequence'
print '--> %s'%( process.patDefaultSequence )
print
print 'patTriggerSequence'
print '--> %s'%( process.patTriggerSequence )
print
print 'patTriggerEventSequence'
print '--> %s'%( process.patTriggerEventSequence )
print
print 'out.outputCommands'
print '--> %s'%( process.out.outputCommands )
