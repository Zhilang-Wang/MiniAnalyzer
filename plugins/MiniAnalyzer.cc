#include <memory>
#include <vector>
#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

using pat::CompositeCandidate;

class MiniAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
    explicit MiniAnalyzer(const edm::ParameterSet&);
    ~MiniAnalyzer() override = default;

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
    void beginJob() override {}
    void analyze(const edm::Event&, const edm::EventSetup&) override;
    void endJob() override {}

    edm::EDGetTokenT<std::vector<CompositeCandidate>> myCandToken_;
    edm::EDGetTokenT<std::vector<reco::Vertex>> vtxToken_;
    edm::EDGetTokenT<edm::View<pat::Jet>> jetAK4Src_;
    edm::EDGetTokenT<edm::View<pat::Jet>> jetAK8Src_;
    edm::EDGetTokenT<std::vector<PileupSummaryInfo>> pileupSrc_;
    edm::EDGetTokenT<pat::PackedCandidateCollection> pfCandsSrc_;

    TTree* tree_;

    float mass_, pt_, eta_, phi_, charge_, energy_;

    float vNChi2_, vProb_;
    float ppdlPV_, ppdlErrPV_, ppdlBS_, ppdlErrBS_, cosAlpha_, DCA_;

    float mu1_pt_, mu1_eta_, mu1_phi_, mu1_energy_;
    float mu2_pt_, mu2_eta_, mu2_phi_, mu2_energy_;

    std::vector<float> ch_pt, ch_eta, ch_phi, ch_mass, ch_energy;

    std::vector<float> ak4_jet_pt;
    std::vector<float> ak4_jet_eta;
    std::vector<float> ak4_jet_phi;
    std::vector<float> ak4_jet_energy;
    std::vector<float> ak4_jet_dr_jpsi;

    std::vector<float> ak4_dau_pt;
    std::vector<float> ak4_dau_eta;
    std::vector<float> ak4_dau_phi;
    std::vector<float> ak4_dau_energy;
    std::vector<int>    ak4_dau_jetIndex;

    std::vector<float> ak8_jet_pt;
    std::vector<float> ak8_jet_eta;
    std::vector<float> ak8_jet_phi;
    std::vector<float> ak8_jet_energy;
    std::vector<float> ak8_jet_dr_jpsi;

    std::vector<float> ak8_dau_pt;
    std::vector<float> ak8_dau_eta;
    std::vector<float> ak8_dau_phi;
    std::vector<float> ak8_dau_energy;
    std::vector<int>    ak8_dau_jetIndex;
};

