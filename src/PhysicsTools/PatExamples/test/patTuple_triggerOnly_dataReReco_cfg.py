## --
## Start with pre-defined skeleton process
## --
from PhysicsTools.PatAlgos.patTemplate_cfg import *
## ... and modify it according to the needs:
# use latest ReReco as input
process.source.fileNames = [
  '/store/data/Run2010A/Mu/AOD/Nov4ReReco_v1/0011/D2E5D86F-AEEC-DF11-B261-0017A4771028.root' # /Mu/Run2010A-Nov4ReReco_v1/AOD, run 144112, 17717 events
]
# use the correct conditions
process.GlobalTag.globaltag = 'GR_R_38X_V14::All'
# use a sufficient number of events
process.maxEvents.input = 1000
# have a proper output file name
process.out.fileName = 'patTuple_triggerOnly_dataReReco.root'

## ---
## Define the path
## ---
process.p = cms.Path(
) # empty skeleton

### ===========
### PAT trigger
### ===========

## --
## Switch on
## --
from PhysicsTools.PatAlgos.tools.trigTools import switchOnTrigger
switchOnTrigger( process, sequence = 'p' ) # overwrite sequence default "patDefaultSequence", since it is not used in any path

## --
## Modify configuration according to your needs
## --
# add L1 algorithms' collection
process.patTrigger.addL1Algos     = cms.bool( True ) # default: 'False'
# add L1 objects collection-wise
process.patTrigger.l1ExtraMu      = cms.InputTag( 'l1extraParticles', ''           , 'RECO' )
process.patTrigger.l1ExtraNoIsoEG = cms.InputTag( 'l1extraParticles', 'NonIsolated', 'RECO' )
process.patTrigger.l1ExtraIsoEG   = cms.InputTag( 'l1extraParticles', 'Isolated'   , 'RECO' )
process.patTrigger.l1ExtraCenJet  = cms.InputTag( 'l1extraParticles', 'Central'    , 'RECO' )
process.patTrigger.l1ExtraForJet  = cms.InputTag( 'l1extraParticles', 'Forward'    , 'RECO' )
process.patTrigger.l1ExtraTauJet  = cms.InputTag( 'l1extraParticles', 'Tau'        , 'RECO' )
process.patTrigger.l1ExtraETM     = cms.InputTag( 'l1extraParticles', 'MET'        , 'RECO' )
process.patTrigger.l1ExtraHTM     = cms.InputTag( 'l1extraParticles', 'MHT'        , 'RECO' )
# save references to original L1 objects (no tool yet)
process.patTrigger.saveL1Refs = cms.bool( True ) # default: 'False'
from PhysicsTools.PatAlgos.patEventContent_cff import patTriggerL1RefsEventContent
process.out.outputCommands += patTriggerL1RefsEventContent
