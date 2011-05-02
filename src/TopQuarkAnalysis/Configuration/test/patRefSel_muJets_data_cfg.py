import FWCore.ParameterSet.Config as cms

process = cms.Process( 'PAT' )


###
### Constants
###

### ------------------------- Reference selection -------------------------- ###

### Switch on/off selection steps

useLooseMuon    = True
useTightMuon    = True
useMuonVeto     = True
useElectronVeto = True
use1Jet         = True
use2Jets        = False
use3Jets        = False
use4Jets        = False

### Trigger selection

# logical expression
#triggerSelection = 'HLT_Mu9'                 # run <  147196
triggerSelection = 'HLT_Mu15 OR HLT_Mu15_v*' # run >= 147196

### Muon configuration

muonsUsePV     = False # use beam spot rather than PV, which is necessary for 'dB' cut
muonEmbedTrack = True  # embedded track needed for muon ID cuts

### Muon selection

# minimal selection, incl. also veto muons
# general reconstruction property
muonCut  =     'isGlobalMuon'
# transverse momentum
muonCut += ' && pt > 10.'
# pseudo-rapisity range
muonCut += ' && abs(eta) < 2.5'
# relative isolation
muonCut += ' && (trackIso+caloIso)/pt < 0.2'

# loose selection, on top of 'muonCut'
# general reconstruction property
looseMuonCut  =     'isTrackerMuon'
# transverse momentum
looseMuonCut += ' && pt > 20.'
# pseudo-rapisity range
looseMuonCut += ' && abs(eta) < 2.1'
# relative isolation
looseMuonCut += ' && (trackIso+caloIso)/pt < 0.1'
# muon ID: 'isGlobalMuonPromptTight'
looseMuonCut += ' && globalTrack.normalizedChi2 < 10.'
looseMuonCut += ' && globalTrack.hitPattern.numberOfValidMuonHits > 0'
# 2-dim impact parameter with respect to beam spot (s. "PAT muon configuration" above)
looseMuonCut += ' && abs(dB) < 0.02'
# tracker reconstruction
looseMuonCut += ' && innerTrack.numberOfValidHits > 10'
looseMuonCut += ' && innerTrack.hitPattern.pixelLayersWithMeasurement >= 1'
# muon chamber reconstruction
looseMuonCut += ' && numberOfMatches > 1'

## longitudinal distance of muon vertex from PV
#muonCut += ' && ()' # DeltaZ between muon vertex and PV < 1.
# minimum DeltaR from any selected jet
muonJetsDR = 0.3

# tight selection, on top of 'looseMuonCut'
tightMuonCut  = '(trackIso+caloIso)/pt < 0.05'

### Jet selection

# transverse momentum
jetCut  =     'pt > 30.'
# pseudo-rapisity range
jetCut += ' && abs(eta) < 2.4'
# jet ID: electro-magnetic energy fraction
jetCut += ' && emEnergyFraction > 0.01'
# jet ID: number of RecHits carying 90% of the total energy
jetCut += ' && jetID.n90Hits > 1'
# jet ID: fraction of energy in the hottest readout
jetCut += ' && jetID.fHPD < 0.98'


### Electron selection

# transverse momentum
electronCut  =     'et > 15.'
# pseudo-rapisity range
electronCut += ' && abs(eta) < 2.5'
# relative isolation
electronCut += ' && (dr03TkSumPt+dr03EcalRecHitSumEt+dr03HcalTowerSumEt)/et < 0.2'

### ------------------------------------------------------------------------ ###


### Trigger matching

# trigger object selection (corresponding to 'triggerSelection' above)
#triggerObjectSelection = 'type("TriggerMuon") && ( path("HLT_Mu9") )'                         # run <  147196
triggerObjectSelection = 'type("TriggerMuon") && ( path("HLT_Mu15") || path("HLT_Mu15_v*") )' # run >= 147196


### Basic

# GlobalTag identifier as in Configuration/PyReleaseValidation/python/autoCond.py
condition = 'com10'
# maximum number of events
maxInputEvents = -1 # reduce for testing
# input files
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
inputFiles = pickRelValInputFiles( cmsswVersion  = 'CMSSW_4_2_2'
                                 , relVal        = 'Mu'
                                 , dataTier      = 'RECO'
                                 , globalTag     = 'GR_R_42_V10_RelVal_mu2010B'
                                 , numberOfFiles = 0
                                 )
# event frequency of Fwk report
fwkReportEvery = 100
# switch for 'TrigReport'/'TimeReport' at job end
wantSummary = True


### ======================================================================== ###


###
### Basic configuration
###

process.load( "TopQuarkAnalysis.Configuration.patRefSel_basics_cff" )
process.MessageLogger.cerr.FwkReport.reportEvery = fwkReportEvery
process.options.wantSummary = wantSummary
from Configuration.PyReleaseValidation.autoCond import autoCond
process.GlobalTag.globaltag = autoCond[ condition ]


###
### Input configuration
###

process.load( "TopQuarkAnalysis.Configuration.patRefSel_inputModule_cff" )
process.source.fileNames = inputFiles
process.maxEvents.input  = maxInputEvents


###
### Output configuration
###

