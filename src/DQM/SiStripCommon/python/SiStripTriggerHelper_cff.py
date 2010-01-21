import FWCore.ParameterSet.Config as cms

SiStripHltFilter_SiStripMonitorHardware = cms.PSet(
    hltInputTag = cms.InputTag( "TriggerResults::HLT" ),
    hltPaths    = cms.vstring(),
    andOr       = cms.bool( False ), # "False": AND, "True": OR
    errorReply  = cms.bool( True )
)

SiStripHltFilter_SiStripMonitorDigi = cms.PSet(
    hltInputTag = cms.InputTag( "TriggerResults::HLT" ),
    hltPaths    = cms.vstring( 'HLT_PhysicsDeclared'
                             ),
    andOr       = cms.bool( False ), # "False": AND, "True": OR
    errorReply  = cms.bool( True )
)

SiStripHltFilter_SiStripMonitorCluster = cms.PSet(
    hltInputTag = cms.InputTag( "TriggerResults::HLT" ),
    hltPaths    = cms.vstring( 'HLT_PhysicsDeclared'
                             ),
    andOr       = cms.bool( False ), # "False": AND, "True": OR
    errorReply  = cms.bool( True )
)

SiStripHltFilter_SiStripMonitorTrack = cms.PSet(
    hltInputTag = cms.InputTag( "TriggerResults::HLT" ),
    hltPaths    = cms.vstring( 'HLT_PhysicsDeclared'
                             , 'HLT_ZeroBias1kHz' # DEBUG
                             ),
    andOr       = cms.bool( False ), # "False": AND, "True": OR
    errorReply  = cms.bool( True )
)

SiStripHltFilter_TrackerMonitorTrack = cms.PSet(
    hltInputTag = cms.InputTag( "TriggerResults::HLT" ),
    hltPaths    = cms.vstring( 'HLT_PhysicsDeclared'
                             ),
    andOr       = cms.bool( False ), # "False": AND, "True": OR
    errorReply  = cms.bool( True )
)

SiStripHltFilter_TrackingMonitor = cms.PSet(
    hltInputTag = cms.InputTag( "TriggerResults::HLT" ),
    hltPaths    = cms.vstring( 'HLT_PhysicsDeclared'
                             ),
    andOr       = cms.bool( False ), # "False": AND, "True": OR
    errorReply  = cms.bool( True )
)
