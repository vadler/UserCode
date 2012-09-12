import os
from PhysicsTools.PatAlgos.patTemplate_cfg import *

## Options
process.options.wantSummary      = False
process.options.allowUnscheduled = cms.untracked.bool( False )

## Messaging
#process.Tracer = cms.Service( "Tracer" )

## Input
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
process.source.fileNames    = pickRelValInputFiles( relVal      = 'RelValProdTTbar'
                                                  , dataTier    = 'AODSIM'
                                                  )
process.source.skipBadFiles = cms.untracked.bool( True )

## Output
process.out.fileName        = '%s/output/myPatTuple_addTriggerInfo_mcRelVal.root'%( os.getenv( "CMSSW_BASE" ) )
process.out.outputCommands += [
  'keep edmTriggerResults_TriggerResults_*_HLT'
, 'keep *_hltTriggerSummaryAOD_*_*'
]

## Processing
process.load( "PhysicsTools.PatAlgos.patSequences_cff" )
process.p = cms.Path(
  process.patDefaultSequence
)

# Trigger
from PhysicsTools.PatAlgos.tools.trigTools import *
from PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff import *
process.hallo                     = patTrigger.clone()
process.hallo.addL1Algos          = cms.bool( True )
process.hallo.l1ExtraMu           = cms.InputTag( 'l1extraParticles' )
process.hallo.saveL1Refs          = cms.bool( True )
process.hallo.addPathModuleLabels = cms.bool( True )
process.tschuess           = patTriggerEvent.clone()
process.tschuess.condGtTag = cms.InputTag( 'conditionsInEdm' )
process.tschuess.l1GtTag   = cms.InputTag( 'gtDigis' )
process.moin = selectedMuonTriggerMatchPDSingleMu.clone()
process.tach = metTriggerMatchHLTMu17.clone()
switchOnTrigger( process )
process.patTrigger.saveL1Refs = cms.bool( True )
switchOnTrigger( process ) # to update event content
switchOnTriggerMatching( process )
switchOnTriggerStandAlone( process )
switchOnTriggerMatchingStandAlone( process )
switchOnTriggerMatchEmbedding( process )
switchOnTrigger( process, triggerProducer = 'hallo', triggerEventProducer = 'tschuess', hltProcess = '*' )
switchOnTriggerMatching( process, triggerMatchers = [ 'moin', 'tach' ], triggerProducer = 'hallo', triggerEventProducer = 'tschuess' )
switchOnTriggerStandAlone( process, triggerProducer = 'hallo', hltProcess = '*' )
switchOnTriggerMatchingStandAlone( process, triggerMatchers = [ 'moin', 'tach' ], triggerProducer = 'hallo' )
switchOnTriggerMatchEmbedding( process, triggerMatchers = [ 'moin', 'tach' ], triggerProducer = 'hallo' )
# print
# print 'Path p'
# print '--> %s'%( process.p )
print
print 'Sequence patDefaultSequenceTrigger'
print '--> %s'%( process.patDefaultSequenceTrigger )
print
print 'Sequence patDefaultSequenceTriggerEvent'
print '--> %s'%( process.patDefaultSequenceTriggerEvent )
print
print 'PoolOutputModule out.outputCommands'
print '--> %s'%( process.out.outputCommands )
