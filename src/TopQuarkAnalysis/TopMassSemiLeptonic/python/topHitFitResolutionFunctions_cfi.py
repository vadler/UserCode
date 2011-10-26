import FWCore.ParameterSet.Config as cms

topHitFitResolutionFunctions = cms.EDAnalyzer(
  "TopHitFitResolutionFunctions"

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
