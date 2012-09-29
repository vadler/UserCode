import os
import FWCore.ParameterSet.Config as cms


### Steering

# Misc
runTest   = True
rfioInput = False

# Origin of existing resolution functions
# era    = 'Spring10'
era    = 'Summer11'
sample = 'Fall11_R4_1'

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
patMuonsReference     = 'referencePatMuons'
patElectronsReference = 'referencePatElectrons'
patJetsReference      = 'referencePatJets'
patMETs      = 'patMETs'
jecLevels = [ 'L1FastJet'
            , 'L2Relative'
            , 'L3Absolute'
            ]

# TQAF MC match
matchMaxNJets   = 6                 # min: 4; default: 4
#matchAlgorithm  = 'unambiguousOnly' # default: 'totalMinDist'
matchAlgorithm  = 'totalMinDist' # default: 'totalMinDist'
matchUseDeltaR  = True              # default: True
matchUseMaxDist = False             # default: False, True enforced for 'unambiguousOnly' algorithm
matchMaxDist    = 0.3               # default: 0.3

### Initialization

process = cms.Process( "USER" )


### Logging

reportEvery = 1000
if runTest:
  reportEvery = 1
elif rfioInput:
  reportEvery = 100000
process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.MessageLogger.cerr.FwkReport.reportEvery = reportEvery
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool( True )
)
if runTest:
  process.MessageLogger.cerr.threshold = 'INFO'
  process.MessageLogger.categories.append( 'TtSemiLepHypSelectionFilter' )
  process.MessageLogger.cerr.TtSemiLepHypSelectionFilter = cms.untracked.PSet(
    limit = cms.untracked.int32( -1 )
  )
  process.MessageLogger.categories.append( 'AnalyzeHitFit' )
  process.MessageLogger.cerr.AnalyzeHitFit = cms.untracked.PSet(
    limit = cms.untracked.int32( -1 )
  )


### Input

inputFiles = [ 'file:%s/output/skimHitFit.local.root'%( os.getenv( "CMSSW_BASE" ) )
             ]
if rfioInput:
  from TopQuarkAnalysis.TopMassSemiLeptonic.input_hitFitPatSkimPF2PAT_cff import files
  if runTest:
    inputFiles = files[ 'Fall11_R4_test' ]
  else:
    inputFiles = files[ sample ]
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


### Output

outputFile = '%s/output/analyzeHitFit_from%s_%s_%s.root'%( os.getenv( "CMSSW_BASE" ), era, sample, matchAlgorithm )

if runTest:
  outputFile = outputFile.replace( 'root', 'test.root' )
if not rfioInput:
  outputFile = outputFile.replace( 'root', 'local.root' )
process.TFileService = cms.Service(
  "TFileService"
, fileName = cms.string( outputFile )
)
logFile = outputFile.replace( 'root', 'log' )
cfgFile = logFile.replace( '.log', '_cfg.py' )
pathPlots = '%s/output/plots/analyzeHitFit/analyzeHitFit_from%s_%s_%s_'%( os.getenv( "CMSSW_BASE" ), era, sample, matchAlgorithm )


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
if not rfioInput:
  process.ttSemiLepEvent.verbosity = 1
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

# MCMatch match
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

# HitFit match
process.makeTtSemiLepEventHitFitMuons = cloneProcessingSnippet( process
                                                              , process.makeTtSemiLepEvent
                                                              , 'HitFitMuons'
                                                              )
massSearchReplaceAnyInputTag( process.makeTtSemiLepEventHitFitMuons
                            , 'selectedPatMuons'
                            , patMuons
                            )
massSearchReplaceAnyInputTag( process.makeTtSemiLepEventHitFitMuons
                            , 'selectedPatJets'
                            , patJets
                            )
process.ttSemiLepEventHitFitMuons.hypotheses = [ 'ttSemiLepHypGenMatchHitFitMuons'
                                               ]
