import os
import FWCore.ParameterSet.Config as cms

### Steering

runOnMC   = False
runMatch  = False
runCiC    = True
runEwk    = True
addGenEvt = False

hltProcess       = 'HLT'
triggerSelection = ''

jetAlgo   = 'AK5'
jecLevels = []
#jecLevels = [ 'L1FastJet', 'L2Relative', 'L3Absolute' ]
#if not runOnMC:
  #jecLevels.append( 'L2L3Residual' )

# muon isolation
muonsIsoR = 0.4
# muon top projection isolation
#muonsIsoTP = 0.15 # PF2PAT
muonsIsoTP = 0.2
# muon object selection
#muonSelect = 'isGlobalMuon && pt > 10. && abs(eta) < 2.5' # RefSel (min. for veto)
muonSelect = ''
# muon event selection
muonsCut = 'isGlobalMuon && pt > 5. && abs(eta) < 3.0'
muonsMin = 0

# electron isolation
electronsIsoR = 0.3
# electron top projection isolation
#electronsIsoTP = 0.2 # PF2PAT
electronsIsoTP = 0.2
# electron object selection
#electronSelect = 'et > 15. && abs(eta) < 2.5' # RefSel (min. for veto)
electronSelect = ''
# electron event selection
electronsCut = 'et > 5. && abs(eta) < 3.0'
electronsMin = 0

# x-leptons event selection
leptonsMin = 1

# jet object selection
#jetSelect = 'pt > 30. && abs(eta) < 2.4' # RefSel
jetSelect = ''
# jet event selection
jetsCut = 'pt > 15. && abs(eta) < 3.0'
jetsMin = 3

### Initialization

process = cms.Process( 'PF2PAT' )

runMatch  = runMatch  and runOnMC
addGenEvt = addGenEvt and runOnMC

### Logging

process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool( True )
)
process.Timing = cms.Service( "Timing"
, summaryOnly = cms.untracked.bool( True )
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

process.source = cms.Source( "PoolSource"
, noEventSort        = cms.untracked.bool( True )
, duplicateCheckMode = cms.untracked.string( 'noDuplicateCheck' )
, fileNames          = cms.untracked.vstring()
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 1000 )
)

from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
if runOnMC:
  process.source.fileNames = pickRelValInputFiles( cmsswVersion  = 'CMSSW_4_2_8'
                                                 , globalTag     = 'START42_V12'
                                                 )
else:
  process.source.fileNames = pickRelValInputFiles( cmsswVersion  = 'CMSSW_4_2_8'
                                                 , dataTier      = 'RECO'
                                                 , relVal        = 'Mu'
                                                 , globalTag     = 'GR_R_42_V14_mu2010B'
                                                 #, relVal        = 'Electron'
                                                 #, globalTag     = 'GR_R_42_V14_electron2010B'
                                                 )

### Output

from PhysicsTools.PatAlgos.patEventContent_cff import patEventContentNoCleaning
process.out = cms.OutputModule( "PoolOutputModule"
, fileName       = cms.untracked.string( 'gentPatSkimPF2PAT.root' )
, SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring( 'p' ) )
, outputCommands = cms.untracked.vstring( 'drop *', *patEventContentNoCleaning )
, dropMetaData   = cms.untracked.string( 'ALL' )
)

# Outpath
process.outpath = cms.EndPath(
  process.out
)

### Cleaning

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

# Trigger
process.load( "HLTrigger.HLTfilters.triggerResultsFilter_cfi" )
process.triggerResultsFilter.hltResults        = cms.InputTag( 'TriggerResults::' + hltProcess )
process.triggerResultsFilter.l1tResults        = cms.InputTag( '' )
process.triggerResultsFilter.triggerConditions = [ triggerSelection ]
process.triggerResultsFilter.throw             = False

# Vertices
pvSelection = cms.PSet(
  minNdof = cms.double( 4. )
, maxZ    = cms.double( 24. )
, maxRho  = cms.double( 2. )
)
process.goodOfflinePrimaryVertices = cms.EDFilter(
  "PrimaryVertexObjectFilter"
, filterParams = pvSelection
, filter       = cms.bool( False )
, src          = cms.InputTag( 'offlinePrimaryVertices' )
)
process.goodOfflinePrimaryVertexFilter = cms.EDFilter(
  "PrimaryVertexFilter"
, pvSelection
, pvSrc = cms.InputTag( 'goodOfflinePrimaryVertices' )
)
process.vertexSelection = cms.Sequence(
  process.goodOfflinePrimaryVertices
* process.goodOfflinePrimaryVertexFilter
)

