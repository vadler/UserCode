import sys
import os
import FWCore.ParameterSet.Config as cms


### Steering

# Misc
runCrab    = True
runTest    = True
reportTime = False

# MC Input (only for 'runCrab' = True)
mc = 'Summer12_MadGraph'
#mc = 'Summer12_MCatNLO'

# Trigger
hltProcess                = 'HLT'
triggerSelectionMuons     = 'HLT_IsoMu17_eta2p1_TriCentralPFNoPUJet50_40_30_v*'
#triggerSelectionElectrons = 'HLT_Ele25_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_TriCentralPFNoPUJet50_40_30_v*' # tight
triggerSelectionElectrons = 'HLT_Ele25_CaloIdVT_CaloIsoVL_TrkIdVL_TrkIsoT_TriCentralPFNoPUJet50_40_30_v*' # loose

# Vertices
pvCollection = 'goodOfflinePrimaryVertices' #'offlinePrimaryVertices' or 'goodOfflinePrimaryVertices'
                                            # recommended: 'goodOfflinePrimaryVertices' (s. https://hypernews.cern.ch/HyperNews/CMS/get/top-selection/38/1/1/1/2/1/1/2/1/3/1.html)

# Muons
# switch for muon isolation cone, PF2PAT default: 0.4 (False)
muonsIsoR03           = False # isolation cone of 0.3, if 'True'
# muon top projection isolation selection, PF2PAT default: 0.15
muonsSelectIsoPf      = 0.5
# muon top projection object selection, PF2PAT default: 'pt > 5.'
muonSelectPf          = 'pt > 5.'
# muon object selection
muonUsePV             = True
muonSelectBase        = '(isGlobalMuon || isTrackerMuon) && pt > 10. && (chargedHadronIso+max(0.,neutralHadronIso+photonIso-0.5*puChargedHadronIso))/pt < 0.2 && abs(eta) < 3.0'
muonSelectHitFit      = muonSelectBase + ' && abs(eta) < 2.4'
muonSelect            = muonSelectBase + ' && abs(eta) < 2.5'
muonSelectSignal      = 'isGlobalMuon && pt > 26. && abs(eta) < 2.1 && globalTrack.normalizedChi2 < 10. && track.hitPattern.trackerLayersWithMeasurement > 5 && globalTrack.hitPattern.numberOfValidMuonHits > 0 && abs(dB) < 0.2 && innerTrack.hitPattern.numberOfValidPixelHits > 0 && numberOfMatches > 1 && (chargedHadronIso+max(0.,neutralHadronIso+photonIso-0.5*puChargedHadronIso))/pt < 0.12'
muonSelectSignalJetDR = 0.3
# counters for muon channel
selectedMuonsMin = 1
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
electronUsePV        = False
electronSelectBase   = 'et > 20. && (chargedHadronIso+max(0.,neutralHadronIso+photonIso-1.*userIsolation("User1Iso")))/pt < 0.15 && abs(eta) < 3.0 && (1. >= electronID("mvaTrigV0") && electronID("mvaTrigV0") >= 0.)'
electronSelectHitFit = electronSelectBase + ' && abs(eta) < 2.5 && passConversionVeto'
electronSelect       = electronSelectBase + ' && abs(eta) < 2.5'
electronSelectSignal = 'et > 30. && abs(eta) < 2.5 && !(1.4442 < abs(superCluster.eta) && abs(superCluster.eta) < 1.5660) && abs(dB) < 0.02 && electronID("mvaTrigV0") > 0.5 && (chargedHadronIso+max(0.,neutralHadronIso+photonIso-1.0*userIsolation("User1Iso")))/pt < 0.1 && passConversionVeto && gsfTrack.trackerExpectedHitsInner.numberOfHits <= 0'
electronSelectSignalJetDR = 0.3
# counters for electron channel
selectedElectronsMin = 1
selectedElectronsMax = 1
referenceElectronsMin = 1
referenceElectronsMax = 1

# X-Leptons
# counters
selectedLeptonsMin = 1
selectedLeptonsMax = 1

# Jets
# algo & JECs
jetAlgo   = 'AK5'
jecLevels = [ 'L1FastJet'
            , 'L2Relative'
            , 'L3Absolute'
            ]
