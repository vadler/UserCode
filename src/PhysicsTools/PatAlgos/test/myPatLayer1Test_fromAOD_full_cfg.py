from PhysicsTools.PatAlgos.patTemplate_cfg import *
#process.GlobalTag.globaltag = 'STARTUP31X_V8::All'
#process.source.fileNames    = [ '/store/relval/CMSSW_3_4_0_pre1/RelValTTbar/GEN-SIM-RECO/STARTUP31X_V8-v1/0007/BAB355DB-DFB4-DE11-AD73-0019B9F72F97.root' ]
# process.maxEvents.input     = ...
# process.out.outputCommands  = [ ... ]
process.out.fileName        = '/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_5_X_2010-03-01-1700/output/myPatLayer1Test_fromAOD_full.root'
# process.options.wantSummary = False

# # memory check
# process.SimpleMemoryCheck = cms.Service( "SimpleMemoryCheck",
# #     oncePerEventMode = cms.untracked.bool( True ),
#     ignoreTotal      = cms.untracked.int32( 0 )
# )

# # HLT analyzers
# process.load( "HLTrigger.HLTcore.hltEventAnalyzerAOD_cfi" )
# process.load( "HLTrigger.HLTcore.triggerSummaryAnalyzerAOD_cfi" )

process.p = cms.Path(
#     process.hltEventAnalyzerAOD       +
#     process.triggerSummaryAnalyzerAOD +
    process.patDefaultSequence
)

# Trigger Test
process.patTrigger = cms.EDProducer( "PATTriggerProducer"
                                   ## L1
                                   , l1ExtraMu         = cms.InputTag( "l1extraParticles", ""           , "RECO" )
                                   , l1ExtraNoIsoEG    = cms.InputTag( "l1extraParticles", "NonIsolated", "RECO" )
                                   , l1ExtraIsoEG      = cms.InputTag( "l1extraParticles", "Isolated"   , "RECO" )
                                   , l1ExtraCenJet     = cms.InputTag( "l1extraParticles", "Central"    , "RECO" )
                                   , l1ExtraForJet     = cms.InputTag( "l1extraParticles", "Forward"    , "RECO" )
                                   , l1ExtraTauJet     = cms.InputTag( "l1extraParticles", "Tau"        , "RECO" )
                                   , l1ExtraETM        = cms.InputTag( "l1extraParticles", "MET"        , "RECO" )
                                   , l1ExtraHTM        = cms.InputTag( "l1extraParticles", "MHT"        , "RECO" )
                                   ## HLT (L3)
                                   , processName    = cms.string( 'HLT' )
                                   , triggerResults = cms.InputTag( "TriggerResults" )
                                   , triggerEvent   = cms.InputTag( "hltTriggerSummaryAOD" )
                                   , onlyStandAlone = cms.bool( False )
                                   # trigger paths
                                   , addPathModuleLabels = cms.bool( False ) # setting this 'True' stores the names of all modules as strings (~8kB/ev.)
                                   )
