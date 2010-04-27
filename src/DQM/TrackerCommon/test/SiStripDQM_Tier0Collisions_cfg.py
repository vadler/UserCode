import FWCore.ParameterSet.Config as cms

# Process
process = cms.Process( "TEST" )

process.load( "FWCore.MessageService.MessageLogger_cfi" )

process.load( "Configuration.StandardSequences.Services_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.globaltag = 'GR_R_36X_V6::All'
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
process.dqmSaver.workflow   = '/MinimumBias/CMSSW_3_5_X/RECO'
process.dqmSaver.dirName    = '/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_3_6_X_2010-04-26-1100/output'
process.load( "DQMOffline.Configuration.DQMOffline_Certification_cff" )

# Input
process.source = cms.Source( "PoolSource",
  fileNames = cms.untracked.vstring(
    # 123596 RAW
    #'/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/FA54A056-42E2-DE11-A6DB-001617E30D40.root', # 20054 events
    #'/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/E8477DE8-38E2-DE11-9DAB-0016177CA778.root',
    #'/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/E2EFCB1E-3FE2-DE11-9378-001D09F2438A.root'
    # 123591 RAW
    #'/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/591/E002A67B-2EE2-DE11-A64A-001617C3B77C.root'
    # 123510 RAW (different trigger key compared to run 123596)
    #'/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/510/EE215294-CAE0-DE11-87F6-003048D37560.root'
    # 124120 RAW
    #'/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/124/120/F6ADE109-6BE8-DE11-9680-000423D991D4.root',
    #'/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/124/120/ECF0E939-68E8-DE11-A59D-003048D2C1C4.root',
    #'/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/124/120/E2071E9D-6EE8-DE11-AD98-0016177CA7A0.root'
    # 132440 RAW
    #'/store/data/Commissioning10/MinimumBias/RAW/v4/000/132/440/EA896425-F93B-DF11-B971-000423D99AAE.root', # 16250 events
    '/store/data/Commissioning10/MinimumBias/RAW-RECO/Apr1Skim_GOODCOLL-v1/0139/FA7B208C-B33E-DF11-A713-003048679010.root', # 7831 events
    # 132658 RAW
    #'/store/data/Commissioning10/MinimumBias/RAW/v4/000/132/658/FE5A8C16-7341-DF11-A72A-0015C5FDE067.root' # 18612 events
    '/store/data/Commissioning10/MinimumBias/RAW-RECO/v8/000/132/658/E8443DDC-AF41-DF11-90D2-003048D4777E.root'
  ),
  #lumisToProcess = cms.untracked.VLuminosityBlockRange(
    #'132440:85-132440:138',
    #'132440:141-132440:401',
    #'132473:1-132473:29',
    #'132476:23-132476:28',
    #'132476:54-132476:57',
    #'132477:1-132477:5',
    #'132477:34-132477:35',
    #'132477:63-132477:64',
    #'132477:90-132477:93',
    #'132477:118-132477:121',
    #'132477:148-132477:149',
    #'132477:176-132477:179',
    #'132477:225-132477:236',
    #'132477:368-132477:384',
    #'132477:517-132477:520',
    #'132569:222-132569:224',
    #'132569:310-132569:310',
    #'132569:411-132569:419',
    #'132569:529-132569:582',
    #'132596:382-132596:383',
    #'132596:447-132596:453',
    #'132598:80-132598:82',
    #'132598:174-132598:188',
    #'132599:1-132599:74',
    #'132601:261-132601:1131',
    #'132602:1-132602:83',
    #'132605:1-132605:444',
    #'132605:446-132605:622',
    #'132605:624-132605:829',
    #'132605:831-132605:968',
    #'132606:1-132606:37',
  #),
  skipEvents    = cms.untracked.uint32( 7800 ),
  inputCommands = cms.untracked.vstring(
    'keep *'
  , 'drop *_hltL1GtObjectMap_*_*'
  )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 100 )
)
process.options = cms.untracked.PSet(
  Rethrow     = cms.untracked.vstring( 'ProductNotFound' ),
  fileMode    = cms.untracked.string( 'FULLMERGE' ),
  wantSummary = cms.untracked.bool( True )
)

# Filter modules
process.physicsBitSelector = cms.EDFilter( "PhysDecl",
  applyfilter = cms.untracked.bool( True ) ,
  debugOn     = cms.untracked.bool( False )
)
process.load( 'L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff' )
process.load( 'HLTrigger.HLTfilters.hltLevel1GTSeed_cfi' )
process.hltLevel1GTSeed.L1TechTriggerSeeding     = True
# process.hltLevel1GTSeed.L1SeedsLogicalExpression = '0 AND ( 40 OR 41 ) AND NOT ( 36 OR 37 OR 38 OR 39 )'
process.hltLevel1GTSeed.L1SeedsLogicalExpression = '0 AND NOT ( 36 OR 37 OR 38 OR 39 )'
process.load( 'HLTrigger.HLTfilters.hltHighLevel_cfi' )
# process.hltHighLevel.HLTPaths = [ 'HLT_ZeroBias1kHz'
#                                 ]
# process.hltHighLevel.andOr    = True

import CondCore.DBCommon.CondDBSetup_cfi
process.dbInput = cms.ESSource( "PoolDBESSource"
                              , CondCore.DBCommon.CondDBSetup_cfi.CondDBSetup
                              , connect = cms.string( 'sqlite_file:/afs/cern.ch/user/v/vadler/scratch0/cms/SiStripDQM/CMSSW_3_5_6/output/TrackerDQMTriggerBits.db' )
                              , toGet   = cms.VPSet( cms.PSet( record = cms.string( 'AlCaRecoTriggerBitsRcd' )
                                                             , tag    = cms.string( 'TrackerDQMTriggerBits_v0_hlt' )
                                                             )
                                                   )
                              )
