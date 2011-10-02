import os
import FWCore.ParameterSet.Config as cms

### Steering

# Misc
runTest = True

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


### Input

inputFiles = [ 'rfio:%s/cms/Top/data/hitFitPatSkimPF2PAT_%s.root'%( os.getenv( "CASTOR_HOME" ), sample )
             ]
if runTest:
  inputFiles = [ 'file:hitFitPatSkimPF2PAT.root'
               ]
process.source = cms.Source(
  "PoolSource"
, fileNames = cms.untracked.vstring( inputFiles )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( -1 )
)


### Output

outputFile = '%s/output/topHitFitAnalyzer_%s.root'%( os.getenv( "CMSSW_BASE" ), sample )
if runTest:
  outputFile = 'topHitFitAnalyzer.root'
process.TFileService = cms.Service(
  "TFileService"
, fileName = cms.string( outputFile )
)


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
process.topHitFitAnalyzer_Reference = process.topHitFitAnalyzer.clone()


### Paths

process.p = cms.Path(
  process.hltHighLevel
* process.topHitFitAnalyzer
)

process.p_Reference = cms.Path(
  process.hltHighLevel_Reference
* process.topHitFitAnalyzer_Reference
)


### Messages

print
print 'Log file:'
if runTest:
  print '%s/output/topHitFitAnalyzer_%s.log'%( os.getenv( "CMSSW_BASE" ), sample )
else:
  print 'topHitFitAnalyzer.log'
print '================================================================================'
print