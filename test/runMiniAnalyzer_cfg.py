import FWCore.ParameterSet.Config as cms
from HeavyFlavorAnalysis.Onia2MuMu.onia2MuMuPAT_cfi import onia2MuMuPAT
from Configuration.AlCa.GlobalTag import GlobalTag

process = cms.Process("MiniAnalyzer")

# ======================================
# Message logger & maxEvents
# ======================================
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(-1))

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
    fileNames=cms.untracked.vstring()
)

# ======================================
# Output TFileService
# ======================================
process.TFileService = cms.Service("TFileService",
                                   fileName=cms.string("DUMMYFILENAME.root"))

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
# Onia2MuMuPAT updated 
# ======================================
process.onia2MuMuPATUpdated = onia2MuMuPAT.clone(
    muons=cms.InputTag("boostedMuons"),
    beamSpotTag=cms.InputTag("offlineBeamSpot"),
    primaryVertexTag=cms.InputTag("offlineSlimmedPrimaryVertices"),
    higherPuritySelection=cms.string("isTrackerMuon"),
    lowerPuritySelection=cms.string("isTrackerMuon"),
    dimuonSelection=cms.string("2 < mass && abs(daughter('muon1').innerTrack.dz - daughter('muon2').innerTrack.dz) < 25"),  # J/psi mass window
    addCommonVertex=cms.bool(True),
    addMuonlessPrimaryVertex=cms.bool(False),
    resolvePileUpAmbiguity=cms.bool(True),     
    addMCTruth = cms.bool(False)  # <--- 关闭 MC truth candidate
)

# ======================================
# Jets
# ======================================
process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")

process.jetCorrFactors = process.updatedPatJetCorrFactors.clone(
    src = cms.InputTag("slimmedJets"),
    levels = ['L1FastJet', 
              'L2Relative', 
              'L3Absolute'],
    payload = 'AK4PFchs'
)

process.slimmedJetsJEC = process.updatedPatJets.clone(
    jetSource = cms.InputTag("slimmedJets"),
    jetCorrFactorsSource = cms.VInputTag(cms.InputTag("jetCorrFactors"))
)

# pileup jet id
process.load("RecoJets.JetProducers.PileupJetID_cfi")

process.pileupJetIdUpdated = process.pileupJetId.clone(
    jets=cms.InputTag("slimmedJets"),
    inputIsCorrected=False,
    applyJec=True,
    vertexes=cms.InputTag("offlineSlimmedPrimaryVertices")
)

process.slimmedJetsJEC.userData.userFloats.src += ['pileupJetIdUpdated:fullDiscriminant']
process.slimmedJetsJEC.userData.userInts.src += ['pileupJetIdUpdated:fullId']


# ======================================
# MiniAnalyzer
# ======================================
process.MiniAnalyzer = cms.EDAnalyzer("MiniAnalyzer",
    myCandLabel=cms.InputTag("onia2MuMuPATUpdated"),
    primaryVertexTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
    pfCandsSrc   = cms.untracked.InputTag("packedPFCandidates"),
    jetsSrc      = cms.untracked.InputTag("slimmedJetsJEC"),
    pileupSrc     = cms.untracked.InputTag("slimmedAddPileupInfo")
)

# ======================================
# Path
# ======================================
process.p = cms.Path(
    process.boostedMuons *
    process.selectedMuons *
    process.onia2MuMuPATUpdated *
    process.jetCorrFactors *
    process.pileupJetIdUpdated *
    process.slimmedJetsJEC *
    process.MiniAnalyzer
)