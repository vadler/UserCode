import FWCore.ParameterSet.Config as cms

### SiStrip DQM Source

# SiStripMonitorCluster
import DQM.SiStripMonitorCluster.SiStripMonitorCluster_cfi
siStripMonitorClusterBadStrips = DQM.SiStripMonitorCluster.SiStripMonitorCluster_cfi.SiStripMonitorCluster.clone()
siStripMonitorClusterBadStrips.OutputMEsInRootFile                     = False
siStripMonitorClusterBadStrips.SelectAllDetectors                      = True
siStripMonitorClusterBadStrips.StripQualityLabel                       = ''
siStripMonitorClusterBadStrips.TH1ClusterPos.moduleswitchon            = True 
siStripMonitorClusterBadStrips.TH1nClusters.layerswitchon              = True 
siStripMonitorClusterBadStrips.TH1ClusterStoN.moduleswitchon           = True
siStripMonitorClusterBadStrips.TH1ClusterStoNVsPos.moduleswitchon      = True
siStripMonitorClusterBadStrips.TH1ClusterNoise.moduleswitchon          = False
siStripMonitorClusterBadStrips.TH1NrOfClusterizedStrips.moduleswitchon = False
siStripMonitorClusterBadStrips.TH1ModuleLocalOccupancy.moduleswitchon  = False
siStripMonitorClusterBadStrips.TH1nClusters.moduleswitchon             = False
siStripMonitorClusterBadStrips.TH1ClusterStoN.moduleswitchon           = False
siStripMonitorClusterBadStrips.TH1ClusterCharge.moduleswitchon         = False
siStripMonitorClusterBadStrips.TH1ClusterWidth.moduleswitchon          = False

# Scheduling
siStripDQMSourceBadStrips = cms.Sequence(
    siStripMonitorClusterBadStrips
)
