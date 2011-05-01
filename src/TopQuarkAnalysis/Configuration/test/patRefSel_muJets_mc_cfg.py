import FWCore.ParameterSet.Config as cms

process = cms.Process( 'PAT' )

###
### Constants
###

### Basic

# GlobalTag identifier as in Configuration/PyReleaseValidation/python/autoCond.py
condition = 'startup'
# maximum number of events
maxInputEvents = -1 # reduce for testing
# input files
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
inputFiles = pickRelValInputFiles( cmsswVersion  = 'CMSSW_4_2_2'
                                 , relVal        = 'RelValTTbar'
                                 , condition     = condition
                                 , numberOfFiles = 0
                                 )
# event frequency of Fwk report
fwkReportEvery = 100
# switch for 'TrigReport'/'TimeReport' at job end
wantSummary = True


### ------------------------- Reference selection -------------------------- ###

### Trigger selection

# logical expression
triggerSelection = 'HLT_Mu15 OR HLT_Mu15_v*'

### Muon selection

# muon configuration
muonsUsePV = False # use beam spot rather than PV, which is necessary for 'dB' cut
# general reconstruction property
muonCut  =     '(isGlobalMuon && isTrackerMuon)'
# transverse momentum
muonCut += ' && pt > 20.'
# pseudo-rapisity range
muonCut += ' && abs(eta) < 2.1'
# relative isolation
muonCut += ' && (trackIso+ecalIso+hcalIso)/pt < 0.05' # RelIso for l+jets
#muonCut += ' && (trackIso+ecalIso+hcalIso)/pt < 0.1'  # RelIso for single top
# muon ID: 'isGlobalMuonPromptTight'
muonCut += ' && globalTrack.normalizedChi2 < 10.'
muonCut += ' && globalTrack.hitPattern.numberOfValidMuonHits > 0'
## distance to selected jets
#muonCut += ' && ()' # DeltaR to any selected jet < 0.3
# 2-dim impact parameter with respect to beam spot (s. "PAT muon configuration" above)
muonCut += ' && dB < 0.02'
## longitudinal distance of muon vertex from PV
#muonCut += ' && ()' # DeltaZ between muon vertex and PV < 1.
# tracker reconstruction
muonCut += ' && innerTrack.numberOfValidHits > 10'
muonCut += ' && innerTrack.hitPattern.pixelLayersWithMeasurement >= 1'
# muon chamber reconstruction
muonCut += ' && numberOfMatches > 1'
# exactly 1 muon selected
minMuons = 1
maxMuons = 1

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
# minimum number of selected jets
minJets = 1 # 1, 2, 3 or 4

### Second loose muon veto selection

# general reconstruction property
muonVetoCut  =     'isGlobalMuon'
# transverse momentum
muonVetoCut += ' && pt > 10.'
# pseudo-rapisity range
muonVetoCut += ' && abs(eta) < 2.5'
# relative isolation
muonVetoCut += ' && (trackIso+ecalIso+hcalIso)/pt < 0.2'
# NO veto muon found
maxVetoMuons = 0 # increase to keep vetoed events

### Electron veto selection

# transverse momentum
electronVetoCut  =     'et > 15.'
# pseudo-rapisity range
electronVetoCut += ' && abs(eta) < 2.5'
# relative isolation
electronVetoCut += ' && (dr03TkSumPt+dr03EcalRecHitSumEt+dr03HcalTowerSumEt)/et < 0.2'
# NO veto electron found
maxVetoElectrons = 0 # increase to keep vetoed events

### ------------------------------------------------------------------------ ###


### Trigger matching

# trigger object selection (corresponding to 'triggerSelection' above)
triggerObjectSelection = 'type("TriggerMuon") && ( path("HLT_Mu15") || path("HLT_Mu15_v*") )'


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

### Good vertex selection
process.load( 'TopQuarkAnalysis.Configuration.patRefSel_goodVertex_cff' )

### Trigger selection
process.load( 'TopQuarkAnalysis.Configuration.patRefSel_triggerSelection_cff' )
# supress exception on unknown trigger names
process.triggerResults.throw = False
# HLT selection
process.triggerResults.triggerConditions = [ triggerSelection ]


###
### PAT configuration
###

process.load( "PhysicsTools.PatAlgos.patSequences_cff" )
process.patJets.embedCaloTowers   = False
process.patJets.embedPFCandidates = False
# remove object cleaning
from PhysicsTools.PatAlgos.tools.coreTools import removeCleaning
removeCleaning( process )
# additional event content has to be added _after_ the call to 'removeCleaning()':
process.out.outputCommands += [ 'keep edmTriggerResults_*_*_*'
                              , 'keep *_hltTriggerSummaryAOD_*_*'
                              # tracks, vertices and beam spot
                              , 'keep recoTracks_generalTracks*_*_*'
                              , 'keep *_offlineBeamSpot_*_*'
                              , 'keep *_offlinePrimaryVertices*_*_*'
                              # MC generator info
                              , 'keep GenEventInfoProduct_*_*_*'
                              , 'keep recoGenParticles_*_*_*'
                              ,]


###
### Muon selection
###

### PAT muon configuration
process.patMuons.usePV = muonsUsePV
process.selectedPatMuons.cut = muonCut

### PAT muon counting
process.countPatMuons.minNumber = minMuons
process.countPatMuons.maxNumber = maxMuons


###
### Jet selection
###

### PAT jet configuration
process.selectedPatJets.cut = jetCut

### PAT jet counting
process.countPatJets.minNumber = minJets


###
### Second loose muon veto selection
###

### PAT muon configuration for veto
process.selectedPatVetoMuons = process.selectedPatMuons.clone( cut = '(' + muonVetoCut + ') && !(' + muonCut + ')' )
# event content
process.out.outputCommands.append( 'keep *_selectedPatVetoMuons*_*_*' )

### PAT muon counting for veto
process.countPatVetoMuons = process.countPatMuons.clone( src = cms.InputTag( 'selectedPatVetoMuons' )
                                                       , minNumber = 0
                                                       , maxNumber = maxVetoMuons
                                                       )


###
### Electron veto selection
###

### PAT electron configuration for veto
process.selectedPatVetoElectrons = process.selectedPatElectrons.clone( cut = electronVetoCut )
# event content
process.out.outputCommands.append( 'keep *_selectedPatVetoElectrons*_*_*' )

### PAT electron counting for veto
process.countPatVetoElectrons = process.countPatElectrons.clone( src = cms.InputTag( 'selectedPatVetoElectrons' )
                                                               , minNumber = 0
                                                               , maxNumber = maxVetoElectrons
                                                               )


###
### Veto summary
###

process.vetoPatCandidateSummary = process.selectedPatCandidateSummary.clone( logName    = 'vetoPatCandidates|PATSummaryTables'
                                                                           , candidates = cms.VInputTag( cms.InputTag( 'selectedPatVetoMuons' )
                                                                                                       , cms.InputTag( 'selectedPatVetoElectrons' )
                                                                                                       )
                                                                           )


###
### Scheduling
###

### Veto sequence
process.vetoPatCandidates = cms.Sequence(
  process.selectedPatVetoMuons
* process.countPatVetoMuons
* process.selectedPatVetoElectrons
* process.countPatVetoElectrons
* process.vetoPatCandidateSummary
)

### The path
process.p = cms.Path(
  # cleaning and trigger selection
  process.triggerSelection
* process.goodVertex
  # PAT, incl. muon and jets selection
* process.patDefaultSequence
  # veto sequence
* process.vetoPatCandidates
)


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