# jet object selection
jetSelectBase   = 'pt > 30. && numberOfDaughters > 1 && chargedEmEnergyFraction < 0.99 && neutralHadronEnergyFraction < 0.99 && neutralEmEnergyFraction < 0.99 && (chargedHadronEnergyFraction > 0. || abs(eta) >= 2.4) && (chargedMultiplicity > 0 || abs(eta) >= 2.4) && abs(eta) < 3.0'
jetSelectHitFit = jetSelectBase + ' && abs(eta) < 2.5'
jetSelect       = jetSelectBase + ' && abs(eta) < 2.5'
jetSelectSignal = ''
# counters
selectedJetsMin = 4
selectedJetsMax = 999999
referenceJetsMin = 4
referenceJetsMax = 999999

# MET
useType0Corr = True
useTypeICorr = True


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

process.load( "Configuration.Geometry.GeometryIdeal_cff" )
process.load( "Configuration.StandardSequences.MagneticField_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
# from Configuration.AlCa.autoCond import autoCond
# process.GlobalTag.globaltag = autoCond[ 'startup' ]
process.GlobalTag.globaltag = 'START53_V21::All'
if mc == 'Summer12_MadGraph' or mc == 'Summer12_MCatNLO':
  process.GlobalTag.globaltag = 'START53_V21::All'


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
  from TopQuarkAnalysis.TopMassSemiLeptonic.input_cff import files
  process.source.fileNames = files[ mc ]


### Output

process.out = cms.OutputModule( "PoolOutputModule"
, fileName       = cms.untracked.string( 'skimHitFit.root' )
, SelectEvents   = cms.untracked.PSet(
    SelectEvents = cms.vstring(
                              )
  )
, outputCommands = cms.untracked.vstring() # screwed up by PAT tools later on anyway :-(
, dropMetaData   = cms.untracked.string( 'ALL' )
)

outputFile = '%s/output/skimHitFit.root'%( os.getenv( "CMSSW_BASE" ) )
if runTest:
  outputFile = outputFile.replace( 'root', 'test.root' )
if not runCrab:
  outputFile = outputFile.replace( 'root', 'local.root' )
  process.out.fileName = outputFile
logFile = outputFile.replace( 'root', 'log' )


### Trigger

# Trigger
if triggerSelectionMuons == '' or triggerSelectionMuons == '*':
  triggerSelectionMuons = 'HLT_*'
if triggerSelectionElectrons == '' or triggerSelectionElectrons == '*':
  triggerSelectionElectrons = 'HLT_*'
from HLTrigger.HLTfilters.triggerResultsFilter_cfi import triggerResultsFilter
process.triggerResultsFilterMuons = triggerResultsFilter.clone( hltResults        = cms.InputTag( 'TriggerResults::%s'%( hltProcess ) )
                                                              , l1tResults        = cms.InputTag( '' )
                                                              , triggerConditions = [ triggerSelectionMuons ]
                                                              , throw             = False
                                                              )
process.triggerResultsFilterElectrons = process.triggerResultsFilterMuons.clone( triggerConditions = [ triggerSelectionElectrons ]
                                                                               )


### Cleaning

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

process.eventCleaning = cms.Sequence( process.goodOfflinePrimaryVertices
                                    )


### PAT

process.load( "PhysicsTools.PatAlgos.patSequences_cff" )

# Misc
from PhysicsTools.PatAlgos.tools.coreTools import *
removeSpecificPATObjects( process
                        , names = [ 'Photons', 'Taus' ]
                        )

# PF2PAT
from PhysicsTools.PatAlgos.tools.pfTools import usePF2PAT
usePF2PAT( process
         , runOnMC             = True
         , jetAlgo             = jetAlgo
         , jetCorrections      = ( '%sPFchs'%( jetAlgo )
                                 , jecLevels
                                 )
         , typeIMetCorrections = useTypeICorr
         , pvCollection        = cms.InputTag( pvCollection )
         )
# still need to fix event content afterwards :-(
process.out.outputCommands.append( 'keep *_addPileupInfo_*_*' )
process.out.outputCommands.append( 'keep edmTriggerResults_*_*_*' )
process.out.outputCommands.append( 'keep *_offlinePrimaryVertices_*_*' )
process.out.outputCommands.append( 'keep *_%s_*_*'%( pvCollection ) )

process.patPF2PATSequence.remove( process.ak7GenJetsNoNu )
process.patPF2PATSequence.remove( process.iterativeCone5GenJetsNoNu )

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

# MC match object selections
process.selectedPatMuonsMCMatch     = process.selectedPatMuons.clone()
process.selectedPatElectronsMCMatch = process.selectedPatElectrons.clone()
process.selectedPatJetsMCMatch      = process.selectedPatJets.clone()
process.patPF2PATSequence.replace( process.selectedPatMuons
                                 , process.selectedPatMuonsMCMatch * process.selectedPatMuons
                                 )
process.patPF2PATSequence.replace( process.selectedPatElectrons
                                 , process.selectedPatElectronsMCMatch * process.selectedPatElectrons
                                 )
process.patPF2PATSequence.replace( process.selectedPatJets
                                 , process.selectedPatJetsMCMatch * process.selectedPatJets
                                 )

# HitFit object selections
process.selectedPatMuonsHitFit     = process.selectedPatMuons.clone()
process.selectedPatElectronsHitFit = process.selectedPatElectrons.clone()
process.selectedPatJetsHitFit      = process.selectedPatJets.clone()
process.patHitFitSequence = cms.Sequence( process.selectedPatMuonsHitFit
                                        + process.selectedPatElectronsHitFit
                                        + process.selectedPatJetsHitFit
                                        )

# Final object selections
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
process.load( "TopQuarkAnalysis.TopTools.muonSelectorVertex_cfi" )
process.muonSelectorVertex.vertexSource = pvCollection
process.muonSelectorVertex.maxDZ        = 1.
process.patReferenceSequence = cms.Sequence( process.muonSelectorVertex
                                           * process.referencePatMuons
                                           * process.referencePatElectrons
                                           * process.referencePatJets
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
process.countReferencePatJets      = process.countPatJets.clone( src = cms.InputTag( 'referencePatJets' )
                                                               , minNumber = referenceJetsMin
                                                               , maxNumber = referenceJetsMax
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
process.pfIsolatedMuons.doDeltaBetaCorrection = True
process.pfIsolatedMuons.deltaBetaFactor       = -0.5
process.pfIsolatedMuons.isolationCut = muonsSelectIsoPf
process.patMuons.embedTrack = True
process.patMuons.usePV      = muonUsePV
if muonsIsoR03:
  process.patMuons.isolationValues.pfNeutralHadrons   = cms.InputTag( 'muPFIsoValueNeutral03' )
  process.patMuons.isolationValues.pfChargedAll       = cms.InputTag( 'muPFIsoValueChargedAll03' )
  process.patMuons.isolationValues.pfChargedHadrons   = cms.InputTag( 'muPFIsoValueCharged03' )
  process.patMuons.isolationValues.pfPUChargedHadrons = cms.InputTag( 'muPFIsoValuePU03' )
  process.patMuons.isolationValues.pfPhotons          = cms.InputTag( 'muPFIsoValueGamma03' )
process.selectedPatMuonsMCMatch.cut     = muonSelectBase
process.selectedPatMuons.cut            = muonSelect
process.selectedPatMuonsHitFit.cut      = muonSelectHitFit
process.referencePatMuons.src           = 'muonSelectorVertex'
process.referencePatMuons.preselection  = muonSelectSignal
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

if not runTest:
  process.out.outputCommands.append( 'drop *_selectedPatMuons_*_*' )
else:
  process.out.outputCommands.append( 'keep *_muonSelectorVertex_*_*' )


# Electrons
process.load( 'EGamma.EGammaAnalysisTools.electronIsolatorFromEffectiveArea_cfi' )
process.elPFIsoValueEA04 = process.elPFIsoValueEA03.clone( EffectiveAreaType = 'kEleGammaAndNeutralHadronIso04'
)
process.pfIdentifiedElectrons = cms.EDFilter(
  "ElectronIDPFCandidateSelector"
, recoGsfElectrons = cms.InputTag( 'gsfElectrons' )
, electronIdMap    = cms.InputTag( 'mvaTrigV0' )
, electronIdCut    = cms.double( 0.0 )
, src              = cms.InputTag( 'pfElectronsFromVertex' )
)
process.pfSelectedElectrons.src = 'pfIdentifiedElectrons'
process.pfSelectedElectrons.cut = electronSelectPf
process.pfIsolatedElectrons.deltaBetaIsolationValueMap = cms.InputTag( 'elPFIsoValueEA04' ) # new Effective Area edm::ValueMap
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
  process.pfElectrons.deltaBetaIsolationValueMap = cms.InputTag( 'elPFIsoValueEA03' ) # new Effective Area edm::ValueMap
process.pfIsolatedElectrons.doDeltaBetaCorrection = True # EA now, in fact
process.pfIsolatedElectrons.deltaBetaFactor       = -1.
process.pfIsolatedElectrons.isolationCut          = electronsSelectIsoPf
process.patElectrons.embedTrack = True
process.patElectrons.usePV      = electronUsePV
#process.load( "RecoEgamma.ElectronIdentification.cutsInCategoriesElectronIdentificationV06_cfi" )
#process.eidCiCSequence = cms.Sequence(
  #process.eidVeryLooseMC
#+ process.eidLooseMC
#+ process.eidMediumMC
#+ process.eidTightMC
#+ process.eidSuperTightMC
#+ process.eidHyperTight1MC
#+ process.eidHyperTight2MC
#+ process.eidHyperTight3MC
#+ process.eidHyperTight4MC
#)
process.load('EGamma.EGammaAnalysisTools.electronIdMVAProducer_cfi')
process.eidMVASequence = cms.Sequence( process.mvaTrigV0
                                     + process.mvaNonTrigV0
                                     )
process.patPF2PATSequence.replace( process.pfSelectedElectrons
                                 #, process.eidCiCSequence * process.patElectrons
                                 , process.eidMVASequence + process.pfIdentifiedElectrons + process.pfSelectedElectrons + process.elPFIsoValueEA03 + process.elPFIsoValueEA04
                                 )
#process.patElectrons.electronIDSources.eidVeryLooseMC   = cms.InputTag( 'eidVeryLooseMC' )
#process.patElectrons.electronIDSources.eidLooseMC       = cms.InputTag( 'eidLooseMC' )
#process.patElectrons.electronIDSources.eidMediumMC      = cms.InputTag( 'eidMediumMC' )
#process.patElectrons.electronIDSources.eidTightMC       = cms.InputTag( 'eidTightMC' )
#process.patElectrons.electronIDSources.eidSuperTightMC  = cms.InputTag( 'eidSuperTightMC' )
#process.patElectrons.electronIDSources.eidHyperTight1MC = cms.InputTag( 'eidHyperTight1MC' )
#process.patElectrons.electronIDSources.eidHyperTight2MC = cms.InputTag( 'eidHyperTight2MC' )
#process.patElectrons.electronIDSources.eidHyperTight3MC = cms.InputTag( 'eidHyperTight3MC' )
#process.patElectrons.electronIDSources.eidHyperTight4MC = cms.InputTag( 'eidHyperTight4MC' )
process.patElectrons.electronIDSources.mvaTrigV0    = cms.InputTag("mvaTrigV0")
process.patElectrons.electronIDSources.mvaNonTrigV0 = cms.InputTag("mvaNonTrigV0")
process.patElectrons.isolationValues.user = cms.VInputTag( cms.InputTag( 'elPFIsoValueEA04' )
                                                                       )
if electronsIsoR03:
  process.patElectrons.isolationValues.pfNeutralHadrons   = cms.InputTag( 'elPFIsoValueNeutral03PFId' )
  process.patElectrons.isolationValues.pfChargedAll       = cms.InputTag( 'elPFIsoValueChargedAll03PFId' )
  process.patElectrons.isolationValues.pfChargedHadrons   = cms.InputTag( 'elPFIsoValueCharged03PFId' )
  process.patElectrons.isolationValues.pfPUChargedHadrons = cms.InputTag( 'elPFIsoValuePU03PFId' )
  process.patElectrons.isolationValues.pfPhotons          = cms.InputTag( 'elPFIsoValueGamma03PFId' )
  process.patElectrons.isolationValues.user               = cms.VInputTag( cms.InputTag( 'elPFIsoValueEA03' )
                                                                         )

process.selectedPatElectronsMCMatch.cut     = electronSelectBase
process.selectedPatElectrons.cut            = electronSelect
process.selectedPatElectronsHitFit.cut      = electronSelectHitFit
process.referencePatElectrons.preselection  = electronSelectSignal
process.referencePatElectrons.checkOverlaps = cms.PSet(
  jets = cms.PSet(
    src                 = cms.InputTag( 'selectedPatJets' )
  , algorithm           = cms.string( 'byDeltaR' )
  , preselection        = cms.string( '' )
  , deltaR              = cms.double( electronSelectSignalJetDR )
  , checkRecoComponents = cms.bool( False )
  , pairCut             = cms.string( '' )
  , requireNoOverlaps   = cms.bool( True)
  )
)

if not runTest:
  process.out.outputCommands.append( 'drop *_selectedPatElectrons_*_*' )

# X-leptons

# Jets and MET
if 'L1FastJet' in jecLevels:
  process.pfPileUp.checkClosestZVertex = False
process.patJetGenJetMatch.resolveByMatchQuality = True
process.patJets.embedPFCandidates = False
process.selectedPatJetsMCMatch.cut    = jetSelectBase
process.selectedPatJets.cut           = jetSelect
process.selectedPatJetsHitFit.cut     = jetSelectHitFit
process.referencePatJets.preselection = jetSelectSignal
if runTest:
  process.out.outputCommands.append( 'keep *_ak5GenJetsNoNu_*_*' )
  #process.out.outputCommands.append( 'keep *_patJetGenJetMatch_*_*' )

process.patMETsUncorr = process.patMETs.clone( metSource = 'pfMET' )
process.patPF2PATSequence.replace( process.patMETs
                                 , process.patMETs * process.patMETsUncorr
                                 )
if useType0Corr:
  process.load("PhysicsTools.PatUtils.patPFMETCorrections_cff")
  #process.selectedVerticesForPFMEtCorrType0.src           = cms.InputTag( pvCollection )
  #process.particleFlowDisplacedVertex.mainVertexLabel     = cms.InputTag( pvCollection )
  #process.trackToVertexAssociation.VertexCollection       = cms.InputTag( pvCollection )
  #process.pfCandidateToVertexAssociation.VertexCollection = cms.InputTag( pvCollection )
  process.producePatPFMETCorrections.replace( process.pfCandMETcorr
                                            , process.type0PFMEtCorrection * process.patPFMETtype0Corr * process.pfCandMETcorr
                                            )
  process.patType1CorrectedPFMet.srcType1Corrections.append( cms.InputTag( 'patPFMETtype0Corr' ) )

if not runTest:
  process.out.outputCommands.append( 'drop *_selectedPatJets_*_*' )
process.out.outputCommands.append( 'keep *_selectedPatJets_genJets_*' ) # for referencePatJets
process.out.outputCommands.append( 'drop *_selectedPatJets*_caloTowers_*' )
process.out.outputCommands.append( 'drop *_selectedPatJets*_tagInfos_*' )
process.out.outputCommands.append( 'drop *_selectedPatJets*_pfCandidates_*' )
process.out.outputCommands.append( 'drop *_kt6PFJets_*_*' )


### TQAF

#process.load( "TopQuarkAnalysis.TopEventProducers.sequences.ttSemiLepEvtBuilder_cff" )
#if not runCrab:
  #process.ttSemiLepEvent.verbosity = 1
process.load( "TopQuarkAnalysis.TopEventProducers.sequences.ttGenEvent_cff" )
#process.ttSemiLepJetPartonMatch.maxNJets = 6
#process.ttSemiLepJetPartonMatch.maxNComb = 1 # no knobs to turn to get several solutions
#if runTest and not runCrab:
  #process.ttSemiLepJetPartonMatch.verbosity = 1
#process.ttSemiLepHypGenMatch.jetCorrectionLevel = jecLevels[ -1 ]

process.out.outputCommands.append( 'keep *_genEvt*_*_*' )
process.out.outputCommands.append( 'keep *_initSubset*_*_*' )
process.out.outputCommands.append( 'keep *_decaySubset*_*_*' )


### Paths

# Cleaning + PF2PAT
process.pf2PatSequence = cms.Sequence( process.eventCleaning
                                     * process.patPF2PATSequence
                                     * process.makeGenEvt
                                     )
process.pf2PatPathMuons = cms.Path( process.triggerResultsFilterMuons
                                  * process.pf2PatSequence
                                  * process.countSelectedPatMuons
                                  * process.countSelectedPatLeptons
                                  * process.countSelectedPatJets
                                  * process.patHitFitSequence
                                  )
process.pf2PatPathElectrons = cms.Path( process.triggerResultsFilterElectrons
                                      * process.pf2PatSequence
                                      * process.countSelectedPatElectrons
                                      * process.countSelectedPatLeptons
                                      * process.countSelectedPatJets
                                      * process.patHitFitSequence
                                      )

# Reference selections
process.referencePathMuons = cms.Path( process.triggerResultsFilterMuons
                                     * process.pf2PatSequence
                                     * process.countSelectedPatMuons
                                     * process.countSelectedPatLeptons
                                     * process.patReferenceSequence
                                     * process.countReferencePatMuons
                                     * process.countReferencePatJets
                                     )
process.referencePathElectrons = cms.Path( process.triggerResultsFilterElectrons
                                         * process.pf2PatSequence
                                         * process.countSelectedPatElectrons
                                         * process.countSelectedPatLeptons
                                         * process.patReferenceSequence
                                         * process.countReferencePatElectrons
                                         * process.countReferencePatJets
                                         )

process.out.SelectEvents.SelectEvents = [ 'pf2PatPathMuons'
                                        , 'pf2PatPathElectrons'
                                        ]

# Outpath
process.outPath = cms.EndPath(
  process.out
)

process.schedule = cms.Schedule(
  process.pf2PatPathMuons
, process.referencePathMuons
, process.pf2PatPathElectrons
, process.referencePathElectrons
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
