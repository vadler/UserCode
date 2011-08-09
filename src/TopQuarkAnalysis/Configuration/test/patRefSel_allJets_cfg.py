import sys

import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

# setup 'standard' options
options = VarParsing.VarParsing ('standard')
options.register('runOnMC', True, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.bool, "decide if run on MC or data")

# parsing command line arguments
if( hasattr(sys, "argv") ):
  #options.parseArguments()
  if(len(sys.argv) > 1):
    print "Parsing command line arguments:"
  for args in sys.argv :
    arg = args.split(',')
    for val in arg:
      val = val.split('=')
      if(len(val)==2):
        print "Setting *", val[0], "* to:", val[1]
        setattr(options,val[0], val[1])

process = cms.Process( 'PAT' )


### ======================================================================== ###
###                                                                          ###
###                                 Constants                                ###
###                            (user job steering)                           ###
###                                                                          ###
### ======================================================================== ###


### Data or MC?
runOnMC = options.runOnMC

### Standard and PF reconstruction
useStandardPAT = True
runPF2PAT      = True

### Switch on/off selection steps

# Step 1
# (trigger selection: QuadJet50_Jet40)
useTrigger      = True
# Step 2
# (good vertex selection)
useGoodVertex   = True
# Step 3a
# (6 jets: pt > 30 GeV & |eta| < 2.4)
use6JetsLoose   = True
# Step 3b
# (4 jets: pt > 60 GeV, 5 jets: pt > 50 GeV, 6 jets: pt > 30 GeV, for all jets: |eta| < 2.4)
# (the cuts for the 4 and 5 leading jets are configurable via jetCutHard / jetCutMedium respectivly)
use6JetsTight   = False

addTriggerMatching = True

### Reference selection

from TopQuarkAnalysis.Configuration.patRefSel_refAllJets import *
#muonsUsePV             = False
#muonEmbedTrack         = True
#muonCutPF              = ''
#looseMuonCutPF         = ''
#tightMuonCutPF         = ''
#muonJetsDR             = 0.3
#jetCutPF               = ''
#jetMuonsDRPF           = 0.1
#electronCutPF          = ''
#jetCutMedium           = ''
#jetCutHard             = ''

# Trigger selection according to run range resp. MC sample:
# lower range limits for data available as suffix;
# available are: 160404 (default)

# sample name for MC available as suffix;
# available are: Summer11 (default)
#triggerSelectionData       = triggerSelection_160404
#triggerObjectSelectionData = triggerObjectSelection_160404
#triggerSelectionMC         = triggerSelection_Summer11
#triggerObjectSelectionMC   = triggerObjectSelection_Summer11

### Particle flow
### takes effect only, if 'runPF2PAT' = True

postfix = 'PF' # needs to be a non-empty string and must not be 'AK5PF', if 'useStandardPAT' = True

# subtract charged hadronic pile-up particles (from wrong PVs)
# effects also JECs
usePFnoPU = True # before any top projection

# other switches for PF top projections (default: all 'True')
useNoMuon     = True # before electron top projection
useNoElectron = True # before jet top projection
useNoJet      = True # before tau top projection
useNoTau      = True # before MET top projection

