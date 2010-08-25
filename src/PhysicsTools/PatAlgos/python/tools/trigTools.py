from FWCore.GuiBrowsers.ConfigToolBase import *

from PhysicsTools.PatAlgos.tools.helpers import *
from PhysicsTools.PatAlgos.patEventContent_cff import patTriggerL1RefsEventContent


def labelsInSequence( process, sequenceLabel ):
    result = [ m.label() for m in listModules( getattr( process, sequenceLabel ) ) ]
    result.extend( [ m.label() for m in listSequences( getattr( process, sequenceLabel ) ) ] )
    return result


class SwitchOnTrigger( ConfigToolBase ):
    """  Enables trigger information in PAT
    SwitchOnTrigger( [cms.Process], hltProcess = 'HLT', triggerProducer = 'patTrigger', triggerEventProducer = 'patTriggerEvent', outputModule = 'out' )
    - [cms.Process]       : name of the 'cms.Process'
    - hltProcess          : HLT process name;
                            optional, default: 'HLT'
    - triggerProducer     : PATTriggerProducer module label;
                            optional, default: 'patTrigger'
    - triggerEventProducer: PATTriggerEventProducer module label;
                            optional, default: 'patTriggerEvent'
    - outputModule        : output module label;
                            empty label indicates no output;
                            optional, default: 'out'
    Using None as any argument restores its default value.
    """
    _label             = 'switchOnTrigger'
    _defaultParameters = dicttypes.SortedKeysDict()

    def __init__( self ):
        ConfigToolBase.__init__( self )
        self.addParameter( self._defaultParameters, 'hltProcess'          , 'HLT'            , "HLT process name, default: 'HLT'" )
        self.addParameter( self._defaultParameters, 'triggerProducer'     , 'patTrigger'     , "PATTriggerProducer module label, default: 'patTrigger'" )
        self.addParameter( self._defaultParameters, 'triggerEventProducer', 'patTriggerEvent', "PATTriggerEventProducer module label, default: 'patTriggerEvent'" )
        self.addParameter( self._defaultParameters, 'outputModule'        , 'out'            , "Output module label, empty label indicates no output, default: 'out'" )
        self._parameters = copy.deepcopy( self._defaultParameters )
        self._comment = ""

    def getDefaultParameters( self ):
        return self._defaultParameters

    def __call__( self, process,
                  hltProcess           = None,
                  triggerProducer      = None,
                  triggerEventProducer = None,
                  outputModule         = None ):
        if hltProcess is None:
            hltProcess = self._defaultParameters[ 'hltProcess' ].value
        if triggerProducer is None:
            triggerProducer = self._defaultParameters[ 'triggerProducer' ].value
        if triggerEventProducer is None:
            triggerEventProducer = self._defaultParameters[ 'triggerEventProducer' ].value
        if outputModule is None:
            outputModule = self._defaultParameters[ 'outputModule' ].value
        self.setParameter( 'hltProcess'          , hltProcess )
        self.setParameter( 'triggerProducer'     , triggerProducer )
        self.setParameter( 'triggerEventProducer', triggerEventProducer )
        self.setParameter( 'outputModule'        , outputModule )
        self.apply( process )

    def toolCode( self, process ):
        hltProcess           = self._parameters[ 'hltProcess' ].value
        triggerProducer      = self._parameters[ 'triggerProducer' ].value
        triggerEventProducer = self._parameters[ 'triggerEventProducer' ].value
        outputModule         = self._parameters[ 'outputModule' ].value
        # Load default producers from existing config files, if needed
        if triggerProducer is self.getDefaultParameters()[ 'triggerProducer' ].value:
            if not hasattr( process, triggerProducer ):
                process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cfi" )
        if triggerEventProducer is self.getDefaultParameters()[ 'triggerEventProducer' ].value:
            if not hasattr( process, triggerEventProducer ):
                process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerEventProducer_cfi" )
        # Maintaine configurations
        trigProdMod             = getattr( process, triggerProducer )
        trigProdMod.processName = hltProcess
        if triggerProducer in labelsInSequence( process, 'patDefaultSequence' ):
            print 'switchOnTrigger(): PATTriggerProducer module %s exists already in patDefaultSequence'%( triggerProducer )
            print '                   ==> re-configured and re-used'
            print '---------------------------------------------------------------------'
            if trigProdMod.onlyStandAlone.value() is True:
                trigProdMod.onlyStandAlone = False
                print 'switchOnTrigger(): configuration parameter automatically changed'
                print '                   PATTriggerProducer %s.onlyStandAlone --> %s'%( triggerProducer, trigProdMod.onlyStandAlone )
                print '---------------------------------------------------------------------'
        else:
            # Sequence arithmetics for PATTriggerProducer module
            if hasattr( process, 'patTriggerSequence' ):
                process.patTriggerSequence *= trigProdMod
            else:
                process.patTriggerSequence  = cms.Sequence( trigProdMod )
                process.patDefaultSequence *= process.patTriggerSequence
        trigEvtProdMod             = getattr( process, triggerEventProducer )
        trigEvtProdMod.processName = hltProcess
        if hasattr( trigEvtProdMod, 'patTriggerProducer' ):
            trigEvtProdMod.patTriggerProducer = triggerProducer
        else:
            trigEvtProdMod.patTriggerProducer = cms.InputTag( triggerProducer )
        if triggerEventProducer in labelsInSequence( process, 'patDefaultSequence' ):
            print 'switchOnTrigger(): PATTriggerEventProducer module %s exists already in patDefaultSequence'%( triggerEventProducer )
            print '                   ==> re-configured and re-used'
            print '---------------------------------------------------------------------'
        else:
            # Sequence arithmetics for PATTriggerEventProducer module
            if hasattr( process, 'patTriggerEventSequence' ):
                process.patTriggerEventSequence *= trigEvtProdMod
            else:
                process.patTriggerEventSequence = cms.Sequence( trigEvtProdMod )
                process.patDefaultSequence     *= process.patTriggerEventSequence
        # Add event content
        if outputModule is not '':
            patTriggerEventContent = [ 'keep patTriggerObjects_%s_*_%s'%( triggerProducer, process.name_() )
                                     , 'keep patTriggerFilters_%s_*_%s'%( triggerProducer, process.name_() )
                                     , 'keep patTriggerPaths_%s_*_%s'%( triggerProducer, process.name_() )
                                     , 'keep patTriggerEvent_%s_*_%s'%( triggerEventProducer, process.name_() )
                                     ]
            if hasattr( trigProdMod, 'addL1Algos' ) and trigProdMod.addL1Algos.value() is True:
                patTriggerEventContent += [ 'keep patTriggerAlgorithms_%s_*_%s'%( triggerProducer, process.name_() )
                                          ]
            if hasattr( trigProdMod, 'saveL1Refs' ) and trigProdMod.saveL1Refs.value() is True:
                patTriggerEventContent += patTriggerL1RefsEventContent
            getattr( process, outputModule ).outputCommands += patTriggerEventContent

