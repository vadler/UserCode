import FWCore.ParameterSet.Config as cms

# Process
process = cms.Process( "TEST" )

process.options = cms.untracked.PSet(
  Rethrow     = cms.untracked.vstring( 'ProductNotFound' )
, fileMode    = cms.untracked.string( 'FULLMERGE' )
, wantSummary = cms.untracked.bool( True )
)
process.load( "FWCore.MessageService.MessageLogger_cfi" )

# Conditions and services
process.load( "Configuration.StandardSequences.Services_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.globaltag = 'GR_R_36X_V9::All'
process.load( "Configuration.StandardSequences.GeometryIdeal_cff" )
process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )

# Reconstruction
process.load( "Configuration.StandardSequences.RawToDigi_Data_cff" )
process.load( "Configuration.StandardSequences.Reconstruction_cff" )
# Event cleaning
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
# DQM
process.load( "DQMOffline.Configuration.DQMOffline_cff" )
process.load( "DQMOffline.Configuration.DQMOffline_SecondStep_cff" )
process.DQMStore.referenceFileName = ''
process.DQMStore.collateHistograms = False
process.load( "DQMServices.Components.DQMEnvironment_cfi" )
process.dqmSaver.convention = 'Offline'
process.dqmSaver.workflow   = '/MinimumBias/CMSSW_3_7_X/RECO'
process.dqmSaver.dirName    = '/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_3_7_0_patch2/output'
process.load( "DQMOffline.Configuration.DQMOffline_Certification_cff" )
# # L1 masking overrides
# process.load( 'L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskAlgoTrigConfig_cff' )
# process.load( 'L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff' )

# Input
process.source = cms.Source( "PoolSource"
, fileNames = cms.untracked.vstring(
    # 132440
    '/store/data/Commissioning10/MinimumBias/RAW-RECO/Apr1Skim_GOODCOLL-v1/0139/FA7B208C-B33E-DF11-A713-003048679010.root', # 7831 events
    # 132658
    '/store/data/Commissioning10/MinimumBias/RAW-RECO/v8/000/132/658/E8443DDC-AF41-DF11-90D2-003048D4777E.root'
    ## 133483
    #'/store/data/Commissioning10/MinimumBias/RAW/v4/000/133/483/FE6CB7B6-E14A-DF11-BF6D-000423D98E54.root',
  )
, skipEvents    = cms.untracked.uint32( 7800 )
, inputCommands = cms.untracked.vstring(
    'keep *'
  , 'drop *_hltL1GtObjectMap_*_*'
  )
)
process.maxEvents = cms.untracked.PSet(
#   input = cms.untracked.int32( 100 )
  input = cms.untracked.int32( 3 )
)

