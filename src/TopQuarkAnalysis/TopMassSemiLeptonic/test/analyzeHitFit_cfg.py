import os
import FWCore.ParameterSet.Config as cms


### Steering

# Misc
runTest   = True
rfioInput = True

# Origin of existing resolution functions
# era = 'Spring10'
era = 'Summer11'

# Event selection
leptonType = 'Muon' # 'Muon' or 'Electron'

# Correlation to input
pileUpFileMCTrue       = 'TopQuarkAnalysis/TopMassSemiLeptonic/data/pileUpFileMC_Fall11.root'
pileUpFileMCObserved   = 'TopQuarkAnalysis/TopMassSemiLeptonic/data/pileUpFileMC_Fall11inTime.root'
pileUpFileDataTrue     = 'TopQuarkAnalysis/TopMassSemiLeptonic/data/pileUpFileData_2011truePixel.root'
pileUpFileDataObserved = 'TopQuarkAnalysis/TopMassSemiLeptonic/data/pileUpFileData_2011observedPixel.root'
#pileUpFileDataTrue     = 'TopQuarkAnalysis/TopMassSemiLeptonic/data/pileUpFileData_2011true.root'
#pileUpFileDataObserve = 'TopQuarkAnalysis/TopMassSemiLeptonic/data/pileUpFileData_2011observed.root'
skimProcess    = 'SKIM'
jecLevels      = [ 'L1FastJet'
                 , 'L2Relative'
                 , 'L3Absolute'
                 ]

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
if not rfioInput:
  process.MessageLogger.cerr.threshold = 'INFO'
  process.MessageLogger.categories.append('AnalyzeHitFit')
  process.MessageLogger.cerr.AnalyzeHitFit = cms.untracked.PSet(
    limit = cms.untracked.int32( -1 )
  )


### Input

inputFiles = [ 'file:%s/output/skimHitFit.local.root'%( os.getenv( "CMSSW_BASE" ) )
             ]
if runTest:
  inputFiles = [ 'file:%s/output/skimHitFit.test.local.root'%( os.getenv( "CMSSW_BASE" ) )
               ]
if rfioInput:
  from TopQuarkAnalysis.TopMassSemiLeptonic.input_hitFitPatSkimPF2PAT_cff import *
  #inputFiles = filesNew_Fall11_R4
  inputFiles = filesNew_Fall11_R4_1
process.source = cms.Source(
  "PoolSource"
, fileNames = cms.untracked.vstring( inputFiles )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( -1 )
)
if rfioInput and runTest:
  process.maxEvents.input = 25000


### Output

outputFile = '%s/output/analyzeHitFit%ss_from%s.root'%( os.getenv( "CMSSW_BASE" ), leptonType, era )

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
process.hltHighLevel.HLTPaths          = [ 'pf2PatPathMuons'
                                         , 'pf2PatPathElectrons'
                                         ]
process.hltHighLevelReference = process.hltHighLevel.clone( HLTPaths = [ 'referencePathMuons'
                                                                       , 'referencePathElectrons'
                                                                       ]
                                                          )


### Analyzer

process.load( "TopQuarkAnalysis.TopMassSemiLeptonic.analyzeHitFit_cfi" )
process.analyzeHitFit.pileUpFileMCTrue       = pileUpFileMCTrue
process.analyzeHitFit.pileUpFileMCObserved   = pileUpFileMCObserved
process.analyzeHitFit.pileUpFileDataTrue     = pileUpFileDataTrue
process.analyzeHitFit.pileUpFileDataObserved = pileUpFileDataObserved
process.analyzeHitFit.ttSemiLeptonicEventMuons     = 'ttSemiLepEventHitFitMuons'
process.analyzeHitFit.ttSemiLeptonicEventElectrons = 'ttSemiLepEventHitFitElectrons'
process.analyzeHitFit.patMuons            = 'selectedPatMuonsHitFit'
process.analyzeHitFit.patElectrons        = 'selectedPatElectronsHitFit'
process.analyzeHitFit.patJets             = 'selectedPatJetsHitFit'
process.analyzeHitFit.patMETs             = 'patMETs'
process.analyzeHitFit.jecLevel            = 'L3Absolute'
process.analyzeHitFit.muonResolutions     = 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafMuonResolution_%s.txt'%( era )
process.analyzeHitFit.electronResolutions = 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafElectronResolution_%s.txt'%( era )
process.analyzeHitFit.udscJetResolutions  = 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafUdscJetResolution_%s.txt'%( era )
process.analyzeHitFit.bJetResolutions     = 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafBJetResolution_%s.txt'%( era )
process.analyzeHitFit.metResolutions      = 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafKtResolution_%s.txt'%( era )
# process.analyzeHitFit_L5Flavor = process.analyzeHitFit.clone( jecLevel = 'L5Flavor'
#                                                                                            )
# process.analyzeHitFit_L7Parton = process.analyzeHitFit.clone( jecLevel = 'L7Parton'
#                                                                                          )
process.analyzeHitFit_Reference = process.analyzeHitFit.clone( ttSemiLeptonicEventMuons     = 'ttSemiLepEventReferenceMuons'
                                                             , ttSemiLeptonicEventElectrons = 'ttSemiLepEventReferenceElectrons'
                                                             , patMuons     = 'referencePatMuons'
                                                             , patElectrons = 'referencePatElectrons'
                                                             , patJets      = 'referencePatJets'
                                                             )


### Paths

process.standardPath = cms.Path(
  process.hltHighLevel
* process.analyzeHitFit
)

if not runTest:
  process.referencePath = cms.Path(
    process.hltHighLevelReference
  * process.analyzeHitFitReference
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
