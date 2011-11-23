import FWCore.ParameterSet.Config as cms

topHitFitResolutionFunctions = cms.EDAnalyzer(
  "TopHitFitResolutionFunctions"

  ### Input ###

  # TQAF semi-leptonic event
, ttSemiLeptonicEvent = cms.InputTag( 'ttSemiLepEvent' )

  # JEC level to be considered
, patJets  = cms.InputTag( 'selectedPatJets' )
, jecLevel = cms.string( 'L2L3Residual' )

  # HitFit resolution file paths
, lepResolutions     = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafMuonResolution.txt' )
#, lwpResolutions     = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafElectronResolution.txt' )
, udscJetResolutions = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafUdscJetResolution.txt' )
, bJetResolutions    = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafBJetResolution.txt' )

  ### Histograms ###

  # Lepton p_t
, binsLepPt = cms.vdouble( 20., 27., 33., 40., 47., 54., 62., 73., 86., 109., 200., 400. )
#, binsLepPt = cms.uint32( 38 )
#, minLepPt  = cms.double(  20. )
#, maxLepPt  = cms.double( 400. )
  # Light jet p_t
, binsUdscJetPt = cms.vdouble( 30., 40., 50., 60., 70., 80., 90., 100., 120., 150., 200., 300., 500. )
#, binsUdscJetPt = cms.uint32( 24 )
#, minUdscJetPt  = cms.double(  30. )
#, maxUdscJetPt  = cms.double( 510. )
  # B-jet p_t
, binsBJetPt = cms.vdouble( 30., 40., 50., 60., 70., 80., 90., 100., 120., 150., 200., 300., 500. )
#, binsBJetPt = cms.uint32( 24 )
#, minBJetPt  = cms.double(  30. )
#, maxBJetPt  = cms.double( 510. )
  # Neutrino p_t
, binsNuPt = cms.vdouble( 10., 20., 29., 37., 44., 51., 59., 69., 80., 96., 122., 200., 300. )
#, binsNuPt = cms.uint32( 20 )
#, minNuPt  = cms.double(   0. )
#, maxNuPt  = cms.double( 500. )

  # Lepton p_t deviation
, binsDiffLepPt   = cms.uint32( 21 )
, maxDiffLepPt    = cms.double( 12.5 )
  # Lepton inverse p_t deviation
, binsDiffLepPtInv = cms.uint32( 21 )
, maxDiffLepPtInv  = cms.double( 0.003 )
  # Lepton eta deviation
, binsDiffLepEta = cms.uint32( 21 )
, maxDiffLepEta  = cms.double( 0.0125 )
  # Lepton phi deviation
, binsDiffLepPhi = cms.uint32( 21 )
, maxDiffLepPhi = cms.double( 0.0125 )
  # Lepton p_z deviation
, binsDiffLepPz = cms.uint32( 21 )
, maxDiffLepPz  = cms.double( 20. )
  # Lepton mass deviation
, binsDiffLepM = cms.uint32( 21 )
, maxDiffLepM  = cms.double( 0.00125 )
  # Light jet p_t deviation
, binsDiffUdscJetPt = cms.uint32( 21 )
, maxDiffUdscJetPt  = cms.double( 50. )
  # Light jet inverse p_t deviation
, binsDiffUdscJetPtInv = cms.uint32( 21 )
, maxDiffUdscJetPtInv  = cms.double( 0.0125 )
  # Light jet eta deviation
, binsDiffUdscJetEta = cms.uint32( 21 )
, maxDiffUdscJetEta  = cms.double( 1. )
  # Light jet phi deviation
, binsDiffUdscJetPhi = cms.uint32( 21 )
, maxDiffUdscJetPhi  = cms.double( 1. )
  # Light jet p_z deviation
, binsDiffUdscJetPz = cms.uint32( 21 )
, maxDiffUdscJetPz  = cms.double( 500. )
  # Light jet mass deviation
, binsDiffUdscJetM = cms.uint32( 21 )
, maxDiffUdscJetM  = cms.double( 40. )
  # B-jet p_t  deviation
, binsDiffBJetPt = cms.uint32( 21 )
, maxDiffBJetPt  = cms.double( 50. )
  # B-jet inverse p_t deviation
, binsDiffBJetPtInv = cms.uint32( 21 )
, maxDiffBJetPtInv  = cms.double( 0.0125 )
  # B-jet eta deviation
, binsDiffBJetEta = cms.uint32( 21 )
, maxDiffBJetEta  = cms.double( 1. )
  # B-jet phi deviation
, binsDiffBJetPhi = cms.uint32( 21 )
, maxDiffBJetPhi  = cms.double( 1. )
  # B-jet p_z deviation
, binsDiffBJetPz = cms.uint32( 21 )
, maxDiffBJetPz  = cms.double( 500. )
  # B-jet mass deviation
, binsDiffBJetM = cms.uint32( 21 )
, maxDiffBJetM  = cms.double( 40. )
  # Neutrino p_t  deviation
, binsDiffNuPt = cms.uint32( 21 )
, maxDiffNuPt  = cms.double( 50. )
  # Neutrino inverse p_t deviation
, binsDiffNuPtInv = cms.uint32( 21 )
, maxDiffNuPtInv  = cms.double( 0.02 )
  # Neutrino eta deviation
, binsDiffNuEta = cms.uint32( 21 )
, maxDiffNuEta  = cms.double( 3. )
  # Neutrino phi deviation
, binsDiffNuPhi = cms.uint32( 21 )
, maxDiffNuPhi  = cms.double( 2. )
  # Neutrino p_z deviation
, binsDiffNuPz = cms.uint32( 21 )
, maxDiffNuPz  = cms.double( 500. )
  # Neutrino mass deviation
, binsDiffNuM = cms.uint32( 21 )
, maxDiffNuM  = cms.double( 0.0000125 )

)
