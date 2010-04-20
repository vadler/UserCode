import FWCore.ParameterSet.Config as cms

patTrigger = cms.EDProducer( "PATTriggerProducer"
                           , onlyStandAlone = cms.bool( False )
#                            ## L1
#                            , l1ExtraMu      = cms.InputTag( "l1extraParticles", ""            ) # default; change only, if you know exactly, what you are doing!
#                            , l1ExtraNoIsoEG = cms.InputTag( "l1extraParticles", "NonIsolated" ) # default; change only, if you know exactly, what you are doing!
#                            , l1ExtraIsoEG   = cms.InputTag( "l1extraParticles", "Isolated"    ) # default; change only, if you know exactly, what you are doing!
#                            , l1ExtraCenJet  = cms.InputTag( "l1extraParticles", "Central"     ) # default; change only, if you know exactly, what you are doing!
#                            , l1ExtraForJet  = cms.InputTag( "l1extraParticles", "Forward"     ) # default; change only, if you know exactly, what you are doing!
#                            , l1ExtraTauJet  = cms.InputTag( "l1extraParticles", "Tau"         ) # default; change only, if you know exactly, what you are doing!
#                            , l1ExtraETM     = cms.InputTag( "l1extraParticles", "MET"         ) # default; change only, if you know exactly, what you are doing!
#                            , l1ExtraHTM     = cms.InputTag( "l1extraParticles", "MHT"         ) # default; change only, if you know exactly, what you are doing!
                           ## HLT (L3)
                           , processName    = cms.string( "HLT" )                    # default; change only, if you know exactly, what you are doing!
#                            , triggerResults = cms.InputTag( "TriggerResults" )       # default; change only, if you know exactly, what you are doing!
#                            , triggerEvent   = cms.InputTag( "hltTriggerSummaryAOD" ) # default; change only, if you know exactly, what you are doing!
#                            , hltPrescaleLabel = cms.string( "0" )
#                            , hltPrescaleTable = cms.string( "hltPrescaleRecorder" )  # only the label!
#                            , addPathModuleLabels = cms.bool( False )                 # setting this "True" stores the names of all modules as strings (~10kB/ev.)
                           )

