#import os
#lxplusTest = ( os.uname()[ 1 ].split( '.' )[ 0 ][ :6 ] == 'lxplus' )
import socket
lxplusTest = ( socket.getfqdn().split( '.' )[ 0 ][ :6 ] == 'lxplus' )

import FWCore.ParameterSet.Config as cms

### Steering

runOnMC       = True
runOnRelVal   = False # If 'False', define input files in l. 184ff.
maxEvents     = -1
gc            = True
createNTuples = True
if lxplusTest:
  gc            = False
  createNTuples = False
  maxEvents     = 100
  if not runOnMC:
    maxEvents = 1000
else:
  runOnRelVal = False # If 'False', define input files in l. 184ff.

runMatch  = True
runCiC    = True
runEwk    = True
addGenEvt = True
writePdfWeights      = False
writeWDecay          = False # this should only be set True for *broken* W datasets
writeNonIsoMuons     = True
writeNonIsoElectrons = True
filterDecayChannels        = False
addMuonChannel             = True
addElectronChannel         = True
addTauChannel              = False
restrictTauChannelMuon     = False
restrictTauChannelElectron = False

hltProcess       = 'HLT'
triggerSelection = ''

pfJetCollection = 'pfJets'
#pfJetCollection = 'pfNoTau'
postfixNonIsoMu = 'NonIsoMu'
postfixNonIsoE = 'NonIsoE'
# muon isolation
usePfMuonIsoConeR03 = False
pfMuonIso = 0.15 # PF2PAT: 0.15
muonsMin = 0
# electron isolation
usePfElectronIsoConeR03 = False
# electron top projection isolation
pfElectronIso = 0.2 # PF2PAT: 0.2
electronsMin = 0
# x-leptons event selection
leptonsMin = 0
jetsMin = 0

jetAlgo   = 'AK5'
jecLevels = []
#jecLevels = [ 'L1FastJet', 'L2Relative', 'L3Absolute' ]
#if not runOnMC:
  #jecLevels.append( 'L2L3Residual' )

# muon object selection
pfMuonSelect = 'pt > 5.' # PF2PAT: 'pt > 5.'
#muonSelect = 'isGlobalMuon && pt > 10. && abs(eta) < 2.5' # RefSel (min. for veto)
muonSelect = ''
# muon event selection
muonCut = 'isGlobalMuon && pt > 5. && abs(eta) < 2.5 && globalTrack().hitPattern().numberOfValidMuonHits() > 0 && numberOfMatches() > 1 && innerTrack().numberOfValidHits()> 10 && innerTrack().hitPattern().numberOfValidPixelHits()>0'

# electron object selection
pfElectronSelect = 'pt > 5. && gsfTrackRef.isNonnull && gsfTrackRef.trackerExpectedHitsInner.numberOfLostHits < 2' # PF2PAT: 'pt > 5. && gsfTrackRef.isNonnull && gsfTrackRef.trackerExpectedHitsInner.numberOfLostHits < 2'
#electronSelect = 'et > 15. && abs(eta) < 2.5' # RefSel (min. for veto)
electronSelect = ''
# electron event selection
electronCut = 'et > 5. && abs(eta) < 3.0'

# jet object selection
#jetSelect = 'pt > 30. && abs(eta) < 2.4' # RefSel
jetSelect = ''
# jet event selection
jetsCut = 'pt > 15. && abs(eta) < 3.0'


if gc:
	runOnMC   = eval('@MC@')
	pfJetCollection = '@PFJETS@'
	muonsIsoR = eval('@MUTPCONE@')
	muonsIsoTP = eval('@MUTPISO@')
	muonsMin = eval('@MINNMU@')
	electronsIsoR = eval('@ETPCONE@')
	electronsIsoTP = eval('@ETPISO@')
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


runMatch  = runMatch  and runOnMC
addGenEvt = addGenEvt and runOnMC
writePdfWeights = writePdfWeights and runOnMC
filterDecayChannels = filterDecayChannels and runOnMC

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
  process.GlobalTag.globaltag = 'START44_V13::All'
else:
  process.GlobalTag.globaltag = 'GR_R_44_V15::All'

# muon propagator requirements
process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi")
process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAlong_cfi")
process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorOpposite_cfi")
process.load("RecoMuon.DetLayers.muonDetLayerGeometry_cfi")

# Process the PDF weights
process.pdfWeights = cms.EDProducer("PdfWeightProducer",
        PdfInfoTag = cms.untracked.InputTag("generator"),
        PdfSetNames = cms.untracked.vstring("cteq66.LHgrid")
	)

### Logging

process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool( True )
)
process.Timing = cms.Service( "Timing"
, summaryOnly = cms.untracked.bool( True )
)

### Input

