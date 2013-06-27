#import os
#lxplusTest = ( os.uname()[ 1 ].split( '.' )[ 0 ][ :6 ] == 'lxplus' )
import socket
lxplusTest = ( socket.getfqdn().split( '.' )[ 0 ][ :6 ] == 'lxplus' )

import FWCore.ParameterSet.Config as cms

### Steering

runOnMC       = True
runOnRelVal   = True # If 'False', define input files in l. 228ff.
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
  runOnRelVal = False # If 'False', define input files in l. 228ff.

runMatch  = True
runMVA    = True
runCiC    = False
runEwk    = False
addGenEvt = False
writePdfWeights      = False    # corresponding actions to be updated, s. https://hypernews.cern.ch/HyperNews/CMS/get/top/1499.html ff.
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

jetAlgo   = 'AK5'
jecLevels = []
#jecLevels.append( 'L1FastJet' )
#jecLevels.append( 'L2Relative' )
#jecLevels.append( 'L3Absolute' )
#if not runOnMC:
  #jecLevels.append( 'L2L3Residual' )
typeIMetCorrections = True

# vertex collection to use
# 'offlinePrimaryVertices' or ( 'goodOfflinePrimaryVertices' or 'goodVertices' ) (done anyway for MET filters)
pvCollection = 'goodOfflinePrimaryVertices' # recommended: 'goodOfflinePrimaryVertices' (s. https://hypernews.cern.ch/HyperNews/CMS/get/top-selection/38/1/1/1/2/1/1/2/1/3/1.html)

# jet collection to use
# 'pfJets' or 'pfNoTau'
pfJetCollection = 'pfJets'

# muon top projection object selection
pfMuonSelect = 'pt > 5.' # PF2PAT: 'pt > 5.'
# muon isolation cone
usePfMuonIsoConeR03 = False
# muon top projection isolation
pfMuonIso = 0.2 # PF2PAT: 0.15
pfMuonIsoUseDeltaBeta = True
postfixNonIsoMu = 'NonIsoMu'
# muon object selection
#muonSelect = 'isPFMuon && (isGlobalMuon || isTrackerMuon) && pt > 10. && abs(eta) < 2.5' # RefSel (min. for veto)
muonSelect = ''
# muon event selection
muonCut = 'isPFMuon && (isGlobalMuon || isTrackerMuon) && pt > 10. && abs(eta) < 2.5'
muonsMin = 0


# electron top projection object selection
pfElectronSelect = 'pt > 10. && gsfTrackRef.isNonnull' # PF2PAT: 'pt > 5. && gsfTrackRef.isNonnull && gsfTrackRef.trackerExpectedHitsInner.numberOfLostHits < 2'
# electron isolation cone
usePfElectronIsoConeR03 = True
# electron top projection isolation
pfElectronIso = 0.2 # PF2PAT: 0.2
pfElectronIsoUseDeltaBeta = False
postfixNonIsoE = 'NonIsoE'
# electron object selection
#electronSelect = 'pt > 20. && abs(eta) < 2.5 && electronID("mvaTrigV0") > 0.' # RefSel (min. for veto)
electronSelect = ''
# electron event selection
electronCut  = 'et > 10. && abs(eta) < 2.5'
electronsMin = 0

# x-leptons event selection
leptonsMin = 0
if lxplusTest:
  leptonsMin = 1

# jet object selection
#jetSelect = 'pt > 20. && abs(eta) < 2.5' # RefSel
jetSelect = 'pt > 15. && abs(eta) < 2.6'
# jet event selection
jetCut  = 'pt > 15. && abs(eta) < 2.6'
jetsMin = 0
if lxplusTest:
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

