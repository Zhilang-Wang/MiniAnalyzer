#include <memory>
#include <vector>
#include "TFile.h"
#include "TTree.h"

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

    // === token for getByToken ===
    edm::EDGetTokenT<std::vector<CompositeCandidate>> myCandToken_;
    edm::EDGetTokenT<std::vector<reco::Vertex>> vtxToken_;

    // === TTree and branches ===
    TTree* tree_;
    float mass_, pt_, eta_, phi_, charge_;
    float vNChi2_, vProb_;
    float ppdlPV_, ppdlErrPV_, ppdlBS_, ppdlErrBS_, cosAlpha_;
   // float vertexWeight_, sumPTPV_, DCA_;
   // float sumPTPV_, DCA_;
    float DCA_;
   // int countTksOfPV_;
};

// === constructor ===
MiniAnalyzer::MiniAnalyzer(const edm::ParameterSet& iConfig)
{
    myCandToken_ = consumes<std::vector<CompositeCandidate>>(
    iConfig.getParameter<edm::InputTag>("myCandLabel"));
    vtxToken_ = consumes<std::vector<reco::Vertex>>(
    iConfig.getParameter<edm::InputTag>("primaryVertexTag"));
    

    edm::Service<TFileService> fs;
    tree_ = fs->make<TTree>("OniaTree", "Jpsi -> mumu candidates");

    tree_->Branch("mass", &mass_, "mass/F");
    tree_->Branch("pt", &pt_, "pt/F");
    tree_->Branch("eta", &eta_, "eta/F");
    tree_->Branch("phi", &phi_, "phi/F");
    tree_->Branch("charge", &charge_, "charge/F");

    tree_->Branch("vNChi2", &vNChi2_, "vNChi2/F");
    tree_->Branch("vProb", &vProb_, "vProb/F");
    tree_->Branch("ppdlPV", &ppdlPV_, "ppdlPV/F");
    tree_->Branch("ppdlErrPV", &ppdlErrPV_, "ppdlErrPV/F");
    tree_->Branch("ppdlBS", &ppdlBS_, "ppdlBS/F");
    tree_->Branch("ppdlErrBS", &ppdlErrBS_, "ppdlErrBS/F");
    tree_->Branch("cosAlpha", &cosAlpha_, "cosAlpha/F");

   // tree_->Branch("vertexWeight", &vertexWeight_, "vertexWeight/F");
  //  tree_->Branch("sumPTPV", &sumPTPV_, "sumPTPV/F");
    tree_->Branch("DCA", &DCA_, "DCA/F");
  //  tree_->Branch("countTksOfPV", &countTksOfPV_, "countTksOfPV/I");    
}

// === beginJob: create TTree ===
void MiniAnalyzer::beginJob(){}


// === analyze: fill TTree ===
void MiniAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup&)
{
    edm::Handle<std::vector<CompositeCandidate>> cands;
    iEvent.getByToken(myCandToken_, cands);
    if (!cands.isValid()) return;

    edm::Handle<std::vector<reco::Vertex>> vertices;
    iEvent.getByToken(vtxToken_, vertices);
    if (!vertices.isValid() || vertices->empty()) return;

    const reco::Vertex &pv = vertices->at(0);

    const CompositeCandidate* bestCand = nullptr;
    float targetMass = 3.0969; // J/psi
    float minMassDiff = 1e6;    // 初始化

    for (const auto& cand : *cands) {
        const pat::Muon* mu1 = dynamic_cast<const pat::Muon*>(cand.daughter(0));
        const pat::Muon* mu2 = dynamic_cast<const pat::Muon*>(cand.daughter(1));
        if (!mu1 || !mu2) continue;

        // SoftMuon 条件
        if (!mu1->isSoftMuon(pv) || !mu2->isSoftMuon(pv)) continue;

        // vProb 条件
        float vProb = cand.userFloat("vProb");
        if (vProb <= 0.01) continue;

        // 选质量最接近 targetMass 的 dimuon
        float massDiff = std::abs(cand.mass() - targetMass);
        if (massDiff < minMassDiff) {
            minMassDiff = massDiff;
            bestCand = &cand;
        }
    }

    // 填充 TTree
    if (bestCand) {
        mass_ = bestCand->mass();
        pt_ = bestCand->pt();
        eta_ = bestCand->eta();
        phi_ = bestCand->phi();
        charge_ = bestCand->charge();

        vNChi2_ = bestCand->userFloat("vNChi2");
        vProb_ = bestCand->userFloat("vProb");
        ppdlPV_ = bestCand->userFloat("ppdlPV");
        ppdlErrPV_ = bestCand->userFloat("ppdlErrPV");
        ppdlBS_ = bestCand->userFloat("ppdlBS");
        ppdlErrBS_ = bestCand->userFloat("ppdlErrBS");
        cosAlpha_ = bestCand->userFloat("cosAlpha");
        DCA_ = bestCand->userFloat("DCA");

        tree_->Fill();
    }
}

// === endJob: write TTree to file ===
void MiniAnalyzer::endJob() {}

// === fillDescriptions ===
// === fillDescriptions ===
void MiniAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("myCandLabel");
    desc.add<edm::InputTag>("primaryVertexTag");
    descriptions.addDefault(desc);
}

// === define module ===
DEFINE_FWK_MODULE(MiniAnalyzer);