process.source = cms.Source( "PoolSource"
, noEventSort        = cms.untracked.bool( True )
, duplicateCheckMode = cms.untracked.string( 'noDuplicateCheck' )
, fileNames          = cms.untracked.vstring()
, skipBadFiles = cms.untracked.bool( True )
)

from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
inputFiles = cms.untracked.vstring()
if runOnRelVal:
  if runOnMC:
    inputFiles = []
  else:
    inputFiles = []
else:
  if runOnMC:
    if lxplusTest:
      #inputFiles = [ '/store/user/vadler/cms/AT/CMSSW_4_4_4/data/Fall11_R3/copyFall11_1_2_ibi.root'
                   #]
      from PhysicsTools.PatAlgos.myPatTuple_mcFall11_R4_cff import fileNames
      inputFiles = fileNames.fileNames
    else:
      inputFiles = [ 'file:////user/bklein/TTbar_44X.root'
                   ]
  else:
    if lxplusTest:
      inputFiles = []
    else:
      inputFiles = []

process.source.fileNames = inputFiles
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

# Filter for Pythia bug
if runOnMC:
  process.load("GeneratorInterface.GenFilters.TotalKinematicsFilter_cfi")

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
  process.HBHENoiseFilter
+ process.scrapingFilter
)

if runOnMC:
  process.eventCleaning += process.totalKinematicsFilter

if triggerSelection != '':
  process.eventCleaning += process.triggerResultsFilter

process.eventCleaning += process.goodOfflinePrimaryVertices

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
         , pvCollection   = 'goodOfflinePrimaryVertices'
         )
from PhysicsTools.PatAlgos.tools.helpers import removeIfInSequence
removeIfInSequence( process, 'patPFParticles', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'selectedPatPFParticles', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'countPatPFParticles', 'patPF2PATSequence', '' )
from PhysicsTools.PatAlgos.tools.coreTools import *
removeSpecificPATObjects( process
                        , names = [ 'Photons', 'Taus' ]
                        )
# The following is not performed (properly) by 'removeSpecificPATObjects()'
process.cleanPatCandidateSummary.candidates.remove( cms.InputTag( 'cleanPatPhotons' ) )
process.cleanPatCandidateSummary.candidates.remove( cms.InputTag( 'cleanPatTaus' ) )
if pfJetCollection == 'pfJets':
  removeIfInSequence( process, 'pfNoJet', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfTauPFJets08Region', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfTauPileUpVertices', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfTauTagInfoProducer', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfJetsPiZeros', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfJetsLegacyTaNCPiZeros', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfJetsLegacyHPSPiZeros', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfTausBase', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfTausBaseDiscriminationByLeadingTrackFinding', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfTausBaseDiscriminationByIsolation', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfTausBaseDiscriminationByLeadingPionPtCut', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfTaus', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfNoTau', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'tauIsoDepositPFCandidates', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'tauIsoDepositPFChargedHadrons', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'tauIsoDepositPFNeutralHadrons', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'tauIsoDepositPFGammas', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'tauMatch', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'tauGenJets', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'tauGenJetsSelectorAllHadrons', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'tauGenJetMatch', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'shrinkingConePFTauDiscriminationByLeadingTrackFinding', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'shrinkingConePFTauDiscriminationByLeadingTrackPtCut', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'shrinkingConePFTauDiscriminationByLeadingPionPtCut', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'shrinkingConePFTauDiscriminationByIsolation', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'shrinkingConePFTauDiscriminationByTrackIsolation', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'shrinkingConePFTauDiscriminationByECALIsolation', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'shrinkingConePFTauDiscriminationByIsolationUsingLeadingPion', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'shrinkingConePFTauDiscriminationByTrackIsolationUsingLeadingPion', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'shrinkingConePFTauDiscriminationByECALIsolationUsingLeadingPion', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'shrinkingConePFTauDiscriminationAgainstElectron', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'shrinkingConePFTauDiscriminationAgainstMuon', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'shrinkingConePFTauDiscriminationByTaNC', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'shrinkingConePFTauDiscriminationByTaNCfrOnePercent', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'shrinkingConePFTauDiscriminationByTaNCfrHalfPercent', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'shrinkingConePFTauDiscriminationByTaNCfrQuarterPercent', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'shrinkingConePFTauDiscriminationByTaNCfrTenthPercent', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'patTaus', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'selectedPatTaus', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'countPatTaus', 'patPF2PATSequence', '' )
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
if runMatch:
  process.patJets.addGenJetMatch = True
