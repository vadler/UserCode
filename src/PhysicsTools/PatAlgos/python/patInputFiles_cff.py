import FWCore.ParameterSet.Config as cms
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles

filesRelValProdTTbarAODSIM = cms.untracked.vstring(
    pickRelValInputFiles( cmsswVersion  = 'CMSSW_6_2_0_pre2'
                        , relVal        = 'RelValProdTTbar'
                        , globalTag     = 'START61_V11'
                        , dataTier      = 'AODSIM'
                        , maxVersions   = 1
                        , numberOfFiles = 1
                        )
    )
filesRelValProdTTbarGENSIMRECO = cms.untracked.vstring(
    pickRelValInputFiles( cmsswVersion  = 'CMSSW_6_2_0_pre2'
                        , relVal        = 'RelValProdTTbar'
                        , globalTag     = 'START61_V11'
                        , dataTier      = 'GEN-SIM-RECO'
                        , maxVersions   = 1
                        , numberOfFiles = 1
                        )
    )
filesRelValTTbarPileUpGENSIMRECO = cms.untracked.vstring(
    pickRelValInputFiles( cmsswVersion  = 'CMSSW_6_2_0_pre2'
                        , relVal        = 'RelValTTbar'
                        , globalTag     = 'PU_START61_V11'
                        , dataTier      = 'GEN-SIM-RECO'
                        , maxVersions   = 1
                        , numberOfFiles = 1
                        )
    )
filesSingleMuRECO = cms.untracked.vstring(
    pickRelValInputFiles( cmsswVersion  = 'CMSSW_6_2_0_pre2'
                        , relVal        = 'SingleMu'
                        , dataTier      = 'RECO'
                        , globalTag     = 'GR_R_61_V6_RelVal_mu2012D'
                        , maxVersions   = 1
                        , numberOfFiles = 1
                        )
    )
