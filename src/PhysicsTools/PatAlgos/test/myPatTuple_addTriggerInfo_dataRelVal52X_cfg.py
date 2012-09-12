import os
import FWCore.ParameterSet.Config as cms

# Steering
cmsswVersion = 'CMSSW_5_2_5_cand1'
globalTag    = 'GR_R_52_V7'
condition    = 'com10'

process = cms.Process("PAT")

## Options
process.options = cms.untracked.PSet(
  wantSummary      = cms.untracked.bool( False )
, allowUnscheduled = cms.untracked.bool( False )
)

## Messaging
process.load( "FWCore.MessageLogger.MessageLogger_cfi" )
#process.Tracer = cms.Service( "Tracer" )

## Conditions
process.load( "Configuration.Geometry.GeometryIdeal_cff" )
process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
from HLTrigger.Configuration.AutoCondGlobalTag import AutoCondGlobalTag
process.GlobalTag = AutoCondGlobalTag( process.GlobalTag, 'auto:%s'%( condition ) )

## Input
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
process.source = cms.Source(
  "PoolSource"
, fileNames = cms.untracked.vstring( pickRelValInputFiles( cmsswVersion  = cmsswVersion
                                                         , relVal        = 'SingleMu'
                                                         , dataTier      = 'RECO'
                                                         , globalTag     = '%s_RelVal_mu2011B'%( globalTag )
                                                         , maxVersions   = 1
                                                         )
                                   )
, skipBadFiles = cms.untracked.bool( True )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 100 )
)

## Output
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContentNoCleaning
process.out = cms.OutputModule(
  "PoolOutputModule"
, SelectEvents = cms.untracked.PSet(
    SelectEvents = cms.vstring(
      'p'
    )
  )
, fileName = cms.untracked.string( '%s/output/myPatTuple_addTriggerInfo_dataRelVal52X.root'%( os.getenv( "CMSSW_BASE" ) ) )
, outputCommands = cms.untracked.vstring(
    *patEventContentNoCleaning
  )
)
process.out.outputCommands += [
  'keep edmTriggerResults_TriggerResults_*_HLT'
, 'keep *_hltTriggerSummaryAOD_*_*'
]
process.outpath = cms.EndPath(
  process.out
)

## Processing
process.load( "PhysicsTools.PatAlgos.patSequences_cff" )
process.p = cms.Path(
  process.patDefaultSequence
)

from PhysicsTools.PatAlgos.tools.myTools import runOnData
runOnData( process )
process.patJetCorrFactors.useRho = cms.bool( False )
process.out.outputCommands += [ 'drop recoGenJets_*_*_*' ]

