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
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/122/314/F62B040F-6CD8-DE11-9007-001D09F24664.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/122/314/F4387297-74D8-DE11-996C-001D09F24F1F.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/122/314/EE7B1AC4-6CD8-DE11-97BB-0030487A1FEC.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/122/314/9264A28F-87D8-DE11-83A1-001D09F24763.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/122/314/7C7495C2-71D8-DE11-ACF2-001D09F248F8.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/122/314/4CAB3B6C-6BD8-DE11-845C-000423D9890C.root',
  '/store/data/BeamCommissioning09/MinimumBias/RECO/v2/000/122/314/02E8544C-70D8-DE11-85CF-001617C3B66C.root'
]
process.maxEvents.input = -1

# configure HLT
process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff')
process.load('HLTrigger/HLTfilters/hltLevel1GTSeed_cfi')
process.hltLevel1GTSeed.L1TechTriggerSeeding = cms.bool(True)
process.hltLevel1GTSeed.L1SeedsLogicalExpression = cms.string('0 AND (40 OR 41)')

process.p = cms.Path(
  process.hltLevel1GTSeed    *
  process.patDefaultSequence
)

process.out.fileName = cms.untracked.string('/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_3_6/output/patLayer1_fromRECO_triggerInfo_data_122314.root')
process.out.outputCommands += ( [
  'keep *_*_*_*'
] )

# PAT trigger
process.load('PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff')
process.patTriggerSequence.remove( process.patTriggerMatcher )
process.patTriggerEvent.patTriggerMatches = []
from PhysicsTools.PatAlgos.tools.trigTools import switchOnTrigger
switchOnTrigger( process )
