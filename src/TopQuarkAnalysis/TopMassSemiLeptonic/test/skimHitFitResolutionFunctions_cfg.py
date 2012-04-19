import sys
import os
import FWCore.ParameterSet.Config as cms


### Steering

# Misc
runCrab    = False
runTest    = True
reportTime = False

# Input
cmsswVersion = 'CMSSW_4_4_2'
relVal       = 'RelValProdTTbar'
dataTier     = 'AODSIM'
globalTag    = 'START44_V7'

# Trigger
hltProcess       = 'HLT'
triggerSelection = 'HLT_IsoMu17_v*'

# Vertices
pvCollection = 'goodOfflinePrimaryVertices' #'offlinePrimaryVertices' or 'goodOfflinePrimaryVertices'
                                            # recommended: 'goodOfflinePrimaryVertices' (s. https://hypernews.cern.ch/HyperNews/CMS/get/top-selection/38/1/1/1/2/1/1/2/1/3/1.html)

# Muons
# switch for muon isolation cone, PF2PAT default: 0.4 (False)
muonsIsoR03           = True # isolation cone of 0.3, if 'True'
# muon top projection isolation selection, PF2PAT default: 0.15
muonsSelectIsoPf      = 0.5
# muon top projection object selection, PF2PAT default: 'pt > 5.'
muonSelectPf          = 'pt > 5.'
# muon object selection
muonSelectHitFit      = 'abs(eta) < 2.4'
muonSelectVeto        = 'isGlobalMuon && pt > 10. && abs(eta) < 2.5 && (chargedHadronIso+neutralHadronIso+photonIso)/pt < 0.2'
muonSelect            = muonSelectHitFit + ' && ' + muonSelectVeto
muonSelectSignal      = 'isTrackerMuon && pt > 20. && abs(eta) < 2.1 && globalTrack.normalizedChi2 < 10. && globalTrack.hitPattern.numberOfValidMuonHits > 0 && abs(dB) < 0.02 && innerTrack.numberOfValidHits > 10 && innerTrack.hitPattern.pixelLayersWithMeasurement >= 1 && numberOfMatches > 1 && (chargedHadronIso+neutralHadronIso+photonIso)/pt < 0.125'
muonSelectSignalJetDR = 0.3
# counters
muonsMin = 1
muonsMax = 1
selectedMuonsMin = 0
selectedMuonsMax = 1
referenceMuonsMin = 1
referenceMuonsMax = 1

# Electrons
# switch for muon isolation cone, PF2PAT default: 0.4 (False)
electronsIsoR03      = True # isolation cone of 0.3, if 'True'
# electron top projection isolation selection, PF2PAT: 0.2
electronsSelectIsoPf = 0.5
# electron top projection object selection, PF2PAT default: 'pt > 5. && gsfTrackRef.isNonnull && gsfTrackRef.trackerExpectedHitsInner.numberOfLostHits < 2'
electronSelectPf     = 'pt > 5. && gsfTrackRef.isNonnull && gsfTrackRef.trackerExpectedHitsInner.numberOfLostHits < 2'
# electron object selection
electronSelectHitFit = 'abs(eta) < 2.5'
electronSelectVeto   = 'et > 15. && abs(eta) < 2.5 && (chargedHadronIso+neutralHadronIso+photonIso)/pt < 0.2'
electronSelect       = electronSelectHitFit + ' && ' + electronSelectVeto
electronSelectSignal = ''
# counters
electronsMin = 0
electronsMax = 0
selectedElectronsMin = 0
selectedElectronsMax = 0
referenceElectronsMin = 0
referenceElectronsMax = 0

# X-Leptons
embedLeptonTracks = False
# counters
leptonsMin = 1
leptonsMax = 1
selectedLeptonsMin = 0
selectedLeptonsMax = 1
referenceLeptonsMin = 1
referenceLeptonsMax = 1

# Jets
# algo & JECs
jetAlgo   = 'AK5'
jecLevels = [ 'L1FastJet'
            , 'L2Relative'
            , 'L3Absolute'
            ]
