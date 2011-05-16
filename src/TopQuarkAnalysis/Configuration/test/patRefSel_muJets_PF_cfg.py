import FWCore.ParameterSet.Config as cms

process = cms.Process( 'PAT' )


### ======================================================================== ###
###                                                                          ###
###                                 Constants                                ###
###                            (user job steering)                           ###
###                                                                          ###
### ======================================================================== ###


### Data or MC?
runOnMC = False

### Switch on/off selection steps

# Step 1
useTrigger      = True
# Step 2
useGoodVertex   = True
# Step 3b
useLooseMuon    = True
# Step 3a
useTightMuon    = True
# Step 4
useMuonVeto     = True
# Step 5
useElectronVeto = True
# Step 6a
use1Jet         = True
# Step 6b
use2Jets        = False
# Step 6c
use3Jets        = False
# Step 7
use4Jets        = False

addTriggerMatching = True

### Reference selection

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

# Trigger selection according to run range:
# lower range limits available as suffix;
# available are: 000000, 147196 (default)
#triggerSelection       = triggerSelection_147196
#triggerObjectSelection = triggerObjectSelection_147196

### Particle flow

postfix = 'PF'

# subtract charged hadronic pile-up particles (from wrong PVs)
# effects also JECs
usePFnoPU = True # before any top projection

# other switches for PF top projections (default: all 'True')
useNoMuon     = True # before electron top projection
useNoElectron = True # before jet top projection
useNoJet      = True # before tau top projection
useNoTau      = True # before MET top projection

### JEC levels

# levels to be accessible from the jets
# jets are corrected to L3Absolute (MC), L2L3Residual (data) automatically, if enabled here
# and remain uncorrected, if none of these levels is enabled here
useL1FastJet    = True  # needs useL1Offset being off, error otherwise
useL1Offset     = False # needs useL1FastJet being off, error otherwise
useL2Relative   = True
useL3Absolute   = True
# useL2L3Residual = True  # takes effect only on data; currently disabled for CMSSW_4_2_X GlobalTags!
useL5Flavor     = True
useL7Parton     = True

### Input

# list of input files
useRelVals = True # if 'False', "inputFiles" is used
inputFiles = [ '/store/relval/CMSSW_4_2_3/RelValTTbar/GEN-SIM-DIGI-RECO/START42_V12_FastSim_PU_156BxLumiPileUp-v1/0072/0635AA67-B37C-E011-B61F-002618943944.root'
             , '/store/relval/CMSSW_4_2_3/RelValTTbar/GEN-SIM-DIGI-RECO/START42_V12_FastSim_PU_156BxLumiPileUp-v1/0072/0E153885-B17C-E011-8C7D-001A928116E0.root'
             , '/store/relval/CMSSW_4_2_3/RelValTTbar/GEN-SIM-DIGI-RECO/START42_V12_FastSim_PU_156BxLumiPileUp-v1/0072/105E01FE-B57C-E011-9AB4-0018F3D09708.root'
             , '/store/relval/CMSSW_4_2_3/RelValTTbar/GEN-SIM-DIGI-RECO/START42_V12_FastSim_PU_156BxLumiPileUp-v1/0072/120718C8-B67C-E011-A070-001A928116D2.root'
             , '/store/relval/CMSSW_4_2_3/RelValTTbar/GEN-SIM-DIGI-RECO/START42_V12_FastSim_PU_156BxLumiPileUp-v1/0072/1232DFFA-AF7C-E011-983D-002618943831.root'
             ]   # overwritten, if "useRelVals" is 'True'

# maximum number of events
maxInputEvents = -1 # reduce for testing
maxInputEvents = 1000

### Conditions

# GlobalTags (w/o suffix '::All')
globalTagData = 'GR_R_42_V12' # default for CMSSW_4_2_3 RelVals: 'GR_R_42_V12'
globalTagMC   = 'START42_V12' # default for CMSSW_4_2_3 RelVals: 'START42_V12'

### Output

# output file
outputFile = 'patRefSel_muJets_' + postfix + '.root'

# event frequency of Fwk report
fwkReportEvery = 1000

# switch for 'TrigReport'/'TimeReport' at job end
wantSummary = True


###                              End of constants                            ###
###                                                                          ###
### ======================================================================== ###


###
### Basic configuration
###

process.load( "TopQuarkAnalysis.Configuration.patRefSel_basics_cff" )
process.MessageLogger.cerr.FwkReport.reportEvery = fwkReportEvery
process.options.wantSummary = wantSummary
if runOnMC:
  process.GlobalTag.globaltag = globalTagMC   + '::All'
else:
  process.GlobalTag.globaltag = globalTagData + '::All'