# cuts used in top projections
from TopQuarkAnalysis.Configuration.patRefSel_PF2PAT import *
# vertices
#pfD0Cut   = 0.2
#pfDzCut   = 0.5
#pfVertices = 'offlinePrimaryVertices'
# muons
#pfMuonSelectionCut = ''
#pfMuonIsoConeR   = 0.4
#pfMuonCombIsoCut = 0.15
# electrons
#pfElectronSelectionCut  = ''
#pfElectronIsoConeR   = 0.4
#pfElectronCombIsoCut = 0.2

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
useRelVals = False # if 'False', "inputFiles" is used
inputFiles = [ '/store/relval/CMSSW_4_2_3/RelValTTbar/GEN-SIM-DIGI-RECO/START42_V12_FastSim_PU_156BxLumiPileUp-v1/0072/0635AA67-B37C-E011-B61F-002618943944.root'
             , '/store/relval/CMSSW_4_2_3/RelValTTbar/GEN-SIM-DIGI-RECO/START42_V12_FastSim_PU_156BxLumiPileUp-v1/0072/0E153885-B17C-E011-8C7D-001A928116E0.root'
             , '/store/relval/CMSSW_4_2_3/RelValTTbar/GEN-SIM-DIGI-RECO/START42_V12_FastSim_PU_156BxLumiPileUp-v1/0072/105E01FE-B57C-E011-9AB4-0018F3D09708.root'
             , '/store/relval/CMSSW_4_2_3/RelValTTbar/GEN-SIM-DIGI-RECO/START42_V12_FastSim_PU_156BxLumiPileUp-v1/0072/120718C8-B67C-E011-A070-001A928116D2.root'
             , '/store/relval/CMSSW_4_2_3/RelValTTbar/GEN-SIM-DIGI-RECO/START42_V12_FastSim_PU_156BxLumiPileUp-v1/0072/1232DFFA-AF7C-E011-983D-002618943831.root'
             ]   # overwritten, if "useRelVals" is 'True'

             #['/store/mc/Summer11/QCD_Pt-15to3000_TuneD6T_Flat_7TeV_pythia6/AODSIM/PU_S3_START42_V11-v2/0000/023735A8-297E-E011-A8B0-0026189438AD.root',
             # '/store/mc/Summer11/QCD_Pt-15to3000_TuneD6T_Flat_7TeV_pythia6/AODSIM/PU_S3_START42_V11-v2/0000/56FA5813-387E-E011-B48D-001A928116D6.root',
             # '/store/mc/Summer11/QCD_Pt-15to3000_TuneD6T_Flat_7TeV_pythia6/AODSIM/PU_S3_START42_V11-v2/0000/84488536-2B7E-E011-989D-001A92810A94.root'
             # ]   # overwritten, if "useRelVals" is 'True'


# maximum number of events
maxInputEvents = -1 # reduce for testing

### Conditions

# GlobalTags (w/o suffix '::All')
globalTagData = 'GR_R_42_V12' # default for CMSSW_4_2_3 RelVals: 'GR_R_42_V12'
globalTagMC   = 'START42_V12' # default for CMSSW_4_2_3 RelVals: 'START42_V12'

### Output

# output file
outputFile = 'patRefSel_fullHad.root'

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
                                     , globalTag     = globalTagData + '_mu2010B' # wrong naming scheme in CMSSW_4_2_3
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
# clear event selection
process.out.SelectEvents.SelectEvents = []


###
### Cleaning and trigger selection configuration
###

### Event cleaning
process.load( 'TopQuarkAnalysis.Configuration.patRefSel_eventCleaning_cff' )

### Trigger selection
if runOnMC:
  triggerSelection = triggerSelectionMC
else:
  triggerSelection = triggerSelectionData
from TopQuarkAnalysis.Configuration.patRefSel_triggerSelection_cff import triggerResults
process.step1 = triggerResults.clone(
  triggerConditions = [ triggerSelection ]
)

