import FWCore.ParameterSet.Config as cms
from HeavyFlavorAnalysis.Onia2MuMu.onia2MuMuPAT_cfi import onia2MuMuPAT
from Configuration.AlCa.GlobalTag import GlobalTag

process = cms.Process("MiniAnalyzer")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(20000))  

process.load("Configuration.StandardSequences.Services_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")

process.GlobalTag = GlobalTag(process.GlobalTag, "126X_mcRun3_2023_forPU65_v3", "")
process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")

process.Timing = cms.Service("Timing",
                             summaryOnly=cms.untracked.bool(True))


process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        #'root://cms-xrd-global.cern.ch//store/mc/RunIII2024Summer24MiniAODv6/QCD_Bin-PT-600to800_TuneCP5_13p6TeV_pythia8/MINIAODSIM/150X_mcRun3_2024_realistic_v2-v2/120000/001ed309-7cae-4607-9912-42b5f774b870.root'
         'root://cms-xrd-global.cern.ch//store/mc/Run3Winter23MiniAOD/JPsiTo2Mu_Pt-0To100_pythia8-gun/MINIAODSIM/GTv3Digi_GTv3_MiniGTv3_126X_mcRun3_2023_forPU65_v3-v2/2550000/03f2d74a-7822-44fb-a914-dec5eaaa7b3e.root'    
    )
)

process.TFileService = cms.Service("TFileService",
                                   fileName=cms.string("./result/jpsi_InJet_20000ev.root"))

# ------------------------------
# Muon cleaning and selection
# ------------------------------
process.boostedMuons = cms.EDProducer("PATMuonCleanerBySegments",
    src=cms.InputTag("slimmedMuons"),
    preselection=cms.string("track.isNonnull"),
    passthrough=cms.string("isGlobalMuon && numberOfMatches >= 2"),
    fractionOfSharedSegments=cms.double(0.499)
)

process.selectedMuons = cms.EDFilter("PATMuonSelector",
    src=cms.InputTag("boostedMuons"),
    cut=cms.string("pt > 3 && abs(eta) < 2.4")
)

# ------------------------------
# Onia2MuMuPAT (J/psi -> mu mu)
# ------------------------------
process.onia2MuMuPATUpdated = onia2MuMuPAT.clone(
    muons=cms.InputTag("selectedMuons"),
    beamSpotTag=cms.InputTag("offlineBeamSpot"),
    primaryVertexTag=cms.InputTag("offlineSlimmedPrimaryVertices"),
    higherPuritySelection=cms.string("isTrackerMuon"),
    lowerPuritySelection=cms.string("isTrackerMuon"),
    dimuonSelection=cms.string("2.9 < mass < 3.3 && abs(daughter('muon1').innerTrack.dz - daughter('muon2').innerTrack.dz) < 25"),
    addCommonVertex=cms.bool(True),
    addMuonlessPrimaryVertex=cms.bool(False),
    resolvePileUpAmbiguity=cms.bool(True),
    addMCTruth=cms.bool(False)
)

# ------------------------------
# AK4 Jets with JEC
# ------------------------------
process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")

process.jetCorrFactors = process.updatedPatJetCorrFactors.clone(
    src = cms.InputTag("slimmedJets"),
    levels = ['L1FastJet', 'L2Relative', 'L3Absolute'],
    payload = 'AK4PFchs'
)

process.slimmedJetsJEC = process.updatedPatJets.clone(
    jetSource = cms.InputTag("slimmedJets"),
    jetCorrFactorsSource = cms.VInputTag(cms.InputTag("jetCorrFactors"))
)

# Pileup Jet ID for AK4
process.load("RecoJets.JetProducers.PileupJetID_cfi")
process.pileupJetIdUpdated = process.pileupJetId.clone(
    jets=cms.InputTag("slimmedJets"),
    inputIsCorrected=False,
    applyJec=True,
    vertexes=cms.InputTag("offlineSlimmedPrimaryVertices")
)
process.slimmedJetsJEC.userData.userFloats.src += ['pileupJetIdUpdated:fullDiscriminant']
process.slimmedJetsJEC.userData.userInts.src += ['pileupJetIdUpdated:fullId']

# ------------------------------
# AK8 Jets with JEC
# ------------------------------
process.ak8JetCorrFactors = process.updatedPatJetCorrFactors.clone(
    src     = cms.InputTag("slimmedJetsAK8"),
    levels  = ['L1FastJet', 'L2Relative', 'L3Absolute'],
    payload = 'AK8PFchs'
)

process.slimmedAK8JetsJEC = process.updatedPatJets.clone(
    jetSource = cms.InputTag("slimmedJetsAK8"),
    jetCorrFactorsSource = cms.VInputTag(cms.InputTag("ak8JetCorrFactors"))
)

# ------------------------------
# MiniAnalyzer
# ------------------------------
process.MiniAnalyzer = cms.EDAnalyzer("MiniAnalyzer",
    myCandLabel        = cms.InputTag("onia2MuMuPATUpdated"),
    primaryVertexTag   = cms.InputTag("offlineSlimmedPrimaryVertices"),
    pfCandsSrc         = cms.untracked.InputTag("packedPFCandidates"),
    ak4JetSrc          = cms.untracked.InputTag("slimmedJetsJEC"),
    ak8JetSrc          = cms.untracked.InputTag("slimmedAK8JetsJEC"),
    pileupSrc          = cms.untracked.InputTag("slimmedAddPileupInfo")
)

# ------------------------------
# Full Path
# ------------------------------
process.p = cms.Path(
    process.boostedMuons
    * process.selectedMuons
    * process.onia2MuMuPATUpdated
    * process.jetCorrFactors
    * process.pileupJetIdUpdated
    * process.slimmedJetsJEC
    * process.ak8JetCorrFactors
    * process.slimmedAK8JetsJEC
    * process.MiniAnalyzer
)