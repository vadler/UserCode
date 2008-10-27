#!/bin/tcsh
tar -czvf ~cctrack/scratch0/DQM/${CMSSW_VERSION}/output/${1}.tar.gz ../*/${1}
rm -r ../*/${1}
