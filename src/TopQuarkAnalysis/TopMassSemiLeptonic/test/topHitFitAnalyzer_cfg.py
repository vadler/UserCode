import os
import FWCore.ParameterSet.Config as cms


### Steering

# Misc
runTest   = True
rfioInput = False

# Input
runOnMC = True
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
process.MessageLogger.cerr.threshold = 'INFO'
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

outputFile = '%s/output/topHitFitAnalyzer_%s.root'%( os.getenv( "CMSSW_BASE" ), sample )
if runTest:
  outputFile = outputFile.replace( 'root', 'test.root' )
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

process.load( "TopQuarkAnalysis.TopMassSemiLeptonic.topHitFitAnalyzer_cfi" )
process.topHitFitAnalyzer.verbosity = 1
if runTest:
  process.topHitFitAnalyzer.verbosity = 11
process.topHitFitAnalyzer_Reference = process.topHitFitAnalyzer.clone( ttSemiLeptonicEvent = 'ttSemiLepEventReference'
                                                                     , patMuons            = 'referencePatMuons'
                                                                     , patJets             = 'referencePatJets'
                                                                     )
process.topHitFitAnalyzer_ReferenceBTag = process.topHitFitAnalyzer.clone( ttSemiLeptonicEvent = 'ttSemiLepEventReferenceBTag'
                                                                         , patMuons            = 'referencePatMuons'
                                                                         , patJets             = 'referencePatJets'
                                                                         )


### Paths

process.p = cms.Path(
  process.hltHighLevel
* process.topHitFitAnalyzer
)

process.p_Reference = cms.Path(
  process.hltHighLevel_Reference
* process.topHitFitAnalyzer_Reference
)

process.p_ReferenceBTag = cms.Path(
  process.hltHighLevel_Reference
* process.topHitFitAnalyzer_ReferenceBTag
)


### Messages

print
print 'Log file:'
print logFile
print '================================================================================'
print