switchOnTrigger = SwitchOnTrigger()


class SwitchOnTriggerStandAlone( ConfigToolBase ):
    """  Enables trigger information in PAT, limited to stand-alone trigger objects
    SwitchOnTriggerStandAlone( [cms.Process], hltProcess = 'HLT', triggerProducer = 'patTrigger', outputModule = 'out' )
    - [cms.Process]       : name of the 'cms.Process'
    - hltProcess          : HLT process name;
                            optional, default: 'HLT'
    - triggerProducer     : PATTriggerProducer module label;
                            optional, default: 'patTrigger'
    - outputModule        : output module label;
                            empty label indicates no output;
                            optional, default: 'out'
    Using None as any argument restores its default value.
    """
    _label             = 'switchOnTriggerStandAlone'
    _defaultParameters = dicttypes.SortedKeysDict()

    def __init__( self ):
        ConfigToolBase.__init__( self )
        self.addParameter( self._defaultParameters, 'hltProcess'     , 'HLT'       , "HLT process name, default: 'HLT'" )
        self.addParameter( self._defaultParameters, 'triggerProducer', 'patTrigger', "PATTriggerProducer module label, default: 'patTrigger'" )
        self.addParameter( self._defaultParameters, 'outputModule'   , 'out'       , "Output module label, empty label indicates no output, default: 'out'" )
        self._parameters = copy.deepcopy( self._defaultParameters )
        self._comment = ""

    def getDefaultParameters( self ):
        return self._defaultParameters

    def __call__( self, process,
                  hltProcess      = None,
                  triggerProducer = None,
                  outputModule    = None ):
        if hltProcess is None:
            hltProcess = self._defaultParameters[ 'hltProcess' ].value
        if triggerProducer is None:
            triggerProducer = self._defaultParameters[ 'triggerProducer' ].value
        if outputModule is None:
            outputModule = self._defaultParameters[ 'outputModule' ].value
        self.setParameter( 'hltProcess'     , hltProcess )
        self.setParameter( 'triggerProducer', triggerProducer )
        self.setParameter( 'outputModule'   , outputModule )
        self.apply( process )

    def toolCode( self, process ):
        hltProcess      = self._parameters[ 'hltProcess' ].value
        triggerProducer = self._parameters[ 'triggerProducer' ].value
        outputModule    = self._parameters[ 'outputModule' ].value
        # Load default producer from existing config file, if needed
        if triggerProducer is self.getDefaultParameters()[ 'triggerProducer' ].value:
            if not hasattr( process, triggerProducer ):
                process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cfi" )
        # Maintaine configuration
        trigProdMod             = getattr( process, triggerProducer )
        trigProdMod.processName = hltProcess
        if triggerProducer in labelsInSequence( process, 'patDefaultSequence' ):
            print 'switchOnTriggerStandAlone(): PATTriggerProducer module %s exists already in patDefaultSequence'%( triggerProducer )
            print '                             ==> re-configured and re-used'
            print '---------------------------------------------------------------------'
        else:
            # Sequence arithmetics for PATTriggerProducer module
            if trigProdMod.onlyStandAlone.value() is False:
                trigProdMod.onlyStandAlone = True
                print 'switchOnTriggerStandAlone(): configuration parameter automatically changed'
                print '                             PATTriggerProducer %s.onlyStandAlone --> %s'%( triggerProducer, trigProdMod.onlyStandAlone )
                print '---------------------------------------------------------------------'
            if hasattr( process, 'patTriggerSequence' ):
                process.patTriggerSequence *= trigProdMod
            else:
                process.patTriggerSequence  = cms.Sequence( trigProdMod )
                process.patDefaultSequence *= process.patTriggerSequence
        # Add event content
        if outputModule is not '':
            patTriggerEventContent = [ 'keep patTriggerObjectStandAlones_%s_*_%s'%( triggerProducer, process.name_() )
                                     ]
            if hasattr( trigProdMod, 'saveL1Refs' ) and trigProdMod.saveL1Refs.value() is True:
                patTriggerEventContent += patTriggerL1RefsEventContent
            getattr( process, outputModule ).outputCommands += patTriggerEventContent

