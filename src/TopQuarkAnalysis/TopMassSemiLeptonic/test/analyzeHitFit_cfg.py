import os
import FWCore.ParameterSet.Config as cms


### Steering

# Misc
runTest   = True
rfioInput = False

# Origin of existing resolution functions
# era = 'Spring10'
era = 'Summer11'

# Correlation to input
pileUpFileMCTrue       = 'CommonTools/MyTools/data/pileUpFileMC_Fall11.root'
pileUpFileMCObserved   = 'CommonTools/MyTools/data/pileUpFileMC_Fall11inTime.root'
pileUpFileDataTrue     = 'CommonTools/MyTools/data/pileUpFileData_2011truePixel.root'
pileUpFileDataObserved = 'CommonTools/MyTools/data/pileUpFileData_2011observedPixel.root'
#pileUpFileDataTrue     = 'CommonTools/MyTools/data/pileUpFileData_2011true.root'
#pileUpFileDataObserve = 'CommonTools/MyTools/data/pileUpFileData_2011observed.root'
ttSemiLeptonicEventMuons     = 'ttSemiLepEventHitFitMuons'
ttSemiLeptonicEventElectrons = 'ttSemiLepEventHitFitElectrons'
ttSemiLepEvtMuons     = 'ttSemiLepEventMCMatchMuons'
ttSemiLepEvtElectrons = 'ttSemiLepEventMCMatchElectrons'
ttSemiLepHypMuons     = 'selectedPatMuonsMCMatch'
ttSemiLepHypElectrons = 'selectedPatElectronsMCMatch'
ttSemiLepHypJets      = 'selectedPatJetsMCMatch'
skimProcess           = 'SKIM'
patMuons     = 'selectedPatMuonsHitFit'
patElectrons = 'selectedPatElectronsHitFit'
patJets      = 'selectedPatJetsHitFit'
patMETs      = 'patMETs'
jecLevels = [ 'L1FastJet'
            , 'L2Relative'
            , 'L3Absolute'
            ]

# TQAF MC match
runMatch        = True
matchMaxNJets   = 6                 # min: 4; default: 4
matchAlgorithm  = 'unambiguousOnly' # default: 'totalMinDist'
matchUseDeltaR  = True              # default: True
matchUseMaxDist = False             # default: False
matchMaxDist    = 0.3               # default: 0.3

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
  process.MessageLogger.categories.append( 'AnalyzeHitFit' )
  process.MessageLogger.cerr.AnalyzeHitFit = cms.untracked.PSet(
    limit = cms.untracked.int32( -1 )
  )
  process.MessageLogger.categories.append( 'TtSemiLepHypSelectionFilter' )
  process.MessageLogger.cerr.TtSemiLepHypSelectionFilter = cms.untracked.PSet(
    limit = cms.untracked.int32( -1 )
  )


### Input

inputFiles = [ 'file:%s/output/skimHitFit.local.root'%( os.getenv( "CMSSW_BASE" ) )
             ]
if rfioInput:
  from TopQuarkAnalysis.TopMassSemiLeptonic.input_hitFitPatSkimPF2PAT_cff import *
  inputFiles = files_Fall11_R4_1
elif runTest:
  inputFiles = [ 'file:%s/output/skimHitFit.test.local.root'%( os.getenv( "CMSSW_BASE" ) )
               ]
process.source = cms.Source(
  "PoolSource"
, fileNames = cms.untracked.vstring( inputFiles )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( -1 )
)
if runTest:
  process.maxEvents.input = 25000


### Output

outputFile = '%s/output/analyzeHitFit_from%s.root'%( os.getenv( "CMSSW_BASE" ), era )

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


### TQAF MC matcher

process.load( "TopQuarkAnalysis.TopEventProducers.sequences.ttSemiLepEvtBuilder_cff" )
process.ttSemiLepJetPartonMatch.maxNJets   = matchMaxNJets
process.ttSemiLepJetPartonMatch.algorithm  = matchAlgorithm
process.ttSemiLepJetPartonMatch.useDeltaR  = matchUseDeltaR
process.ttSemiLepJetPartonMatch.useMaxDist = matchUseMaxDist
process.ttSemiLepJetPartonMatch.maxDist    = matchMaxDist
process.ttSemiLepHypGenMatch.jetCorrectionLevel = jecLevels[ -1 ]
if not rfioInput:
  process.ttSemiLepEvent.verbosity = 1
  if runTest:
    process.ttSemiLepJetPartonMatch.verbosity = 1

from PhysicsTools.PatAlgos.tools.helpers import massSearchReplaceAnyInputTag
from PhysicsTools.PatAlgos.tools.helpers import cloneProcessingSnippet
process.makeTtSemiLepEventMCMatchMuons = cloneProcessingSnippet( process
                                                               , process.makeTtSemiLepEvent
                                                               , 'MCMatchMuons'
                                                               )
massSearchReplaceAnyInputTag( process.makeTtSemiLepEventMCMatchMuons
                            , 'selectedPatMuons'
                            , ttSemiLepHypMuons
                            )
