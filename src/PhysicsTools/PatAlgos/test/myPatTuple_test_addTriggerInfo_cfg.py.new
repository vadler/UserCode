from PhysicsTools.PatAlgos.patTemplate_cfg import *
process.out.fileName = '/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_8_0_pre7/output/myPatTuple_test_addTriggerInfo.root'

process.p = cms.Path(
  process.patDefaultSequence
)

# Trigger Test
process.patTrigger = cms.EDProducer( "PATTriggerProducer"
  ## L1
, addL1Algos     = cms.bool( True )
, l1ExtraMu      = cms.InputTag( 'l1extraParticles', ''           , 'HLT' )
, l1ExtraNoIsoEG = cms.InputTag( 'l1extraParticles', 'NonIsolated', 'HLT' )
, l1ExtraIsoEG   = cms.InputTag( 'l1extraParticles', 'Isolated'   , 'HLT' )
, l1ExtraCenJet  = cms.InputTag( 'l1extraParticles', 'Central'    , 'HLT' )
, l1ExtraForJet  = cms.InputTag( 'l1extraParticles', 'Forward'    , 'HLT' )
, l1ExtraTauJet  = cms.InputTag( 'l1extraParticles', 'Tau'        , 'HLT' )
, l1ExtraETM     = cms.InputTag( 'l1extraParticles', 'MET'        , 'HLT' )
, l1ExtraHTM     = cms.InputTag( 'l1extraParticles', 'MHT'        , 'HLT' )
, saveL1Refs     = cms.bool( True )
  ## HLT (L3)
, processName    = cms.string( 'HLT' )
, triggerResults = cms.InputTag( "TriggerResults" )
, triggerEvent   = cms.InputTag( "hltTriggerSummaryAOD" )
, onlyStandAlone = cms.bool( False )
  # trigger paths
, addPathModuleLabels = cms.bool( False ) # setting this 'True' stores the names of all modules as strings (~8kB/ev.)
)
process.electronTriggerTestMatchHLTElectrons = cms.EDProducer( "PATTriggerMatcherDRLessByR"
, src     = cms.InputTag( "cleanPatElectrons" )
, matched = cms.InputTag( "patTrigger" )
, andOr          = cms.bool( False )
, filterIdsEnum  = cms.vstring( 'TriggerElectron' )
, filterIds      = cms.vint32( 82 )
, filterLabels   = cms.vstring( '*' )
, pathNames      = cms.vstring( '*' )
, collectionTags = cms.vstring( '*' )
, maxDPtRel = cms.double( 0.5 )
, maxDeltaR = cms.double( 0.5 )
, resolveAmbiguities    = cms.bool( True )
, resolveByMatchQuality = cms.bool( False )
)
process.electronTriggerTestMatchHLTFilterEGammas = cms.EDProducer( "PATTriggerMatcherDRLessByR"
, src     = cms.InputTag( "cleanPatElectrons" )
, matched = cms.InputTag( "patTrigger" )
, andOr          = cms.bool( False )
, filterIdsEnum  = cms.vstring( '*' )
, filterIds      = cms.vint32( 0 )
, filterLabels   = cms.vstring( 'hltL1sL1SingleEG5' )
, pathNames      = cms.vstring( '*' )
, collectionTags = cms.vstring( '*' )
, maxDPtRel = cms.double( 0.5 )
, maxDeltaR = cms.double( 0.5 )
, resolveAmbiguities    = cms.bool( True )
, resolveByMatchQuality = cms.bool( False )
)
process.muonTriggerTestMatchL1Muons = cms.EDProducer( "PATTriggerMatcherDRLessByR"
, src     = cms.InputTag( "cleanPatMuons" )
, matched = cms.InputTag( "patTrigger" )
, andOr          = cms.bool( False )
, filterIdsEnum  = cms.vstring( '*' )
, filterIds      = cms.vint32( 0 )
, filterLabels   = cms.vstring( '*' )
, pathNames      = cms.vstring( '*' )
, collectionTags = cms.vstring( 'l1extraParticles' )
, maxDPtRel = cms.double( 0.5 )
, maxDeltaR = cms.double( 0.5 )
, resolveAmbiguities    = cms.bool( True )
, resolveByMatchQuality = cms.bool( False )
)
process.muonTriggerTestMatchL1FilterLabelMuons = cms.EDProducer( "PATTriggerMatcherDRLessByR"
, src     = cms.InputTag( "cleanPatMuons" )
, matched = cms.InputTag( "patTrigger" )
, andOr          = cms.bool( True )
, filterIdsEnum  = cms.vstring( '*' )
, filterIds      = cms.vint32( 0 )
, filterLabels   = cms.vstring( 'hltMuLevel1PathL1Filtered' )
, pathNames      = cms.vstring( '*' )
, collectionTags = cms.vstring( '*' )
, maxDPtRel = cms.double( 0.5 )
, maxDeltaR = cms.double( 0.5 )
, resolveAmbiguities    = cms.bool( True )
, resolveByMatchQuality = cms.bool( False )
)
process.muonTriggerTestMatchNoMuons = cms.EDProducer( "PATTriggerMatcherDRLessByR"
, src     = cms.InputTag( "cleanPatMuons" )
, matched = cms.InputTag( "patTrigger" )
, andOr          = cms.bool( False )
, filterIdsEnum  = cms.vstring( '*' )
, filterIds      = cms.vint32( 0 )
, filterLabels   = cms.vstring( '*' )
, pathNames      = cms.vstring( 'someFunnyPath' )
, collectionTags = cms.vstring( '*' )
, maxDPtRel = cms.double( 0.5 )
, maxDeltaR = cms.double( 0.5 )
, resolveAmbiguities    = cms.bool( True )
, resolveByMatchQuality = cms.bool( False )
)
process.jetTriggerTestMatchHLTJet15U = cms.EDProducer( "PATTriggerMatcherDRLessByR"
, src     = cms.InputTag( "cleanPatJets" )
, matched = cms.InputTag( "patTrigger" )
, andOr          = cms.bool( False )
, filterIdsEnum  = cms.vstring( '*' )
, filterIds      = cms.vint32( 0 )
, filterLabels   = cms.vstring( '*' )
, pathNames      = cms.vstring( 'HLT_Jet15U' )
, collectionTags = cms.vstring( '*' )
, maxDPtRel = cms.double( 0.5 )
, maxDeltaR = cms.double( 0.5 )
, resolveAmbiguities    = cms.bool( True )
, resolveByMatchQuality = cms.bool( False )
)
process.metTriggerTestMatchHLTMET45 = cms.EDProducer( "PATTriggerMatcherDRLessByR"
, src     = cms.InputTag( "patMETs" )
, matched = cms.InputTag( "patTrigger" )
, andOr          = cms.bool( False )
, filterIdsEnum  = cms.vstring( '*' )
, filterIds      = cms.vint32( 0 )
, filterLabels   = cms.vstring( '*' )
, pathNames      = cms.vstring( 'HLT_MET45' )
, collectionTags = cms.vstring( '*' )
, maxDPtRel = cms.double( 0.5 )
, maxDeltaR = cms.double( 0.5 )
, resolveAmbiguities    = cms.bool( True )
, resolveByMatchQuality = cms.bool( False )
)
process.metTriggerTestMatchHLTMu3 = cms.EDProducer( "PATTriggerMatcherDRLessByR"
, src     = cms.InputTag( "patMETs" )
, matched = cms.InputTag( "patTrigger" )
, andOr          = cms.bool( False )
, filterIdsEnum  = cms.vstring( '*' )
, filterIds      = cms.vint32( 0 )
, filterLabels   = cms.vstring( '*' )
, pathNames      = cms.vstring( 'HLT_Mu3' )
, collectionTags = cms.vstring( '*' )
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
process.cleanPatPhotonsTriggerTestMatch = cms.EDProducer( "PATTriggerMatchPhotonEmbedder",
    src     = cms.InputTag( "cleanPatPhotons" ),
    matches = cms.VInputTag(
                           )
)
process.cleanPatElectronsTriggerTestMatch = cms.EDProducer( "PATTriggerMatchElectronEmbedder"
, src     = cms.InputTag( "cleanPatElectrons" )
, matches = cms.VInputTag(
    "electronTriggerTestMatchHLTElectrons"
  , "electronTriggerTestMatchHLTFilterEGammas"
  )
)
process.cleanPatMuonsTriggerTestMatch = cms.EDProducer( "PATTriggerMatchMuonEmbedder"
, src     = cms.InputTag( "cleanPatMuons" )
, matches = cms.VInputTag(
    "muonTriggerTestMatchL1Muons"
  , "muonTriggerTestMatchL1FilterLabelMuons"
  , "muonTriggerTestMatchNoMuons"
  )
)
process.cleanPatTausTriggerTestMatch = cms.EDProducer( "PATTriggerMatchTauEmbedder"
, src     = cms.InputTag( "cleanPatTaus" )
, matches = cms.VInputTag(
  )
)
process.cleanPatJetsTriggerTestMatch = cms.EDProducer( "PATTriggerMatchJetEmbedder"
, src     = cms.InputTag( "cleanPatJets" )
, matches = cms.VInputTag(
    "jetTriggerTestMatchHLTJet15U"
  )
)
process.patMETsTriggerTestMatch = cms.EDProducer( "PATTriggerMatchMETEmbedder"
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
process.patTriggerEventTest = cms.EDProducer( "PATTriggerEventProducer"
, processName        = cms.string( 'HLT' )
, triggerResults     = cms.InputTag( "TriggerResults" )
, patTriggerProducer = cms.InputTag( "patTrigger" )
, patTriggerMatches  = cms.VInputTag(
    "electronTriggerTestMatchHLTElectrons"
  , "electronTriggerTestMatchHLTFilterEGammas"
  , "muonTriggerTestMatchL1Muons"
  , "muonTriggerTestMatchL1FilterLabelMuons"
  , "muonTriggerTestMatchNoMuons"
  , "jetTriggerTestMatchHLTJet15U"
  , "metTriggerTestMatchHLTMET45"
  , "metTriggerTestMatchHLTMu3"
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
, 'keep edmTriggerResults_TriggerResults_*_HLT'
, 'keep *_hltTriggerSummaryAOD_*_*'
, 'keep *_l1extraParticles_*_HLT'
, 'keep *_gctDigis_*_HLT'
]
