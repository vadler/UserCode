import FWCore.ParameterSet.Config as cms

topHitFitResolutionFunctions = cms.EDAnalyzer(
  "TopHitFitResolutionFunctions"

  ### Misc ###

  # Verbosity #
, verbosity = cms.uint32( 0 )

  ### Input ###

  # TQAF semi-leptonic event
, ttSemiLeptonicEvent = cms.InputTag( 'ttSemiLepEvent' )

  # HitFit resolution file paths
, lepResolutions     = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafMuonResolution.txt' )
#, lwpResolutions     = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafElectronResolution.txt' )
, udscJetResolutions = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafUdscJetResolution.txt' )
, bJetResolutions    = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafBJetResolution.txt' )

  ### Histograms ###

  # Lepton p_t
, binsLepPt = cms.uint32( 50 )
, minLepPt  = cms.double(   0. )
, maxLepPt  = cms.double( 250. )
  # Lepton  inverse p_t
, binsLepPtInv = cms.uint32( 50 )
, minLepPtInv  = cms.double( 0. )
, maxLepPtInv  = cms.double( 1. )
  # Lepton eta
, binsLepEta = cms.uint32( 50 )
, maxLepEta  = cms.double(  2.5 )
  # Lepton phi
, binsLepPhi = cms.uint32( 50 )
  # Lepton p_z
, binsLepPz = cms.uint32( 50 )
, minLepPz  = cms.double(   0. )
, maxLepPz  = cms.double( 500. )
  # Lepton mass
, binsLepM = cms.uint32( 25 )
, minLepM  = cms.double( -0.00005 )
, maxLepM  = cms.double(  0.00005 )
  # Neutrino p_t
, binsNuPt = cms.uint32( 50 )
, minNuPt  = cms.double(   0. )
, maxNuPt  = cms.double( 250. )
  # Neutrino inverse p_t
, binsNuPtInv = cms.uint32( 50 )
, minNuPtInv  = cms.double( 0. )
, maxNuPtInv  = cms.double( 1. )
  # Neutrino eta
, binsNuEta = cms.uint32( 50 )
, maxNuEta  = cms.double( 4. )
  # Neutrino phi
, binsNuPhi = cms.uint32( 50 )
  # Neutrino p_z
, binsNuPz = cms.uint32( 50 )
, minNuPz  = cms.double(   0. )
, maxNuPz  = cms.double( 750. )
  # Neutrino mass
, binsNuM = cms.uint32( 30 )
, minNuM  = cms.double( -0.000015 )
, maxNuM  = cms.double(  0.000015 )
  # Light jet p_t
, binsUdscJetPt = cms.uint32( 50 )
, minUdscJetPt  = cms.double(   0. )
, maxUdscJetPt  = cms.double( 250. )
  # Llght jet inverse p_t
, binsUdscJetPtInv = cms.uint32( 50 )
, minUdscJetPtInv  = cms.double( 0. )
, maxUdscJetPtInv  = cms.double( 1. )
  # Light jet eta
, binsUdscJetEta = cms.uint32( 50 )
, maxUdscJetEta  = cms.double( 4. )
  # Light jet phi
, binsUdscJetPhi = cms.uint32( 50 )
  # Light jet p_z
, binsUdscJetPz = cms.uint32( 50 )
, minUdscJetPz  = cms.double(   0. )
, maxUdscJetPz  = cms.double( 750. )
  # Light jet mass
, binsUdscJetM = cms.uint32( 20 )
, minUdscJetM  = cms.double( -10. )
, maxUdscJetM  = cms.double(  10. )
  # B-jet p_t
, binsBJetPt = cms.uint32( 50 )
, minBJetPt  = cms.double(   0. )
, maxBJetPt  = cms.double( 250. )
  # B-jet inverse p_t
, binsBJetPtInv = cms.uint32( 50 )
, minBJetPtInv  = cms.double( 0. )
, maxBJetPtInv  = cms.double( 1. )
  # B-jet eta
, binsBJetEta = cms.uint32( 50 )
, maxBJetEta  = cms.double( 4. )
  # B-jet phi
, binsBJetPhi = cms.uint32( 50 )
  # B-jet p_z
, binsBJetPz = cms.uint32( 50 )
, minBJetPz  = cms.double(   0. )
, maxBJetPz  = cms.double( 750. )
  # B-jet mass
, binsBJetM = cms.uint32( 20 )
, minBJetM  = cms.double( -10. )
, maxBJetM  = cms.double(  10. )

  # Lepton p_t deviation
, binsDiffLepPt   = cms.uint32( 50 )
, maxDiffLepPt    = cms.double( 20. )
  # Lepton inverse p_t deviation
, binsDiffLepPtInv = cms.uint32( 50 )
, maxDiffLepPtInv  = cms.double( 0.008 )
  # Lepton eta deviation
, binsDiffLepEta = cms.uint32( 50 )
, maxDiffLepEta  = cms.double( 0.05 )
  # Lepton phi deviation
, binsDiffLepPhi = cms.uint32( 50 )
, maxDiffLepPhi = cms.double( 0.05 )
  # Lepton p_z deviation
, binsDiffLepPz = cms.uint32( 50 )
, maxDiffLepPz  = cms.double( 50. )
  # Lepton mass deviation
, binsDiffLepM = cms.uint32( 50 )
, maxDiffLepM  = cms.double( 1. )
  # Neutrino p_t  deviation
, binsDiffNuPt = cms.uint32( 50 )
, maxDiffNuPt  = cms.double( 25. )
  # Neutrino inverse p_t deviation
, binsDiffNuPtInv = cms.uint32( 50 )
, maxDiffNuPtInv  = cms.double( 0.008 )
  # Neutrino eta deviation
, binsDiffNuEta = cms.uint32( 50 )
, maxDiffNuEta  = cms.double( 1. )
  # Neutrino phi deviation
, binsDiffNuPhi = cms.uint32( 50 )
, maxDiffNuPhi  = cms.double( 1. )
  # Neutrino p_z deviation
, binsDiffNuPz = cms.uint32( 50 )
, maxDiffNuPz  = cms.double( 500. )
  # Neutrino mass deviation
, binsDiffNuM = cms.uint32( 25 )
, maxDiffNuM  = cms.double( 0.000025 )
  # Light jet p_t deviation
, binsDiffUdscJetPt = cms.uint32( 50 )
, maxDiffUdscJetPt  = cms.double( 25. )
  # Light jet inverse p_t deviation
, binsDiffUdscJetPtInv = cms.uint32( 50 )
, maxDiffUdscJetPtInv  = cms.double( 0.008 )
  # Light jet eta deviation
, binsDiffUdscJetEta = cms.uint32( 50 )
, maxDiffUdscJetEta  = cms.double( 1. )
  # Light jet phi deviation
, binsDiffUdscJetPhi = cms.uint32( 50 )
, maxDiffUdscJetPhi  = cms.double( 1. )
  # Light jet p_z deviation
, binsDiffUdscJetPz = cms.uint32( 50 )
, maxDiffUdscJetPz  = cms.double( 500. )
  # Light jet mass deviation
, binsDiffUdscJetM = cms.uint32( 20 )
, maxDiffUdscJetM  = cms.double( 20. )
  # B-jet p_t  deviation
, binsDiffBJetPt = cms.uint32( 50 )
, maxDiffBJetPt  = cms.double( 25. )
  # B-jet inverse p_t deviation
, binsDiffBJetPtInv = cms.uint32( 50 )
, maxDiffBJetPtInv  = cms.double( 0.008 )
  # B-jet eta deviation
, binsDiffBJetEta = cms.uint32( 50 )
, maxDiffBJetEta  = cms.double( 1. )
  # B-jet phi deviation
, binsDiffBJetPhi = cms.uint32( 50 )
, maxDiffBJetPhi  = cms.double( 1. )
  # B-jet p_z deviation
, binsDiffBJetPz = cms.uint32( 50 )
, maxDiffBJetPz  = cms.double( 500. )
  # B-jet mass deviation
, binsDiffBJetM = cms.uint32( 20 )
, maxDiffBJetM  = cms.double( 20. )

)
