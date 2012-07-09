import FWCore.ParameterSet.Config as cms


### Steering

runOnMC        = False
runMatch       = False
runGenJetMatch = False # separate from rest of matches due to rapidly inceasing data volume
runCiC         = True
runEwk         = True
addGenEvt      = False

hltProcess       = 'HLT'
triggerSelection = ''

jetAlgo   = 'AK5'
jecLevels = []
#jecLevels = [ 'L1FastJet', 'L2Relative', 'L3Absolute' ]
#if not runOnMC:
  #jecLevels.append( 'L2L3Residual' )

# vertex collection to use
# 'offlinePrimaryVertices' or 'goodOfflinePrimaryVertices'
pvCollection = 'goodOfflinePrimaryVertices' # recommended: 'goodOfflinePrimaryVertices' (s. https://hypernews.cern.ch/HyperNews/CMS/get/top-selection/38/1/1/1/2/1/1/2/1/3/1.html)

# muon object selection
#muonSelect = 'isGlobalMuon && pt > 10. && abs(eta) < 2.5' # RefSel (min. for veto)
muonSelect = ''
# muon event selection
muonCut  = 'isGlobalMuon && pt > 5. && abs(eta) < 3.0'
muonsMin = 0

# electron object selection
#electronSelect = 'et > 15. && abs(eta) < 2.5' # RefSel (min. for veto)
electronSelect = ''
# electron event selection
electronCut  = 'et > 5. && abs(eta) < 3.0'
electronsMin = 0

# x-leptons event selection
leptonsMin = 1

# jet object selection
#jetSelect = 'pt > 30. && abs(eta) < 2.4' # RefSel
jetSelect = ''
# jet event selection
jetCut  = 'pt > 15. && abs(eta) < 3.0'
jetsMin = 3


### Initialization

process = cms.Process( 'PAT' )

runMatch       = runMatch       and runOnMC
runGenJetMatch = runGenJetMatch and runOnMC
addGenEvt      = addGenEvt      and runOnMC

### Logging

process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool( True )
)
process.Timing = cms.Service( "Timing"
, summaryOnly = cms.untracked.bool( True )
)


### Conditions

process.load( "Configuration.StandardSequences.Geometry_cff" )
process.load( "Configuration.StandardSequences.MagneticField_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
if runOnMC:
  process.GlobalTag.globaltag = 'START42_V17::All'
else:
  process.GlobalTag.globaltag = 'GR_R_42_V23::All'


### Input

process.source = cms.Source( "PoolSource"
, noEventSort        = cms.untracked.bool( True )
, duplicateCheckMode = cms.untracked.string( 'noDuplicateCheck' )
, fileNames          = cms.untracked.vstring()
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 1000 )
)

from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
if runOnMC:
  #process.source.fileNames = pickRelValInputFiles( cmsswVersion  = 'CMSSW_4_2_6'
                                                 #, globalTag     = 'START42_V12'
                                                 #)
  process.source.fileNames = []
else:
  #process.source.fileNames = pickRelValInputFiles( cmsswVersion  = 'CMSSW_4_2_6'
                                                 #, dataTier      = 'RECO'
                                                 #, relVal        = 'Mu'
                                                 #, globalTag     = 'GR_R_42_V14_wzMu2010B'
                                                 ##, relVal        = 'Electron'
                                                 ##, globalTag     = 'GR_R_42_V14_wzEG2010B'
                                                 #)
  process.source.fileNames = [
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/F20DF449-B402-E111-9804-BCAEC5364C4B.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/EE98420A-BA02-E111-81EF-BCAEC518FF69.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/CEA605D0-D402-E111-86C0-BCAEC518FF54.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/CCF275FD-D302-E111-8666-0025901D5D7E.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/C664F0FF-4403-E111-9892-0019B9F581C9.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/BCA44733-AD02-E111-851A-BCAEC5329728.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/BAD88163-B602-E111-B62A-BCAEC5364C6C.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/BACB95AA-D102-E111-A597-0025901D624E.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/B2FFB7F4-C002-E111-9908-BCAEC518FF6B.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/B2D7CDFE-CD02-E111-A6E7-BCAEC5364C6C.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/A61DD0FD-D302-E111-88DC-BCAEC53296FB.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/9679DA0F-BC02-E111-9D34-485B3977172C.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/866A05DA-DB02-E111-A1B5-003048D3C982.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/723ABEB7-DE02-E111-9FEA-485B3962633D.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/6ECCE52B-EC02-E111-8652-003048F118C4.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/6ECA888F-D802-E111-92E2-BCAEC5364C4C.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/6E4C8ED1-D202-E111-84A4-BCAEC5329703.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/68C57433-DB02-E111-B214-003048D2BEA8.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/5E83A11B-E002-E111-92DC-003048F117EC.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/4E47A6DC-C502-E111-82EA-003048D37524.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/4C1FFB39-C002-E111-A228-BCAEC5329705.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/44065824-C702-E111-9A7C-BCAEC532972D.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/2410C7BD-E302-E111-9F81-003048F118AA.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/1A5F9088-BF02-E111-9D22-BCAEC54DB5D6.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/14913DB5-D702-E111-84D8-BCAEC5329708.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/0C6454BD-E302-E111-8368-003048F11112.root',
       '/store/data/Run2011B/SingleMu/AOD/PromptReco-v1/000/180/076/02D447C2-B702-E111-B8D7-0025901D5C86.root'
  ]


