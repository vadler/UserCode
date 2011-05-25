#
# This file contains the Top PAG reference selection work-flow for mu + jets analysis.
# as defined in
# https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopLeptonPlusJetsRefSel_mu#Selection_Version_SelV4_valid_fr
#

import sys

import FWCore.ParameterSet.Config as cms

process = cms.Process( 'PAT' )


### ======================================================================== ###
###                                                                          ###
###                                 Constants                                ###
###                            (user job steering)                           ###
###                                                                          ###
### ======================================================================== ###


### Data or MC?
runOnMC       = True
runOnSpring11 = False # takes effect only on MC, overwritten, if "useRelVals" is 'True'

### Standard and PF reconstruction
useStandardPAT = True
runPF2PAT      = True

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

# Trigger selection according to run range resp. MC sample:
# lower range limits for data available as suffix;
# available are: 000000, 147196, 160404, 163270 (default)
# sample name for MC available as suffix;
# available are: Spring11 (default)
#triggerSelectionData       = triggerSelection_163270
#triggerObjectSelectionData = triggerObjectSelection_163270
#triggerSelectionMC       = triggerSelection_Spring11
#triggerObjectSelectionMC = triggerObjectSelection_Spring11

postfix = 'PF' # needs to be a non-empty string, if 'useStandardPAT' = True

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
useRelVals = True # if 'False', "inputFiles" is used; if 'True' "runOnSpring11" is set to 'False'
#inputFiles = [ '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/FE6FA0FF-B676-E011-9456-001617E30D0A.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/F609315B-9376-E011-A06F-003048F01E88.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/F2A39943-B676-E011-B28A-000423D9A2AE.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/F03CB9FD-BD76-E011-9078-001617E30D00.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/EC843243-B676-E011-89FF-000423D98950.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/EA829D30-A876-E011-A47D-000423D9997E.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/E86A9D42-C976-E011-909B-0030487CD840.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/DE8B3E69-8676-E011-826B-003048F024DC.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/DA272D6F-8676-E011-A525-003048F1110E.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/D8937201-9076-E011-A6B9-001D09F24498.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/D461BE46-D076-E011-AF9C-0030487CD704.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/C0F87B60-CB76-E011-AE2E-0030487CD840.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/BE0175F3-C976-E011-AE92-003048F1110E.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/BA9FE134-8276-E011-80FE-001D09F241B9.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/B63D9E75-B376-E011-B6C3-000423D9A212.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/ACBCC6DB-B476-E011-97CA-003048F1C836.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/AA1D0A95-7C76-E011-A9D8-003048F024DE.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/A89B4AAE-7E76-E011-B4A1-003048F11C28.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/A08EDA34-8276-E011-8F43-001D09F291D2.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/9E28883B-C276-E011-BDA3-003048F024C2.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/987F96DB-C776-E011-95B8-003048F117EA.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/924A094B-9976-E011-AF39-003048D37560.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/907A87CB-8576-E011-BB72-0030487CD704.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/88C68ACD-4077-E011-B206-0030487CD14E.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/887DE118-C076-E011-A099-001617C3B5F4.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/7EE5B273-E076-E011-9249-0030487C2B86.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/7A6856A0-C376-E011-B99C-003048CFB40C.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/5E734B5A-8F76-E011-A9AF-003048F117B4.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/5032D5B2-9276-E011-92E6-001D09F295FB.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/3AC49569-8676-E011-8A6D-003048F1C424.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/3A50C497-9D76-E011-B64F-000423D996C8.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/36E69F60-9976-E011-8171-003048D2BC42.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/34AA8C4E-BB76-E011-A3BF-001D09F2AF96.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/2EF394CD-8C76-E011-AE4B-003048D2BB90.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/2EC4A6A3-B076-E011-8B6F-000423D9A212.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/2E571281-8E76-E011-89EE-001D09F241B9.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/2C259724-C776-E011-8329-003048F1110E.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/2C012DD4-8776-E011-949C-003048F118AC.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/1E24DE82-AE76-E011-AAA4-003048F11CF0.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/148E2B4A-9976-E011-A201-001D09F24E39.root'
             #, '/store/data/Run2011A/MuHad/AOD/PromptReco-v2/000/163/817/0278B64E-AA76-E011-8F0E-003048F024FA.root'
