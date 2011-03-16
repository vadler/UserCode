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
    '/store/data/Run2011A/MuHad/RECO/PromptReco-v1/000/160/433/C6F4247F-B54F-E011-88F1-003048F1C420.root'
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
process.GlobalTag.globaltag = cms.string( autoCond[ 'hltonline' ] )
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
