import os
cmsswBase = os.getenv( "CMSSW_BASE" )

import FWCore.ParameterSet.Config as cms

process = cms.Process( "PAT" )

## Options and Output Report
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool( False )
)

## Source
from PhysicsTools.PatAlgos.myPatTuple_dataFromRAW_cff import fileNames
process.source = cms.Source( "PoolSource"
, fileNames
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 1000 )
)

## Geometry and Detector Conditions (needed for a few patTuple production steps)
process.load( "Configuration.StandardSequences.Services_cff" )
process.load( "Configuration.Geometry.GeometryIdeal_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
from Configuration.AlCa.autoCond import autoCond
process.GlobalTag.globaltag = cms.string( autoCond[ 'com10' ] )
process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )

## RAW to DIGI and RECO pre-requisites
process.load( "Configuration.StandardSequences.RawToDigi_Data_cff" )

process.p = cms.Path(
  process.gtDigis
)

## Output
process.out = cms.OutputModule(
  "PoolOutputModule"
, fileName       = cms.untracked.string( '%s/output/myPatTuple_addTriggerStandAloneInfoOnly_dataFromRAW.root'%( cmsswBase ) )
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
switchOnTriggerStandAlone( process, sequence = 'p', hltProcess = '*' )
