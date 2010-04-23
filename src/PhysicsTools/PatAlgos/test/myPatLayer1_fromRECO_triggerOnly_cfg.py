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
      # Collisions 7TeV, run 132658, reco. CMSSW_3_5_4 :-(
        '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/FEC1BA30-9141-DF11-94DE-001617DBD224.root'
      , '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/EA06A2EE-7E41-DF11-B781-003048D2C0F0.root'
      , '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/D4C1C7C1-8141-DF11-B0C6-0030487A3C9A.root'
      , '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/D444AE75-8241-DF11-9FD9-000423D8FA38.root'
      , '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/C203B0BA-8441-DF11-8F6D-000423D99EEE.root'
      , '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/B65D26A6-7F41-DF11-AB89-003048D37514.root'
      , '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/8AA5ECEC-8341-DF11-94D5-000423D9A2AE.root'
      , '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/86038173-8241-DF11-ADB6-000423D98EC8.root'
      , '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/74C8AC6C-8241-DF11-9801-000423D94A04.root'
      , '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/700EBA3A-7E41-DF11-8ED2-001D09F2532F.root'
      , '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/669FAD5E-8041-DF11-8954-0030487C7828.root'
      , '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/62F777A0-7F41-DF11-9E4C-000423D98804.root'
      , '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/622AAD6C-8241-DF11-9B7A-000423D99658.root'
      , '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/4E78BFEC-7E41-DF11-B565-001D09F34488.root'
      , '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/4A8C09ED-7E41-DF11-B57E-001D09F24EAC.root'
      , '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/1CFC9075-8241-DF11-BA03-001D09F24664.root'
      , '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/103A4D9E-7F41-DF11-810B-003048D2C1C4.root'
      , '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/084B7D45-8141-DF11-9599-0030487C6088.root'
      , '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/04AA91C8-8441-DF11-A98F-0030487CD7B4.root'
      , '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/048FDE75-8241-DF11-BD23-000423D99AA2.root'
      , '/store/data/Commissioning10/MinimumBias/RECO/v8/000/132/658/0474545F-8E41-DF11-82AC-0030487CAF0E.root'
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
process.GlobalTag.globaltag = 'GR10_P_V2::All'
process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )

## PAT trigger
process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff" )
# process.patTrigger.processName      = 'HLT'
# process.patTrigger.hltPrescaleLabel = cms.string( '0' )
# process.patTrigger.hltPrescaleTable = cms.string( "hltPrescaleRecorder" )
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
  , fileName       = cms.untracked.string( '/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_5_7/output/myPatLayer1_fromRECO_triggerOnly.root' )
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