MiniAnalyzer::MiniAnalyzer(const edm::ParameterSet& iConfig)
{
    myCandToken_ = consumes<std::vector<CompositeCandidate>>(
        iConfig.getParameter<edm::InputTag>("myCandLabel"));

    vtxToken_ = consumes<std::vector<reco::Vertex>>(
        iConfig.getParameter<edm::InputTag>("primaryVertexTag"));

    pfCandsSrc_ = consumes<pat::PackedCandidateCollection>(
        iConfig.getUntrackedParameter<edm::InputTag>("pfCandsSrc"));

    jetAK4Src_ = consumes<edm::View<pat::Jet>>(
        iConfig.getUntrackedParameter<edm::InputTag>("ak4JetSrc"));

    jetAK8Src_ = consumes<edm::View<pat::Jet>>(
        iConfig.getUntrackedParameter<edm::InputTag>("ak8JetSrc"));

    pileupSrc_ = consumes<std::vector<PileupSummaryInfo>>(
        iConfig.getUntrackedParameter<edm::InputTag>("pileupSrc"));

    edm::Service<TFileService> fs;
    tree_ = fs->make<TTree>("OniaTree","Jpsi -> mumu + all jets");

    tree_->Branch("mass",      &mass_,      "mass/F");
    tree_->Branch("pt",        &pt_,        "pt/F");
    tree_->Branch("eta",       &eta_,       "eta/F");
    tree_->Branch("phi",       &phi_,       "phi/F");
    tree_->Branch("energy",    &energy_,    "energy/F");
    tree_->Branch("charge",    &charge_,    "charge/F");

    tree_->Branch("vNChi2",    &vNChi2_,    "vNChi2/F");
    tree_->Branch("vProb",     &vProb_,     "vProb/F");
    tree_->Branch("ppdlPV",    &ppdlPV_,    "ppdlPV/F");
    tree_->Branch("ppdlErrPV", &ppdlErrPV_, "ppdlErrPV/F");
    tree_->Branch("ppdlBS",    &ppdlBS_,    "ppdlBS/F");
    tree_->Branch("ppdlErrBS", &ppdlErrBS_, "ppdlErrBS/F");
    tree_->Branch("cosAlpha",  &cosAlpha_,  "cosAlpha/F");
    tree_->Branch("DCA",       &DCA_,       "DCA/F");

    tree_->Branch("mu1_pt",    &mu1_pt_,    "mu1_pt/F");
    tree_->Branch("mu1_eta",   &mu1_eta_,   "mu1_eta/F");
    tree_->Branch("mu1_phi",   &mu1_phi_,   "mu1_phi/F");
    tree_->Branch("mu1_energy",&mu1_energy_,"mu1_energy/F");

    tree_->Branch("mu2_pt",    &mu2_pt_,    "mu2_pt/F");
    tree_->Branch("mu2_eta",   &mu2_eta_,   "mu2_eta/F");
    tree_->Branch("mu2_phi",   &mu2_phi_,   "mu2_phi/F");
    tree_->Branch("mu2_energy",&mu2_energy_,"mu2_energy/F");

    tree_->Branch("ch_pt",     &ch_pt);
    tree_->Branch("ch_eta",    &ch_eta);
    tree_->Branch("ch_phi",    &ch_phi);
    tree_->Branch("ch_mass",   &ch_mass);
    tree_->Branch("ch_energy", &ch_energy);

    tree_->Branch("ak4_jet_pt",      &ak4_jet_pt);
    tree_->Branch("ak4_jet_eta",     &ak4_jet_eta);
    tree_->Branch("ak4_jet_phi",     &ak4_jet_phi);
    tree_->Branch("ak4_jet_energy",  &ak4_jet_energy);
    tree_->Branch("ak4_jet_dr_jpsi", &ak4_jet_dr_jpsi);

    tree_->Branch("ak4_dau_pt",      &ak4_dau_pt);
    tree_->Branch("ak4_dau_eta",     &ak4_dau_eta);
    tree_->Branch("ak4_dau_phi",     &ak4_dau_phi);
    tree_->Branch("ak4_dau_energy",  &ak4_dau_energy);
    tree_->Branch("ak4_dau_jetIndex",&ak4_dau_jetIndex);

    tree_->Branch("ak8_jet_pt",      &ak8_jet_pt);
    tree_->Branch("ak8_jet_eta",     &ak8_jet_eta);
    tree_->Branch("ak8_jet_phi",     &ak8_jet_phi);
    tree_->Branch("ak8_jet_energy",  &ak8_jet_energy);
    tree_->Branch("ak8_jet_dr_jpsi", &ak8_jet_dr_jpsi);

    tree_->Branch("ak8_dau_pt",      &ak8_dau_pt);
    tree_->Branch("ak8_dau_eta",     &ak8_dau_eta);
    tree_->Branch("ak8_dau_phi",     &ak8_dau_phi);
    tree_->Branch("ak8_dau_energy",  &ak8_dau_energy);
    tree_->Branch("ak8_dau_jetIndex",&ak8_dau_jetIndex);
}

void MiniAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup&)
{
    ch_pt.clear();
    ch_eta.clear();
    ch_phi.clear();
    ch_mass.clear();
    ch_energy.clear();

    ak4_jet_pt.clear();
    ak4_jet_eta.clear();
    ak4_jet_phi.clear();
    ak4_jet_energy.clear();
    ak4_jet_dr_jpsi.clear();
    ak4_dau_pt.clear();
    ak4_dau_eta.clear();
    ak4_dau_phi.clear();
    ak4_dau_energy.clear();
    ak4_dau_jetIndex.clear();

    ak8_jet_pt.clear();
    ak8_jet_eta.clear();
    ak8_jet_phi.clear();
    ak8_jet_energy.clear();
    ak8_jet_dr_jpsi.clear();
    ak8_dau_pt.clear();
    ak8_dau_eta.clear();
    ak8_dau_phi.clear();
    ak8_dau_energy.clear();
    ak8_dau_jetIndex.clear();

    edm::Handle<std::vector<CompositeCandidate>> cands;
    iEvent.getByToken(myCandToken_, cands);
    if (!cands.isValid() || cands->empty()) return;

    edm::Handle<std::vector<reco::Vertex>> vtxs;
    iEvent.getByToken(vtxToken_, vtxs);
    if (!vtxs.isValid() || vtxs->empty()) return;
    const reco::Vertex& pv = vtxs->at(0);

    const CompositeCandidate* bestCand = nullptr;
    float minDm = 1e9;
    for (const auto& cand : *cands) {
        float m = cand.mass();
        if (m < 2.9 || m > 3.3) continue;
        const auto* mu1 = (const pat::Muon*)cand.daughter(0);
        const auto* mu2 = (const pat::Muon*)cand.daughter(1);
        if (!mu1 || !mu2) continue;
        if (mu1->pt() < 3 || mu2->pt() < 3) continue;
        if (cand.userFloat("vProb") < 0.01) continue;
        float dm = fabs(m - 3.0969);
        if (dm < minDm) {
            minDm = dm;
            bestCand = &cand;
        }
    }
    if (!bestCand) return;

    const pat::Muon* mu1 = (const pat::Muon*)bestCand->daughter(0);
    const pat::Muon* mu2 = (const pat::Muon*)bestCand->daughter(1);

    mass_   = bestCand->mass();
    pt_     = bestCand->pt();
    eta_    = bestCand->eta();
    phi_    = bestCand->phi();
    energy_ = bestCand->energy();
    charge_ = bestCand->charge();

    vNChi2_    = bestCand->userFloat("vNChi2");
    vProb_     = bestCand->userFloat("vProb");
    ppdlPV_    = bestCand->userFloat("ppdlPV");
    ppdlErrPV_ = bestCand->userFloat("ppdlErrPV");
    ppdlBS_    = bestCand->userFloat("ppdlBS");
    ppdlErrBS_ = bestCand->userFloat("ppdlErrBS");
    cosAlpha_  = bestCand->userFloat("cosAlpha");
    DCA_       = bestCand->userFloat("DCA");

    mu1_pt_     = mu1->pt();
    mu1_eta_    = mu1->eta();
    mu1_phi_    = mu1->phi();
    mu1_energy_ = mu1->energy();

    mu2_pt_     = mu2->pt();
    mu2_eta_    = mu2->eta();
    mu2_phi_    = mu2->phi();
    mu2_energy_ = mu2->energy();

    TLorentzVector jpsi;
    jpsi.SetPtEtaPhiE(pt_, eta_, phi_, energy_);

    edm::Handle<pat::PackedCandidateCollection> pfCands;
    iEvent.getByToken(pfCandsSrc_, pfCands);
    if (pfCands.isValid()) {
        for (const auto& pf : *pfCands) {
            if (pf.pt() < 1 || pf.charge() == 0 || pf.fromPV() < 0)
                continue;
            int id = abs(pf.pdgId());
            if (id == 11 || id == 13)
                continue;
            ch_pt.push_back(pf.pt());
            ch_eta.push_back(pf.eta());
            ch_phi.push_back(pf.phi());
            ch_mass.push_back(pf.mass());
            ch_energy.push_back(pf.energy());
        }
    }

    edm::Handle<edm::View<pat::Jet>> ak4Jets;
    iEvent.getByToken(jetAK4Src_, ak4Jets);
    int idx4 = 0;
    for (const auto& j : *ak4Jets) {
        if (j.pt() < 30 || fabs(j.eta()) > 5)
            continue;
        TLorentzVector v;
        v.SetPtEtaPhiE(j.pt(), j.eta(), j.phi(), j.energy());
        ak4_jet_pt.push_back(j.pt());
        ak4_jet_eta.push_back(j.eta());
        ak4_jet_phi.push_back(j.phi());
        ak4_jet_energy.push_back(j.energy());
        ak4_jet_dr_jpsi.push_back(v.DeltaR(jpsi));

        for (const auto& dtr : j.daughterPtrVector()) {
            const auto* pf = (const pat::PackedCandidate*)dtr.get();
            if (!pf) continue;
            ak4_dau_pt.push_back(pf->pt());
            ak4_dau_eta.push_back(pf->eta());
            ak4_dau_phi.push_back(pf->phi());
            ak4_dau_energy.push_back(pf->energy());
            ak4_dau_jetIndex.push_back(idx4);
        }
        idx4++;
    }

    edm::Handle<edm::View<pat::Jet>> ak8Jets;
    iEvent.getByToken(jetAK8Src_, ak8Jets);
    int idx8 = 0;
    for (const auto& j : *ak8Jets) {
        if (j.pt() < 30 || fabs(j.eta()) > 5)
            continue;
        TLorentzVector v;
        v.SetPtEtaPhiE(j.pt(), j.eta(), j.phi(), j.energy());
        ak8_jet_pt.push_back(j.pt());
        ak8_jet_eta.push_back(j.eta());
        ak8_jet_phi.push_back(j.phi());
        ak8_jet_energy.push_back(j.energy());
        ak8_jet_dr_jpsi.push_back(v.DeltaR(jpsi));

        for (const auto& dtr : j.daughterPtrVector()) {
            const auto* pf = (const pat::PackedCandidate*)dtr.get();
            if (!pf) continue;
            ak8_dau_pt.push_back(pf->pt());
            ak8_dau_eta.push_back(pf->eta());
            ak8_dau_phi.push_back(pf->phi());
            ak8_dau_energy.push_back(pf->energy());
            ak8_dau_jetIndex.push_back(idx8);
        }
        idx8++;
    }

    tree_->Fill();
}

void MiniAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("myCandLabel");
    desc.add<edm::InputTag>("primaryVertexTag");
    desc.addUntracked<edm::InputTag>("pfCandsSrc", edm::InputTag("packedPFCandidates"));
    desc.addUntracked<edm::InputTag>("ak4JetSrc", edm::InputTag("slimmedJetsJEC"));
    desc.addUntracked<edm::InputTag>("ak8JetSrc", edm::InputTag("slimmedAK8JetsJEC"));
    desc.addUntracked<edm::InputTag>("pileupSrc", edm::InputTag("slimmedAddPileupInfo"));
    descriptions.add("default", desc);
}

DEFINE_FWK_MODULE(MiniAnalyzer);