# jet object selection
jetSelectHitFit = 'abs(eta) < 3.0'
jetSelectVeto   = 'pt > 30. && abs(eta) < 2.4 && numberOfDaughters > 1 && chargedEmEnergyFraction < 0.99 && neutralHadronEnergyFraction < 0.99 && neutralEmEnergyFraction < 0.99 && (chargedHadronEnergyFraction > 0. || abs(eta) >= 2.4) && (chargedMultiplicity > 0 || abs(eta) >= 2.4)'
jetSelect       = jetSelectHitFit + ' && ' + jetSelectVeto
jetSelectSignal = ''
#jetBTagAlgo          = '' # empty string switches the use of b-Tagging off
#jetBTagAlgo          = 'trackCountingHighPurBJetTags' # empty string switches the use of b-Tagging off
jetBTagAlgo          = 'simpleSecondaryVertexHighEffBJetTags'
# working points from https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagPerformanceOP:
#jetMinBDiscBJets     = 1.93 # TCHPM
#jetMaxBDiscLightJets = 1.93 # should it be the same?
#jetMinBDiscBJets     = 3.14 # TCHPT
#jetMaxBDiscLightJets = 3.14 # should it be the same?
jetMinBDiscBJets     = 1.74 # SSVHEM
jetMaxBDiscLightJets = 1.74 # should it be the same?
# counters
jetsMin = 4
jetsMax = 999999
selectedJetsMin = 0
selectedJetsMax = 999999
referenceJetsMin = 4
referenceJetsMax = 999999


### Initialization

process = cms.Process( 'SKIM' )

if len( jecLevels ) == 0:
  print 'ERROR: No JECs are applied!'
  print '       HitFit does not allow for that. Exit....'
  sys.exit( 1 )

pvCollectionTag = pvCollection + '::%s'%( process.name_() )

if runTest:
  process.Timing = cms.Service( "Timing"
  , summaryOnly = cms.untracked.bool( not reportTime )
  )


### Logging

reportEvery = 1000
if runTest:
  reportEvery = 1
process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.MessageLogger.cerr.FwkReport.reportEvery = reportEvery
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool( True )
)


### Conditions

