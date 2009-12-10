# Start with pre-defined skeleton process
from PhysicsTools.PatAlgos.patTemplate_cfg import *
process.maxEvents.input     = 1000 # Reduce number of events for testing.
process.out.fileName        = 'edmPatTriggerWB.root'
process.options.wantSummary = False # to suppress the long output at the end of the job
# Using a Summer09 CMSSW_3_1_X input file
process.source.fileNames = [ '/store/mc/Summer09/TTbar/AODSIM/MC_31X_V3_156BxLumiPileUp_AODSIM-v1/0000/FC925113-858D-DE11-BC0C-003048D479A6.root' ]
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import *
run33xOn31xMC(
    process,
    jetSrc   = cms.InputTag( "antikt5CaloJets" ),
    jetIdTag = "antikt5"
)
restrictInputToAOD31X( process )
from PhysicsTools.PatAlgos.tools.tauTools import *
switchTo31Xdefaults(process)

# Define the path
process.p = cms.Path(
    process.patDefaultSequence
)

### PAT trigger ###
process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff" )

## Modifications of default ##

#  #

# Remove trigger matching #
process.patTrigger.remove( process.patTriggerMatcher )
process.patTriggerEvent.patTriggerMatches = []

## Switch everything on ##
from PhysicsTools.PatAlgos.tools.trigTools import switchOnTrigger
switchOnTrigger( process )
