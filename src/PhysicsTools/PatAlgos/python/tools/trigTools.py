from FWCore.GuiBrowsers.ConfigToolBase import *

from PhysicsTools.PatAlgos.tools.helpers import *
from PhysicsTools.PatAlgos.patEventContent_cff import patTriggerL1RefsEventContent


def labelsInSequence(process, sequenceLabel, postfix=""):
    result = [ m.label()[:-len(postfix)] for m in listModules( getattr(process,sequenceLabel+postfix))]
    result.extend([ m.label()[:-len(postfix)] for m in listSequences( getattr(process,sequenceLabel+postfix))]  )
    if postfix == "":
        result = [ m.label() for m in listModules( getattr(process,sequenceLabel+postfix))]
        result.extend([ m.label() for m in listSequences( getattr(process,sequenceLabel+postfix))]  )
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
        existTrigProd        = triggerProducer in labelsInSequence( process, 'patDefaultSequence' )
        if triggerProducer is self._defaultParameters[ 'triggerProducer' ].value:
            if not hasattr( process, triggerProducer ):
                process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cfi" )
        if triggerEventProducer is self._defaultParameters[ 'triggerEventProducer' ].value:
            if not hasattr( process, triggerEventProducer ):
                process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerEventProducer_cfi" )
        trigProd    = getattr( process, triggerProducer )
        trigEvtProd = getattr( process, triggerEventProducer )
        trigProd.processName = hltProcess
        if trigProd.onlyStandAlone.value() is True:
            trigProd.onlyStandAlone = False
            print "---------------------------------------------------------------------"
            print 'INFO   : configuration parameter automatically changed by %s!'%( self._label )
            print '         PATTriggerProducer %s.onlyStandAlone --> %s'%( triggerProducer, trigProd.onlyStandAlone )
        trigEvtProd.processName = hltProcess
        if hasattr( trigEvtProd, 'patTriggerProducer' ): # check needed, since this parameter is optional
            trigEvtProd.patTriggerProducer = triggerProducer
        else:
            trigEvtProd.patTriggerProducer = cms.InputTag( triggerProducer )
        if not existTrigProd:
            process.patDefaultSequence += trigProd
        process.patTriggerEventSequence = cms.Sequence( trigEvtProd )
        process.patDefaultSequence += process.patTriggerEventSequence
        if outputModule is not '':
            patTriggerEventContent = [
              'keep patTriggerObjects_%s_*_*'%( triggerProducer )
            , 'keep patTriggerFilters_%s_*_*'%( triggerProducer )
            , 'keep patTriggerPaths_%s_*_*'%( triggerProducer )
            , 'keep patTriggerEvent_%s_*_*'%( triggerEventProducer )
            ]
            if hasattr( trigProd, 'addL1Algos' ) and trigProd.addL1Algos.value() is True:
                patTriggerEventContent += [
                  'keep patTriggerAlgorithms_%s_*_*'%( triggerProducer )
                ]
            if not existTrigProd and hasattr( trigProd, 'saveL1Refs' ) and trigProd.saveL1Refs.value() is True:
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
        existTrigProd   = triggerProducer in labelsInSequence( process, 'patDefaultSequence' )
        if triggerProducer is self._defaultParameters[ 'triggerProducer' ].value:
            if not hasattr( process, triggerProducer ):
                process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cfi" )
        trigProd = getattr( process, triggerProducer )
        trigProd.processName = hltProcess
        if not existTrigProd and trigProd.onlyStandAlone.value() is False:
            trigProd.onlyStandAlone = True
            print "---------------------------------------------------------------------"
            print 'INFO   : configuration parameter automatically changed by %s!'%( self._label )
            print '         PATTriggerProducer %s.onlyStandAlone --> %s'%( triggerProducer, trigProd.onlyStandAlone )
        if not existTrigProd:
            process.patDefaultSequence += trigProd
        if outputModule is not '':
            patTriggerEventContent = [
              'keep patTriggerObjectStandAlones_%s_*_*'%( triggerProducer )
            ]
            if not existTrigProd and hasattr( trigProd, 'saveL1Refs' ) and trigProd.saveL1Refs.value() is True:
                patTriggerEventContent += patTriggerL1RefsEventContent
            getattr( process, outputModule ).outputCommands += patTriggerEventContent

switchOnTriggerStandAlone = SwitchOnTriggerStandAlone()
