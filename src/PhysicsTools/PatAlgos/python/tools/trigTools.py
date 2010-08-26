from FWCore.GuiBrowsers.ConfigToolBase import *

from PhysicsTools.PatAlgos.tools.helpers import *
from PhysicsTools.PatAlgos.patEventContent_cff import patTriggerL1RefsEventContent


_defaultTriggerMatchers = [ 'cleanMuonTriggerMatchHLTMu9'
                          , 'cleanMuonTriggerMatchHLTDoubleIsoMu3'
                          , 'cleanElectronTriggerMatchHLTEle20SWL1R'
                          , 'cleanTauTriggerMatchHLTDoubleLooseIsoTau15'
                          , 'cleanJetTriggerMatchHLTJet15U'
                          , 'metTriggerMatchHLTMET45'
                          ]


def _labelsInSequence( process, sequenceLabel ):
    result = [ m.label() for m in listModules( getattr( process, sequenceLabel ) ) ]
    result.extend( [ m.label() for m in listSequences( getattr( process, sequenceLabel ) ) ] )
    return result


def _addEventContent( outputCommands, eventContent ):
    for content in eventContent:
        if content not in outputCommands:
            outputCommands += [ content ]
    listToRemove = []
    for i in range( len( outputCommands ) ):
        if i in listToRemove:
            continue
        command = outputCommands[ i ]
        if command[ : 4 ] == 'keep':
            dropCommand = 'drop%s'%( command[ 4: ] )
            for j in range( i + 1, len( outputCommands ) ):
                testCommand = outputCommands[ j ]
                if testCommand == command:
                    listToRemove += [ j ]
                elif testCommand == dropCommand:
                    listToRemove += [ i, j ]
                    break
    newOutputCommands = cms.untracked.vstring()
    for i in range( len( outputCommands ) ):
        if i not in listToRemove:
            newOutputCommands += [ outputCommands[ i ] ]
    return newOutputCommands


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
        # Maintain configurations
        trigProdMod             = getattr( process, triggerProducer )
        trigProdMod.processName = hltProcess
        if triggerProducer in _labelsInSequence( process, 'patDefaultSequence' ):
            print 'switchOnTrigger(): PATTriggerProducer module %s exists already in patDefaultSequence'%( triggerProducer )
            print '                   ==> entry re-used'
            if trigProdMod.onlyStandAlone.value() is True:
                trigProdMod.onlyStandAlone = False
                print '                   configuration parameter automatically changed'
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
        if triggerEventProducer in _labelsInSequence( process, 'patDefaultSequence' ):
            print 'switchOnTrigger(): PATTriggerEventProducer module %s exists already in patDefaultSequence'%( triggerEventProducer )
            print '                   ==> entry re-used'
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
            getattr( process, outputModule ).outputCommands = _addEventContent( getattr( process, outputModule ).outputCommands, patTriggerEventContent )

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
        # Maintain configuration
        trigProdMod             = getattr( process, triggerProducer )
        trigProdMod.processName = hltProcess
        if triggerProducer in _labelsInSequence( process, 'patDefaultSequence' ):
            print 'switchOnTriggerStandAlone(): PATTriggerProducer module %s exists already in patDefaultSequence'%( triggerProducer )
            print '                             ==> entry re-used'
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
            getattr( process, outputModule ).outputCommands = _addEventContent( getattr( process, outputModule ).outputCommands, patTriggerEventContent )

switchOnTriggerStandAlone = SwitchOnTriggerStandAlone()