# Trigger
from PhysicsTools.PatAlgos.tools.trigTools import *
from PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff import *
process.hallo                     = patTrigger.clone()
process.hallo.saveL1Refs          = cms.bool( True )
process.hallo.addL1Algos          = cms.bool( True )
process.hallo.addPathModuleLabels = cms.bool( True )
process.tschuess           = patTriggerEvent.clone()
process.tschuess.condGtTag = cms.InputTag( 'conditionsInEdm' )
process.tschuess.l1GtTag   = cms.InputTag( 'gtDigis' )
process.moin = selectedMuonTriggerMatchPDSingleMu.clone( matchedCuts = cms.string( 'path( "HLT_RelIso1p0Mu5_v*" ) || path( "HLT_RelIso1p0Mu20_v*" ) || path( "HLT_Mu5_v*" ) || path( "HLT_Mu50_eta2p1_v*" ) || path( "HLT_Mu40_v*" ) || path( "HLT_Mu40_eta2p1_v*" ) || path( "HLT_Mu40_eta2p1_Track60_dEdx3p7_v*" ) || path( "HLT_Mu40_eta2p1_Track50_dEdx3p6_v*" ) || path( "HLT_Mu30_v*" ) || path( "HLT_Mu30_eta2p1_v*" ) || path( "HLT_Mu24_v*" ) || path( "HLT_Mu24_eta2p1_v*" ) || path( "HLT_Mu24_PFJet30_PFJet25_Deta3_CentralPFJet25_v*" ) || path( "HLT_Mu24_CentralPFJet30_CentralPFJet25_v*" ) || path( "HLT_Mu24_CentralPFJet30_CentralPFJet25_v*" ) || path( "HLT_Mu17_eta2p1_TriCentralPFNoPUJet45_35_25_v*" ) || path( "HLT_Mu17_eta2p1_CentralPFNoPUJet30_BTagIPIter_v*" ) || path( "HLT_Mu15_eta2p1_v*" ) || path( "HLT_Mu15_eta2p1_TriCentral_40_20_20_v*" ) || path( "HLT_Mu15_eta2p1_TriCentral_40_20_20_DiBTagIP3D1stTrack_v*" ) || path( "HLT_Mu15_eta2p1_TriCentral_40_20_20_BTagIP3D1stTrack_v*" ) || path( "HLT_Mu15_eta2p1_L1Mu10erJetC12WdEtaPhi1DiJetsC_v*" ) || path( "HLT_Mu12_v*" ) || path( "HLT_Mu12_eta2p1_L1Mu10erJetC12WdEtaPhi1DiJetsC_v*" ) || path( "HLT_Mu12_eta2p1_DiCentral_40_20_v*" ) || path( "HLT_Mu12_eta2p1_DiCentral_40_20_DiBTagIP3D1stTrack_v*" ) || path( "HLT_Mu12_eta2p1_DiCentral_20_v*" ) || path( "HLT_L2Mu70_2Cha_eta2p1_PFMET60_v*" ) || path( "HLT_L2Mu70_2Cha_eta2p1_PFMET55_v*" ) || path( "HLT_IsoMu40_eta2p1_v*" ) || path( "HLT_IsoMu34_eta2p1_v*" ) || path( "HLT_IsoMu30_v*" ) || path( "HLT_IsoMu30_eta2p1_v*" ) || path( "HLT_IsoMu24_v*" ) || path( "HLT_IsoMu24_eta2p1_v*" ) || path( "HLT_IsoMu24_PFJet30_PFJet25_Deta3_CentralPFJet25_v*" ) || path( "HLT_IsoMu24_CentralPFJet30_CentralPFJet25_v*" ) || path( "HLT_IsoMu24_CentralPFJet30_CentralPFJet25_PFMET20_v*" ) || path( "HLT_IsoMu20_eta2p1_v*" ) || path( "HLT_IsoMu20_eta2p1_CentralPFJet80_v*" ) || path( "HLT_IsoMu20_WCandPt80_v*" ) || path( "HLT_IsoMu17_eta2p1_TriCentralPFNoPUJet45_35_25_v*" ) || path( "HLT_IsoMu17_eta2p1_TriCentralPFNoPUJet30_v*" ) || path( "HLT_IsoMu17_eta2p1_DiCentralPFNoPUJet30_v*" ) || path( "HLT_IsoMu17_eta2p1_CentralPFNoPUJet30_v*" ) || path( "HLT_IsoMu17_eta2p1_CentralPFNoPUJet30_BTagIPIter_v*" )' ) )
process.tach = metTriggerMatchHLTMu17.clone( matchedCuts = 'path( "HLT_Mu20_v*" )' )
switchOnTrigger( process )
process.patTrigger.saveL1Refs = cms.bool( True )
switchOnTrigger( process ) # to update event content
switchOnTriggerMatching( process )
switchOnTriggerStandAlone( process )
switchOnTriggerMatchingStandAlone( process )
switchOnTriggerMatchEmbedding( process )
switchOnTrigger( process, triggerProducer = 'hallo', triggerEventProducer = 'tschuess', hltProcess = '*' )
switchOnTriggerMatching( process, triggerMatchers = [ 'moin', 'tach' ], triggerProducer = 'hallo', triggerEventProducer = 'tschuess' )
switchOnTriggerStandAlone( process, triggerProducer = 'hallo', hltProcess = '*' )
switchOnTriggerMatchingStandAlone( process, triggerMatchers = [ 'moin', 'tach' ], triggerProducer = 'hallo' )
switchOnTriggerMatchEmbedding( process, triggerMatchers = [ 'moin', 'tach' ], triggerProducer = 'hallo' )
# print
# print 'Path p'
# print '--> %s'%( process.p )
print
print 'Sequence patDefaultSequenceTrigger'
print '--> %s'%( process.patDefaultSequenceTrigger )
print
print 'Sequence patDefaultSequenceTriggerEvent'
print '--> %s'%( process.patDefaultSequenceTriggerEvent )
print
print 'PoolOutputModule out.outputCommands'
print '--> %s'%( process.out.outputCommands )
