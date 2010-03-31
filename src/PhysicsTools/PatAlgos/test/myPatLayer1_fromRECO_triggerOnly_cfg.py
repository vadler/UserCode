import FWCore.ParameterSet.Config as cms

process = cms.Process( "PAT" )

## MessageLogger
process.load( "FWCore.MessageLogger.MessageLogger_cfi" )

## Options and Output Report
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool( False )
  )

## Source
process.source = cms.Source(
    "PoolSource"
  , fileNames = cms.untracked.vstring(
        'file:/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_5_6/output/myHlt1E31_fromRAW_triggerOnly.root'
      )
  )
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( -1 )
  )

## Geometry and Detector Conditions (needed for a few patTuple production steps)
process.load( "Configuration.StandardSequences.Services_cff" )
process.load( "Configuration.StandardSequences.Geometry_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.globaltag = 'MC_3XY_V25::All'
process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )

## PAT trigger
process.load("PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff")
process.patTrigger.processName      = 'HLT1E31'
# process.patTrigger.hltPrescaleLabel = cms.string( '1' )
# process.patTrigger.hltPrescaleTable = cms.string( "hltPrescaleRecorder" )
process.p = cms.Path(
    process.patTrigger
  )

## Output
from PhysicsTools.PatAlgos.patEventContent_cff import patTriggerEventContent
process.out = cms.OutputModule(
    "PoolOutputModule"
  , fileName       = cms.untracked.string( '/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_5_6/output/myPatLayer1_fromRECO_triggerOnly.root' )
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
