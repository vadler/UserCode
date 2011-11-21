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

# Event selection
hitFitProcess = 'HitFit'

### Initialization

process = cms.Process( "TEST" )


### Logging

reportEvery = 1000
if runTest:
  reportEvery = 1
process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.MessageLogger.cerr.FwkReport.reportEvery = reportEvery
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool( True )
)
if runTest:
  process.MessageLogger.cerr.threshold = 'INFO'
  process.MessageLogger.categories.append('TopHitFitResolutionFunctions')
  process.MessageLogger.cerr.TopHitFitResolutionFunctions = cms.untracked.PSet(
    limit = cms.untracked.int32( -1 )
  )
  process.MessageLogger.categories.append('TtSemiLeptonicEvent')
  process.MessageLogger.cerr.TtSemiLeptonicEvent = cms.untracked.PSet(
    limit = cms.untracked.int32( -1 )
  )


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

outputFile = '%s/output/topHitFitResolutionFunctions_%s.root'%( os.getenv( "CMSSW_BASE" ), sample )
if runTest:
  outputFile = outputFile.replace( 'root', 'test.root' )
if not rfioInput:
  outputFile = outputFile.replace( 'root', 'local.root' )
process.TFileService = cms.Service(
  "TFileService"
, fileName = cms.string( outputFile )
)
logFile = outputFile.replace( 'root', 'log' )


### Event selection

# Trigger
process.load( "HLTrigger.HLTfilters.hltHighLevel_cfi" )
process.hltHighLevel.TriggerResultsTag = cms.InputTag( 'TriggerResults::%s'%( hitFitProcess ) )
process.hltHighLevel.HLTPaths          = [ 'HitFit_PF2PATHitFit'
                                         ]
process.hltHighLevel_Reference = process.hltHighLevel.clone( HLTPaths = [ 'HitFit_ReferenceHitFit'
                                                                        ]
                                                           )


### Analyzer

process.load( "TopQuarkAnalysis.TopMassSemiLeptonic.topHitFitResolutionFunctions_cfi" )
process.topHitFitResolutionFunctions.verbosity = 1
if runTest:
  process.topHitFitResolutionFunctions.verbosity = 11
process.topHitFitResolutionFunctions_Reference = process.topHitFitResolutionFunctions.clone( ttSemiLeptonicEvent = 'ttSemiLepEventReference'
                                                                                           )


### Paths

process.p = cms.Path(
  process.hltHighLevel
* process.topHitFitResolutionFunctions
)

process.p_Reference = cms.Path(
  process.hltHighLevel_Reference
* process.topHitFitResolutionFunctions_Reference
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