else:
  process.patMuons.addGenMatch = False
  removeIfInSequence( process, 'muonMatch', 'patPF2PATSequence', '' )
  process.patElectrons.addGenMatch = False
  removeIfInSequence( process, 'electronMatch', 'patPF2PATSequence', '' )
  process.patJets.addGenPartonMatch   = False
  process.patJets.embedGenPartonMatch = False
  process.patJets.genPartonMatch      = cms.InputTag( '' )
  removeIfInSequence( process, 'patJetPartonMatch', 'patPF2PATSequence', '' )
  process.patJets.getJetMCFlavour    = False
  process.patJets.JetPartonMapSource = cms.InputTag( '' )
  removeIfInSequence( process, 'patJetFlavourId', 'patPF2PATSequence', '' )
  process.patJets.addGenJetMatch = False
  process.patJets.genJetMatch    = cms.InputTag( '' )
  removeIfInSequence( process, 'patJetGenJetMatch', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'ak5GenJetsNoNu', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'genParticlesForJetsNoNu', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'ak7GenJetsNoNu', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'iterativeCone5GenJetsNoNu', 'patPF2PATSequence', '' )

process.patJets.jetSource = cms.InputTag(pfJetCollection)
process.jetTracksAssociatorAtVertex.jets = cms.InputTag(pfJetCollection)
process.patJetCorrFactors.src = cms.InputTag(pfJetCollection)
process.patJetGenJetMatch.src = cms.InputTag(pfJetCollection)
process.patJetPartonAssociation.jets = cms.InputTag(pfJetCollection)
process.patJetPartonMatch.src = cms.InputTag(pfJetCollection)
process.pfJetTracksAssociatorAtVertex.jets = cms.InputTag("pfJets")
process.pfMET.jets = cms.InputTag("pfJets")
process.softMuonTagInfosAOD.jets = cms.InputTag(pfJetCollection)#end add

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
    process.out.outputCommands += [ 'keep *_decaySubset_*_*'
                                  , 'keep *_initSubset_*_*'
                                  , 'keep *_genEvt_*_*'
                                  ]

process.out.outputCommands += [ 'keep double_kt6PFJets_*_*' ]

# Muons
process.pfSelectedMuons.cut = pfMuonSelect
if usePfMuonIsoConeR03:
  process.pfIsolatedMuons.isolationValueMapsCharged  = cms.VInputTag( cms.InputTag( 'muPFIsoValueCharged03' )
                                                                    )
  process.pfIsolatedMuons.deltaBetaIsolationValueMap = cms.InputTag( 'muPFIsoValuePU03' )
  process.pfIsolatedMuons.isolationValueMapsNeutral  = cms.VInputTag( cms.InputTag( 'muPFIsoValueNeutral03' )
                                                                    , cms.InputTag( 'muPFIsoValueGamma03' )
                                                                    )
  process.pfMuons.isolationValueMapsCharged  = cms.VInputTag( cms.InputTag( 'muPFIsoValueCharged03' )
                                                            )
  process.pfMuons.deltaBetaIsolationValueMap = cms.InputTag( 'muPFIsoValuePU03' )
  process.pfMuons.isolationValueMapsNeutral  = cms.VInputTag( cms.InputTag( 'muPFIsoValueNeutral03' )
                                                            , cms.InputTag( 'muPFIsoValueGamma03' )
                                                            )
process.pfIsolatedMuons.isolationCut = pfMuonIso
process.patMuons.embedTrack = True
if usePfMuonIsoConeR03:
  process.patMuons.isolationValues.pfNeutralHadrons   = cms.InputTag( 'muPFIsoValueNeutral03' )
  process.patMuons.isolationValues.pfChargedAll       = cms.InputTag( 'muPFIsoValueChargedAll03' )
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
  process.pfIsolatedElectrons.isolationValueMapsCharged  = cms.VInputTag( cms.InputTag( 'elPFIsoValueCharged03' )
                                                                        )
  process.pfIsolatedElectrons.deltaBetaIsolationValueMap = cms.InputTag( 'elPFIsoValuePU03' )
  process.pfIsolatedElectrons.isolationValueMapsNeutral  = cms.VInputTag( cms.InputTag( 'elPFIsoValueNeutral03' )
                                                                        , cms.InputTag( 'elPFIsoValueGamma03' )
                                                                        )
  process.pfElectrons.isolationValueMapsCharged  = cms.VInputTag( cms.InputTag( 'elPFIsoValueCharged03PFId' )
                                                                )
  process.pfElectrons.deltaBetaIsolationValueMap = cms.InputTag( 'elPFIsoValuePU03PFId' )
  process.pfElectrons.isolationValueMapsNeutral  = cms.VInputTag( cms.InputTag( 'elPFIsoValueNeutral03PFId' )
                                                                , cms.InputTag( 'elPFIsoValueGamma03PFId' )
                                                                )