process.makeTtSemiLepEventHitFitElectrons = cloneProcessingSnippet( process
                                                                  , process.makeTtSemiLepEvent
                                                                  , 'HitFitElectrons'
                                                                  )
massSearchReplaceAnyInputTag( process.makeTtSemiLepEventHitFitElectrons
                            , 'selectedPatMuons'
                            , patElectrons
                            )
massSearchReplaceAnyInputTag( process.makeTtSemiLepEventHitFitElectrons
                            , 'selectedPatJets'
                            , patJets
                            )
process.ttSemiLepEventHitFitElectrons.hypotheses = [ 'ttSemiLepHypGenMatchHitFitElectrons'
                                                   ]

# Reference match
process.makeTtSemiLepEventReferenceMuons = cloneProcessingSnippet( process
                                                                 , process.makeTtSemiLepEvent
                                                                 , 'ReferenceMuons'
                                                                 )
massSearchReplaceAnyInputTag( process.makeTtSemiLepEventReferenceMuons
                            , 'selectedPatMuons'
                            , patMuonsReference
                            )
massSearchReplaceAnyInputTag( process.makeTtSemiLepEventReferenceMuons
                            , 'selectedPatJets'
                            , patJetsReference
                            )
process.ttSemiLepEventReferenceMuons.hypotheses = [ 'ttSemiLepHypGenMatchReferenceMuons'
                                                  ]
process.makeTtSemiLepEventReferenceElectrons = cloneProcessingSnippet( process
                                                                     , process.makeTtSemiLepEvent
                                                                     , 'ReferenceElectrons'
                                                                     )
massSearchReplaceAnyInputTag( process.makeTtSemiLepEventReferenceElectrons
                            , 'selectedPatMuons'
                            , patElectronsReference
                            )
massSearchReplaceAnyInputTag( process.makeTtSemiLepEventReferenceElectrons
                            , 'selectedPatJets'
                            , patJetsReference
                            )
process.ttSemiLepEventReferenceElectrons.hypotheses = [ 'ttSemiLepHypGenMatchReferenceElectrons'
                                                      ]

