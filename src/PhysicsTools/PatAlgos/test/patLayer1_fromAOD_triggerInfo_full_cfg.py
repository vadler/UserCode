# This is an example PAT configuration showing the production of PAT Trigger information

# Starting with a skeleton process which gets imported with the following line
from PhysicsTools.PatAlgos.patTemplate_cfg import *

# load the standard PAT config
process.load("PhysicsTools.PatAlgos.patSequences_cff")

# add the trigger information to the configuration
from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTrigger( process )

process.p = cms.Path(
    process.patDefaultSequence
)

# In addition you usually want to change the following parameters:
#
#   process.GlobalTag.globaltag =  ...     ## (according to https://twiki.cern.ch/twiki/bin/view/CMS/SWGuideFrontierConditions)
#   process.source.fileNames = [ ... ]     ## (e.g. 'file:AOD.root')
#   process.maxEvents.input = ...          ## (e.g. -1 to run on all events)
#   process.out.outputCommands = [ ... ]   ## (e.g. taken from PhysicsTools/PatAlgos/python/patEventContent_cff.py)
#   process.out.fileName = ...             ## (e.g. 'myTuple.root')
#   process.options.wantSummary = False    ## (to suppress the long output at the end of the job)
