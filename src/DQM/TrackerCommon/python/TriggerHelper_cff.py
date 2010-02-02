import FWCore.ParameterSet.Config as cms

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
    errorReplyL1 = cms.bool( False ),
    # DCS
    dcsInputTag   = cms.InputTag( "scalersRawToDigi" ),
    dcsPartitions = cms.vint32( 24
                              , 25
                              , 26
                              , 27
                              ),
    andOrDcs      = cms.bool( False ),
    errorReplyDcs = cms.bool( False )
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
    errorReplyL1 = cms.bool( False ),
    # DCS
    dcsInputTag   = cms.InputTag( "scalersRawToDigi" ),
    dcsPartitions = cms.vint32( 24
                              , 25
                              , 26
                              , 27
                              ),
    andOrDcs      = cms.bool( False ),
    errorReplyDcs = cms.bool( False )
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
    errorReplyL1 = cms.bool( False ),
    # DCS
    dcsInputTag   = cms.InputTag( "scalersRawToDigi" ),
    dcsPartitions = cms.vint32( 24
                              , 25
                              , 26
                              , 27
                              ),
    andOrDcs      = cms.bool( False ),
    errorReplyDcs = cms.bool( False )
)

SiStripHltFilter_SiStripMonitorTrack = cms.PSet(
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
    errorReplyL1 = cms.bool( False ),
    # DCS
    dcsInputTag   = cms.InputTag( "scalersRawToDigi" ),
    dcsPartitions = cms.vint32( 24
                              , 25
                              , 26
                              , 27
                              ),
    andOrDcs      = cms.bool( False ),
    errorReplyDcs = cms.bool( False )
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
    errorReplyL1 = cms.bool( False ),
    # DCS
    dcsInputTag   = cms.InputTag( "scalersRawToDigi" ),
    dcsPartitions = cms.vint32( 24
                              , 25
                              , 26
                              , 27
                              ),
    andOrDcs      = cms.bool( False ),
    errorReplyDcs = cms.bool( False )
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
    errorReplyL1 = cms.bool( False ),
    # DCS
    dcsInputTag   = cms.InputTag( "scalersRawToDigi" ),
    dcsPartitions = cms.vint32( 24
                              , 25
                              , 26
                              , 27
                              ),
    andOrDcs      = cms.bool( False ),
    errorReplyDcs = cms.bool( False )
)
