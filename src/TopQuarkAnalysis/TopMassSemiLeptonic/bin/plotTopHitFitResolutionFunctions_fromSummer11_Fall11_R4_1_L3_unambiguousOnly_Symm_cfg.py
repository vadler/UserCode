import os
import FWCore.ParameterSet.Config as cms
process = cms.PSet(
    verbose = cms.bool(False),
    objectCategories = cms.vstring('Mu',
        'UdscJet',
        'Elec',
        'BJet',
        'MET'),
    overwrite = cms.bool(True),
    usePileUp = cms.bool(False),
    useAlt = cms.bool(False),
    useSymm = cms.bool(True),
    refGen = cms.bool(False),
    refSel = cms.bool(False),
    io = cms.PSet(
        sample = cms.string('Fall11_R4_1_L3_unambiguousOnly'),
        resolutionFile = cms.string('file:/afs/cern.ch/user/v/vadler/cms/Top/CMSSW_4_4_4/output/existingHitFitResolutionFunctions_Summer11.root'),
        outputFile = cms.string('file:/afs/cern.ch/user/v/vadler/cms/Top/CMSSW_4_4_4/output/plotTopHitFitResolutionFunctions_fromSummer11_Fall11_R4_1_L3_unambiguousOnly.root'),
        inputFiles = cms.vstring('file:/afs/cern.ch/user/v/vadler/cms/Top/CMSSW_4_4_4/output/fitTopHitFit_fromSummer11_Fall11_R4_1_L3_unambiguousOnly.root'),
        pathPlots = cms.string('/afs/cern.ch/user/v/vadler/cms/Top/CMSSW_4_4_4/output/plots/fitTopHitFitResolutionFunctions/plotTopHitFitResolutionFunctions_fromSummer11_Fall11_R4_1_L3_unambiguousOnly_')
    ),
    plot = cms.PSet(
        accuEvery = cms.uint32(5),
        onlyExisting = cms.bool(True)
    )
)
