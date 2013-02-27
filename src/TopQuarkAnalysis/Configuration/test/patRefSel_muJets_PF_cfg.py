import FWCore.ParameterSet.Config as cms

process = cms.Process( 'PAT' )


###
### Constants
###

### Data or MC?

runOnMC = True

### Particle flow

usePFnoPU = True

postfix = 'PF'
jetAlgo = 'AK5'

### Switch on/off selection steps

useTrigger      = True
useGoodVertex   = True
useLooseMuon    = True
useTightMuon    = True
useMuonVeto     = True
useElectronVeto = True
use1Jet         = True
use2Jets        = False
use3Jets        = False
use4Jets        = False

addTriggerMatching = True

### Import reference selection

from TopQuarkAnalysis.Configuration.patRefSel_refMuJets import *
#muonsUsePV             = False
#muonEmbedTrack         = True
#muonCutPF                = ''
#looseMuonCutPF           = ''
#tightMuonCutPF           = ''
#muonJetsDR             = 0.3
#jetCutPF               = ''
#jetMuonsDRPF           = 0.1
#electronCutPF            = ''
#triggerSelection       = 'HLT_Mu15 OR HLT_Mu15_v*' # 'HLT_Mu9' for run numbers < 147196
#triggerObjectSelection = 'type("TriggerMuon") && ( path("HLT_Mu15") || path("HLT_Mu15_v*") )'

### Basics

# GlobalTag identifier as in Configuration/PyReleaseValidation/python/autoCond.py
condition = 'com10'
if runOnMC:
  condition = 'startup'
# maximum number of events
maxInputEvents = -1 # reduce for testing
# input files
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
inputFiles = pickRelValInputFiles( cmsswVersion  = 'CMSSW_4_2_2'
                                 , relVal        = 'Mu'
                                 , dataTier      = 'RECO'
                                 , globalTag     = 'GR_R_42_V10_RelVal_mu2010B'
                                 , numberOfFiles = 0 # "0" means "all"
                                 )
if runOnMC:
  inputFiles = pickRelValInputFiles( cmsswVersion  = 'CMSSW_4_2_2'
                                   , relVal        = 'RelValTTbar'
                                   , condition     = condition
                                   , numberOfFiles = 0 # "0" means "all"
                                   )
# output file
outputFile = 'patRefSel_muJets_PF_data.root'
if runOnMC:
  #outputFile = 'patRefSel_muJets_PF_mc.root'
  outputFile = 'patRefSel_muJets_PF_mcPU.root'
# event frequency of Fwk report
fwkReportEvery = 100
# switch for 'TrigReport'/'TimeReport' at job end
wantSummary = True


### ======================================================================== ###


###
### Basic configuration
###

process.load( "TopQuarkAnalysis.Configuration.patRefSel_basics_cff" )
process.MessageLogger.cerr.FwkReport.reportEvery = fwkReportEvery
process.options.wantSummary = wantSummary
from Configuration.PyReleaseValidation.autoCond import autoCond
process.GlobalTag.globaltag = autoCond[ condition ]


###
### Input configuration
###

process.load( "TopQuarkAnalysis.Configuration.patRefSel_inputModule_cff" )
process.source.fileNames = inputFiles
process.maxEvents.input  = maxInputEvents


###
### Output configuration
###

process.load( "TopQuarkAnalysis.Configuration.patRefSel_outputModule_cff" )
# output file name
process.out.fileName = outputFile
# event content
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContent
process.out.outputCommands += patEventContent


###
### Cleaning and trigger selection configuration
###

### Event cleaning
process.load( 'TopQuarkAnalysis.Configuration.patRefSel_eventCleaning_cff' )

### Trigger selection
from TopQuarkAnalysis.Configuration.patRefSel_triggerSelection_cff import triggerResults
process.step1 = triggerResults.clone(
  throw             = False
, triggerConditions = [ triggerSelection ]
)

### Good vertex selection
from TopQuarkAnalysis.Configuration.patRefSel_goodVertex_cff import goodVertex
process.step2 = goodVertex.clone()


###
### PF2PAT configuration
###