process.eventCleaning = cms.Sequence(
  process.HBHENoiseFilter
+ process.scrapingFilter
)
if triggerSelection != '':
  process.eventCleaning += process.triggerResultsFilter
process.eventCleaning += process.vertexSelection

### PAT

process.load( "PhysicsTools.PatAlgos.patSequences_cff" )

# Misc
from PhysicsTools.PatAlgos.tools.pfTools import usePF2PAT
usePF2PAT( process
         , runOnMC        = runOnMC
         , jetAlgo        = jetAlgo
         , jetCorrections = ( jetAlgo + 'PFchs'
                            , jecLevels
                            )
         )
process.patPF2PATSequence.remove( process.patPFParticles )
process.patPF2PATSequence.remove( process.selectedPatPFParticles )
process.patPF2PATSequence.remove( process.countPatPFParticles )
from PhysicsTools.PatAlgos.tools.coreTools import *
removeSpecificPATObjects( process
                        , names = [ 'Photons', 'Taus' ]
                        )
# The following is not performed (properly) by 'removeSpecificPATObjects()'
process.cleanPatCandidateSummary.candidates.remove( cms.InputTag( 'cleanPatPhotons' ) )
process.cleanPatCandidateSummary.candidates.remove( cms.InputTag( 'cleanPatTaus' ) )
process.patPF2PATSequence.remove( process.countPatTaus )
process.patPF2PATSequence.replace( process.selectedPatCandidateSummary
                                 , process.selectedPatCandidateSummary * ( process.cleanPatMuons
                                                                         + process.cleanPatElectrons
                                                                         + process.cleanPatJets
                                                                         )
                                 )
for m in listModules( process.countPatCandidates ):
  if hasattr( m, 'src' ): m.src = m.src.value().replace( 'selectedPat', 'cleanPat' )
process.countPatLeptons.electronSource = process.countPatLeptons.electronSource.value().replace( 'selectedPat', 'cleanPat' )
process.countPatLeptons.muonSource     = process.countPatLeptons.muonSource.value().replace( 'selectedPat', 'cleanPat' )
process.countPatLeptons.tauSource      = process.countPatLeptons.tauSource.value().replace( 'selectedPat', 'cleanPat' )
if not runOnMC:
  runOnData( process
           , names = [ 'PFAll' ]
           )
if not runMatch:
  process.patMuons.addGenMatch = False
  process.patPF2PATSequence.remove( process.muonMatch )
  process.patElectrons.addGenMatch = False
  process.patPF2PATSequence.remove( process.electronMatch )
  process.patJets.addGenPartonMatch   = False
  process.patJets.embedGenPartonMatch = False
  process.patJets.genPartonMatch      = cms.InputTag( '' )
  process.patPF2PATSequence.remove( process.patJetPartonMatch )
  process.patJets.getJetMCFlavour    = False
  process.patJets.JetPartonMapSource = cms.InputTag( '' )
  process.patPF2PATSequence.remove( process.patJetFlavourId )
process.patJets.addGenJetMatch = False
process.patJets.genJetMatch    = cms.InputTag( '' )
process.patPF2PATSequence.remove( process.patJetGenJetMatch )
process.patPF2PATSequence.remove( process.ak5GenJetsNoNu )
process.patPF2PATSequence.remove( process.ak7GenJetsNoNu )
process.patPF2PATSequence.remove( process.iterativeCone5GenJetsNoNu )
process.patPF2PATSequence.remove( process.genParticlesForJetsNoNu )
# The following need to be fixed _after_ the (potential) calls to 'removeSpecificPATObjects()' and 'runOnData()'
process.patJetCorrFactors.payload = jetAlgo + 'PFchs'
process.patJetCorrFactors.levels  = jecLevels
process.out.outputCommands += [ 'drop recoGenJets_*_*_*'
                              , 'drop recoBaseTagInfosOwned_*_*_*'
                              , 'drop CaloTowers_*_*_*'
                              , 'keep edmTriggerResults_*_*_*'
                              , 'drop edmTriggerResults_*_*_*RECO*'
                              , 'drop edmTriggerResults_*_*_*NONE*'
                              , 'keep *_hltTriggerSummaryAOD_*_*'
                              , 'keep *_offlineBeamSpot_*_*'
                              , 'keep *_goodOfflinePrimaryVertices_*_*'
                              # for conversion rejection
                              , 'keep recoTracks_generalTracks_*_*'
                              , 'keep recoGsfTracks_electronGsfTracks_*_*'
                              ]