###
### Input configuration
###

process.load( "TopQuarkAnalysis.Configuration.patRefSel_inputModule_cfi" )
if useRelVals:
  from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
  if runOnMC:
    inputFiles = pickRelValInputFiles( cmsswVersion  = 'CMSSW_4_2_3'
                                     , relVal        = 'RelValTTbar'
                                     , globalTag     = globalTagMC
                                     , numberOfFiles = -1 # "-1" means "all"
                                     )
  else:
    inputFiles = pickRelValInputFiles( cmsswVersion  = 'CMSSW_4_2_3'
                                     , relVal        = 'Mu'
                                     , dataTier      = 'RECO'
                                     #, globalTag     = globalTagData + '_RelVal_mu2010B'
                                     , globalTag     = globalTagData + '_mu2010B'
                                     , numberOfFiles = -1 # "-1" means "all"
                                     )
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
  triggerConditions = [ triggerSelection ]
)

### Good vertex selection
process.load( "TopQuarkAnalysis.Configuration.patRefSel_goodVertex_cfi" )


###
### PF2PAT configuration
###

### Check JECs

# JEC set
if usePFnoPU:
  jecSetPF += 'chs'
# JEC levels
if useL1FastJet and useL1Offset:
  print 'ERROR: switch off either "L1FastJet" or "L1Offset"'
  exit
jecLevelsPF = []
if useL1FastJet:
  jecLevelsPF.append( 'L1FastJet' )
if useL1Offset:
  jecLevelsPF.append( 'L1Offset' )
if useL2Relative:
  jecLevelsPF.append( 'L2Relative' )
if useL3Absolute:
  jecLevelsPF.append( 'L3Absolute' )
# if useL2L3Residual and not runOnMC:
#   jecLevelsPF.append( 'L2L3Residual' )
if useL5Flavor:
  jecLevelsPF.append( 'L5Flavor' )
if useL7Parton:
  jecLevelsPF.append( 'L7Parton' )

### Switch configuration

process.load( "PhysicsTools.PatAlgos.patSequences_cff" )
from PhysicsTools.PatAlgos.tools.coreTools import *
from PhysicsTools.PatAlgos.tools.pfTools import usePF2PAT
usePF2PAT( process
         , runPF2PAT      = True
         , runOnMC        = runOnMC
         , jetAlgo        = jetAlgo
         , postfix        = postfix
         , jetCorrections = ( jecSetPF
                            , jecLevelsPF
                            )
         )
applyPostfix( process, 'pfNoPileUp'  , postfix ).enable = usePFnoPU
applyPostfix( process, 'pfNoMuon'    , postfix ).enable = useNoMuon
applyPostfix( process, 'pfNoElectron', postfix ).enable = useNoElectron
applyPostfix( process, 'pfNoJet'     , postfix ).enable = useNoJet
applyPostfix( process, 'pfNoTau'     , postfix ).enable = useNoTau
if useL1FastJet:
  applyPostfix( process, 'pfJets', postfix ).Vertices      = cms.InputTag( 'goodOfflinePrimaryVertices' )
  applyPostfix( process, 'pfJets', postfix ).doAreaFastjet = True
  applyPostfix( process, 'pfJets', postfix ).doRhoFastjet  = False
  applyPostfix( process, 'pfPileUp', postfix ).checkClosestZVertex = False

# remove MC matching, object cleaning, photons and taus and adapt JECs
if not runOnMC:
  runOnData( process
           , names = [ 'PFAll' ]
           , postfix = postfix
           )
removeSpecificPATObjects( process
                        , names = [ 'Photons', 'Taus' ]
                        , postfix = postfix
                        ) # includes 'removeCleaning'
# additional event content has to be (re-)added _after_ the call to 'removeCleaning()':
process.out.outputCommands += [ 'keep edmTriggerResults_*_*_*'
                              , 'keep *_hltTriggerSummaryAOD_*_*'
                              # vertices and beam spot
                              , 'keep *_offlineBeamSpot_*_*'
                              , 'keep *_offlinePrimaryVertices*_*_*'
                              , 'keep *_goodOfflinePrimaryVertices*_*_*'
                              ]
if runOnMC:
  process.out.outputCommands += [ 'keep GenEventInfoProduct_*_*_*'
                                , 'keep recoGenParticles_*_*_*'
                                ]


###
### Additional configuration
###

from TopQuarkAnalysis.Configuration.patRefSel_refMuJets_cfi import *

### Muons

intermediatePatMuons.src = cms.InputTag( 'selectedPatMuons' + postfix )
setattr( process, 'intermediatePatMuons' + postfix, intermediatePatMuons )
process.out.outputCommands.append( 'keep *_intermediatePatMuons' + postfix + '_*_*' )