process.load( "Configuration.StandardSequences.Geometry_cff" )
process.load( "Configuration.StandardSequences.MagneticField_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
# from Configuration.AlCa.autoCond import autoCond
# process.GlobalTag.globaltag = autoCond[ 'startup' ]
process.GlobalTag.globaltag = 'START44_V13::All'
if runCrab:
  process.GlobalTag.globaltag = 'START44_V5D::All' # Fall11_R3
  #process.GlobalTag.globaltag = 'START44_V9C::All' # Fall11_R4


### Input

inputEvents = -1
if runTest:
  inputEvents = 1000

process.source = cms.Source( "PoolSource"
, noEventSort        = cms.untracked.bool( True )
, duplicateCheckMode = cms.untracked.string( 'noDuplicateCheck' )
, fileNames          = cms.untracked.vstring()
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( inputEvents )
)

if not runCrab:
  from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
  process.source.fileNames = pickRelValInputFiles( cmsswVersion = cmsswVersion
                                                 , relVal       = relVal
                                                 , dataTier     = dataTier
                                                 , globalTag    = globalTag
                                                 , maxVersions  = 1
                                                 )
  triggerSelection = 'HLT_IsoMu15_v*'


### Output

process.out = cms.OutputModule( "PoolOutputModule"
, fileName       = cms.untracked.string( 'skimHitFitResolutionFunctions.root' )
, SelectEvents   = cms.untracked.PSet(
    SelectEvents = cms.vstring(
                              )
  )
, outputCommands = cms.untracked.vstring() # screwed up by PAT tools later on anyway :-(
, dropMetaData   = cms.untracked.string( 'ALL' )
)

outputFile = '%s/output/skimHitFitResolutionFunctions.root'%( os.getenv( "CMSSW_BASE" ) )
if runTest:
  outputFile = outputFile.replace( 'root', 'test.root' )
if not runCrab:
  outputFile = outputFile.replace( 'root', 'local.root' )
  process.out.fileName = outputFile
logFile = outputFile.replace( 'root', 'log' )


### Cleaning

# Fix for Pythia bug in 2011 MC
process.load("GeneratorInterface.GenFilters.TotalKinematicsFilter_cfi")
process.totalKinematicsFilter.tolerance = 5. # from Martijn

# Trigger
if triggerSelection == '' or triggerSelection == '*':
  triggerSelection = 'HLT_*'
process.load( "HLTrigger.HLTfilters.triggerResultsFilter_cfi" )
process.triggerResultsFilter.hltResults        = cms.InputTag( 'TriggerResults::%s'%( hltProcess ) )
process.triggerResultsFilter.l1tResults        = cms.InputTag( '' )
process.triggerResultsFilter.triggerConditions = [ triggerSelection ]
process.triggerResultsFilter.throw             = False

# Vertices
process.goodOfflinePrimaryVertices = cms.EDFilter(
  "PrimaryVertexObjectFilter"
, src          = cms.InputTag( 'offlinePrimaryVertices' )
, filter       = cms.bool( True )
, filterParams = cms.PSet(
    minNdof = cms.double(  4. )
  , maxZ    = cms.double( 24. )
  , maxRho  = cms.double(  2. )
  )
)

process.eventCleaning = cms.Sequence( process.totalKinematicsFilter
                                    + process.triggerResultsFilter
                                    + process.goodOfflinePrimaryVertices
                                    )


### PAT

process.load( "PhysicsTools.PatAlgos.patSequences_cff" )

# Misc
from PhysicsTools.PatAlgos.tools.coreTools import *
removeSpecificPATObjects( process
                        , names = [ 'Photons', 'Taus' ]
                        )
process.patJets.addGenJetMatch = False
process.patJets.genJetMatch    = ''
process.patDefaultSequence.remove( process.patJetGenJetMatch )

# PF2PAT
from PhysicsTools.PatAlgos.tools.pfTools import usePF2PAT
usePF2PAT( process
         , runOnMC        = True
         , jetAlgo        = jetAlgo
         , jetCorrections = ( '%sPFchs'%( jetAlgo )
                            , jecLevels
                            )
         , pvCollection   = cms.InputTag( pvCollection )
         )
# still need to fix event content afterwards :-(
process.out.outputCommands.append( 'keep *_addPileupInfo_*_*' )
process.out.outputCommands.append( 'keep edmTriggerResults_*_*_*' )
process.out.outputCommands.append( 'keep *_offlinePrimaryVertices_*_*' )
process.out.outputCommands.append( 'keep *_%s_*_*'%( pvCollection ) )

process.patPF2PATSequence.remove( process.ak5GenJetsNoNu )
process.patPF2PATSequence.remove( process.ak7GenJetsNoNu )
process.patPF2PATSequence.remove( process.iterativeCone5GenJetsNoNu )
process.patPF2PATSequence.remove( process.genParticlesForJetsNoNu )

process.patPF2PATSequence.remove( process.patPFParticles )
process.patPF2PATSequence.remove( process.selectedPatPFParticles )
process.patPF2PATSequence.remove( process.countPatPFParticles )
# Restructure counters
process.patPF2PATSequence.remove( process.countPatMuons )
process.patPF2PATSequence.remove( process.countPatElectrons )
process.patPF2PATSequence.remove( process.countPatLeptons )
process.patPF2PATSequence.remove( process.countPatJets )
process.countSelectedPatMuons     = process.countPatMuons.clone( minNumber = selectedMuonsMin
                                                               , maxNumber = selectedMuonsMax
                                                               )
process.countSelectedPatElectrons = process.countPatElectrons.clone( minNumber = selectedElectronsMin
                                                                   , maxNumber = selectedElectronsMax
                                                                   )
process.countSelectedPatLeptons   = process.countPatLeptons.clone( minNumber = selectedLeptonsMin
                                                                 , maxNumber = selectedLeptonsMax
                                                                 )
process.countSelectedPatJets      = process.countPatJets.clone( minNumber = selectedJetsMin
                                                              , maxNumber = selectedJetsMax
                                                              )
process.patPF2PATCounters = cms.Sequence( process.countSelectedPatMuons
                                        + process.countSelectedPatElectrons
                                        + process.countSelectedPatLeptons
                                        + process.countSelectedPatJets
                                        )
process.countSelectedPatMuonsHitFit = process.countPatMuons.clone( minNumber = referenceMuonsMin
                                                                 , maxNumber = referenceMuonsMax
                                                                 )
process.countSelectedPatJetsHitFit = process.countPatJets.clone( minNumber = referenceJetsMin
                                                               , maxNumber = referenceJetsMax
                                                               )
process.patPF2PATCountersHitFit = cms.Sequence( process.countSelectedPatMuonsHitFit
                                              + process.countSelectedPatJetsHitFit
                                              )

# Final object selection
process.referencePatMuons     = process.cleanPatMuons.clone( preselection  = ''
                                                           , checkOverlaps = cms.PSet()
                                                           )
process.referencePatElectrons = process.cleanPatElectrons.clone( preselection  = ''
                                                               , checkOverlaps = cms.PSet()
                                                               )
process.referencePatJets      = process.cleanPatJets.clone( preselection  = ''
                                                          , checkOverlaps = cms.PSet()
                                                          )
process.referencePatCandidateSummary = process.cleanPatCandidateSummary.clone( logName = 'referencePatCandidates|PATSummaryTables'
                                                                             , candidates = cms.VInputTag ( cms.InputTag( 'referencePatMuons' )
                                                                                                          , cms.InputTag( 'referencePatElectrons' )
                                                                                                          , cms.InputTag( 'referencePatJets' )
                                                                                                          )
                                                                             )
process.patReferenceSequence = cms.Sequence( process.referencePatMuons
                                           + process.referencePatElectrons
                                           + process.referencePatJets
                                           )
process.patReferenceSequence *= process.referencePatCandidateSummary

# Reference selection counters
process.countReferencePatMuons     = process.countPatMuons.clone( src       = cms.InputTag( 'referencePatMuons' )
                                                                , minNumber = referenceMuonsMin
                                                                , maxNumber = referenceMuonsMax
                                                                )
process.countReferencePatElectrons = process.countPatElectrons.clone( src       = cms.InputTag( 'referencePatElectrons' )
                                                                    , minNumber = referenceElectronsMin
                                                                    , maxNumber = referenceElectronsMax
                                                                    )
process.countReferencePatLeptons   = process.countPatLeptons.clone( muonSource     = cms.InputTag( 'referencePatMuons' )
                                                                  , electronSource = cms.InputTag( 'referencePatElectrons' )
                                                                  , tauSource      = cms.InputTag( 'referencePatTaus' )      # only for consistency, since not active
                                                                  , minNumber      = referenceLeptonsMin
                                                                  , maxNumber      = referenceLeptonsMax
                                                                  )
process.countReferencePatJets      = process.countPatJets.clone( src = cms.InputTag( 'referencePatJets' )
                                                               , minNumber = referenceJetsMin
                                                               , maxNumber = referenceJetsMax
                                                               )
process.patReferenceCounters = cms.Sequence( process.countReferencePatMuons
                                           + process.countReferencePatElectrons
                                           + process.countReferencePatLeptons
                                           + process.countReferencePatJets
                                           )

process.out.outputCommands.append( 'keep *_referencePat*_*_*' )

# Muons
process.pfSelectedMuons.cut = muonSelectPf
if muonsIsoR03:
  process.pfIsolatedMuons.isolationValueMapsCharged = cms.VInputTag( cms.InputTag( 'muPFIsoValueCharged03' )
                                                                   )
  process.pfIsolatedMuons.isolationValueMapsNeutral = cms.VInputTag( cms.InputTag( 'muPFIsoValueNeutral03' )
                                                                   , cms.InputTag( 'muPFIsoValueGamma03' )
                                                                   )
  process.pfIsolatedMuons.deltaBetaIsolationValueMap = cms.InputTag( 'muPFIsoValuePU03' )
  process.pfMuons.isolationValueMapsCharged = cms.VInputTag( cms.InputTag( 'muPFIsoValueCharged03' )
                                                           )
  process.pfMuons.isolationValueMapsNeutral = cms.VInputTag( cms.InputTag( 'muPFIsoValueNeutral03' )
                                                           , cms.InputTag( 'muPFIsoValueGamma03' )
                                                           )
  process.pfMuons.deltaBetaIsolationValueMap = cms.InputTag( 'muPFIsoValuePU03' )
process.pfIsolatedMuons.isolationCut = muonsSelectIsoPf
process.patMuons.embedTrack = embedLeptonTracks
if muonsIsoR03:
  process.patMuons.isolationValues.pfNeutralHadrons   = cms.InputTag( 'muPFIsoValueNeutral03' )
  process.patMuons.isolationValues.pfPhotons          = cms.InputTag( 'muPFIsoValueGamma03' )
  process.patMuons.isolationValues.pfChargedHadrons   = cms.InputTag( 'muPFIsoValueCharged03' )
  process.patMuons.isolationValues.pfPUChargedHadrons = cms.InputTag( 'muPFIsoValuePU03' )
process.selectedPatMuons.cut = muonSelect
process.referencePatMuons.preselection = muonSelectSignal
process.referencePatMuons.checkOverlaps = cms.PSet(
  jets = cms.PSet(
    src                 = cms.InputTag( 'selectedPatJets' )
  , algorithm           = cms.string( 'byDeltaR' )
  , preselection        = cms.string( '' )
  , deltaR              = cms.double( muonSelectSignalJetDR )
  , checkRecoComponents = cms.bool( False )
  , pairCut             = cms.string( '' )
  , requireNoOverlaps   = cms.bool( True)
  )
)

# Electrons
process.pfSelectedElectrons.cut = electronSelectPf
if electronsIsoR03:
  process.pfIsolatedElectrons.isolationValueMapsCharged = cms.VInputTag( cms.InputTag( 'elPFIsoValueCharged03PFId' )
                                                                       )
  process.pfIsolatedElectrons.isolationValueMapsNeutral = cms.VInputTag( cms.InputTag( 'elPFIsoValueNeutral03PFId' )
                                                                       , cms.InputTag( 'elPFIsoValueGamma03PFId' )
                                                                       )
  process.pfIsolatedElectrons.deltaBetaIsolationValueMap = cms.InputTag( 'elPFIsoValuePU03PFId' )
  process.pfElectrons.isolationValueMapsCharged = cms.VInputTag( cms.InputTag( 'elPFIsoValueCharged03PFId' )
                                                               )
  process.pfElectrons.isolationValueMapsNeutral = cms.VInputTag( cms.InputTag( 'elPFIsoValueNeutral03PFId' )
                                                               , cms.InputTag( 'elPFIsoValueGamma03PFId' )
                                                               )
  process.pfElectrons.deltaBetaIsolationValueMap = cms.InputTag( 'elPFIsoValuePU03PFId' )
process.pfIsolatedElectrons.isolationCut = electronsSelectIsoPf
process.patElectrons.embedTrack = embedLeptonTracks
if electronsIsoR03:
  process.patElectrons.isolationValues.pfNeutralHadrons   = cms.InputTag( 'elPFIsoValueNeutral03PFId' )
  process.patElectrons.isolationValues.pfPhotons          = cms.InputTag( 'elPFIsoValueGamma03PFId' )
  process.patElectrons.isolationValues.pfChargedHadrons   = cms.InputTag( 'elPFIsoValueCharged03PFId' )
  process.patElectrons.isolationValues.pfPUChargedHadrons = cms.InputTag( 'elPFIsoValuePU03PFId' )
process.selectedPatElectrons.cut = electronSelect
process.referencePatElectrons.preselection = electronSelectSignal

# X-leptons

# Jets
if 'L1FastJet' in jecLevels:
  process.pfPileUp.checkClosestZVertex = False
process.patJets.embedPFCandidates = False
process.selectedPatJets.cut = jetSelect
process.referencePatJets.preselection = jetSelectSignal

process.out.outputCommands.append( 'drop *_selectedPatJets*_caloTowers_*' )
process.out.outputCommands.append( 'drop *_selectedPatJets*_tagInfos_*' )
process.out.outputCommands.append( 'drop *_selectedPatJets*_pfCandidates_*' )
process.out.outputCommands.append( 'drop *_kt6PFJets_*_*' )
process.out.outputCommands.append( 'drop *_selectedPatJets*_genJets_*' )


### TQAF

process.load( "TopQuarkAnalysis.TopEventProducers.sequences.ttSemiLepEvtBuilder_cff" )
if not runCrab:
  process.ttSemiLepEvent.verbosity = 1
from TopQuarkAnalysis.TopEventProducers.sequences.ttSemiLepEvtBuilder_cff import addTtSemiLepHypotheses
process.load( "TopQuarkAnalysis.TopEventProducers.sequences.ttGenEvent_cff" )
addTtSemiLepHypotheses( process
                      , [ "kGenMatch"
                        ]
                       )
process.ttSemiLepJetPartonMatch.maxNJets = 6
process.ttSemiLepJetPartonMatch.maxNComb = 1 # no knobs to turn to get several solutions
if runTest and not runCrab:
  process.ttSemiLepJetPartonMatch.verbosity = 1
process.ttSemiLepHypGenMatch.jetCorrectionLevel = jecLevels[ -1 ]

process.out.outputCommands.append( 'keep *_*TtSemiLepEventHypothesis*_*_*' )
process.out.outputCommands.append( 'keep *_ttSemiLep*_*_*' )

process.out.outputCommands.append( 'keep *_genEvt*_*_*' )
process.out.outputCommands.append( 'keep *_initSubset*_*_*' )
process.out.outputCommands.append( 'keep *_decaySubset*_*_*' )

from PhysicsTools.PatAlgos.tools.helpers import massSearchReplaceAnyInputTag
from PhysicsTools.PatAlgos.tools.helpers import cloneProcessingSnippet
process.makeTtSemiLepEventReference = cloneProcessingSnippet( process
                                                            , process.makeTtSemiLepEvent
                                                            , 'Reference'
                                                            )
massSearchReplaceAnyInputTag( process.makeTtSemiLepEventReference
                            , 'selectedPatMuons'
                            , 'referencePatMuons'
                             )
massSearchReplaceAnyInputTag( process.makeTtSemiLepEventReference
                            , 'selectedPatJets'
                            , 'referencePatJets'
                             )


### Paths

# Cleaning + PF2PAT
process.pf2PatSequence = cms.Sequence( process.eventCleaning
                                     * process.patPF2PATSequence
                                     * process.patPF2PATCounters
                                     * process.makeGenEvt
                                     )
process.pf2PatPath = cms.Path( process.pf2PatSequence
                             * process.makeTtSemiLepEvent
                             )

# Reference selection with HitFit
process.referencePath = cms.Path( process.pf2PatSequence
                                * process.patReferenceSequence
                                * process.patReferenceCounters
                                * process.makeTtSemiLepEventReference
                                )

process.out.SelectEvents.SelectEvents = [ 'pf2PatPath'
                                        ]

# Outpath
process.outPath = cms.EndPath(
  process.out
)

process.schedule = cms.Schedule(
  process.pf2PatPath
, process.referencePath
, process.outPath
)


### Messages

if not runCrab:
  print
  print 'Input:'
  print '------'
  if inputEvents == -1:
    print 'all events'
  else:
    print '%i events'%( inputEvents )
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
