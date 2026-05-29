from CRABClient.UserUtilities import config
config = config()

# ================================
# General
# ================================
config.General.requestName = 'MiniAnalyzer_QCD_PT600to800_2024'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True

# ================================
# JobType
# ================================
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'runMiniAnalyzer_qcd_sample.py'   
config.JobType.allowUndistributedCMSSW = True

# ================================
# Data
# ================================
#  QCD 数据集名称
config.Data.inputDataset = '/QCD_Bin-PT-600to800_TuneCP5_13p6TeV_pythia8/RunIII2024Summer24MiniAODv6-150X_mcRun3_2024_realistic_v2-v2/MINIAODSIM'

config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 2   

config.Site.storageSite = 'T3_CH_CERNBOX'
config.Data.publication = False

config.Data.outputDatasetTag = 'MiniAnalyzer_QCD_PT600to800_2024'

# ================================
# Site
# ================================
# 允许在 Bari、DESY 或 CERN 跑
#config.Site.whitelist = ['T2_IT_Bari', 'T2_DE_DESY', 'T2_CH_CERN']