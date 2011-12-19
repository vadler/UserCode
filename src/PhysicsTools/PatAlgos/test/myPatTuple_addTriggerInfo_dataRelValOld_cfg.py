import os
from PhysicsTools.PatAlgos.patTemplate_cfg import *
cmsswVersion = 'CMSSW_5_0_0_pre7'
globalTag    = 'GR_R_50_V5'
process.GlobalTag.globaltag = '%s::All'%( globalTag )
process.source.fileNames    = pickRelValInputFiles( #formerVersion = True
                                                    cmsswVersion  = cmsswVersion
                                                  , relVal        = 'SingleMu'
                                                  , dataTier      = 'RECO'
                                                  , globalTag     = '%s_RelVal_mu2011A'%( globalTag )
                                                  , maxVersions   = 1
                                                  )
process.options.wantSummary = False
process.out.fileName        = '%s/output/myPatTuple_addTriggerInfo_dataRelValOld.root'%( os.getenv( "CMSSW_BASE" ) )

process.p = cms.Path(
  process.patDefaultSequence
)

process.out.outputCommands += [
  'keep edmTriggerResults_TriggerResults_*_HLT'
, 'keep *_hltTriggerSummaryAOD_*_*'
]

from PhysicsTools.PatAlgos.tools.coreTools import runOnData
runOnData( process )
process.patJetCorrFactors.levels = [ 'L1Offset'
                                   , 'L2Relative'
                                   , 'L3Absolute'
                                   , 'L2L3Residual'
                                   , 'L5Flavor'
                                   , 'L7Parton'
                                   ]
process.patJetCorrFactors.useRho = cms.bool( False )

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
print 'Sequence patDefaultSequenceTrigger'
print '--> %s'%( process.patDefaultSequenceTrigger )
print
print 'Sequence patDefaultSequenceTriggerEvent'
print '--> %s'%( process.patDefaultSequenceTriggerEvent )
print
print 'PoolOutputModule out.outputCommands'
print '--> %s'%( process.out.outputCommands )