process.load( "PhysicsTools.PatAlgos.patSequences_cff" )
from PhysicsTools.PatAlgos.tools.pfTools import usePF2PAT
usePF2PAT( process
         , runPF2PAT = True
         , runOnMC   = runOnMC
         , jetAlgo   = jetAlgo
         , postfix   = postfix
         )
getattr( process, 'pfNoPileUp' + postfix ).enable = usePFnoPU
if usePFnoPU:
  getattr( process, 'pfJets' + postfix ).Vertices      = cms.InputTag( 'goodOfflinePrimaryVertices' )
  getattr( process, 'pfJets' + postfix ).doAreaFastjet = True
  getattr( process, 'pfJets' + postfix ).doRhoFastjet  = False
  getattr( process, 'pfPileUp' + postfix ).checkClosestZVertex = cms.bool(False)


# remove MC matching, object cleaning, photons and taus and adapt JECs
from PhysicsTools.PatAlgos.tools.coreTools import *
if not runOnMC:
  runOnData( process
           , names = [ 'PFAll' ]
           , postfix = postfix
           )
removeSpecificPATObjects( process
                        , names = [ 'Photons', 'Taus' ]
                        , postfix = postfix
                        ) # includes 'removeCleaning'
# additional event content has to be added _after_ the call to 'removeCleaning()':
process.out.outputCommands += [ 'keep edmTriggerResults_*_*_*'
                              , 'keep *_hltTriggerSummaryAOD_*_*'
                              # tracks, vertices and beam spot
                              , 'keep *_offlineBeamSpot_*_*'
                              , 'keep *_offlinePrimaryVertices*_*_*'
                              ]
if runOnMC:
  process.out.outputCommands += [ 'keep GenEventInfoProduct_*_*_*'
                                , 'keep recoGenParticles_*_*_*'
                                ]


###
### Selection configuration
###

from TopQuarkAnalysis.Configuration.patRefSel_refMuJets_cfi import *

### Muons

applyPostfix( process, 'patMuons', postfix ).usePV      = muonsUsePV
applyPostfix( process, 'patMuons', postfix ).embedTrack = muonEmbedTrack

applyPostfix( process, 'selectedPatMuons', postfix ).cut = muonCutPF

intermediatePatMuons.src          = cms.InputTag( 'selectedPatMuons' + postfix )
intermediatePatMuons.preselection = looseMuonCutPF
setattr( process, 'intermediatePatMuons' + postfix, intermediatePatMuons )
process.out.outputCommands.append( 'keep *_intermediatePatMuons' + postfix + '_*_*' )

loosePatMuons.src                       = cms.InputTag( 'intermediatePatMuons' + postfix )
loosePatMuons.preselection              = looseMuonCutPF
loosePatMuons.checkOverlaps.jets.src    = cms.InputTag( 'goodPatJets' + postfix )
loosePatMuons.checkOverlaps.jets.deltaR = muonJetsDR
setattr( process, 'loosePatMuons' + postfix, loosePatMuons )
process.out.outputCommands.append( 'keep *_loosePatMuons' + postfix + '_*_*' )

process.step3b = step3b.clone( src = cms.InputTag( 'loosePatMuons' + postfix ) )

tightPatMuons.src          = cms.InputTag( 'loosePatMuons' + postfix )
tightPatMuons.preselection = tightMuonCutPF
setattr( process, 'tightPatMuons' + postfix, tightPatMuons )
process.out.outputCommands.append( 'keep *_tightPatMuons' + postfix + '_*_*' )

process.step3a = step3a.clone( src = cms.InputTag( 'tightPatMuons' + postfix ) )

process.step4 = step4.clone( src = cms.InputTag( 'selectedPatMuons' + postfix ) )

### Jets

goodPatJets.src          = cms.InputTag( 'selectedPatJets' + postfix  )
goodPatJets.preselection = jetCutPF
goodPatJets.checkOverlaps.muons.src    = cms.InputTag( 'intermediatePatMuons' + postfix )
goodPatJets.checkOverlaps.muons.deltaR = jetMuonsDRPF
setattr( process, 'goodPatJets' + postfix, goodPatJets )
process.out.outputCommands.append( 'keep *_goodPatJets' + postfix + '_*_*' )