### Output

from PhysicsTools.PatAlgos.patEventContent_cff import patEventContentNoCleaning
process.out = cms.OutputModule( "PoolOutputModule"
, fileName       = cms.untracked.string( 'gentPatSkim.root' )
, SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring( 'p' ) )
, outputCommands = cms.untracked.vstring( 'drop *', *patEventContentNoCleaning )
, dropMetaData   = cms.untracked.string( 'ALL' )
)

# Outpath
process.outpath = cms.EndPath(
  process.out
)


### Cleaning

# HBHE noise filter
process.load( "CommonTools.RecoAlgos.HBHENoiseFilter_cfi" )
process.HBHENoiseFilter.minIsolatedNoiseSumE        = 999999.
process.HBHENoiseFilter.minNumIsolatedNoiseChannels = 999999
process.HBHENoiseFilter.minIsolatedNoiseSumEt       = 999999.

# Scraping filter
process.scrapingFilter = cms.EDFilter( "FilterOutScraping"
, applyfilter = cms.untracked.bool( True )
, debugOn     = cms.untracked.bool( False )
, numtrack    = cms.untracked.uint32( 10 )
, thresh      = cms.untracked.double( 0.25 )
)

# Trigger
process.load( "HLTrigger.HLTfilters.triggerResultsFilter_cfi" )
process.triggerResultsFilter.hltResults        = cms.InputTag( 'TriggerResults::' + hltProcess )
process.triggerResultsFilter.l1tResults        = cms.InputTag( '' )
process.triggerResultsFilter.triggerConditions = [ triggerSelection ]
process.triggerResultsFilter.throw             = False

# Vertices
process.goodOfflinePrimaryVertices = cms.EDFilter(
  "PrimaryVertexObjectFilter"
, src          = cms.InputTag( 'offlinePrimaryVertices' )
, filter       = cms.bool( True )
, filterParams = cms.PSet( minNdof = cms.double(  4. )
                         , maxZ    = cms.double( 24. )
                         , maxRho  = cms.double(  2. )
                         )
)

process.eventCleaning = cms.Sequence(
  process.HBHENoiseFilter
+ process.scrapingFilter
)
if triggerSelection != '':
  process.eventCleaning += process.triggerResultsFilter
process.eventCleaning += process.goodOfflinePrimaryVertices


### PAT

process.load( "PhysicsTools.PatAlgos.patSequences_cff" )

# Misc
from PhysicsTools.PatAlgos.tools.coreTools import *
removeSpecificPATObjects( process
                        , names         = [ 'Photons', 'Taus' ]
                        , outputModules = []
                        )
# The following is not performed (properly) by 'removeSpecificPATObjects()'
process.patDefaultSequence.remove( getattr( process, 'cleanPatPhotons' ) )
process.cleanPatCandidateSummary.candidates.remove( cms.InputTag( 'cleanPatPhotons' ) )
process.patDefaultSequence.remove( getattr( process, 'cleanPatTaus' ) )
process.cleanPatCandidateSummary.candidates.remove( cms.InputTag( 'cleanPatTaus' ) )
if not runOnMC:
  runOnData( process )
if not runMatch:
  process.patMuons.addGenMatch = False
  process.patDefaultSequence.remove( process.muonMatch )
  process.patElectrons.addGenMatch = False
  process.patDefaultSequence.remove( process.electronMatch )
  process.patJets.addGenPartonMatch   = False
  process.patJets.embedGenPartonMatch = False
  process.patJets.genPartonMatch      = cms.InputTag( '' )
  process.patDefaultSequence.remove( process.patJetPartonMatch )
  process.patJets.getJetMCFlavour    = False
  process.patJets.JetPartonMapSource = cms.InputTag( '' )
  process.patDefaultSequence.remove( process.patJetFlavourId )