inputFiles = [ 'rfio:/castor/cern.ch/user/v/vadler/cms/PAT/data/TTJets_TuneD6T_7TeV-madgraph-tauola__Spring11-PU_S1_START311_V1G1-v1__AODSIM/D42E9BB3-C759-E011-B8DC-00A0D1EE8E74.root'
             ]   # overwritten, if "useRelVals" is 'True'

# maximum number of events
maxInputEvents = -1 # reduce for testing
maxInputEvents = 1000

### Conditions

# GlobalTags (w/o suffix '::All')
globalTagData = 'GR_R_41_V0' # default for CMSSW_4_1_6 RelVals: 'GR_R_41_V0'
globalTagMC   = 'START41_V0' # default for CMSSW_4_1_6 RelVals: 'START41_V0'

### Output

# output file
outputFile = 'patRefSel_muJets.root'

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
    runOnSpring11 = False
    inputFiles    = pickRelValInputFiles( cmsswVersion  = 'CMSSW_4_1_6'
                                        , relVal        = 'RelValTTbar'
                                        , globalTag     = 'START311_V2'
                                        , numberOfFiles = 0 # "0" means "all"
                                        )
  else:
    inputFiles = pickRelValInputFiles( cmsswVersion  = 'CMSSW_4_1_6'
                                     , relVal        = 'Mu'
                                     , dataTier      = 'RECO'
                                     , globalTag     = 'GR_R_311_V2_RelVal_mu2010B'
                                     , numberOfFiles = 0 # "0" means "all"
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
# clear event selection
process.out.SelectEvents.SelectEvents = []


###
### Cleaning and trigger selection configuration
###

### Event cleaning
process.load( 'TopQuarkAnalysis.Configuration.patRefSel_eventCleaning_cff' )

### Trigger selection
from TopQuarkAnalysis.Configuration.patRefSel_triggerSelection_cff import triggerResults, hltTag, hltTagSpring11
if runOnMC:
  triggerSelection = triggerSelectionMC
  if runOnSpring11:
    hltTag = hltTagSpring11
else:
  triggerSelection = triggerSelectionData
process.step1 = triggerResults.clone(
  hltResults        = hltTag
, triggerConditions = [ triggerSelection ]
)

### Good vertex selection
process.load( "TopQuarkAnalysis.Configuration.patRefSel_goodVertex_cfi" )


###
### PAT/PF2PAT configuration
###

if useStandardPAT and runPF2PAT:
  if postfix == '':
    sys.exit( 'ERROR: running standard PAT and PF2PAT in parallel requires a defined "postfix" for PF2PAT' )
if not useStandardPAT and not runPF2PAT:
  sys.exit( 'ERROR: standard PAT and PF2PAT are both switched off' )

process.load( "PhysicsTools.PatAlgos.patSequences_cff" )
from PhysicsTools.PatAlgos.tools.coreTools import *

### Check JECs

# JEC set
jecSet   = jecSetBase + 'Calo'
jecSetPF = jecSetBase + 'PF'
if usePFnoPU:
  jecSetPF += 'chs'

# JEC levels
if useL1FastJet and useL1Offset:
  sys.exit( 'ERROR: switch off either "L1FastJet" or "L1Offset"' )
jecLevels = []
if useL1FastJet:
  jecLevels.append( 'L1FastJet' )
if useL1Offset:
  jecLevels.append( 'L1Offset' )
if useL2Relative:
  jecLevels.append( 'L2Relative' )
if useL3Absolute:
  jecLevels.append( 'L3Absolute' )
# if useL2L3Residual and not runOnMC:
#   jecLevelsPF.append( 'L2L3Residual' )
if useL5Flavor:
  jecLevels.append( 'L5Flavor' )
if useL7Parton:
  jecLevels.append( 'L7Parton' )

### Switch configuration

if runPF2PAT:
  from PhysicsTools.PatAlgos.tools.pfTools import usePF2PAT
  usePF2PAT( process
           , runPF2PAT = runPF2PAT
           , runOnMC   = runOnMC
           , jetAlgo   = jetAlgo
           , postfix   = postfix
           )
  applyPostfix( process, 'pfNoPileUp'  , postfix ).enable = usePFnoPU
  applyPostfix( process, 'pfNoMuon'    , postfix ).enable = useNoMuon
  applyPostfix( process, 'pfNoElectron', postfix ).enable = useNoElectron
  applyPostfix( process, 'pfNoJet'     , postfix ).enable = useNoJet
  applyPostfix( process, 'pfNoTau'     , postfix ).enable = useNoTau
  applyPostfix( process, 'patJetCorrFactors', postfix ).payload = jecSetPF
  applyPostfix( process, 'patJetCorrFactors', postfix ).levels  = jecLevels
  if useL1FastJet:
    applyPostfix( process, 'pfPileUp', postfix ).Vertices = cms.InputTag( 'goodOfflinePrimaryVertices' )
    applyPostfix( process, 'pfJets', postfix ).doAreaFastjet = True
    applyPostfix( process, 'pfJets', postfix ).doRhoFastjet  = False

# remove MC matching, object cleaning, photons and taus
if useStandardPAT:
  if not runOnMC:
    runOnData( process )
  removeSpecificPATObjects( process
                          , names = [ 'Photons', 'Taus' ]
                          ) # includes 'removeCleaning'
if runPF2PAT:
  if not runOnMC:
    runOnData( process
             , names = [ 'PFAll' ]
             , postfix = postfix
             )
  removeSpecificPATObjects( process
                          , names = [ 'Photons', 'Taus' ]
                          , postfix = postfix
                          ) # includes 'removeCleaning'

# JetCorrFactorsProducer configuration has to be fixed _after_ any call to 'removeCleaning()':
if useStandardPAT:
  process.patJetCorrFactors.payload = jecSet
  process.patJetCorrFactors.levels  = jecLevels
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

if useStandardPAT:

  process.intermediatePatMuons = intermediatePatMuons.clone()
  process.loosePatMuons        = loosePatMuons.clone()
  process.step3b               = step3b.clone()
  process.tightPatMuons        = tightPatMuons.clone()
  process.step3a               = step3a.clone()
  process.step4                = step4.clone()

  if useL1FastJet:
    process.kt6PFJets = kt6PFJets.clone( doRhoFastjet = True )
    process.patDefaultSequence.replace( process.patJetCorrFactors
                                      , process.kt6PFJets * process.patJetCorrFactors
                                      )
    process.out.outputCommands.append( 'keep double_*_*_' + process.name_() )

  process.goodPatJets = goodPatJets.clone()
  process.step6a      = step6a.clone()
  process.step6b      = step6b.clone()
  process.step6c      = step6c.clone()
  process.step7       = step7.clone()

  process.step5 = step5.clone()

if runPF2PAT:

  ### Muons

  intermediatePatMuonsPF = intermediatePatMuons.clone( src = cms.InputTag( 'selectedPatMuons' + postfix ) )
  setattr( process, 'intermediatePatMuons' + postfix, intermediatePatMuonsPF )

  loosePatMuonsPF = loosePatMuons.clone( src = cms.InputTag( 'intermediatePatMuons' + postfix ) )
  setattr( process, 'loosePatMuons' + postfix, loosePatMuonsPF )
  getattr( process, 'loosePatMuons' + postfix ).checkOverlaps.jets.src = cms.InputTag( 'goodPatJets' + postfix )

  step3bPF = step3b.clone( src = cms.InputTag( 'loosePatMuons' + postfix ) )
  setattr( process, 'step3b' + postfix, step3bPF )

  tightPatMuonsPF = tightPatMuons.clone( src = cms.InputTag( 'loosePatMuons' + postfix ) )
  setattr( process, 'tightPatMuons' + postfix, tightPatMuonsPF )

  step3aPF = step3a.clone( src = cms.InputTag( 'tightPatMuons' + postfix ) )
  setattr( process, 'step3a' + postfix, step3aPF )

  step4PF = step4.clone( src = cms.InputTag( 'selectedPatMuons' + postfix ) )
  setattr( process, 'step4' + postfix, step4PF )

  ### Jets

  applyPostfix( process, 'patJetCorrFactors', postfix ).primaryVertices = cms.InputTag( 'goodOfflinePrimaryVertices' )
  if useL1FastJet:
    if usePFnoPU:
      kt6PFJetsPFChs = kt6PFJetsChs.clone( src = cms.InputTag( 'pfNoElectron' + postfix ) )
      setattr( process, 'kt6PFJetsChs' + postfix, kt6PFJetsPFChs )
      getattr( process, 'patPF2PATSequence' + postfix).replace( getattr( process, 'patJetCorrFactors' + postfix )
                                                              , getattr( process, 'kt6PFJetsChs' + postfix ) * getattr( process, 'patJetCorrFactors' + postfix )
                                                              )
      applyPostfix( process, 'patJetCorrFactors', postfix ).rho = cms.InputTag( 'kt6PFJetsChs' + postfix, 'rho' )
    else:
      kt6PFJetsPF = kt6PFJets.clone( doRhoFastjet = True )
      setattr( process, 'kt6PFJets' + postfix, kt6PFJetsPF )
      getattr( process, 'patPF2PATSequence' + postfix).replace( getattr( process, 'patJetCorrFactors' + postfix )
                                                              , getattr( process, 'kt6PFJets' + postfix ) * getattr( process, 'patJetCorrFactors' + postfix )
                                                              )
      applyPostfix( process, 'patJetCorrFactors', postfix ).rho = cms.InputTag( 'kt6PFJets' + postfix, 'rho' )
    process.out.outputCommands.append( 'keep double_*' + postfix + '*_*_' + process.name_() )

  goodPatJetsPF = goodPatJets.clone( src = cms.InputTag( 'selectedPatJets' + postfix ) )
  setattr( process, 'goodPatJets' + postfix, goodPatJetsPF )
  getattr( process, 'goodPatJets' + postfix ).checkOverlaps.muons.src = cms.InputTag( 'intermediatePatMuons' + postfix )

  step6aPF = step6a.clone( src = cms.InputTag( 'goodPatJets' + postfix ) )
  setattr( process, 'step6a' + postfix, step6aPF )
  step6bPF = step6b.clone( src = cms.InputTag( 'goodPatJets' + postfix ) )
  setattr( process, 'step6b' + postfix, step6bPF )
  step6cPF = step6c.clone( src = cms.InputTag( 'goodPatJets' + postfix ) )
  setattr( process, 'step6c' + postfix, step6cPF )
  step7PF = step7.clone( src = cms.InputTag( 'goodPatJets' + postfix ) )
  setattr( process, 'step7'  + postfix, step7PF  )

  ### Electrons

  step5PF = step5.clone( src = cms.InputTag( 'selectedPatElectrons' + postfix ) )
  setattr( process, 'step5' + postfix, step5PF )

process.out.outputCommands.append( 'keep *_intermediatePatMuons*_*_*' )
process.out.outputCommands.append( 'keep *_loosePatMuons*_*_*' )
process.out.outputCommands.append( 'keep *_tightPatMuons*_*_*' )
process.out.outputCommands.append( 'keep *_goodPatJets*_*_*' )


###
### Selection configuration
###

if useStandardPAT:

  ### Muons

  process.patMuons.usePV      = muonsUsePV
  process.patMuons.embedTrack = muonEmbedTrack

  process.selectedPatMuons.cut = muonCut

  process.intermediatePatMuons.preselection = looseMuonCut

  process.loosePatMuons.checkOverlaps.jets.deltaR = muonJetsDR

  process.tightPatMuons.preselection = tightMuonCut

  ### Jets

  process.goodPatJets.preselection = jetCut

  ### Electrons

  process.selectedPatElectrons.cut = electronCut

if runPF2PAT:

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

if useStandardPAT:
  process.patAddOnSequence = cms.Sequence(
    process.intermediatePatMuons
  * process.goodPatJets
  * process.loosePatMuons
  * process.tightPatMuons
  )
if runPF2PAT:
  patAddOnSequence = cms.Sequence(
    getattr( process, 'intermediatePatMuons' + postfix )
  * getattr( process, 'goodPatJets' + postfix )
  * getattr( process, 'loosePatMuons' + postfix )
  * getattr( process, 'tightPatMuons' + postfix )
  )
  setattr( process, 'patAddOnSequence' + postfix, patAddOnSequence )

# The paths
if useStandardPAT:
  process.p = cms.Path()
  if not runOnMC:
    process.p += process.eventCleaning
  if useTrigger:
    process.p += process.step1
  process.p += process.goodOfflinePrimaryVertices
  if useGoodVertex:
    process.p += process.step2
  process.p += process.patDefaultSequence
  process.p += process.patAddOnSequence
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
  process.out.SelectEvents.SelectEvents.append( 'p' )

if runPF2PAT:
  pPF = cms.Path()
  if not runOnMC:
    pPF += process.eventCleaning
  if useTrigger:
    pPF += process.step1
  pPF += process.goodOfflinePrimaryVertices
  if useGoodVertex:
    pPF += process.step2
  pPF += getattr( process, 'patPF2PATSequence' + postfix )
  pPF += getattr( process, 'patAddOnSequence' + postfix )
  if useLooseMuon:
    pPF += getattr( process, 'step3b' + postfix )
  if useTightMuon:
    pPF += getattr( process, 'step3a' + postfix )
  if useMuonVeto:
    pPF += getattr( process, 'step4' + postfix )
  if useElectronVeto:
    pPF += getattr( process, 'step5' + postfix )
  if use1Jet:
    pPF += getattr( process, 'step6a' + postfix )
  if use2Jets:
    pPF += getattr( process, 'step6b' + postfix )
  if use3Jets:
    pPF += getattr( process, 'step6c' + postfix )
  if use4Jets:
    pPF += getattr( process, 'step7' + postfix )
  setattr( process, 'p' + postfix, pPF )
  process.out.SelectEvents.SelectEvents.append( 'p' + postfix )


###
### Trigger matching
###

if addTriggerMatching:
  if runOnMC:
    triggerObjectSelection = triggerObjectSelectionMC
  else:
    triggerObjectSelection = triggerObjectSelectionData
  ### Trigger matching configuration
  from PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cfi import patTrigger
  from TopQuarkAnalysis.Configuration.patRefSel_triggerMatching_cfi import patMuonTriggerMatch
  from PhysicsTools.PatAlgos.tools.trigTools import *
  if useStandardPAT:
    triggerProducer = patTrigger.clone()
    setattr( process, 'patTrigger', triggerProducer )
    process.triggerMatch = patMuonTriggerMatch.clone( matchedCuts = triggerObjectSelection )
    switchOnTriggerMatchEmbedding( process
                                 , triggerMatchers = [ 'triggerMatch' ]
                                 , hltProcess      = '*'
                                 )
    removeCleaningFromTriggerMatching( process )
    process.intermediatePatMuons.src = cms.InputTag( 'selectedPatMuonsTriggerMatch' )
  if runPF2PAT:
    triggerProducerPF = patTrigger.clone()
    setattr( process, 'patTrigger' + postfix, triggerProducerPF )
    triggerMatchPF = patMuonTriggerMatch.clone( matchedCuts = triggerObjectSelection )
    setattr( process, 'triggerMatch' + postfix, triggerMatchPF )
    switchOnTriggerMatchEmbedding( process
                                 , triggerProducer = 'patTrigger' + postfix
                                 , triggerMatchers = [ 'triggerMatch' + postfix ]
                                 , sequence        = 'patPF2PATSequence' + postfix
                                 , postfix         = postfix
                                 , hltProcess      = '*'
                                 )
    removeCleaningFromTriggerMatching( process
                                     , sequence = 'patPF2PATSequence' + postfix
                                     )
    getattr( process, 'intermediatePatMuons' + postfix ).src = cms.InputTag( 'selectedPatMuons' + postfix + 'TriggerMatch' )
