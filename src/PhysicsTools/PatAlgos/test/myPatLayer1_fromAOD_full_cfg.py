from PhysicsTools.PatAlgos.patTemplate_cfg import *
#process.GlobalTag.globaltag = 'STARTUP31X_V8::All'
#process.source.fileNames    = [ '/store/relval/CMSSW_3_4_0_pre1/RelValTTbar/GEN-SIM-RECO/STARTUP31X_V8-v1/0007/BAB355DB-DFB4-DE11-AD73-0019B9F72F97.root' ]
# process.maxEvents.input     = ...
# process.out.outputCommands  = [ ... ]
process.out.fileName        = '/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_5_0_pre3/output/myPatLayer1_fromAOD_full.root'
# process.options.wantSummary = False

# # memory check
# process.SimpleMemoryCheck = cms.Service( "SimpleMemoryCheck",
# #     oncePerEventMode = cms.untracked.bool( True ),
#     ignoreTotal      = cms.untracked.int32( 0 )
# )

# # HLT analyzers
# process.load( "HLTrigger.HLTcore.hltEventAnalyzerAOD_cfi" )
# process.load( "HLTrigger.HLTcore.triggerSummaryAnalyzerAOD_cfi" )

process.p = cms.Path(
#     process.hltEventAnalyzerAOD       +
#     process.triggerSummaryAnalyzerAOD +
    process.patDefaultSequence
)

# # Trigger Test
# process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff" )
# process.patTrigger.onlyStandAlone      = False
# process.patTrigger.addPathModuleLabels = True
# process.patDefaultSequence += process.patTriggerTestSequence
# for matchLabel in process.patTriggerEventTest.patTriggerMatches:
#     process.out.outputCommands += [ 'keep patTriggerObjectsedmAssociation_patTriggerEventTest_' + matchLabel + '_*' ]
# process.patTriggerSequence += process.patTriggerTestMatchEmbedder
# from PhysicsTools.PatAlgos.patEventContent_cff import patTriggerEventContent
# process.out.outputCommands += [ 'keep patTriggerObjects_patTrigger_*_*'
#                               , 'keep patTriggerFilters_patTrigger_*_*'
#                               , 'keep patTriggerPaths_patTrigger_*_*'
#                               , 'keep patTriggerEvent_patTriggerEventTest_*_*'
#                               , 'keep *_cleanPatPhotonsTriggerTestMatch_*_*'
#                               , 'keep *_cleanPatElectronsTriggerTestMatch_*_*'
#                               , 'keep *_cleanPatMuonsTriggerTestMatch_*_*'
#                               , 'keep *_cleanPatTausTriggerTestMatch_*_*'
#                               , 'keep *_cleanPatJetsTriggerTestMatch_*_*'
#                               , 'keep *_patMETsTriggerTestMatch_*_*'
#                               , 'keep patTriggerObjectStandAlones_patTrigger_*_*'
#                               , 'keep patTriggerObjectStandAlonesedmAssociation_*_*_*'
#                               , 'keep edmTriggerResults_TriggerResults_*_HLT'
#                               , 'keep *_hltTriggerSummaryAOD_*_*'
#                               ]

# Trigger
from PhysicsTools.PatAlgos.tools.trigTools import *
### switch START ###
# ## no stand-alone trigger objects
# switchOnTrigger( process )
## all; default to run myTriggerTest_cfg.py afterwards
switchOnTriggerAll( process )
process.patTrigger.addPathModuleLabels = True
switchOnTriggerMatchEmbedding( process )
process.out.outputCommands += [ 'keep edmTriggerResults_TriggerResults_*_HLT'
                              , 'keep *_hltTriggerSummaryAOD_*_*'
                              ]
# ## stand-alone trigger objects only
# switchOnTriggerStandAlone( process )
# ## embedded trigger object matches only
# switchOnTriggerMatchEmbedding( process )
### switch END ###
