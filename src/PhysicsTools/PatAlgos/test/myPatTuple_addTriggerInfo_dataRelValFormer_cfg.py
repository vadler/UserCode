import os
import FWCore.ParameterSet.Config as cms

# Steering
cmsswVersion = 'CMSSW_6_1_0_pre6'
globalTag    = 'GR_R_61_V3'
condition    = 'com10_7E33v3'

process = cms.Process("PAT")

## Options
process.options = cms.untracked.PSet(
  wantSummary      = cms.untracked.bool( False )
, allowUnscheduled = cms.untracked.bool( False )
)

## Messaging
process.load( "FWCore.MessageLogger.MessageLogger_cfi" )
#process.Tracer = cms.Service( "Tracer" )

## Conditions
process.load( "Configuration.Geometry.GeometryIdeal_cff" )
process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag( process.GlobalTag, 'auto:%s'%( condition ) )

## Input
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
process.source = cms.Source(
  "PoolSource"
, fileNames = cms.untracked.vstring( pickRelValInputFiles( cmsswVersion  = cmsswVersion
                                                         , relVal        = 'SingleMu'
                                                         , dataTier      = 'RECO'
                                                         , globalTag     = '%s_RelVal_mu2012A'%( globalTag )
                                                         , maxVersions   = 1
                                                         )
                                   )
, skipBadFiles = cms.untracked.bool( True )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 100 )
)

## Output
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContentNoCleaning
process.out = cms.OutputModule(
  "PoolOutputModule"
, SelectEvents = cms.untracked.PSet(
    SelectEvents = cms.vstring(
      'p'
    )
  )
, fileName = cms.untracked.string( '%s/output/myPatTuple_addTriggerInfo_dataRelValFormer.root'%( os.getenv( "CMSSW_BASE" ) ) )
, outputCommands = cms.untracked.vstring(
    *patEventContentNoCleaning
  )
)
process.out.outputCommands.append( 'keep edmTriggerResults_TriggerResults_*_*' )
process.out.outputCommands.append( 'keep *_hltTriggerSummaryAOD_*_*' )
process.outpath = cms.EndPath(
  process.out
)

## Processing
process.load( "PhysicsTools.PatAlgos.patSequences_cff" )
process.p = cms.Path(
  process.patDefaultSequence
)

from PhysicsTools.PatAlgos.tools.myTools import runOnData
runOnData( process )
process.patJetCorrFactors.useRho = cms.bool( False )
process.out.outputCommands += [ 'drop recoGenJets_*_*_*' ]

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
process.moin = somePatMuonTriggerMatchPDSingleMu.clone()
process.tach = somePatMetTriggerMatchHLTMu17.clone()
switchOnTrigger( process )
switchOnTriggerMatching( process )
switchOnTriggerStandAlone( process )
switchOnTriggerMatchingStandAlone( process )
#switchOnTriggerMatchEmbedding( process )
switchOnTrigger( process, triggerProducer = 'hallo', triggerEventProducer = 'tschuess', hltProcess = '*' )
switchOnTriggerMatching( process, triggerMatchers = [ 'moin', 'tach' ], triggerProducer = 'hallo', triggerEventProducer = 'tschuess', hltProcess = '*' )
switchOnTriggerStandAlone( process, triggerProducer = 'hallo', hltProcess = '*' )
switchOnTriggerMatchingStandAlone( process, triggerMatchers = [ 'moin', 'tach' ], triggerProducer = 'hallo', hltProcess = '*' )
switchOnTriggerMatchEmbedding( process, triggerMatchers = [ 'moin', 'tach' ], triggerProducer = 'hallo', hltProcess = '*' )
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
