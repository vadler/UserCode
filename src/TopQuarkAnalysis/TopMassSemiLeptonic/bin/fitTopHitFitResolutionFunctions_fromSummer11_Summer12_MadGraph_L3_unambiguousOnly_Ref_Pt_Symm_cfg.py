cms.PSet(
    verbose = cms.uint32(1),
    objectCategories = cms.vstring('UdscJet', 
        'BJet', 
        'Mu', 
        'Elec'),
    usePileUp = cms.bool(False),
    useAlt = cms.bool(False),
    useSymm = cms.bool(True),
    refGen = cms.bool(False),
    refSel = cms.bool(True),
    pileUp = cms.string('PileUpWeightTrue'),
    io = cms.PSet(
        resolutionFile = cms.string('file:/afs/cern.ch/user/v/vadler/cms/Top/CMSSW_5_3_11/output/existingHitFitResolutionFunctions_Summer11.root'),
        outputFile = cms.string('file:/afs/cern.ch/user/v/vadler/cms/Top/CMSSW_5_3_11/output/fitTopHitFitResolutionFunctions_fromSummer11_Summer12_MadGraph_L3_unambiguousOnly.root'),
        sample = cms.string('Summer12_MadGraph_L3_unambiguousOnly'),
        pathPlots = cms.string('/afs/cern.ch/user/v/vadler/cms/Top/CMSSW_5_3_11/output/plots/fitTopHitFitResolutionFunctions/fitTopHitFitResolutionFunctions_fromSummer11_Summer12_MadGraph_L3_unambiguousOnly_Ref_'),
        overwrite = cms.bool(True),
        inputFile = cms.string('file:/afs/cern.ch/user/v/vadler/cms/Top/CMSSW_5_3_11/output/analyzeHitFit_fromSummer11_Summer12_MadGraph_L3_unambiguousOnly.root')
    ),
    histos = cms.PSet(
        METEtaBins = cms.uint32(50),
        ElecEtaInvBins = cms.uint32(50),
        METPhiBins = cms.uint32(50),
        BJetPhiInvBins = cms.uint32(50),
        MuPtInvBins = cms.uint32(50),
        ElecPtMax = cms.double(12.5),
        UdscJetEtaInvMax = cms.double(0.5),
        METPtInvMax = cms.double(0.025),
        BJetPtBins = cms.uint32(50),
        MuPtBins = cms.uint32(50),
        MuEtaInvBins = cms.uint32(50),
        BJetEtaInvBins = cms.uint32(50),
        MuPhiInvMax = cms.double(0.002),
        METEtaInvMax = cms.double(10.0),
        MuPtInvMax = cms.double(0.0025),
        METPhiInvBins = cms.uint32(50),
        UdscJetEtaInvBins = cms.uint32(50),
        UdscJetPhiInvBins = cms.uint32(50),
        MuEtaMax = cms.double(0.02),
        BJetPhiInvMax = cms.double(0.5),
        MuPhiMax = cms.double(0.002),
        ElecEtaInvMax = cms.double(0.02),
        UdscJetPtInvBins = cms.uint32(50),
        METPtBins = cms.uint32(50),
        UdscJetEtaMax = cms.double(0.5),
        ElecPhiInvMax = cms.double(0.002),
        BJetPtInvBins = cms.uint32(50),
        BJetEtaBins = cms.uint32(50),
        MuPhiBins = cms.uint32(50),
        BJetPtMax = cms.double(50.0),
        ElecPhiBins = cms.uint32(50),
        ElecPtBins = cms.uint32(50),
        METEtaMax = cms.double(10.0),
        widthFactor = cms.double(5.0),
        METPhiMax = cms.double(1.6),
        MuPtMax = cms.double(12.5),
        ElecPhiMax = cms.double(0.002),
        MuEtaBins = cms.uint32(50),
        METPtInvBins = cms.uint32(50),
        UdscJetPhiMax = cms.double(0.5),
        UdscJetPtMax = cms.double(50.0),
        UdscJetPtBins = cms.uint32(50),
        BJetPhiBins = cms.uint32(50),
        ElecEtaMax = cms.double(0.02),
        BJetEtaInvMax = cms.double(0.5),
        UdscJetPtInvMax = cms.double(0.02),
        BJetPtInvMax = cms.double(0.02),
        METPtMax = cms.double(100.0),
        MuEtaInvMax = cms.double(0.02),
        UdscJetPhiInvMax = cms.double(0.5),
        ElecPtInvBins = cms.uint32(50),
        ElecEtaBins = cms.uint32(50),
        MuPhiInvBins = cms.uint32(50),
        ElecPtInvMax = cms.double(0.0025),
        METEtaInvBins = cms.uint32(50),
        METPhiInvMax = cms.double(1.6),
        ElecPhiInvBins = cms.uint32(50),
        UdscJetPhiBins = cms.uint32(50),
        BJetPhiMax = cms.double(0.5),
        UdscJetEtaBins = cms.uint32(50),
        BJetEtaMax = cms.double(0.5)
    ),
    resolution = cms.PSet(
        resolutionFunctionInverse = cms.string('sqrt(([0]*[0]/x+[1]*[1])/x+[2]*[2])'),
        pathOut = cms.string('/afs/cern.ch/user/v/vadler/cms/Top/CMSSW_5_3_11/src/TopQuarkAnalysis/TopHitFit/data/resolution_fromSummer11'),
        resolutionFunctionInverseInv = cms.string('sqrt(([2]*[2]*x+[1]*[1])*x+[0]*[0])*x'),
        resolutionFunctionInverseRel = cms.string('sqrt(([0]*[0]/x+[1]*[1])/x+[2]*[2])/x'),
        fitOptionsSigma = cms.string('MERS'),
        doFit = cms.bool(True),
        resolutionFunction = cms.string('sqrt(([0]*[0]*x+[1]*[1])*x+[2]*[2])'),
        resolutionFunctionRel = cms.string('sqrt(([0]*[0]*x+[1]*[1])*x+[2]*[2])/x'),
        fitOptions = cms.string('RS'),
        fitFunction = cms.string('gaus'),
        fitMaxPt = cms.double(999999.0),
        fitRange = cms.double(2.0),
        resolutionFunctionInverseInvRel = cms.string('sqrt(([2]*[2]*x+[1]*[1])*x+[0]*[0])'),
        onlyExisting = cms.bool(True),
        norm = cms.int32(0),
        scale = cms.bool(True)
    )
)