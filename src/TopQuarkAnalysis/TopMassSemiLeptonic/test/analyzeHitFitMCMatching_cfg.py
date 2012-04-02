import os
import FWCore.ParameterSet.Config as cms


### Steering

# Misc
runTest   = True
rfioInput = False

# Event selection
skimProcess = 'SKIM'

### Initialization

process = cms.Process( "USER" )


### Logging

reportEvery = 1000
if runTest:
  if not rfioInput:
    reportEvery = 1
else:
  if rfioInput:
    reportEvery = 100000
process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.MessageLogger.cerr.FwkReport.reportEvery = reportEvery
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool( True )
)
if runTest:
  process.MessageLogger.cerr.threshold = 'INFO'
  process.MessageLogger.categories.append('AnalyzeHitFitMCMatching')
  process.MessageLogger.cerr.AnalyzeHitFitMCMatching = cms.untracked.PSet(
    limit = cms.untracked.int32( -1 )
  )


### Input

inputFiles = [ 'file:%s/output/skimHitFitResolutionFunctions.local.root'%( os.getenv( "CMSSW_BASE" ) )
             ]
if runTest:
  inputFiles = [ 'file:%s/output/skimHitFitResolutionFunctions.test.local.root'%( os.getenv( "CMSSW_BASE" ) )
               ]
if rfioInput:
  from TopQuarkAnalysis.TopMassSemiLeptonic.input_hitFitPatSkimPF2PAT_cff import *
  #inputFiles = files_Summer11
  inputFiles = files_Fall11_R3
process.source = cms.Source(
  "PoolSource"
, fileNames = cms.untracked.vstring( inputFiles )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( -1 )
)
if rfioInput and runTest:
  process.maxEvents.input = 10000


### Output

outputFile = '%s/output/analyzeHitFitMCMatching.root'%( os.getenv( "CMSSW_BASE" ) )

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
process.hltHighLevel.TriggerResultsTag = cms.InputTag( 'TriggerResults::%s'%( skimProcess ) )
process.hltHighLevel.HLTPaths          = [ 'pf2PatPath'
                                         ]
process.hltHighLevel_Reference = process.hltHighLevel.clone( HLTPaths = [ 'referencePath'
                                                                        ]
                                                           )


### Analyzer

process.load( "TopQuarkAnalysis.TopMassSemiLeptonic.analyzeHitFitMCMatching_cfi" )
process.analyzeHitFitMCMatching.patLeptons = cms.InputTag( 'selectedPat' + process.analyzeHitFitMCMatching.leptonType.value() ) # obligatory assignment
process.analyzeHitFitMCMatching_Reference = process.analyzeHitFitMCMatching.clone( ttSemiLeptonicEvent = 'ttSemiLepEventReference'
                                                                                 , patLeptons          = cms.InputTag( 'referencePat' + process.analyzeHitFitMCMatching.leptonType.value() )
                                                                                 , patJets             = 'referencePatJets'
                                                                                 )


### Paths

process.standardPath = cms.Path(
  process.hltHighLevel
* process.analyzeHitFitMCMatching
)

process.referencePath = cms.Path(
  process.hltHighLevel_Reference
* process.analyzeHitFitMCMatching_Reference
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
