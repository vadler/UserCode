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
process.load( "Configuration.Geometry.GeometryRecoDB_cff" )
process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
from Configuration.AlCa.autoCond import autoCond
condition                   = 'com10_5E33v4'
process.GlobalTag.globaltag = cms.string( autoCond[ condition ][ 0 ] )
l1Menu                      = autoCond[ condition ][ 1 ].split( ',' )
process.GlobalTag.toGet.append( cms.PSet( tag     = cms.string( l1Menu[ 0 ] )
                                        , record  = cms.string( l1Menu[ 1 ] )
                                        , connect = cms.untracked.string( l1Menu[ 2 ] )
                                        )
                              )

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
