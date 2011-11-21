import os
import FWCore.ParameterSet.Config as cms


### Initialization

process = cms.Process( "TEST" )


### Input

process.source = cms.Source(
  "EmptySource"
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 1 )
)


### Output

outputFile = '%s/output/topHitFitResolutionFunctionsAnalyzer.root'%( os.getenv( "CMSSW_BASE" ) )
process.TFileService = cms.Service(
  "TFileService"
, fileName = cms.string( outputFile )
)


### Analyzer

process.load( "TopQuarkAnalysis.TopMassSemiLeptonic.topHitFitResolutionFunctionsAnalyzer_cfi" )


### Paths

process.p = cms.Path(
  process.topHitFitResolutionFunctionsAnalyzer
)