### Good vertex selection
process.load( "TopQuarkAnalysis.Configuration.patRefSel_goodVertex_cfi" )
process.step2 = process.goodOfflinePrimaryVertexFilter.clone()


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
           , runPF2PAT      = runPF2PAT
           , runOnMC        = runOnMC
           , jetAlgo        = jetAlgo
           , postfix        = postfix
           , jetCorrections = ( jecSetPF
                              , jecLevels
                              )
           )
  applyPostfix( process, 'pfNoPileUp'  , postfix ).enable = usePFnoPU
  applyPostfix( process, 'pfNoMuon'    , postfix ).enable = useNoMuon
  applyPostfix( process, 'pfNoElectron', postfix ).enable = useNoElectron
  applyPostfix( process, 'pfNoJet'     , postfix ).enable = useNoJet
  applyPostfix( process, 'pfNoTau'     , postfix ).enable = useNoTau
  if useL1FastJet:
    applyPostfix( process, 'pfJets', postfix ).doAreaFastjet = True
    applyPostfix( process, 'pfJets', postfix ).doRhoFastjet  = False
    applyPostfix( process, 'pfPileUp', postfix ).checkClosestZVertex = False
    applyPostfix( process, 'pfPileUp', postfix ).Vertices = cms.InputTag( pfVertices )
  applyPostfix( process, 'pfMuonsFromVertex'    , postfix ).vertices = cms.InputTag( pfVertices )
  applyPostfix( process, 'pfMuonsFromVertex'    , postfix ).d0Cut    = pfD0Cut
  applyPostfix( process, 'pfMuonsFromVertex'    , postfix ).dzCut    = pfDzCut
  applyPostfix( process, 'pfSelectedMuons'      , postfix ).cut = pfMuonSelectionCut
  applyPostfix( process, 'isoValMuonWithCharged', postfix ).deposits[0].deltaR = pfMuonIsoConeR
  applyPostfix( process, 'isoValMuonWithNeutral', postfix ).deposits[0].deltaR = pfMuonIsoConeR
  applyPostfix( process, 'isoValMuonWithPhotons', postfix ).deposits[0].deltaR = pfMuonIsoConeR
  applyPostfix( process, 'pfIsolatedMuons'      , postfix ).combinedIsolationCut = pfMuonCombIsoCut
  applyPostfix( process, 'pfElectronsFromVertex'    , postfix ).vertices = cms.InputTag( pfVertices )
  applyPostfix( process, 'pfElectronsFromVertex'    , postfix ).d0Cut    = pfD0Cut
  applyPostfix( process, 'pfElectronsFromVertex'    , postfix ).dzCut    = pfDzCut
  applyPostfix( process, 'pfSelectedElectrons'      , postfix ).cut = pfElectronSelectionCut
  applyPostfix( process, 'isoValElectronWithCharged', postfix ).deposits[0].deltaR = pfElectronIsoConeR
  applyPostfix( process, 'isoValElectronWithNeutral', postfix ).deposits[0].deltaR = pfElectronIsoConeR
  applyPostfix( process, 'isoValElectronWithPhotons', postfix ).deposits[0].deltaR = pfElectronIsoConeR
  applyPostfix( process, 'pfIsolatedElectrons'      , postfix ).combinedIsolationCut = pfElectronCombIsoCut

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
  from PhysicsTools.PatAlgos.tools.jetTools import *
  jecSetPFNoCHS = jecSetPF.rstrip('chs')
  addJetCollection(process,cms.InputTag('ak5PFJets'),'AK5','PF',
                   doJTA        = True,
                   doBTagging   = True,
                   jetCorrLabel = (jecSetPFNoCHS, jecLevels),
                   doType1MET   = False,
                   doL1Cleaning = False,
                   doL1Counters = True,
                   genJetCollection=cms.InputTag('ak5GenJets'),
                   doJetID      = True,
                   )
  from PhysicsTools.PatAlgos.tools.metTools import *
  addPfMET(process, 'AK5PF')

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

from TopQuarkAnalysis.Configuration.patRefSel_refAllJets_cfi import *

if useStandardPAT:

  process.intermediatePatMuons = intermediatePatMuons.clone()
  process.loosePatMuons        = loosePatMuons.clone()
  process.tightPatMuons        = tightPatMuons.clone()
  process.step3a               = step3a.clone()

  process.step3b_1 = step3b_1.clone()
  process.step3b_2 = step3b_2.clone()
  process.step3b_3 = step3b_3.clone()
  process.step3b   = cms.Sequence( process.step3b_1 * process.step3b_2 * process.step3b_3 )

  process.kt6PFJets = kt6PFJets.clone( doRhoFastjet = True )
  process.patDefaultSequence.replace( process.patJetCorrFactors
                                    , process.kt6PFJets * process.patJetCorrFactors
                                    )
  process.out.outputCommands.append( 'keep double_*_*_' + process.name_() )

  process.goodPatJets       = goodPatJets.clone()
  process.goodPatJetsMedium = process.goodPatJets.clone()
  process.goodPatJetsHard   = process.goodPatJets.clone()
  process.goodPatJetsAK5PF       = goodPatJets.clone()
  process.goodPatJetsMediumAK5PF = process.goodPatJets.clone()
  process.goodPatJetsHardAK5PF   = process.goodPatJets.clone()

