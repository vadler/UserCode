import os
import FWCore.ParameterSet.Config as cms

# Steering
condition = 'com10_5E33v4'

process = cms.Process( "PAT" )

## Options
process.options = cms.untracked.PSet(
  wantSummary      = cms.untracked.bool( False )
, allowUnscheduled = cms.untracked.bool( False )
)

## Messaging
process.load( "FWCore.MessageLogger.MessageLogger_cfi" )
#process.Tracer = cms.Service( "Tracer" )

## Conditions
process.load( "Configuration.StandardSequences.Services_cff" )
process.load( "Configuration.Geometry.GeometryIdeal_cff" )
process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag( process.GlobalTag, 'auto:%s'%( condition ) )

## Input
from PhysicsTools.PatAlgos.myPatTuple_dataFromRAW_cff import fileNames2011
process.source = cms.Source(
  "PoolSource"
, fileNames2011
, skipBadFiles = cms.untracked.bool( True )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 100 )
)

## Output
from L1Trigger.Configuration.L1Trigger_EventContent_cff import L1TriggerAOD
process.out = cms.OutputModule(
  "PoolOutputModule"
, SelectEvents = cms.untracked.PSet(
    SelectEvents = cms.vstring(
      'p'
    )
  )
, fileName       = cms.untracked.string( '%s/output/myPatTuple_addTriggerInfoOnly_dataRAW2011.root'%( os.getenv( "CMSSW_BASE" ) ) )
, outputCommands = cms.untracked.vstring(
    *L1TriggerAOD.outputCommands
  )
)
process.out.outputCommands.append( 'keep edmTriggerResults_TriggerResults_*_*' )
process.out.outputCommands.append( 'keep *_hltTriggerSummaryAOD_*_*' )
process.outpath = cms.EndPath(
  process.out
)

## RAW to DIGI and RECO pre-requisites
process.load( "Configuration.StandardSequences.RawToDigi_Data_cff" )
process.load( "L1Trigger.Configuration.L1Reco_cff" )
process.p = cms.Path(
  process.gctDigis
* process.gtDigis
* process.gtEvmDigis
* process.scalersRawToDigi
* process.L1Reco
)

## PAT trigger
from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTrigger( process, sequence = 'p', hltProcess = '*' )
process.patTrigger.addL1Algos     = cms.bool( True )
process.patTrigger.l1ExtraMu      = cms.InputTag( 'l1extraParticles', ''            )
process.patTrigger.l1ExtraNoIsoEG = cms.InputTag( 'l1extraParticles', 'NonIsolated' )
process.patTrigger.l1ExtraIsoEG   = cms.InputTag( 'l1extraParticles', 'Isolated'    )
process.patTrigger.l1ExtraCenJet  = cms.InputTag( 'l1extraParticles', 'Central'     )
process.patTrigger.l1ExtraForJet  = cms.InputTag( 'l1extraParticles', 'Forward'     )
process.patTrigger.l1ExtraTauJet  = cms.InputTag( 'l1extraParticles', 'Tau'         )
process.patTrigger.l1ExtraETM     = cms.InputTag( 'l1extraParticles', 'MET'         )
process.patTrigger.l1ExtraHTM     = cms.InputTag( 'l1extraParticles', 'MHT'         )
process.patTrigger.mainBxOnly     = cms.bool( False )
process.patTrigger.saveL1Refs     = cms.bool( True )
process.patTrigger.exludeCollections = cms.vstring(
  "hltL1extraParticles*"
)
# process.patTriggerEvent.condGtTag = cms.InputTag( 'conditionsInEdm' ) # This does not work due to the lumi and run products
process.patTriggerEvent.l1GtTag   = cms.InputTag( 'gtDigis' )
switchOnTrigger( process, sequence = 'p', hltProcess = '*' ) # to update event content
switchOnTriggerStandAlone( process, sequence = 'p', hltProcess = '*' )
