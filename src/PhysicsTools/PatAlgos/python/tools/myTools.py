import os
from subprocess import *

from FWCore.GuiBrowsers.ConfigToolBase import *

from Configuration.PyReleaseValidation.autoCond import autoCond

class PickRelValInputFile( ConfigToolBase ):
    """
    """

    _label             = 'pickRelValInputFile'
    _defaultParameters = dicttypes.SortedKeysDict()

    def __init__( self ):
        ConfigToolBase.__init__( self )
        self.addParameter( self._defaultParameters, 'condition'   , 'startup'                                                        , '' )
        self.addParameter( self._defaultParameters, 'globalTag'   , autoCond[ self._defaultParameters[ 'condition' ].value ][ 0: -5 ], 'auto' )
        self.addParameter( self._defaultParameters, 'cmsswVersion', os.getenv( "CMSSW_VERSION" )                                     , 'auto' )
        self.addParameter( self._defaultParameters, 'relVal'      , 'RelValTTbar'                                                    , '' )
        self.addParameter( self._defaultParameters, 'dataTier'    , 'GEN-SIM-RECO'                                                   , '' )
        self.addParameter( self._defaultParameters, 'maxVersions' , '9'                                                              , '' )
        self.addParameter( self._defaultParameters, 'maxFile'     , '1'                                                              , '' )
        self._parameters = copy.deepcopy( self._defaultParameters )
        self._comment = ""

    def getDefaultParameters( self ):
        return self._defaultParameters

    def __call__( self
                , condition    = None
                , globalTag    = None
                , cmsswVersion = None
                , relVal       = None
                , dataTier     = None
                , maxVersions  = None
                , maxFile      = None
                ):
        if condition is None:
            condition = self._defaultParameters[ 'condition' ].value
        if globalTag is None:
            globalTag = self._defaultParameters[ 'globalTag' ].value
        if cmsswVersion is None:
            cmsswVersion = self._defaultParameters[ 'cmsswVersion' ].value
        if relVal is None:
            relVal = self._defaultParameters[ 'relVal' ].value
        if dataTier is None:
            dataTier = self._defaultParameters[ 'dataTier' ].value
        if maxVersions is None:
            maxVersions = self._defaultParameters[ 'maxVersions' ].value
        if maxFile is None:
            maxFile = self._defaultParameters[ 'maxFile' ].value
        self.setParameter( 'condition'   , condition )
        self.setParameter( 'globalTag'   , globalTag )
        self.setParameter( 'cmsswVersion', cmsswVersion )
        self.setParameter( 'relVal'      , relVal )
        self.setParameter( 'dataTier'    , dataTier )
        self.setParameter( 'maxVersions' , maxVersions )
        self.setParameter( 'maxFile'     , maxFile )
        return self.apply()

    def apply( self ):
        condition    = self._parameters[ 'condition'    ].value
        globalTag    = self._parameters[ 'globalTag'    ].value
        cmsswVersion = self._parameters[ 'cmsswVersion' ].value
        relVal       = self._parameters[ 'relVal'       ].value
        dataTier     = self._parameters[ 'dataTier'     ].value
        maxVersions  = self._parameters[ 'maxVersions'  ].value
        maxFile      = self._parameters[ 'maxFile'      ].value

        maxFileInt = int( maxFile )
        command    = 'nsls'
        rfdirPath  = '/store/relval/%s/%s/%s/%s-v'%( cmsswVersion, relVal, dataTier, globalTag )
        argument   = '/castor/cern.ch/cms%s'%( rfdirPath )
        filePath   = ''
        fileCount  = 0

        for version in range( 1, int( maxVersions ) + 1 ):
            directories = Popen( [ command, argument + '%i'%( version ) ], stdout = PIPE, stderr = PIPE ).communicate()[0]
            for directory in directories.splitlines():
                files = Popen( [ command, argument + '%i/%s'%( version, directory ) ], stdout = PIPE, stderr = PIPE ).communicate()[0]
                for file in files.splitlines():
                    if len( file ) > 0:
                        fileCount += 1
                    if fileCount >= maxFileInt:
                        filePath = rfdirPath + '%i/%s/%s'%( version, directory, file )
                        break
                if fileCount >= maxFileInt:
                    break
            if fileCount >= maxFileInt:
              break
        if fileCount < maxFileInt:
            print 'Error: could not find any file in %s*'%( argument )

        return filePath

pickRelValInputFile = PickRelValInputFile()
