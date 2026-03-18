## JPsiTo2Mu Analyzer for CMS Run3

## Workflow

1. **Obtain the CMS environment** 
- cmsrel CMSSW_15_0_14
- cd CMSSW_15_0_14/src
- cmsenv
2. **Skeleton for CMSSW Analysis**
- mkdir UserCode
- cd UserCode
- mkedanlzr MiniAnalyzer
- cd plugins
- vim MiniAnalyzer.cc
- vim BuildFile.xml
2. **Code compilation**
- cd /path/CMSSW_15_0_14/src
- scram b -j8

## Local run and test

3. **Have a valid grid proxy**
- cd /path/CMSSW_15_0_14/src/UserCode/MiniAnalyzer/test
- vim runMiniAnalyzer_cfg.py
- export XRD_NETWORKSTACK=IPv4
- voms-proxy-init --voms cms
4. **Run Configuration File**
- cmsRun runMiniAnalyzer_cfg.py

5. **Check the output of the root file**
- cd /path/CMSSW_15_0_14/src/UserCode/MiniAnalyzer/plot
- python3 draw.py

## Submit CRAB

6. **Submit a CRAB job on an AFS volume**
- cd /afs/.../CMSSW_15_0_14/src
- cmsenv
- cd /path/test
- export XRD_NETWORKSTACK=IPv4
- voms-proxy-init --voms cms
- crab submit -c crabConfig.py

7. **Check the status of the task.**
- crab status -d crab_projects/MiniAnalyzer_JPsi_Run3

8. **Check the output of the root files**
- cd /eos/.../JPsiTo2Mu_Pt-0To100_pythia8-gun/MiniAnalyzer_JPsi_Run3/260317_121721/0000
- hadd PsiTo2Mu_Pt-0To100_pythia8.root *.root
- cd /path/CMSSW_15_0_14/src/UserCode/MiniAnalyzer/plot
- python3 draw.py

## Calculate the correlation between charged hadrons and JPsi

9. **Prepare the calculation script and plot the coschi**
- cd /path/CMSSW_15_0_14/src/UserCode/MiniAnalyzer/QEC/calcQEC
- vim calQEC.cpp
```bash
 g++ calQEC.cpp -o calQEC `root-config --cflags --libs`
```
-./calQEC input.root output.root
- cd ../plot/
- python3 draw.py