process.load( "TopQuarkAnalysis.Configuration.patRefSel_outputModule_cff" )
# output file name
process.out.fileName = 'patRefSel_muJets_mc.root'
# event content
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContent
process.out.outputCommands += patEventContent


###
### Cleaning and trigger selection configuration
###

### Event cleaning
process.load( 'TopQuarkAnalysis.Configuration.patRefSel_eventCleaning_cff' )

### Trigger selection
from TopQuarkAnalysis.Configuration.patRefSel_triggerSelection_cff import triggerResults
process.step1 = triggerResults.clone(
  throw             = False
, triggerConditions = [ triggerSelection ]
)

### Good vertex selection
from TopQuarkAnalysis.Configuration.patRefSel_goodVertex_cff import goodVertex
process.step2 = goodVertex.clone()


###
### PAT configuration
###

process.load( "PhysicsTools.PatAlgos.patSequences_cff" )
# remove MC matching, object cleaning and photons and taus
from PhysicsTools.PatAlgos.tools.coreTools import *
removeMCMatching( process )
removeCleaning( process )
removeSpecificPATObjects( process, [ 'Photons', 'Taus' ] )
# additional event content has to be added _after_ the call to 'removeCleaning()':
process.out.outputCommands += [ 'keep edmTriggerResults_*_*_*'
                              , 'keep *_hltTriggerSummaryAOD_*_*'
                              # tracks, vertices and beam spot
                              , 'keep *_offlineBeamSpot_*_*'
                              , 'keep *_offlinePrimaryVertices*_*_*'
                              ]


###
### Muons
###

process.patMuons.usePV      = muonsUsePV
process.patMuons.embedTrack = muonEmbedTrack

process.selectedPatMuons.cut = muonCut

process.step4 = process.countPatMuons.clone( maxNumber = 1 ) #

process.loosePatMuons = process.cleanPatMuons.clone(
  preselection = looseMuonCut
, checkOverlaps = cms.PSet(
    jets = cms.PSet(
      src                 = cms.InputTag( 'goodPatJets' )
    , algorithm           = cms.string( 'byDeltaR' )
    , preselection        = cms.string( '' )
    , deltaR              = cms.double( muonJetsDR )
    , checkRecoComponents = cms.bool( False )
    , pairCut             = cms.string( '' )
    , requireNoOverlaps   = cms.bool( True)
    )
  )
)
process.out.outputCommands.append( 'keep *_loosePatMuons_*_*' )

process.step3b = process.countPatMuons.clone( src = cms.InputTag( 'loosePatMuons' )
                                            , minNumber = 1
                                            , maxNumber = 1
                                            )

process.tightPatMuons = process.cleanPatMuons.clone(
  src          = cms.InputTag( 'loosePatMuons' )
, preselection = tightMuonCut
)
process.out.outputCommands.append( 'keep *_tightPatMuons_*_*' )

process.step3a = process.countPatMuons.clone( src = cms.InputTag( 'tightPatMuons' )
                                            , minNumber = 1
                                            , maxNumber = 1
                                            )


###
### Jets
###

process.goodPatJets = process.cleanPatJets.clone(
  preselection = jetCut
, checkOverlaps = cms.PSet()
)
process.out.outputCommands.append( 'keep *_goodPatJets_*_*' )

process.step6a = process.countPatJets.clone( minNumber = 1 )
process.step6b = process.countPatJets.clone( minNumber = 2 )
process.step6c = process.countPatJets.clone( minNumber = 3 )
process.step7  = process.countPatJets.clone( minNumber = 4 )


###
### Electrons
###

process.selectedPatElectrons.cut = electronCut

process.step5 = process.countPatElectrons.clone( maxNumber = 0 )


###
### Scheduling
###

# The additional sequence
process.patAddOnSequence = cms.Sequence(
  process.goodPatJets
* process.loosePatMuons
* process.tightPatMuons
)

# The path
process.p = cms.Path(
  process.eventCleaning
* process.step1
* process.step2
* process.patDefaultSequence
* process.patAddOnSequence
)
if useLooseMuon:
  process.p += process.step3b
if useTightMuon:
  process.p += process.step3a
if useMuonVeto:
  process.p += process.step4
if useElectronVeto:
  process.p += process.step5
if use1Jet:
  process.p += process.step6a
if use2Jets:
  process.p += process.step6b
if use3Jets:
  process.p += process.step6c
if use4Jets:
  process.p += process.step7


###
### Trigger matching
###

### Trigger matching configuration
from TopQuarkAnalysis.Configuration.patRefSel_triggerMatching_cfi import patMuonTriggerMatch
process.triggerMatch = patMuonTriggerMatch.clone( matchedCuts = triggerObjectSelection )

### Enabling trigger matching and embedding
from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTriggerMatchEmbedding( process, triggerMatchers = [ 'triggerMatch' ] )
# remove object cleaning as for the PAT default sequence
removeCleaningFromTriggerMatching( process )
switchOnTriggerMatchEmbedding( process, triggerMatchers = [ 'triggerMatch' ] ) # once more in order to fix event content
# adapt input sources for additional muon collections
process.loosePatMuons.src = cms.InputTag( 'selectedPatMuonsTriggerMatch' )
process.tightPatMuons.src = cms.InputTag( 'selectedPatMuonsTriggerMatch' )
