import os
import FWCore.ParameterSet.Config as cms

### Steering

gc = False

runOnMC     = True
runOnPrompt = True

runMatch  = True
runCiC    = True
runEwk    = True
addGenEvt = True
createNTuples        = False
writePdfWeights      = False
writeWDecay          = False	# this should only be set True for *broken* W datasets
writeNonIsoMuons     = True
writeNonIsoElectrons = True

maxEvents = 100

hltProcess       = 'HLT'
triggerSelection = ''

jetAlgo   = 'AK5'
jecLevels = []
#jecLevels = [ 'L1FastJet', 'L2Relative', 'L3Absolute' ]
#if not runOnMC:
  #jecLevels.append( 'L2L3Residual' )

# vertex collection to use
# 'offlinePrimaryVertices' or 'goodOfflinePrimaryVertices'
pvCollection = 'goodOfflinePrimaryVertices' # recommended: 'goodOfflinePrimaryVertices' (s. https://hypernews.cern.ch/HyperNews/CMS/get/top-selection/38/1/1/1/2/1/1/2/1/3/1.html)

# jet collection to use
# 'pfJets' or 'pfNoTau'
pfJetCollection = 'pfJets'

# muon top projection object selection
pfMuonSelect = 'pt > 5.' # PF2PAT: 'pt > 5.'
# muon isolation cone
usePfMuonIsoConeR03 = True
# muon top projection isolation
pfMuonIso = 0.5 # PF2PAT: 0.15
postfixNonIsoMu = 'NonIsoMu'
# muon object selection
#muonSelect = 'isGlobalMuon && pt > 10. && abs(eta) < 2.5' # RefSel (min. for veto)
muonSelect = ''
# muon event selection
#muonCut = 'isGlobalMuon && pt > 5. && abs(eta) < 3.0'
#muonCut = 'isGlobalMuon && pt > 5. && abs(eta) < 2.5 && globalTrack().hitPattern().numberOfValidMuonHits() > 0 && numberOfMatches() > 1 && innerTrack().numberOfValidHits()> 10 && innerTrack().hitPattern().numberOfValidPixelHits()>0'
muonCut = 'isGlobalMuon && pt > 10. && abs(eta) < 2.5 && globalTrack().hitPattern().numberOfValidMuonHits()> 0 && numberOfMatchedStations() > 1 && innerTrack().hitPattern().numberOfValidPixelHits()>0 && track().hitPattern().trackerLayersWithMeasurement() > 8'
muonsMin = 0

# electron top projection object selection
pfElectronSelect = 'pt > 5. && gsfTrackRef.isNonnull && gsfTrackRef.trackerExpectedHitsInner.numberOfLostHits < 2' # PF2PAT: 'pt > 5. && gsfTrackRef.isNonnull && gsfTrackRef.trackerExpectedHitsInner.numberOfLostHits < 2'
# electron isolation cone
usePfElectronIsoConeR03 = True
# electron top projection isolation
pfElectronIso = 0.5 # PF2PAT: 0.2
postfixNonIsoE = 'NonIsoE'
# electron object selection
#electronSelect = 'et > 15. && abs(eta) < 2.5' # RefSel (min. for veto)
electronSelect = ''
# electron event selection
electronCut  = 'et > 5. && abs(eta) < 3.0'
electronsMin = 0

# x-leptons event selection
leptonsMin = 1

# jet object selection
#jetSelect = 'pt > 30. && abs(eta) < 2.4' # RefSel
jetSelect = ''
# jet event selection
jetCut  = 'pt > 15. && abs(eta) < 3.0'
jetsMin = 3

# Flat NTuple production
processing_mode = 0
electron = "selectedPatElectrons"
muon = "selectedPatMuons"
jet = "selectedPatJets"
nonisomuon = "selectedPatMuons"+postfixNonIsoMu
nomujet = "selectedPatJets"+postfixNonIsoMu
nonisoelectron = "selectedPatElectrons"+postfixNonIsoE
noejet = "selectedPatJets"+postfixNonIsoE
jetCorService = ""
jetID = ""
met = "patMETs"
pfmet = ""
pfjet = ""
pfjetCorService = ""
pfjetID = ""
genjet = ""
tcmet = ""
pfmetTypeI = ""
pfmetTypeII = ""

