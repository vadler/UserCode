import FWCore.ParameterSet.Config as cms

# Process
process = cms.Process( "TEST" )

process.load( "FWCore.MessageService.MessageLogger_cfi" )

process.load( "Configuration.StandardSequences.Services_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.globaltag = 'GR_R_36X_V9::All'
process.load( "Configuration.StandardSequences.GeometryIdeal_cff" )
process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )

process.load( "Configuration.StandardSequences.RawToDigi_Data_cff" )
process.load( "Configuration.StandardSequences.Reconstruction_cff" )
# skip events with HV off
process.fourthPLSeeds.ClusterCheckPSet.MaxNumberOfCosmicClusters = 20000
process.fifthSeeds.ClusterCheckPSet.MaxNumberOfCosmicClusters    =  5000
# PV overrides
process.offlinePrimaryVerticesWithBS.PVSelParameters.maxDistanceToBeam    =   2
process.offlinePrimaryVerticesWithBS.TkFilterParameters.maxNormalizedChi2 =  20
process.offlinePrimaryVerticesWithBS.TkFilterParameters.minSiliconHits    = cms.int32( 6 )
process.offlinePrimaryVerticesWithBS.TkFilterParameters.maxD0Significance = 100
process.offlinePrimaryVerticesWithBS.TkFilterParameters.minPixelHits      = cms.int32( 1 )
process.offlinePrimaryVerticesWithBS.TkClusParameters.zSeparation         = cms.double( 10. )
process.offlinePrimaryVertices.PVSelParameters.maxDistanceToBeam          =   2
process.offlinePrimaryVertices.TkFilterParameters.maxNormalizedChi2       =  20
process.offlinePrimaryVertices.TkFilterParameters.minSiliconHits          = cms.int32( 6 )
process.offlinePrimaryVertices.TkFilterParameters.maxD0Significance       = 100
process.offlinePrimaryVertices.TkFilterParameters.minPixelHits            = cms.int32( 1 )
process.offlinePrimaryVertices.TkClusParameters.zSeparation               = cms.double( 10. )
process.load( "DQMOffline.Configuration.DQMOffline_cff" )
process.load( "DQMOffline.Configuration.DQMOffline_SecondStep_cff" )
process.DQMStore.referenceFileName = ''
process.DQMStore.collateHistograms = False
process.load( "DQMServices.Components.DQMEnvironment_cfi" )
process.dqmSaver.convention = 'Offline'
process.dqmSaver.workflow   = '/MinimumBias/CMSSW_3_6_X/RECO'
process.dqmSaver.dirName    = '/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_3_6_1/output'
process.load( "DQMOffline.Configuration.DQMOffline_Certification_cff" )
process.load( 'L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff' )

# Input
process.source = cms.Source( "PoolSource"
, fileNames = cms.untracked.vstring(
    # 132440 RAW
    '/store/data/Commissioning10/MinimumBias/RAW-RECO/Apr1Skim_GOODCOLL-v1/0139/FA7B208C-B33E-DF11-A713-003048679010.root', # 7831 events
    # 132658 RAW
    '/store/data/Commissioning10/MinimumBias/RAW-RECO/v8/000/132/658/E8443DDC-AF41-DF11-90D2-003048D4777E.root'
  )
, skipEvents    = cms.untracked.uint32( 7800 )
, inputCommands = cms.untracked.vstring(
    'keep *'
  , 'drop *_hltL1GtObjectMap_*_*'
  )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 100 )
)
process.options = cms.untracked.PSet(
  Rethrow     = cms.untracked.vstring( 'ProductNotFound' )
, fileMode    = cms.untracked.string( 'FULLMERGE' )
, wantSummary = cms.untracked.bool( True )
)