process.electronTriggerTestMatchHLTElectrons = cms.EDFilter( "PATTriggerMatcherDRLessByR",
    src     = cms.InputTag( "cleanPatElectrons" ),
    matched = cms.InputTag( "patTrigger" ),
    andOr          = cms.bool( False ),
    filterIdsEnum  = cms.vstring( 'TriggerElectron' ),
    filterIds      = cms.vint32( 82 ),
    filterLabels   = cms.vstring( '*' ),
    pathNames      = cms.vstring( '*' ),
    collectionTags = cms.vstring( '*' ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)
process.electronTriggerTestMatchHLTFilterEGammas = cms.EDFilter( "PATTriggerMatcherDRLessByR",
    src     = cms.InputTag( "cleanPatElectrons" ),
    matched = cms.InputTag( "patTrigger" ),
    andOr          = cms.bool( False ),
    filterIdsEnum  = cms.vstring( '*' ),
    filterIds      = cms.vint32( 0 ),
    filterLabels   = cms.vstring( 'hltL1sL1SingleEG5' ),
    pathNames      = cms.vstring( '*' ),
    collectionTags = cms.vstring( '*' ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)
process.muonTriggerTestMatchL1Muons = cms.EDFilter( "PATTriggerMatcherDRLessByR",
    src     = cms.InputTag( "cleanPatMuons" ),
    matched = cms.InputTag( "patTrigger" ),
    andOr          = cms.bool( False ),
    filterIdsEnum  = cms.vstring( 'TriggerL1Mu' ),
    filterIds      = cms.vint32( -81 ),
    filterLabels   = cms.vstring( '*' ),
    pathNames      = cms.vstring( '*' ),
    collectionTags = cms.vstring( '*' ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)
process.muonTriggerTestMatchL1CollectionMuons = cms.EDFilter( "PATTriggerMatcherDRLessByR",
    src     = cms.InputTag( "cleanPatMuons" ),
    matched = cms.InputTag( "patTrigger" ),
    andOr          = cms.bool( True ),
    filterIdsEnum  = cms.vstring( '*' ),
    filterIds      = cms.vint32( 0 ),
    filterLabels   = cms.vstring( 'hltMuLevel1PathL1Filtered' ),
    pathNames      = cms.vstring( '*' ),
    collectionTags = cms.vstring( '*' ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)
process.muonTriggerTestMatchNoMuons = cms.EDFilter( "PATTriggerMatcherDRLessByR",
    src     = cms.InputTag( "cleanPatMuons" ),
    matched = cms.InputTag( "patTrigger" ),
    andOr          = cms.bool( False ),
    filterIdsEnum  = cms.vstring( '*' ),
    filterIds      = cms.vint32( 0 ),
    filterLabels   = cms.vstring( '*' ),
    pathNames      = cms.vstring( 'someFunnyPath' ),
    collectionTags = cms.vstring( '*' ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)
process.jetTriggerTestMatchHLTJet15U = cms.EDFilter( "PATTriggerMatcherDRLessByR",
    src     = cms.InputTag( "cleanPatJets" ),
    matched = cms.InputTag( "patTrigger" ),
    andOr          = cms.bool( False ),
    filterIdsEnum  = cms.vstring( '*' ),
    filterIds      = cms.vint32( 0 ),
    filterLabels   = cms.vstring( '*' ),
    pathNames      = cms.vstring( 'HLT_Jet15U' ),
    collectionTags = cms.vstring( '*' ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)
process.metTriggerTestMatchHLTMET45 = cms.EDFilter( "PATTriggerMatcherDRLessByR",
    src     = cms.InputTag( "patMETs" ),
    matched = cms.InputTag( "patTrigger" ),
    andOr          = cms.bool( False ),
    filterIdsEnum  = cms.vstring( '*' ),
    filterIds      = cms.vint32( 0 ),
    filterLabels   = cms.vstring( '*' ),
    pathNames      = cms.vstring( 'HLT_MET45' ),
    collectionTags = cms.vstring( '*' ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)
process.metTriggerTestMatchHLTMu3 = cms.EDFilter( "PATTriggerMatcherDRLessByR",
    src     = cms.InputTag( "patMETs" ),
    matched = cms.InputTag( "patTrigger" ),
    andOr          = cms.bool( False ),
    filterIdsEnum  = cms.vstring( '*' ),
    filterIds      = cms.vint32( 0 ),
    filterLabels   = cms.vstring( '*' ),
    pathNames      = cms.vstring( 'HLT_Mu3' ),
    collectionTags = cms.vstring( '*' ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)
process.patTriggerTestMatcher = cms.Sequence( process.electronTriggerTestMatchHLTElectrons
                                    + process.electronTriggerTestMatchHLTFilterEGammas
                                    + process.muonTriggerTestMatchL1Muons
                                    + process.muonTriggerTestMatchL1CollectionMuons
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
process.cleanPatElectronsTriggerTestMatch = cms.EDProducer( "PATTriggerMatchElectronEmbedder",
    src     = cms.InputTag( "cleanPatElectrons" ),
    matches = cms.VInputTag( "electronTriggerTestMatchHLTElectrons"
                           , "electronTriggerTestMatchHLTFilterEGammas"
                           )
)
process.cleanPatMuonsTriggerTestMatch = cms.EDProducer( "PATTriggerMatchMuonEmbedder",
    src     = cms.InputTag( "cleanPatMuons" ),
    matches = cms.VInputTag( "muonTriggerTestMatchL1Muons"
                           , "muonTriggerTestMatchL1CollectionMuons"
                           , "muonTriggerTestMatchNoMuons"
                           )
)
process.cleanPatTausTriggerTestMatch = cms.EDProducer( "PATTriggerMatchTauEmbedder",
    src     = cms.InputTag( "cleanPatTaus" ),
    matches = cms.VInputTag(
                           )
)
process.cleanPatJetsTriggerTestMatch = cms.EDProducer( "PATTriggerMatchJetEmbedder",
    src     = cms.InputTag( "cleanPatJets" ),
    matches = cms.VInputTag( "jetTriggerTestMatchHLTJet15U"
                           )
)
process.patMETsTriggerTestMatch = cms.EDProducer( "PATTriggerMatchMETEmbedder",
    src     = cms.InputTag( "patMETs" ),
    matches = cms.VInputTag( "metTriggerTestMatchHLTMET45"
                           , "metTriggerTestMatchHLTMu3"
                           )
)
process.patTriggerTestMatchEmbedder = cms.Sequence( process.cleanPatPhotonsTriggerTestMatch
                                                  + process.cleanPatElectronsTriggerTestMatch
                                                  + process.cleanPatMuonsTriggerTestMatch
                                                  + process.cleanPatTausTriggerTestMatch
                                                  + process.cleanPatJetsTriggerTestMatch
                                                  + process.patMETsTriggerTestMatch
                                                  )
process.patTriggerEventTest = cms.EDProducer( "PATTriggerEventProducer",
    processName        = cms.string( 'HLT' ),
    triggerResults     = cms.InputTag( "TriggerResults" ),
    patTriggerProducer = cms.InputTag( "patTrigger" ),
    patTriggerMatches  = cms.VInputTag( "electronTriggerTestMatchHLTElectrons"
                                      , "electronTriggerTestMatchHLTFilterEGammas"
                                      , "muonTriggerTestMatchL1Muons"
                                      , "muonTriggerTestMatchL1CollectionMuons"
                                      , "muonTriggerTestMatchNoMuons"
                                      , "jetTriggerTestMatchHLTJet15U"
                                      , "metTriggerTestMatchHLTMET45"
                                      , "metTriggerTestMatchHLTMu3"
                                      )
)
process.patTriggerTestSequence = cms.Sequence( process.patTrigger
                                             * process.patTriggerTestMatcher
                                             * process.patTriggerTestMatchEmbedder
                                             * process.patTriggerEventTest
                                             )
process.patDefaultSequence += process.patTriggerTestSequence
process.out.outputCommands += [ 'keep patTriggerObjects_patTrigger_*_*'
                              , 'keep patTriggerFilters_patTrigger_*_*'
                              , 'keep patTriggerPaths_patTrigger_*_*'
                              , 'keep patTriggerEvent_patTriggerEventTest_*_*'
                              , 'keep patTriggerObjectsedmAssociation_patTriggerEventTest_*_*'
                              , 'keep *_cleanPatPhotonsTriggerTestMatch_*_*'
                              , 'keep *_cleanPatElectronsTriggerTestMatch_*_*'
                              , 'keep *_cleanPatMuonsTriggerTestMatch_*_*'
                              , 'keep *_cleanPatTausTriggerTestMatch_*_*'
                              , 'keep *_cleanPatJetsTriggerTestMatch_*_*'
                              , 'keep *_patMETsTriggerTestMatch_*_*'
                              , 'keep patTriggerObjectStandAlones_patTrigger_*_*'
                              , 'keep patTriggerObjectStandAlonesedmAssociation_*_*_*'
                              , 'keep edmTriggerResults_TriggerResults_*_HLT'
                              , 'keep *_hltTriggerSummaryAOD_*_*'
                              , 'keep L1GlobalTriggerReadoutRecord_gtDigis_*_*'
                              , 'keep *_l1GtRecord_*_*'
                              , 'keep *_l1extraParticles_*_*'
                              ]