process.load( "Configuration.Geometry.GeometryIdeal_cff" )
process.load( "Configuration.StandardSequences.MagneticField_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
if runOnMC:
  process.GlobalTag.globaltag = 'START53_V21::All'
else:
  process.GlobalTag.globaltag = 'FT_53_V21_AN3::All'

if gc:
	runOnMC   = eval('@MC@')
	#pfJetCollection = '@PFJETS@'
	#usePfMuonIsoConeR03 = eval('@MUTPCONE03@')
	#pfMuonIso = eval('@MUTPISO@')
	#muonsMin = eval('@MINNMU@')
	#usePfElectronIsoConeR03 = eval('@ETPCONE03@')
	#pfElectronsIso = eval('@ETPISO@')
	#electronsMin = eval('@MINNE@')
	#leptonsMin = eval('@MINNLEP@')
	#jetsMin = eval('@MINNJETS@')
        writeNonIsoMuons   = eval('@WRITENONISOMU@')
       	writeNonIsoElectrons   = eval('@WRITENONISOE@')
	if '@WRITENONISOMU@'.lower() == 'false':
        	writeNonIsoMuons   = False
	else:
		writeNonIsoMuons = True

	if '@WRITENONISOE@'.lower() == 'false':
        	writeNonIsoElectrons   = False
	else:
		writeNonIsoElectrons = True

	process.GlobalTag.globaltag = '@GLOBALTAG@'
	# if not set in gc config, take default value
	if '@PDFWEIGHTS@'.lower() == 'true' or '@PDFWEIGHTS@'.lower() == 'false':
		writePdfWeights = eval('@PDFWEIGHTS@')
	else:
		writePdfWeights = False

runMatch            = runMatch            and runOnMC
addGenEvt           = addGenEvt           and runOnMC
writePdfWeights     = writePdfWeights     and runOnMC
filterDecayChannels = filterDecayChannels and runOnMC

## muon propagator requirements # FIXME: Is this needed any more?
#process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi")
#process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAlong_cfi")
#process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorOpposite_cfi")
#process.load("RecoMuon.DetLayers.muonDetLayerGeometry_cfi")

### Logging

process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.MessageLogger.cerr.FwkReport.reportEvery = 100
if lxplusTest:
  process.MessageLogger.cerr.FwkReport.reportEvery = 1
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool( True )
)
if lxplusTest:
  process.Timing = cms.Service( "Timing"
  , summaryOnly = cms.untracked.bool( True )
  )
  process.SimpleMemoryCheck = cms.Service( "SimpleMemoryCheck"
  , ignoreTotal = cms.untracked.int32( 1 )
  )

### Input

process.source = cms.Source( "PoolSource"
, noEventSort        = cms.untracked.bool( True )
, duplicateCheckMode = cms.untracked.string( 'noDuplicateCheck' )
, fileNames          = cms.untracked.vstring()
, skipBadFiles = cms.untracked.bool( True )
)

inputFiles = cms.untracked.vstring()
if runOnRelVal:
  if runOnMC:
    from PhysicsTools.PatAlgos.patInputFiles_cff import filesRelValProdTTbarAODSIM
    inputFiles = filesRelValProdTTbarAODSIM
  else:
    inputFiles = [
       '/store/relval/CMSSW_5_3_7_patch6-GR_R_53_V16_RelVal_mu2012D_Reference/SingleMu/RECO/v1/00004/FE49EA68-7975-E211-B190-00259059642A.root',
       '/store/relval/CMSSW_5_3_7_patch6-GR_R_53_V16_RelVal_mu2012D_Reference/SingleMu/RECO/v1/00004/FCFDF118-7975-E211-8BBC-00261894395A.root',
       '/store/relval/CMSSW_5_3_7_patch6-GR_R_53_V16_RelVal_mu2012D_Reference/SingleMu/RECO/v1/00004/FC859A16-7875-E211-A52F-0026189438C4.root',
       '/store/relval/CMSSW_5_3_7_patch6-GR_R_53_V16_RelVal_mu2012D_Reference/SingleMu/RECO/v1/00004/F882C139-7B75-E211-9583-0025905964C4.root',
       '/store/relval/CMSSW_5_3_7_patch6-GR_R_53_V16_RelVal_mu2012D_Reference/SingleMu/RECO/v1/00004/F819E38B-7A75-E211-AD37-002590593878.root'
       ]
       #'/store/relval/CMSSW_5_3_7_patch6-GR_R_53_V16_RelVal_electron2012D_Reference/SingleElectron/RECO/v1/00001/FEA8DFEB-2975-E211-8991-003048FF3C9A.root',
       #'/store/relval/CMSSW_5_3_7_patch6-GR_R_53_V16_RelVal_electron2012D_Reference/SingleElectron/RECO/v1/00001/FE3A4355-2A75-E211-B4CD-0025902CB620.root',
       #'/store/relval/CMSSW_5_3_7_patch6-GR_R_53_V16_RelVal_electron2012D_Reference/SingleElectron/RECO/v1/00001/FCFD8DE0-2975-E211-9C3A-003048CF99DE.root',
       #'/store/relval/CMSSW_5_3_7_patch6-GR_R_53_V16_RelVal_electron2012D_Reference/SingleElectron/RECO/v1/00001/FC336D49-2975-E211-8972-C86000151BB0.root',
       #'/store/relval/CMSSW_5_3_7_patch6-GR_R_53_V16_RelVal_electron2012D_Reference/SingleElectron/RECO/v1/00001/FA8D3A7D-AC75-E211-A5EB-001D09F2465A.root'
       #]

    #hltProcess = 'reHLT'