process.pfIsolatedElectrons.isolationCut = pfElectronIso
process.patElectrons.embedTrack = True
if usePfElectronIsoConeR03:
  process.patElectrons.isolationValues.pfNeutralHadrons   = cms.InputTag( 'elPFIsoValueNeutral03' )
  process.patElectrons.isolationValues.pfChargedAll       = cms.InputTag( 'elPFIsoValueChargedAll03PFId' )
  process.patElectrons.isolationValues.pfPUChargedHadrons = cms.InputTag( 'elPFIsoValuePU03' )
  process.patElectrons.isolationValues.pfPhotons          = cms.InputTag( 'elPFIsoValueGamma03' )
  process.patElectrons.isolationValues.pfChargedHadrons   = cms.InputTag( 'elPFIsoValueCharged03' )
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

# Jets
if len( jecLevels ) is 0:
  process.patJets.addJetCorrFactors = False
  print 'WARNING: No JECs are stored or applied!'
  print '         Following settings are adjusted for on-the-fly usage of L1FastJet (CHS):'
  print '         - pfPileUp.checkClosestZVertex = False'
  print '         - pfJets.doAreaFastjet = True'
  print '         - pfJets.doRhoFastjet  = False'
  process.pfPileUp.checkClosestZVertex = False
  process.pfJets.doAreaFastjet = True
  process.pfJets.doRhoFastjet  = False
elif 'L1FastJet' in jecLevels:
  process.pfPileUp.checkClosestZVertex = False
  process.pfJets.doAreaFastjet = True
  process.pfJets.doRhoFastjet  = False
process.patJets.embedCaloTowers   = False
process.patJets.embedPFCandidates = False
process.selectedPatJets.cut = jetSelect
process.cleanPatJets.src           = cms.InputTag( 'patJets' )
process.cleanPatJets.preselection  = jetsCut
process.cleanPatJets.checkOverlaps = cms.PSet()
process.countPatJets.minNumber = jetsMin

### Re-run PF2PAT for non isolated Muons
from PhysicsTools.PatAlgos.tools.helpers import cloneProcessingSnippet
if writeNonIsoMuons:
  cloneProcessingSnippet(process,process.patPF2PATSequence,postfixNonIsoMu)
  getattr(process,'pfNoMuon'+postfixNonIsoMu).topCollection = "pfMuons" + postfixNonIsoMu
  if runOnMC:
    getattr(process,'muonMatch'+postfixNonIsoMu).src = "pfMuons" + postfixNonIsoMu
  getattr(process,'patMuons'+postfixNonIsoMu).pfMuonSource = "pfMuons" + postfixNonIsoMu
if writeNonIsoElectrons:
  cloneProcessingSnippet(process,process.patPF2PATSequence,postfixNonIsoE)
  getattr(process,'pfNoElectron'+postfixNonIsoE).topCollection = "pfElectrons" + postfixNonIsoE
  #if runOnMC:
    #getattr(process,'electronMatch'+postfixNonIsoE).src = "pfElectrons" + postfixNonIsoE
  getattr(process,'patElectrons'+postfixNonIsoE).pfElectronSource = "pfElectrons" + postfixNonIsoE

### TQAF
if addGenEvt:
  process.load( "TopQuarkAnalysis.TopEventProducers.sequences.ttGenEvent_cff" )
if filterDecayChannels:
  process.load( "TopQuarkAnalysis.TopSkimming.ttDecayChannelFilter_cff" )
  if addGenEvt:
    process.ttSemiLeptonicFilter.src = 'genEvt'
  if not addMuonChannel:
    process.ttSemiLeptonicFilter.allowedTopDecays.decayBranchA.muon = False
  if not addElectronChannel:
    process.ttSemiLeptonicFilter.allowedTopDecays.decayBranchA.electron = False
  if addTauChannel:
    process.ttSemiLeptonicFilter.allowedTopDecays.decayBranchA.tau = True
    if restrictTauChannelMuon:
      process.ttSemiLeptonicFilter.allowedTopDecays.restrictTauDecays.muon = cms.bool( True )
    if restrictTauChannelElectron:
      process.ttSemiLeptonicFilter.allowedTopDecays.restrictTauDecays.electron = cms.bool( True )

### Path
process.p = cms.Path( process.eventCleaning
                    )

if addGenEvt:
  process.p *= process.makeGenEvt
if filterDecayChannels:
  process.p *= process.ttSemiLeptonicFilter

process.p *= process.patPF2PATSequence

if writeNonIsoMuons:
  process.p *= process.patPF2PATSequenceNonIsoMu
if writeNonIsoElectrons:
  process.p *= process.patPF2PATSequenceNonIsoE

if addGenEvt:
  process.p *= process.makeGenEvt

if writePdfWeights:
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
	WriteGenParticles = cms.bool(runOnMC),		# write selected MC particles
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
