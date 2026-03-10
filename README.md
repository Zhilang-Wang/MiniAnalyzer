## Workflow

1. **Obtain the CMS environment** 
- cmsrel CMSSW_15_0_14
- cd CMSSW_15_0_14/src
- cmsenv
2. **Skeleton for CMSSW Analysis**
- mkdir UserCode
- mkedanlzr MiniAnalyzer
- cd plugins
- vim MiniAnalyzer.cc
- vim BuildFile.xml
2. **Code compilation**
- cd /path/CMSSW_15_0_14/src
- scram b -j8
3. **Have a valid grid proxy**
- cd /path/CMSSW_15_0_14/src/UserCode/MiniAnalyzer/test
- vim runMiniAnalyzer_cfg.py
- export XRD_NETWORKSTACK=IPv4
- voms-proxy-init --voms cms
4. **Run Configuration File**
- cmsRun runMiniAnalyzer_cfg.py