### Initialization

process = cms.Process( 'PF2PAT' )

### Conditions

process.load( "Configuration.StandardSequences.Geometry_cff" )
process.load( "Configuration.StandardSequences.MagneticField_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
if runOnMC:
  process.GlobalTag.globaltag = 'START52_V7::All'
else:
  process.GlobalTag.globaltag = 'GR_R_52_V7::All'

if gc:
	runOnMC   = eval('@MC@')
	pfJetCollection = '@PFJETS@'
	usePfMuonIsoConeR03 = eval('@MUTPCONE03@') # FIXME in input file
	pfMuonIso = eval('@MUTPISO@')
	muonsMin = eval('@MINNMU@')
	usePfElectronIsoConeR03 = eval('@ETPCONE03@')
	pfElectronsIso = eval('@ETPISO@') # FIXME in input file
	electronsMin = eval('@MINNE@')
	leptonsMin = eval('@MINNLEP@')
	jetsMin = eval('@MINNJETS@')
        writeNonIsoMuons   = eval('@WRITENONISOMU@')
        writeNonIsoElectrons   = eval('@WRITENONISOE@')
	process.GlobalTag.globaltag = '@GLOBALTAG@'
	# if not set in gc config, take default value
	if '@WRITEWDECAY@'.lower() == 'true' or '@WRITEWDECAY@'.lower() == 'false':
		writeWDecay = eval('@WRITEWDECAY@')
	else:
		writeWDecay = False
	# if not set in gc config, take default value
	if '@PDFWEIGHTS@'.lower() == 'true' or '@PDFWEIGHTS@'.lower() == 'false':
		writePdfWeights = eval('@PDFWEIGHTS@')
	else:
		writePdfWeights = False

runOnPrompt     = runOnPrompt     and not runOnMC
runMatch        = runMatch        and runOnMC
addGenEvt       = addGenEvt       and runOnMC
writePdfWeights = writePdfWeights and runOnMC

# muon propagator requirements
process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi")
process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAlong_cfi")
process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorOpposite_cfi")
process.load("RecoMuon.DetLayers.muonDetLayerGeometry_cfi")

### Logging

process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.MessageLogger.cerr.FwkReport.reportEvery = 1
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool( True )
)
process.Timing = cms.Service( "Timing"
, summaryOnly = cms.untracked.bool( True )
)

### Input

from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
inputFiles = cms.untracked.vstring()
if runOnMC:
  inputFiles = pickRelValInputFiles( cmsswVersion  = 'CMSSW_5_2_3'
                                   , globalTag     = 'START52_V5'
                                   , maxVersions   = 1
                                   )
