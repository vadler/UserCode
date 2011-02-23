## import skeleton process
import os
from PhysicsTools.PatAlgos.patTemplate_cfg import *
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *
process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )
process.source.fileNames    = pickRelValInputFiles( cmsswVersion = 'CMSSW_4_2_0_pre5'
                                                  , globalTag    = 'START42_V3'
                                                  )
process.maxEvents.input     = 10
process.out.fileName        = '%s/output/myPatTuple_addTriggerMatchesEarly_mcRelValOld.root'%( os.getenv( "CMSSW_BASE" ) )
process.options.wantSummary = False

# Internediate step to get the taus working in CMSSW_3_10_X RelVals
process.load("RecoTauTag.Configuration.RecoPFTauTag_cff")
process.p0 = cms.Path(
  process.PFTau
)

## let it run
process.p = cms.Path(
  process.patCandidates
)
process.patTaus.tauIDSources = cms.PSet(
        leadingTrackFinding = cms.InputTag("shrinkingConePFTauDiscriminationByLeadingTrackFinding"),
        #leadingTrackPtCut = cms.InputTag("shrinkingConePFTauDiscriminationByLeadingTrackPtCut"),
        leadingPionPtCut = cms.InputTag("shrinkingConePFTauDiscriminationByLeadingPionPtCut"),
        #trackIsolation = cms.InputTag("shrinkingConePFTauDiscriminationByTrackIsolation"),
        #trackIsolationUsingLeadingPion = cms.InputTag("shrinkingConePFTauDiscriminationByTrackIsolationUsingLeadingPion"),
        #ecalIsolation = cms.InputTag("shrinkingConePFTauDiscriminationByECALIsolation"),
        #ecalIsolationUsingLeadingPion = cms.InputTag("shrinkingConePFTauDiscriminationByECALIsolationUsingLeadingPion"),
        #byIsolation = cms.InputTag("shrinkingConePFTauDiscriminationByIsolation"),
        byIsolationUsingLeadingPion = cms.InputTag("shrinkingConePFTauDiscriminationByIsolationUsingLeadingPion"),
        againstElectron = cms.InputTag("shrinkingConePFTauDiscriminationAgainstElectron"),
        againstMuon = cms.InputTag("shrinkingConePFTauDiscriminationAgainstMuon")#,
        #byTaNC = cms.InputTag("shrinkingConePFTauDiscriminationByTaNC"),
        #byTaNCfrOnePercent = cms.InputTag("shrinkingConePFTauDiscriminationByTaNCfrOnePercent"),
        #byTaNCfrHalfPercent = cms.InputTag("shrinkingConePFTauDiscriminationByTaNCfrHalfPercent"),
        #byTaNCfrQuarterPercent = cms.InputTag("shrinkingConePFTauDiscriminationByTaNCfrQuarterPercent"),
        #byTaNCfrTenthPercent = cms.InputTag("shrinkingConePFTauDiscriminationByTaNCfrTenthPercent")
    )

## add trigger information to the configuration
from PhysicsTools.PatAlgos.triggerLayer1.triggerMatcher_cfi import *
process.muonTriggerMatchHLTMu9                  = cleanMuonTriggerMatchHLTMu9.clone( src = 'patMuons' )
process.muonTriggerMatchHLTDoubleIsoMu3         = cleanMuonTriggerMatchHLTDoubleIsoMu3.clone( src = 'patMuons' )
process.photonTriggerMatchHLTPhoton20CleanedL1R = cleanPhotonTriggerMatchHLTPhoton20CleanedL1R.clone( src = 'patPhotons' )
process.electronTriggerMatchHLTEle20SWL1R       = cleanElectronTriggerMatchHLTEle20SWL1R.clone( src = 'patElectrons' )
process.tauTriggerMatchHLTDoubleLooseIsoTau15   = cleanTauTriggerMatchHLTDoubleLooseIsoTau15.clone( src = 'patTaus' )
process.jetTriggerMatchHLTJet15U                = cleanJetTriggerMatchHLTJet15U.clone( src = 'patJets' )
from PhysicsTools.PatAlgos.tools.trigTools import switchOnTriggerMatchEmbedding
switchOnTriggerMatchEmbedding( process
                             , [ 'muonTriggerMatchHLTMu9'
                               , 'muonTriggerMatchHLTDoubleIsoMu3'
                               , 'photonTriggerMatchHLTPhoton20CleanedL1R'
                               , 'electronTriggerMatchHLTEle20SWL1R'
                               , 'tauTriggerMatchHLTDoubleLooseIsoTau15'
                               , 'jetTriggerMatchHLTJet15U'
                               , 'metTriggerMatchHLTMET45'
                               ]
                             , sequence = 'patCandidates'
                             , outputModule = ''
                             )
process.selectedPatMuons.src     = 'patMuonsTriggerMatch'
process.selectedPatPhotons.src   = 'patPhotonsTriggerMatch'
process.selectedPatElectrons.src = 'patElectronsTriggerMatch'
process.selectedPatTaus.src      = 'patTausTriggerMatch'
process.selectedPatJets.src      = 'patJetsTriggerMatch'
# special for MET
process.out.outputCommands += [ 'drop *_patMETs_*_*'
                              , 'keep *_patMETsTriggerMatch_*_*'
                              ]

process.p *= process.selectedPatCandidates
process.p *= process.cleanPatCandidates
process.p *= process.countPatCandidates
print
print process.patTriggerSequence
print
print process.patCandidates
print
print process.p
print