process.es_prefer_trackerDqm = cms.ESPrefer( "PoolDBESSource", "dbInput" )
# process.SiStripMonitorTrack.andOr         = True
# process.SiStripMonitorTrack.dcsInputTag   = "scalersRawToDigi"
# process.SiStripMonitorTrack.dcsPartitions = [ 24
#                                             , 25
#                                             , 26
#                                             , 27
#                                             ]
# process.SiStripMonitorTrack.andOrDcs      = True
# process.SiStripMonitorTrack.errorReplyDcs = True
# process.SiStripMonitorTrack.gtInputTag    = "gtDigis"
# process.SiStripMonitorTrack.gtDBKey       = 'TrackerDQM_Gt'
# process.SiStripMonitorTrack.gtStatusBits  = [ 'PhysicsDeclared'
#                                             ]
# process.SiStripMonitorTrack.andOrGt       = False
# process.SiStripMonitorTrack.errorReplyGt  = False
# process.SiStripMonitorTrack.l1DBKey       = cms.string( 'TrackerDqmL1Tracking' )
# process.SiStripMonitorTrack.l1Algorithms  = cms.vstring()
# process.SiStripMonitorTrack.andOrL1       = cms.bool( False )
# process.SiStripMonitorTrack.errorReplyL1  = cms.bool( False )
process.SiStripMonitorTrack.hltInputTag   = cms.InputTag( "TriggerResults::HLT" )
process.SiStripMonitorTrack.hltDBKey      = cms.string( '' )
process.SiStripMonitorTrack.hltPaths      = cms.vstring( 'HLT_MinBias'
                                                       )
process.SiStripMonitorTrack.andOrHlt      = cms.bool( False )
process.SiStripMonitorTrack.errorReplyHlt = cms.bool( False )

# process.TrackerCollisionTrackMon.andOr         = True
# process.TrackerCollisionTrackMon.dcsInputTag   = "scalersRawToDigi"
# process.TrackerCollisionTrackMon.dcsPartitions = [ 24
#                                                  , 25
#                                                  , 26
#                                                  , 27
#                                                  ]
# process.TrackerCollisionTrackMon.andOrDcs      = True
# process.TrackerCollisionTrackMon.errorReplyDcs = True
# process.TrackerCollisionTrackMon.gtInputTag    = "gtDigis"
# process.TrackerCollisionTrackMon.gtDBKey       = 'TrackerDQM_Gt'
# process.TrackerCollisionTrackMon.gtStatusBits  = [ 'PhysicsDeclared'
#                                                  ]
# process.TrackerCollisionTrackMon.andOrGt       = False
# process.TrackerCollisionTrackMon.errorReplyGt  = False
#process.TrackerCollisionTrackMon.l1DBKey       = cms.string( 'TrackerDQM_L1_Tracking' )
process.TrackerCollisionTrackMon.l1DBKey       = cms.string( 'TrackerDQM_L1_Tracking' )
process.TrackerCollisionTrackMon.l1Algorithms  = cms.vstring( 'L1Tech_BPTX_plus_AND_minus.v0'
                                                            , 'L1Tech_BSC_minBias_threshold1.v0 OR L1Tech_BSC_minBias_threshold2.v0'
                                                            , '~L1Tech_BSC_halo_beam2_inner.v0'
                                                            , '~L1Tech_BSC_halo_beam2_outer.v0'
                                                            , '~L1Tech_BSC_halo_beam1_inner.v0'
                                                            , '~L1Tech_BSC_halo_beam1_outer.v0'
                                                            )
process.TrackerCollisionTrackMon.andOrL1       = cms.bool( False )
process.TrackerCollisionTrackMon.errorReplyL1  = cms.bool( False )
# process.TrackerCollisionTrackMon.hltInputTag   = cms.InputTag( "TriggerResults::HLT" )
# process.TrackerCollisionTrackMon.hltDBKey      = cms.string( '' )
# process.TrackerCollisionTrackMon.hltPaths      = cms.vstring()
# process.TrackerCollisionTrackMon.andOrHlt      = cms.bool( False )
# process.TrackerCollisionTrackMon.errorReplyHlt = cms.bool( False )

process.raw2Digi = cms.Sequence(
  process.scalersRawToDigi +
  process.siPixelDigis     +
  process.siStripDigis
)
process.reco = cms.Sequence(
  process.trackerlocalreco   *
  process.offlineBeamSpot    *
  process.recopixelvertexing *
  process.ckftracks          *
  process.vertexreco         *
  process.ctfTracksPixelLess *
  process.logErrorHarvester
)
process.dqm = cms.Sequence(
  process.SiStripDQMTier0  *
  process.DQMMessageLogger
)

process.path = cms.Path(
  # preparation
  process.gtDigis            *
#   process.physicsBitSelector *
#   process.hltLevel1GTSeed    *
#   process.hltHighLevel       *
  process.raw2Digi           *
  process.reco               *
  # DQM sources
  process.dqm                *
  # DQM client
  process.dqmRefHistoRootFileGetter *
  process.SiStripOfflineDQMClient   *
  process.DQMMessageLoggerClient    *
  process.siStripDaqInfo           *
  process.siStripDcsInfo           *
  process.siStripCertificationInfo *
  process.dqmSaver
)

process.out = cms.OutputModule( "PoolOutputModule",
  fileName       = cms.untracked.string( '/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_3_6_X_2010-04-26-1100/output/SiStripDQM_Tier0Collisions.root' ),
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