elif runOnPrompt:
  inputFiles = [ '/store/data/Run2012A/MuHad/RECO/PromptReco-v1/000/191/226/EEFA1B9F-E687-E111-9C6D-BCAEC5364C62.root'
               , '/store/data/Run2012A/MuHad/RECO/PromptReco-v1/000/191/226/EC6F55F7-E187-E111-9C56-0030486780EC.root'
               , '/store/data/Run2012A/MuHad/RECO/PromptReco-v1/000/191/226/D6979235-D787-E111-8E2F-001D09F2527B.root'
               , '/store/data/Run2012A/MuHad/RECO/PromptReco-v1/000/191/226/CA288C4D-D287-E111-98F5-0025901D5DB2.root'
               , '/store/data/Run2012A/MuHad/RECO/PromptReco-v1/000/191/226/BAEDD978-CC87-E111-98FC-BCAEC53296F4.root'
               , '/store/data/Run2012A/MuHad/RECO/PromptReco-v1/000/191/226/9A94FDB6-DE87-E111-B1E7-BCAEC518FF80.root'
               , '/store/data/Run2012A/MuHad/RECO/PromptReco-v1/000/191/226/98C29851-D287-E111-AB01-5404A6388699.root'
               , '/store/data/Run2012A/MuHad/RECO/PromptReco-v1/000/191/226/98234FE6-DF87-E111-8EE9-E0CB4E4408E7.root'
               , '/store/data/Run2012A/MuHad/RECO/PromptReco-v1/000/191/226/7C5F35B9-DE87-E111-B35B-5404A63886EE.root'
               , '/store/data/Run2012A/MuHad/RECO/PromptReco-v1/000/191/226/70EEB83A-D987-E111-8EFE-0025B3203898.root'
               , '/store/data/Run2012A/MuHad/RECO/PromptReco-v1/000/191/226/38E568D8-E087-E111-ACE0-BCAEC518FF50.root'
               , '/store/data/Run2012A/MuHad/RECO/PromptReco-v1/000/191/226/32A98280-E387-E111-AA76-001D09F2AD84.root'
               , '/store/data/Run2012A/MuHad/RECO/PromptReco-v1/000/191/226/2CAE5610-D687-E111-B6E6-BCAEC53296FB.root'
               , '/store/data/Run2012A/MuHad/RECO/PromptReco-v1/000/191/226/28B730D4-D387-E111-AFD7-E0CB4E4408E3.root'
               , '/store/data/Run2012A/MuHad/RECO/PromptReco-v1/000/191/226/14610E71-DC87-E111-A90B-BCAEC518FF40.root'
               ]
else:
  inputFiles = pickRelValInputFiles( cmsswVersion  = 'CMSSW_5_2_2'
                                   , dataTier      = 'RECO'
                                   , relVal        = 'SingleMu'
                                   , globalTag     = 'GR_R_52_V4_RelVal_mu2011B'
                                   #, relVal        = 'Electron'
                                   #, globalTag     = 'GR_R_52_V4_RelVal_electron2011B'
                                   )

process.source = cms.Source( "PoolSource"
, noEventSort        = cms.untracked.bool( True )
, duplicateCheckMode = cms.untracked.string( 'noDuplicateCheck' )
, fileNames          = cms.untracked.vstring( inputFiles )
#, fileNames          = cms.untracked.vstring('file:////user/walsh/tmp/20110812_Wjets_AODforsync.root')
#, fileNames          = cms.untracked.vstring('file:////user/mccartin/TTbarFall11.root')
#, fileNames          = cms.untracked.vstring('file:////user/bklein/WjetsScaleUpAOD.root')
#, fileNames          = cms.untracked.vstring('file:////user/bklein/AOD_v6_Mu_latest_trigger_menu.root')
#, fileNames          = cms.untracked.vstring('file:///user/ksbeerna/RECO2012DataTEST.root')
, skipBadFiles = cms.untracked.bool( True )
)
if runOnPrompt:
  process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange( '191226:81-191226:831'
                                                                     )

process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( maxEvents )
)

### Output

from PhysicsTools.PatAlgos.patEventContent_cff import patEventContentNoCleaning
process.out = cms.OutputModule( "PoolOutputModule"
, fileName       = cms.untracked.string( 'gentPatSkimPF2PATBroc.root' )
, SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring( 'p' ) )
, outputCommands = cms.untracked.vstring( 'drop *', *patEventContentNoCleaning )
, dropMetaData   = cms.untracked.string( 'ALL' )
)

if not createNTuples:
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
, NPV   = cms.int32( 1 )
)
process.vertexSelection = cms.Sequence(
  process.goodOfflinePrimaryVertices
* process.goodOfflinePrimaryVertexFilter
)

process.eventCleaning = cms.Sequence(
)

if not runOnMC:
  process.eventCleaning += process.HBHENoiseFilter
  process.eventCleaning += process.scrapingFilter

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
         , pvCollection   = cms.InputTag( pvCollection )
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