if usePFnoPU:
  kt6PFJets.src = cms.InputTag( 'pfNoElectron' + postfix )
  setattr( process, 'kt6PFJets' + postfix, kt6PFJets )
  process.out.outputCommands.append( 'keep double_*' + postfix + '*_*_' + process.name_() )

process.step6a = step6a.clone( src = cms.InputTag( 'goodPatJets' + postfix ) )
process.step6b = step6b.clone( src = cms.InputTag( 'goodPatJets' + postfix ) )
process.step6c = step6c.clone( src = cms.InputTag( 'goodPatJets' + postfix ) )
process.step7  = step7.clone( src = cms.InputTag( 'goodPatJets' + postfix ) )

### Electrons

applyPostfix( process, 'selectedPatElectrons', postfix ).cut = electronCutPF

process.step5 = step5.clone( src = cms.InputTag( 'selectedPatElectrons' + postfix ) )


###
### Scheduling
###

# The additional sequence
patAddOnSequence = cms.Sequence(
  getattr( process, 'intermediatePatMuons' + postfix )
* getattr( process, 'goodPatJets' + postfix )
* getattr( process, 'loosePatMuons' + postfix )
* getattr( process, 'tightPatMuons' + postfix )
)
setattr( process, 'patAddOnSequence' + postfix, patAddOnSequence )

# The path
process.p = cms.Path()
if not runOnMC:
  process.p += process.eventCleaning
if useTrigger:
  process.p += process.step1
if useGoodVertex:
  process.p += process.step2
process.p += getattr( process, 'patPF2PATSequence' + postfix )
if usePFnoPU:
  process.p += getattr( process, 'kt6PFJets' + postfix )
process.p += getattr( process, 'patAddOnSequence' + postfix )
if useLooseMuon:
  process.p += process.step3b
if useTightMuon:
  process.p += process.step3a
if useMuonVeto:
  process.p += process.step4
if useElectronVeto:
  process.p += process.step5
if use1Jet:
  process.p += process.step6a
if use2Jets:
  process.p += process.step6b
if use3Jets:
  process.p += process.step6c
if use4Jets:
  process.p += process.step7


###
### Trigger matching
###

if addTriggerMatching:

  ### Trigger matching configuration
  from TopQuarkAnalysis.Configuration.patRefSel_triggerMatching_cfi import patMuonTriggerMatch
  triggerMatch = patMuonTriggerMatch.clone( matchedCuts = triggerObjectSelection )
  setattr( process, 'triggerMatch' + postfix, triggerMatch )

  ### Enabling trigger matching and embedding
  from PhysicsTools.PatAlgos.tools.trigTools import *
  switchOnTriggerMatchEmbedding( process
                               , triggerMatchers = [ 'triggerMatch' + postfix ]
                               , sequence        = 'patPF2PATSequence' + postfix
                               )
  # remove object cleaning as for the PAT default sequence
  removeCleaningFromTriggerMatching( process
                                   , sequence = 'patPF2PATSequence' + postfix
                                   )
  setattr( process, 'selectedPatMuons' + postfix + 'TriggerMatch', getattr( process, 'selectedPatMuonsTriggerMatch' ) )
  getattr( process, 'selectedPatMuons' + postfix + 'TriggerMatch').src = cms.InputTag( 'selectedPatMuons' + postfix )
  applyPostfix( process, 'patPF2PATSequence', postfix ).replace( getattr( process, 'selectedPatMuonsTriggerMatch' )
                                                               , getattr( process, 'selectedPatMuons' + postfix + 'TriggerMatch' )
                                                               )
  # adapt input sources for additional muon collections
  applyPostfix( process, 'triggerMatch', postfix ).src = cms.InputTag( 'selectedPatMuons' + postfix )
  getattr( process, 'loosePatMuons' + postfix ).src = cms.InputTag( 'selectedPatMuons' + postfix + 'TriggerMatch' )
  getattr( process, 'tightPatMuons' + postfix ).src = cms.InputTag( 'selectedPatMuons' + postfix + 'TriggerMatch' )
  # adapt event content
  process.out.outputCommands += [ 'drop *_selectedPatMuons*_*_*'
                                , 'keep *_selectedPatMuons' + postfix + 'TriggerMatch_*_*'
                                ]
