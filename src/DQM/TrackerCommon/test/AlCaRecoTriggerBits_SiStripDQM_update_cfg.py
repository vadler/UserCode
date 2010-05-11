import FWCore.ParameterSet.Config as cms

process = cms.Process( "UPDATE" )

process.load( "FWCore.MessageLogger.MessageLogger_cfi" )
process.MessageLogger.cerr = cms.untracked.PSet(
  placeholder = cms.untracked.bool( True )
)
process.MessageLogger.cout = cms.untracked.PSet(
  INFO = cms.untracked.PSet(
    reportEvery = cms.untracked.int32( 1 )
  )
)

process.SiStripDQMUpdate = cms.EDAnalyzer( "AlCaRecoTriggerBitsRcdUpdate"
, firstRunIOV     = cms.uint32( 132500 )
, lastRunIOV      = cms.int32( -1 )
, startEmpty      = cms.bool( False )
, listNamesRemove = cms.vstring(
    'SiStripDQM_L1'
  )
, triggerListsAdd = cms.VPSet(
    cms.PSet(
      listName = cms.string( 'SiStripDQM_L1' )
    , hltPaths = cms.vstring(
        'L1Tech_BPTX_plus_AND_minus.v0'                                        # 0
#       , 'L1Tech_BSC_minBias_threshold1.v0 OR L1Tech_BSC_minBias_threshold2.v0' # 40 OR 41
      , 'L1Tech_BSC_minBias_threshold2.v0'                                     # 41
      , 'NOT L1Tech_BSC_halo_beam2_inner.v0'                                   # NOT 36
      , 'NOT L1Tech_BSC_halo_beam2_outer.v0'                                   # NOT 37
      , 'NOT L1Tech_BSC_halo_beam1_inner.v0'                                   # NOT 38
      , 'NOT L1Tech_BSC_halo_beam1_outer.v0'                                   # NOT 39
      , 'NOT (L1Tech_BSC_splash_beam1.v0 AND NOT L1Tech_BSC_splash_beam2.v0)'  # NOT (42 AND NOT 43)
      , 'NOT (L1Tech_BSC_splash_beam2.v0 AND NOT L1Tech_BSC_splash_beam1.v0)'  # NOT (43 AND NOT 42)
      )
    )
  )
)

process.source = cms.Source( "EmptySource"
, firstRun = cms.untracked.uint32( 1 )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 1 )
)

import CondCore.DBCommon.CondDBSetup_cfi
process.PoolDBESSource = cms.ESSource( "PoolDBESSource"
, CondCore.DBCommon.CondDBSetup_cfi.CondDBSetup
, connect = cms.string( 'sqlite_file:/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_3_6_1/output/AlCaRecoTriggerBits_SiStripDQM.db' )
, toGet   = cms.VPSet(
    cms.PSet(
      record  = cms.string( 'AlCaRecoTriggerBitsRcd' )
    , tag     = cms.string( 'AlCaRecoTriggerBits_SiStripDQM_v0_express' )
    )
  )
)
process.PoolDBOutputService = cms.Service( "PoolDBOutputService"
, CondCore.DBCommon.CondDBSetup_cfi.CondDBSetup
, logconnect = cms.untracked.string( 'sqlite_file:/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_3_6_1/output/AlCaRecoTriggerBits_SiStripDQM_update_log.db' )
, timetype   = cms.untracked.string( 'runnumber' )
, connect    = cms.string( 'sqlite_file:/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_3_6_1/output/AlCaRecoTriggerBits_SiStripDQM.db' )
, toPut      = cms.VPSet(
    cms.PSet(
      record = cms.string( 'AlCaRecoTriggerBitsRcd' )
    , tag    = cms.string( 'AlCaRecoTriggerBits_SiStripDQM_v0_express' )
    )
  )
)

process.p = cms.Path(
  process.SiStripDQMUpdate
)
