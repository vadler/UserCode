import os
cmsswBase = os.getenv( "CMSSW_BASE" )

import FWCore.ParameterSet.Config as cms

process = cms.Process( "PAT" )

## Options and Output Report
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool( False )
)

## Source
process.source = cms.Source( "PoolSource"
, fileNames = cms.untracked.vstring(
    '/store/data/Run2011A/SingleMu/RAW/v1/000/161/016/007F6B4D-FB53-E011-90E7-00304879BAB2.root'
  , '/store/data/Run2011A/SingleMu/RAW/v1/000/161/016/0407FF39-FE53-E011-97DD-0030487C90EE.root'
  , '/store/data/Run2011A/SingleMu/RAW/v1/000/161/016/1ADCFE2E-0054-E011-895C-001617E30CC8.root'
  , '/store/data/Run2011A/SingleMu/RAW/v1/000/161/016/20AAEB44-F753-E011-957D-001D09F251D1.root'
  , '/store/data/Run2011A/SingleMu/RAW/v1/000/161/016/62BD8FE9-F453-E011-92D2-001D09F24498.root'
  , '/store/data/Run2011A/SingleMu/RAW/v1/000/161/016/9E91218A-F853-E011-AA94-001D09F29114.root'
  , '/store/data/Run2011A/SingleMu/RAW/v1/000/161/016/AA5CE18E-F353-E011-BC2D-0016177CA778.root'
  , '/store/data/Run2011A/SingleMu/RAW/v1/000/161/016/C6C1C9F8-F253-E011-950F-0030487CD7E0.root'
  , '/store/data/Run2011A/SingleMu/RAW/v1/000/161/016/FE09BCDE-F953-E011-B059-001617DBD472.root'
  )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 100 )
)
# FIXME: Is this still needed?
process.load( "L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskAlgoTrigConfig_cff" )
process.load( "L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff" )

## Geometry and Detector Conditions (needed for a few patTuple production steps)
process.load( "Configuration.StandardSequences.Services_cff" )
process.load( "Configuration.StandardSequences.Geometry_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
from Configuration.PyReleaseValidation.autoCond import autoCond
process.GlobalTag.globaltag = cms.string( autoCond[ 'com10' ] )
process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )

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

## Output
from L1Trigger.Configuration.L1Trigger_EventContent_cff import L1TriggerAOD
process.out = cms.OutputModule(
  "PoolOutputModule"
, fileName       = cms.untracked.string( '%s/output/myPatTuple_addTriggerInfoOnly_dataFromRAW.root'%( cmsswBase ) )
, SelectEvents   = cms.untracked.PSet(
    SelectEvents = cms.vstring(
      'p'
    )
  )
, outputCommands = cms.untracked.vstring(
    'drop *'
  , *L1TriggerAOD.outputCommands
  )
)
process.outpath = cms.EndPath(
  process.out
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
