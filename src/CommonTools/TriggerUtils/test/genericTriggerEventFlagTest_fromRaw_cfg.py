import FWCore.ParameterSet.Config as cms

process = cms.Process( "TEST" )

## Logging
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.categories.append( 'GenericTriggerEventFlag' )
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.cerr.GenericTriggerEventFlag = cms.untracked.PSet( limit = cms.untracked.int32( -1 ) )
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool( True ) )

# Conditions
from Configuration.AlCa.autoCond import autoCond
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = 'GR_R_50_V6::All'

## Source
process.source = cms.Source( "PoolSource"
, fileNames  = cms.untracked.vstring( '/store/data/Run2011A/SingleMu/RAW/v1/000/165/121/060FB2F7-C07F-E011-B8AD-001D09F23944.root'
                                    , '/store/data/Run2011B/SingleMu/RAW/v1/000/177/719/02BA0913-DFEC-E011-A8F1-E0CB4E55367F.root'
                                    )
, skipEvents = cms.untracked.uint32( 21200 )
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( 100 ) )

## Reco
process.load( "Configuration.StandardSequences.RawToDigi_Data_cff" )
process.load( "L1Trigger.Configuration.L1Reco_cff" )
process.s = cms.Sequence(
  process.gctDigis
* process.gtDigis
* process.gtEvmDigis
* process.scalersRawToDigi
* process.L1Reco
)

# Test modules
# L1
process.genericTriggerEventFlagL1Pass = cms.EDFilter( "GenericTriggerEventFlagTest"
, andOr          = cms.bool( False )
, verbosityLevel = cms.uint32( 2 )
, andOrL1      = cms.bool( False )
, l1Algorithms = cms.vstring( 'L1_SingleMu12 OR L1_SingleMu14_Eta2p1'
                            )
, errorReplyL1 = cms.bool( False )
)
process.genericTriggerEventFlagL1Fail     = process.genericTriggerEventFlagL1Pass.clone( l1Algorithms = [ 'L1_SingleMu12_' ] )
process.genericTriggerEventFlagL1Test     = process.genericTriggerEventFlagL1Pass.clone( l1Algorithms = [ 'L1_SingleMu12* OR L1_SingleMu14*' ] )
process.genericTriggerEventFlagL1TestFail = process.genericTriggerEventFlagL1Pass.clone( l1Algorithms = [ 'L1_SingleMu12_v*' ] )
# HLT
process.genericTriggerEventFlagHLTPass = cms.EDFilter( "GenericTriggerEventFlagTest"
, andOr          = cms.bool( False )
, verbosityLevel = cms.uint32( 2 )
, andOrHlt      = cms.bool( False )
, hltInputTag   = cms.InputTag( 'TriggerResults::HLT' )
, hltPaths      = cms.vstring( 'HLT_IsoMu24_eta2p1_v3' # only in 2011B
                             )
, errorReplyHlt = cms.bool( False )
)
process.genericTriggerEventFlagHLTFail      = process.genericTriggerEventFlagHLTPass.clone( hltPaths = [ 'HLT_IsoMu24_eta2p1' ] )
process.genericTriggerEventFlagHLTTestTight = process.genericTriggerEventFlagHLTPass.clone( hltPaths = [ 'HLT_IsoMu24_eta2p1_v*' ] )
process.genericTriggerEventFlagHLTTestLoose = process.genericTriggerEventFlagHLTPass.clone( hltPaths = [ 'HLT_IsoMu24_*' ] )
process.genericTriggerEventFlagHLTTestFail  = process.genericTriggerEventFlagHLTPass.clone( hltPaths = [ 'HLT_IsoMu2*_v*' ] )        # does not fail, in fact :-)

# Paths
# L1
process.pL1Pass = cms.Path(
  process.s
* process.genericTriggerEventFlagL1Pass
)
process.pL1Fail = cms.Path(
  process.s
* process.genericTriggerEventFlagL1Fail
)
process.pL1Test = cms.Path(
  process.s
* process.genericTriggerEventFlagL1Test
)
process.pL1TestFail = cms.Path(
  process.s
* process.genericTriggerEventFlagL1TestFail
)
# HLT
process.pHLTPass = cms.Path(
  process.s
* process.genericTriggerEventFlagHLTPass
)
process.pHLTFail = cms.Path(
  process.s
* process.genericTriggerEventFlagHLTFail
)
process.pHLTTestTight = cms.Path(
  process.s
* process.genericTriggerEventFlagHLTTestTight
)
process.pHLTTestLoose = cms.Path(
  process.s
* process.genericTriggerEventFlagHLTTestLoose
)
process.pHLTTestFail = cms.Path(
  process.s
* process.genericTriggerEventFlagHLTTestFail
)