loosePatMuons.src                    = cms.InputTag( 'intermediatePatMuons' + postfix )
loosePatMuons.checkOverlaps.jets.src = cms.InputTag( 'goodPatJets' + postfix )
setattr( process, 'loosePatMuons' + postfix, loosePatMuons )
process.out.outputCommands.append( 'keep *_loosePatMuons' + postfix + '_*_*' )

process.step3b = step3b.clone( src = cms.InputTag( 'loosePatMuons' + postfix ) )

tightPatMuons.src = cms.InputTag( 'loosePatMuons' + postfix )
setattr( process, 'tightPatMuons' + postfix, tightPatMuons )
process.out.outputCommands.append( 'keep *_tightPatMuons' + postfix + '_*_*' )

process.step3a = step3a.clone( src = cms.InputTag( 'tightPatMuons' + postfix ) )

process.step4 = step4.clone( src = cms.InputTag( 'selectedPatMuons' + postfix ) )

### Jets

applyPostfix( process, 'patJetCorrFactors', postfix ).primaryVertices = cms.InputTag( 'goodOfflinePrimaryVertices' )
if useL1FastJet:
  if usePFnoPU:
    kt6PFJetsChs.src = cms.InputTag( 'pfNoElectron' + postfix )
    setattr( process, 'kt6PFJetsChs' + postfix, kt6PFJetsChs )
    getattr( process, 'patPF2PATSequence' + postfix).replace( getattr( process, 'patJetCorrFactors' + postfix )
                                                            , getattr( process, 'kt6PFJetsChs' + postfix ) * getattr( process, 'patJetCorrFactors' + postfix )
                                                            )
    applyPostfix( process, 'patJetCorrFactors', postfix ).rho = cms.InputTag( 'kt6PFJetsChs' + postfix, 'rho' )
  else:
    kt6PFJets.doRhoFastjet = True
    setattr( process, 'kt6PFJets' + postfix, kt6PFJets )
    getattr( process, 'patPF2PATSequence' + postfix).replace( getattr( process, 'patJetCorrFactors' + postfix )
                                                            , getattr( process, 'kt6PFJets' + postfix ) * getattr( process, 'patJetCorrFactors' + postfix )
                                                            )
    applyPostfix( process, 'patJetCorrFactors', postfix ).rho = cms.InputTag( 'kt6PFJets' + postfix, 'rho' )
  process.out.outputCommands.append( 'keep double_*' + postfix + '*_*_' + process.name_() )

goodPatJets.src                        = cms.InputTag( 'selectedPatJets' + postfix  )
goodPatJets.checkOverlaps.muons.src    = cms.InputTag( 'intermediatePatMuons' + postfix )
setattr( process, 'goodPatJets' + postfix, goodPatJets )
process.out.outputCommands.append( 'keep *_goodPatJets' + postfix + '_*_*' )

process.step6a = step6a.clone( src = cms.InputTag( 'goodPatJets' + postfix ) )
process.step6b = step6b.clone( src = cms.InputTag( 'goodPatJets' + postfix ) )
process.step6c = step6c.clone( src = cms.InputTag( 'goodPatJets' + postfix ) )
process.step7  = step7.clone( src = cms.InputTag( 'goodPatJets' + postfix ) )

### Electrons

process.step5 = step5.clone( src = cms.InputTag( 'selectedPatElectrons' + postfix ) )


###
### Selection configuration
###

### Muons

applyPostfix( process, 'patMuons', postfix ).usePV      = muonsUsePV
applyPostfix( process, 'patMuons', postfix ).embedTrack = muonEmbedTrack

applyPostfix( process, 'selectedPatMuons', postfix ).cut = muonCutPF

getattr( process, 'intermediatePatMuons' + postfix ).preselection = looseMuonCutPF

getattr( process, 'loosePatMuons' + postfix ).preselection              = looseMuonCutPF
getattr( process, 'loosePatMuons' + postfix ).checkOverlaps.jets.deltaR = muonJetsDR

getattr( process, 'tightPatMuons' + postfix ).preselection = tightMuonCutPF

### Jets

getattr( process, 'goodPatJets' + postfix ).preselection               = jetCutPF
getattr( process, 'goodPatJets' + postfix ).checkOverlaps.muons.deltaR = jetMuonsDRPF

### Electrons

applyPostfix( process, 'selectedPatElectrons', postfix ).cut = electronCutPF


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
process.p += process.goodOfflinePrimaryVertices
if useGoodVertex:
  process.p += process.step2
process.p += getattr( process, 'patPF2PATSequence' + postfix )
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