else:
  if runOnMC:
    if lxplusTest:
      inputFiles = [ '/store/user/vadler/cms/Top/CMSSW_5_3_10_patch1/output/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola__Summer12_DR53X-PU_S10_START53_V7C-v1__AODSIM_1_1_8JH.root'
                   , '/store/user/vadler/cms/Top/CMSSW_5_3_10_patch1/output/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola__Summer12_DR53X-PU_S10_START53_V7C-v1__AODSIM_2_1_7TX.root'
                   , '/store/user/vadler/cms/Top/CMSSW_5_3_10_patch1/output/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola__Summer12_DR53X-PU_S10_START53_V7C-v1__AODSIM_3_1_qEJ.root'
                   , '/store/user/vadler/cms/Top/CMSSW_5_3_10_patch1/output/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola__Summer12_DR53X-PU_S10_START53_V7C-v1__AODSIM_4_1_LX5.root'
                   , '/store/user/vadler/cms/Top/CMSSW_5_3_10_patch1/output/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola__Summer12_DR53X-PU_S10_START53_V7C-v1__AODSIM_5_1_5vg.root'
                   ]
      #inputFiles = [ '/store/user/vadler/cms/Top/CMSSW_5_3_10_patch1/output/TT_8TeV-mcatnlo__Summer12_DR53X-PU_S10_START53_V7A-v1__AODSIM_1_1_e2E.root'
                   #, '/store/user/vadler/cms/Top/CMSSW_5_3_10_patch1/output/TT_8TeV-mcatnlo__Summer12_DR53X-PU_S10_START53_V7A-v1__AODSIM_2_1_8Ng.root'
                   #, '/store/user/vadler/cms/Top/CMSSW_5_3_10_patch1/output/TT_8TeV-mcatnlo__Summer12_DR53X-PU_S10_START53_V7A-v1__AODSIM_3_1_TyS.root'
                   #, '/store/user/vadler/cms/Top/CMSSW_5_3_10_patch1/output/TT_8TeV-mcatnlo__Summer12_DR53X-PU_S10_START53_V7A-v1__AODSIM_4_1_kI3.root'
                   #, '/store/user/vadler/cms/Top/CMSSW_5_3_10_patch1/output/TT_8TeV-mcatnlo__Summer12_DR53X-PU_S10_START53_V7A-v1__AODSIM_5_1_qxT.root'
                   #, '/store/user/vadler/cms/Top/CMSSW_5_3_10_patch1/output/TT_8TeV-mcatnlo__Summer12_DR53X-PU_S10_START53_V7A-v1__AODSIM_6_1_1yB.root'
                   #]
    else:
      inputFiles = [ 'file:////user/bklein/TTbar_2012_synchronisation_ex.root'
                   ]
  else:
    if lxplusTest:
      process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange( #'201197:1-201197:23'
                                                                         )
    inputFiles = [ #'/store/data/Run2012C/MuHad/AOD/PromptReco-v2/000/201/197/94C63DB6-D2EB-E111-AE27-003048D375AA.root'
                 ]
    #inputFiles = [ #'/store/data/Run2012C/ElectronHad/AOD/PromptReco-v2/000/201/197/2C1E6199-D2EB-E111-B2C6-0025B3203898.root'
                 #]