# process.SiStripMonitorTrack.andOr         = True
# process.SiStripMonitorTrack.dcsInputTag   = "scalersRawToDigi"
# process.SiStripMonitorTrack.dcsPartitions = [
#   24
# , 25
# , 26
# , 27
# ]
# process.SiStripMonitorTrack.andOrDcs      = True
# process.SiStripMonitorTrack.errorReplyDcs = True
# process.SiStripMonitorTrack.gtInputTag    = "gtDigis"
# process.SiStripMonitorTrack.gtDBKey       = 'SiStripDQM_Gt'
# process.SiStripMonitorTrack.gtStatusBits  = [
#   'PhysicsDeclared'
# ]
# process.SiStripMonitorTrack.andOrGt       = False
# process.SiStripMonitorTrack.errorReplyGt  = False
# process.SiStripMonitorTrack.l1DBKey       = cms.string( 'SiStripDQM_L1' )
# process.SiStripMonitorTrack.l1Algorithms  = cms.vstring()
# process.SiStripMonitorTrack.andOrL1       = cms.bool( False )
# process.SiStripMonitorTrack.errorReplyL1  = cms.bool( False )
process.SiStripMonitorTrack.hltInputTag   = cms.InputTag( "TriggerResults::HLT" )
process.SiStripMonitorTrack.hltDBKey      = cms.string( '' )
process.SiStripMonitorTrack.hltPaths      = cms.vstring(
  'HLT_L1SingleTauJet'
)
process.SiStripMonitorTrack.andOrHlt      = cms.bool( False )
process.SiStripMonitorTrack.errorReplyHlt = cms.bool( False )

# process.TrackerCollisionTrackMon.andOr         = True
# process.TrackerCollisionTrackMon.dcsInputTag   = "scalersRawToDigi"
# process.TrackerCollisionTrackMon.dcsPartitions = [
#   24
# , 25
# , 26
# , 27
# ]
# process.TrackerCollisionTrackMon.andOrDcs      = True
# process.TrackerCollisionTrackMon.errorReplyDcs = True
# process.TrackerCollisionTrackMon.gtInputTag    = "gtDigis"
# process.TrackerCollisionTrackMon.gtDBKey       = 'SiStripDQM_Gt'
# process.TrackerCollisionTrackMon.gtStatusBits  = [
#   'PhysicsDeclared'
# ]
# process.TrackerCollisionTrackMon.andOrGt       = False
# process.TrackerCollisionTrackMon.errorReplyGt  = False
process.TrackerCollisionTrackMon.l1DBKey       = cms.string( 'SiStripDQM_L1' )
process.TrackerCollisionTrackMon.l1Algorithms  = cms.vstring(
#   'L1Tech_BPTX_plus_AND_minus.v0'                                        # 0
# , 'L1Tech_BSC_minBias_threshold1.v0 OR L1Tech_BSC_minBias_threshold2.v0' # 40 OR 41
  'L1Tech_BSC_minBias_threshold2.v0'                                     # 41
, 'NOT L1Tech_BSC_halo_beam2_inner.v0'                                   # NOT 36
, 'NOT L1Tech_BSC_halo_beam2_outer.v0'                                   # NOT 37
, 'NOT L1Tech_BSC_halo_beam1_inner.v0'                                   # NOT 38
, 'NOT L1Tech_BSC_halo_beam1_outer.v0'                                   # NOT 39
, 'NOT (L1Tech_BSC_splash_beam1.v0 AND NOT L1Tech_BSC_splash_beam2.v0)'  # NOT (42 AND NOT 43)
, 'NOT (L1Tech_BSC_splash_beam2.v0 AND NOT L1Tech_BSC_splash_beam1.v0)'  # NOT (43 AND NOT 42)
)
process.TrackerCollisionTrackMon.andOrL1       = cms.bool( False )
process.TrackerCollisionTrackMon.errorReplyL1  = cms.bool( False )
# process.TrackerCollisionTrackMon.hltInputTag   = cms.InputTag( "TriggerResults::HLT" )
# process.TrackerCollisionTrackMon.hltDBKey      = cms.string( '' )
# process.TrackerCollisionTrackMon.hltPaths      = cms.vstring()
# process.TrackerCollisionTrackMon.andOrHlt      = cms.bool( False )
# process.TrackerCollisionTrackMon.errorReplyHlt = cms.bool( False )

