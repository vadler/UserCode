## import skeleton process
import os
from PhysicsTools.PatAlgos.patTemplate_cfg import *
from PhysicsTools.PatAlgos.tools.myTools import pickRelValInputFile
process.source.fileNames    = [ pickRelValInputFile() ]
process.maxEvents.input     = 10
process.out.fileName        = '%s/output/myPatTuple_addTriggerMatchesEarly.root'%( os.getenv( "CMSSW_BASE" ) )
process.options.wantSummary = False

## let it run
process.p = cms.Path(
  process.patCandidates
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