process.source.fileNames = cms.untracked.vstring( inputFiles )
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( maxEvents )
)

### Output

outputModules = []
if not createNTuples:
  from PhysicsTools.PatAlgos.patEventContent_cff import patEventContentNoCleaning
  process.out = cms.OutputModule( "PoolOutputModule"
  , fileName       = cms.untracked.string( 'gentPatSkimPF2PATBroc.root' )
  , SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring( 'p' ) )
  , outputCommands = cms.untracked.vstring( 'drop *', *patEventContentNoCleaning )
  , dropMetaData   = cms.untracked.string( 'ALL' )
  )
  outputModules.append( 'out' )
  # Outpath
  process.outpath = cms.EndPath(
    process.out
    )

### Cleaning

process.load("RecoMET.METFilters.metFilters_cff")

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
process.vertexSelection = cms.Sequence(
  getattr( process, pvCollection )
)

if not runOnMC:
  process.goodOfflinePrimaryVertexFilter = cms.EDFilter(
    "PrimaryVertexFilter"
  , pvSelection
  , pvSrc = cms.InputTag( pvCollection )
  , NPV   = cms.int32( 1 )
  )
  process.vertexSelection *= process.goodOfflinePrimaryVertexFilter

process.eventCleaning = cms.Sequence()
if triggerSelection != '':
  process.eventCleaning *= process.triggerResultsFilter
if not runOnMC:
  process.eventCleaning += process.scrapingFilter
process.eventCleaning += process.vertexSelection
process.eventCleaning += process.metFilters

if writePdfWeights:
  process.pdfWeights = cms.EDProducer(
    "PdfWeightProducer"
  , PdfInfoTag  = cms.untracked.InputTag( 'generator' )
  , PdfSetNames = cms.untracked.vstring( 'cteq66.LHgrid' )
  #, PdfSetNames = cms.untracked.vstring( 'cteq66.LHgrid', 'CT10.LHgrid' )
  )

### PAT

process.load( "PhysicsTools.PatAlgos.patSequences_cff" )

# Misc
if len( jecLevels ) is 0:
  typeIMetCorrections = False
from PhysicsTools.PatAlgos.tools.pfTools import usePF2PAT
usePF2PAT( process
         , runOnMC             = runOnMC
         , jetAlgo             = jetAlgo
         , jetCorrections      = ( jetAlgo + 'PFchs'
                                 , jecLevels
                                 )
         , typeIMetCorrections = typeIMetCorrections
         , pvCollection        = cms.InputTag( pvCollection )
         , outputModules       = outputModules
         )
from PhysicsTools.PatAlgos.tools.helpers import removeIfInSequence
removeIfInSequence( process, 'patPFParticles', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'selectedPatPFParticles', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'countPatPFParticles', 'patPF2PATSequence', '' )
from PhysicsTools.PatAlgos.tools.coreTools import *
removeSpecificPATObjects( process
                        , names          = [ 'Photons', 'Taus' ]
                        , outputModules  = outputModules
                        )