from TopQuarkAnalysis.TopTools.ttSemiLepHypSelectionFilter_cfi import ttSemiLepHypSelectionFilter
muonCut     = 'isTrackerMuon && pt > 26. && abs(eta) < 2.1 && globalTrack.normalizedChi2 < 10. && globalTrack.hitPattern.numberOfValidMuonHits > 0 && abs(dB) < 0.02 && innerTrack.hitPattern.trackerLayersWithMeasurement > 8 && innerTrack.hitPattern.pixelLayersWithMeasurement >= 1 && numberOfMatches > 1 && (chargedHadronIso+neutralHadronIso+photonIso)/pt < 0.125'
electronCut = 'et > 35. && abs(eta) < 2.5 && !(1.4442 < abs(superCluster.eta) && abs(superCluster.eta) < 1.5660) && abs(dB) < 0.02 && (electronID("eidHyperTight1MC") == 9. || electronID("eidHyperTight1MC") == 11. || electronID("eidHyperTight1MC") == 13. || electronID("eidHyperTight1MC") == 15.) && (chargedHadronIso+neutralHadronIso+photonIso)/pt < 0.1 && passConversionVeto && gsfTrack.trackerExpectedHitsInner.numberOfLostHits == 0'
jetCut      = 'pt > 30. && numberOfDaughters > 1 && chargedEmEnergyFraction < 0.99 && neutralHadronEnergyFraction < 0.99 && neutralEmEnergyFraction < 0.99 && (chargedHadronEnergyFraction > 0. || abs(eta) >= 2.4) && (chargedMultiplicity > 0 || abs(eta) >= 2.4) && abs(eta) < 3.0 && abs(eta) < 2.5'
process.ttSemiLepHypSelectionFilterMuons = ttSemiLepHypSelectionFilter.clone( ttSemiLepEvt        = ttSemiLepEvtMuons
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

process.ttSemiLepHypSelectionFilterMuonsReference = ttSemiLepHypSelectionFilter.clone( ttSemiLepEvt        = ttSemiLepEvtMuons
                                                                                     , ttSemiLepHypLeptons = ttSemiLepHypMuons
                                                                                     , ttSemiLepHypJets    = ttSemiLepHypJets
                                                                                     , processName         = skimProcess
                                                                                     , leptonSelector      = patMuonsReference
                                                                                     , jetSelector         = patJetsReference
                                                                                     , jetCut              = cms.string( jetCut )
                                                                                     , leptonCut           = cms.string( muonCut )
                                                                                     )
process.ttSemiLepHypSelectionFilterElectronsReference = ttSemiLepHypSelectionFilter.clone( ttSemiLepEvt        = ttSemiLepEvtElectrons
                                                                                         , ttSemiLepHypLeptons = ttSemiLepHypElectrons
                                                                                         , ttSemiLepHypJets    = ttSemiLepHypJets
                                                                                         , processName         = skimProcess
                                                                                         , leptonSelector      = patElectronsReference
                                                                                         , jetSelector         = patJetsReference
                                                                                         , jetCut              = cms.string( jetCut )
                                                                                         , leptonCut           = cms.string( electronCut )
                                                                                         )

process.matcherSequenceBase = cms.Sequence(
  process.makeTtSemiLepEventMCMatchMuons
* process.makeTtSemiLepEventMCMatchElectrons
)
process.matcherSequence = cms.Sequence(
  process.ttSemiLepHypSelectionFilterMuons
* process.ttSemiLepHypSelectionFilterElectrons
* process.makeTtSemiLepEventHitFitMuons
* process.makeTtSemiLepEventHitFitElectrons
)
process.matcherSequenceReference = cms.Sequence(
  process.ttSemiLepHypSelectionFilterMuonsReference
* process.ttSemiLepHypSelectionFilterElectronsReference
* process.makeTtSemiLepEventReferenceMuons
* process.makeTtSemiLepEventReferenceElectrons
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
if runTest:
  pathPlots = pathPlots.replace( 'analyzeHitFit', 'analyzeHitFitTest', 1 )
else:
  process.analyzeHitFit.pathPlots = pathPlots
#process.analyzeHitFit.pathPlots = pathPlots
# process.analyzeHitFit_L5Flavor = process.analyzeHitFit.clone( jecLevel = 'L5Flavor'
#                                                                                            )
# process.analyzeHitFit_L7Parton = process.analyzeHitFit.clone( jecLevel = 'L7Parton'
#                                                                                          )
process.analyzeHitFitReference = process.analyzeHitFit.clone( ttSemiLeptonicEventMuons     = 'ttSemiLepEventReferenceMuons'
                                                            , ttSemiLeptonicEventElectrons = 'ttSemiLepEventReferenceElectrons'
                                                            , patMuons     = patMuonsReference
                                                            , patElectrons = patElectronsReference
                                                            , patJets      = patJetsReference
                                                            )


### Paths

process.standardPath = cms.Path(
  process.hltHighLevel
* process.matcherSequenceBase
* process.matcherSequence
* process.analyzeHitFit
)

if not runTest:
  process.referencePath = cms.Path(
    process.hltHighLevelReference
  * process.matcherSequenceBase
  * process.matcherSequenceReference
  * process.analyzeHitFitReference
  )


### Messages

cfgFile = logFile.replace( '.log', '_cfg.py' )
f = open( cfgFile, 'w' )
print >> f, process.dumpPython()
f.close()

print
print 'Config file:'
print '------------'
print cfgFile
print
print 'Output file:'
print '------------'
print outputFile
if process.analyzeHitFit.pathPlots != '':
  print
  print 'Plots destination:'
  print '------------------'
  print pathPlots + '*.png'
print
print 'Log file destination:'
print '---------------------'
print logFile
print '================================================================================'
print
