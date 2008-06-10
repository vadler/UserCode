import os.path

global CONFIGDIR
CONFIGDIR = os.path.normcase(os.path.abspath(__file__)).rsplit('/', 1)[0]
LAYOUTS   = ("%s/%s-layouts.py" % (CONFIGDIR, x) for x in ( "csc", "dt", "eb", "ecal", "ee", "hcal", "l1temulator", "l1t", "pixel", "rpc", "strip"))

modules = ("GuiDQM",)

envsetup = """
source /exports/slc4/CMSSW/scripts/setup.sh
source /exports/slc4/CMSSW/Development/DQM/rpms/slc4_ia32_gcc345/cms/webtools/1.3.0/etc/profile.d/dependencies-setup.sh
export QUIET_ASSERT=a
"""

server.serverDir   = '/exports/slc4/CMSSW/Development/DQM/gui'
server.baseUrl     = '/dqm/online'
server.title       = 'CMS data quality'
server.serviceName = 'Online'

server.source('dqm'    , 'DQMLive'   ,                     '--listen 9091', '--collector localhost:9090')
server.source('file'   , 'DQMArchive', '/home/dqm/dqm.db', '--listen 9097')
server.source('layouts', 'DQMLayout' , *LAYOUTS)

execfile(CONFIGDIR + "/dqm-services.py")
execfile(CONFIGDIR + "/workspaces-online.py")
