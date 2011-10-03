import sys
import os
import FWCore.ParameterSet.Config as cms


### Steering

# Misc
runTest    = True
reportTime = False

# Input
runOnMC             = True
relValMC            = 'RelValTTbar'
#relValMC            = 'RelValZMM'
#relValMC            = 'RelValWM'
relValMCGlobalTag   = 'START42_V12'
relValData          = 'Mu'
relValDataGlobalTag = 'GR_R_42_V14_mu2010B'
#relValData          = 'Jet'
#relValDataGlobalTag = 'GR_R_42_V14_jet2010B'

runPartonMatch = True
runJetMatch    = True
runCiC         = False
runEwk         = False

# Trigger
hltProcess       = 'HLT'
#triggerSelection = 'HLT_IsoMu17 OR HLT_IsoMu17_v*'
triggerSelection = 'HLT_IsoMu17_v*'

# Vertices
pvCollection = 'goodOfflinePrimaryVertices' #'offlinePrimaryVertices' or 'goodOfflinePrimaryVertices'
                                            # recommended: 'goodOfflinePrimaryVertices' (s. https://hypernews.cern.ch/HyperNews/CMS/get/top-selection/38/1/1/1/2/1/1/2/1/3/1.html)

# Muons
# switch for muon isolation cone, PF2PAT default: 0.4 (False)
muonsIsoR03           = False # isolation cone of 0.3, if 'True'
# muon top projection isolation selection, PF2PAT default: 0.15
muonsSelectIsoPf      = 0.2
# muon top projection object selection, PF2PAT default: 'pt > 5.'
muonSelectPf          = 'pt > 5.'
# muon object selection
muonSelectHitFit      = 'abs(eta) < 9.0'
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
electronsSelectIsoPf = 0.2
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
embedLeptonTracks = True
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
            , 'L2L3Residual'
            ]
# jet object selection
jetSelectHitFit = 'abs(eta) < 3.0'
jetSelectVeto   = 'pt > 30. && abs(eta) < 2.4 && numberOfDaughters > 1 && chargedEmEnergyFraction < 0.99 && neutralHadronEnergyFraction < 0.99 && neutralEmEnergyFraction < 0.99 && (chargedHadronEnergyFraction > 0. || abs(eta) >= 2.4) && (chargedMultiplicity > 0 || abs(eta) >= 2.4)'
jetSelect       = jetSelectHitFit + ' && ' + jetSelectVeto
jetSelectSignal = ''
# counters
jetsMin = 4
jetsMax = 999999
selectedJetsMin = 0
selectedJetsMax = 999999
referenceJetsMin = 4
referenceJetsMax = 999999


### Initialization

process = cms.Process( 'HitFit' )

if len( jecLevels ) == 0:
  print 'ERROR: No JECs are applied!'
  print '       HitFit does not allow for that. Exit....'
  sys.exit( 1 )

pvCollection += '::%s'%( process.name_() )

# MC related
runPartonMatch = runPartonMatch and runOnMC
runJetMatch    = runJetMatch    and runOnMC
if runOnMC and 'L2L3Residual' in jecLevels:
  jecLevels.remove( 'L2L3Residual' )


### Logging

reportEvery = 1000
if runTest:
  reportEvery = 1
process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.MessageLogger.cerr.FwkReport.reportEvery = reportEvery
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool( True )
)
process.Timing = cms.Service( "Timing"
, summaryOnly = cms.untracked.bool( not reportTime )
)


### Conditions

