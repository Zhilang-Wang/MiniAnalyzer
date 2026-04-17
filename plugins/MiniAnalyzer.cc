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
    ~MiniAnalyzer() override {}

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
    void beginJob() override;
    void analyze(const edm::Event&, const edm::EventSetup&) override;
    void endJob() override;

    edm::EDGetTokenT<std::vector<CompositeCandidate>> myCandToken_;
    edm::EDGetTokenT<std::vector<reco::Vertex>> vtxToken_;
    edm::EDGetTokenT<edm::View<pat::Jet>> jetAK4Src_;
    edm::EDGetTokenT<edm::View<pat::Jet>> jetAK8Src_;
    edm::EDGetTokenT<std::vector<PileupSummaryInfo>> pileupSrc_;
    edm::EDGetTokenT<pat::PackedCandidateCollection> pfCandsSrc_;

    TTree* tree_;

    float mass_, pt_, eta_, phi_, charge_, energy_;
    float vNChi2_, vProb_;
    float ppdlPV_, ppdlErrPV_, ppdlBS_, ppdlErrBS_, cosAlpha_;
    float DCA_;

    std::vector<float> ch_pt;
    std::vector<float> ch_eta;
    std::vector<float> ch_phi;
    std::vector<float> ch_mass;
    std::vector<float> ch_energy;

    float ak4jet_pt, ak4jet_eta, ak4jet_phi, ak4jet_energy;
    float ak4jet_dr_jpsi;
    int   ak4jet_ndau;
    std::vector<float> ak4_dau_pt, ak4_dau_eta, ak4_dau_phi, ak4_dau_energy;

    float ak8jet_pt, ak8jet_eta, ak8jet_phi, ak8jet_energy;
    float ak8jet_dr_jpsi;
    int   ak8jet_ndau;
    std::vector<float> ak8_dau_pt, ak8_dau_eta, ak8_dau_phi, ak8_dau_energy;

    template<typename JetView>
    void matchJetsToJpsi(
        const JetView& jets,
        const TLorentzVector& jpsi,
        float& jet_pt, float& jet_eta, float& jet_phi, float& jet_energy,
        float& dr_jpsi, int& ndau,
        std::vector<float>& dau_pt, std::vector<float>& dau_eta,
        std::vector<float>& dau_phi, std::vector<float>& dau_energy,
        float jetR,
        float jetPtCut,
        float jetEtaCut
    );
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
    tree_ = fs->make<TTree>("OniaTree","Jpsi -> mumu candidates");

    tree_->Branch("mass",&mass_,"mass/F");
    tree_->Branch("pt",&pt_,"pt/F");
    tree_->Branch("eta",&eta_,"eta/F");
    tree_->Branch("phi",&phi_,"phi/F");
    tree_->Branch("energy",&energy_,"energy/F");
    tree_->Branch("charge",&charge_,"charge/F");

    tree_->Branch("vNChi2",&vNChi2_,"vNChi2/F");
    tree_->Branch("vProb",&vProb_,"vProb/F");

    tree_->Branch("ppdlPV",&ppdlPV_,"ppdlPV/F");
    tree_->Branch("ppdlErrPV",&ppdlErrPV_,"ppdlErrPV/F");
    tree_->Branch("ppdlBS",&ppdlBS_,"ppdlBS/F");
    tree_->Branch("ppdlErrBS",&ppdlErrBS_,"ppdlErrBS/F");
    tree_->Branch("cosAlpha",&cosAlpha_,"cosAlpha/F");
    tree_->Branch("DCA",&DCA_,"DCA/F");

    tree_->Branch("ch_pt",&ch_pt);
    tree_->Branch("ch_eta",&ch_eta);
    tree_->Branch("ch_phi",&ch_phi);
    tree_->Branch("ch_mass",&ch_mass);
    tree_->Branch("ch_energy",&ch_energy);

    tree_->Branch("ak4jet_pt",      &ak4jet_pt,      "ak4jet_pt/F");
    tree_->Branch("ak4jet_eta",     &ak4jet_eta,     "ak4jet_eta/F");
    tree_->Branch("ak4jet_phi",     &ak4jet_phi,     "ak4jet_phi/F");
    tree_->Branch("ak4jet_energy",  &ak4jet_energy,  "ak4jet_energy/F");
    tree_->Branch("ak4jet_dr_jpsi", &ak4jet_dr_jpsi, "ak4jet_dr_jpsi/F");
    tree_->Branch("ak4jet_ndau",    &ak4jet_ndau,    "ak4jet_ndau/I");
    tree_->Branch("ak4_dau_pt",     &ak4_dau_pt);
    tree_->Branch("ak4_dau_eta",    &ak4_dau_eta);
    tree_->Branch("ak4_dau_phi",    &ak4_dau_phi);
    tree_->Branch("ak4_dau_energy", &ak4_dau_energy);

    tree_->Branch("ak8jet_pt",      &ak8jet_pt,      "ak8jet_pt/F");
    tree_->Branch("ak8jet_eta",     &ak8jet_eta,     "ak8jet_eta/F");
    tree_->Branch("ak8jet_phi",     &ak8jet_phi,     "ak8jet_phi/F");
    tree_->Branch("ak8jet_energy",  &ak8jet_energy,  "ak8jet_energy/F");
    tree_->Branch("ak8jet_dr_jpsi", &ak8jet_dr_jpsi, "ak8jet_dr_jpsi/F");
    tree_->Branch("ak8jet_ndau",    &ak8jet_ndau,    "ak8jet_ndau/I");
    tree_->Branch("ak8_dau_pt",     &ak8_dau_pt);
    tree_->Branch("ak8_dau_eta",    &ak8_dau_eta);
    tree_->Branch("ak8_dau_phi",    &ak8_dau_phi);
    tree_->Branch("ak8_dau_energy", &ak8_dau_energy);
}

