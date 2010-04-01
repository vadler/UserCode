import FWCore.ParameterSet.Config as cms

process = cms.Process( "READ" )

process.load( "FWCore.MessageLogger.MessageLogger_cfi" )
process.MessageLogger.cerr = cms.untracked.PSet( placeholder = cms.untracked.bool( True )
                                               )
process.MessageLogger.cout = cms.untracked.PSet( INFO = cms.untracked.PSet( reportEvery = cms.untracked.int32( 250 )
                                                                          )
                                               )

process.TrackerDQMTriggerBitsRcdCreateNewRead = cms.EDAnalyzer( "AlCaRecoTriggerBitsRcdRead"
                                                              , outputType = cms.untracked.string( 'text' )
                                                              , rawFileName = cms.untracked.string( '/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_3_5_6/output/TrackerDQMTriggerBits' )
                                                              )

process.source = cms.Source( "EmptySource"
                           , numberEventsInRun = cms.untracked.uint32( 1 ) # do not change!
                           , firstRun          = cms.untracked.uint32( 123000 )
                           )
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( 2000 )
                                      )

import CondCore.DBCommon.CondDBSetup_cfi
process.dbInput = cms.ESSource( "PoolDBESSource"
                              , CondCore.DBCommon.CondDBSetup_cfi.CondDBSetup
                              , connect = cms.string( 'sqlite_file:/afs/cern.ch/user/v/vadler/scratch0/cms/SiStripDQM/CMSSW_3_5_6/output/TrackerDQMTriggerBits.db' )
                              , toGet   = cms.VPSet( cms.PSet( record = cms.string( 'AlCaRecoTriggerBitsRcd' )
                                                             , tag    = cms.string( 'TrackerDQMTriggerBits_v0_hlt' )
                                                             )
                                                   )
                              )

process.p = cms.Path( process.TrackerDQMTriggerBitsRcdCreateNewRead
                    )