if runPF2PAT:

  ### Muons

  intermediatePatMuonsPF = intermediatePatMuons.clone( src = cms.InputTag( 'selectedPatMuons' + postfix ) )
  setattr( process, 'intermediatePatMuons' + postfix, intermediatePatMuonsPF )

  loosePatMuonsPF = loosePatMuons.clone( src = cms.InputTag( 'intermediatePatMuons' + postfix ) )
  setattr( process, 'loosePatMuons' + postfix, loosePatMuonsPF )
  getattr( process, 'loosePatMuons' + postfix ).checkOverlaps.jets.src = cms.InputTag( 'goodPatJets' + postfix )

  tightPatMuonsPF = tightPatMuons.clone( src = cms.InputTag( 'loosePatMuons' + postfix ) )
  setattr( process, 'tightPatMuons' + postfix, tightPatMuonsPF )

  ### Jets

  applyPostfix( process, 'patJetCorrFactors', postfix ).primaryVertices = cms.InputTag( 'goodOfflinePrimaryVertices' )
  if usePFnoPU:
    kt6PFJetsPFChs = kt6PFJetsChs.clone( src = cms.InputTag( 'pfNoElectron' + postfix ) )
    setattr( process, 'kt6PFJetsChs' + postfix, kt6PFJetsPFChs )
    getattr( process, 'patPF2PATSequence' + postfix).replace( getattr( process, 'patJetCorrFactors' + postfix )
                                                            , getattr( process, 'kt6PFJetsChs' + postfix ) * getattr( process, 'patJetCorrFactors' + postfix )
                                                            )
    if useL1FastJet:
      applyPostfix( process, 'patJetCorrFactors', postfix ).rho = cms.InputTag( 'kt6PFJetsChs' + postfix, 'rho' )
  else:
    kt6PFJetsPF = kt6PFJets.clone( doRhoFastjet = True )
    setattr( process, 'kt6PFJets' + postfix, kt6PFJetsPF )
    getattr( process, 'patPF2PATSequence' + postfix).replace( getattr( process, 'patJetCorrFactors' + postfix )
                                                            , getattr( process, 'kt6PFJets' + postfix ) * getattr( process, 'patJetCorrFactors' + postfix )
                                                            )
    if useL1FastJet:
      applyPostfix( process, 'patJetCorrFactors', postfix ).rho = cms.InputTag( 'kt6PFJets' + postfix, 'rho' )
  process.out.outputCommands.append( 'keep double_*' + postfix + '*_*_' + process.name_() )

  goodPatJetsPF = goodPatJets.clone( src = cms.InputTag( 'selectedPatJets' + postfix ) )
  setattr( process, 'goodPatJets' + postfix, goodPatJetsPF )
  getattr( process, 'goodPatJets' + postfix ).checkOverlaps.muons.src = cms.InputTag( 'intermediatePatMuons' + postfix )

  goodPatJetsMediumPF = getattr( process, 'goodPatJets' + postfix ).clone()
  setattr( process, 'goodPatJetsMedium' + postfix, goodPatJetsMediumPF )
  goodPatJetsHardPF = getattr( process, 'goodPatJets' + postfix ).clone()
  setattr( process, 'goodPatJetsHard' + postfix, goodPatJetsHardPF )

  step3aPF = step3a.clone( src = cms.InputTag( 'goodPatJets' + postfix ) )
  setattr( process, 'step3a' + postfix, step3aPF )

  step3b_1PF = step3b_1.clone()
  setattr( process, 'step3b_1' + postfix, step3b_1PF )
  step3b_2PF = step3b_2.clone()
  setattr( process, 'step3b_2' + postfix, step3b_2PF )
  step3b_3PF = step3b_3.clone()
  setattr( process, 'step3b_3' + postfix, step3b_3PF )
  step3bPF = cms.Sequence( step3b_1PF * step3b_2PF * step3b_3PF )
  setattr( process, 'step3b' + postfix, step3bPF )

  ### Electrons


# keep produced collections in the PAT tuple
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

  process.goodPatJets.preselection       = jetCut
  process.goodPatJetsMedium.preselection = jetCut + jetCutMedium
  process.goodPatJetsHard.preselection   = jetCut + jetCutHard

  process.goodPatJetsAK5PF.src       = 'selectedPatJetsAK5PF'
  process.goodPatJetsMediumAK5PF.src = 'selectedPatJetsAK5PF'
  process.goodPatJetsHardAK5PF.src   = 'selectedPatJetsAK5PF'

  process.goodPatJetsAK5PF.preselection       = jetCutPF
  process.goodPatJetsMediumAK5PF.preselection = jetCutPF + jetCutMedium
  process.goodPatJetsHardAK5PF.preselection   = jetCutPF + jetCutHard

  process.goodPatJetsAK5PF.checkOverlaps.muons.deltaR       = jetMuonsDRPF
  process.goodPatJetsMediumAK5PF.checkOverlaps.muons.deltaR = jetMuonsDRPF
  process.goodPatJetsHardAK5PF.checkOverlaps.muons.deltaR   = jetMuonsDRPF

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

  getattr( process, 'goodPatJets'       + postfix ).preselection               = jetCutPF
  getattr( process, 'goodPatJets'       + postfix ).checkOverlaps.muons.deltaR = jetMuonsDRPF
  getattr( process, 'goodPatJetsMedium' + postfix ).preselection               = jetCutPF + jetCutMedium
  getattr( process, 'goodPatJetsMedium' + postfix ).checkOverlaps.muons.deltaR = jetMuonsDRPF
  getattr( process, 'goodPatJetsHard'   + postfix ).preselection               = jetCutPF + jetCutHard
  getattr( process, 'goodPatJetsHard'   + postfix ).checkOverlaps.muons.deltaR = jetMuonsDRPF

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
  * process.goodPatJetsMedium
  * process.goodPatJetsHard
  * process.goodPatJetsAK5PF
  * process.goodPatJetsMediumAK5PF
  * process.goodPatJetsHardAK5PF
  * process.loosePatMuons
  * process.tightPatMuons
  )