# process.SiStripMonitorTrack.andOr          = False
# process.SiStripMonitorTrack.dbLabel        = cms.string( "test" )
process.SiStripMonitorTrack.verbosityLevel = cms.uint32( 2 )
# process.SiStripMonitorTrack.dcsInputTag   = cms.InputTag( "scalersRawToDigi" )
# process.SiStripMonitorTrack.dcsPartitions = cms.vint32 (
#   24
# , 25
# , 26
# , 27
# )
# process.SiStripMonitorTrack.andOrDcs      = cms.bool( False )
# process.SiStripMonitorTrack.errorReplyDcs = cms.bool( True )
# process.SiStripMonitorTrack.gtInputTag    = cms.InputTag( "gtDigis" )
# process.SiStripMonitorTrack.gtDBKey       = cms.string( 'SiStripDQM_Gt' )
# process.SiStripMonitorTrack.gtStatusBits  = cms.vstring(
#   'PhysicsDeclared'
# )
# process.SiStripMonitorTrack.andOrGt       = cms.bool( False )
# process.SiStripMonitorTrack.errorReplyGt  = cms.bool( False )
# process.SiStripMonitorTrack.l1DBKey       = 'SiStripDQM_L1'
# process.SiStripMonitorTrack.l1Algorithms  = [
# #   'L1Tech_BPTX_plus_AND_minus.v0'                                        # 0
# # , 'L1Tech_BSC_minBias_threshold1.v0 OR L1Tech_BSC_minBias_threshold2.v0' # 40 OR 41
#   'L1Tech_BSC_minBias_threshold2.v0'                                     # 41
# , 'NOT L1Tech_BSC_halo_beam2_inner.v0'                                   # NOT 36
# , 'NOT L1Tech_BSC_halo_beam2_outer.v0'                                   # NOT 37
# , 'NOT L1Tech_BSC_halo_beam1_inner.v0'                                   # NOT 38
# , 'NOT L1Tech_BSC_halo_beam1_outer.v0'                                   # NOT 39
# , 'NOT (L1Tech_BSC_splash_beam1.v0 AND NOT L1Tech_BSC_splash_beam2.v0)'  # NOT (42 AND NOT 43)
# , 'NOT (L1Tech_BSC_splash_beam2.v0 AND NOT L1Tech_BSC_splash_beam1.v0)'  # NOT (43 AND NOT 42)
# ]
# process.SiStripMonitorTrack.andOrL1       = False
# process.SiStripMonitorTrack.l1BeforeMask  = cms.bool( True )
# process.SiStripMonitorTrack.errorReplyL1  = True
# process.SiStripMonitorTrack.hltInputTag   = cms.InputTag( "TriggerResults::HLT" )
# process.SiStripMonitorTrack.hltDBKey      = cms.string( '' )
# process.SiStripMonitorTrack.hltPaths      = cms.vstring(
#   'HLT_L1SingleTauJet'
# )
# process.SiStripMonitorTrack.andOrHlt      = cms.bool( False )
# process.SiStripMonitorTrack.errorReplyHlt = cms.bool( False )

# process.MonitorTrackResiduals.andOr          = False
# process.MonitorTrackResiduals.dbLabel        = cms.string( "" )
process.MonitorTrackResiduals.verbosityLevel = cms.uint32( 2 )
# process.MonitorTrackResiduals.dcsInputTag   = cms.InputTag( "scalersRawToDigi" )
# process.MonitorTrackResiduals.dcsPartitions = cms.vint32 (
#   24
# , 25
# , 26
# , 27
# )
# process.MonitorTrackResiduals.andOrDcs      = cms.bool( False )
# process.MonitorTrackResiduals.errorReplyDcs = cms.bool( True )
# process.MonitorTrackResiduals.gtInputTag    = cms.InputTag( "gtDigis" )
# process.MonitorTrackResiduals.gtDBKey       = cms.string( 'SiStripDQM_Gt' )
# process.MonitorTrackResiduals.gtStatusBits  = cms.vstring(
#   'PhysicsDeclared'
# )
# process.MonitorTrackResiduals.andOrGt       = cms.bool( False )
# process.MonitorTrackResiduals.errorReplyGt  = cms.bool( False )
# process.MonitorTrackResiduals.l1DBKey       = 'SiStripDQM_L1'
# process.MonitorTrackResiduals.l1Algorithms  = [
# #   'L1Tech_BPTX_plus_AND_minus.v0'                                        # 0
# # , 'L1Tech_BSC_minBias_threshold1.v0 OR L1Tech_BSC_minBias_threshold2.v0' # 40 OR 41
#   'L1Tech_BSC_minBias_threshold2.v0'                                     # 41
# , 'NOT L1Tech_BSC_halo_beam2_inner.v0'                                   # NOT 36
# , 'NOT L1Tech_BSC_halo_beam2_outer.v0'                                   # NOT 37
# , 'NOT L1Tech_BSC_halo_beam1_inner.v0'                                   # NOT 38
# , 'NOT L1Tech_BSC_halo_beam1_outer.v0'                                   # NOT 39
# , 'NOT (L1Tech_BSC_splash_beam1.v0 AND NOT L1Tech_BSC_splash_beam2.v0)'  # NOT (42 AND NOT 43)
# , 'NOT (L1Tech_BSC_splash_beam2.v0 AND NOT L1Tech_BSC_splash_beam1.v0)'  # NOT (43 AND NOT 42)
# ]
# process.MonitorTrackResiduals.andOrL1       = False
# process.MonitorTrackResiduals.l1BeforeMask  = cms.bool( True )
# process.MonitorTrackResiduals.errorReplyL1  = True
process.MonitorTrackResiduals.hltInputTag   = cms.InputTag( "TriggerResults::HLT" )
process.MonitorTrackResiduals.hltDBKey      = cms.string( '' )
process.MonitorTrackResiduals.hltPaths      = cms.vstring(
  'HLT_L1SingleTauJet'
)
process.MonitorTrackResiduals.andOrHlt      = cms.bool( False )
process.MonitorTrackResiduals.errorReplyHlt = cms.bool( False )

