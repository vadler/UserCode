import os
from PhysicsTools.PatAlgos.patTemplate_cfg import *

# Steering
cmsswVersion = 'CMSSW_6_2_0_pre4'
globalTag    = 'PRE_ST61_V1'

## Options
process.options.wantSummary      = True
process.options.allowUnscheduled = cms.untracked.bool( True )

## Input
#from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
#process.source.fileNames    = pickRelValInputFiles( cmsswVersion  = cmsswVersion
                                                  #, relVal        = 'RelValProdTTbar'
                                                  #, dataTier      = 'AODSIM'
                                                  #, globalTag     = globalTag
                                                  #, maxVersions   = 1
                                                  #)
process.source.fileNames = cms.untracked.vstring( '/store/relval/CMSSW_6_2_0_pre4/RelValProdTTbar/AODSIM/PRE_ST61_V1-v1/00000/E2A9A526-FB90-E211-AA90-003048F1748A.root' )
process.maxEvents.input = 100
process.source.skipBadFiles = cms.untracked.bool( True )

# Messaging
if process.maxEvents.input.value() <= 1:
    process.Tracer = cms.Service( "Tracer" )

## Output
process.out.fileName = '%s/output/myPatTupleUnscheduled_addTriggerInfo_mcRelValFormer.root'%( os.getenv( "CMSSW_BASE" ) )
process.out.outputCommands = cms.untracked.vstring('drop *' )
process.out.outputCommands.append( 'keep edmTriggerResults_TriggerResults_*_*' )
process.out.outputCommands.append( 'keep *_hltTriggerSummaryAOD_*_*' )

## Processing
process.load("PhysicsTools.PatAlgos.producersLayer1.patCandidates_cff")
process.load("PhysicsTools.PatAlgos.selectionLayer1.selectedPatCandidates_cff")
#process.p = cms.Path(
  #process.selectedPatCandidateSummary
#)

# Trigger
from PhysicsTools.PatAlgos.tools.trigTools import *
#switchOnTrigger( process, path = 'p' )
switchOnTrigger( process )
switchOnTriggerMatching( process )
switchOnTriggerStandAlone( process )
switchOnTriggerMatchingStandAlone( process )
switchOnTriggerMatchEmbedding( process )
from PhysicsTools.PatAlgos.triggerLayer1.triggerMatcherExamples_cfi import *
process.moin = somePatMuonTriggerMatchPDSingleMu.clone()
process.tach = somePatMetTriggerMatchHLTMu17.clone()
switchOnTrigger( process, triggerProducer = 'hallo', triggerEventProducer = 'tschuess', hltProcess = '*' )
process.hallo.addL1Algos          = cms.bool( True )
process.hallo.l1ExtraMu           = cms.InputTag( 'l1extraParticles' )
process.hallo.saveL1Refs          = cms.bool( True )
process.hallo.addPathModuleLabels = cms.bool( True )
process.tschuess.condGtTag = cms.InputTag( 'conditionsInEdm' )
process.tschuess.l1GtTag   = cms.InputTag( 'gtDigis' )
switchOnTrigger( process, triggerProducer = 'hallo', triggerEventProducer = 'tschuess', hltProcess = '*' )
switchOnTriggerMatching( process, triggerMatchers = [ 'moin', 'tach' ], triggerProducer = 'hallo', triggerEventProducer = 'tschuess', hltProcess = '*' )
switchOnTriggerStandAlone( process, triggerProducer = 'hallo', hltProcess = '*' )
switchOnTriggerMatchingStandAlone( process, triggerMatchers = [ 'moin', 'tach' ], triggerProducer = 'hallo', hltProcess = '*' )
switchOnTriggerMatchEmbedding( process, triggerMatchers = [ 'moin', 'tach' ], triggerProducer = 'hallo', hltProcess = '*' )
print 'PoolOutputModule out.outputCommands'
print '--> %s'%( process.out.outputCommands )
