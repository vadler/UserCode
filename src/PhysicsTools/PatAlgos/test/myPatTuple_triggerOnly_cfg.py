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
# , fileNames = cms.untracked.vstring(
#     # Run 136441
#     '/store/data/Run2010A/MinimumBias/RECO/v2/000/136/441/48CD034A-2D6B-DF11-9CFE-0030487CD718.root'
#     # Run 144112, switch
#   , '/store/data/Run2010A/Mu/RECO/v4/000/144/112/E6F6D3D9-CBB3-DF11-8A0E-001D09F2AD4D.root' # prompt reco
#   #, 'rfio:/castor/cern.ch/user/v/vadler/cms/PAT/data/Mu__Run2010A-Sep17ReReco_v2__RECO/5E60A76B-11C8-DF11-829A-0017A4770C18.root' # re-reco
#   )
# , skipEvents = cms.untracked.uint32( 26400 ) # end of run 136441 file
, fileNames = cms.untracked.vstring(
    # Re-RECO
    'rfio:/castor/cern.ch/user/v/vadler/cms/PAT/data/Mu__Run2010A-Sep17ReReco_v2__RECO/5E60A76B-11C8-DF11-829A-0017A4770C18.root' # same lumi sections/events
    # Prompt RECO, switch
  #, '/store/data/Run2010A/Mu/RECO/v4/000/144/112/E6F6D3D9-CBB3-DF11-8A0E-001D09F2AD4D.root' # same lumi sections/events
  , '/store/data/Run2010A/Mu/RECO/v4/000/144/112/EE66B7B6-0BB4-DF11-BC6C-001D09F2B30B.root' # other lumi sectionsevents
  )
, skipEvents = cms.untracked.uint32( 18000 ) # end of re-reco file
# , fileNames = cms.untracked.vstring(
#     # Re-RECO, 1 event locally
#     'file:/afs/cern.ch/user/v/vadler/scratch0/data/PAT/Mu__Run2010A-Sep17ReReco_v2__RECO/r144112l406e423329356.root'
#   )
# , fileNames = cms.untracked.vstring(
#     # AOD, latest Re-RECO
#     'rfio:/castor/cern.ch/user/v/vadler/cms/PAT/data/Multijet__Run2010B-Dec22ReReco_v1__AOD/66A62074-7A1B-E011-8DD5-002618FDA265.root'
#   )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 100 )
#   input = cms.untracked.int32( 1 )
)
process.load( "L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskAlgoTrigConfig_cff" )
process.load( "L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff" )

## Geometry and Detector Conditions (needed for a few patTuple production steps)
process.load( "Configuration.StandardSequences.Services_cff" )
process.load( "Configuration.StandardSequences.Geometry_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.globaltag = 'GR_R_38X_V15::All'
process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )
process.load( "L1Trigger.GlobalTriggerAnalyzer.l1GtAnalyzer_cfi" )
process.l1GtAnalyzer.AlgorithmName = "L1_SingleMu7"
process.l1GtAnalyzer.ConditionName = "SingleMu_0x0B"

process.p = cms.Path(
  # process.l1GtAnalyzer
)

## Output
process.out = cms.OutputModule(
  "PoolOutputModule"
# , fileName       = cms.untracked.string( '%s/output/myPatTuple_triggerOnly.root'%( cmsswBase ) )
, fileName       = cms.untracked.string( '%s/output/myPatTuple_triggerOnly.root'%( cmsswBase ) )
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
