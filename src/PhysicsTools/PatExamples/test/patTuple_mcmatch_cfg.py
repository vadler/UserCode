# This is an example PAT configuration showing the usage of PAT on full sim samples

# Starting with a skeleton process which gets imported with the following line
from PhysicsTools.PatAlgos.patTemplate_cfg import *

## load the exercise config
process.load( "PhysicsTools.PatExamples.mcMatch_cfi" ) # The file to modify!

## modify the default configuration according to the settings as required 
## by Exercise 1. You may want to comment these lines in order to run the
## default configuration of the mc matching rather than your OWN MC matching.
## The default configuration is taken from PhysicsTools/PatExamples/python/
## mcMatching_cfi
## CAUTION: when being uncommented, this does *NOT* run out-of-the-box! You 
## have to configure your own muon match modules first. This has to be done
## in the file PhysicsTools/PatExamples/mcMatch_cfi

## replace the std muon match from pat by a user defined muon match from
## mcMatch_cfi.py
process.makePatMuons.remove( process.muonMatch )
process.makePatMuons += process.myMuonMatch
## this is to restore the right order of modules
process.makePatMuons.remove( process.patMuons )
process.makePatMuons += process.patMuons
process.patMuons.genParticleMatch = "myMuonMatch"
process.patMuons.embedGenMatch = True

## replace the std muon match from pat by a user defined muon match from
## mcMatch_cfi.py
process.makePatJets.remove( process.patJetGenJetMatch )
process.makePatJets += process.myJetGenJetMatch
## this is to restore the right order of modules
process.makePatJets.remove( process.patJets )
process.makePatJets += process.patJets
process.patJets.genJetMatch = "myJetGenJetMatch"

# let it run
process.p = cms.Path(
    process.patDefaultSequence
    )

process.maxEvents.input     = 1000 
process.out.fileName        = 'edmPatMcMatch.root'
process.out.outputCommands += [ 'keep recoGenParticles_genParticles_*_*' ] 
process.options.wantSummary = False
