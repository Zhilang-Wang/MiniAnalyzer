#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TLorentzVector.h"
#include <vector>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {

    if (argc < 3) {
        cout << "Usage: " << argv[0] << " input.root output.root" << endl;
        return 1;
    }

    TFile* inputFile = TFile::Open(argv[1]);
    TTree* tree = (TTree*)inputFile->Get("MiniAnalyzer/OniaTree");

    // Inclusive J/psi
    float jpsi_pt, jpsi_eta, jpsi_phi, jpsi_energy;
    tree->SetBranchAddress("pt",      &jpsi_pt);
    tree->SetBranchAddress("eta",     &jpsi_eta);
    tree->SetBranchAddress("phi",     &jpsi_phi);
    tree->SetBranchAddress("energy",  &jpsi_energy);

    // AK4 jet
    float ak4jet_pt, ak4jet_eta, ak4jet_phi, ak4jet_energy;
    float ak4jet_dr_jpsi;
    tree->SetBranchAddress("ak4jet_pt",      &ak4jet_pt);
    tree->SetBranchAddress("ak4jet_eta",     &ak4jet_eta);
    tree->SetBranchAddress("ak4jet_phi",     &ak4jet_phi);
    tree->SetBranchAddress("ak4jet_energy",  &ak4jet_energy);
    tree->SetBranchAddress("ak4jet_dr_jpsi", &ak4jet_dr_jpsi);

    // Charged hadrons
    vector<float>* ch_pt = nullptr;
    vector<float>* ch_eta = nullptr;
    vector<float>* ch_phi = nullptr;
    vector<float>* ch_energy = nullptr;
    tree->SetBranchAddress("ch_pt",     &ch_pt);
    tree->SetBranchAddress("ch_eta",    &ch_eta);
    tree->SetBranchAddress("ch_phi",    &ch_phi);
    tree->SetBranchAddress("ch_energy", &ch_energy);

    // Jet constituents
    vector<float>* ak4_dau_pt = nullptr;
    vector<float>* ak4_dau_eta = nullptr;
    vector<float>* ak4_dau_phi = nullptr;
    vector<float>* ak4_dau_energy = nullptr;
    tree->SetBranchAddress("ak4_dau_pt",     &ak4_dau_pt);
    tree->SetBranchAddress("ak4_dau_eta",    &ak4_dau_eta);
    tree->SetBranchAddress("ak4_dau_phi",    &ak4_dau_phi);
    tree->SetBranchAddress("ak4_dau_energy", &ak4_dau_energy);

    // Histograms
    TH1D* h_coschi_inclusive_chadrons
        = new TH1D("coschi_inclusive_chadrons", "Inclusive J/#psi + charged hadrons;cos#chi;", 20, -1, 1);

    TH1D* h_coschi_inclusive_jetconst
        = new TH1D("coschi_inclusive_jetconst", "Inclusive J/#psi + jet constituents;cos#chi;", 20, -1, 1);

    TH1D* h_coschi_inclusive_jpsi_jet
        = new TH1D("coschi_inclusive_jpsi_jet", "Inclusive J/#psi + AK4 jet;cos#chi;", 20, -1, 1);

    // ✨ NEW: J/psi inside AK4 jet (ΔR < 0.4)
    TH1D* h_coschi_jet_in_jpsi_jet
        = new TH1D("coschi_jet_in_jpsi_jet", "J/#psi inside AK4 jet + AK4 jet;cos#chi;", 20, -1, 1);


    Long64_t nEntries = tree->GetEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);

        TLorentzVector jpsi;
        jpsi.SetPtEtaPhiE(jpsi_pt, jpsi_eta, jpsi_phi, jpsi_energy);
        double m_jpsi = jpsi.M();
        TVector3 boost_jpsi = -jpsi.BoostVector();

        // 1. Inclusive J/psi + charged hadrons
        for (size_t j = 0; j < ch_pt->size(); ++j) {
            TLorentzVector h;
            h.SetPtEtaPhiE(ch_pt->at(j), ch_eta->at(j), ch_phi->at(j), ch_energy->at(j));
            h.Boost(boost_jpsi);
            double coschi = jpsi.Vect().Dot(h.Vect()) / (jpsi.Vect().Mag() * h.Vect().Mag());
            double w = h.E() / m_jpsi;
            h_coschi_inclusive_chadrons->Fill(coschi, w);
        }

        // 2. Inclusive J/psi + jet constituents
        for (size_t j = 0; j < ak4_dau_pt->size(); ++j) {
            TLorentzVector h;
            h.SetPtEtaPhiE(ak4_dau_pt->at(j), ak4_dau_eta->at(j), ak4_dau_phi->at(j), ak4_dau_energy->at(j));
            h.Boost(boost_jpsi);
            double coschi = jpsi.Vect().Dot(h.Vect()) / (jpsi.Vect().Mag() * h.Vect().Mag());
            double w = h.E() / m_jpsi;
            h_coschi_inclusive_jetconst->Fill(coschi, w);
        }

        // 3. Inclusive J/psi + AK4 jet (ORIGINAL)
        if (ak4jet_pt > 0) {
            TLorentzVector h;
            h.SetPtEtaPhiE(ak4jet_pt, ak4jet_eta, ak4jet_phi, ak4jet_energy);
            h.Boost(boost_jpsi);

            double coschi = jpsi.Vect().Dot(h.Vect()) / (jpsi.Vect().Mag() * h.Vect().Mag());
            double w = h.E() / m_jpsi;
            h_coschi_inclusive_jpsi_jet->Fill(coschi, w);
        }

        // ✨ 4. NEW: J/psi INSIDE jet (ΔR < 0.4)
        if (ak4jet_pt > 0 && ak4jet_dr_jpsi < 0.4) {
            TLorentzVector h;
            h.SetPtEtaPhiE(ak4jet_pt, ak4jet_eta, ak4jet_phi, ak4jet_energy);
            h.Boost(boost_jpsi);

            double coschi = jpsi.Vect().Dot(h.Vect()) / (jpsi.Vect().Mag() * h.Vect().Mag());
            double w = h.E() / m_jpsi;
            h_coschi_jet_in_jpsi_jet->Fill(coschi, w);
        }

    }

    TFile* outFile = new TFile(argv[2], "RECREATE");
    h_coschi_inclusive_chadrons->Write();
    h_coschi_inclusive_jetconst->Write();
    h_coschi_inclusive_jpsi_jet->Write();
    h_coschi_jet_in_jpsi_jet->Write();  
    outFile->Close();

    inputFile->Close();
    cout << "Inclusive QEC done." << endl;

    return 0;
}