process.patJets.jetSource = cms.InputTag(pfJetCollection)#Added S
process.jetTracksAssociatorAtVertex.jets = cms.InputTag(pfJetCollection)
process.patJetCorrFactors.src = cms.InputTag(pfJetCollection)
process.patJetGenJetMatch.src = cms.InputTag(pfJetCollection)
process.patJetPartonAssociation.jets = cms.InputTag(pfJetCollection)
process.patJetPartonMatch.src = cms.InputTag(pfJetCollection)
process.pfJetTracksAssociatorAtVertex.jets = cms.InputTag("pfJets")
process.pfMET.jets = cms.InputTag("pfJets")
process.softMuonTagInfosAOD.jets = cms.InputTag(pfJetCollection)#end add

if runMatch:
	process.patJets.addGenJetMatch = True
else:
        process.patJets.addGenJetMatch = False

#process.patJets.genJetMatch    = cms.InputTag( '' )
#process.patPF2PATSequence.remove( process.patJetGenJetMatch )
#process.patPF2PATSequence.remove( process.ak5GenJetsNoNu )
process.patPF2PATSequence.remove( process.ak7GenJetsNoNu )
process.patPF2PATSequence.remove( process.iterativeCone5GenJetsNoNu )
#process.patPF2PATSequence.remove( process.genParticlesForJetsNoNu )
# The following need to be fixed _after_ the (potential) calls to 'removeSpecificPATObjects()' and 'runOnData()'
process.patJetCorrFactors.payload = jetAlgo + 'PFchs'
process.patJetCorrFactors.levels  = jecLevels
if not createNTuples:
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
process.pfSelectedMuons.cut = pfMuonSelect
if usePfMuonIsoConeR03:
  process.pfIsolatedMuons.isolationValueMapsCharged  = cms.VInputTag( cms.InputTag( 'muPFIsoValueCharged03' )
                                                                    )
  process.pfIsolatedMuons.deltaBetaIsolationValueMap = cms.InputTag( 'muPFIsoValuePU03' )
  process.pfIsolatedMuons.isolationValueMapsNeutral  = cms.VInputTag( cms.InputTag( 'muPFIsoValueNeutral03' )
                                                                    , cms.InputTag( 'muPFIsoValueGamma03' )
                                                                    )
process.pfIsolatedMuons.isolationCut = pfMuonIso
process.patMuons.embedTrack = True
if usePfMuonIsoConeR03:
  process.patMuons.isolationValues.pfNeutralHadrons   = cms.InputTag( 'muPFIsoValueNeutral03' )
  process.patMuons.isolationValues.pfPUChargedHadrons = cms.InputTag( 'muPFIsoValuePU03' )
  process.patMuons.isolationValues.pfPhotons          = cms.InputTag( 'muPFIsoValueGamma03' )
  process.patMuons.isolationValues.pfChargedHadrons   = cms.InputTag( 'muPFIsoValueCharged03' )
process.selectedPatMuons.cut = muonSelect
process.cleanPatMuons.src           = cms.InputTag( 'patMuons' )
process.cleanPatMuons.preselection  = muonCut
process.cleanPatMuons.checkOverlaps = cms.PSet()
process.countPatMuons.minNumber = muonsMin

# Electrons
process.pfSelectedElectrons.cut = pfElectronSelect
if usePfElectronIsoConeR03:
  process.pfIsolatedElectrons.isolationValueMapsCharged  = cms.VInputTag( cms.InputTag( 'elPFIsoValueCharged03PFId' )
                                                                        )
  process.pfIsolatedElectrons.deltaBetaIsolationValueMap = cms.InputTag( 'elPFIsoValuePU03PFId' )
  process.pfIsolatedElectrons.isolationValueMapsNeutral  = cms.VInputTag( cms.InputTag( 'elPFIsoValueNeutral03PFId' )
                                                                        , cms.InputTag( 'elPFIsoValueGamma03PFId' )
                                                                        )
process.pfIsolatedElectrons.isolationCut = pfElectronIso
process.patElectrons.embedTrack = True
if usePfElectronIsoConeR03:
  process.patElectrons.isolationValues.pfNeutralHadrons   = cms.InputTag( 'elPFIsoValueNeutral03PFId' )
  process.patElectrons.isolationValues.pfPUChargedHadrons = cms.InputTag( 'elPFIsoValuePU03PFId' )
  process.patElectrons.isolationValues.pfPhotons          = cms.InputTag( 'elPFIsoValueGamma03PFId' )
  process.patElectrons.isolationValues.pfChargedHadrons   = cms.InputTag( 'elPFIsoValueCharged03PFId' )
