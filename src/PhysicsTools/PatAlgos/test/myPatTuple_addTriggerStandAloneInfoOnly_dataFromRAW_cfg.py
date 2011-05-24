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

## Geometry and Detector Conditions (needed for a few patTuple production steps)
process.load( "Configuration.StandardSequences.Services_cff" )
process.load( "Configuration.StandardSequences.Geometry_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
from Configuration.PyReleaseValidation.autoCond import autoCond
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