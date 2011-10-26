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

  # Lepton p_t
, binsLepPt = cms.uint32( 50 )
, maxLepPt  = cms.double( 250. )
  # Jet p_t
, binsJetPt = cms.uint32( 50 )
, maxJetPt  = cms.double( 500. )
  # Neutrino p_t
, binsNuPt = cms.uint32( 50 )   # binsLepPt
, maxNuPt  = cms.double( 250. ) # maxLepPt

)
