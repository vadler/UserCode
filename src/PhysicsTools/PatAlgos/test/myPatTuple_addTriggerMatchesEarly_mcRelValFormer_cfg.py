import os
from PhysicsTools.PatAlgos.patTemplate_cfg import *

# Steering
cmsswVersion = 'CMSSW_6_1_0_pre5'
globalTag    = 'START61_V4'

# only for transaition from 610p2 to 610p3:
process.GlobalTag.globaltag = '%s::All'%( globalTag )

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
process.out.outputCommands.append( 'keep edmTriggerResults_TriggerResults_*_*' )
process.out.outputCommands.append( 'keep *_hltTriggerSummaryAOD_*_*' )

## Processing
process.load( "PhysicsTools.PatAlgos.patSequences_cff" )
process.p = cms.Path(
  process.patCandidates
)

# Trigger
from PhysicsTools.PatAlgos.triggerLayer1.triggerMatcher_cfi import *
process.patMuonTriggerMatchHLTMu17                                     = somePatMuonTriggerMatchHLTMu17.clone( src = 'patMuons' )
process.patMuonTriggerMatchHLTDoubleMu5IsoMu5                          = somePatMuonTriggerMatchHLTDoubleMu5IsoMu5.clone( src = 'patMuons' )
process.patPhotonTriggerMatchHLTPhoton26Photon18                       = somePatPhotonTriggerMatchHLTPhoton26Photon18.clone( src = 'patPhotons' )
process.patElectronTriggerMatchHLTEle17CaloIdTCaloIsoVLTrkIdVLTrkIsoVL = somePatElectronTriggerMatchHLTEle17CaloIdTCaloIsoVLTrkIdVLTrkIsoVL.clone( src = 'patElectrons' )
process.patTauTriggerMatchHLTDoubleMediumIsoPFTau30Trk1eta2p1          = somePatTauTriggerMatchHLTDoubleMediumIsoPFTau30Trk1eta2p1.clone( src = 'patTaus' )
process.patJetTriggerMatchHLTPFJet40                                   = somePatJetTriggerMatchHLTPFJet40.clone( src = 'patJets' )
process.patMetTriggerMatchHLTMET120                                       = somePatMetTriggerMatchHLTMET120.clone()
process.patMuonTriggerMatchHLTMu8DiJet30                               = somePatMuonTriggerMatchHLTMu8DiJet30.clone( src = 'patMuons' )
process.patJetTriggerMatchHLTMu8DiJet30                                = somePatJetTriggerMatchHLTMu8DiJet30.clone( src = 'patJets' )
from PhysicsTools.PatAlgos.tools.trigTools import switchOnTriggerMatchEmbedding
switchOnTriggerMatchEmbedding( process
                             , [ 'patMuonTriggerMatchHLTMu17'
                               , 'patMuonTriggerMatchHLTDoubleMu5IsoMu5'
                               , 'patPhotonTriggerMatchHLTPhoton26Photon18'
                               , 'patElectronTriggerMatchHLTEle17CaloIdTCaloIsoVLTrkIdVLTrkIsoVL'
                               , 'patTauTriggerMatchHLTDoubleMediumIsoPFTau30Trk1eta2p1'
                               , 'patJetTriggerMatchHLTPFJet40'
                               , 'patMetTriggerMatchHLTMET120'
                               , 'patMuonTriggerMatchHLTMu8DiJet30'
                               , 'patJetTriggerMatchHLTMu8DiJet30'
                               ]
                             , sequence = 'patCandidates'
                             , outputModule = ''
                             )
process.selectedPatMuons.src     = 'patMuonsTriggerMatch'
process.selectedPatPhotons.src   = 'patPhotonsTriggerMatch'
process.selectedPatElectrons.src = 'patElectronsTriggerMatch'
process.selectedPatTaus.src      = 'patTausTriggerMatch'
process.selectedPatJets.src      = 'patJetsTriggerMatch'

process.p *= process.selectedPatCandidates
print
print process.patCandidatesTrigger
print
print process.patCandidates
print
print process.p
print
