import os
import FWCore.ParameterSet.Config as cms


### Steering

# Misc
runTest   = True
rfioInput = False

# Input
sample  = 'RelValTTbar'
#sample  = 'RelValZMM'
#sample  = 'RelValWM'
#sample  = 'Mu'
#sample  = 'Jet'

### Initialization

process = cms.Process( "TEST" )


### Input

from TopQuarkAnalysis.TopMassSemiLeptonic.input_hitFitPatSkimPF2PAT_cff import *
inputFiles = [ 'file:%s/output/hitFitPatSkimPF2PAT_%s.root'%( os.getenv( "CMSSW_BASE" ), sample )
             ]
if runTest:
  inputFiles = [ 'file:%s/output/hitFitPatSkimPF2PAT_%s.test.root'%( os.getenv( "CMSSW_BASE" ), sample )
               ]
if rfioInput:
  inputFiles = filesCastor
  if runTest:
    inputFiles = filesCastorTest
process.source = cms.Source(
  "PoolSource"
, fileNames = cms.untracked.vstring( inputFiles )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( -1 )
)
if runTest:
  process.maxEvents.input = 10000


### Output

outputFile = '%s/output/topHitFitResolutionFunctions%s.root'%( os.getenv( "CMSSW_BASE" ), sample )
if runTest:
  outputFile = outputFile.replace( 'root', 'test.root' )
if not rfioInput:
  outputFile = outputFile.replace( 'root', 'local.root' )
process.TFileService = cms.Service(
  "TFileService"
, fileName = cms.string( outputFile )
)
logFile = outputFile.replace( 'root', 'log' )


### Analyzer

process.load( "TopQuarkAnalysis.TopMassSemiLeptonic.topHitFitResolutionFunctions_cfi" )


### Paths

process.p = cms.Path(
  process.topHitFitResolutionFunctions
)


### Messages

print
print 'Output file:'
print '------------'
print outputFile
print
print 'Log file destination:'
print '---------------------'
print logFile
print '================================================================================'
print
