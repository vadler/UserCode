import FWCore.ParameterSet.Config as cms

patTrigger = cms.EDProducer( "PATTriggerProducer"
                           ## L1
                           , l1GtReadoutRecord = cms.InputTag( "gtDigis", "" , "RECO" )
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

