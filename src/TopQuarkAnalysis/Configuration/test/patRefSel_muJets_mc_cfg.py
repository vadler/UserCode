import FWCore.ParameterSet.Config as cms

process = cms.Process( 'PAT' )

###
### Constants
###

condition = 'startup'

###
### Basic configuration
###

process.load( "TopQuarkAnalysis.Configuration.patRefSel_basics_cff" )
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.options.wantSummary = True
from Configuration.PyReleaseValidation.autoCond import autoCond
process.GlobalTag.globaltag = autoCond[ condition ]

###
### Input configuration
###

process.load( "TopQuarkAnalysis.Configuration.patRefSel_inputModule_cff" )
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
process.source.fileNames = pickRelValInputFiles( cmsswVersion  = 'CMSSW_4_2_2'
                                               , relVal        = 'RelValTTbar'
                                               , condition     = condition
                                               , numberOfFiles = 0
                                               )
process.maxEvents.input = -1

###
### Output configuration
###

process.load( "TopQuarkAnalysis.Configuration.patRefSel_outputModule_cff" )
process.out.fileName = 'patRefSel_muJets_mc.root'
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContent
process.out.outputCommands += patEventContent

###
### Cleaning and trigger selection configuration
###

### Good vertex selection
process.load( 'TopQuarkAnalysis.Configuration.patRefSel_goodVertex_cff' )

### Trigger selection
process.load( 'TopQuarkAnalysis.Configuration.patRefSel_triggerSelection_cff' )
process.triggerResults.throw = False
process.triggerResults.triggerConditions = [ 'HLT_Mu15 OR HLT_Mu15_v*'
                                           ]

###
### PAT configuration
###

process.load( "PhysicsTools.PatAlgos.patSequences_cff" )
process.patJets.embedCaloTowers   = False
process.patJets.embedPFCandidates = False
# Remove object cleaning
from PhysicsTools.PatAlgos.tools.coreTools import removeCleaning
removeCleaning( process )
# Additional event content has to be added _after_ the call to 'removeCleaning()'
process.out.outputCommands += [ 'keep edmTriggerResults_*_*_*'
                              , 'keep *_hltTriggerSummaryAOD_*_*'
                              , 'keep recoTracks_generalTracks*_*_*'
                              , 'keep *_offlineBeamSpot_*_*'
                              , 'keep *_offlinePrimaryVertices*_*_*'
                              , 'keep GenEventInfoProduct_*_*_*'
                              , 'keep recoGenParticles_*_*_*'
                              ,]

###
### Muon selection
###

process.patMuons.usePV = False # necessary for 'dB' cut
muonCut  =     '(isGlobalMuon && isTrackerMuon)'
muonCut += ' && pt > 20.'
muonCut += ' && abs(eta) < 2.1'
muonCut += ' && (trackIso+ecalIso+hcalIso)/pt < 0.05' # < 0.05 or < 0.1
muonCut += ' && globalTrack.normalizedChi2 < 10.'
muonCut += ' && globalTrack.hitPattern.numberOfValidMuonHits > 0'
muonCut += ' && innerTrack.numberOfValidHits > 10'
#muonCut += ' && ()' # DeltaR to any selected jet < 0.3
muonCut += ' && dB < 0.02'
#muonCut += ' && ()' # DeltaZ between muon vertex and PV < 1.
muonCut += ' && innerTrack.hitPattern.pixelLayersWithMeasurement >= 1'
muonCut += ' && numberOfMatches > 1'
process.selectedPatMuons.cut = muonCut
process.countPatMuons.minNumber = 1
process.countPatMuons.maxNumber = 1

###
### Jet selection
###

jetCut  =     'pt > 30.'
jetCut += ' && abs(eta) < 2.4'
jetCut += ' && emEnergyFraction > 0.01'
jetCut += ' && jetID.n90Hits > 1'
jetCut += ' && jetID.fHPD < 0.98'
process.selectedPatJets.cut = jetCut
process.countPatJets.minNumber = 1 # 1, 2, 3

###
### Additional muon veto
###

process.selectedPatVetoMuons = process.selectedPatMuons.clone()
process.out.outputCommands.append( 'keep *_selectedPatVetoMuons*_*_*' )
process.selectedPatCandidateSummary.candidates.append( cms.InputTag( 'selectedPatVetoMuons' ) )
muonVetoCut  =     'isGlobalMuon'
muonVetoCut += ' && pt > 10.'
muonVetoCut += ' && abs(eta) < 2.5'
muonVetoCut += ' && (trackIso+ecalIso+hcalIso)/pt < 0.2'
process.selectedPatVetoMuons.cut = '(' + muonVetoCut + ') && !(' + muonCut + ')'
process.countPatVetoMuons = process.countPatMuons.clone()
process.countPatVetoMuons.src = cms.InputTag( 'selectedPatVetoMuons' )
process.countPatVetoMuons.minNumber = 0
process.countPatVetoMuons.maxNumber = 0

###
### Electron veto
###

process.selectedPatVetoElectrons = process.selectedPatElectrons.clone()
process.out.outputCommands.append( 'keep *_selectedPatVetoElectrons*_*_*' )
process.selectedPatCandidateSummary.candidates.append( cms.InputTag( 'selectedPatVetoElectrons' ) )
electronVetoCut  =     'et > 15.'
electronVetoCut += ' && abs(eta) < 2.5'
electronVetoCut += ' && (dr03TkSumPt+dr03EcalRecHitSumEt+dr03HcalTowerSumEt)/et < 0.2'
process.selectedPatVetoElectrons.cut = electronVetoCut
process.countPatVetoElectrons = process.countPatElectrons.clone()
process.countPatVetoElectrons.src = cms.InputTag( 'selectedPatVetoElectrons' )
process.countPatVetoElectrons.minNumber = 0
process.countPatVetoElectrons.maxNumber = 0

process.vetoPatCandidateSummary = process.selectedPatCandidateSummary.clone()
process.vetoPatCandidateSummary.logName = 'vetoPatCandidates|PATSummaryTables'
process.vetoPatCandidateSummary.candidates = cms.VInputTag( cms.InputTag( 'selectedPatVetoMuons' )
                                                          , cms.InputTag( 'selectedPatVetoElectrons' )
                                                          )

###
### Scheduling
###

process.vetoPatCandidates = cms.Sequence(
  process.selectedPatVetoMuons
* process.countPatVetoMuons
* process.selectedPatVetoElectrons
* process.countPatVetoElectrons
* process.vetoPatCandidateSummary
)

process.p = cms.Path(
  process.triggerSelection
* process.goodVertex
* process.patDefaultSequence
* process.vetoPatCandidates
)

###
### Trigger matching
###

from TopQuarkAnalysis.Configuration.patRefSel_triggerMatching_cfi import patMuonTriggerMatch
process.triggerMatch = patMuonTriggerMatch.clone()
process.triggerMatch.matchedCuts = 'type("TriggerMuon") && ( path("HLT_Mu15") || path("HLT_Mu15_v*") )'
process.triggerMatch.maxDPtRel = 0.5
process.triggerMatch.maxDeltaR = 0.5
process.triggerMatch.resolveAmbiguities    = True
process.triggerMatch.resolveByMatchQuality = True
from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTriggerMatchEmbedding( process, triggerMatchers = [ 'triggerMatch' ] )
removeCleaningFromTriggerMatching( process )
switchOnTriggerMatchEmbedding( process, triggerMatchers = [ 'triggerMatch' ] ) # once more in order to fix event content
