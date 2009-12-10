# Starting with a skeleton process which gets imported with the following line
from PhysicsTools.PatAlgos.patTemplate_cfg import *
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32( 1000 )
process.options.wantSummary                      = True

from PhysicsTools.PatAlgos.tools.coreTools import *
removeMCMatching( process, 'All' )
from PhysicsTools.PatAlgos.tools.tauTools import *
switchTo31Xdefaults( process )

process.allLayer1Jets.addJetCorrFactors = False
process.allLayer1Jets.addTagInfos       = False
process.selectedLayer1Jets.cut          = cms.string( 'pt > 5' )
process.countLayer1Jets.minNumber       = 0

process.source.fileNames =  [
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/575/102A5CB2-B3E1-DE11-A45E-001617E30CE8.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/575/3891C1EB-A3E1-DE11-AB9A-0030487C6090.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/575/46EFF8F0-AAE1-DE11-9232-0016177CA7A0.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/575/82355A91-A4E1-DE11-A5AA-003048D37456.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/575/D0CB515A-A0E1-DE11-8592-001617C3B6FE.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/575/F04F8FC7-A6E1-DE11-AD29-000423D951D4.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/596/0A71AE7F-4DE2-DE11-8B2F-001D09F251CC.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/596/14D22A92-62E2-DE11-9B14-000423D990CC.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/596/263E80C6-41E2-DE11-A194-001617C3B66C.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/596/26EC6965-4CE2-DE11-ABCF-003048D373AE.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/596/2A6903C0-68E2-DE11-B6BA-001D09F28D54.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/596/2EC732CC-5CE2-DE11-A781-001D09F290BF.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/596/4693BF9A-40E2-DE11-BDBD-000423D944F8.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/596/54A3A373-4CE2-DE11-8658-000423D99AAA.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/596/5C4B3A8E-63E2-DE11-A02A-000423D99E46.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/596/68ED95B5-50E2-DE11-B4C8-001D09F27003.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/596/7E34865F-45E2-DE11-896A-000423D98F98.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/596/7EFA67BE-66E2-DE11-AE17-001617C3B79A.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/596/903D2066-61E2-DE11-9F6E-0019B9F704D6.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/596/A0FC9BDF-65E2-DE11-A6A1-000423D174FE.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/596/A4D9D21B-58E2-DE11-8F7A-000423D986A8.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/596/B60CC58F-5CE2-DE11-9FC7-001D09F24DDF.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/596/D0586F90-5FE2-DE11-8976-001D09F24691.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/596/E432BCD7-55E2-DE11-B670-001617C3B6CC.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/123/596/F67BCF17-48E2-DE11-98B1-000423D94534.root'
]
process.maxEvents.input = 100000

# configure HLT
process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff')
process.load('HLTrigger/HLTfilters/hltLevel1GTSeed_cfi')
process.hltLevel1GTSeed.L1TechTriggerSeeding = cms.bool(True)
process.hltLevel1GTSeed.L1SeedsLogicalExpression = cms.string('0 AND (40 OR 41)')

process.p = cms.Path(
  process.hltLevel1GTSeed    *
  process.patDefaultSequence
)

process.out.fileName = cms.untracked.string('/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_3_6/output/patLayer1_fromRECO_triggerInfo_data_123575_123596.root')
process.out.outputCommands += ( [
  'keep *_*_*_*'
] )

# PAT trigger
process.load('PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff')
process.patTriggerSequence.remove( process.patTriggerMatcher )
process.patTriggerEvent.patTriggerMatches = []
from PhysicsTools.PatAlgos.tools.trigTools import switchOnTrigger
switchOnTrigger( process )
