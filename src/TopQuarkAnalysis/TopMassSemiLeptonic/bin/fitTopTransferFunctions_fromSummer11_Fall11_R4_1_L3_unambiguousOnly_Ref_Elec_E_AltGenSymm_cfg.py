import os
import FWCore.ParameterSet.Config as cms
process = cms.PSet(
    verbose = cms.uint32(1),
    objectCategories = cms.vstring('Elec'),
    usePileUp = cms.bool(False),
    useAlt = cms.bool(True),
    useNonT = cms.bool(True),
    useSymm = cms.bool(True),
    refGen = cms.bool(True),
    refSel = cms.bool(True),
    pileUp = cms.string('PileUpWeightTrue'),
    io = cms.PSet(
        sample = cms.string('Fall11_R4_1_L3_unambiguousOnly'),
        pathPlots = cms.string('/afs/cern.ch/user/v/vadler/cms/Top/CMSSW_4_4_4/output/plots/fitTopTransferFunctions/fitTopTransferFunctions_fromSummer11_Fall11_R4_1_L3_unambiguousOnly_Ref_'),
        outputFile = cms.string('file:/afs/cern.ch/user/v/vadler/cms/Top/CMSSW_4_4_4/output/fitTopTransferFunctions_fromSummer11_Fall11_R4_1_L3_unambiguousOnly.root'),
        overwrite = cms.bool(True),
        inputFile = cms.string('file:/afs/cern.ch/user/v/vadler/cms/Top/CMSSW_4_4_4/output/analyzeHitFit_fromSummer11_Fall11_R4_1_L3_unambiguousOnly.root')
    ),
    histos = cms.PSet(
        DeltaPtMax = cms.double(50.0),
        widthFactor = cms.double(5.0),
        DeltaPtBins = cms.uint32(50)
    ),
    fit = cms.PSet(
        maxDR = cms.double(0.1),
        fitNonRestr = cms.bool(False),
        minPt = cms.double(40.0),
        fitEtaBins = cms.bool(False)
    ),
    transfer = cms.PSet(
        scale = cms.bool(True),
        fitRange = cms.double(1.0),
        dependencyFunction = cms.string('squared'),
        fitOptions = cms.string('IBRS+'),
        fitFunction = cms.string('sGauss'),
        fitMaxPt = cms.double(999999.0),
        pathOut = cms.string('/afs/cern.ch/user/v/vadler/cms/Top/CMSSW_4_4_4/src/TopQuarkAnalysis/TopMassSemiLeptonic/data/transfer_fromSummer11'),
        doFit = cms.bool(True),
        norm = cms.int32(0)
    )
)
