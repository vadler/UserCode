import FWCore.ParameterSet.Config as cms

from DQM.SiStripCommon.SiStripTriggerHelper_cff import SiStripHltFilter_TrackingMonitor

TrackMon = cms.EDFilter("TrackingMonitor",
    SiStripHltFilter_TrackingMonitor,

    # input tags
    TrackProducer = cms.InputTag("generalTracks"),
    SeedProducer  = cms.InputTag("newSeedFromTriplets"),
    TCProducer    = cms.InputTag("newTrackCandidateMaker"),
    beamSpot      = cms.InputTag("offlineBeamSpot"),

    # output parameters
    OutputMEsInRootFile = cms.bool(False),
    AlgoName            = cms.string('GenTk'),
    Quality             = cms.string(''),
    OutputFileName      = cms.string('MonitorTrack.root'),
    FolderName          = cms.string('Tracking/GlobalParameters'),
    BSFolderName        = cms.string('Tracking/BeamSpotParameters'),

    # determines where to evaluate track parameters
    # options: 'default'      --> straight up track parametes
    #		   'ImpactPoint'  --> evalutate at impact point
    #		   'InnerSurface' --> evalutate at innermost measurement state
    #		   'OuterSurface' --> evalutate at outermost measurement state
    MeasurementState = cms.string('ImpactPoint'),

    # which plots to do
    doTrackerSpecific     = cms.bool(True),
    doAllPlots            = cms.bool(False),
    doBeamSpotPlots       = cms.bool(True),
    doSeedParameterHistos = cms.bool(True),

    # paramters of the Track
    # ============================================================

    # chi2
    Chi2Bin = cms.int32(100),
    Chi2Max = cms.double(199.5),
    Chi2Min = cms.double(-0.5),

    # chi2 dof
    Chi2NDFBin = cms.int32(100),
    Chi2NDFMax = cms.double(19.5),
    Chi2NDFMin = cms.double(-0.5),

    # chi^2 probability
    Chi2ProbBin = cms.int32(100),
    Chi2ProbMax = cms.double(1.0),
    Chi2ProbMin = cms.double(0.0),

    # Number of Tracks per Event
    TkSizeBin = cms.int32(100),
    TkSizeMax = cms.double(99.5),
    TkSizeMin = cms.double(-0.5),

    # Number of seeds per Event
    TkSeedSizeBin = cms.int32(150),
    TkSeedSizeMax = cms.double(149.5),
    TkSeedSizeMin = cms.double(-0.5),

    # Number of Track Cadidates per Event
    TCSizeBin = cms.int32(150),
    TCSizeMax = cms.double(149.5),
    TCSizeMin = cms.double(-0.5),

    # num rec hits
    TrackQBin = cms.int32(8),
    TrackQMax = cms.double(2.5),
    TrackQMin = cms.double(-2.5),

    # num rec hits in seed
    SeedHitBin = cms.int32(6),
    SeedHitMax = cms.double(5.5),
    SeedHitMin = cms.double(-0.5),

    # num rec hits per track candidate
    TCHitBin = cms.int32(40),
    TCHitMax = cms.double(39.5),
    TCHitMin = cms.double(-0.5),

    # num rec hits
    RecHitBin = cms.int32(40),
    RecHitMax = cms.double(39.5),
    RecHitMin = cms.double(-0.5),

    # mean rec hits
    MeanHitBin = cms.int32(30),
    MeanHitMax = cms.double(29.5),
    MeanHitMin = cms.double(-0.5),

    # num TOB rec hits
    TOBHitBin = cms.int32(15),
    TOBHitMax = cms.double(14.5),
    TOBHitMin = cms.double(-0.5),

    # num TIB rec hits
    TIBHitBin = cms.int32(15),
    TIBHitMax = cms.double(14.5),
    TIBHitMin = cms.double(-0.5),

    # num TID rec hits
    TIDHitBin = cms.int32(15),
    TIDHitMax = cms.double(14.5),
    TIDHitMin = cms.double(-0.5),

    # num TEC rec hits
    TECHitBin = cms.int32(25),
    TECHitMax = cms.double(24.5),
    TECHitMin = cms.double(-0.5),

    # num PXB rec hits
    PXBHitBin = cms.int32(10),
    PXBHitMax = cms.double(9.5),
    PXBHitMin = cms.double(-0.5),

    # num PXF rec hits
    PXFHitBin = cms.int32(10),
    PXFHitMax = cms.double(9.5),
    PXFHitMin = cms.double(-0.5),

    # num rec hits lost
    RecLostBin = cms.int32(10),
    RecLostMax = cms.double(9.5),
    RecLostMin = cms.double(-0.5),

    # num layers
    RecLayBin = cms.int32(25),
    RecLayMax = cms.double(24.5),
    RecLayMin = cms.double(-0.5),

    # mean layers
    MeanLayBin = cms.int32(20),
    MeanLayMax = cms.double(19.5),
    MeanLayMin = cms.double(-0.5),

    # num TOB Layers
    TOBLayBin = cms.int32(10),
    TOBLayMax = cms.double(9.5),
    TOBLayMin = cms.double(-0.5),

    # num TIB Layers
    TIBLayBin = cms.int32(6),
    TIBLayMax = cms.double(5.5),
    TIBLayMin = cms.double(-0.5),

    # num TID Layers
    TIDLayBin = cms.int32(6),
    TIDLayMax = cms.double(5.5),
    TIDLayMin = cms.double(-0.5),

    # num TEC Layers
    TECLayBin = cms.int32(15),
    TECLayMax = cms.double(14.5),
    TECLayMin = cms.double(-0.5),

    # num PXB Layers
    PXBLayBin = cms.int32(6),
    PXBLayMax = cms.double(5.5),
    PXBLayMin = cms.double(-0.5),

    # num PXF Layers
    PXFLayBin = cms.int32(6),
    PXFLayMax = cms.double(5.5),
    PXFLayMin = cms.double(-0.5),

    # Track |p|
    TrackPBin = cms.int32(100),
    TrackPMax = cms.double(100),
    TrackPMin = cms.double(0),

    # Track pT
    TrackPtBin = cms.int32(100),
    TrackPtMax = cms.double(100),
    TrackPtMin = cms.double(0.1),

    # Track px
    TrackPxBin = cms.int32(100),
    TrackPxMax = cms.double(20.0),
    TrackPxMin = cms.double(-20.0),

    # Track py
    TrackPyBin = cms.int32(100),
    TrackPyMax = cms.double(20.0),
    TrackPyMin = cms.double(-20.0),

    # Track pz
    TrackPzBin = cms.int32(100),
    TrackPzMax = cms.double(30.0),
    TrackPzMin = cms.double(-30.0),

    # track theta
    ThetaBin = cms.int32(100),
    ThetaMax = cms.double(3.2),
    ThetaMin = cms.double(0.0),

    # track eta
    EtaBin = cms.int32(80),
    EtaMax = cms.double(4.0),
    EtaMin = cms.double(-4.0),

    # track phi
    PhiBin = cms.int32(36),
    PhiMax = cms.double(3.2),
    PhiMin = cms.double(-3.2),

    # Track |p|	error
    pErrBin = cms.int32(100),
    pErrMax = cms.double(0.1),
    pErrMin = cms.double(0.0),

    # Track pT error
    ptErrBin = cms.int32(100),
    ptErrMax = cms.double(0.1),
    ptErrMin = cms.double(0.0),

    # Track px error
    pxErrBin = cms.int32(100),
    pxErrMax = cms.double(0.5),
    pxErrMin = cms.double(0.0),

    # Track py error
    pyErrBin = cms.int32(100),
    pyErrMax = cms.double(0.5),
    pyErrMin = cms.double(0.0),

    # Track pz error
    pzErrBin = cms.int32(100),
    pzErrMax = cms.double(0.5),
    pzErrMin = cms.double(0.0),

    # track eta error
    etaErrBin = cms.int32(100),
    etaErrMax = cms.double(0.1),
    etaErrMin = cms.double(0.0),

    # track phi Error
    phiErrBin = cms.int32(100),
    phiErrMax = cms.double(0.1),
    phiErrMin = cms.double(0.0),

    # PCA x position
    VXBin = cms.int32(100),
    VXMax = cms.double(0.5),
    VXMin = cms.double(-0.5),

    # PCA y position
    VYBin = cms.int32(100),
    VYMax = cms.double(0.5),
    VYMin = cms.double(-0.5),

    # PCA z position
    VZBin = cms.int32(100),
    VZMax = cms.double(30.0),
    VZMin = cms.double(-30.0),

    # PCA x position for 2D plot
    X0Bin = cms.int32(100),
    X0Max = cms.double(0.5),
    X0Min = cms.double(-0.5),

    # PCA y position for 2D plot
    Y0Bin = cms.int32(100),
    Y0Max = cms.double(0.5),
    Y0Min = cms.double(-0.5),

    # PCA z position for 2D plot
    Z0Bin = cms.int32(120),
    Z0Max = cms.double(60.0),
    Z0Min = cms.double(-60.0),

    # Track dxy (transverse impact parameter)
    DxyBin = cms.int32(100),
    DxyMax = cms.double(0.5),
    DxyMin = cms.double(-0.5),

    # Seed dxy (transverse impact parameter)
    SeedDxyBin = cms.int32(100),
    SeedDxyMax = cms.double(0.5),
    SeedDxyMin = cms.double(-0.5),

    # Seed dz (longitudinal impact parameter)
    SeedDzBin = cms.int32(120),
    SeedDzMax = cms.double(30.0),
    SeedDzMin = cms.double(-30.0),

    # Track Candidate dxy (transverse impact parameter)
    TCDxyBin = cms.int32(200),
    TCDxyMax = cms.double(100.0),
    TCDxyMin = cms.double(-100.0),

    # Track Candidate dz (transverse impact parameter)
    TCDzBin = cms.int32(200),
    TCDzMax = cms.double(400.0),
    TCDzMin = cms.double(-400.0),

    TTRHBuilder = cms.string('WithTrackAngle')
)
