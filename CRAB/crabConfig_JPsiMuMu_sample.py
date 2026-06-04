from CRABClient.UserUtilities import config
config = config()

config.General.requestName = 'MiniAnalyzer_JPsiMuMu_Signal_2024'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'runMiniAnalyzer_JPsiMuMu_sample.py'   
config.JobType.allowUndistributedCMSSW = True

config.Data.inputDataset = '/JPsiMuMu_Fil-JPsiNo-2MuPtEta_TuneCP5_13p6TeV_pythia8-evtgen/RunIII2024Summer24MiniAODv6-150X_mcRun3_2024_realistic_v2-v4/MINIAODSIM'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 2   
config.Data.publication = False
config.Data.outputDatasetTag = 'MiniAnalyzer_JPsiMuMu_Signal_2024'

config.Site.storageSite = 'T3_CH_CERNBOX'