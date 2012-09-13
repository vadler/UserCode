import os
import FWCore.ParameterSet.Config as cms

# Steering
condition = 'com10'

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
from PhysicsTools.PatAlgos.myPatTuple_dataFromRAW_cff import fileNames
process.source = cms.Source(
  "PoolSource"
, fileNames
, skipBadFiles = cms.untracked.bool( True )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 1000 )
)

## Output
process.out = cms.OutputModule(
  "PoolOutputModule"
, SelectEvents = cms.untracked.PSet(
    SelectEvents = cms.vstring(
      'p'
    )
  )
, fileName       = cms.untracked.string( '%s/output/myPatTuple_addTriggerStandAloneInfoOnly_dataRAW.root'%( os.getenv( "CMSSW_BASE" ) ) )
, outputCommands = cms.untracked.vstring()
)
process.out.outputCommands.append( 'keep edmTriggerResults_TriggerResults_*_*' )
process.out.outputCommands.append( 'keep *_hltTriggerSummaryAOD_*_*' )
process.outpath = cms.EndPath(
  process.out
)

## RAW to DIGI and RECO pre-requisites
process.load( "Configuration.StandardSequences.RawToDigi_Data_cff" )
process.p = cms.Path(
  process.gtDigis
)

## PAT trigger
from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTriggerStandAlone( process, sequence = 'p', hltProcess = '*' )