switchOnTriggerStandAlone = SwitchOnTriggerStandAlone()


class SwitchOnTriggerMatching( ConfigToolBase ):
    """  Enables trigger matching in PAT
    SwitchOnTriggerMatching( [cms.Process], triggerMatchers, triggerEventProducer = 'patTriggerEvent', outputModule = 'out' )
    - [cms.Process]       : name of the 'cms.Process'
    - hltProcess          : HLT process name;
                            optional, default: 'HLT'
    - triggerMatchers     : PAT trigger matcher module labels (list)
                            optional; default: defined in 'triggerMatchingDefaultSequence'
                            (s. PhysicsTools/PatAlgos/python/triggerLayer1/triggerMatcher_cfi.py)
    - triggerProducer     : PATTriggerProducer module label;
                            optional, default: 'patTrigger'
    - triggerEventProducer: PATTriggerEventProducer module label;
                            optional, default: 'patTriggerEvent'
    - outputModule        : output module label;
                            empty label indicates no output;
                            optional, default: 'out'
    Using None as any argument restores its default value.
    """
    _label             = 'switchOnTriggerMatching'
    _defaultParameters = dicttypes.SortedKeysDict()

    def __init__( self ):
        ConfigToolBase.__init__( self )
        self.addParameter( self._defaultParameters, 'hltProcess'          , 'HLT'                                          , "HLT process name, default: 'HLT'" )
        self.addParameter( self._defaultParameters, 'triggerMatchers'     , [ 'cleanMuonTriggerMatchHLTIsoMu3'            ,
                                                                              'cleanMuonTriggerMatchHLTDoubleIsoMu3'      ,
                                                                              'cleanElectronTriggerMatchHLTEle15LWL1R'    ,
                                                                              'cleanTauTriggerMatchHLTDoubleLooseIsoTau15',
                                                                              'cleanJetTriggerMatchHLTJet15U'             ,
                                                                              'metTriggerMatchHLTMET45'                   ], "trigger matcher modules' labels, default: [ '' ]" )
        self.addParameter( self._defaultParameters, 'triggerProducer'     , 'patTrigger'                                   , "PATTriggerProducer module label, default: 'patTrigger'" )
        self.addParameter( self._defaultParameters, 'triggerEventProducer', 'patTriggerEvent'                              , "PATTriggerEventProducer module label, default: 'patTriggerEvent'" )
        self.addParameter( self._defaultParameters, 'outputModule'        , 'out'                                          , "Output module label, empty label indicates no output, default: 'out'" )
        self._parameters = copy.deepcopy( self._defaultParameters )
        self._comment = ""

    def getDefaultParameters( self ):
        return self._defaultParameters

    def __call__( self, process,
                  hltProcess           = None,
                  triggerMatchers      = None,
                  triggerProducer      = None,
                  triggerEventProducer = None,
                  outputModule         = None ):
        if hltProcess is None:
            hltProcess = self._defaultParameters[ 'hltProcess' ].value
        if triggerMatchers is None:
            triggerMatchers = self._defaultParameters[ 'triggerMatchers' ].value
        if triggerProducer is None:
            triggerProducer = self._defaultParameters[ 'triggerProducer' ].value
        if triggerEventProducer is None:
            triggerEventProducer = self._defaultParameters[ 'triggerEventProducer' ].value
        if outputModule is None:
            outputModule = self._defaultParameters[ 'outputModule' ].value
        self.setParameter( 'hltProcess'          , hltProcess )
        self.setParameter( 'triggerMatchers'     , triggerMatchers )
        self.setParameter( 'triggerProducer'     , triggerProducer )
        self.setParameter( 'triggerEventProducer', triggerEventProducer )
        self.setParameter( 'outputModule'        , outputModule )
        self.apply( process )

    def toolCode( self, process ):
        hltProcess           = self._parameters[ 'hltProcess' ].value
        triggerMatchers      = self._parameters[ 'triggerMatchers' ].value
        triggerProducer      = self._parameters[ 'triggerProducer' ].value
        triggerEventProducer = self._parameters[ 'triggerEventProducer' ].value
        outputModule         = self._parameters[ 'outputModule' ].value
        # Load default producers from existing config file, if needed
        if triggerMatchers is self.getDefaultParameters()[ 'triggerMatchers' ].value:
            if not hasattr( process, 'triggerMatchingDefaultSequence' ):
                process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerMatcher_cfi" )
        # Switch on PAT trigger information if needed
        if ( triggerProducer not in labelsInSequence( process, 'patDefaultSequence' ) or triggerEventProducer not in labelsInSequence( process, 'patDefaultSequence' ) ):
            print 'switchOnTriggerMatching(): PAT trigger production switched on automatically using'
            print '                           switchOnTrigger( process, %s, %s, %s, %s )'%( hltProcess, triggerProducer, triggerEventProducer, outputModule )
            print '---------------------------------------------------------------------'
            switchOnTrigger( process, hltProcess, triggerProducer, triggerEventProducer, outputModule )
        # Maintaine configurations
        for matcher in triggerMatchers:
            trigMchMod         = getattr( process, matcher )
            trigMchMod.matched = triggerProducer
            if matcher in labelsInSequence( process, 'patDefaultSequence' ):
                print 'switchOnTriggerMatching(): PAT trigger matcher %s exists already in patDefaultSequence'%( matcher )
                print '                           ==> re-configured and re-used'
                print '---------------------------------------------------------------------'
            else:
                # Sequence arithmetics for PAT trigger matcher modules
                process.patTriggerSequence *= trigMchMod
        trigEvtProdMod = getattr( process, triggerEventProducer )
        if len( getattr( trigEvtProdMod, 'patTriggerMatches' ) ) > 0:
            print 'switchOnTriggerMatching(): PAT trigger matchers already attached to existing PATTriggerEventProducer %s'%( triggerEventProducer )
            print '                           configuration parameters automatically changed'
            for matcher in getattr( trigEvtProdMod, 'patTriggerMatches' ):
                trigMchMod = getattr( process, matcher )
                if trigMchMod.matched.value() is not triggerProducer:
                    process.patTriggerSequence.remove( trigMchMod )
                    trigMchMod.matched = triggerProducer
                    process.patTriggerSequence *= trigMchMod
                    print '                           PAT trigger matcher %s.matched --> %s'%( matcher, trigMchMod.matched )
            print '---------------------------------------------------------------------'
        else:
            trigEvtProdMod.patTriggerMatches = cms.VInputTag()
        trigEvtProdMod.patTriggerMatches += triggerMatchers
        # Add event content
        if outputModule is not '':
            patTriggerEventContent = []
            for matcher in triggerMatchers:
                patTriggerEventContent += [ 'keep patTriggerObjectsedmAssociation_%s_%s_%s'%( triggerEventProducer, matcher, process.name_() )
                                          , 'keep *_%s_*_*'%( getattr( process, matcher ).src.value() )
                                          ]
            getattr( process, outputModule ).outputCommands += patTriggerEventContent

switchOnTriggerMatching = SwitchOnTriggerMatching()
