# The following comments couldn't be translated into the new config version:

# use patLayer0_withoutPFTau if you can't get PFTaus working 
import FWCore.ParameterSet.Config as cms

process = cms.Process("PAT")

### General ###

# initialize MessageLogger
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.cerr.INFO = cms.PSet(
    default = cms.untracked.PSet(
        limit = cms.untracked.int32(0)
    ),
    PATLayer0Summary = cms.untracked.PSet(
        limit = cms.untracked.int32(-1)
    )
)
process.MessageLogger.categories.append('PATLayer0Summary')

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

### Input ###

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)
# process.source = cms.Source("PoolSource",
#     fileNames = cms.untracked.vstring(
#         'file:/afs/cern.ch/cms/PRS/top/cmssw-data/relval200-for-pat-testing/RelValTTbar-210p5-AODSIM.100.root'
#     )
# )
# dataset /RelValTTbar/CMSSW_2_1_0_pre5-RelVal-1212543891-STARTUP-2nd-STARTUP/GEN-SIM-DIGI-RAW-HLTDEBUG-RECO
# process.source = cms.Source("PoolSource",
#     fileNames = cms.untracked.vstring(
#         '/store/relval/2008/6/4/RelVal-RelValTTbar-1212543891-STARTUP-2nd/0000/185E7133-9C32-DD11-BC8E-001617E30CC8.root'
#     )
# )
# dataset /RelValZEE/CMSSW_2_1_0_pre5-RelVal-1212543891-STARTUP-2nd-STARTUP/GEN-SIM-DIGI-RAW-HLTDEBUG-RECO
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        '/store/relval/2008/6/4/RelVal-RelValZEE-1212543891-STARTUP-2nd/0000/EC78F835-9C32-DD11-BBFE-001617E30F50.root', 
        '/store/relval/2008/6/4/RelVal-RelValZEE-1212543891-STARTUP-2nd/0000/F2BA62E8-9632-DD11-BC30-001617C3B76A.root'
    )
)
# dataset /RelValTTbar/CMSSW_2_1_0_pre5-RelVal-1212531852-IDEAL_V1-2nd-IDEAL_V1/GEN-SIM-DIGI-RAW-HLTDEBUG-RECO
# process.source = cms.Source("PoolSource",
#     fileNames = cms.untracked.vstring(
#         '/store/relval/2008/6/4/RelVal-RelValTTbar-1212531852-IDEAL_V1-2nd/0000/32E6ABAA-A232-DD11-BD3B-000423D6B48C.root',
#         '/store/relval/2008/6/4/RelVal-RelValTTbar-1212531852-IDEAL_V1-2nd/0000/38DF7E6D-9F32-DD11-80C3-000423D6BA18.root',
#         '/store/relval/2008/6/4/RelVal-RelValTTbar-1212531852-IDEAL_V1-2nd/0000/F2883AD8-9732-DD11-A42C-000423D6CA42.root'
#     )
# )

### PAT steering ###

process.load("PhysicsTools.PatAlgos.patLayer0_cff")
# process.load("PhysicsTools.PatAlgos.test.patLayer0_ReplaceDefaults_full_cff")
process.load("PhysicsTools.PatAlgos.patLayer1_cff")
# process.load("PhysicsTools.PatAlgos.test.patLayer1_ReplaceDefaults_full_cff")

process.triggerSummaryAnalyzerAOD = cms.EDFilter("TriggerSummaryAnalyzerAOD",
    inputTag = cms.InputTag("hltTriggerSummaryAOD","","HLT")
)

process.p = cms.Path(
    process.triggerSummaryAnalyzerAOD * 
    process.patLayer0 * 
    process.patLayer1
)


### Output ###

# define the event content
process.patEventContent = cms.PSet(
    outputCommands = cms.untracked.vstring('drop *')
)
process.load("PhysicsTools.PatAlgos.patLayer1_EventContent_cff")
process.patEventContent.outputCommands.extend(process.patLayer1EventContent.outputCommands)

# only accept events passing the complete path
process.patEventSelection = cms.PSet(
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('p')
    )
)

# the actual output module
process.out = cms.OutputModule("PoolOutputModule",
    process.patEventSelection,
    process.patEventContent,
    verbose = cms.untracked.bool(False),
    fileName = cms.untracked.string('output/PATLayer1_Output.fromAOD_full.root')
)

process.outpath = cms.EndPath(process.out)