process.selectedPatElectrons.cut = electronSelect
process.cleanPatElectrons.src           = cms.InputTag( 'patElectrons' )
process.cleanPatElectrons.preselection  = electronCut
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

#process.pfPileUp = cms.EDProducer("PFPileUp",
    #PFCandidates = cms.InputTag("particleFlow"),
    #Enable = cms.bool(True),
    #checkClosestZVertex = cms.bool(False),
    #verbose = cms.untracked.bool(False),
    #Vertices = cms.InputTag("goodOfflinePrimaryVertices")
#)

# Jets
if len( jecLevels ) is 0:
  process.patJets.addJetCorrFactors = False
  print 'WARNING: No JECs are stored or applied!'
elif 'L1FastJet' in jecLevels:
  process.pfPileUp.checkClosestZVertex = False
  process.pfJets.doAreaFastjet = True
  process.pfJets.doRhoFastjet  = False
process.patJets.embedCaloTowers   = False
process.patJets.embedPFCandidates = False
process.selectedPatJets.cut = jetSelect
process.cleanPatJets.src           = cms.InputTag( 'patJets' )
process.cleanPatJets.preselection  = jetCut
process.cleanPatJets.checkOverlaps = cms.PSet()
process.countPatJets.minNumber = jetsMin

### Re-run PF2PAT for non isolated Muons
from PhysicsTools.PatAlgos.tools.helpers import cloneProcessingSnippet
if writeNonIsoMuons:
  cloneProcessingSnippet(process,process.patPF2PATSequence,postfixNonIsoMu)
  getattr(process,'pfNoMuon'+postfixNonIsoMu).topCollection = "pfSelectedMuons" + postfixNonIsoMu
  if runOnMC:
    getattr(process,'muonMatch'+postfixNonIsoMu).src = "pfSelectedMuons" + postfixNonIsoMu
  getattr(process,'patMuons'+postfixNonIsoMu).pfMuonSource = "pfSelectedMuons" + postfixNonIsoMu
  #getattr(process,'pfIsolatedMuons'+postfixNonIsoMu).isolationCut = 999999.
if writeNonIsoElectrons:
  cloneProcessingSnippet(process,process.patPF2PATSequence,postfixNonIsoE)
  getattr(process,'pfNoElectron'+postfixNonIsoE).topCollection = "pfSelectedElectrons" + postfixNonIsoE
  getattr(process,'patElectrons'+postfixNonIsoE).pfElectronSource = "pfSelectedElectrons" + postfixNonIsoE
  #getattr(process,'pfIsolatedElectrons'+postfixNonIsoE).isolationCut = 999999.

### TQAF
if addGenEvt:
  process.load( "TopQuarkAnalysis.TopEventProducers.sequences.ttGenEvent_cff" )

### Path
process.p = cms.Path()

if runOnMC:
  process.load("GeneratorInterface.GenFilters.TotalKinematicsFilter_cfi")
  process.p *= process.totalKinematicsFilter

process.p += process.eventCleaning
process.p += process.patPF2PATSequence
if writeNonIsoMuons:
  process.p += process.patPF2PATSequenceNonIsoMu
if writeNonIsoElectrons:
  process.p += process.patPF2PATSequenceNonIsoE

if addGenEvt:
  process.p *= process.makeGenEvt

if writePdfWeights:
  process.pdfWeights = cms.EDProducer("PdfWeightProducer",
        PdfInfoTag = cms.untracked.InputTag("generator"),
        PdfSetNames = cms.untracked.vstring("cteq66.LHgrid")
	)
  process.p *= process.pdfWeights