if runOnMC:
  process.out.outputCommands += [ 'keep *_addPileupInfo_*_*'
                                ]
  if not runMatch:
    process.out.outputCommands += [ 'keep recoGenParticles_*_*_*'
                                  ]
if addGenEvt:
  process.out.outputCommands += [ 'keep *_genParticles_*_*'
                                , 'keep *_genEvt_*_*'
                                ]
process.out.outputCommands += [ 'keep double_kt6PFJets_*_' + process.name_() ]

# Muons
process.isoValMuonWithCharged.deposits[0].deltaR = muonsIsoR
process.isoValMuonWithNeutral.deposits[0].deltaR = muonsIsoR
process.isoValMuonWithPhotons.deposits[0].deltaR = muonsIsoR
process.pfIsolatedMuons.combinedIsolationCut = muonsIsoTP
process.patMuons.embedTrack = True
process.selectedPatMuons.cut = muonSelect
process.cleanPatMuons.src           = cms.InputTag( 'patMuons' )
process.cleanPatMuons.preselection  = muonsCut
process.cleanPatMuons.checkOverlaps = cms.PSet()
process.countPatMuons.minNumber = muonsMin

# Electrons
process.isoValElectronWithCharged.deposits[0].deltaR = electronsIsoR
process.isoValElectronWithNeutral.deposits[0].deltaR = electronsIsoR
process.isoValElectronWithPhotons.deposits[0].deltaR = electronsIsoR
process.pfIsolatedElectrons.combinedIsolationCut = electronsIsoTP
process.patElectrons.embedTrack = True
process.selectedPatElectrons.cut = electronSelect
process.cleanPatElectrons.src           = cms.InputTag( 'patElectrons' )
process.cleanPatElectrons.preselection  = electronsCut
process.cleanPatElectrons.checkOverlaps = cms.PSet()
process.countPatElectrons.minNumber = electronsMin
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
process.countPatLeptons.minNumber = leptonsMin

# Jets
if len( jecLevels ) is 0:
  process.patJets.addJetCorrFactors = False
  print 'WARNING: No JECs are stored or applied!'
elif 'L1FastJet' in jecLevels:
  process.pfPileUp.checkClosestZVertex = False
  process.pfJets.doAreaFastjet = True
  process.pfJets.doRhoFastjet  = False
process.load( "RecoJets.Configuration.RecoPFJets_cff" )
process.kt6PFJets.src          = cms.InputTag( 'pfNoElectron' )
process.kt6PFJets.voronoiRfact = cms.double( -0.9 ) # to ensure not to use the Voronoi tessalation for the moment (s. https://hypernews.cern.ch/HyperNews/CMS/get/JetMET/1215.html)
process.patPF2PATSequence.replace( process.patJetCorrFactors
                                 , process.kt6PFJets * process.patJetCorrFactors
                                 )
process.patJets.embedCaloTowers   = False
process.patJets.embedPFCandidates = False
process.selectedPatJets.cut = jetSelect
process.cleanPatJets.src           = cms.InputTag( 'patJets' )
process.cleanPatJets.preselection  = jetsCut
process.cleanPatJets.checkOverlaps = cms.PSet()
process.countPatJets.minNumber = jetsMin

### TQAF

if addGenEvt:
  process.load( "TopQuarkAnalysis.TopEventProducers.sequences.ttGenEvent_cff" )

### Path
process.p = cms.Path(
  process.eventCleaning
* process.patPF2PATSequence
)
if addGenEvt:
  process.p *= process.makeGenEvt