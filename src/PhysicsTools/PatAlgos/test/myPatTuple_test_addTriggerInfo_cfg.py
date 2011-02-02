import os
cmsswBase = os.getenv( "CMSSW_BASE" )

from PhysicsTools.PatAlgos.patTemplate_cfg import *
from Configuration.PyReleaseValidation.autoCond import autoCond
process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )
process.source.fileNames    = [ 'rfio:/castor/cern.ch/user/v/vadler/cms/PAT/data/ZJetToMuMu_Pt_15to3000_TuneZ2_Flat_7TeV_pythia6__Fall10-E7TeV_ProbDist_2010Data_BX156_START38_V12-v1__GEN-SIM-RECO/FED5121F-B1E4-DF11-B53F-00261834B580.root' ]
process.out.fileName        = '%s/output/myPatTuple_test_addTriggerInfo.root'%( cmsswBase )
# process.MessageLogger.cerr.threshold = 'INFO'
# process.MessageLogger.categories.append( 'autoProcessName' )
# process.MessageLogger.cerr.INFO = cms.untracked.PSet(
#   limit = cms.untracked.int32( -1 )
# )

process.p = cms.Path(
  process.patDefaultSequence
)

# Trigger Test
process.patTrigger = cms.EDProducer(
  "PATTriggerProducer"
, processName    = cms.string( '*' )
  ## L1
, addL1Algos     = cms.bool( True )
, l1ExtraMu      = cms.InputTag( 'l1extraParticles', '' )
, l1ExtraNoIsoEG = cms.InputTag( 'l1extraParticles', 'NonIsolated' )
, l1ExtraIsoEG   = cms.InputTag( 'l1extraParticles', 'Isolated' )
, l1ExtraCenJet  = cms.InputTag( 'l1extraParticles', 'Central' )
, l1ExtraForJet  = cms.InputTag( 'l1extraParticles', 'Forward' )
, l1ExtraTauJet  = cms.InputTag( 'l1extraParticles', 'Tau' )
, l1ExtraETM     = cms.InputTag( 'l1extraParticles', 'MET' )
, l1ExtraHTM     = cms.InputTag( 'l1extraParticles', 'MHT' )
, saveL1Refs     = cms.bool( True )
  ## HLT (L3)
, triggerResults = cms.InputTag( "TriggerResults" )
, triggerEvent   = cms.InputTag( "hltTriggerSummaryAOD" )
, onlyStandAlone = cms.bool( False )
  # trigger paths
, addPathModuleLabels = cms.bool( False ) # setting this 'True' stores the names of all modules as strings (~8kB/ev.)
)
process.electronTriggerTestMatchHLTElectrons = cms.EDProducer(
  "PATTriggerMatcherDRLessByR"
, src     = cms.InputTag( "cleanPatElectrons" )
, matched = cms.InputTag( "patTrigger" )
, matchedCuts = cms.string( 'type( "TriggerElectron" )' )
, maxDPtRel = cms.double( 0.5 )
, maxDeltaR = cms.double( 0.5 )
, resolveAmbiguities    = cms.bool( True )
, resolveByMatchQuality = cms.bool( False )
)
process.electronTriggerTestMatchHLTFilterEGammas = cms.EDProducer(
  "PATTriggerMatcherDRLessByR"
, src     = cms.InputTag( "cleanPatElectrons" )
, matched = cms.InputTag( "patTrigger" )
, matchedCuts = cms.string( 'filter( "hltL1sL1*EG*" )' )
, maxDPtRel = cms.double( 0.5 )
, maxDeltaR = cms.double( 0.5 )
, resolveAmbiguities    = cms.bool( True )
, resolveByMatchQuality = cms.bool( False )
)
process.muonTriggerTestMatchL1Muons = cms.EDProducer(
  "PATTriggerMatcherDRLessByR"
, src     = cms.InputTag( "cleanPatMuons" )
, matched = cms.InputTag( "patTrigger" )
, matchedCuts = cms.string( 'coll( "l1extraParticles" )' )
, maxDPtRel = cms.double( 0.5 )
, maxDeltaR = cms.double( 0.5 )
, resolveAmbiguities    = cms.bool( True )
, resolveByMatchQuality = cms.bool( False )
)
process.muonTriggerTestMatchL1FilterLabelMuons = cms.EDProducer(
  "PATTriggerMatcherDRLessByR"
, src     = cms.InputTag( "cleanPatMuons" )
, matched = cms.InputTag( "patTrigger" )
, matchedCuts = cms.string( 'filter( "hltL1SingleMu7L1Filtered0" )' )
, maxDPtRel = cms.double( 0.5 )
, maxDeltaR = cms.double( 0.5 )
, resolveAmbiguities    = cms.bool( True )
, resolveByMatchQuality = cms.bool( False )
)
process.muonTriggerTestMatchNoMuons = cms.EDProducer(
  "PATTriggerMatcherDRLessByR"
, src     = cms.InputTag( "cleanPatMuons" )
, matched = cms.InputTag( "patTrigger" )
, matchedCuts = cms.string( 'path( "someFunnyPath" )' )
, maxDPtRel = cms.double( 0.5 )
, maxDeltaR = cms.double( 0.5 )
, resolveAmbiguities    = cms.bool( True )
, resolveByMatchQuality = cms.bool( False )
)
process.jetTriggerTestMatchHLTJet15U = cms.EDProducer(
  "PATTriggerMatcherDRLessByR"
, src     = cms.InputTag( "cleanPatJets" )
, matched = cms.InputTag( "patTrigger" )
, matchedCuts = cms.string( 'path( "HLT_Jet15U" )' )
, maxDPtRel = cms.double( 3.0 )
, maxDeltaR = cms.double( 0.4 )
, resolveAmbiguities    = cms.bool( True )
, resolveByMatchQuality = cms.bool( False )
)
process.metTriggerTestMatchHLTMET45 = cms.EDProducer(
  "PATTriggerMatcherDRLessByR"
, src     = cms.InputTag( "patMETs" )
, matched = cms.InputTag( "patTrigger" )
, matchedCuts = cms.string( 'path( "HLT_MET45" )' )
, maxDPtRel = cms.double( 0.5 )
, maxDeltaR = cms.double( 0.5 )
, resolveAmbiguities    = cms.bool( True )
, resolveByMatchQuality = cms.bool( False )
)
process.metTriggerTestMatchHLTMu3 = cms.EDProducer(
  "PATTriggerMatcherDRLessByR"
, src     = cms.InputTag( "patMETs" )
, matched = cms.InputTag( "patTrigger" )
, matchedCuts = cms.string( 'path( "HLT_Mu3", 0 )' )
, maxDPtRel = cms.double( 0.5 )
, maxDeltaR = cms.double( 0.5 )
, resolveAmbiguities    = cms.bool( True )
, resolveByMatchQuality = cms.bool( False )
)
process.patTriggerTestMatcher = cms.Sequence(
  process.electronTriggerTestMatchHLTElectrons
+ process.electronTriggerTestMatchHLTFilterEGammas
+ process.muonTriggerTestMatchL1Muons
+ process.muonTriggerTestMatchL1FilterLabelMuons
+ process.muonTriggerTestMatchNoMuons
+ process.jetTriggerTestMatchHLTJet15U
+ process.metTriggerTestMatchHLTMET45
+ process.metTriggerTestMatchHLTMu3
)
process.cleanPatPhotonsTriggerTestMatch = cms.EDProducer(
  "PATTriggerMatchPhotonEmbedder"
, src     = cms.InputTag( "cleanPatPhotons" )
, matches = cms.VInputTag(
  )
)
process.cleanPatElectronsTriggerTestMatch = cms.EDProducer(
  "PATTriggerMatchElectronEmbedder"
, src     = cms.InputTag( "cleanPatElectrons" )
, matches = cms.VInputTag(
    "electronTriggerTestMatchHLTElectrons"
  , "electronTriggerTestMatchHLTFilterEGammas"
  )
)
process.cleanPatMuonsTriggerTestMatch = cms.EDProducer(
  "PATTriggerMatchMuonEmbedder"
, src     = cms.InputTag( "cleanPatMuons" )
, matches = cms.VInputTag(
    "muonTriggerTestMatchL1Muons"
  , "muonTriggerTestMatchL1FilterLabelMuons"
  , "muonTriggerTestMatchNoMuons"
  )
)
process.cleanPatTausTriggerTestMatch = cms.EDProducer(
  "PATTriggerMatchTauEmbedder"
, src     = cms.InputTag( "cleanPatTaus" )
, matches = cms.VInputTag(
  )
)
process.cleanPatJetsTriggerTestMatch = cms.EDProducer(
  "PATTriggerMatchJetEmbedder"
, src     = cms.InputTag( "cleanPatJets" )
, matches = cms.VInputTag(
    "jetTriggerTestMatchHLTJet15U"
  )
)
process.patMETsTriggerTestMatch = cms.EDProducer(
  "PATTriggerMatchMETEmbedder"
, src     = cms.InputTag( "patMETs" )
, matches = cms.VInputTag(
    "metTriggerTestMatchHLTMET45"
  , "metTriggerTestMatchHLTMu3"
  )
)
process.patTriggerTestMatchEmbedder = cms.Sequence(
  process.cleanPatPhotonsTriggerTestMatch
+ process.cleanPatElectronsTriggerTestMatch
+ process.cleanPatMuonsTriggerTestMatch
+ process.cleanPatTausTriggerTestMatch
+ process.cleanPatJetsTriggerTestMatch
+ process.patMETsTriggerTestMatch
)
process.patTriggerEventTest = cms.EDProducer(
  "PATTriggerEventProducer"
, processName        = cms.string( '*' )
, triggerResults     = cms.InputTag( 'TriggerResults' )
, patTriggerProducer = cms.InputTag( 'patTrigger' )
, condGtTag          = cms.InputTag( 'conditionsInEdm' )
, l1GtTag            = cms.InputTag( 'gtDigis' )
, patTriggerMatches  = cms.VInputTag(
    'electronTriggerTestMatchHLTElectrons'
  , 'electronTriggerTestMatchHLTFilterEGammas'
  , 'muonTriggerTestMatchL1Muons'
  , 'muonTriggerTestMatchL1FilterLabelMuons'
  , 'muonTriggerTestMatchNoMuons'
  , 'jetTriggerTestMatchHLTJet15U'
  , 'metTriggerTestMatchHLTMET45'
  , 'metTriggerTestMatchHLTMu3'
  )
)
process.patTriggerTestSequence = cms.Sequence(
  process.patTrigger
* process.patTriggerTestMatcher
* process.patTriggerTestMatchEmbedder
* process.patTriggerEventTest
)
process.patDefaultSequence += process.patTriggerTestSequence
process.out.outputCommands += [
  'keep *_patTrigger_*_*'
, 'keep *_patTriggerEventTest_*_*'
, 'keep *_*TriggerTestMatch_*_*'
, 'keep patTriggerObjectStandAlonesedmAssociation_*_*_*'
, 'keep edmTriggerResults_TriggerResults_*_*'
, 'keep *_hltTriggerSummaryAOD_*_*'
, 'keep *_l1extraParticles_*_*'
, 'keep *_gctDigis_*_*'
]
