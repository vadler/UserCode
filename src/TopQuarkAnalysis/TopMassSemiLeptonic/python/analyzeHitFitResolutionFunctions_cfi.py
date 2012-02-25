import FWCore.ParameterSet.Config as cms

# Steering

analyzeHitFitResolutionFunctions = cms.EDAnalyzer(
  "AnalyzeHitFitResolutionFunctions"

  ### Input ###

  # TQAF semi-leptonic event
, ttSemiLeptonicEvent = cms.InputTag( 'ttSemiLepEvent' )

  # Switch for using alternative kinematics
, useAlt = cms.bool( False )

  # Switch for eta symmetric distribution
, useSymm = cms.bool( False ) # add histograms adding up symmetric bins in eta

  # Switches for referring to generator truth
, refGen = cms.bool( False ) # add histograms with generated kinematics as reference

  # Leptons to use
, leptonType = cms.string( 'Muons' ) # 'Muons' or 'Electrons'

  # JEC level to be considered
, patLeptons = cms.InputTag( '' ) # dummy, needs assignemnt in main config
, patJets    = cms.InputTag( 'selectedPatJets' )
, jecLevel   = cms.string( 'L3Absolute' )

  # Eta binning (overrides input from resolution files, if not empty)
, muonEtaBins     = cms.vdouble()
, electronEtaBins = cms.vdouble()
, udscJetEtaBins  = cms.vdouble()
, bJetEtaBins     = cms.vdouble()
, metEtaBins      = cms.vdouble()

  # HitFit resolution file paths (only used, if eta binning empty per objects category)
, muonResolutions     = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafMuonResolution.txt' )
, electronResolutions = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafElectronResolution.txt' )
, udscJetResolutions  = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafUdscJetResolution.txt' )
, bJetResolutions     = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafBJetResolution.txt' )
, metResolutions      = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafKtResolution.txt' )

  # P_t binning (overrides input from resolution files, if not empty)
, muonPtBins     = cms.vdouble( 20., 27., 33., 40., 47., 54., 62.,  73.,  86., 109., 200., 400. )
, electronPtBins = cms.vdouble( 20., 27., 33., 40., 47., 54., 62.,  73.,  86., 109., 200., 400. )
, udscJetPtBins  = cms.vdouble( 30., 40., 50., 60., 70., 80., 90., 100., 120., 150., 200., 300., 500. )
, bJetPtBins     = cms.vdouble( 30., 40., 50., 60., 70., 80., 90., 100., 120., 150., 200., 300., 500. )
, metPtBins      = cms.vdouble( 10., 20., 29., 37., 44., 51., 59.,  69.,  80.,  96., 122., 200., 300. )

  # Quantities' Delta binning
  # Muons
, muonDeltaPtBins = cms.uint32( 50 )
, muonDeltaPtMax  = cms.double( 12.5 )
, muonDeltaPtInvBins = cms.uint32( 50 )
, muonDeltaPtInvMax  = cms.double( 0.0025 )
, muonDeltaEtaBins = cms.uint32( 50 )
, muonDeltaEtaMax  = cms.double( 0.02 )
, muonDeltaEtaInvBins = cms.uint32( 50 )
, muonDeltaEtaInvMax  = cms.double( 0.02 )
, muonDeltaPhiBins = cms.uint32( 50 )
, muonDeltaPhiMax  = cms.double( 0.002 )
, muonDeltaPhiInvBins = cms.uint32( 50 )
, muonDeltaPhiInvMax  = cms.double( 0.002 )
  # Electrons
, electronDeltaPtBins = cms.uint32( 50 )
, electronDeltaPtMax  = cms.double( 12.5 )
, electronDeltaPtInvBins = cms.uint32( 50 )
, electronDeltaPtInvMax  = cms.double( 0.0025 )
, electronDeltaEtaBins = cms.uint32( 50 )
, electronDeltaEtaMax  = cms.double( 0.02 )
, electronDeltaEtaInvBins = cms.uint32( 50 )
, electronDeltaEtaInvMax  = cms.double( 0.02 )
, electronDeltaPhiBins = cms.uint32( 50 )
, electronDeltaPhiMax  = cms.double( 0.002 )
, electronDeltaPhiInvBins = cms.uint32( 50 )
, electronDeltaPhiInvMax  = cms.double( 0.002 )
  # Light jets
, udscJetDeltaPtBins = cms.uint32( 50 )
, udscJetDeltaPtMax  = cms.double( 50. )
, udscJetDeltaPtInvBins = cms.uint32( 50 )
, udscJetDeltaPtInvMax  = cms.double( 0.02 )
, udscJetDeltaEtaBins = cms.uint32( 50 )
, udscJetDeltaEtaMax  = cms.double( 0.5 )
, udscJetDeltaEtaInvBins = cms.uint32( 50 )
, udscJetDeltaEtaInvMax  = cms.double( 0.5 )
, udscJetDeltaPhiBins = cms.uint32( 50 )
, udscJetDeltaPhiMax  = cms.double( 0.5 )
, udscJetDeltaPhiInvBins = cms.uint32( 50 )
, udscJetDeltaPhiInvMax  = cms.double( 0.5 )
  # B-jets
, bJetDeltaPtBins = cms.uint32( 50 )
, bJetDeltaPtMax  = cms.double( 50. )
, bJetDeltaPtInvBins = cms.uint32( 50 )
, bJetDeltaPtInvMax  = cms.double( 0.02 )
, bJetDeltaEtaBins = cms.uint32( 50 )
, bJetDeltaEtaMax  = cms.double( 0.5 )
, bJetDeltaEtaInvBins = cms.uint32( 50 )
, bJetDeltaEtaInvMax  = cms.double( 0.5 )
, bJetDeltaPhiBins = cms.uint32( 50 )
, bJetDeltaPhiMax  = cms.double( 0.5 )
, bJetDeltaPhiInvBins = cms.uint32( 50 )
, bJetDeltaPhiInvMax  = cms.double( 0.5 )
  # MET
, metDeltaPtBins = cms.uint32( 50 )
, metDeltaPtMax  = cms.double( 100. )
, metDeltaPtInvBins = cms.uint32( 50 )
, metDeltaPtInvMax  = cms.double( 0.025 )
, metDeltaEtaBins = cms.uint32( 50 )
, metDeltaEtaMax  = cms.double( 10. )
, metDeltaEtaInvBins = cms.uint32( 50 )
, metDeltaEtaInvMax  = cms.double( 10. )
, metDeltaPhiBins = cms.uint32( 50 )
, metDeltaPhiMax  = cms.double( 1.6 )
, metDeltaPhiInvBins = cms.uint32( 50 )
, metDeltaPhiInvMax  = cms.double( 1.6 )
)
