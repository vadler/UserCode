import os
import FWCore.ParameterSet.Config as cms

### Steering

sample = 'RelValTTbar'
#sample = 'RelValZMM'
#sample = 'RelValWM'
#sample = 'Mu'
#sample = 'Jet'

process = cms.Process( "TEST" )

process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool( False )
)

process.source = cms.Source(
  "PoolSource"
, fileNames = cms.untracked.vstring(
    'rfio:%s/cms/Top/data/hitFitPatSkimPF2PAT_%s.root'%( os.getenv( "CASTOR_HOME" ), sample )
    #'file:hitFitPatSkimPF2PAT.root'
  )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( -1 )
)

process.TFileService = cms.Service(
  "TFileService"
, fileName = cms.string( '%s/output/topHitFitAnalyzer_%s.root'%( os.getenv( "CMSSW_BASE" ), sample ) )
#, fileName = cms.string( 'topHitFitAnalyzer.root' )
)
print 'Log file:'
print '%s/output/topHitFitAnalyzer_%s.log'%( os.getenv( "CMSSW_BASE" ), sample )

process.load( "TopQuarkAnalysis.TopMassSemiLeptonic.topHitFitAnalyzer_cfi" )

process.p = cms.Path(
  process.topHitFitAnalyzer
)