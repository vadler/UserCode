# The following comments couldn't be translated into the new config version:

# Minimum Tk Pt
# inner cone veto (endcaps, |eta| >= 1.479)
import FWCore.ParameterSet.Config as cms

allLayer1Photons = cms.EDProducer("PATPhotonProducer",
    # General configurables
    photonSource = cms.InputTag("allLayer0Photons"),

                                  
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
      # add "inline" functions here
      userFunctions = cms.vstring(""),
      userFunctionLabels = cms.vstring("")
    ),

    embedSuperCluster = cms.bool(True), ## whether to embed in AOD externally stored supercluster

    # Isolation configurables
    #   store isolation values
    isolation = cms.PSet(
        tracker = cms.PSet(
            # source IsoDeposit
            src = cms.InputTag("layer0PhotonIsolations","gamIsoDepositTk"),
            # parameters (E/gamma POG defaults)
            deltaR = cms.double(0.3),              # Cone radius
            vetos  = cms.vstring('0.015',          # Inner veto cone radius
                                'Threshold(1.0)'), # Pt threshold
            skipDefaultVeto = cms.bool(True),
        ),
        ecal = cms.PSet(
            # source IsoDeposit
            #src = cms.InputTag("layer0PhotonIsolations","gamIsoDepositEcalFromHits"),
            src = cms.InputTag("layer0PhotonIsolations","gamIsoDepositEcalFromClusts"),
            # parameters (E/gamma POG defaults)
            deltaR          = cms.double(0.4),
            vetos           = cms.vstring('EcalBarrel:0.045', 'EcalEndcaps:0.070'),
            skipDefaultVeto = cms.bool(True),
        ),
        ## other option, using gamIsoDepositEcalSCVetoFromClust (see also recoLayer0/photonIsolation_cff.py)
        #PSet ecal = cms.PSet( 
        #   src    = cms.InputTag("layer0PhotonIsolations", "gamIsoDepositEcalSCVetoFromClusts")
        #   deltaR = cms.double(0.4)
        #   vetos  = cms.vstring()     # no veto, already done with SC
        #   skipDefaultVeto = cms.bool(True)
        #),
        hcal = cms.PSet(
            # source IsoDeposit
            #src = cms.InputTag("layer0PhotonIsolations","gamIsoDepositHcalFromHits"),
            src = cms.InputTag("layer0PhotonIsolations","gamIsoDepositHcalFromTowers"),
            # parameters (E/gamma POG defaults)
            deltaR          = cms.double(0.4),
            skipDefaultVeto = cms.bool(True),
        ),
        user = cms.VPSet(),
    ),
    #   store isodeposits to recompute isolation
    isoDeposits = cms.PSet(
        tracker = cms.InputTag("layer0PhotonIsolations","gamIsoDepositTk"),
        #ecal    = cms.InputTag("layer0PhotonIsolations","gamIsoDepositEcalFromHits"),
        ecal    = cms.InputTag("layer0PhotonIsolations","gamIsoDepositEcalFromClusts"),
        #hcal    = cms.InputTag("layer0PhotonIsolations","gamIsoDepositHcalFromHits"),
        hcal    = cms.InputTag("layer0PhotonIsolations","gamIsoDepositHcalFromTowers"),
    ),

    # PhotonID configurables
    addPhotonID = cms.bool(True),
    photonIDSource = cms.InputTag("layer0PhotonID"), ## ValueMap<reco::PhotonID> keyed to photonSource

    # Trigger matching configurables
    addTrigMatch = cms.bool(True),
    trigPrimMatch = cms.VInputTag(cms.InputTag("photonTrigMatchHLT1PhotonRelaxed")),

    # MC matching configurables
    addGenMatch = cms.bool(True),
    embedGenMatch = cms.bool(False),
    genParticleMatch = cms.InputTag("photonMatch"), ## particles source to be used for the matching

    # Efficiencies
    addEfficiencies = cms.bool(False),
    efficiencies    = cms.PSet(),

)


