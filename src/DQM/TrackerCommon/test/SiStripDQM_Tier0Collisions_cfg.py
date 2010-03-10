import FWCore.ParameterSet.Config as cms

# Process
process = cms.Process( "TEST" )

process.load( "FWCore.MessageService.MessageLogger_cfi" )

process.load( "Configuration.StandardSequences.Services_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.globaltag = 'GR09_R_35X_V3::All'
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
process.offlinePrimaryVerticesWithBS.TkFilterParameters.minSiliconHits    =   6
process.offlinePrimaryVerticesWithBS.TkFilterParameters.maxD0Significance = 100
process.offlinePrimaryVerticesWithBS.TkFilterParameters.minPixelHits      =   1
process.offlinePrimaryVerticesWithBS.TkClusParameters.zSeparation         =  10
process.offlinePrimaryVertices.PVSelParameters.maxDistanceToBeam          =   2
process.offlinePrimaryVertices.TkFilterParameters.maxNormalizedChi2       =  20
process.offlinePrimaryVertices.TkFilterParameters.minSiliconHits          =   6
process.offlinePrimaryVertices.TkFilterParameters.maxD0Significance       = 100
process.offlinePrimaryVertices.TkFilterParameters.minPixelHits            =   1
process.offlinePrimaryVertices.TkClusParameters.zSeparation               =  10
process.load( "DQMOffline.Configuration.DQMOffline_cff" )
process.load( "DQMOffline.Configuration.DQMOffline_SecondStep_cff" )
process.DQMStore.referenceFileName = ''
process.DQMStore.collateHistograms = False
process.dqmSaver.convention = 'Offline'
process.dqmSaver.workflow   = '/MinimumBias/CMSSW_3_5_X/RECO'
process.dqmSaver.dirName    = '/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_3_5_4/output'
process.load( "DQMOffline.Configuration.DQMOffline_Certification_cff" )

# Input
process.source = cms.Source( "PoolSource",
  fileNames = cms.untracked.vstring(
    # 123596 RAW
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/FA54A056-42E2-DE11-A6DB-001617E30D40.root',
    #'/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/E8477DE8-38E2-DE11-9DAB-0016177CA778.root',
    #'/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/E2EFCB1E-3FE2-DE11-9378-001D09F2438A.root'
#     # 123596 Express FEVT
#     '/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/FEE389F2-33E2-DE11-A62E-001617C3B76E.root',
#     '/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/FE90F72C-43E2-DE11-8E89-001D09F2B30B.root',
#     '/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/FE0807BE-48E2-DE11-9A06-001D09F28F25.root'
    # 124120 RAW
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/124/120/F6ADE109-6BE8-DE11-9680-000423D991D4.root',
    #'/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/124/120/ECF0E939-68E8-DE11-A59D-003048D2C1C4.root',
    #'/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/124/120/E2071E9D-6EE8-DE11-AD98-0016177CA7A0.root'

  ),
  skipEvents = cms.untracked.uint32( 20000 )
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
                              , connect = cms.string( 'sqlite_file:/afs/cern.ch/user/v/vadler/scratch0/cms/SiStripDQM/CMSSW_3_5_4/output/TrackerDQMTriggerBits.db' )
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
# process.SiStripMonitorTrack.gtDBKey       = "TrackerDQM_Gt"
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
process.SiStripMonitorTrack.hltPaths      = cms.vstring( 'HLT_ZeroBias1kHz'
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
# process.TrackerCollisionTrackMon.gtDBKey       = "TrackerDQM_Gt"
# process.TrackerCollisionTrackMon.gtStatusBits  = [ 'PhysicsDeclared'
#                                                  ]
# process.TrackerCollisionTrackMon.andOrGt       = False
# process.TrackerCollisionTrackMon.errorReplyGt  = False
process.TrackerCollisionTrackMon.l1DBKey       = cms.string( 'TrackerDQM_L1_Tracking' )
process.TrackerCollisionTrackMon.l1Algorithms  = cms.vstring()
process.TrackerCollisionTrackMon.l1Algorithms  = cms.vstring()
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
  fileName       = cms.untracked.string( '/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_3_5_4/output/SiStripDQM_Tier0Collisions.root' ),
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