if not runGenJetMatch:
  process.patJets.addGenJetMatch = False
  process.patJets.genJetMatch    = cms.InputTag( '' )
  process.patDefaultSequence.remove( process.patJetGenJetMatch )
process.out.outputCommands += [ 'drop recoGenJets_*_*_*'
                              , 'drop recoBaseTagInfosOwned_*_*_*'
                              , 'drop CaloTowers_*_*_*'
                              , 'keep edmTriggerResults_*_*_*'
                              , 'drop edmTriggerResults_*_*_*RECO*'
                              , 'drop edmTriggerResults_*_*_*NONE*'
                              , 'keep *_hltTriggerSummaryAOD_*_*'
                              , 'keep *_offlineBeamSpot_*_*'
                              , 'keep *_goodOfflinePrimaryVertices_*_*'
                              # for conversion rejection
                              , 'keep recoTracks_generalTracks_*_*'
                              , 'keep recoGsfTracks_electronGsfTracks_*_*'
                              ]
if runGenJetMatch:
  process.out.outputCommands += [ 'keep recoGenJets_ak5GenJets_*_*'
                                ]
if runOnMC:
  process.out.outputCommands += [ 'keep *_addPileupInfo_*_*'
                                ]
  if not runMatch or not runGenJetMatch:
    process.out.outputCommands += [ 'keep recoGenParticles_*_*_*'
                                  ]

# Vertices
pvCollection += '::%s'%( process.name_() )
process.patElectrons.pvSrc                = cms.InputTag( pvCollection )
process.patMuons.pvSrc                    = cms.InputTag( pvCollection )
process.patJetCorrFactors.primaryVertices = cms.InputTag( pvCollection )

# Muons
process.patMuons.embedTrack = True
process.selectedPatMuons.cut = muonSelect
process.cleanPatMuons.src           = cms.InputTag( 'patMuons' )
process.cleanPatMuons.preselection  = muonCut
process.cleanPatMuons.checkOverlaps = cms.PSet()
process.countPatMuons.minNumber = muonsMin

# Electrons
process.patElectrons.embedTrack = True
process.selectedPatElectrons.cut = electronSelect
process.cleanPatElectrons.src           = cms.InputTag( 'patElectrons' )
process.cleanPatElectrons.preselection  = electronCut
process.cleanPatElectrons.checkOverlaps = cms.PSet()
process.countPatElectrons.minNumber = electronsMin
if runEwk:
  process.load( "ElectroWeakAnalysis.WENu.simpleEleIdSequence_cff" )
  process.patDefaultSequence.replace( process.patElectrons
                                   , process.simpleEleIdSequence * process.patElectrons
                                   )
  process.patElectrons.electronIDSources.simpleEleId95relIso = cms.InputTag( 'simpleEleId95relIso' )
  process.patElectrons.electronIDSources.simpleEleId90relIso = cms.InputTag( 'simpleEleId90relIso' )
  process.patElectrons.electronIDSources.simpleEleId85relIso = cms.InputTag( 'simpleEleId85relIso' )
  process.patElectrons.electronIDSources.simpleEleId80relIso = cms.InputTag( 'simpleEleId80relIso' )
  process.patElectrons.electronIDSources.simpleEleId70relIso = cms.InputTag( 'simpleEleId70relIso' )
  process.patElectrons.electronIDSources.simpleEleId60relIso = cms.InputTag( 'simpleEleId60relIso' )
  process.patElectrons.electronIDSources.simpleEleId95cIso   = cms.InputTag( 'simpleEleId95cIso' )
  process.patElectrons.electronIDSources.simpleEleId90cIso   = cms.InputTag( 'simpleEleId90cIso' )
  process.patElectrons.electronIDSources.simpleEleId85cIso   = cms.InputTag( 'simpleEleId85cIso' )
  process.patElectrons.electronIDSources.simpleEleId80cIso   = cms.InputTag( 'simpleEleId80cIso' )
  process.patElectrons.electronIDSources.simpleEleId70cIso   = cms.InputTag( 'simpleEleId70cIso' )
  process.patElectrons.electronIDSources.simpleEleId60cIso   = cms.InputTag( 'simpleEleId60cIso' )