if runPF2PAT:
  patAddOnSequence = cms.Sequence(
    getattr( process, 'intermediatePatMuons' + postfix )
  * getattr( process, 'goodPatJets'          + postfix )
  * getattr( process, 'goodPatJetsMedium'    + postfix )
  * getattr( process, 'goodPatJetsHard'      + postfix )
  * getattr( process, 'loosePatMuons'        + postfix )
  * getattr( process, 'tightPatMuons'        + postfix )
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
  if use6JetsLoose:
    process.p += process.step3a
  if use6JetsTight:
    process.p += process.step3b
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
  if use6JetsLoose:
    pPF += getattr( process, 'step3a' + postfix )
  if use6JetsTight:
    pPF += getattr( process, 'step3b' + postfix )
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
  from TopQuarkAnalysis.Configuration.patRefSel_triggerMatching_cfi import patJetTriggerMatch
  from PhysicsTools.PatAlgos.tools.trigTools import *
  if useStandardPAT:
    triggerProducer = patTrigger.clone()
    setattr( process, 'patTrigger', triggerProducer )
    process.triggerMatch      = patJetTriggerMatch.clone( matchedCuts = triggerObjectSelection )
    process.triggerMatchAK5PF = patJetTriggerMatch.clone( matchedCuts = triggerObjectSelection, src = 'selectedPatJetsAK5PF' )
    switchOnTriggerMatchEmbedding( process
                                 , triggerMatchers = [ 'triggerMatch', 'triggerMatchAK5PF' ]
                                 )
    removeCleaningFromTriggerMatching( process )
    process.goodPatJets.src       = cms.InputTag( 'selectedPatJetsTriggerMatch' )
    process.goodPatJetsMedium.src = cms.InputTag( 'selectedPatJetsTriggerMatch' )
    process.goodPatJetsHard.src   = cms.InputTag( 'selectedPatJetsTriggerMatch' )
    process.goodPatJetsAK5PF.src       = cms.InputTag( 'selectedPatJetsAK5PFTriggerMatch' )
    process.goodPatJetsMediumAK5PF.src = cms.InputTag( 'selectedPatJetsAK5PFTriggerMatch' )
    process.goodPatJetsHardAK5PF.src   = cms.InputTag( 'selectedPatJetsAK5PFTriggerMatch' )
  if runPF2PAT:
    triggerProducerPF = patTrigger.clone()
    setattr( process, 'patTrigger' + postfix, triggerProducerPF )
    triggerMatchPF = patJetTriggerMatch.clone( matchedCuts = triggerObjectSelection )
    setattr( process, 'triggerMatch' + postfix, triggerMatchPF )
    switchOnTriggerMatchEmbedding( process
                                 , triggerProducer = 'patTrigger' + postfix
                                 , triggerMatchers = [ 'triggerMatch' + postfix ]
                                 , sequence        = 'patPF2PATSequence' + postfix
                                 , postfix         = postfix
                                 )
    removeCleaningFromTriggerMatching( process
                                     , sequence = 'patPF2PATSequence' + postfix
                                     )
    getattr( process, 'goodPatJets'       + postfix ).src = cms.InputTag( 'selectedPatJets' + postfix + 'TriggerMatch' )
    getattr( process, 'goodPatJetsMedium' + postfix ).src = cms.InputTag( 'selectedPatJets' + postfix + 'TriggerMatch' )
    getattr( process, 'goodPatJetsHard'   + postfix ).src = cms.InputTag( 'selectedPatJets' + postfix + 'TriggerMatch' )
