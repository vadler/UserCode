import os
import FWCore.ParameterSet.Config as cms


### Steering

# Misc
runTest = True

# Event selection
skimProcess = 'SKIM'

### Initialization

process = cms.Process( "USER" )


### Logging

reportEvery = 1000
if runTest:
  reportEvery = 1
process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.MessageLogger.cerr.FwkReport.reportEvery = reportEvery
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool( True )
)
if runTest:
  process.MessageLogger.cerr.threshold = 'INFO'
  process.MessageLogger.categories.append('AnalyzeSpinCorrelations')
  process.MessageLogger.cerr.AnalyzeSpinCorrelations = cms.untracked.PSet(
    limit = cms.untracked.int32( -1 )
  )


### Input

process.source = cms.Source(
  "PoolSource"
, fileNames = cms.untracked.vstring( '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_1_1_RA5.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_2_0_NUC.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_3_0_2FJ.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_4_0_0Dh.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_5_0_VKa.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_6_0_iFd.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_7_0_qsl.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_8_0_Auq.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_9_0_uYe.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_10_0_UU7.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_11_0_LU9.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_12_0_lLr.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_13_0_o6h.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_14_0_PTl.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_15_0_bt0.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_16_0_Uns.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_17_0_1Pr.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_18_0_lZ5.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_19_0_Lwt.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_20_0_cYJ.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_21_0_ej5.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_22_0_tdB.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_23_0_MJM.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_24_0_6NT.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_25_1_jPH.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_26_2_SHW.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_27_2_WgQ.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_28_2_fCQ.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_29_1_fpv.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_30_1_nD8.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_31_1_K32.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_32_1_L2p.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_33_1_WBP.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_34_1_5jV.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_35_1_Rys.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_36_1_fp6.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_37_0_ffT.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_38_0_ncm.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_39_0_sZg.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_40_0_FmI.root'
                                   , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_41_0_OMv.root'
                                   )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( -1 )
)
if runTest:
  process.maxEvents.input = 1000


### Output

outputFile = '%s/output/analyzeSpinCorrelations.root'%( os.getenv( "CMSSW_BASE" ) )
if runTest:
  outputFile = outputFile.replace( 'root', 'test.root' )
process.TFileService = cms.Service(
  "TFileService"
, fileName = cms.string( outputFile )
)
logFile = outputFile.replace( 'root', 'log' )


### Event selection

# Trigger
process.load( "HLTrigger.HLTfilters.hltHighLevel_cfi" )
process.hltHighLevel.TriggerResultsTag = cms.InputTag( 'TriggerResults::%s'%( skimProcess ) )
process.hltHighLevel.HLTPaths          = [ 'pf2PatPath'
                                         ]
process.hltHighLevel_Reference = process.hltHighLevel.clone( HLTPaths = [ 'referencePath'
                                                                        ]
                                                           )


### Analyzer

process.load( "TopQuarkAnalysis.TtSpinCorrelations.analyzeSpinCorrelations_cfi" )
process.analyzeSpinCorrelations_Reference = process.analyzeSpinCorrelations.clone( ttSemiLeptonicEvent = 'ttSemiLepEventReference'
                                                                                           )


### Paths

process.standardPath = cms.Path(
  process.hltHighLevel
* process.analyzeSpinCorrelations
)

if not runTest:
  process.referencePath = cms.Path(
    process.hltHighLevel_Reference
  * process.analyzeSpinCorrelations_Reference
  )

### Messages

print
print 'Output file:'
print '------------'
print outputFile
print
print 'Log file destination:'
print '---------------------'
print logFile
print '================================================================================'
print
