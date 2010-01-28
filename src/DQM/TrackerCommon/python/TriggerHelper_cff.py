import FWCore.ParameterSet.Config as cms

# "hltInputTag":
# The inout tag has to contain also the process name.
# Removing this parameter completly switches the filter off.
# "hltPaths":
# The filter has no effect, if the vector is empty.
# Paths can be negated by prepending a '~'.
# "andOr":
# False = AND, True = OR
# "errorReplyHlt":
#

SiStripHltFilter_SiStripMonitorHardware = cms.PSet(
    # General
    andOr = cms.bool( False ),
    # HLT
    hltInputTag   = cms.InputTag( "TriggerResults::HLT" ),
    hltPaths      = cms.vstring( 'HLT_PhysicsDeclared'
                               ),
    andOrHlt      = cms.bool( False ),
    errorReplyHlt = cms.bool( False ),
    # L1
    l1Algorithms = cms.vstring(),
    andOrL1      = cms.bool( False ),
    errorReplyL1 = cms.bool( False )
)

SiStripHltFilter_SiStripMonitorDigi = cms.PSet(
    # General
    andOr = cms.bool( False ),
    # HLT
    hltInputTag   = cms.InputTag( "TriggerResults::HLT" ),
    hltPaths      = cms.vstring( 'HLT_PhysicsDeclared'
                               ),
    andOrHlt      = cms.bool( False ),
    errorReplyHlt = cms.bool( False ),
    # L1
    l1Algorithms = cms.vstring(),
    andOrL1      = cms.bool( False ),
    errorReplyL1 = cms.bool( False )
)

SiStripHltFilter_SiStripMonitorCluster = cms.PSet(
    # General
    andOr = cms.bool( False ),
    # HLT
    hltInputTag   = cms.InputTag( "TriggerResults::HLT" ),
    hltPaths      = cms.vstring( 'HLT_PhysicsDeclared'
                               ),
    andOrHlt      = cms.bool( False ),
    errorReplyHlt = cms.bool( False ),
    # L1
    l1Algorithms = cms.vstring(),
    andOrL1      = cms.bool( False ),
    errorReplyL1 = cms.bool( False )
)

SiStripHltFilter_SiStripMonitorTrack = cms.PSet(
    # General
    andOr = cms.bool( False ),
    # HLT
    hltInputTag   = cms.InputTag( "TriggerResults::HLT" ),
    hltPaths      = cms.vstring( 'HLT_PhysicsDeclared'
                               , 'HLT_ZeroBias1kHz'    # DEBUG
                               ),
    andOrHlt      = cms.bool( False ),
    errorReplyHlt = cms.bool( False ),
    # L1
    l1Algorithms = cms.vstring( 'L1Tech_BPTX_plus_AND_minus.v0'    # DEBUG 0
                              , 'L1Tech_BSC_minBias_threshold1.v0' # DEBUG 40
#                               , 'L1Tech_BSC_minBias_threshold2.v0' # DEBUG 41
                              , '~L1Tech_BSC_halo_beam2_inner.v0' # DEBUG ~36
                              , '~L1Tech_BSC_halo_beam2_outer.v0' # DEBUG ~37
                              , '~L1Tech_BSC_halo_beam1_inner.v0' # DEBUG ~38
                              , '~L1Tech_BSC_halo_beam1_outer.v0' # DEBUG ~39
                              ),
    andOrL1      = cms.bool( False ),
    errorReplyL1 = cms.bool( False )
)

SiStripHltFilter_TrackerMonitorTrack = cms.PSet(
    # General
    andOr = cms.bool( False ),
    # HLT
    hltInputTag   = cms.InputTag( "TriggerResults::HLT" ),
    hltPaths      = cms.vstring( 'HLT_PhysicsDeclared'
                               ),
    andOrHlt      = cms.bool( False ),
    errorReplyHlt = cms.bool( False ),
    # L1
    l1Algorithms = cms.vstring(),
    andOrL1      = cms.bool( False ),
    errorReplyL1 = cms.bool( False )
)

SiStripHltFilter_TrackingMonitor = cms.PSet(
    # General
    andOr = cms.bool( False ),
    # HLT
    hltInputTag   = cms.InputTag( "TriggerResults::HLT" ),
    hltPaths      = cms.vstring( 'HLT_PhysicsDeclared'
                               ),
    andOrHlt      = cms.bool( False ),
    errorReplyHlt = cms.bool( False ),
    # L1
    l1Algorithms = cms.vstring(),
    andOrL1      = cms.bool( False ),
    errorReplyL1 = cms.bool( False )
)
