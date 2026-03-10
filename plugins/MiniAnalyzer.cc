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

#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"

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

    // === TTree and branches ===
    TTree* tree_;
    float mass_, pt_, eta_, phi_, charge_;
    float vNChi2_, vProb_;
    float ppdlPV_, ppdlErrPV_, ppdlBS_, ppdlErrBS_, cosAlpha_;
    float vertexWeight_, sumPTPV_, DCA_;
    int countTksOfPV_;
    int momPDGId_;
    float ppdlTrue_;
};

// === constructor ===
MiniAnalyzer::MiniAnalyzer(const edm::ParameterSet& iConfig)
{
    myCandToken_ = consumes<std::vector<CompositeCandidate>>(
        iConfig.getParameter<edm::InputTag>("myCandLabel"));
}

// === beginJob: create TTree ===
void MiniAnalyzer::beginJob()
{
    tree_ = new TTree("OniaTree", "Jpsi → μμ candidates");

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

    tree_->Branch("vertexWeight", &vertexWeight_, "vertexWeight/F");
    tree_->Branch("sumPTPV", &sumPTPV_, "sumPTPV/F");
    tree_->Branch("DCA", &DCA_, "DCA/F");
    tree_->Branch("countTksOfPV", &countTksOfPV_, "countTksOfPV/I");

    tree_->Branch("momPDGId", &momPDGId_, "momPDGId/I");
    tree_->Branch("ppdlTrue", &ppdlTrue_, "ppdlTrue/F");
}

// === analyze: fill TTree ===
void MiniAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup&)
{
    edm::Handle<std::vector<CompositeCandidate>> cands;
    iEvent.getByToken(myCandToken_, cands);

    if (!cands.isValid()) return;

    for (const auto& cand : *cands) {
        mass_ = cand.mass();
        pt_ = cand.pt();
        eta_ = cand.eta();
        phi_ = cand.phi();
        charge_ = cand.charge();

        vNChi2_ = cand.userFloat("vNChi2");
        vProb_ = cand.userFloat("vProb");
        ppdlPV_ = cand.userFloat("ppdlPV");
        ppdlErrPV_ = cand.userFloat("ppdlErrPV");
        ppdlBS_ = cand.userFloat("ppdlBS");
        ppdlErrBS_ = cand.userFloat("ppdlErrBS");
        cosAlpha_ = cand.userFloat("cosAlpha");

        vertexWeight_ = cand.userFloat("vertexWeight");
        sumPTPV_ = cand.userFloat("sumPTPV");
        DCA_ = cand.userFloat("DCA");
        countTksOfPV_ = cand.userInt("countTksOfPV");

        momPDGId_ = cand.userInt("momPDGId");
        ppdlTrue_ = cand.userFloat("ppdlTrue");

        tree_->Fill();
    }
}

// === endJob: write TTree to file ===
void MiniAnalyzer::endJob()
{
    TFile f("MiniAnalyzer_output.root", "RECREATE");
    tree_->Write();
    f.Close();
}

// === fillDescriptions ===
void MiniAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("myCandLabel");
    descriptions.addDefault(desc);
}

// === define module ===
DEFINE_FWK_MODULE(MiniAnalyzer);