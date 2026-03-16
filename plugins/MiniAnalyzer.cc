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

    // === tokens ===
    edm::EDGetTokenT<std::vector<CompositeCandidate>> myCandToken_;
    edm::EDGetTokenT<std::vector<reco::Vertex>> vtxToken_;
    edm::EDGetTokenT<edm::View<pat::Jet>> jetSrc_;
    edm::EDGetTokenT<std::vector<PileupSummaryInfo>> pileupSrc_;
    edm::EDGetTokenT<pat::PackedCandidateCollection> pfCandsSrc_;

    // === TTree ===
    TTree* tree_;

    // === J/psi variables ===
    float mass_, pt_, eta_, phi_, charge_, energy_;
    float vNChi2_, vProb_;
    float ppdlPV_, ppdlErrPV_, ppdlBS_, ppdlErrBS_, cosAlpha_;
    float DCA_;

    // === charged hadrons ===
    std::vector<float> ch_pt;
    std::vector<float> ch_eta;
    std::vector<float> ch_phi;
    std::vector<float> ch_mass;
    std::vector<float> ch_energy;
};


// =====================================
// constructor
// =====================================

MiniAnalyzer::MiniAnalyzer(const edm::ParameterSet& iConfig)
{
    myCandToken_ = consumes<std::vector<CompositeCandidate>>(
        iConfig.getParameter<edm::InputTag>("myCandLabel"));

    vtxToken_ = consumes<std::vector<reco::Vertex>>(
        iConfig.getParameter<edm::InputTag>("primaryVertexTag"));

    pfCandsSrc_ = consumes<pat::PackedCandidateCollection>(
        iConfig.getUntrackedParameter<edm::InputTag>("pfCandsSrc"));

    jetSrc_ = consumes<edm::View<pat::Jet>>(
        iConfig.getUntrackedParameter<edm::InputTag>("jetsSrc"));

    pileupSrc_ = consumes<std::vector<PileupSummaryInfo>>(
        iConfig.getUntrackedParameter<edm::InputTag>("pileupSrc"));

    edm::Service<TFileService> fs;

    tree_ = fs->make<TTree>("OniaTree","Jpsi -> mumu candidates");

    // ===== J/psi branches =====

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

    // ===== charged hadron branches =====

    tree_->Branch("ch_pt",&ch_pt);
    tree_->Branch("ch_eta",&ch_eta);
    tree_->Branch("ch_phi",&ch_phi);
    tree_->Branch("ch_mass",&ch_mass);
    tree_->Branch("ch_energy",&ch_energy);
}


// =====================================
// beginJob
// =====================================

void MiniAnalyzer::beginJob(){}


// =====================================
// analyze
// =====================================

void MiniAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup&)
{

    // ===== get dimuon candidates =====

    edm::Handle<std::vector<CompositeCandidate>> cands;
    iEvent.getByToken(myCandToken_,cands);
    if(!cands.isValid()) return;


    // ===== get vertices =====

    edm::Handle<std::vector<reco::Vertex>> vertices;
    iEvent.getByToken(vtxToken_,vertices);
    if(!vertices.isValid() || vertices->empty()) return;

    const reco::Vertex &pv = vertices->at(0);


    // ===== get PF candidates =====

    edm::Handle<pat::PackedCandidateCollection> pfs;
    iEvent.getByToken(pfCandsSrc_,pfs);


    // ===== find best J/psi =====

    const CompositeCandidate* bestCand = nullptr;

    float targetMass = 3.0969;
    float minMassDiff = 1e6;


    for(const auto &cand : *cands){

        const pat::Muon* mu1 =
            dynamic_cast<const pat::Muon*>(cand.daughter(0));

        const pat::Muon* mu2 =
            dynamic_cast<const pat::Muon*>(cand.daughter(1));

        if(!mu1 || !mu2) continue;

        // muon pT cut

        if(mu1->pt() < 3.0 || mu2->pt() < 3.0) continue;

        // SoftMuon ID

        if(!mu1->isSoftMuon(pv) || !mu2->isSoftMuon(pv)) continue;

        // vertex probability

        float vProb = cand.userFloat("vProb");
        if(vProb <= 0.01) continue;

        // choose J/psi closest to nominal mass

        float massDiff = std::abs(cand.mass() - targetMass);

        if(massDiff < minMassDiff){
            minMassDiff = massDiff;
            bestCand = &cand;
        }
    }


    // ===== clear vectors =====

    ch_pt.clear();
    ch_eta.clear();
    ch_phi.clear();
    ch_mass.clear();
    ch_energy.clear();


    // ===== fill J/psi =====

    if(bestCand){

        mass_ = bestCand->mass();
        pt_   = bestCand->pt();
        eta_  = bestCand->eta();
        phi_  = bestCand->phi();
        charge_ = bestCand->charge();
        energy_ = bestCand->energy();

        vNChi2_ = bestCand->userFloat("vNChi2");
        vProb_  = bestCand->userFloat("vProb");

        ppdlPV_     = bestCand->userFloat("ppdlPV");
        ppdlErrPV_  = bestCand->userFloat("ppdlErrPV");

        ppdlBS_     = bestCand->userFloat("ppdlBS");
        ppdlErrBS_  = bestCand->userFloat("ppdlErrBS");

        cosAlpha_ = bestCand->userFloat("cosAlpha");

        DCA_ = bestCand->userFloat("DCA");


        // ===== save charged hadrons =====

        if(pfs.isValid()){

            for(const auto &pf : *pfs){

                if(pf.pt() <= 1.0) continue;

                if(pf.charge() == 0) continue;

                if(pf.fromPV() <= 0) continue;

                int pdgid = pf.pdgId();

                if(std::abs(pdgid)==11 || std::abs(pdgid)==13) continue;

                ch_pt.push_back(pf.pt());
                ch_eta.push_back(pf.eta());
                ch_phi.push_back(pf.phi());
                ch_mass.push_back(pf.mass());
                ch_energy.push_back(pf.energy());
            }
        }

        tree_->Fill();
    }
}


// =====================================
// endJob
// =====================================

void MiniAnalyzer::endJob(){}


// =====================================
// fillDescriptions
// =====================================

void MiniAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions){

    edm::ParameterSetDescription desc;

    desc.add<edm::InputTag>("myCandLabel");
    desc.add<edm::InputTag>("primaryVertexTag");

    desc.addUntracked<edm::InputTag>(
        "pfCandsSrc",
        edm::InputTag("packedPFCandidates")
    );

    desc.addUntracked<edm::InputTag>(
        "jetsSrc",
        edm::InputTag("slimmedJetsJEC")
    );

    desc.addUntracked<edm::InputTag>(
        "pileupSrc",
        edm::InputTag("slimmedAddPileupInfo")
    );

    descriptions.addDefault(desc);
}


// =====================================

DEFINE_FWK_MODULE(MiniAnalyzer);