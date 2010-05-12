import FWCore.ParameterSet.Config as cms

process = cms.Process( "PAT" )

## MessageLogger
process.load( "FWCore.MessageLogger.MessageLogger_cfi" )

## Options and Output Report
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool( False )
  )

## Source
process.source = cms.Source(
    "PoolSource"
  , fileNames = cms.untracked.vstring(
        'file:/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_6_1/output/myHlt1E31_fromRAW_triggerOnly.root'
      )
  )
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( 1 )
  )
process.physDecl = cms.EDFilter(
    "PhysDecl"
  , applyfilter = cms.untracked.bool( True )
  )
process.load( "L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff" )
process.load( "HLTrigger.HLTfilters.hltLevel1GTSeed_cfi" )
process.hltLevel1GTSeed.L1TechTriggerSeeding     = cms.bool( True )
process.hltLevel1GTSeed.L1SeedsLogicalExpression = cms.string( '0 AND (40 OR 41) AND NOT (36 OR 37 OR 38 OR 39)' )
process.scrapingVeto = cms.EDFilter(
    "FilterOutScraping"
  , applyfilter = cms.untracked.bool( True )
  , debugOn     = cms.untracked.bool( False )
  , numtrack    = cms.untracked.uint32( 10 )
  , thresh      = cms.untracked.double( 0.2 )
  )
process.primaryVertexFilter = cms.EDFilter(
    "GoodVertexFilter"
  , vertexCollection = cms.InputTag( 'offlinePrimaryVertices' )
  , minimumNDOF      = cms.uint32( 4 )
  , maxAbsZ          = cms.double( 15 )
  , maxd0            = cms.double( 2 )
  )

## Geometry and Detector Conditions (needed for a few patTuple production steps)
process.load( "Configuration.StandardSequences.Services_cff" )
process.load( "Configuration.StandardSequences.Geometry_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.globaltag = 'MC_36Y_V4::All'
process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )

## PAT trigger
process.load("PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff")
process.patTrigger.processName      = 'HLT1E31'
# process.patTrigger.hltPrescaleLabel = cms.string( '1' )
process.patTrigger.hltPrescaleTable = cms.string( "hltPrescaleRecorder1" )
process.patTriggerEvent.patTriggerMatches = []
process.p = cms.Path(
#     process.hltLevel1GTSeed
#   * process.scrapingVeto
#   * process.physDecl
#   * process.primaryVertexFilter
#   * process.patTrigger
    process.patTrigger
  * process.patTriggerEvent
  )

## Output
from PhysicsTools.PatAlgos.patEventContent_cff import patTriggerEventContent
process.out = cms.OutputModule(
    "PoolOutputModule"
  , fileName       = cms.untracked.string( '/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_6_1/output/myPatTuple_triggerOnly.root' )
  , SelectEvents   = cms.untracked.PSet(
        SelectEvents = cms.vstring(
            'p'
          )
      )
  , outputCommands = cms.untracked.vstring(
        'drop *'
      , *patTriggerEventContent
      )
  )
process.outpath = cms.EndPath(
    process.out
  )
