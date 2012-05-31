#import os
#lxplusTest = ( os.uname()[ 1 ].split( '.' )[ 0 ][ : 6 ] == 'lxplus' )
import socket
lxplusTest = ( socket.getfqdn().split( '.' )[ 0 ][ : 6 ] == 'lxplus' )

### Steering

maxEvents = -1
if lxplusTest:
  maxEvents = 100

### Initialization

import FWCore.ParameterSet.Config as cms

process = cms.Process( 'USER' )

### Logging

reportEvery = 1000
wantSummary = False
if lxplusTest:
  reportEvery = 1
  wantSummary = True

process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.MessageLogger.cerr.FwkReport.reportEvery = reportEvery
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool( wantSummary )
)

### Input

inputFiles = []
if lxplusTest:
  inputFiles = [ '/store/relval/CMSSW_4_4_2_patch10/RelValProdTTbar/AODSIM/START44_V7_special_120119-v1/0084/1AFC5FFF-3143-E111-9E01-001BFCDBD1BC.root'
               , '/store/relval/CMSSW_4_4_2_patch10/RelValProdTTbar/AODSIM/START44_V7_special_120119-v1/0088/0ECC7FA5-7A43-E111-9B36-002618943861.root'
               ]


process.source = cms.Source(
  "PoolSource"
, fileNames = cms.untracked.vstring( inputFiles )
)

process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( maxEvents )
)

### Output

process.out = cms.OutputModule(
  "PoolOutputModule"
, fileName = cms.untracked.string( 'copy.root' )
, outputCommands = cms.untracked.vstring( 'keep *'
                                        )
)

process.outpath = cms.EndPath(
  process.out
)
