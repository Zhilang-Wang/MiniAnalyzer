from CRABClient.UserUtilities import config
config = config()

config.General.requestName = 'MiniAnalyzer_QCD_Run3_Large'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'runMiniAnalyzer_qcd_sample.py'   
config.JobType.allowUndistributedCMSSW = True

config.Data.inputDataset = '/QCD_Bin-PT-600to800_TuneCP5_13p6TeV_pythia8/RunIII2024Summer24MiniAODv6-150X_mcRun3_2024_realistic_v2-v2/MINIAODSIM'

config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 5

config.Data.publication = False
config.Data.outputDatasetTag = 'MiniAnalyzer_QCD_Run3'

config.Site.storageSite = 'T3_CH_CERNBOX'
config.Site.whitelist = ['T2_IT_Bari', 'T2_CH_CERN', 'T1_US_FNAL']