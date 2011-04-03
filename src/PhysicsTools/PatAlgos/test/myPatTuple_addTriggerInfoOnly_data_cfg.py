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
  # Prompt RECO
    '/store/data/Run2011A/SingleMu/AOD/PromptReco-v1/000/161/016/1C159A2D-D455-E011-9502-003048F01E88.root'
  , '/store/data/Run2011A/SingleMu/AOD/PromptReco-v1/000/161/016/1C1D1BBA-D255-E011-81F6-003048F1182E.root'
  , '/store/data/Run2011A/SingleMu/AOD/PromptReco-v1/000/161/016/2C9C9787-C755-E011-AC9A-0019DB2F3F9A.root'
  , '/store/data/Run2011A/SingleMu/AOD/PromptReco-v1/000/161/016/52713845-C855-E011-A1AB-000423D98B6C.root'
  , '/store/data/Run2011A/SingleMu/AOD/PromptReco-v1/000/161/016/702DDB20-E355-E011-BA84-0030487C2B86.root'
  , '/store/data/Run2011A/SingleMu/AOD/PromptReco-v1/000/161/016/78186858-D155-E011-B08A-003048F118C4.root'
  , '/store/data/Run2011A/SingleMu/AOD/PromptReco-v1/000/161/016/86102236-D455-E011-85C5-0030487CD812.root'
  , '/store/data/Run2011A/SingleMu/AOD/PromptReco-v1/000/161/016/9E612DE9-D455-E011-8FA6-003048F11C28.root'
  , '/store/data/Run2011A/SingleMu/AOD/PromptReco-v1/000/161/016/F8DFDD84-4B56-E011-84BD-003048D2BE08.root'
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

process.p = cms.Path(
)

## Output
process.out = cms.OutputModule(
  "PoolOutputModule"
, fileName       = cms.untracked.string( '%s/output/myPatTuple_addTriggerInfoOnly_data.root'%( cmsswBase ) )
, SelectEvents   = cms.untracked.PSet(
    SelectEvents = cms.vstring(
      'p'
    )
  )
, outputCommands = cms.untracked.vstring(
    'drop *'
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
process.patTriggerEvent.condGtTag = cms.InputTag( 'conditionsInEdm' )
process.patTriggerEvent.l1GtTag   = cms.InputTag( 'gtDigis' )
switchOnTrigger( process, sequence = 'p', hltProcess = '*' ) # to update event content
switchOnTriggerStandAlone( process, sequence = 'p', hltProcess = '*' )
