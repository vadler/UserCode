import os
import FWCore.ParameterSet.Config as cms

# Steering
condition = 'com10_7E33v4'

process = cms.Process("PAT")

## Options
process.options = cms.untracked.PSet(
  wantSummary      = cms.untracked.bool( True )
, allowUnscheduled = cms.untracked.bool( True )
)

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
, fileNames = cms.untracked.vstring( pickRelValInputFiles( relVal      = 'SingleMu'
                                                         , dataTier    = 'RECO'
                                                         , globalTag   = '%s_RelVal_mu2012A'%( process.GlobalTag.globaltag.value()[ : -5 ] )
                                                         )
                                   )
, skipBadFiles = cms.untracked.bool( True )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 100 )
)

## Messaging
process.load( "FWCore.MessageLogger.MessageLogger_cfi" )
if process.maxEvents.input.value() <= 1:
    process.Tracer = cms.Service( "Tracer" )

## Output
process.out = cms.OutputModule(
  "PoolOutputModule"
, fileName       = cms.untracked.string( '%s/output/myPatTuple_addTriggerInfo_dataRelVal.root'%( os.getenv( "CMSSW_BASE" ) ) )
, outputCommands = cms.untracked.vstring(
    'drop *'
  , 'keep edmTriggerResults_TriggerResults_*_*'
  , 'keep *_hltTriggerSummaryAOD_*_*'
  )
)
process.outpath = cms.EndPath(
  process.out
)

## Processing
process.load("PhysicsTools.PatAlgos.producersLayer1.patCandidates_cff")
process.load("PhysicsTools.PatAlgos.selectionLayer1.selectedPatCandidates_cff")

from PhysicsTools.PatAlgos.tools.myTools import runOnData
runOnData( process )
process.patJetCorrFactors.useRho = cms.bool( False )
process.out.outputCommands += [ 'drop recoGenJets_*_*_*' ]

# Trigger
from PhysicsTools.PatAlgos.tools.trigTools import *
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
