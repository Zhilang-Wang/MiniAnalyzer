import FWCore.ParameterSet.Config as cms
from HeavyFlavorAnalysis.Onia2MuMu.onia2MuMuPAT_cfi import onia2MuMuPAT
from Configuration.AlCa.GlobalTag import GlobalTag

process = cms.Process("MiniAnalyzer")

# ======================================
# Message logger & maxEvents
# ======================================
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(1000))

# ======================================
# Standard services, geometry, magnetic field
# ======================================
process.load("Configuration.StandardSequences.Services_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")

# ======================================
# GlobalTag
# ======================================
process.GlobalTag = GlobalTag(process.GlobalTag, "126X_mcRun3_2023_forPU65_v3", "")
process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")

# ======================================
# Timing service
# ======================================
process.Timing = cms.Service("Timing",
                             summaryOnly=cms.untracked.bool(True))

# ======================================
# Input files (MiniAODSIM)
# ======================================
process.source = cms.Source("PoolSource",
    fileNames=cms.untracked.vstring(
        "root://cms-xrd-global.cern.ch//store/mc/Run3Winter23MiniAOD/JPsiTo2Mu_Pt-0To100_pythia8-gun/MINIAODSIM/GTv3Digi_GTv3_MiniGTv3_126X_mcRun3_2023_forPU65_v3-v2/2550000/03f2d74a-7822-44fb-a914-dec5eaaa7b3e.root"
    )
)

# ======================================
# Output TFileService
# ======================================
process.TFileService = cms.Service("TFileService",
                                   fileName=cms.string("MiniAnalyzer_output.root"))

# ======================================
# Clean muons
# ======================================
process.boostedMuons = cms.EDProducer("PATMuonCleanerBySegments",
    src=cms.InputTag("slimmedMuons"),
    preselection=cms.string("track.isNonnull"),
    passthrough=cms.string("isGlobalMuon && numberOfMatches >= 2"),
    fractionOfSharedSegments=cms.double(0.499)
)

process.selectedMuons = cms.EDFilter("PATMuonSelector",
    src=cms.InputTag("boostedMuons"),
    cut=cms.string("pt > 5 && abs(eta) < 2.4")
)

# ======================================
# Onia2MuMuPAT updated (MiniAOD-safe)
# ======================================
process.onia2MuMuPATUpdated = onia2MuMuPAT.clone(
    muons=cms.InputTag("boostedMuons"),
    beamSpotTag=cms.InputTag("offlineBeamSpot"),
    primaryVertexTag=cms.InputTag("offlineSlimmedPrimaryVertices"),
    higherPuritySelection=cms.string("isTrackerMuon"),
    lowerPuritySelection=cms.string("isTrackerMuon"),
    dimuonSelection=cms.string(""),  # J/psi mass window
    addCommonVertex=cms.bool(True),
    addMuonlessPrimaryVertex=cms.bool(False),
    resolvePileUpAmbiguity=cms.bool(True)
)

# ======================================
# MiniAnalyzer
# ======================================
process.MiniAnalyzer = cms.EDAnalyzer("MiniAnalyzer",
    myCandLabel=cms.InputTag("onia2MuMuPATUpdated")
)

# ======================================
# Path
# ======================================
process.p = cms.Path(
    process.boostedMuons *
    process.selectedMuons *
    process.onia2MuMuPATUpdated *
    process.MiniAnalyzer
)