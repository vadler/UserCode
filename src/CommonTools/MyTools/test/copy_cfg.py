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
  inputFiles = []


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
, fileName = cms.untracked.string( 'TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola__Summer12_DR53X-PU_S10_START53_V7C-v1__AODSIM.root' )
#, fileName = cms.untracked.string( 'TT_8TeV-mcatnlo__Summer12_DR53X-PU_S10_START53_V7A-v1__AODSIM.root' )
, outputCommands = cms.untracked.vstring( 'keep *'
                                        )
)

process.outpath = cms.EndPath(
  process.out
)
