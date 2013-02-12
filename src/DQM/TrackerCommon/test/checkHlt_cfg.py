import FWCore.ParameterSet.Config as cms

process = cms.Process( "TEST" )

## MessageLogger
process.load( "FWCore.MessageLogger.MessageLogger_cfi" )

## Options and Output Report
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool( False )
                                    )

## Source
process.source = cms.Source( "PoolSource"
                           #, fileNames = cms.untracked.vstring( '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/FA54A056-42E2-DE11-A6DB-001617E30D40.root'
                                                              #)
                           , fileNames = cms.untracked.vstring( '/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/FEE389F2-33E2-DE11-A62E-001617C3B76E.root'
                                                              )
                           )
## Maximal Number of Events
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

# HLT analyzers
process.load( "HLTrigger.HLTcore.hltEventAnalyzerAOD_cfi" )
process.hltEventAnalyzerAOD.triggerName = cms.string( '@' )
process.load( "HLTrigger.HLTcore.triggerSummaryAnalyzerAOD_cfi" )

process.p = cms.Path( process.hltEventAnalyzerAOD
                    + process.triggerSummaryAnalyzerAOD
                    )
