import os
from PhysicsTools.PatAlgos.patTemplate_cfg import *
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *
process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] )
process.source.fileNames    = pickRelValInputFiles()
process.options.wantSummary = False
process.out.fileName        = '%s/output/myPatTuple_addTriggerInfo_mcRelVal.root'%( os.getenv( "CMSSW_BASE" ) )

# # memory check
# process.SimpleMemoryCheck = cms.Service( "SimpleMemoryCheck"
# # , oncePerEventMode = cms.untracked.bool( True )
# , ignoreTotal      = cms.untracked.int32( 0 )
# )

# Internediate step to get the taus working in CMSSW_3_10_X RelVals
process.load("RecoTauTag.Configuration.RecoPFTauTag_cff")
process.p0 = cms.Path(
  process.PFTau
)

process.p = cms.Path(
  process.patDefaultSequence
)
process.patTaus.tauIDSources = cms.PSet(
        leadingTrackFinding = cms.InputTag("shrinkingConePFTauDiscriminationByLeadingTrackFinding"),
        #leadingTrackPtCut = cms.InputTag("shrinkingConePFTauDiscriminationByLeadingTrackPtCut"),
        leadingPionPtCut = cms.InputTag("shrinkingConePFTauDiscriminationByLeadingPionPtCut"),
        #trackIsolation = cms.InputTag("shrinkingConePFTauDiscriminationByTrackIsolation"),
        #trackIsolationUsingLeadingPion = cms.InputTag("shrinkingConePFTauDiscriminationByTrackIsolationUsingLeadingPion"),
        #ecalIsolation = cms.InputTag("shrinkingConePFTauDiscriminationByECALIsolation"),
        #ecalIsolationUsingLeadingPion = cms.InputTag("shrinkingConePFTauDiscriminationByECALIsolationUsingLeadingPion"),
        #byIsolation = cms.InputTag("shrinkingConePFTauDiscriminationByIsolation"),
        byIsolationUsingLeadingPion = cms.InputTag("shrinkingConePFTauDiscriminationByIsolationUsingLeadingPion"),
        againstElectron = cms.InputTag("shrinkingConePFTauDiscriminationAgainstElectron"),
        againstMuon = cms.InputTag("shrinkingConePFTauDiscriminationAgainstMuon")#,
        #byTaNC = cms.InputTag("shrinkingConePFTauDiscriminationByTaNC"),
        #byTaNCfrOnePercent = cms.InputTag("shrinkingConePFTauDiscriminationByTaNCfrOnePercent"),
        #byTaNCfrHalfPercent = cms.InputTag("shrinkingConePFTauDiscriminationByTaNCfrHalfPercent"),
        #byTaNCfrQuarterPercent = cms.InputTag("shrinkingConePFTauDiscriminationByTaNCfrQuarterPercent"),
        #byTaNCfrTenthPercent = cms.InputTag("shrinkingConePFTauDiscriminationByTaNCfrTenthPercent")
    )

process.out.outputCommands += [
  'keep edmTriggerResults_TriggerResults_*_HLT'
, 'keep *_hltTriggerSummaryAOD_*_*'
]

# from PhysicsTools.PatAlgos.tools.coreTools import removeCleaning
# removeCleaning( process )

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
process.moin = cleanMuonTriggerMatchPDMu.clone()
process.tach = metTriggerMatchHLTMu3.clone()
switchOnTrigger( process )
# process.patTrigger.saveL1Refs = cms.bool( True )
# switchOnTrigger( process ) # to update event content
switchOnTriggerMatching( process )
switchOnTriggerStandAlone( process )
switchOnTriggerMatchingStandAlone( process )
switchOnTriggerMatchEmbedding( process )
switchOnTrigger( process, triggerProducer = 'hallo', triggerEventProducer = 'tschuess', hltProcess = '*' )
switchOnTriggerMatching( process, triggerMatchers = [ 'moin', 'tach' ], triggerProducer = 'hallo', triggerEventProducer = 'tschuess' )
switchOnTriggerStandAlone( process, triggerProducer = 'hallo', hltProcess = '*' )
switchOnTriggerMatchingStandAlone( process, triggerMatchers = [ 'moin', 'tach' ], triggerProducer = 'hallo' )
switchOnTriggerMatchEmbedding( process, triggerMatchers = [ 'moin', 'tach' ], triggerProducer = 'hallo' )
# removeCleaningFromTriggerMatching( process )
# print
# print 'Path p'
# print '--> %s'%( process.p )
print
print 'Sequence patTriggerSequence'
print '--> %s'%( process.patTriggerSequence )
print
print 'Sequence patTriggerEventSequence'
print '--> %s'%( process.patTriggerEventSequence )
print
print 'PoolOutputModule out.outputCommands'
print '--> %s'%( process.out.outputCommands )
