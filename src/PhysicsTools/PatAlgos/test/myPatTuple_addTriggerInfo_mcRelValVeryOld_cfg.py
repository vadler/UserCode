import os
from PhysicsTools.PatAlgos.patTemplate_cfg import *
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
cmsswVersion = 'CMSSW_5_1_2'
globalTag    = 'START50_V15A'
process.GlobalTag.globaltag = '%s::All'%( globalTag )
process.source.fileNames    = pickRelValInputFiles( #formerVersion = True
                                                    cmsswVersion  = cmsswVersion
                                                  , relVal        = 'RelValProdTTbar'
                                                  , dataTier      = 'AODSIM'
                                                  , globalTag     = globalTag
                                                  , maxVersions   = 1
                                                  )
process.source.skipBadFiles = cms.untracked.bool( True )
process.options.wantSummary = False
process.out.fileName        = '%s/output/myPatTuple_addTriggerInfo_mcRelValOld.root'%( os.getenv( "CMSSW_BASE" ) )

from PhysicsTools.PatAlgos.tools.coreTools import removeSpecificPATObjects
removeSpecificPATObjects( process
                        , names = [ 'Taus' ] # Tau RECO differs in CMSSW_5_1_X
                        )
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
switchOnTrigger( process )
process.patTrigger.addL1Algos          = cms.bool( True )
process.patTrigger.l1ExtraMu           = cms.InputTag( 'l1extraParticles' )
process.patTrigger.saveL1Refs          = cms.bool( True )
process.patTrigger.addPathModuleLabels = cms.bool( True )
process.patTriggerEvent.condGtTag = cms.InputTag( 'conditionsInEdm' )
process.patTriggerEvent.l1GtTag   = cms.InputTag( 'gtDigis' )
removeCleaningFromTriggerMatching( process )
switchOnTrigger( process )
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import run52xOn51xTrigger
run52xOn51xTrigger( process )
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
