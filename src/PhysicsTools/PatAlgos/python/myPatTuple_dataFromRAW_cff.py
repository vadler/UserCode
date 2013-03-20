import FWCore.ParameterSet.Config as cms

fileNames = cms.PSet(
  fileNames = cms.untracked.vstring(
    '/store/data/Run2012D/SingleMu/RAW/v1/000/204/552/862877D1-470F-E211-944A-003048F024FE.root '
  )
, lumisToProcess = cms.untracked.VLuminosityBlockRange(
    '204552:1-204552:15'
  )
)
