import FWCore.ParameterSet.Config as cms

# FED integrity Check
from DQM.SiStripMonitorHardware.siStripFEDCheck_cfi import *
siStripFEDCheck.HistogramUpdateFrequency = 0
siStripFEDCheck.DoPayloadChecks          = True
siStripFEDCheck.CheckChannelLengths      = True
siStripFEDCheck.CheckChannelPacketCodes  = True
siStripFEDCheck.CheckFELengths           = True
siStripFEDCheck.CheckChannelStatus       = True

# FED Monitoring
from DQM.SiStripMonitorHardware.siStripFEDMonitor_Tier0_cff import *

# SiStripMonitorDigi ####
from DQM.SiStripMonitorDigi.SiStripMonitorDigi_cfi import *
SiStripMonitorDigi.Mod_On = False
SiStripMonitorDigi.TProfDigiApvCycle.subdetswitchon = True

# SiStripMonitorCluster ####
from DQM.SiStripMonitorCluster.SiStripMonitorCluster_cfi import *
SiStripMonitorCluster.Mod_On = False
SiStripMonitorCluster.TProfClustersApvCycle.subdetswitchon = True
SiStripMonitorCluster.TProfTotalNumberOfClusters.subdetswitchon = True

# SiStripMonitorTrack ####
from DQM.SiStripMonitorTrack.SiStripMonitorTrack_cfi import *
SiStripMonitorTrack.TrackProducer = 'generalTracks'
SiStripMonitorTrack.Mod_On        = False
SiStripMonitorTrack.FolderName    = 'Tracking/TrackParameters'
# Filters
SiStripMonitorTrack.andOr         = cms.bool( False )
SiStripMonitorTrack.dcsInputTag   = cms.InputTag( "scalersRawToDigi" )
SiStripMonitorTrack.dcsPartitions = cms.vint32( 24
                                              , 25
                                              , 26
                                              , 27
                                              )
SiStripMonitorTrack.andOrDcs      = cms.bool( False )
SiStripMonitorTrack.errorReplyDcs = cms.bool( False )
SiStripMonitorTrack.gtInputTag    = cms.InputTag( "gtDigis" )
SiStripMonitorTrack.gtStatusBits  = cms.vstring( 'PhysicsDeclared'
                                               )
SiStripMonitorTrack.andOrGt       = cms.bool( False )
SiStripMonitorTrack.errorReplyGt  = cms.bool( False )
# SiStripMonitorTrack.l1Algorithms  = cms.vstring()
# SiStripMonitorTrack.andOrL1       = cms.bool( False )
# SiStripMonitorTrack.errorReplyL1  = cms.bool( False )
# SiStripMonitorTrack.hltInputTag   = cms.InputTag( "TriggerResults::HLT" )
# SiStripMonitorTrack.hltPaths      = cms.vstring()
# SiStripMonitorTrack.andOrHlt      = cms.bool( False )
# SiStripMonitorTrack.errorReplyHlt = cms.bool( False )

# TrackerMonitorTrack ####
from DQM.TrackerMonitorTrack.MonitorTrackResiduals_cfi import *
MonitorTrackResiduals.trajectoryInput = 'generalTracks'
MonitorTrackResiduals.OutputMEsInRootFile = False
MonitorTrackResiduals.Mod_On = False

# TrackingMonitor ####
from DQM.TrackingMonitor.TrackerCollisionTrackingMonitor_cfi import *
TrackerCollisionTrackMon.FolderName   = 'Tracking/TrackParameters'
TrackerCollisionTrackMon.BSFolderName = 'Tracking/TrackParameters/BeamSpotParameters'
# Filters
TrackerCollisionTrackMon.andOr         = cms.bool( False )
TrackerCollisionTrackMon.dcsInputTag   = cms.InputTag( "scalersRawToDigi" )
TrackerCollisionTrackMon.dcsPartitions = cms.vint32( 24
                                                   , 25
                                                   , 26
                                                   , 27
                                                   )
TrackerCollisionTrackMon.andOrDcs      = cms.bool( False )
TrackerCollisionTrackMon.errorReplyDcs = cms.bool( False )
TrackerCollisionTrackMon.gtInputTag    = cms.InputTag( "gtDigis" )
TrackerCollisionTrackMon.gtStatusBits  = cms.vstring( 'PhysicsDeclared'
                                                    )
TrackerCollisionTrackMon.andOrGt       = cms.bool( False )
TrackerCollisionTrackMon.errorReplyGt  = cms.bool( False )
# TrackerCollisionTrackMon.l1Algorithms  = cms.vstring()
# TrackerCollisionTrackMon.andOrL1       = cms.bool( False )
# TrackerCollisionTrackMon.errorReplyL1  = cms.bool( False )
# TrackerCollisionTrackMon.hltInputTag   = cms.InputTag( "TriggerResults::HLT" )
# TrackerCollisionTrackMon.hltPaths      = cms.vstring()
# TrackerCollisionTrackMon.andOrHlt      = cms.bool( False )
# TrackerCollisionTrackMon.errorReplyHlt = cms.bool( False )

# DQM Services
dqmInfoSiStrip = cms.EDFilter("DQMEventInfo",
    subSystemFolder = cms.untracked.string('SiStrip')
)

# Services needed for TkHistoMap
TkDetMap = cms.Service("TkDetMap")
SiStripDetInfoFileReade = cms.Service("SiStripDetInfoFileReader")

# Event History Producer
from  DPGAnalysis.SiStripTools.eventwithhistoryproducerfroml1abc_cfi import *

# APV Phase Producer
from DPGAnalysis.SiStripTools.apvcyclephaseproducerfroml1abc_GR09_cfi import *

# Sequence
SiStripDQMTier0 = cms.Sequence(APVPhases*consecutiveHEs*siStripFEDCheck*siStripFEDMonitor*SiStripMonitorDigi*SiStripMonitorCluster*SiStripMonitorTrack*MonitorTrackResiduals*TrackerCollisionTrackMon*dqmInfoSiStrip)


