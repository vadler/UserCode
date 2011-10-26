import FWCore.ParameterSet.Config as cms

topHitFitAnalyzer = cms.EDAnalyzer(
  "TopHitFitAnalyzer"

  ### Misc ###

  # Verbosity #
, verbosity = cms.uint32( 0 )

  ### Input ###

  # TQAF semi-leptonic event
, ttSemiLeptonicEvent = cms.InputTag( 'ttSemiLepEvent' )
, patMuons            = cms.InputTag( 'selectedPatMuons' )
, patJets             = cms.InputTag( 'selectedPatJets' )
, patMETs             = cms.InputTag( 'patMETs' )

  ### Histograms ###

  # HitFit validity
, binsHitFitNValid = cms.uint32( 24 )
  # HitFit probability
, binsHitFitProb = cms.uint32( 25 )
, binsHitFitProbLow = cms.uint32( 25 )
, maxHitFitProbLow  = cms.double( 0.04 )
, binsHitFitProbLog = cms.uint32( 25 )
, minHitFitProbLog  = cms.double( -10. )
  # HitFit chi^2
, binsHitFitChi2 = cms.uint32( 50 )
, maxHitFitChi2  = cms.double( 100. )
  # HitFit top mass
, binsHitFitMT = cms.uint32( 100 )
, minHitFitMT  = cms.double(  90. )
, maxHitFitMT  = cms.double( 340. )
  # HitFit top mass uncertainty
, binsHitFitSigMT = cms.uint32( 25 )
, maxHitFitSigMT  = cms.double( 50. )
  # Relative HitFit top mass uncertainty
, binsHitFitSigMTRel = cms.uint32( 25 )
, maxHitFitSigMTRel  = cms.double( 0.25 )

  # GenMatch validity
, binsGenMatchNValid = cms.uint32( 24 )
  # GenMatch sum p_t
, binsGenMatchSumPt = cms.uint32( 50 )
, maxGenMatchSumPt  = cms.double( 500. )
  # GenMatch sum Delta R
, binsGenMatchSumDR = cms.uint32( 20 )
, maxGenMatchSumDR  = cms.double( 10. )

  # Top mass
, binsTopM = cms.uint32( 100 )  # binsHitFitMT
, minTopM  = cms.double(  90. ) # minHitFitMT
, maxTopM  = cms.double( 340. ) # maxHitFitMT
  # Lepton mass
, binsGenLepM = cms.uint32( 25 )
, maxGenLepM  = cms.double( 1. )
, binsLepM = cms.uint32( 25 )
, maxLepM  = cms.double( 0.00005 )
  # Lepton p_t
, binsLepPt = cms.uint32( 50 )
, maxLepPt  = cms.double( 250. )
  # Lepton eta
, binsLepEta = cms.uint32( 50 )
, maxLepEta  = cms.double( 2.5 )
  # Lepton phi
, binsLepPhi = cms.uint32( 50 )
  # Lepton p_z
, binsLepPz = cms.uint32( 50 )
, maxLepPz  = cms.double( 500. )
  # Neutrino mass
, binsNuM = cms.uint32( 30 )
, maxNuM  = cms.double( 0.000015 )
  # Neutrino p_t
, binsNuPt = cms.uint32( 50 )   # binsLepPt
, maxNuPt  = cms.double( 250. ) # maxLepPt
  # Neutrino eta
, binsNuEta = cms.uint32( 50 )
, maxNuEta  = cms.double( 4. )
  # Neutrino phi
, binsNuPhi = cms.uint32( 50 ) # binsLepPhi
  # Neutrino p_z
, binsNuPz = cms.uint32( 50 )
, maxNuPz  = cms.double( 750. )

  # Top mass deviation
, binsDiffTopM = cms.uint32( 50 )
, maxDiffTopM  = cms.double( 100. )
  # Lepton mass deviation
, binsDiffLepM = cms.uint32( 50 )
, maxDiffLepM  = cms.double( 1. ) # maxGenLepM
  # Lepton p_t deviation
, binsDiffLepPt   = cms.uint32( 50 )
, maxDiffLepPt    = cms.double( 20. )
, maxDiffLepPtInv = cms.double( 0.008 )
  # Lepton eta deviation
, binsDiffLepEta = cms.uint32( 50 )
, maxDiffLepEta  = cms.double( 0.05 )
  # Lepton phi deviation
, binsDiffLepPhi = cms.uint32( 50 )
, maxDiffLepPhi = cms.double( 0.05 )
  # Lepton p_z deviation
, binsDiffLepPz = cms.uint32( 50 )
, maxDiffLepPz  = cms.double( 50. )
  # Neutrino mass deviation
, binsDiffNuM = cms.uint32( 25 )       # binsNuM
, maxDiffNuM  = cms.double( 0.000025 ) # maxNuM
  # Neutrino p_t  deviation
, binsDiffNuPt = cms.uint32( 50 )  # binsDiffLepPt
, maxDiffNuPt  = cms.double( 25. ) # maxDiffLepPt
  # Neutrino eta deviation
, binsDiffNuEta = cms.uint32( 50 )  # binsDiffLepEta
, maxDiffNuEta  = cms.double( 1. ) # maxDiffLepEta
  # Neutrino phi deviation
, binsDiffNuPhi = cms.uint32( 50 )  # binsDiffLepPhi
, maxDiffNuPhi  = cms.double( 1. ) # maxDiffLepPhi
  # Neutrino p_z  deviation
, binsDiffNuPz = cms.uint32( 50 )   # binsDiffLepPz
, maxDiffNuPz  = cms.double( 500. ) # maxDiffLepPz

)