if runCiC:
  process.load( "RecoEgamma.ElectronIdentification.cutsInCategoriesElectronIdentificationV06_cfi" )
  process.eidCiCSequence = cms.Sequence(
    process.eidVeryLooseMC
  + process.eidLooseMC
  + process.eidMediumMC
  + process.eidTightMC
  + process.eidSuperTightMC
  + process.eidHyperTight1MC
  + process.eidHyperTight2MC
  + process.eidHyperTight3MC
  + process.eidHyperTight4MC
  )
  process.patDefaultSequence.replace( process.patElectrons
                                   , process.eidCiCSequence * process.patElectrons
                                   )
  process.patElectrons.electronIDSources.eidVeryLooseMC   = cms.InputTag( 'eidVeryLooseMC' )
  process.patElectrons.electronIDSources.eidLooseMC       = cms.InputTag( 'eidLooseMC' )
  process.patElectrons.electronIDSources.eidMediumMC      = cms.InputTag( 'eidMediumMC' )
  process.patElectrons.electronIDSources.eidTightMC       = cms.InputTag( 'eidTightMC' )
  process.patElectrons.electronIDSources.eidSuperTightMC  = cms.InputTag( 'eidSuperTightMC' )
  process.patElectrons.electronIDSources.eidHyperTight1MC = cms.InputTag( 'eidHyperTight1MC' )
  process.patElectrons.electronIDSources.eidHyperTight2MC = cms.InputTag( 'eidHyperTight2MC' )
  process.patElectrons.electronIDSources.eidHyperTight3MC = cms.InputTag( 'eidHyperTight3MC' )
  process.patElectrons.electronIDSources.eidHyperTight4MC = cms.InputTag( 'eidHyperTight4MC' )

# X-leptons
process.countPatLeptons.minNumber = leptonsMin

# Jets
if len( jecLevels ) is 0:
  process.patJets.addJetCorrFactors = False
  print 'WARNING: No JECs are stored or applied!'
from PhysicsTools.PatAlgos.tools.jetTools import switchJetCollection
switchJetCollection( process
                   , cms.InputTag( jetAlgo.lower() + 'PFJets::' + process.name_() )
                   , doJTA            = False
                   , doBTagging       = False
                   , jetCorrLabel     = ( jetAlgo + 'PF', jecLevels )
                   , doType1MET       = False
                   , genJetCollection = cms.InputTag( jetAlgo.lower() + 'GenJets' )
                   , doJetID          = True
                   )
from RecoJets.Configuration.RecoPFJets_cff import ak5PFJets
process.ak5PFJets = ak5PFJets.clone( doAreaFastjet = True )
from RecoJets.Configuration.RecoPFJets_cff import kt6PFJets
process.kt6PFJets = kt6PFJets.clone( doAreaFastjet = True
                                   , doRhoFastjet  = True
                                   , voronoiRfact  = -0.9
                                   ) # to ensure not to use the Voronoi tessalation for the moment (s. https://hypernews.cern.ch/HyperNews/CMS/get/JetMET/1215.html)
process.patDefaultSequence.replace( process.patJetCorrFactors
                                  , process.kt6PFJets * process.patJetCorrFactors
                                  )
process.out.outputCommands += [ 'keep *_kt6PFJets_rho_' + process.name_() ]
process.patJetCorrFactors.payload = jetAlgo + 'PF' # needs to be fixed _after_ the (potential) calls to 'removeSpecificPATObjects()' and 'runOnData()'
process.patJetCorrFactors.levels  = jecLevels      # needs to be fixed _after_ the (potential) calls to 'removeSpecificPATObjects()' and 'runOnData()'
process.patJets.embedCaloTowers   = False
process.patJets.embedPFCandidates = False
process.selectedPatJets.cut = jetSelect
process.cleanPatJets.src           = cms.InputTag( 'patJets' )
process.cleanPatJets.preselection  = jetCut
process.cleanPatJets.checkOverlaps = cms.PSet()
process.countPatJets.minNumber = jetsMin

# MET
from PhysicsTools.PatAlgos.tools.metTools import addPfMET
addPfMET( process
        , jetAlgo + 'PF'
        )
process.patDefaultSequence.remove( getattr( process, 'metJESCorAK5CaloJet' ) )
process.patDefaultSequence.remove( getattr( process, 'metJESCorAK5CaloJetMuons' ) )
process.patDefaultSequence.remove( getattr( process, 'patMETs' ) )


### TQAF

if addGenEvt:
  process.load( "TopQuarkAnalysis.TopEventProducers.sequences.ttGenEvent_cff" )
  process.out.outputCommands += [ 'keep *_genParticles_*_*'
                                , 'keep *_genEvt_*_*'
                                , 'keep *_initSubset_*_*'
                                , 'keep *_decaySubset_*_*'
                                ]


### Path
process.p = cms.Path(
  process.eventCleaning
* process.ak5PFJets
* process.patDefaultSequence
)
if addGenEvt:
  process.p *= process.makeGenEvt