# process.TrackerCollisionTrackMon.andOr         = false
# process.TrackerCollisionTrackMon.dbLabel        = cms.string( "" )
process.TrackerCollisionTrackMon.verbosityLevel = cms.uint32( 2 )
# process.TrackerCollisionTrackMon.dcsInputTag   = "scalersRawToDigi"
# process.TrackerCollisionTrackMon.dcsPartitions = [
#   24
# , 25
# , 26
# , 27
# , 28
# , 29
# ]
# process.TrackerCollisionTrackMon.andOrDcs      = False
# process.TrackerCollisionTrackMon.errorReplyDcs = True
# process.TrackerCollisionTrackMon.gtInputTag    = cms.InputTag( "gtDigis" )
# process.TrackerCollisionTrackMon.gtDBKey       = cms.string( 'SiStripDQM_Gt' )
# process.TrackerCollisionTrackMon.gtStatusBits  = cms.vstring(
#   'PhysicsDeclared'
# )
# process.TrackerCollisionTrackMon.andOrGt       = cms.bool( False )
# process.TrackerCollisionTrackMon.errorReplyGt  = cms.bool( True )
# process.TrackerCollisionTrackMon.l1DBKey       = 'SiStripDQM_L1'
# process.TrackerCollisionTrackMon.l1Algorithms  = [
# #   'L1Tech_BPTX_plus_AND_minus.v0'                                        # 0
# # , 'L1Tech_BSC_minBias_threshold1.v0 OR L1Tech_BSC_minBias_threshold2.v0' # 40 OR 41
#   'L1Tech_BSC_minBias_threshold2.v0'                                     # 41
# , 'NOT L1Tech_BSC_halo_beam2_inner.v0'                                   # NOT 36
# , 'NOT L1Tech_BSC_halo_beam2_outer.v0'                                   # NOT 37
# , 'NOT L1Tech_BSC_halo_beam1_inner.v0'                                   # NOT 38
# , 'NOT L1Tech_BSC_halo_beam1_outer.v0'                                   # NOT 39
# , 'NOT (L1Tech_BSC_splash_beam1.v0 AND NOT L1Tech_BSC_splash_beam2.v0)'  # NOT (42 AND NOT 43)
# , 'NOT (L1Tech_BSC_splash_beam2.v0 AND NOT L1Tech_BSC_splash_beam1.v0)'  # NOT (43 AND NOT 42)
# ]
# process.TrackerCollisionTrackMon.andOrL1       = False
# process.TrackerCollisionTrackMon.l1BeforeMask  = cms.bool( True )
# process.TrackerCollisionTrackMon.errorReplyL1  = True
# process.TrackerCollisionTrackMon.hltInputTag   = cms.InputTag( "TriggerResults::HLT" )
# process.TrackerCollisionTrackMon.hltDBKey      = cms.string( '' )
# process.TrackerCollisionTrackMon.hltPaths      = cms.vstring(
#   'HLT_L1SingleTauJet'
# )
# process.TrackerCollisionTrackMon.andOrHlt      = cms.bool( False )
# process.TrackerCollisionTrackMon.errorReplyHlt = cms.bool( False )

