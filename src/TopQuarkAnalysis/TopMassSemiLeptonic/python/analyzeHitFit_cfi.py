import FWCore.ParameterSet.Config as cms

# Steering

analyzeHitFit = cms.EDAnalyzer(
  "AnalyzeHitFit"

  ### Input ###

  # Pile-up distributions
, pileUpFileMCTrue       = cms.FileInPath( 'CommonTools/MyTools/data/pileUpFileMC_Fall11.root' )
, pileUpFileMCObserved   = cms.FileInPath( 'CommonTools/MyTools/data/pileUpFileMC_Fall11inTime.root' )
, pileUpFileDataTrue     = cms.FileInPath( 'CommonTools/MyTools/data/pileUpFileData_2011truePixel.root' )
, pileUpFileDataObserved = cms.FileInPath( 'CommonTools/MyTools/data/pileUpFileData_2011observedPixel.root' )

  # TQAF semi-leptonic event
, ttSemiLeptonicEventMuons     = cms.InputTag( 'ttSemiLepEventHitFitMuons' )
, ttSemiLeptonicEventElectrons = cms.InputTag( 'ttSemiLepEventHitFitElectrons' )

  # TQAF MC event
, ttGenEvent = cms.InputTag( 'genEvt' )

  # PAT objects
, patMuons     = cms.InputTag( 'selectedPatMuons' )
, patElectrons = cms.InputTag( 'selectedPatElectrons' )
, patJets      = cms.InputTag( 'selectedPatJets' )
, patMETs      = cms.InputTag( 'patMETs' )              # 'patMETs' (TypeI) or 'patMETsUncorr' (none)

  # JEC level to be considered
, jecLevel = cms.string( 'L3Absolute' )

  # HitFit resolution file paths (only used, if eta binning empty per objects category)
, muonResolutions     = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafMuonResolution.txt' )
, electronResolutions = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafElectronResolution.txt' )
, udscJetResolutions  = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafUdscJetResolution.txt' )
, bJetResolutions     = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafBJetResolution.txt' )
, metResolutions      = cms.string( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafKtResolution.txt' )

  ### Output ###

, pathPlots = cms.string( '' ) # empty string prevents from plotting

  # Eta binning (overrides input from resolution files, if not empty)
, muonEtaBins     = cms.vdouble()
, electronEtaBins = cms.vdouble()
, udscJetEtaBins  = cms.vdouble()
, bJetEtaBins     = cms.vdouble()
, metEtaBins      = cms.vdouble()

  # P_t binning (overrides input from resolution files, if not empty)
, muonPtBins     = cms.vdouble( 20., 27., 33., 40., 47., 54., 62.,  73.,  86., 109., 200., 400. )
, electronPtBins = cms.vdouble( 20., 27., 33., 40., 47., 54., 62.,  73.,  86., 109., 200., 400. )
, udscJetPtBins  = cms.vdouble( 30., 40., 50., 60., 70., 80., 90., 100., 120., 150., 200., 300., 500. )
, bJetPtBins     = cms.vdouble( 30., 40., 50., 60., 70., 80., 90., 100., 120., 150., 200., 300., 500. )
, metPtBins      = cms.vdouble( 10., 20., 29., 37., 44., 51., 59.,  69.,  80.,  96., 122., 200., 300. )

)
