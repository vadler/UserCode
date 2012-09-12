import os
from PhysicsTools.PatAlgos.patTemplate_cfg import *

# Steering
cmsswVersion = 'CMSSW_6_0_0'
globalTag    = 'START60_V4'

## Options
process.options.wantSummary      = False
process.options.allowUnscheduled = cms.untracked.bool( False )

## Messaging
#process.Tracer = cms.Service( "Tracer" )

## Input
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
process.source.fileNames    = pickRelValInputFiles( cmsswVersion  = cmsswVersion
                                                  , relVal        = 'RelValProdTTbar'
                                                  , dataTier      = 'AODSIM'
                                                  , globalTag     = globalTag
                                                  , maxVersions   = 1
                                                  )
process.source.skipBadFiles = cms.untracked.bool( True )
process.maxEvents.input     = 10

## Output
process.out.fileName = '%s/output/myPatTuple_addTriggerMatchesEarly_mcRelValFormer.root'%( os.getenv( "CMSSW_BASE" ) )

## Processing
process.load( "PhysicsTools.PatAlgos.patSequences_cff" )
process.p = cms.Path(
  process.patCandidates
)

# Trigger
from PhysicsTools.PatAlgos.triggerLayer1.triggerMatcher_cfi import *
process.muonTriggerMatchHLTMu17                                     = selectedMuonTriggerMatchHLTMu17.clone( src = 'patMuons' )
process.muonTriggerMatchHLTDoubleMu5IsoMu5                          = selectedMuonTriggerMatchHLTDoubleMu5IsoMu5.clone( src = 'patMuons' )
process.photonTriggerMatchHLTPhoton26Photon18                       = selectedPhotonTriggerMatchHLTPhoton26Photon18.clone( src = 'patPhotons' )
process.electronTriggerMatchHLTEle17CaloIdTCaloIsoVLTrkIdVLTrkIsoVL = selectedElectronTriggerMatchHLTEle17CaloIdTCaloIsoVLTrkIdVLTrkIsoVL.clone( src = 'patElectrons' )
process.tauTriggerMatchHLTDoubleMediumIsoPFTau30Trk1eta2p1          = selectedTauTriggerMatchHLTDoubleMediumIsoPFTau30Trk1eta2p1.clone( src = 'patTaus' )
process.jetTriggerMatchHLTPFJet40                                   = selectedJetTriggerMatchHLTPFJet40.clone( src = 'patJets' )
process.muonTriggerMatchHLTMu8DiJet30                               = selectedMuonTriggerMatchHLTMu8DiJet30.clone( src = 'patMuons' )
process.jetTriggerMatchHLTMu8DiJet30                                = selectedJetTriggerMatchHLTMu8DiJet30.clone( src = 'patJets' )
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
print
print process.patCandidatesTrigger
print
print process.patCandidates
print
print process.p
print
