import FWCore.ParameterSet.Config as cms

process = cms.Process( 'TEST' )

process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.MessageLogger.cerr.FwkReport.reportEvery = 999999

process.source = cms.Source(
  "PoolSource"
, fileNames = cms.untracked.vstring( '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_14_0_Bqt.root' )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( -1 )
)

process.load( "HLTrigger.HLTfilters.hltHighLevel_cfi" )
process.hltHighLevel.TriggerResultsTag = cms.InputTag( 'TriggerResults::SKIM' )
process.hltHighLevel.HLTPaths          = [ 'pf2PatPath'
                                         ]
process.hltHighLevel_Reference = process.hltHighLevel.clone( HLTPaths = [ 'referencePath'
                                                                        ]
                                                           )

process.dump = cms.EDAnalyzer(
  'DumpEventNumbers'
, fileName = cms.string( 'dump.log' )
)
process.dumpRef = process.dump.clone( fileName = 'dumpRef.log' )
process.dumpAll = process.dump.clone( fileName = 'dumpAll.log' )

process.p = cms.Path(
  process.dumpAll
* process.hltHighLevel
* process.dump
* process.hltHighLevel_Reference
* process.dumpRef
)