if createNTuples:
  process.produceNTuples = cms.EDAnalyzer('FlatNTuples',
	tauTag      = cms.untracked.InputTag("selectedPatTaus"),
	photonTag   = cms.untracked.InputTag("selectedPatPhotons"),

	##########################
	###	Select inputs
	###	For AOD objects make sure you know what you are doing. It's
	###	important to select the correct JetID, JEC, etc.
	##########################
        electronTag = cms.untracked.InputTag(electron),
        muonTag     = cms.untracked.InputTag(muon),
        jetTag      = cms.untracked.InputTag(jet),
        genjetTag      = cms.untracked.InputTag(genjet),
        jetCorServiceName = cms.untracked.string(jetCorService),
        jetIDTag      = cms.untracked.InputTag(jetID),
        metTag      = cms.untracked.InputTag(met),
	pfmetTag    = cms.untracked.InputTag(pfmet),
	pfjetTag    = cms.untracked.InputTag(pfjet),
	pfjetCorServiceName = cms.untracked.string(pfjetCorService),
	pfjetIDTag = cms.untracked.InputTag(pfjetID),
	tcmetTag        = cms.untracked.InputTag(tcmet),
	pfmetTypeITag   = cms.untracked.InputTag(pfmetTypeI),
	pfmetTypeIITag   = cms.untracked.InputTag(pfmetTypeII),
        nonisomuonTag     = cms.untracked.InputTag(nonisomuon),
        nomujetTag      = cms.untracked.InputTag(nomujet),
        nonisoelectronTag     = cms.untracked.InputTag(nonisoelectron),
        noejetTag      = cms.untracked.InputTag(noejet),


	# propagation to station 2
	muon_propagator_cfg = cms.PSet(
		    # Choice of matching algorithm
		useTrack = cms.string("tracker"),  # 'none' to use Candidate P4; or 'tracker ', 'muon', 'global'
		useState = cms.string("atVertex"), # 'innermost' and 'outermost' require the TrackExtra
		useSimpleGeometry = cms.bool(True), # just use a cylinder plus two disks.
		fallbackToME1 = cms.bool(False)    # If propagation to ME2 fails, propagate to ME1
	),


	# Processing Mode:
	# 0: PAT
	# 1: AOD Calo
	# 2: AOD PF
	ProcessingMode = cms.int32(processing_mode),

	#primaryVertexTag   = cms.untracked.InputTag("offlinePrimaryVertices"),
	primaryVertexTag   = cms.untracked.InputTag("goodOfflinePrimaryVertices"),
	HLTAodSummary = cms.InputTag( 'hltTriggerSummaryAOD'),
	HLTriggerResults = cms.InputTag( 'TriggerResults'),
	MuonIDs	= cms.vstring("AllGlobalMuons","AllStandAloneMuons", "AllTrackerMuons"),
	ElectronIDs	= cms.vstring("simpleEleId70Run2011", "simpleEleId95Run2011", "simpleEleId70Run2010", "simpleEleId95Run2010"
					, "eidVeryLooseMC"
					, "eidLooseMC"
					, "eidMediumMC"
					, "eidTightMC"
					, "eidSuperTightMC"
					, "eidHyperTight1MC"
					, "eidHyperTight2MC"
					, "eidHyperTight3MC"
					, "eidHyperTight4MC"
					, "eidVeryLooseMC_eID_only"
					, "eidLooseMC_eID_only"
					, "eidMediumMC_eID_only"
					, "eidTightMC_eID_only"
					, "eidSuperTightMC_eID_only"
					, "eidHyperTight1MC_eID_only"
					, "eidHyperTight2MC_eID_only"
					, "eidHyperTight3MC_eID_only"
					, "eidHyperTight4MC_eID_only"
					, "eidVeryLooseMC_no_iso"
					, "eidLooseMC_no_iso"
					, "eidMediumMC_no_iso"
					, "eidTightMC_no_iso"
					, "eidSuperTightMC_no_iso"
					, "eidHyperTight1MC_no_iso"
					, "eidHyperTight2MC_no_iso"
					, "eidHyperTight3MC_no_iso"
					, "eidHyperTight4MC_no_iso"
					),
	TriggerList      = cms.vstring("HLT_Ele[2-9]\\\d+(?:(?!(No|Anti)BPTX|Tau|MT|MHT|Deta|SC17).)*","HLT_(Iso)?Mu([1-9]\\\d|9)_(?:(?!(Photon|Deta|MT|Ele|HT|MET|Track|Tk|Vertex|NoBPTX|AntiBPTX|Jpsi|Single|Tau|MR|R0)).)*","HLT_(Central)?(Tri|Quad)Jet(?:(?!(No|Anti)BPTX|BTag|Tau|MT|MHT|MET|NoJetID).)*"),
	VetoObjectTriggers = cms.vstring("HLT_.*MET.*"),
	BTagAlgorithms	= cms.vstring(
#BTag algorithms for PAT::Jets
				      "trackCountingHighEffBJetTags",
				      "trackCountingHighPurBJetTags",
				      "jetBProbabilityBJetTags",
      				      "jetProbabilityBJetTags",
				      "softMuonByPtBJetTags",
				      "softMuonByIP3dBJetTags",
				      "softMuonBJetTags",
				      "simpleSecondaryVertexHighEffBJetTags",
				      "simpleSecondaryVertexHighPurBJetTags",
				      "combinedSecondaryVertexMVABJetTags",
				      "combinedSecondaryVertexBJetTags",
				      #"softElectronByPtBJetTags",
				      #"softElectronByIP3dBJetTags"
### BTag algorithms for PF or JPT jets, after rerunning btag
#				      "newTrackCountingHighEffBJetTags",
#				      "newTrackCountingHighPurBJetTags",
#				      "newJetBProbabilityBJetTags",
#      				      "newJetProbabilityBJetTags",
#				      "newSoftMuonByPtBJetTags",
#				      "newSoftMuonByIP3dBJetTags",
#				      "newSoftMuonBJetTags",
#				      "newSimpleSecondaryVertexHighEffBJetTags",
#				      "newSimpleSecondaryVertexHighPurBJetTags",
#				      "newCombinedSecondaryVertexMVABJetTags",
#				      "newCombinedSecondaryVertexBJetTags"
				      #"softElectronByPtBJetTags", #excluded when rerunning btagging
				      #"softElectronByIP3dBJetTags"#if want to include, use dR matching in BTagAssociator
				      ),
        LeptonPFIsoConeSizes = cms.vstring("0.05","0.1","0.15","0.2","0.25","0.3","0.35","0.4","0.45","0.5","0.55","0.6"),
	FillTriggerObjects	= cms.bool(True),
	GetL1FromL1Extra	= cms.bool(False),
	PropagateMuToStation2	= cms.bool(True),
	EnablePartnerTrackFinder = cms.bool(True),
	WriteMET	= cms.bool(True),
	WriteTriggerPrescales	= cms.bool(True),
	WritePDFEventWeights = cms.bool(writePdfWeights),
	WriteWDecayInformation = cms.bool(writeWDecay),
	PDFWeights = cms.VInputTag("pdfWeights:cteq66"),
	Writed0wrtPV	= cms.bool(False),		# write d0 wrt PV
        WriteLooseMuons = cms.bool(writeNonIsoMuons),
        WriteLooseElectrons = cms.bool(writeNonIsoElectrons),
	WriteGenParticles = cms.bool(False),		# write selected MC particles
	SelectedGenParticles = cms.vint32(11,13),	# pdgIds of MC particles to write
	SelectedGenParticlesMinPt = cms.double(1),	# min pt of MC particles to write
	SelectedGenParticlesMaxEta = cms.double(3.0),	# max eta of MC particles to write
	outfile		= cms.string("NTuple.root")
  )
  process.p *= process.produceNTuples

# process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
# process.printTree = cms.EDAnalyzer("ParticleTreeDrawer",
#                                    src = cms.InputTag("genParticles"),
#                                    printP4 = cms.untracked.bool(False),
#                                    printPtEtaPhi = cms.untracked.bool(False),
#                                    printVertex = cms.untracked.bool(False),
#                                    printStatus = cms.untracked.bool(False),
#                                    printIndex = cms.untracked.bool(False),
#                                    status = cms.untracked.vint32( 3,2,1 )
#                                    )
# process.p *= process.printTree