class SwitchOnTriggerMatching( ConfigToolBase ):
    """  Enables trigger matching in PAT
    SwitchOnTriggerMatching( [cms.Process], hltProcess = 'HLT', triggerMatchers = [default list], triggerProducer = 'patTrigger', triggerEventProducer = 'patTriggerEvent', outputModule = 'out' )
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
        self.addParameter( self._defaultParameters, 'hltProcess'          , 'HLT'                  , "HLT process name, default: 'HLT'" )
        self.addParameter( self._defaultParameters, 'triggerMatchers'     , _defaultTriggerMatchers, "trigger matcher modules' labels, default: [ '' ]" )
        self.addParameter( self._defaultParameters, 'triggerProducer'     , 'patTrigger'           , "PATTriggerProducer module label, default: 'patTrigger'" )
        self.addParameter( self._defaultParameters, 'triggerEventProducer', 'patTriggerEvent'      , "PATTriggerEventProducer module label, default: 'patTriggerEvent'" )
        self.addParameter( self._defaultParameters, 'outputModule'        , 'out'                  , "Output module label, empty label indicates no output, default: 'out'" )
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
        if ( triggerProducer not in _labelsInSequence( process, 'patDefaultSequence' ) or triggerEventProducer not in _labelsInSequence( process, 'patDefaultSequence' ) ):
            print 'switchOnTriggerMatching(): PAT trigger production switched on automatically using'
            print '                           switchOnTrigger( process, %s, %s, %s, %s )'%( hltProcess, triggerProducer, triggerEventProducer, outputModule )
            print '---------------------------------------------------------------------'
            switchOnTrigger( process, hltProcess, triggerProducer, triggerEventProducer, outputModule )
        # Maintain configurations
        trigEvtProdMod = getattr( process, triggerEventProducer )
        if trigEvtProdMod.patTriggerProducer.value() is not triggerProducer:
            print 'switchOnTriggerMatching(): Configuration conflict found'
            print '                           triggerProducer = %s'%( triggerProducer )
            print '                           differs from'
            print '                           %s.patTriggerProducer = %s'%( triggerEventProducer, trigEvtProdMod.patTriggerProducer )
            print '                           parameter automatically changed'
            print '                           ==> triggerProducer --> %s'%( trigEvtProdMod.patTriggerProducer )
            triggerProducer = trigEvtProdMod.patTriggerProducer
        for matcher in triggerMatchers:
            trigMchMod         = getattr( process, matcher )
            trigMchMod.matched = triggerProducer
            if matcher in _labelsInSequence( process, 'patDefaultSequence' ):
                print 'switchOnTriggerMatching(): PAT trigger matcher %s exists already in patDefaultSequence'%( matcher )
                print '                           ==> entry re-used'
                print '---------------------------------------------------------------------'
            else:
                # Sequence arithmetics for PAT trigger matcher modules
                process.patTriggerSequence *= trigMchMod
        if len( getattr( trigEvtProdMod, 'patTriggerMatches' ) ) > 0:
            print 'switchOnTriggerMatching(): PAT trigger matchers already attached to existing PATTriggerEventProducer %s'%( triggerEventProducer )
            print '                           configuration parameters automatically changed'
            for matcher in getattr( trigEvtProdMod, 'patTriggerMatches' ):
                trigMchMod = getattr( process, matcher )
                if trigMchMod.matched.value() is not triggerProducer:
                    process.patTriggerSequence.remove( trigMchMod )
                    trigMchMod.matched          = triggerProducer
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
            getattr( process, outputModule ).outputCommands = _addEventContent( getattr( process, outputModule ).outputCommands, patTriggerEventContent )

switchOnTriggerMatching = SwitchOnTriggerMatching()


class SwitchOnTriggerMatchingStandAlone( ConfigToolBase ):
    """  Enables trigger matching in PAT
    SwitchOnTriggerMatchingStandAlone( [cms.Process], hltProcess = 'HLT', triggerMatchers = [default list], triggerProducer = 'patTrigger', outputModule = 'out' )
    - [cms.Process]  : name of the 'cms.Process'
    - hltProcess     : HLT process name;
                       optional, default: 'HLT'
    - triggerMatchers: PAT trigger matcher module labels (list)
                       optional; default: defined in 'triggerMatchingDefaultSequence'
                       (s. PhysicsTools/PatAlgos/python/triggerLayer1/triggerMatcher_cfi.py)
    - triggerProducer: PATTriggerProducer module label;
                       optional, default: 'patTrigger'
    - outputModule   : output module label;
                       empty label indicates no output;
                       optional, default: 'out'
    Using None as any argument restores its default value.
    """
    _label             = 'switchOnTriggerMatchingStandAlone'
    _defaultParameters = dicttypes.SortedKeysDict()

    def __init__( self ):
        ConfigToolBase.__init__( self )
        self.addParameter( self._defaultParameters, 'hltProcess'     , 'HLT'                  , "HLT process name, default: 'HLT'" )
        self.addParameter( self._defaultParameters, 'triggerMatchers', _defaultTriggerMatchers, "trigger matcher modules' labels, default: [ '' ]" )
        self.addParameter( self._defaultParameters, 'triggerProducer', 'patTrigger'           , "PATTriggerProducer module label, default: 'patTrigger'" )
        self.addParameter( self._defaultParameters, 'outputModule'   , 'out'                  , "Output module label, empty label indicates no output, default: 'out'" )
        self._parameters = copy.deepcopy( self._defaultParameters )
        self._comment = ""

    def getDefaultParameters( self ):
        return self._defaultParameters

    def __call__( self, process,
                  hltProcess      = None,
                  triggerMatchers = None,
                  triggerProducer = None,
                  outputModule    = None ):
        if hltProcess is None:
            hltProcess = self._defaultParameters[ 'hltProcess' ].value
        if triggerMatchers is None:
            triggerMatchers = self._defaultParameters[ 'triggerMatchers' ].value
        if triggerProducer is None:
            triggerProducer = self._defaultParameters[ 'triggerProducer' ].value
        if outputModule is None:
            outputModule = self._defaultParameters[ 'outputModule' ].value
        self.setParameter( 'hltProcess'     , hltProcess )
        self.setParameter( 'triggerMatchers', triggerMatchers )
        self.setParameter( 'triggerProducer', triggerProducer )
        self.setParameter( 'outputModule'   , outputModule )
        self.apply( process )

    def toolCode( self, process ):
        hltProcess      = self._parameters[ 'hltProcess' ].value
        triggerMatchers = self._parameters[ 'triggerMatchers' ].value
        triggerProducer = self._parameters[ 'triggerProducer' ].value
        outputModule    = self._parameters[ 'outputModule' ].value
        # Load default producers from existing config file, if needed
        if triggerMatchers is self.getDefaultParameters()[ 'triggerMatchers' ].value:
            if not hasattr( process, 'triggerMatchingDefaultSequence' ):
                process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerMatcher_cfi" )
        # Switch on PAT trigger information if needed
        if triggerProducer not in _labelsInSequence( process, 'patDefaultSequence' ):
            print 'switchOnTriggerMatchingStandAlone(): PAT trigger production switched on automatically using'
            print '                                     switchOnTriggerStandAlone( process, %s, %s, %s )'%( hltProcess, triggerProducer, outputModule )
            print '---------------------------------------------------------------------'
            switchOnTriggerStandAlone( process, hltProcess, triggerProducer, outputModule )
        # Maintain configurations
        for matcher in triggerMatchers:
            trigMchMod         = getattr( process, matcher )
            trigMchMod.matched = triggerProducer
            if matcher in _labelsInSequence( process, 'patDefaultSequence' ):
                print 'switchOnTriggerMatchingStandAlone(): PAT trigger matcher %s exists already in patDefaultSequence'%( matcher )
                print '                                     ==> entry re-used'
                print '---------------------------------------------------------------------'
            else:
                # Sequence arithmetics for PAT trigger matcher modules
                process.patTriggerSequence *= trigMchMod
        # Add event content
        if outputModule is not '':
            patTriggerEventContent = []
            for matcher in triggerMatchers:
                patTriggerEventContent += [ 'keep patTriggerObjectStandAlonesedmAssociation_%s_*_%s'%( matcher, process.name_() )
                                          , 'keep *_%s_*_*'%( getattr( process, matcher ).src.value() )
                                          ]
            getattr( process, outputModule ).outputCommands = _addEventContent( getattr( process, outputModule ).outputCommands, patTriggerEventContent )

switchOnTriggerMatchingStandAlone = SwitchOnTriggerMatchingStandAlone()


class SwitchOnTriggerMatchEmbedding( ConfigToolBase ):
    """  Enables embedding of trigger matches into PAT objects
    SwitchOnTriggerMatchEmbedding( [cms.Process], hltProcess = 'HLT', triggerMatchers = [default list], triggerProducer = 'patTrigger', outputModule = 'out' )
    - [cms.Process]  : name of the 'cms.Process'
    - hltProcess     : HLT process name;
                       optional, default: 'HLT'
    - triggerMatchers: PAT trigger matcher module labels (list)
                       optional; default: defined in 'triggerMatchingDefaultSequence'
                       (s. PhysicsTools/PatAlgos/python/triggerLayer1/triggerMatcher_cfi.py)
    - triggerProducer: PATTriggerProducer module label;
                       optional, default: 'patTrigger'
    - outputModule   : output module label;
                       empty label indicates no output;
                       optional, default: 'out'
    Using None as any argument restores its default value.
    """
    _label             = 'switchOnTriggerMatchEmbedding'
    _defaultParameters = dicttypes.SortedKeysDict()

    def __init__( self ):
        ConfigToolBase.__init__( self )
        self.addParameter( self._defaultParameters, 'hltProcess'     , 'HLT'                  , "HLT process name, default: 'HLT'" )
        self.addParameter( self._defaultParameters, 'triggerMatchers', _defaultTriggerMatchers, "trigger matcher modules' labels, default: [ '' ]" )
        self.addParameter( self._defaultParameters, 'triggerProducer', 'patTrigger'           , "PATTriggerProducer module label, default: 'patTrigger'" )
        self.addParameter( self._defaultParameters, 'outputModule'   , 'out'                  , "Output module label, empty label indicates no output, default: 'out'" )
        self._parameters = copy.deepcopy( self._defaultParameters )
        self._comment = ""

    def getDefaultParameters( self ):
        return self._defaultParameters

    def __call__( self, process,
                  hltProcess      = None,
                  triggerMatchers = None,
                  triggerProducer = None,
                  outputModule    = None ):
        if hltProcess is None:
            hltProcess = self._defaultParameters[ 'hltProcess' ].value
        if triggerMatchers is None:
            triggerMatchers = self._defaultParameters[ 'triggerMatchers' ].value
        if triggerProducer is None:
            triggerProducer = self._defaultParameters[ 'triggerProducer' ].value
        if outputModule is None:
            outputModule = self._defaultParameters[ 'outputModule' ].value
        self.setParameter( 'hltProcess'     , hltProcess )
        self.setParameter( 'triggerMatchers', triggerMatchers )
        self.setParameter( 'triggerProducer', triggerProducer )
        self.setParameter( 'outputModule'   , outputModule )
        self.apply( process )

    def toolCode( self, process ):
        hltProcess      = self._parameters[ 'hltProcess' ].value
        triggerMatchers = self._parameters[ 'triggerMatchers' ].value
        triggerProducer = self._parameters[ 'triggerProducer' ].value
        outputModule    = self._parameters[ 'outputModule' ].value
        dictEmbedders = { 'selectedPatPhotons'  : 'PATTriggerMatchPhotonEmbedder'
                        , 'cleanPatPhotons'     : 'PATTriggerMatchPhotonEmbedder'
                        , 'selectedPatElectrons': 'PATTriggerMatchElectronEmbedder'
                        , 'cleanPatElectrons'   : 'PATTriggerMatchElectronEmbedder'
                        , 'selectedPatMuons'    : 'PATTriggerMatchMuonEmbedder'
                        , 'cleanPatMuons'       : 'PATTriggerMatchMuonEmbedder'
                        , 'selectedPatTaus'     : 'PATTriggerMatchTauEmbedder'
                        , 'cleanPatTaus'        : 'PATTriggerMatchTauEmbedder'
                        , 'selectedPatJets'     : 'PATTriggerMatchJetEmbedder'
                        , 'cleanPatJets'        : 'PATTriggerMatchJetEmbedder'
                        , 'patMETs'             : 'PATTriggerMatchMETEmbedder'
                        }
        # Load default producers from existing config file, if needed
        if not hasattr( process, 'patTriggerMatchEmbedderDefaultSequence' ):
            process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerMatchEmbedder_cfi" )
        # Switch on PAT trigger matching if needed
        for matcher in triggerMatchers:
            if matcher not in _labelsInSequence( process, 'patDefaultSequence' ):
                print 'switchOnTriggerMatchEmbedding(): PAT trigger matching switched on automatically using'
                print '                                 switchOnTriggerMatchingStandAlone( process, %s, %s, %s, %s )'%( hltProcess, triggerMatchers, triggerProducer, outputModule )
                print '---------------------------------------------------------------------'
                switchOnTriggerMatchingStandAlone( process, hltProcess, triggerMatchers, triggerProducer, '' )
                break
        # Maintain configurations
        dictConfig = {}
        for matcher in triggerMatchers:
            trigMchMod = getattr( process, matcher )
            if trigMchMod.src.value() in dictConfig:
                dictConfig[ trigMchMod.src.value() ] += [ matcher ]
            else:
                dictConfig[ trigMchMod.src.value() ] = [ matcher ]
        patTriggerEventContent = []
        for srcInput in dictConfig.keys():
            if dictEmbedders.has_key( srcInput ):
                label      = srcInput + 'TriggerMatch'
                trigEmbMod = getattr( process, label )
                if label in _labelsInSequence( process, 'patDefaultSequence' ):
                    print 'switchOnTriggerMatchEmbedding(): PAT trigger matcher embedder %s exists already in patDefaultSequence'%( label )
                    print '                                 ==> entry moved'
                    print '---------------------------------------------------------------------'
                    process.patTriggerSequence.remove( trigEmbMod )
                trigEmbMod.matches         += cms.VInputTag( dictConfig[ srcInput ] )
                process.patTriggerSequence *= trigEmbMod
                # Add event content
                patTriggerEventContent += [ 'drop *_%s_*_*'%( srcInput )
                                          , 'keep *_%s_*_%s'%( label, process.name_() )
                                          ]
            else:
                print 'switchOnTriggerMatchEmbedding(): Invalid input source for trigger match embedding'
                print '                                 ==> %s with matchers %s is skipped'%( srcInput, dictConfig[ srcInput ] )
                print '---------------------------------------------------------------------'
        if outputModule is not '':
            getattr( process, outputModule ).outputCommands = _addEventContent( getattr( process, outputModule ).outputCommands, patTriggerEventContent )

switchOnTriggerMatchEmbedding = SwitchOnTriggerMatchEmbedding()
