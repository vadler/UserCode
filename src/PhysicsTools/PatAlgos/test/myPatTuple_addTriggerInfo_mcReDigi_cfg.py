import os
cmsswBase = os.getenv( 'CMSSW_BASE' )

from PhysicsTools.PatAlgos.patTemplate_cfg import *
from Configuration.PyReleaseValidation.autoCond import autoCond
process.GlobalTag.globaltag = cms.string( autoCond[ 'startup' ] ) # contains L1 menu 'L1GtTriggerMenu_L1Menu_Collisions2011_v0a_mc'

# To run on 3XY MC
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import run42xOn3yzMcInput
run42xOn3yzMcInput( process )

process.options.wantSummary = False
process.source.fileNames    = [ 'rfio:/castor/cern.ch/user/v/vadler/cms/PAT/data/ZJetToMuMu_Pt_15to3000_TuneZ2_Flat_7TeV_pythia6__Fall10-E7TeV_ProbDist_2010Data_BX156_START38_V12-v1__GEN-SIM-RECO/FED5121F-B1E4-DF11-B53F-00261834B580.root' ]
process.maxEvents.input     = 100
process.out.fileName        = '%s/output/myPatTuple_addTriggerInfo_mcReDigi.root'%( cmsswBase )

process.p = cms.Path(
)

# Trigger
from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTrigger( process, sequence = 'p', hltProcess = '*' )
process.patTrigger.triggerResults = cms.InputTag( 'TriggerResults'      , '', '*' )
process.patTrigger.triggerEvent   = cms.InputTag( 'hltTriggerSummaryAOD', '', '*' )
process.patTrigger.addL1Algos     = cms.bool( True )
process.patTrigger.l1ExtraMu      = cms.InputTag( 'l1extraParticles', ''           , 'RECO' ) # process '*' should produce errors
process.patTrigger.l1ExtraNoIsoEG = cms.InputTag( 'l1extraParticles', 'NonIsolated', '' )
process.patTrigger.l1ExtraIsoEG   = cms.InputTag( 'l1extraParticles', 'Isolated'   , '' )
process.patTrigger.l1ExtraCenJet  = cms.InputTag( 'l1extraParticles', 'Central'    , '' )
process.patTrigger.l1ExtraForJet  = cms.InputTag( 'l1extraParticles', 'Forward'    , '' )
process.patTrigger.l1ExtraTauJet  = cms.InputTag( 'l1extraParticles', 'Tau'        , '' )
process.patTrigger.l1ExtraETM     = cms.InputTag( 'l1extraParticles', 'MET' )
process.patTrigger.l1ExtraHTM     = cms.InputTag( 'l1extraParticles', 'MHT' )
process.patTrigger.mainBxOnly     = cms.bool( False )
process.patTrigger.saveL1Refs     = cms.bool( True )
process.patTriggerEvent.triggerResults = cms.InputTag( 'TriggerResults'      , '' )
process.patTriggerEvent.triggerEvent   = cms.InputTag( 'hltTriggerSummaryAOD' )
switchOnTrigger( process, sequence = 'p', hltProcess = '*' )