import FWCore.ParameterSet.Config as cms

#
# define derivations from std pat defaults
#

## object cleaning
from PhysicsTools.PatAlgos.cleaningLayer0.electronCleaner_cfi import allLayer0Electrons

allLayer0Electrons.isolation.tracker = cms.PSet(
    src    = cms.InputTag("patAODElectronIsolations", "eleIsoDepositTk"),
    deltaR = cms.double(0.3),
    cut    = cms.double(3.0)
    )
allLayer0Electrons.isolation.ecal    = cms.PSet(
    src    = cms.InputTag("patAODElectronIsolations", "eleIsoDepositEcalFromClusts"),
   #src    = cms.InputTag("patAODElectronIsolations","eleIsoDepositEcalFromHits"),
    deltaR = cms.double(0.4),
    cut    = cms.double(5.0)
    )
allLayer0Electrons.isolation.hcal    = cms.PSet(
    src    = cms.InputTag("patAODElectronIsolations", "eleIsoDepositHcalFromTowers"),
   #src    = cms.InputTag("patAODElectronIsolations","eleIsoDepositHcalFromHits"),
    deltaR = cms.double(0.4),
    cut    = cms.double(5.0)
    )


## mc matching
from PhysicsTools.PatAlgos.mcMatchLayer0.jetMatch_cfi import jetPartonMatch

jetPartonMatch.mcStatus = [3]


## production
from PhysicsTools.PatAlgos.producersLayer1.electronProducer_cfi import allLayer1Electrons

allLayer1Electrons.embedGsfTrack     = True 
allLayer1Electrons.embedSuperCluster = True
allLayer1Electrons.isoDeposits       = cms.PSet(
    tracker= cms.InputTag("layer0ElectronIsolations", "eleIsoDepositTk"),
    ecal   = cms.InputTag("layer0ElectronIsolations", "eleIsoDepositEcalFromClusts"),
   #ecal   = cms.InputTag("layer0ElectronIsolations", "eleIsoDepositEcalFromHits"),
    hcal   = cms.InputTag("layer0ElectronIsolations", "eleIsoDepositHcalFromTowers")
   #hcal   = cms.InputTag("layer0ElectronIsolations", "eleIsoDepositHcalFromHits")
    )
allLayer1Electrons.addTrigMatch = True
allLayer1Electrons.trigPrimMatch = cms.VInputTag(
    cms.InputTag("electronTrigMatchHLTIsoEle15LWL1I"), 
    cms.InputTag("electronTrigMatchHLTEle15LWL1R"), 
    cms.InputTag("electronTrigMatchHLTDoubleIsoEle10LWL1I"), 
    cms.InputTag("electronTrigMatchHLTDoubleEle5SWL1R")
    )

from PhysicsTools.PatAlgos.producersLayer1.muonProducer_cfi import allLayer1Muons

allLayer1Muons.embedCombinedMuon     = True
allLayer1Muons.embedStandAloneMuon   = True
allLayer1Muons.addTrigMatch = True
allLayer1Muons.trigPrimMatch = cms.VInputTag(
    cms.InputTag("muonTrigMatchHLTIsoMu11"), 
    cms.InputTag("muonTrigMatchHLTMu11"), 
    cms.InputTag("muonTrigMatchHLTDoubleIsoMu3"), 
    cms.InputTag("muonTrigMatchHLTDoubleMu3")
    )

from PhysicsTools.PatAlgos.producersLayer1.jetProducer_cfi import allLayer1Jets

allLayer1Jets.embedCaloTowers        = False
allLayer1Jets.addTagInfoRefs         = True
allLayer1Jets.addTrigMatch = False

from PhysicsTools.PatAlgos.producersLayer1.tauProducer_cfi import allLayer1Taus

allLayer1Taus.addTrigMatch = True
allLayer1Taus.trigPrimMatch = cms.VInputTag(
    cms.InputTag("tauTrigMatchHLTLooseIsoTauMET30L1MET"), 
    cms.InputTag("tauTrigMatchHLTDoubleIsoTauTrk3")
    )

from PhysicsTools.PatAlgos.producersLayer1.metProducer_cfi import allLayer1METs

allLayer1METs.addTrigMatch = False

from PhysicsTools.PatAlgos.producersLayer1.photonProducer_cfi import allLayer1Photons

allLayer1Photons.addTrigMatch = False

## selection
from PhysicsTools.PatAlgos.selectionLayer1.electronSelector_cfi import selectedLayer1Electrons

selectedLayer1Electrons.src = cms.InputTag("allLayer1Electrons")
selectedLayer1Electrons.cut = cms.string('pt > 0.')

from PhysicsTools.PatAlgos.selectionLayer1.muonSelector_cfi import selectedLayer1Muons

selectedLayer1Muons.src     = cms.InputTag("allLayer1Muons")
selectedLayer1Muons.cut     = cms.string('pt > 0.')

from PhysicsTools.PatAlgos.selectionLayer1.tauSelector_cfi import selectedLayer1Taus

selectedLayer1Taus.src      = cms.InputTag("allLayer1Taus")
selectedLayer1Taus.cut      = cms.string('pt > 10.')

from PhysicsTools.PatAlgos.selectionLayer1.jetSelector_cfi import selectedLayer1Jets

selectedLayer1Jets.src      = cms.InputTag("allLayer1Jets")
selectedLayer1Jets.cut      = cms.string('et > 15. & nConstituents > 0')

from PhysicsTools.PatAlgos.selectionLayer1.metSelector_cfi import selectedLayer1METs

selectedLayer1METs.src      = cms.InputTag("allLayer1METs")
selectedLayer1METs.cut      = cms.string('et >= 0.')