# The following is not performed (properly) by 'removeSpecificPATObjects()'
process.cleanPatCandidateSummary.candidates.remove( cms.InputTag( 'cleanPatPhotons' ) )
process.cleanPatCandidateSummary.candidates.remove( cms.InputTag( 'cleanPatTaus' ) )
if not pfJetCollection == 'pfNoTau':
  if not typeIMetCorrections:
    removeIfInSequence( process, 'pfNoJet', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfTauPFJets08Region', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfTauPileUpVertices', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfTauTagInfoProducer', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfJetsPiZeros', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfJetsLegacyTaNCPiZeros', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfJetsLegacyHPSPiZeros', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfTausBase', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'hpsSelectionDiscriminator', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'hpsPFTauProducerSansRefs', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'hpsPFTauProducer', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfTausBaseDiscriminationByDecayModeFinding', 'patPF2PATSequence', '' )
  removeIfInSequence( process, 'pfTausBaseDiscriminationByLooseCombinedIsolationDBSumPtCorr', 'patPF2PATSequence', '' )
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
removeIfInSequence( process, 'hpsPFTauDiscriminationByDecayModeFinding', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByVLooseChargedIsolation', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByLooseChargedIsolation', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByMediumChargedIsolation', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByTightChargedIsolation', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByVLooseIsolation', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByLooseIsolation', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByMediumIsolation', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByTightIsolation', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByVLooseIsolationDBSumPtCorr', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByLooseIsolationDBSumPtCorr', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByMediumIsolationDBSumPtCorr', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByTightIsolationDBSumPtCorr', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByRawCombinedIsolationDBSumPtCorr', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByRawChargedIsolationDBSumPtCorr', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByRawGammaIsolationDBSumPtCorr', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByVLooseCombinedIsolationDBSumPtCorr', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByLooseCombinedIsolationDBSumPtCorr', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByMediumCombinedIsolationDBSumPtCorr', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByTightCombinedIsolationDBSumPtCorr', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByLooseElectronRejection', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByMediumElectronRejection', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByTightElectronRejection', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByMVAElectronRejection', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByLooseMuonRejection', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByMediumMuonRejection', 'patPF2PATSequence', '' )
removeIfInSequence( process, 'hpsPFTauDiscriminationByTightMuonRejection', 'patPF2PATSequence', '' )
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

process.patJets.jetSource = cms.InputTag( pfJetCollection )
process.jetTracksAssociatorAtVertex.jets = cms.InputTag( pfJetCollection )
process.patJetCorrFactors.src = cms.InputTag( pfJetCollection )
process.patJetGenJetMatch.src = cms.InputTag( pfJetCollection )
process.patJetPartonAssociation.jets = cms.InputTag( pfJetCollection )
process.patJetPartonMatch.src = cms.InputTag( pfJetCollection )
process.pfJetTracksAssociatorAtVertex.jets = cms.InputTag( pfJetCollection )
process.pfMET.jets = cms.InputTag( pfJetCollection )

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
                                , 'keep *_%s_*_*'%( pvCollection )
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
process.pfIsolatedMuons.isolationCut          = pfMuonIso
process.pfIsolatedMuons.doDeltaBetaCorrection = pfMuonIsoUseDeltaBeta
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
  process.pfIsolatedElectrons.isolationValueMapsCharged  = cms.VInputTag( cms.InputTag( 'elPFIsoValueCharged03PFId' )
                                                                        )
  process.pfIsolatedElectrons.deltaBetaIsolationValueMap = cms.InputTag( 'elPFIsoValuePU03PFId' )
  process.pfIsolatedElectrons.isolationValueMapsNeutral  = cms.VInputTag( cms.InputTag( 'elPFIsoValueNeutral03PFId' )
                                                                        , cms.InputTag( 'elPFIsoValueGamma03PFId' )
                                                                        )
  process.pfElectrons.isolationValueMapsCharged  = cms.VInputTag( cms.InputTag( 'elPFIsoValueCharged03PFId' )
                                                                )
  process.pfElectrons.deltaBetaIsolationValueMap = cms.InputTag( 'elPFIsoValuePU03PFId' )
  process.pfElectrons.isolationValueMapsNeutral  = cms.VInputTag( cms.InputTag( 'elPFIsoValueNeutral03PFId' )
                                                                , cms.InputTag( 'elPFIsoValueGamma03PFId' )
                                                                )
process.pfIsolatedElectrons.isolationCut          = pfElectronIso
process.pfIsolatedElectrons.doDeltaBetaCorrection = pfElectronIsoUseDeltaBeta
process.patElectrons.embedTrack = True
if usePfElectronIsoConeR03:
  process.patElectrons.isolationValues.pfNeutralHadrons   = cms.InputTag( 'elPFIsoValueNeutral03PFId' )
  process.patElectrons.isolationValues.pfChargedAll       = cms.InputTag( 'elPFIsoValueChargedAll03PFId' )
  process.patElectrons.isolationValues.pfPUChargedHadrons = cms.InputTag( 'elPFIsoValuePU03PFId' )
  process.patElectrons.isolationValues.pfPhotons          = cms.InputTag( 'elPFIsoValueGamma03PFId' )
  process.patElectrons.isolationValues.pfChargedHadrons   = cms.InputTag( 'elPFIsoValueCharged03PFId' )
process.selectedPatElectrons.cut = electronSelect
process.cleanPatElectrons.src           = cms.InputTag( 'patElectrons' )
process.cleanPatElectrons.preselection  = electronCut
process.cleanPatElectrons.checkOverlaps = cms.PSet()
process.countPatElectrons.minNumber = electronsMin
if runMVA:
  process.load('EgammaAnalysis.ElectronTools.electronIdMVAProducer_cfi')
  process.eidMVASequence = cms.Sequence( process.mvaTrigV0
                                       + process.mvaNonTrigV0
                                       )
  process.patPF2PATSequence.replace( process.patElectrons
                                   , process.eidMVASequence * process.patElectrons
                                   )
  process.patElectrons.electronIDSources.mvaTrigV0    = cms.InputTag("mvaTrigV0")
  process.patElectrons.electronIDSources.mvaNonTrigV0 = cms.InputTag("mvaNonTrigV0")
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
  print '         TypeI MET corrections have been switched off in PF2PAT.'
  print '         Following settings are adjusted for on-the-fly usage of L1FastJet (CHS):'
  print '         - pfPileUp.checkClosestZVertex = False'
  print '         - pfJets.doAreaFastjet = True'
  process.pfPileUp.checkClosestZVertex = False
  process.pfJets.doAreaFastjet = True
elif 'L1FastJet' in jecLevels:
  process.pfPileUp.checkClosestZVertex = False
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
  #if runOnMC:
    #getattr(process,'electronMatch'+postfixNonIsoE).src = "pfElectrons" + postfixNonIsoE
  getattr(process,'patElectrons'+postfixNonIsoE).pfElectronSource = "pfSelectedElectrons" + postfixNonIsoE
  #getattr(process,'pfIsolatedElectrons'+postfixNonIsoE).isolationCut = 999999.

### TQAF

if addGenEvt:
  process.load( "TopQuarkAnalysis.TopEventProducers.sequences.ttGenEvent_cff" )
if filterDecayChannels:
  process.load( "TopQuarkAnalysis.TopSkimming.ttDecayChannelFilters_cff" )
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
					#, "mvaTrigV0"
					#, "mvaNonTrigV0"
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
        EnableConvRej2012 = cms.bool(True),
	WriteMET	= cms.bool(True),
	WriteTriggerPrescales	= cms.bool(True),
	WritePDFEventWeights = cms.bool(writePdfWeights),
	WriteWDecayInformation = cms.bool(False),
	PDFWeights = cms.VInputTag("pdfWeights:cteq66"),
	Writed0wrtPV	= cms.bool(True),		# write d0 wrt PV
        WriteLooseMuons = cms.bool(writeNonIsoMuons),
        WriteLooseElectrons = cms.bool(writeNonIsoElectrons),
	WriteGenParticles = cms.bool(runOnMC),		# write selected MC particles
	SelectedGenParticles = cms.vint32(5),	# pdgIds of MC particles to write
	SelectedGenParticlesMinPt = cms.double(0.),	# min pt of MC particles to write
	SelectedGenParticlesMaxEta = cms.double(0.1),	# max eta of MC particles to write
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
