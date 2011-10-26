import FWCore.ParameterSet.Config as cms

topHitFitResolutionFunctions = cms.EDAnalyzer(
  "TopHitFitResolutionFunctions"

  ### Input ###

  # Muon resolution file
, muonResolutions     = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafMuonResolution.txt' )
  # Electron resolution file
, electronResolutions = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafElectronResolution.txt' )
  # Light jet resolution file
, udscJetResolutions  = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafUdscJetResolution.txt' )
  # b-jet resolution file
, bJetResolutions     = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafBJetResolution.txt' )
  # MET resolution file
, metResolutions      = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafKtResolution.txt' )

  ### Functions ###

  # Lepton #
, binsLepPt = cms.uint32(  50 )
, maxLepPt  = cms.double( 250. )
  # Lepton p_t
, maxDiffLepPt       = cms.double( 10. )
, maxDiffLepPtInv    = cms.double(  0.0004 )
, maxDiffLepPtInvRel = cms.double(  0.0015 )
  # Lepton eta
, maxDiffLepEta = cms.double( 0.0025 )
  # Lepton phi
, maxDiffLepPhi = cms.double( 0.005 )

  # Jet #
, binsJetPt = cms.uint32(  50 )
, maxJetPt  = cms.double( 500. )
  # Jet p_t
, maxDiffJetPt       = cms.double( 50. )
, maxDiffJetPtInv    = cms.double(  0.004 )
, maxDiffJetPtInvRel = cms.double(  0.0025 )
  # Jet eta
, maxDiffJetEta = cms.double( 0.2 )
  # Jet phi
, maxDiffJetPhi = cms.double( 0.2 )

  # Neutrino #
, binsNuPt = cms.uint32(  50 )
, maxNuPt  = cms.double( 250. )
  # Neutrino p_t
, maxDiffNuPt       = cms.double( 50. )
, maxDiffNuPtInv    = cms.double(  0.004 )
, maxDiffNuPtInvRel = cms.double(  0.0025 )
  # Neutrino eta
, maxDiffNuEta = cms.double( 0.2 )
  # Neutrino phi
, maxDiffNuPhi = cms.double( 0.2 )

)