process.load( "Configuration.StandardSequences.Geometry_cff" )
process.load( "Configuration.StandardSequences.MagneticField_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
if runOnMC:
  process.GlobalTag.globaltag = 'START42_V13::All'
else:
  process.GlobalTag.globaltag = 'GR_R_42_V19::All'


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

sample = ''
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
if runOnMC:
  sample = relValMC
  process.source.fileNames = pickRelValInputFiles( cmsswVersion  = 'CMSSW_4_2_8'
                                                 , relVal        = relValMC
                                                 , globalTag     = relValMCGlobalTag
                                                 )
else:
  sample = relValData
  process.source.fileNames = pickRelValInputFiles( cmsswVersion  = 'CMSSW_4_2_8'
                                                 , relVal        = relValData
                                                 , dataTier      = 'RECO'
                                                 , globalTag     = relValDataGlobalTag
                                                 )


### Output

outputFile = '%s/output/hitFitPatSkimPF2PAT_%s.root'%( os.getenv( "CMSSW_BASE" ), sample )
if runTest:
  outputFile = 'hitFitPatSkimPF2PAT.root'

process.out = cms.OutputModule( "PoolOutputModule"
, fileName       = cms.untracked.string( outputFile )
, SelectEvents   = cms.untracked.PSet(
    SelectEvents = cms.vstring(
                              )
  )
, outputCommands = cms.untracked.vstring() # screwed up by PAT tolls lateron anyway :-(
, dropMetaData   = cms.untracked.string( 'ALL' )
)

# Outpath
process.outpath = cms.EndPath(
  process.out
)


### Cleaning

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

# HBHE noise filter
process.load( "CommonTools.RecoAlgos.HBHENoiseFilter_cfi" )
process.HBHENoiseFilter.minIsolatedNoiseSumE        = 999999.
process.HBHENoiseFilter.minNumIsolatedNoiseChannels = 999999
process.HBHENoiseFilter.minIsolatedNoiseSumEt       = 999999.

# Scraping filter
process.scrapingFilter = cms.EDFilter( "FilterOutScraping"
, applyfilter = cms.untracked.bool( True )
, debugOn     = cms.untracked.bool( False )
, numtrack    = cms.untracked.uint32( 10 )
, thresh      = cms.untracked.double( 0.25 )
)

process.eventCleaning = cms.Sequence ( process.triggerResultsFilter
                                     + process.goodOfflinePrimaryVertices
                                     + process.HBHENoiseFilter
                                     + process.scrapingFilter
                                     )


### PAT

process.load( "PhysicsTools.PatAlgos.patSequences_cff" )

# Misc
from PhysicsTools.PatAlgos.tools.coreTools import *
if not runOnMC:
  runOnData( process
           , names = [ 'All' ]
           )
removeSpecificPATObjects( process
                        , names = [ 'Photons', 'Taus' ]
                        )
if runOnMC:
  if not runPartonMatch:
    process.patMuons.addGenMatch      = False
    process.patMuons.embedGenMatch    = False
    process.patMuons.genParticleMatch = ''
    process.patDefaultSequence.remove( process.muonMatch )
    process.patElectrons.addGenMatch      = False
    process.patElectrons.embedGenMatch    = False
    process.patElectrons.genParticleMatch = ''
    process.patDefaultSequence.remove( process.electronMatch )
    process.patJets.addGenPartonMatch   = False
    process.patJets.embedGenPartonMatch = False
    process.patJets.genPartonMatch      = ''
    process.patDefaultSequence.remove( process.patJetPartonMatch )
    process.patJets.getJetMCFlavour    = False
    process.patJets.JetPartonMapSource = ''
    process.patDefaultSequence.remove( process.patJetFlavourId )
  if not runJetMatch:
    process.patJets.addGenJetMatch = False
    process.patJets.genJetMatch    = ''
    process.patDefaultSequence.remove( process.patJetGenJetMatch )

# PF2PAT
from PhysicsTools.PatAlgos.tools.pfTools import usePF2PAT
usePF2PAT( process
         , runOnMC        = runOnMC
         , jetAlgo        = jetAlgo
         , jetCorrections = ( '%sPFchs'%( jetAlgo )
                            , jecLevels
                            )
         , pvCollection   = cms.InputTag( pvCollection )
         )
# still need to fix event content afterwards :-(
process.out.outputCommands.append( 'keep edmTriggerResults_*_*_*' )
process.out.outputCommands.append( 'drop edmTriggerResults_*_*_*RECO*' )
process.out.outputCommands.append( 'drop edmTriggerResults_*_*_*NONE*' )
process.out.outputCommands.append( 'keep *_hltTriggerSummaryAOD_*_*' )
process.out.outputCommands.append( 'keep *_offlineBeamSpot_*_*' )
process.out.outputCommands.append( 'keep *_offlinePrimaryVertices_*_*' )
process.out.outputCommands.append( 'keep *_%s_*_*'%( process.goodOfflinePrimaryVertices.label_() ) )
if not runOnMC:
  process.out.outputCommands.append( 'keep *_addPileupInfo_*_*' )

if jetAlgo != 'AK5' or not runJetMatch:
  process.patPF2PATSequence.remove( process.ak5GenJetsNoNu )
if jetAlgo != 'AK7' or not runJetMatch:
  process.patPF2PATSequence.remove( process.ak7GenJetsNoNu )
if jetAlgo != 'IC5' or not runJetMatch:
  process.patPF2PATSequence.remove( process.iterativeCone5GenJetsNoNu )
if not runJetMatch:
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
process.pfIsolatedMuons.isolationCut = muonsSelectIsoPf
process.patMuons.embedTrack = embedLeptonTracks
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
  process.isoValElectronWithCharged.deposits[0].deltaR = 0.3
  process.isoValElectronWithNeutral.deposits[0].deltaR = 0.3
  process.isoValElectronWithPhotons.deposits[0].deltaR = 0.3
process.pfIsolatedElectrons.isolationCut = electronsSelectIsoPf
process.patElectrons.embedTrack = embedLeptonTracks
process.selectedPatElectrons.cut = electronSelect
process.referencePatElectrons.preselection = electronSelectSignal
if runEwk:
  process.load( "ElectroWeakAnalysis.WENu.simpleEleIdSequence_cff" )
  process.patPF2PATSequence.replace( process.patElectrons
                                   , process.simpleEleIdSequence * process.patElectrons
                                   )
  process.patElectrons.electronIDSources.simpleEleId95relIso = cms.InputTag( 'simpleEleId95relIso' )
  process.patElectrons.electronIDSources.simpleEleId90relIso = cms.InputTag( 'simpleEleId90relIso' )
  process.patElectrons.electronIDSources.simpleEleId85relIso = cms.InputTag( 'simpleEleId85relIso' )
  process.patElectrons.electronIDSources.simpleEleId80relIso = cms.InputTag( 'simpleEleId80relIso' )
  process.patElectrons.electronIDSources.simpleEleId70relIso = cms.InputTag( 'simpleEleId70relIso' )
  process.patElectrons.electronIDSources.simpleEleId60relIso = cms.InputTag( 'simpleEleId60relIso' )
  process.patElectrons.electronIDSources.simpleEleId95cIso   = cms.InputTag( 'simpleEleId95cIso' )
  process.patElectrons.electronIDSources.simpleEleId90cIso   = cms.InputTag( 'simpleEleId90cIso' )
  process.patElectrons.electronIDSources.simpleEleId85cIso   = cms.InputTag( 'simpleEleId85cIso' )
  process.patElectrons.electronIDSources.simpleEleId80cIso   = cms.InputTag( 'simpleEleId80cIso' )
  process.patElectrons.electronIDSources.simpleEleId70cIso   = cms.InputTag( 'simpleEleId70cIso' )
  process.patElectrons.electronIDSources.simpleEleId60cIso   = cms.InputTag( 'simpleEleId60cIso' )
if runCiC:
  process.load( "RecoEgamma.ElectronIdentification.cutsInCategoriesElectronIdentificationV06_cfi" )
  process.eidCiCSequence = cms.Sequence(
    process.eidVeryLooseMC
  + process.eidLooseMC
  + process.eidMediumMC
  + process.eidTightMC
  + process.eidSuperTightMC
  + process.eidHyperTight1MC
  + process.eidHyperTight2MC
  + process.eidHyperTight3MC
  + process.eidHyperTight4MC
  )
  process.patPF2PATSequence.replace( process.patElectrons
                                   , process.eidCiCSequence * process.patElectrons
                                   )
  process.patElectrons.electronIDSources.eidVeryLooseMC   = cms.InputTag( 'eidVeryLooseMC' )
  process.patElectrons.electronIDSources.eidLooseMC       = cms.InputTag( 'eidLooseMC' )
  process.patElectrons.electronIDSources.eidMediumMC      = cms.InputTag( 'eidMediumMC' )
  process.patElectrons.electronIDSources.eidTightMC       = cms.InputTag( 'eidTightMC' )
  process.patElectrons.electronIDSources.eidSuperTightMC  = cms.InputTag( 'eidSuperTightMC' )
  process.patElectrons.electronIDSources.eidHyperTight1MC = cms.InputTag( 'eidHyperTight1MC' )
  process.patElectrons.electronIDSources.eidHyperTight2MC = cms.InputTag( 'eidHyperTight2MC' )
  process.patElectrons.electronIDSources.eidHyperTight3MC = cms.InputTag( 'eidHyperTight3MC' )
  process.patElectrons.electronIDSources.eidHyperTight4MC = cms.InputTag( 'eidHyperTight4MC' )

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
process.out.outputCommands.append( 'keep *_kt6PFJets_rho_%s'%( process.name_() ) )
if not runJetMatch:
  process.out.outputCommands.append( 'drop *_selectedPatJets*_genJets_*' )


### TQAF

process.load( "TopQuarkAnalysis.TopEventProducers.sequences.ttSemiLepEvtBuilder_cff" )
process.ttSemiLepEvent.verbosity = 1
from TopQuarkAnalysis.TopEventProducers.sequences.ttSemiLepEvtBuilder_cff import addTtSemiLepHypotheses
addTtSemiLepHypotheses( process
                      , [ "kKinFit"
                        , "kHitFit"
                        ]
                       )
process.hitFitTtSemiLepEventHypothesis.maxNJets           = 6
process.hitFitTtSemiLepEventHypothesis.maxNComb           = 24
process.hitFitTtSemiLepEventHypothesis.jetCorrectionLevel = jecLevels[ -1 ]

if runOnMC:
  process.load( "TopQuarkAnalysis.TopEventProducers.sequences.ttGenEvent_cff" )
  addTtSemiLepHypotheses( process
                        , [ "kGenMatch"
                        ]
                       )
  process.ttSemiLepJetPartonMatch.maxNJets        = 6
  process.ttSemiLepJetPartonMatch.maxNComb        = 24
  process.ttSemiLepHypGenMatch.jetCorrectionLevel = jecLevels[ -1 ]
else:
  from TopQuarkAnalysis.TopEventProducers.sequences.ttSemiLepEvtBuilder_cff import removeTtSemiLepHypGenMatch
  removeTtSemiLepHypGenMatch( process )

process.out.outputCommands.append( 'keep *_*TtSemiLepEventHypothesis*_*_*' )
process.out.outputCommands.append( 'keep *_ttSemiLep*_*_*' )

if runOnMC:
  process.out.outputCommands.append( 'keep *_genEvt*_*_*' )
  process.out.outputCommands.append( 'keep *_initSubset*_*_*' )
  process.out.outputCommands.append( 'keep *_decaySubset*_*_*' )

from PhysicsTools.PatAlgos.tools.helpers import cloneProcessingSnippet
process.makeTtSemiLepEventReference = cloneProcessingSnippet( process
                                                            , process.makeTtSemiLepEvent
                                                            , 'Reference'
                                                            )
from PhysicsTools.PatAlgos.tools.helpers import massSearchReplaceAnyInputTag
massSearchReplaceAnyInputTag( process.makeTtSemiLepEventReference
                            , 'selectedPatMuons'
                            , 'referencePatMuons'
                             )
massSearchReplaceAnyInputTag( process.makeTtSemiLepEventReference
                            , 'selectedPatJets'
                            , 'referencePatJets'
                             )


### Paths

# Cleaning
process.HitFit_Cleaning = cms.Path( process.eventCleaning
                                  )
# Single cleaning steps
process.HitFit_Cleaning_HLT = cms.Path( process.triggerResultsFilter
                                      )
process.HitFit_Cleaning_Vertex = cms.Path( process.goodOfflinePrimaryVertices
                                         )
process.HitFit_Cleaning_HBHE = cms.Path( process.HBHENoiseFilter
                                       )
process.HitFit_Cleaning_Scraping = cms.Path( process.scrapingFilter
                                           )

# PF2PAT
process.HitFit_PF2PAT = cms.Path( process.eventCleaning
                                * process.patPF2PATSequence
                                * process.patPF2PATCounters
                                )
if runOnMC:
  process.HitFit_PF2PAT *= process.makeGenEvt
process.HitFit_PF2PAT *= process.makeTtSemiLepEvent
# Single counters
process.HitFit_PF2PAT_MuonVeto = cms.Path( process.eventCleaning
                                         * process.patPF2PATSequence
                                         * process.countSelectedPatMuons
                                         )
process.HitFit_PF2PAT_ElectronVeto = cms.Path( process.eventCleaning
                                             * process.patPF2PATSequence
                                             * process.countSelectedPatElectrons
                                             )
# process.HitFit_PF2PAT_LeptonVeto = cms.Path( process.eventCleaning
#                                            * process.patPF2PATSequence
#                                            * process.countSelectedPatLeptons
#                                            )
# process.HitFit_PF2PAT_JetVeto = cms.Path( process.eventCleaning
#                                         * process.patPF2PATSequence
#                                         * process.countSelectedPatJets
#                                         )

# PF2PAT with HitFit
process.HitFit_PF2PATHitFit = cms.Path( process.eventCleaning
                                      * process.patPF2PATSequence
                                      * process.patPF2PATCountersHitFit
                                      )
if runOnMC:
  process.HitFit_PF2PATHitFit *= process.makeGenEvt
process.HitFit_PF2PATHitFit *= process.makeTtSemiLepEvent
# Single counters
process.HitFit_PF2PAT_MuonsHitFit = cms.Path( process.eventCleaning
                                            * process.patPF2PATSequence
                                            * process.countSelectedPatMuonsHitFit
                                            )
process.HitFit_PF2PAT_JetsHitFit = cms.Path( process.eventCleaning
                                           * process.patPF2PATSequence
                                           * process.countSelectedPatJetsHitFit
                                           )

# Reference selection
process.HitFit_Reference = cms.Path( process.eventCleaning
                                   * process.patPF2PATSequence
                                   * process.patPF2PATCounters
                                   * process.patReferenceSequence
                                   * process.patReferenceCounters
                                   )
# Single counters
process.HitFit_Reference_Muons = cms.Path( process.eventCleaning
                                         * process.patPF2PATSequence
                                         * process.patPF2PATCounters
                                         * process.patReferenceSequence
                                         * process.countReferencePatMuons
                                         )
# process.HitFit_Reference_Electrons = cms.Path( process.eventCleaning
#                                              * process.patPF2PATSequence
#                                              * process.patPF2PATCounters
#                                              * process.patReferenceSequence
#                                              * process.countReferencePatElectrons
#                                              )
# process.HitFit_Reference_Leptons = cms.Path( process.eventCleaning
#                                            * process.patPF2PATSequence
#                                            * process.patPF2PATCounters
#                                            * process.patReferenceSequence
#                                            * process.countReferencePatLeptons
#                                            )
process.HitFit_Reference_Jets = cms.Path( process.eventCleaning
                                        * process.patPF2PATSequence
                                        * process.patPF2PATCounters
                                        * process.patReferenceSequence
                                        * process.countReferencePatJets
                                        )

# Reference selection with HitFit
process.HitFit_ReferenceHitFit = cms.Path( process.eventCleaning
                                         * process.patPF2PATSequence
                                         * process.patPF2PATCounters
                                         * process.patReferenceSequence
                                         * process.patReferenceCounters
                                         )
if runOnMC:
  process.HitFit_ReferenceHitFit *= process.makeGenEvt
process.HitFit_ReferenceHitFit *= process.makeTtSemiLepEventReference

process.out.SelectEvents.SelectEvents = [ 'HitFit_PF2PATHitFit'
                                        ]
if runTest or runOnMC:
  process.out.SelectEvents.SelectEvents = [ 'HitFit_Cleaning'
                                          ]


### Messages

print
print 'Input:'
print '------'
if inputEvents == -1:
  print 'all events of %s'%( sample )
else:
  print '%i events of %s'%( inputEvents, sample )
print
print 'Output file:'
print '------------'
print outputFile
print
print 'Log file destination:'
print '---------------------'
if runTest:
  print 'hitFitPatSkimPF2PAT.log'
else:
  print '%s/output/hitFitPatSkimPF2PAT_%s.log'%( os.getenv( "CMSSW_BASE" ), sample )
print '================================================================================'
print
