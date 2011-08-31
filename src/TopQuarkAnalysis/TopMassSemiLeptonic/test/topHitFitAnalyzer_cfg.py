import os

import FWCore.ParameterSet.Config as cms

process = cms.Process( "TEST" )

process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool( True )
)

process.source = cms.Source(
  "PoolSource"
, fileNames = cms.untracked.vstring(
    'file:%s/output/gentPatSkimPF2PAT.root'%( os.getenv( "CMSSW_BASE" )
  )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( -1 )
)

process.TFileService = cms.Service(
  "TFileService"
, fileName = cms.string( '%s/output/topHitFitAnalyzer.root'%( os.getenv( "CMSSW_BASE" ) )
)

process.load( "TopQuarkAnalysis.TopMassSemiLeptonic.topHitFitAnalyzer_cfi" )

process.p = cms.Path(
  process.topHitFitAnalyzer
)