# DB accesses

# SiStrip AlCaRecoTriggerBits for logical expressions in GenericTriggerEventFlag
import CondCore.DBCommon.CondDBCommon_cfi
process.dbTrackerTriggerBits = cms.ESSource( "PoolDBESSource"
, CondCore.DBCommon.CondDBCommon_cfi.CondDBCommon
, toGet   = cms.VPSet(
    # SiStrip
    cms.PSet(
#       connect = cms.untracked.string( 'sqlite_file:/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_3_7_0_patch2/output/AlCaRecoTriggerBits_SiStripDQM.db' )
#       connect = cms.untracked.string( 'frontier://FrontierPrep/CMS_COND_STRIP' )
      connect = cms.untracked.string( 'frontier://FrontierProd/CMS_COND_31X_STRIP' )
    , record  = cms.string( 'AlCaRecoTriggerBitsRcd' )
    , tag     = cms.string( 'AlCaRecoTriggerBits_SiStripDQM_v0_express' )
#     , label   = cms.untracked.string( 'AlCaRecoTriggerBits_SiStripDQM_v0_express' )
    )
  )
)
process.es_prefer_trackerDqmTriggerBits = cms.ESPrefer( "PoolDBESSource", "dbTrackerTriggerBits" )

# SiStrip DQMXMLFile for quality tests
process.dbTrackerQTests = cms.ESSource( "PoolDBESSource"
, CondCore.DBCommon.CondDBCommon_cfi.CondDBCommon
, toGet   = cms.VPSet(
    # SiStrip
    cms.PSet(
#       connect = cms.untracked.string( 'sqlite_file:/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_3_7_0_patch2/output/DQMXMLFile_SiStripDQM.db' )
#       connect = cms.untracked.string( 'frontier://FrontierPrep/CMS_COND_STRIP' )
      connect = cms.untracked.string( 'frontier://FrontierProd/CMS_COND_31X_STRIP' )
    , record  = cms.string( 'DQMXMLFileRcd' )
    , tag     = cms.string( 'DQMXMLFile_SiStripDQM_v0_express' )
#     , label   = cms.untracked.string( 'DQMXMLFile_SiStripDQM_v0_express' )
    )
    # SiPixel
  , cms.PSet(
#       connect = cms.untracked.string( 'sqlite_file:/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_3_7_0_patch2/output/DQMXMLFile_SiPixelDQM.db' )
#       connect = cms.untracked.string( 'frontier://FrontierPrep/CMS_COND_PIXEL' )
      connect = cms.untracked.string( 'frontier://FrontierProd/CMS_COND_31X_PIXEL' )
    , record  = cms.string( 'DQMXMLFileRcd' )
    , tag     = cms.string( 'DQMXMLFile_SiPixelDQM_v0_express' )
#     , label   = cms.untracked.string( 'DQMXMLFile_SiPixelDQM_v0_express' )
    )
  )
)
# process.es_prefer_trackerDqmQTests = cms.ESPrefer( "PoolDBESSource", "dbTrackerQTests" )
process.siStripQTester.getQualityTestsFromFile = False
#process.siStripQTester.label                   = cms.untracked.string( 'DQMXMLFile_SiStripDQM_v0_express' )

# Scheduling

# Sequences
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

# Paths
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

# Output
process.out = cms.OutputModule( "PoolOutputModule"
, fileName       = cms.untracked.string( '/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_3_7_0_patch2/output/SiStripDQM_Tier0Collisions.root' )
, SelectEvents   = cms.untracked.PSet(
    SelectEvents = cms.vstring(
      'path'
    )
  )
, outputCommands = cms.untracked.vstring(
    'drop *'
  , 'keep *_*_*_TEST'
  )
)

# Endpaths
process.outpath = cms.EndPath(
  process.out
)
