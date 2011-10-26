import os
import FWCore.ParameterSet.Config as cms


### Steering

# Misc
runTest = True

# Input
runOnMC = True
sample  = 'RelValTTbar'
#sample  = 'RelValZMM'
#sample  = 'RelValWM'
#sample  = 'Mu'
#sample  = 'Jet'

# Event selection
hitFitProcess = 'HitFit'


### Initialization

process = cms.Process( "TEST" )


### Logging

reportEvery = 1000
if runTest:
  reportEvery = 1
process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.MessageLogger.cerr.FwkReport.reportEvery = reportEvery
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool( True )
)
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.categories.append('TtSemiLeptonicEvent')
process.MessageLogger.cerr.TtSemiLeptonicEvent = cms.untracked.PSet(
  limit = cms.untracked.int32( -1 )
)


### Input

inputFiles = [ 'rfio:%s/cms/Top/data/hitFitPatSkimPF2PAT_%s.root'%( os.getenv( "CASTOR_HOME" ), sample )
             ]
if runTest:
  #inputFiles = [ 'file:hitFitPatSkimPF2PAT.root'
               #]
  inputFiles = [ 'rfio:/castor/cern.ch/user/v/vadler/cms/Top/CMSSW_4_4_1/data/v2/hitFitPatSkimPF2PAT_1_2_rku.root'
               , 'rfio:/castor/cern.ch/user/v/vadler/cms/Top/CMSSW_4_4_1/data/v2/hitFitPatSkimPF2PAT_2_1_hGY.root'
               , 'rfio:/castor/cern.ch/user/v/vadler/cms/Top/CMSSW_4_4_1/data/v2/hitFitPatSkimPF2PAT_3_1_W60.root'
               , 'rfio:/castor/cern.ch/user/v/vadler/cms/Top/CMSSW_4_4_1/data/v2/hitFitPatSkimPF2PAT_4_1_oYI.root'
               , 'rfio:/castor/cern.ch/user/v/vadler/cms/Top/CMSSW_4_4_1/data/v2/hitFitPatSkimPF2PAT_5_1_9Gv.root'
               , 'rfio:/castor/cern.ch/user/v/vadler/cms/Top/CMSSW_4_4_1/data/v2/hitFitPatSkimPF2PAT_6_1_C1R.root'
               , 'rfio:/castor/cern.ch/user/v/vadler/cms/Top/CMSSW_4_4_1/data/v2/hitFitPatSkimPF2PAT_7_1_YR9.root'
               , 'rfio:/castor/cern.ch/user/v/vadler/cms/Top/CMSSW_4_4_1/data/v2/hitFitPatSkimPF2PAT_8_1_WtQ.root'
               , 'rfio:/castor/cern.ch/user/v/vadler/cms/Top/CMSSW_4_4_1/data/v2/hitFitPatSkimPF2PAT_9_3_BrI.root'
               , 'rfio:/castor/cern.ch/user/v/vadler/cms/Top/CMSSW_4_4_1/data/v2/hitFitPatSkimPF2PAT_10_2_tBm.root'
               ]
process.source = cms.Source(
  "PoolSource"
, fileNames = cms.untracked.vstring( inputFiles )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( -1 )
)
if runTest:
  process.maxEvents.input = 10000


### Output

outputFile = '%s/output/topHitFitAnalyzer_%s.root'%( os.getenv( "CMSSW_BASE" ), sample )
if runTest:
  outputFile = 'topHitFitAnalyzer.root'
process.TFileService = cms.Service(
  "TFileService"
, fileName = cms.string( outputFile )
)


### Event selection

# Trigger
process.load( "HLTrigger.HLTfilters.hltHighLevel_cfi" )
process.hltHighLevel.TriggerResultsTag = cms.InputTag( 'TriggerResults::%s'%( hitFitProcess ) )
process.hltHighLevel.HLTPaths          = [ 'HitFit_PF2PATHitFit'
                                         ]
process.hltHighLevel_Reference = process.hltHighLevel.clone( HLTPaths = [ 'HitFit_ReferenceHitFit'
                                                                        ]
                                                           )


### Analyzer

process.load( "TopQuarkAnalysis.TopMassSemiLeptonic.topHitFitAnalyzer_cfi" )
process.topHitFitAnalyzer.verbosity = 1
if runTest:
  process.topHitFitAnalyzer.verbosity = 11
process.topHitFitAnalyzer_Reference = process.topHitFitAnalyzer.clone( ttSemiLeptonicEvent = 'ttSemiLepEventReference'
                                                                     , patMuons            = 'referencePatMuons'
                                                                     , patJets             = 'referencePatJets'
                                                                     )
process.topHitFitAnalyzer_ReferenceBTag = process.topHitFitAnalyzer.clone( ttSemiLeptonicEvent = 'ttSemiLepEventReferenceBTag'
                                                                         , patMuons            = 'referencePatMuons'
                                                                         , patJets             = 'referencePatJets'
                                                                         )


### Paths

process.p = cms.Path(
  process.hltHighLevel
* process.topHitFitAnalyzer
)

process.p_Reference = cms.Path(
  process.hltHighLevel_Reference
* process.topHitFitAnalyzer_Reference
)

process.p_ReferenceBTag = cms.Path(
  process.hltHighLevel_Reference
* process.topHitFitAnalyzer_ReferenceBTag
)


### Messages

print
print 'Log file:'
if runTest:
  print 'topHitFitAnalyzer.log'
else:
  print '%s/output/topHitFitAnalyzer_%s.log'%( os.getenv( "CMSSW_BASE" ), sample )
print '================================================================================'
print