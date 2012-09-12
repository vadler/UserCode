## import skeleton process
import os
from PhysicsTools.PatAlgos.patTemplate_cfg import *

from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
process.source.fileNames    = pickRelValInputFiles( cmsswVersion  = 'CMSSW_6_0_0'
                                                  , relVal        = 'RelValProdTTbar'
                                                  , dataTier      = 'AODSIM'
                                                  , globalTag     = 'START60_V4'
                                                  , maxVersions   = 1
                                                  )
process.source.skipBadFiles = cms.untracked.bool( True )
process.GlobalTag.globaltag = 'START60_V5::All'
process.maxEvents.input     = 10
process.options.wantSummary = False
process.out.fileName        = '%s/output/myPatTuple_addTriggerMatchesEarly_mcRelValOld.root'%( os.getenv( "CMSSW_BASE" ) )


## let it run
process.load( "PhysicsTools.PatAlgos.patSequences_cff" )
process.p = cms.Path(
  process.patCandidates
)

## add trigger information to the configuration
from PhysicsTools.PatAlgos.triggerLayer1.triggerMatcher_cfi import *
process.muonTriggerMatchHLTMu17                                     = cleanMuonTriggerMatchHLTMu17.clone( src = 'patMuons' )
process.muonTriggerMatchHLTDoubleMu5IsoMu5                          = cleanMuonTriggerMatchHLTDoubleMu5IsoMu5.clone( src = 'patMuons' )
process.photonTriggerMatchHLTPhoton26Photon18                       = cleanPhotonTriggerMatchHLTPhoton26Photon18.clone( src = 'patPhotons' )
process.electronTriggerMatchHLTEle17CaloIdTCaloIsoVLTrkIdVLTrkIsoVL = cleanElectronTriggerMatchHLTEle17CaloIdTCaloIsoVLTrkIdVLTrkIsoVL.clone( src = 'patElectrons' )
process.tauTriggerMatchHLTDoubleMediumIsoPFTau30Trk1eta2p1          = cleanTauTriggerMatchHLTDoubleMediumIsoPFTau30Trk1eta2p1.clone( src = 'patTaus' )
process.jetTriggerMatchHLTPFJet40                                   = cleanJetTriggerMatchHLTPFJet40.clone( src = 'patJets' )
process.muonTriggerMatchHLTMu8DiJet30                               = cleanMuonTriggerMatchHLTMu8DiJet30.clone( src = 'patMuons' )
process.jetTriggerMatchHLTMu8DiJet30                                = cleanJetTriggerMatchHLTMu8DiJet30.clone( src = 'patJets' )
from PhysicsTools.PatAlgos.tools.trigTools import switchOnTriggerMatchEmbedding
switchOnTriggerMatchEmbedding( process
                             , [ 'muonTriggerMatchHLTMu17'
                               , 'muonTriggerMatchHLTDoubleMu5IsoMu5'
                               , 'photonTriggerMatchHLTPhoton26Photon18'
                               , 'electronTriggerMatchHLTEle17CaloIdTCaloIsoVLTrkIdVLTrkIsoVL'
                               , 'tauTriggerMatchHLTDoubleMediumIsoPFTau30Trk1eta2p1'
                               , 'jetTriggerMatchHLTPFJet40'
                               , 'metTriggerMatchHLTMET120'
                               , 'muonTriggerMatchHLTMu8DiJet30'
                               , 'jetTriggerMatchHLTMu8DiJet30'
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
print process.patCandidatesTrigger
print
print process.patCandidates
print
print process.p
print
