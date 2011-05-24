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
    '/store/data/Run2011A/MuHad/RAW/v1/000/165/121/30B69E98-C67F-E011-8697-003048D37560.root'
  , '/store/data/Run2011A/MuHad/RAW/v1/000/165/121/80E66677-C27F-E011-8A15-001D09F29321.root'
  , '/store/data/Run2011A/MuHad/RAW/v1/000/165/121/8E69B419-B57F-E011-821E-001617C3B65A.root'
  , '/store/data/Run2011A/MuHad/RAW/v1/000/165/121/A219173E-B07F-E011-ACC8-0030487CD6DA.root'
  , '/store/data/Run2011A/MuHad/RAW/v1/000/165/121/B028587F-D47F-E011-94FD-0030487CD906.root'
  , '/store/data/Run2011A/MuHad/RAW/v1/000/165/121/CCCA933C-BE7F-E011-A992-0030486730C6.root'
  , '/store/data/Run2011A/MuHad/RAW/v1/000/165/121/DCC9E8A9-BA7F-E011-A73A-003048D37580.root'
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
from Configuration.AlCa.autoCond import autoCond
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
