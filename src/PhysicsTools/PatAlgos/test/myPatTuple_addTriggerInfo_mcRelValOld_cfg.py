import os
from PhysicsTools.PatAlgos.patTemplate_cfg import *
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *
process.GlobalTag.globaltag = autoCond[ 'startup' ]
process.source.fileNames    = pickRelValInputFiles( formerVersion = True
                                                  , globalTag     = 'START311_V2'
                                                  )
process.options.wantSummary = False
process.out.fileName        = '%s/output/myPatTuple_addTriggerInfo_mcRelValOld.root'%( os.getenv( "CMSSW_BASE" ) )

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
process.moin = cleanMuonTriggerMatchPDSingleMu.clone()
process.tach = metTriggerMatchHLTMu20.clone()
switchOnTrigger( process )
# process.patTrigger.saveL1Refs = cms.bool( True )
# switchOnTrigger( process ) # to update event content
switchOnTriggerMatching( process )
switchOnTriggerStandAlone( process )
switchOnTriggerMatchingStandAlone( process )
switchOnTriggerMatchEmbedding( process )
switchOnTrigger( process, triggerProducer = 'hallo', triggerEventProducer = 'tschuess', hltProcess = '*' )
switchOnTriggerMatching( process, triggerMatchers = [ 'moin', 'tach' ], triggerProducer = 'hallo', triggerEventProducer = 'tschuess' )
switchOnTriggerStandAlone( process, triggerProducer = 'hallo', hltProcess = '*' )
switchOnTriggerMatchingStandAlone( process, triggerMatchers = [ 'moin', 'tach' ], triggerProducer = 'hallo' )
switchOnTriggerMatchEmbedding( process, triggerMatchers = [ 'moin', 'tach' ], triggerProducer = 'hallo' )
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
