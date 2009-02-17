import FWCore.ParameterSet.Config as cms

# Examples for configurations of the trigger match for various physics objects
# (cuts are NOT tuned, using old values from TQAF MC match, january 2008)

# Filter IDs are found as enum trigger::TriggerObjectType
# in http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/CMSSW/DataFormats/HLTReco/interface/TriggerTypeDefs.h?sortby=file&view=log


# matches to Egamma triggers
# matches to CandHLT1ElectronStartup
electronTriggerMatchCandHLT1ElectronStartup = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Electrons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT1Photon
photonTriggerMatchHLT1Photon = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Photons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 1.0 ),
    maxDeltaR = cms.double( 0.2 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT1PhotonRelaxed
photonTriggerMatchHLT1PhotonRelaxed = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Photons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 1.0 ),
    maxDeltaR = cms.double( 0.2 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT2Photon
photonTriggerMatchHLT2Photon = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Photons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 1.0 ),
    maxDeltaR = cms.double( 0.2 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT2PhotonRelaxed
photonTriggerMatchHLT2PhotonRelaxed = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Photons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 1.0 ),
    maxDeltaR = cms.double( 0.2 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT1Electron
electronTriggerMatchHLT1Electron = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Electrons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT1ElectronRelaxed
# including example of "wrong" match (jets which fired electron trigger),
electronTriggerMatchHLT1ElectronRelaxed = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Electrons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

jetTriggerMatchHLT1ElectronRelaxed = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Jets" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT2Electron
electronTriggerMatchHLT2Electron = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Electrons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT2ElectronRelaxed
electronTriggerMatchHLT2ElectronRelaxed = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Electrons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to Muon triggers
# matches to HLT1MuonIso
muonTriggerMatchHLT1MuonIso = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Muons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT1MuonNonIso
muonTriggerMatchHLT1MuonNonIso = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Muons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT2MuonNonIso
muonTriggerMatchHLT2MuonNonIso = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Muons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to BTau triggers
# matches to HLT1Tau
tauTriggerMatchHLT1Tau = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Taus" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT2TauPixel
tauTriggerMatchHLT2TauPixel = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Taus" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to JetMET triggers
# matches to HLT2jet
jetTriggerMatchHLT2jet = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Jets" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 3.0 ),
    maxDeltaR = cms.double( 0.4 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT3jet
jetTriggerMatchHLT3jet = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Jets" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 3.0 ),
    maxDeltaR = cms.double( 0.24 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT4jet
jetTriggerMatchHLT4jet = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Jets" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 3.0 ),
    maxDeltaR = cms.double( 0.4 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT1MET65
# including example of "wrong" match (cleanLayer1Muons which fired MET trigger),
metTriggerMatchHLT1MET65 = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "layer1METs" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 99.0 ),
    maxDeltaR = cms.double( 99.0 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

muonTriggerMatchHLT1MET65 = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Muons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)


# matches to ALL
electronTriggerMatchHltElectrons = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Electrons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 92 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

muonTriggerMatchL1Muons = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Muons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 81 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

tauTriggerMatchTriggerTaus = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Taus" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 86, 94 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)


## patTuple ##

# matches to HLT_IsoMu11
muonTriggerMatchHLTIsoMu11 = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Muons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT_Mu11
muonTriggerMatchHLTMu11 = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Muons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT_DoubleIsoMu3
muonTriggerMatchHLTDoubleIsoMu3 = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Muons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT_DoubleMu3
muonTriggerMatchHLTDoubleMu3 = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Muons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT_IsoEle15_LW_L1I
electronTriggerMatchHLTIsoEle15LWL1I = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Electrons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT_Ele15_LW_L1R
electronTriggerMatchHLTEle15LWL1R = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Electrons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT_DoubleIsoEle10_LW_L1I
electronTriggerMatchHLTDoubleIsoEle10LWL1I = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Electrons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT_DoubleEle5_SW_L1R
electronTriggerMatchHLTDoubleEle5SWL1R = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Electrons" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT_LooseIsoTau_MET30_L1MET
tauTriggerMatchHLTLooseIsoTauMET30L1MET = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Taus" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)

# matches to HLT_DoubleIsoTau_Trk3
tauTriggerMatchHLTDoubleIsoTauTrk3 = cms.EDFilter( "PATTriggerMatcherDRDPtLessByR",
    src     = cms.InputTag( "cleanLayer1Taus" ),
    matched = cms.InputTag( "patTrigger" ),
    filterIds = cms.vuint32( 0 ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( False )
)


# sequences
patTriggerMatcher = cms.Sequence(
   electronTriggerMatchHltElectrons +
   muonTriggerMatchL1Muons          +
   tauTriggerMatchTriggerTaus
)
