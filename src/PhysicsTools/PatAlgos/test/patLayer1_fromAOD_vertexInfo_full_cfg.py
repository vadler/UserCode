import FWCore.ParameterSet.Config as cms

process = cms.Process("PAT")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.categories.append('PATSummaryTables')
process.MessageLogger.cerr.INFO = cms.untracked.PSet(
    default          = cms.untracked.PSet( limit = cms.untracked.int32(0)  ),
    PATSummaryTables = cms.untracked.PSet( limit = cms.untracked.int32(-1) )
)
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

# source
process.source = cms.Source("PoolSource", 
     fileNames = cms.untracked.vstring('file:/afs/cern.ch/cms/PRS/top/cmssw-data/relval200-for-pat-testing/FullSimTTBar-2_2_X_2008-11-03-STARTUP_V7-AODSIM.100.root')
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )


process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('STARTUP_V7::All')
process.load("Configuration.StandardSequences.MagneticField_cff")

# PAT Layer 0+1
process.load("PhysicsTools.PatAlgos.patSequences_cff")

from PhysicsTools.PatAlgos.tools.trackTools import *

## ========================== PAT WITH GENERIC TRACKS ===========================
makeTrackCandidates(process, 
        label='TrackCands',                   # output collection will be 'allLayer0TrackCands', 'allLayer1TrackCands', 'selectedLayer1TrackCands'
        tracks=cms.InputTag('generalTracks'), # input track collection
        particleType="pi+",                   # particle type (for assigning a mass)
        preselection='pt > 10',               # preselection cut on candidates. Only methods of 'reco::Candidate' are available
        selection='pt > 10',                  # Selection on PAT Layer 1 objects ('selectedLayer1TrackCands')
        isolation={'tracker':0.3,             # Isolations to use ('source':deltaR; set to {} for None)
                   'ecalTowers':0.3,          # 'tracker' => as muon track iso
                   'hcalTowers':0.3},         # 'ecalTowers', 'hcalTowers' => as muon iso from calo towers.
        isodeposits=[],                       # examples: 'tracker','ecalTowers','hcalTowers'; [] = empty list = none   
        mcAs=cms.InputTag("muons")            # Replicate MC match as the one used by PAT on this AOD collection (None = no mc match)
        );                                    #  you can specify more than one collection for this
## ========================== VERTEXING =========================================
## Select best vertex
process.bestVertex = cms.EDFilter("PATSingleVertexSelector",
    mode      = cms.string("nearestToCandidate"),
    fallbacks = cms.vstring("fromCandidate", "beamSpot"),
    vertices              = cms.InputTag("offlinePrimaryVerticesWithBS"),
    vertexPreselection    = cms.vstring("(chi2prob(chi2,ndf) > 0.01) && (trackSize >= 3)"),
    candidates           = cms.VInputTag(cms.InputTag('pixelMatchGsfElectrons'), cms.InputTag('muons')),
    candidatePreselection = cms.string("pt > 5"),
    beamSpot              = cms.InputTag('offlineBeamSpot'),
)
## Produce vertex associations
process.patTrackVertexInfo = cms.EDProducer("PATVertexAssociationProducer",
    candidates = cms.VInputTag( 
        cms.InputTag('pixelMatchGsfElectrons'),
        cms.InputTag('muons'),
        cms.InputTag('patAODTrackCands'),
    ),
    useTracks = cms.bool(True), # use the track to compute vertexing info
    vertices  = cms.InputTag('bestVertex'),
);
## Add to sequence
process.patAODExtraReco += process.bestVertex * process.patTrackVertexInfo
## Add it to GenericParticles (FIXME: add to muons and electrons too)
process.allLayer1TrackCands.vertexing = cms.PSet(
    vertexAssociations = cms.InputTag("patTrackVertexInfo"),
)

process.p = cms.Path(
        process.patDefaultSequence 
)

# Output module configuration
process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('PATLayer1_Output.fromAOD_vertexInfo_full.root'),
    # save only events passing the full path
    SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p') ),
    outputCommands = cms.untracked.vstring('drop *')
)
process.outpath = cms.EndPath(process.out)
# save PAT Layer 1 output
from PhysicsTools.PatAlgos.patEventContent_cff import *
process.out.outputCommands += patEventContent 
process.out.outputCommands.append('keep *_selectedLayer1TrackCands_*_*')

#### Dump the python config
#
# f = open("patLayer1_fromAOD_genericTracks_full.dump.py", "w")
# f.write(process.dumpPython())
# f.close()
#
 
