import FWCore.ParameterSet.Config as cms

process = cms.Process( "READ" )

process.load( "FWCore.MessageLogger.MessageLogger_cfi" )
process.MessageLogger.cerr = cms.untracked.PSet(
  placeholder = cms.untracked.bool( True )
)
process.MessageLogger.cout = cms.untracked.PSet(
  INFO = cms.untracked.PSet(
    reportEvery = cms.untracked.int32( 250 )
  )
)

process.SiStripDQMRead = cms.EDAnalyzer( "AlCaRecoTriggerBitsRcdRead"
, outputType  = cms.untracked.string( 'text' )
, rawFileName = cms.untracked.string( '/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_3_6_1/output/AlCaRecoTriggerBits_SiStripDQM' )
)

process.source = cms.Source( "EmptySource"
, numberEventsInRun = cms.untracked.uint32( 1 ) # do not change!
, firstRun          = cms.untracked.uint32( 132000 )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 1000 )
)

import CondCore.DBCommon.CondDBSetup_cfi
process.dbInput = cms.ESSource( "PoolDBESSource"
, CondCore.DBCommon.CondDBSetup_cfi.CondDBSetup
, connect = cms.string( 'sqlite_file:/afs/cern.ch/user/v/vadler/scratch0/cms/SiStripDQM/CMSSW_3_6_1/output/AlCaRecoTriggerBits_SiStripDQM.db' )
, toGet   = cms.VPSet(
    cms.PSet(
      record = cms.string( 'AlCaRecoTriggerBitsRcd' )
    , tag    = cms.string( 'AlCaRecoTriggerBits_SiStripDQM_v0_express' )
    )
  )
)

process.p = cms.Path(
  process.SiStripDQMRead
)
