import os
import FWCore.ParameterSet.Config as cms


### Steering

# Misc
runTest   = True
rfioInput = False

# Origin of existing resolution functions
# era = 'Spring10'
era = 'Summer11'

# Event selection
skimProcess = 'SKIM'

# Settings
usePtRel = False # ( Delta p_t ) / p_t instead of Delta ( 1 / p_t ) in inverse p_t
useJetEt = False

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
  process.MessageLogger.categories.append('AnalyzeHitFitResolutionFunctions')
  process.MessageLogger.cerr.AnalyzeHitFitResolutionFunctions = cms.untracked.PSet(
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
  if runTest:
    #inputFiles = filesTest_Summer11
    inputFiles = filesTest_Fall11_R3
process.source = cms.Source(
  "PoolSource"
, fileNames = cms.untracked.vstring( inputFiles )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( -1 )
)


### Output

outputFile = '%s/output/analyzeHitFitResolutionFunctions_from%s.root'%( os.getenv( "CMSSW_BASE" ), era )
if usePtRel:
  outputFile = outputFile.replace( '.root', '_ptRel.root' )
if useJetEt:
  outputFile = outputFile.replace( '.root', '_jetEt.root' )
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

process.load( "TopQuarkAnalysis.TopMassSemiLeptonic.analyzeHitFitResolutionFunctions_cfi" )
process.analyzeHitFitResolutionFunctions.usePtRel            = usePtRel
process.analyzeHitFitResolutionFunctions.useJetEt            = useJetEt
process.analyzeHitFitResolutionFunctions.jecLevel            = 'L3Absolute'
process.analyzeHitFitResolutionFunctions.muonResolutions     = 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafMuonResolution_%s.txt'%( era )
process.analyzeHitFitResolutionFunctions.electronResolutions = 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafElectronResolution_%s.txt'%( era )
process.analyzeHitFitResolutionFunctions.udscJetResolutions  = 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafUdscJetResolution_%s.txt'%( era )
process.analyzeHitFitResolutionFunctions.bJetResolutions     = 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafBJetResolution_%s.txt'%( era )
process.analyzeHitFitResolutionFunctions.metResolutions      = 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafKtResolution_%s.txt'%( era )
# process.analyzeHitFitResolutionFunctions_L5Flavor = process.analyzeHitFitResolutionFunctions.clone( jecLevel = 'L5Flavor'
#                                                                                            )
# process.analyzeHitFitResolutionFunctions_L7Parton = process.analyzeHitFitResolutionFunctions.clone( jecLevel = 'L7Parton'
#                                                                                            )
process.analyzeHitFitResolutionFunctions_Reference = process.analyzeHitFitResolutionFunctions.clone( ttSemiLeptonicEvent = 'ttSemiLepEventReference'
                                                                                           )


### Paths

process.standardPath = cms.Path(
  process.hltHighLevel
* process.analyzeHitFitResolutionFunctions
)

# process.jecL5Path = cms.Path(
#   process.hltHighLevel
# * process.analyzeHitFitResolutionFunctions_L5Flavor
# )

# process.jecL7Path = cms.Path(
#   process.hltHighLevel
# * process.analyzeHitFitResolutionFunctions_L7Parton
# )

if not runTest:
  process.referencePath = cms.Path(
    process.hltHighLevel_Reference
  * process.analyzeHitFitResolutionFunctions_Reference
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