void MiniAnalyzer::beginJob(){}

template<typename JetView>
void MiniAnalyzer::matchJetsToJpsi(
    const JetView& jets,
    const TLorentzVector& jpsi,
    float& jet_pt, float& jet_eta, float& jet_phi, float& jet_energy,
    float& dr_jpsi, int& ndau,
    std::vector<float>& dau_pt, std::vector<float>& dau_eta,
    std::vector<float>& dau_phi, std::vector<float>& dau_energy,
    float jetR,
    float jetPtCut,
    float jetEtaCut)
{
    jet_pt = -99;
    jet_eta = -99;
    jet_phi = -99;
    jet_energy = -99;
    dr_jpsi = 999;
    ndau = 0;

    dau_pt.clear();
    dau_eta.clear();
    dau_phi.clear();
    dau_energy.clear();

    const pat::Jet* bestJet = nullptr;
    float minDR = 999;

    for (const auto& j : jets) {
        if (j.pt() < jetPtCut) continue;
        if (fabs(j.eta()) > jetEtaCut) continue;

        TLorentzVector jjet;
        jjet.SetPtEtaPhiE(j.pt(), j.eta(), j.phi(), j.energy());
        float dr = jjet.DeltaR(jpsi);
        if (dr < minDR) {
            minDR = dr;
            bestJet = &j;
        }
    }

    if (!bestJet) return;
    //if (minDR > jetR) return;

    dr_jpsi = minDR;
    jet_pt = bestJet->pt();
    jet_eta = bestJet->eta();
    jet_phi = bestJet->phi();
    jet_energy = bestJet->energy();

    for (const auto& dau : bestJet->daughterPtrVector()) {
        const pat::PackedCandidate* pf = dynamic_cast<const pat::PackedCandidate*>(dau.get());
        if (!pf) continue;
        if (pf->pt() < 1.0) continue;

        dau_pt.push_back(pf->pt());
        dau_eta.push_back(pf->eta());
        dau_phi.push_back(pf->phi());
        dau_energy.push_back(pf->energy());
    }
    ndau = dau_pt.size();
}

void MiniAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup&)
{
    ch_pt.clear();
    ch_eta.clear();
    ch_phi.clear();
    ch_mass.clear();
    ch_energy.clear();

    ak4jet_pt = -99;
    ak4jet_eta = -99;
    ak4jet_phi = -99;
    ak4jet_energy = -99;
    ak4jet_dr_jpsi = 999;
    ak4jet_ndau = 0;
    ak4_dau_pt.clear();
    ak4_dau_eta.clear();
    ak4_dau_phi.clear();
    ak4_dau_energy.clear();

    ak8jet_pt = -99;
    ak8jet_eta = -99;
    ak8jet_phi = -99;
    ak8jet_energy = -99;
    ak8jet_dr_jpsi = 999;
    ak8jet_ndau = 0;
    ak8_dau_pt.clear();
    ak8_dau_eta.clear();
    ak8_dau_phi.clear();
    ak8_dau_energy.clear();

    edm::Handle<std::vector<CompositeCandidate>> cands;
    iEvent.getByToken(myCandToken_, cands);
    if (!cands.isValid()) return;

    edm::Handle<std::vector<reco::Vertex>> vertices;
    iEvent.getByToken(vtxToken_, vertices);
    if (!vertices.isValid() || vertices->empty()) return;
    const reco::Vertex& pv = vertices->at(0);

    const CompositeCandidate* bestCand = nullptr;
    float minMassDiff = 1e6;
    const float targetMass = 3.0969;

    for (const auto& cand : *cands) {
        float m = cand.mass();
        if (m < 2.9 || m > 3.3) continue;

        const pat::Muon* mu1 = dynamic_cast<const pat::Muon*>(cand.daughter(0));
        const pat::Muon* mu2 = dynamic_cast<const pat::Muon*>(cand.daughter(1));
        if (!mu1 || !mu2) continue;

        if (mu1->pt() < 3.0 || mu2->pt() < 3.0) continue;
        if (!mu1->isSoftMuon(pv) || !mu2->isSoftMuon(pv)) continue;

        float vProb = cand.userFloat("vProb");
        if (vProb <= 0.01) continue;

        float diff = fabs(cand.mass() - targetMass);
        if (diff < minMassDiff) {
            minMassDiff = diff;
            bestCand = &cand;
        }
    }

    if (!bestCand) return;

    mass_ = bestCand->mass();
    pt_ = bestCand->pt();
    eta_ = bestCand->eta();
    phi_ = bestCand->phi();
    energy_ = bestCand->energy();
    charge_ = bestCand->charge();

    vNChi2_ = bestCand->userFloat("vNChi2");
    vProb_ = bestCand->userFloat("vProb");
    ppdlPV_ = bestCand->userFloat("ppdlPV");
    ppdlErrPV_ = bestCand->userFloat("ppdlErrPV");
    ppdlBS_ = bestCand->userFloat("ppdlBS");
    ppdlErrBS_ = bestCand->userFloat("ppdlErrBS");
    cosAlpha_ = bestCand->userFloat("cosAlpha");
    DCA_ = bestCand->userFloat("DCA");

    TLorentzVector jpsi;
    jpsi.SetPtEtaPhiE(pt_, eta_, phi_, energy_);

    edm::Handle<pat::PackedCandidateCollection> pfs;
    iEvent.getByToken(pfCandsSrc_, pfs);
    if (pfs.isValid()) {
        for (const auto& pf : *pfs) {
            if (pf.pt() <= 1.0) continue;
            if (pf.charge() == 0) continue;
            if (pf.fromPV() <= 0) continue;

            int pdg = pf.pdgId();
            if (abs(pdg) == 11 || abs(pdg) == 13) continue;

            ch_pt.push_back(pf.pt());
            ch_eta.push_back(pf.eta());
            ch_phi.push_back(pf.phi());
            ch_mass.push_back(pf.mass());
            ch_energy.push_back(pf.energy());
        }
    }

    edm::Handle<edm::View<pat::Jet>> ak4Jets;
    edm::Handle<edm::View<pat::Jet>> ak8Jets;
    iEvent.getByToken(jetAK4Src_, ak4Jets);
    iEvent.getByToken(jetAK8Src_, ak8Jets);

    const float jptCut = 30.0;
    const float jetaCut = 5.0;

    matchJetsToJpsi(*ak4Jets, jpsi,
        ak4jet_pt, ak4jet_eta, ak4jet_phi, ak4jet_energy,
        ak4jet_dr_jpsi, ak4jet_ndau,
        ak4_dau_pt, ak4_dau_eta, ak4_dau_phi, ak4_dau_energy,
        0.4, jptCut, jetaCut);

    matchJetsToJpsi(*ak8Jets, jpsi,
        ak8jet_pt, ak8jet_eta, ak8jet_phi, ak8jet_energy,
        ak8jet_dr_jpsi, ak8jet_ndau,
        ak8_dau_pt, ak8_dau_eta, ak8_dau_phi, ak8_dau_energy,
        0.8, jptCut, jetaCut);

    //bool inAK4 = (ak4jet_dr_jpsi < 0.4);
    //bool inAK8 = (ak8jet_dr_jpsi < 0.8);
    //if (!inAK4 && !inAK8) return;

    tree_->Fill();
}

void MiniAnalyzer::endJob(){}

void MiniAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions){
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