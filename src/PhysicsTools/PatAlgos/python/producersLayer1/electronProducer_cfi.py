import FWCore.ParameterSet.Config as cms

allLayer1Electrons = cms.EDProducer("PATElectronProducer",
    # input collection
    electronSource = cms.InputTag("gsfElectrons"),

    # use particle flow instead of std reco    
    useParticleFlow  =  cms.bool( False ),
    pfElectronSource = cms.InputTag("pfIsolatedElectrons"),
                                    
    # user data to add
    userData = cms.PSet(
      # add custom classes here
      userClasses = cms.PSet(
        src = cms.VInputTag('')
      ),
      # add doubles here
      userFloats = cms.PSet(
        src = cms.VInputTag('')
      ),
      # add ints here
      userInts = cms.PSet(
        src = cms.VInputTag('')
      ),
      # add candidate ptrs here
      userCands = cms.PSet(
        src = cms.VInputTag('')
      ),
      # add "inline" functions here
      userFunctions = cms.vstring(),
      userFunctionLabels = cms.vstring()
    ),

    # embedding of AOD items
    embedTrack        = cms.bool(False), ## embed in AOD externally stored track (note: gsf electrons don't have a track)
    embedGsfTrack     = cms.bool(True),  ## embed in AOD externally stored gsf track
    embedSuperCluster = cms.bool(True),  ## embed in AOD externally stored supercluster
    embedPFCandidate  = cms.bool(True),  ## embed in AOD externally stored particle flow candidate
                                    
    # embed IsoDeposits to recompute isolation
    isoDeposits = cms.PSet(
       #tracker = cms.InputTag("eleIsoDepositTk"),
       #ecal    = cms.InputTag("eleIsoDepositEcalFromHits"),
       #hcal    = cms.InputTag("eleIsoDepositHcalFromTowers"),
    ),

    # user defined isolation variables the variables defined here will be accessible
    # via pat::Electron::userIsolation(IsolationKeys key) with the key as defined in
    # DataFormats/PatCandidates/interface/Isolation.h
    userIsolation = cms.PSet(
       #tracker = cms.PSet(
       #    src = cms.InputTag("eleIsoFromDepsTk"),
       #),
       #ecal = cms.PSet(
       #    src = cms.InputTag("eleIsoFromDepsEcalFromHitsByCrystal"),
       #),
       #hcal = cms.PSet(
       #    src = cms.InputTag("eleIsoFromDepsHcalFromTowers"),
       #),
       #user = cms.VPSet(),
    ),

    # electron ID
    addElectronID = cms.bool(True),
    electronIDSources = cms.PSet(
        # configure many IDs as InputTag <someName> = <someTag> you
        # can comment out those you don't want to save some disk space
        eidRobustLoose      = cms.InputTag("eidRobustLoose"),
        eidRobustTight      = cms.InputTag("eidRobustTight"),
        eidLoose            = cms.InputTag("eidLoose"),
        eidTight            = cms.InputTag("eidTight"),
        eidRobustHighEnergy = cms.InputTag("eidRobustHighEnergy"),
    ),

    # mc matching
    addGenMatch      = cms.bool(True),
    embedGenMatch    = cms.bool(True),
    genParticleMatch = cms.InputTag("electronMatch"), ## Association between electrons and generator particles
    
    # efficiencies
    addEfficiencies = cms.bool(False),
    efficiencies    = cms.PSet(),

    # resolution configurables
    addResolutions   = cms.bool(False),
    resolutions      = cms.PSet(),

    # high level selections
    embedHighLevelSelection = cms.bool(True),
    beamLineSrc             = cms.InputTag("offlineBeamSpot")
)


