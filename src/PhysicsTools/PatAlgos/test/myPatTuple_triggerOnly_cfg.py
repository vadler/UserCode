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
#     # RAW (needs process.gtDigis in the path)
#     '/store/data/Run2010A/MinimumBias/RAW/v1/000/136/441/3CC638F4-1F6B-DF11-8067-00304879BAB2.root'
  )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 1 )
)
process.load( "L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskAlgoTrigConfig_cff" )
process.load( "L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff" )

## Geometry and Detector Conditions (needed for a few patTuple production steps)
process.load( "Configuration.StandardSequences.Services_cff" )
process.load( "Configuration.StandardSequences.Geometry_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
# process.GlobalTag.globaltag = 'GR_R_37X_V3::All'
process.GlobalTag.globaltag = 'GR10_P_V5::All'
process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )
# process.load( "Configuration.StandardSequences.RawToDigi_Data_cff" )

## PAT trigger
process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff" )
process.patTriggerEvent.patTriggerMatches = []
process.p = cms.Path(
#   process.gtDigis
# * process.patTrigger
  process.patTrigger
* process.patTriggerEvent
)

## Output
from PhysicsTools.PatAlgos.patEventContent_cff import patTriggerEventContent
process.out = cms.OutputModule(
  "PoolOutputModule"
, fileName       = cms.untracked.string( '/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_8_0_pre3/output/myPatTuple_triggerOnly_data.root' )
, SelectEvents   = cms.untracked.PSet(
    SelectEvents = cms.vstring(
      'p'
    )
  )
, outputCommands = cms.untracked.vstring(
    'drop *'
  , *patTriggerEventContent
  )
)
process.outpath = cms.EndPath(
  process.out
)