massSearchReplaceAnyInputTag( process.makeTtSemiLepEventMCMatchMuons
                            , 'selectedPatJets'
                            , ttSemiLepHypJets
                            )
process.ttSemiLepEventMCMatchMuons.hypotheses = [ 'ttSemiLepHypGenMatchMCMatchMuons'
                                                ]
process.makeTtSemiLepEventMCMatchElectrons = cloneProcessingSnippet( process
                                                                   , process.makeTtSemiLepEvent
                                                                   , 'MCMatchElectrons'
                                                                   )
massSearchReplaceAnyInputTag( process.makeTtSemiLepEventMCMatchElectrons
                            , 'selectedPatMuons'
                            , ttSemiLepHypElectrons
                            )
massSearchReplaceAnyInputTag( process.makeTtSemiLepEventMCMatchElectrons
                            , 'selectedPatJets'
                            , ttSemiLepHypJets
                            )
process.ttSemiLepEventMCMatchElectrons.hypotheses = [ 'ttSemiLepHypGenMatchMCMatchElectrons'
                                                    ]

from TopQuarkAnalysis.TopTools.ttSemiLepHypSelectionFilter_cfi import ttSemiLepHypSelectionFilter
process.ttSemiLepHypSelectionFilterMuons     = ttSemiLepHypSelectionFilter.clone( ttSemiLepEvt        = ttSemiLepEvtMuons
                                                                                , ttSemiLepHypLeptons = ttSemiLepHypMuons
                                                                                , ttSemiLepHypJets    = ttSemiLepHypJets
                                                                                , processName         = skimProcess
                                                                                , leptonSelector      = patMuons
                                                                                , jetSelector         = patJets
                                                                                )
process.ttSemiLepHypSelectionFilterElectrons = ttSemiLepHypSelectionFilter.clone( ttSemiLepEvt        = ttSemiLepEvtElectrons
                                                                                , ttSemiLepHypLeptons = ttSemiLepHypElectrons
                                                                                , ttSemiLepHypJets    = ttSemiLepHypJets
                                                                                , processName         = skimProcess
                                                                                , leptonSelector      = patElectrons
                                                                                , jetSelector         = patJets
                                                                                )

process.matcherSequence = cms.Sequence(
  process.makeTtSemiLepEventMCMatchMuons
* process.ttSemiLepHypSelectionFilterMuons
* process.makeTtSemiLepEventMCMatchElectrons
* process.ttSemiLepHypSelectionFilterElectrons
)


### Analyzer

process.load( "TopQuarkAnalysis.TopMassSemiLeptonic.analyzeHitFit_cfi" )
process.analyzeHitFit.pileUpFileMCTrue       = pileUpFileMCTrue
process.analyzeHitFit.pileUpFileMCObserved   = pileUpFileMCObserved
process.analyzeHitFit.pileUpFileDataTrue     = pileUpFileDataTrue
process.analyzeHitFit.pileUpFileDataObserved = pileUpFileDataObserved
process.analyzeHitFit.ttSemiLeptonicEventMuons     = ttSemiLeptonicEventMuons
process.analyzeHitFit.ttSemiLeptonicEventElectrons = ttSemiLeptonicEventElectrons
process.analyzeHitFit.patMuons            = patMuons
process.analyzeHitFit.patElectrons        = patElectrons
process.analyzeHitFit.patJets             = patJets
process.analyzeHitFit.patMETs             = patMETs
process.analyzeHitFit.jecLevel            = jecLevels[ -1 ]
process.analyzeHitFit.muonResolutions     = 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafMuonResolution_%s.txt'%( era )
process.analyzeHitFit.electronResolutions = 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafElectronResolution_%s.txt'%( era )
process.analyzeHitFit.udscJetResolutions  = 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafUdscJetResolution_%s.txt'%( era )
process.analyzeHitFit.bJetResolutions     = 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafBJetResolution_%s.txt'%( era )
process.analyzeHitFit.metResolutions      = 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafKtResolution_%s.txt'%( era )
# process.analyzeHitFit_L5Flavor = process.analyzeHitFit.clone( jecLevel = 'L5Flavor'
#                                                                                            )
# process.analyzeHitFit_L7Parton = process.analyzeHitFit.clone( jecLevel = 'L7Parton'
#                                                                                          )
process.analyzeHitFitReference = process.analyzeHitFit.clone( ttSemiLeptonicEventMuons     = 'ttSemiLepEventReferenceMuons'
                                                            , ttSemiLeptonicEventElectrons = 'ttSemiLepEventReferenceElectrons'
                                                            , patMuons     = 'referencePatMuons'
                                                            , patElectrons = 'referencePatElectrons'
                                                            , patJets      = 'referencePatJets'
                                                            )


### Paths

process.standardPath = cms.Path(
  process.hltHighLevel
)
if runMatch:
  process.standardPath *= process.matcherSequence
process.standardPath *= process.analyzeHitFit

if not runTest:
  process.referencePath = cms.Path(
    process.hltHighLevelReference
  )
  if runMatch:
    process.referencePath *= process.matcherSequence
  process.referencePath *= process.analyzeHitFitReference


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
