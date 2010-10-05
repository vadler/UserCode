import FWCore.ParameterSet.Config as cms

process = cms.Process( "PAT" )

## MessageLogger
process.load( "FWCore.MessageLogger.MessageLogger_cfi" )

## Options and Output Report
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool( False )
)

## Source
process.source = cms.Source( "PoolSource"
, fileNames = cms.untracked.vstring(
    # Prompt RECO
    '/store/data/Run2010A/MinimumBias/RECO/v2/000/136/441/48CD034A-2D6B-DF11-9CFE-0030487CD718.root'
  )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 100 )
)
process.load( "L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskAlgoTrigConfig_cff" )
process.load( "L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff" )

## Geometry and Detector Conditions (needed for a few patTuple production steps)
process.load( "Configuration.StandardSequences.Services_cff" )
process.load( "Configuration.StandardSequences.Geometry_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.globaltag = 'GR_R_38X_V13::All'
process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )
process.load( "Configuration.StandardSequences.RawToDigi_Data_cff" )
process.load( "Configuration.StandardSequences.L1Reco_cff" )

process.p = cms.Path(
)

## Output
process.out = cms.OutputModule(
  "PoolOutputModule"
, fileName       = cms.untracked.string( '/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_9_0_pre6/output/myPatTuple_triggerOnly.root' )
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
process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff" )
process.patTrigger.addL1Algos     = cms.bool( True )
process.patTrigger.l1ExtraMu      = cms.InputTag( 'l1extraParticles', ''           , 'RECO' )
process.patTrigger.l1ExtraNoIsoEG = cms.InputTag( 'l1extraParticles', 'NonIsolated', 'RECO' )
process.patTrigger.l1ExtraIsoEG   = cms.InputTag( 'l1extraParticles', 'Isolated'   , 'RECO' )
process.patTrigger.l1ExtraCenJet  = cms.InputTag( 'l1extraParticles', 'Central'    , 'RECO' )
process.patTrigger.l1ExtraForJet  = cms.InputTag( 'l1extraParticles', 'Forward'    , 'RECO' )
process.patTrigger.l1ExtraTauJet  = cms.InputTag( 'l1extraParticles', 'Tau'        , 'RECO' )
process.patTrigger.l1ExtraETM     = cms.InputTag( 'l1extraParticles', 'MET'        , 'RECO' )
process.patTrigger.l1ExtraHTM     = cms.InputTag( 'l1extraParticles', 'MHT'        , 'RECO' )
process.patTrigger.saveL1Refs     = cms.bool( True )
process.patTriggerEvent.condGtTag = cms.InputTag( 'conditionsInEdm' )
process.patTriggerEvent.l1GtTag   = cms.InputTag( 'gtDigis' )
from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTrigger( process, None, None, 'p' )
