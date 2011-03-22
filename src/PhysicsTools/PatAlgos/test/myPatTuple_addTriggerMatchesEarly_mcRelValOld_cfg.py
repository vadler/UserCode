## import skeleton process
import os
from PhysicsTools.PatAlgos.patTemplate_cfg import *
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *
process.source.fileNames    = pickRelValInputFiles( formerVersion = True
                                                  , globalTag     = 'START42_V4'
                                                  )
process.maxEvents.input     = 10
process.out.fileName        = '%s/output/myPatTuple_addTriggerMatchesEarly_mcRelValOld.root'%( os.getenv( "CMSSW_BASE" ) )
process.options.wantSummary = False


## let it run
process.p = cms.Path(
  process.patCandidates
)

## add trigger information to the configuration
from PhysicsTools.PatAlgos.triggerLayer1.triggerMatcher_cfi import *
process.muonTriggerMatchHLTMu20                                   = cleanMuonTriggerMatchHLTMu20.clone( src = 'patMuons' )
process.muonTriggerMatchHLTDoubleMu6                              = cleanMuonTriggerMatchHLTDoubleMu6.clone( src = 'patMuons' )
process.photonTriggerMatchHLTPhoton26IsoVLPhoton18                = cleanPhotonTriggerMatchHLTPhoton26IsoVLPhoton18.clone( src = 'patPhotons' )
process.electronTriggerMatchHLTEle27CaloIdVTCaloIsoTTrkIdTTrkIsoT = cleanElectronTriggerMatchHLTEle27CaloIdVTCaloIsoTTrkIdTTrkIsoT.clone( src = 'patElectrons' )
process.tauTriggerMatchHLTDoubleIsoPFTau20Trk5                    = cleanTauTriggerMatchHLTDoubleIsoPFTau20Trk5.clone( src = 'patTaus' )
process.jetTriggerMatchHLTJet240                                  = cleanJetTriggerMatchHLTJet240.clone( src = 'patJets' )
from PhysicsTools.PatAlgos.tools.trigTools import switchOnTriggerMatchEmbedding
switchOnTriggerMatchEmbedding( process
                             , [ 'muonTriggerMatchHLTMu20'
                               , 'muonTriggerMatchHLTDoubleMu6'
                               , 'photonTriggerMatchHLTPhoton26IsoVLPhoton18'
                               , 'electronTriggerMatchHLTEle27CaloIdVTCaloIsoTTrkIdTTrkIsoT'
                               , 'tauTriggerMatchHLTDoubleIsoPFTau20Trk5'
                               , 'jetTriggerMatchHLTJet240'
                               , 'metTriggerMatchHLTMET100'
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
