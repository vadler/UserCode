import os

import FWCore.ParameterSet.Config as cms

process = cms.Process("TEST")

## add message logger
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.categories.append('KinFitter')
process.MessageLogger.categories.append('TtSemiLepKinFitter')
process.MessageLogger.categories.append('TtSemiLeptonicEvent')
process.MessageLogger.cerr.TtSemiLeptonicEvent = cms.untracked.PSet(
    limit = cms.untracked.int32(-1)
)

## define input
condition = 'mc'
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        pickRelValInputFiles( cmsswVersion = 'CMSSW_3_9_9'
                            , condition    = condition
                            )
    )
)

## define maximal number of events to loop over
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)

## configure process options
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(False)
)

## configure geometry & conditions
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.PyReleaseValidation.autoCond import autoCond
process.GlobalTag.globaltag = autoCond[condition]

## std sequence for PAT
process.load("PhysicsTools.PatAlgos.patSequences_cff")

## std sequence to produce the ttGenEvt
process.load("TopQuarkAnalysis.TopEventProducers.sequences.ttGenEvent_cff")

## modules to produce the ttSemiLepEvent
process.load("TopQuarkAnalysis.TopTools.TtSemiLepJetPartonMatch_cfi")
process.ttSemiLepJetPartonMatch.verbosity  = 1
process.load("TopQuarkAnalysis.TopJetCombination.TtSemiLepHypGenMatch_cfi")
from TopQuarkAnalysis.TopKinFitter.TtSemiLepKinFitProducer_Muons_cfi import kinFitTtSemiLepEvent
process.ttSemiLepKinFit          = kinFitTtSemiLepEvent.clone()
process.ttSemiLepKinFit.maxNComb = -1
process.load("TopQuarkAnalysis.TopJetCombination.TtSemiLepHypKinFit_cfi")
process.ttSemiLepHypKinFit.match       = cms.InputTag("ttSemiLepKinFit")
process.ttSemiLepHypKinFit.status      = cms.InputTag("ttSemiLepKinFit","Status")
process.ttSemiLepHypKinFit.leptons     = cms.InputTag("ttSemiLepKinFit","Leptons")
process.ttSemiLepHypKinFit.neutrinos   = cms.InputTag("ttSemiLepKinFit","Neutrinos")
process.ttSemiLepHypKinFit.partonsHadP = cms.InputTag("ttSemiLepKinFit","PartonsHadP")
process.ttSemiLepHypKinFit.partonsHadQ = cms.InputTag("ttSemiLepKinFit","PartonsHadQ")
process.ttSemiLepHypKinFit.partonsHadB = cms.InputTag("ttSemiLepKinFit","PartonsHadB")
process.ttSemiLepHypKinFit.partonsLepB = cms.InputTag("ttSemiLepKinFit","PartonsLepB")
process.ttSemiLepKinFitGenMatch              = kinFitTtSemiLepEvent.clone()
process.ttSemiLepKinFitGenMatch.match        = 'ttSemiLepJetPartonMatch'
process.ttSemiLepKinFitGenMatch.useOnlyMatch = True
process.ttSemiLepHypKinFitGenMatch             = process.ttSemiLepHypKinFit.clone()
process.ttSemiLepHypKinFitGenMatch.match       = cms.InputTag("ttSemiLepKinFitGenMatch")
process.ttSemiLepHypKinFitGenMatch.status      = cms.InputTag("ttSemiLepKinFitGenMatch","Status")
process.ttSemiLepHypKinFitGenMatch.leptons     = cms.InputTag("ttSemiLepKinFitGenMatch","Leptons")
process.ttSemiLepHypKinFitGenMatch.neutrinos   = cms.InputTag("ttSemiLepKinFitGenMatch","Neutrinos")
process.ttSemiLepHypKinFitGenMatch.partonsHadP = cms.InputTag("ttSemiLepKinFitGenMatch","PartonsHadP")
process.ttSemiLepHypKinFitGenMatch.partonsHadQ = cms.InputTag("ttSemiLepKinFitGenMatch","PartonsHadQ")
process.ttSemiLepHypKinFitGenMatch.partonsHadB = cms.InputTag("ttSemiLepKinFitGenMatch","PartonsHadB")
process.ttSemiLepHypKinFitGenMatch.partonsLepB = cms.InputTag("ttSemiLepKinFitGenMatch","PartonsLepB")
process.load("TopQuarkAnalysis.TopEventProducers.producers.TtSemiLepEvtBuilder_cfi")
process.ttSemiLepEvent.hypotheses  = [ 'ttSemiLepHypGenMatch'
                                     , 'ttSemiLepHypKinFit'
                                     , 'ttSemiLepHypKinFitGenMatch'
                                     ]
process.ttSemiLepEvent.kinFit.chi2 = cms.InputTag("ttSemiLepKinFit","Chi2")
process.ttSemiLepEvent.kinFit.prob = cms.InputTag("ttSemiLepKinFit","Prob")
process.ttSemiLepEvent.verbosity   = 13

## change maximum number of jets taken into account per event (default: 4)
#setForAllTtSemiLepHypotheses(process, "maxNJets", 5)
## solve kinematic equation to determine neutrino pz
#setForAllTtSemiLepHypotheses(process, "neutrinoSolutionType", 2)

## process path
process.p = cms.Path(process.patDefaultSequence *
                     process.makeGenEvt         *
                     process.ttSemiLepJetPartonMatch *
                     process.ttSemiLepHypGenMatch *
                     process.ttSemiLepKinFit *
                     process.ttSemiLepHypKinFit *
                     process.ttSemiLepKinFitGenMatch *
                     process.ttSemiLepHypKinFitGenMatch *
                     process.ttSemiLepEvent
                     )

## configure output module
process.out = cms.OutputModule("PoolOutputModule",
    fileName     = cms.untracked.string('%s/output/ttSemiLepKinFitEvtBuilder.root'%( os.getenv( "CMSSW_BASE" ))),
    SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('p') ),
    outputCommands = cms.untracked.vstring('drop *'),
    dropMetaData = cms.untracked.string('DROPPED')
)
process.outpath = cms.EndPath(process.out)

## PAT content
from PhysicsTools.PatAlgos.patEventContent_cff import *
process.out.outputCommands += patExtraAodEventContent
process.out.outputCommands += patEventContentNoCleaning
## TQAF content
from TopQuarkAnalysis.TopEventProducers.tqafEventContent_cff import *
process.out.outputCommands += tqafEventContent
process.out.outputCommands += ['keep *_ttSemiLepJetPartonMatch_*_*'
                              ,'keep *_ttSemiLepKinFit_*_*'
                              ,'keep *_ttSemiLepKinFitGenMatch_*_*']
