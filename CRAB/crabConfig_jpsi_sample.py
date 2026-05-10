from CRABClient.UserUtilities import config
config = config()

# ================================
# General
# ================================
config.General.requestName = 'MiniAnalyzer_JPsi_Run3'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True

# ================================
# JobType
# ================================
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'runMiniAnalyzer_cfg.py'   
config.JobType.allowUndistributedCMSSW = True

# ================================
# Data
# ================================
config.Data.inputDataset = '/JPsiTo2Mu_Pt-0To100_pythia8-gun/Run3Winter23MiniAOD-GTv3Digi_GTv3_MiniGTv3_126X_mcRun3_2023_forPU65_v3-v2/MINIAODSIM'

config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 2   

config.Site.storageSite = 'T3_CH_CERNBOX'
config.Data.publication = False
config.Data.outputDatasetTag = 'MiniAnalyzer_JPsi_Run3'

# ================================
# Site
# ================================
config.Site.whitelist = ['T2_IT_Bari']

