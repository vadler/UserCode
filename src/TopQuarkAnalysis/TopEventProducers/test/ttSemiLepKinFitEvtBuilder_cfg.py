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
    input = cms.untracked.int32(1000)
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

## std sequence to produce the ttSemiLepEvent
process.load("TopQuarkAnalysis.TopTools.TtSemiLepJetPartonMatch_cfi")
process.load("TopQuarkAnalysis.TopJetCombination.TtSemiLepHypGenMatch_cfi")
process.load("TopQuarkAnalysis.TopKinFitter.TtSemiLepKinFitProducer_Muons_cfi")
#process.kinFitTtSemiLepEvent.match        = 'ttSemiLepJetPartonMatch'
#process.kinFitTtSemiLepEvent.useOnlyMatch = True
process.kinFitTtSemiLepEvent.maxNComb     = -1
process.load("TopQuarkAnalysis.TopJetCombination.TtSemiLepHypKinFit_cfi")
process.ttSemiLepHypKinFit.match       = cms.InputTag("kinFitTtSemiLepEvent")
process.ttSemiLepHypKinFit.status      = cms.InputTag("kinFitTtSemiLepEvent","Status")
process.ttSemiLepHypKinFit.leptons     = cms.InputTag("kinFitTtSemiLepEvent","Leptons")
process.ttSemiLepHypKinFit.neutrinos   = cms.InputTag("kinFitTtSemiLepEvent","Neutrinos")
process.ttSemiLepHypKinFit.partonsHadP = cms.InputTag("kinFitTtSemiLepEvent","PartonsHadP")
process.ttSemiLepHypKinFit.partonsHadQ = cms.InputTag("kinFitTtSemiLepEvent","PartonsHadQ")
process.ttSemiLepHypKinFit.partonsHadB = cms.InputTag("kinFitTtSemiLepEvent","PartonsHadB")
process.ttSemiLepHypKinFit.partonsLepB = cms.InputTag("kinFitTtSemiLepEvent","PartonsLepB")
process.load("TopQuarkAnalysis.TopEventProducers.producers.TtSemiLepEvtBuilder_cfi")
process.ttSemiLepEvent.hypotheses = [ 'ttSemiLepHypGenMatch'
                                    , 'ttSemiLepHypKinFit'
                                    ]
process.ttSemiLepEvent.kinFit.chi2 = cms.InputTag("kinFitTtSemiLepEvent","Chi2")
process.ttSemiLepEvent.kinFit.prob = cms.InputTag("kinFitTtSemiLepEvent","Prob")
process.ttSemiLepEvent.verbosity  = 13

## change maximum number of jets taken into account per event (default: 4)
#setForAllTtSemiLepHypotheses(process, "maxNJets", 5)
## solve kinematic equation to determine neutrino pz
#setForAllTtSemiLepHypotheses(process, "neutrinoSolutionType", 2)

## process path
process.p = cms.Path(process.patDefaultSequence *
                     process.makeGenEvt         *
                     process.ttSemiLepJetPartonMatch *
                     process.ttSemiLepHypGenMatch *
                     process.kinFitTtSemiLepEvent *
                     process.ttSemiLepHypKinFit *
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
process.out.outputCommands += ['keep *_kinFitTtSemiLepEvent_*_*']
