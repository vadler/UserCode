import FWCore.ParameterSet.Config as cms

process = cms.Process( "UPDATE" )

process.load( "FWCore.MessageLogger.MessageLogger_cfi" )
process.MessageLogger.cerr = cms.untracked.PSet( placeholder = cms.untracked.bool( True )
                                               )
process.MessageLogger.cout = cms.untracked.PSet( INFO = cms.untracked.PSet( reportEvery = cms.untracked.int32( 1 )
                                                                          )
                                               )

# the module writing to DB
process.TriggerBitsRcdUpdate = cms.EDAnalyzer( "AlCaRecoTriggerBitsRcdUpdate"
                                             , firstRunIOV = cms.uint32( 124000 )
                                             , lastRunIOV  = cms.int32( -1 )
                                             , startEmpty = cms.bool( False )
                                             , listNamesRemove = cms.vstring( 'selectL1'
                                                                            )
                                             , triggerListsAdd = cms.VPSet( cms.PSet( listName = cms.string( 'selectL1' )
                                                                                    , hltPaths = cms.vstring( 'L1Tech_BPTX_plus_AND_minus.v0'
                                                                                                            #, 'L1Tech_BSC_minBias_threshold1.v0 OR L1Tech_BSC_minBias_threshold2.v0'
                                                                                                            , '~L1Tech_BSC_halo_beam2_inner.v0'
                                                                                                            , '~L1Tech_BSC_halo_beam2_outer.v0'
                                                                                                            , '~L1Tech_BSC_halo_beam1_inner.v0'
                                                                                                            , '~L1Tech_BSC_halo_beam1_outer.v0'
                                                                                                            )
                                                                                    )
                                                                          )
                                             )

process.source = cms.Source( "EmptySource"
                           , firstRun = cms.untracked.uint32( 1 )
                           )
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( 1 )
                                      )

import CondCore.DBCommon.CondDBSetup_cfi
process.PoolDBESSource = cms.ESSource( "PoolDBESSource"
                                     , CondCore.DBCommon.CondDBSetup_cfi.CondDBSetup
                                     , connect = cms.string( 'sqlite_file:TriggerBits.db' )
#                                      , connect = cms.string( 'frontier://FrontierProd/CMS_COND_31X_HLT' )
                                     , toGet   = cms.VPSet( cms.PSet( record  = cms.string( 'AlCaRecoTriggerBitsRcd' )
                                                                    , tag     = cms.string( 'TriggerBits_v0_express' )
                                                                    )
                                                          )
                                     )
process.PoolDBOutputService = cms.Service( "PoolDBOutputService"
                                         , CondCore.DBCommon.CondDBSetup_cfi.CondDBSetup
                                         , timetype = cms.untracked.string( 'runnumber' )
                                         , connect  = cms.string( 'sqlite_file:TriggerBits.db' )
                                         , toPut    = cms.VPSet( cms.PSet( record = cms.string( 'AlCaRecoTriggerBitsRcd' )
                                                                         , tag    = cms.string( 'TriggerBits_v0_express' )
                                                                         )
                                                               )
                                         )

process.p = cms.Path( process.TriggerBitsRcdUpdate
                    )