import CondCore.DBCommon.CondDBSetup_cfi
process.dbSiStripTriggerBits = cms.ESSource( "PoolDBESSource"
, CondCore.DBCommon.CondDBSetup_cfi.CondDBSetup
, connect = cms.string( 'sqlite_file:/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_3_6_1/output/AlCaRecoTriggerBits_SiStripDQM.db' )
# , connect = cms.string( 'frontier://FrontierPrep/CMS_COND_STRIP' )
# , connect = cms.string( 'frontier://FrontierProd/CMS_COND_31X_STRIP' )
, toGet   = cms.VPSet(
    cms.PSet(
      record = cms.string( 'AlCaRecoTriggerBitsRcd' )
    , tag    = cms.string( 'AlCaRecoTriggerBits_SiStripDQM_v0_express' )
    )
  )
)
process.es_prefer_trackerDqmTriggerBits = cms.ESPrefer( "PoolDBESSource", "dbSiStripTriggerBits" )

process.dbSiStripQTests = cms.ESSource( "PoolDBESSource"
, CondCore.DBCommon.CondDBSetup_cfi.CondDBSetup
, connect = cms.string( 'sqlite_file:/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_3_6_1/output/DQMXMLFile_SiStripDQM.db' )
# , connect = cms.string( 'frontier://FrontierPrep/CMS_COND_STRIP' )
# , connect = cms.string( 'frontier://FrontierProd/CMS_COND_31X_STRIP' )
, toGet   = cms.VPSet(
    cms.PSet(
      record = cms.string( 'DQMXMLFileRcd' )
    , tag    = cms.string( 'DQMXMLFile_SiStripDQM_v0_express' )
    )
  )
)
process.es_prefer_trackerDqmQTests = cms.ESPrefer( "PoolDBESSource", "dbSiStripQTests" )
process.siStripQTester.getQualityTestsFromFile = False

process.dbSiPixelQTests = cms.ESSource( "PoolDBESSource"
, CondCore.DBCommon.CondDBSetup_cfi.CondDBSetup
, connect = cms.string( 'sqlite_file:/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_3_6_1/output/DQMXMLFile_SiPixelDQM.db' )
# , connect = cms.string( 'frontier://FrontierPrep/CMS_COND_PIXEL' )
# , connect = cms.string( 'frontier://FrontierProd/CMS_COND_31X_PIXEL' )
, toGet   = cms.VPSet(
    cms.PSet(
      record = cms.string( 'DQMXMLFileRcd' )
    , tag    = cms.string( 'DQMXMLFile_SiPixelDQM_v0_express' )
    )
  )
)
process.es_prefer_trackerDqmQTests = cms.ESPrefer( "PoolDBESSource", "dbSiPixelQTests" )

process.raw2Digi = cms.Sequence(
  process.scalersRawToDigi
+ process.siPixelDigis
+ process.siStripDigis
)
process.reco = cms.Sequence(
  process.trackerlocalreco
* process.offlineBeamSpot
* process.recopixelvertexing
* process.ckftracks
* process.vertexreco
* process.ctfTracksPixelLess
* process.logErrorHarvester
)
process.dqm = cms.Sequence(
  process.SiStripDQMTier0
* process.DQMMessageLogger
)

process.path = cms.Path(
  # preparation
  process.gtDigis
* process.raw2Digi
* process.reco
  # DQM sources
* process.dqm
  # DQM client
* process.dqmRefHistoRootFileGetter
* process.SiStripOfflineDQMClient
* process.DQMMessageLoggerClient
* process.siStripDaqInfo
* process.siStripDcsInfo
* process.siStripCertificationInfo
* process.dqmSaver
)

process.out = cms.OutputModule( "PoolOutputModule",
  fileName       = cms.untracked.string( '/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_3_6_1/output/SiStripDQM_Tier0Collisions.root' ),
  SelectEvents   = cms.untracked.PSet(
    SelectEvents = cms.vstring(
      'path'
    )
  ),
  outputCommands = cms.untracked.vstring(
    'drop *',
    'keep *_*_*_TEST'
  )
)

process.outpath = cms.EndPath(
  process.out
)
