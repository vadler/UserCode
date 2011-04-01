import os
cmsswBase = os.getenv( 'CMSSW_BASE' )

from PhysicsTools.PatAlgos.patTemplate_cfg import * # conditions contain L1 menu 'L1GtTriggerMenu_L1Menu_Collisions2011_v0a_mc'

process.options.wantSummary = False
process.source.fileNames    = [ 'rfio:/castor/cern.ch/user/v/vadler/cms/PAT/data/TTJets_TuneD6T_7TeV-madgraph-tauola__Spring11-PU_S1_START311_V1G1-v1__AODSIM/D42E9BB3-C759-E011-B8DC-00A0D1EE8E74.root' ]
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
process.patTrigger.l1ExtraMu      = cms.InputTag( 'l1extraParticles', ''           , '' )
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
